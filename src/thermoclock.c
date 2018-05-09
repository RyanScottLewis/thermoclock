#include <linux/bitops.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/uaccess.h>

#include <linux/string.h>

#include "thermoclock.h"

#define DEVICE_NAME        "thermoclock"
#define CLASS_NAME         "thermoclock"
#define THERMOCLOCK_MAJOR (1)
static int thermoclock_major = THERMOCLOCK_MAJOR;

struct thermoclock_state {
  // SPI device
  struct spi_device *spi;

  // Character device
  int          major;
  struct cdev  cdev;
  struct class *class;
  dev_t        devno;

  // Buffers
  short messageSize;
  char  message[256];
  char  rx[3];
  char  tx[3];
};

// -- Forward declarations -------------------------------------------------------------------------

// Character device
static int     thermoclock_open(struct inode *inodep, struct file *filep);
static ssize_t thermoclock_read(struct file *filep, char *buffer, size_t len, loff_t *offset);
static ssize_t thermoclock_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);
static int     thermoclock_release(struct inode *inodep, struct file *filep);

// SPI device
static int thermoclock_probe(struct spi_device* spi);
static int thermoclock_remove(struct spi_device* spi);

// -- Variables ------------------------------------------------------------------------------------

static struct file_operations thermoclock_fops = {
  .open    = thermoclock_open,
  .read    = thermoclock_read,
  .write   = thermoclock_write,
  .release = thermoclock_release,
};

static const struct spi_device_id thermoclock_ids[] = {
  { "thermoclock", 0 },
  {}
};

static struct spi_driver thermoclock_spi_driver = {
  .driver   = {
    .name   = "thermoclock",
  },
  .probe    = thermoclock_probe,
  .remove   = thermoclock_remove,
  .id_table = thermoclock_ids,
};

unsigned char thermoclock_bitreverse(unsigned char b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

static char *thermoclock_devnode(struct device *dev, umode_t *mode) {
  if (!mode) return NULL;

  // TODO: Sup with this
  /*if (dev->devt == MKDEV(THERMOCLOCK_MAJOR, 0) || dev->devt == MKDEV(THERMOCLOCK_MAJOR, 2)) {*/
    *mode = 0666;
  /*}*/

  return NULL;
}

// == Character device =============================================================================
// This is the interface, located at /dev/thermoclock, between userspace and the SPI driver
// This also handles reading the analog voltage via GPIO pin # TODO iio driver?

// -- File operation callbacks ---------------------------------------------------------------------

static int thermoclock_open(struct inode *inodep, struct file *filep) {
  struct thermoclock_state *state = container_of(inodep->i_cdev, struct thermoclock_state, cdev);

  if (!state) return -EFAULT;
  filep->private_data = state;

  return 0;
}

static ssize_t thermoclock_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
  struct thermoclock_state *state = filep->private_data;

  int errors = copy_to_user(buffer, state->message, state->messageSize);

  if (errors == 0) {
    state->messageSize = 0;

    return 0;
  } else {
    return -EFAULT;
  }
}

// Keeping it portable
// Last byte must be pushed out first, LSB_FIRST is not supported either so we reverse
static ssize_t thermoclock_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
  struct thermoclock_segment *segment;
  struct thermoclock_state   *state;
  unsigned char              *command;

  for (segment = thermoclock_segments; segment->name != NULL; ++segment) {
    if (strncmp(segment->name, buffer, len-1) == 0) {
      state = filep->private_data;
      struct spi_transfer transfer = {
        .rx_buf = state->rx,
        .tx_buf = state->tx,
        .len    = 3,
      };
      command = (unsigned char*)&segment->value;

      if (segment->value < 0) return len;

      state->tx[0] = thermoclock_bitreverse(command[0]);
      state->tx[1] = thermoclock_bitreverse(command[1]);
      state->tx[2] = thermoclock_bitreverse(command[2]);

      // TODO: Check errors
      spi_sync_transfer(state->spi, &transfer, 1);

      return len;
    }
  }

  return len;
}

static int thermoclock_release(struct inode *inodep, struct file *filep) {
  filep->private_data = NULL;

  return 0;
}

// == SPI device ===================================================================================

static int thermoclock_probe(struct spi_device* spi) {
  struct thermoclock_state *state  = NULL;
  static struct device     *devicep = NULL;  // The device driver device struct pointer
  int                      ret;

  // -- State --------------------------------------------------------------------------------------

  state = kzalloc(sizeof(struct thermoclock_state), GFP_KERNEL); 
  if (unlikely(!state)) {
    ret = -ENOMEM;
    goto out;
  }

  // -- SPI device ---------------------------------------------------------------------------------

  // Setup SPI bus

  spi->bits_per_word = 8;
  spi->mode          = SPI_MODE_0;

  ret = spi_setup(spi);
  if (ret < 0) goto out;

  // Setup driver data

  state->spi = spi;
  dev_set_drvdata(&spi->dev, state);

  // -- Character device ---------------------------------------------------------------------------

  state->devno = MKDEV(thermoclock_major, 0);

  // Register device

  cdev_init(&state->cdev, &thermoclock_fops);
  state->cdev.owner = THIS_MODULE;
  state->cdev.ops   = &thermoclock_fops;

  ret = cdev_add(&state->cdev, state->devno, 1);
  if (ret) {
    printk(KERN_ALERT "Failed to add character device\n");
    goto out;
  }

  // Create class

  state->class = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(state->class)) {
    printk(KERN_ALERT "Failed to register device class\n");
    goto out;
  }

  // Device permissions

  state->class->devnode = thermoclock_devnode;

  // Create device

  devicep = device_create(state->class, NULL, state->devno, NULL, DEVICE_NAME);
  if (IS_ERR(devicep)) {
    printk(KERN_ALERT "Failed to create the device\n");
    class_destroy(state->class);
    goto out;
  }


  return 0;

out:
    unregister_chrdev_region(state->devno, 1);

    if (state) kfree(state);

    return ret;
}

static int thermoclock_remove(struct spi_device* spi) {
  struct thermoclock_state *state = dev_get_drvdata(&spi->dev);

  if (state) {
    cdev_del(&state->cdev);
    unregister_chrdev_region(state->devno, 1);
    device_destroy(state->class, state->devno); 
    class_destroy(state->class);
  }

  dev_set_drvdata(&spi->dev, NULL);

  return 0;
}

// == LKM ==========================================================================================

MODULE_DEVICE_TABLE(spi, thermoclock_ids);

module_spi_driver(thermoclock_spi_driver);

MODULE_AUTHOR("Ryan Scott Lewis <ryanscottlewis@gmail.com>");
MODULE_DESCRIPTION("Thermoclock SPI device driver");
MODULE_LICENSE("GPL v2");

