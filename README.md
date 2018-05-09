# Thermoclock

An implementation of a clock display for Linux from hardware, to kernel SPI/character device drivers,
to user space application.

## Hardware

The display is pulled from an old clock, It has 4 seven segment digits, 2 dot delimiter, each day of
the week, AM indicator, and a few errata as outputs.

Using a multimeter in diode/LED mode, the IO was determined to use the first 2 pins as separate
common-anode pins for two "banks" of segments, which are assigned in a strange manner (detailed in
[Pinout.md][pinout].)

I also discovered that the very first digit has no "F" segment. I think this is to save money on a
single LED, since that digit on the clock probably only ever showed a 1 or 2, so the F segment was
not needed.

To interface with this, I've used 3 74HC595 serial-in parallel-out (SIPO) shift registers connected
in series. This is then connected to the MISO and MOSI lines of the SPI bus. The CLK and CE lines
on all the shift registers are tied together and also connected to the SPI bus.

This way, to power a bank the shift register needs to contain a 1 or high signal at the bit offset
where the common-anode pin for that bank is connected to. To power a segment on that bank, the shift
register must contain a 0 or low signal at the bit offset the segment's cathode pin is connected to.

## Driver

The Linux driver is a simple SPI driver, which then sets up a character device file.

At the moment, the segment's name/identifier can be written to the character device file and the
corrent data (command) will be shifted into the registers to power that segment.

The syntax is:

```
0G   0th digit, segment G
3A   2rd digit, segment A
WED  segment for WED
```

The full list can be found in `src/thermoclock.h`.

### Overlay

The driver can be mounted on another SPI bus or use another CE line by using parameters when adding
the device to the device tree:

`/boot/config.txt` (on RaspberryPi)

```
# Use CE 1
dtoverlay=thermoclock:reg=1

# Use different SPI frequency (74HC595 max speed)
dtoverlay=thermoclock:spi-max-frequency=100000000
```

## Application

Usage: `thermoclock`

The application parses the current time and generates a list of commands that will display the correct
current time. It then cycles through this list of commands and sends them repeatedly, using persistance
of vision to give the illusion that all of the segments are active at the same time.

The display can also be cleared, if needed with `thermoclock --clear` or `thermoclock -c`

## TODO

* Display temp
  * NOAA/Wunderground APIs
* Eliminate flickers
  * Use binary format?
  * Send raw commands directly instead of translating within the char driver?

## Development

### Linux

```sh
sudo pacman -S linux-headers
```

### RaspberryPi

```sh
sudo pacman -S linux-raspberrypi-headers
```

### Cross Compiling

> TODO

[pinout]: ./drc/Pinout.md
