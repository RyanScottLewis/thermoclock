include /usr/share/rive/make/env.make

#DTS     = $(shell find dts -type f -name '*.dts')
#DTBO    = $(patsubst dts/%.dts, $(BUILD)/%.dtbo, $(DTS))
#KO      = $(patsubst src/%.c, $(BUILD)/%.ko.gz, $(SOURCES))

C    = $(filter %.c,$(SOURCES))
DTS  = $(filter %.dts,$(SOURCES))
SYS  = $(filter %.service,$(SOURCES))
EXE  = $(SRCDIR)/thermoclock
DTBO = $(DTS:$(SRCDIR)/%.dts=$(BUILDDIR)/%.dtbo)
KO   = $(C:$(SRCDIR)/%.c=$(BUILDDIR)/%.ko.gz)

ARCH      = $(shell uname -r)
BUILDDIR := $(BUILDDIR)/$(ARCH)
MODDIR    = /lib/modules/$(ARCH)

DEST_KO   = $(DESTDIR)/$(MODDIR)/kernel/drivers/rypi
DEST_DTBO = $(DESTDIR)/boot/overlays
DEST_EXE  = $(DESTDIR)/usr/bin
DEST_SYS  = $(DESTDIR)/usr/lib/systemd/system

include /usr/share/rive/make/build.make
include /usr/share/rive/make/install.make
include /usr/share/rive/make/clean.make
include /usr/share/rive/make/list.make

build:: $(DTBO) $(KO)

install::
	$(call install,$(DTBO),$(DEST_DTBO))
	$(call install,$(KO),$(DEST_KO))
	$(call install,$(SYS),$(DEST_SYS))
	$(call install,$(EXE),$(DEST_EXE),655)
	depmod
	modprobe thermoclock

uninstall::
	$(call uninstall,$(DTBO),$(DEST_DTBO))
	$(call uninstall,$(KO),$(DEST_KO))
	$(call uninstall,$(SYS),$(DEST_SYS))
	$(call uninstall,$(EXE),$(DEST_EXE))
	rmmod thermoclock

$(KO): $(C)
	touch $(BUILDDIR)/Makefile # I don't like this. But, it works.
	make -C $(MODDIR)/build M=$(abspath $(BUILDDIR)) src=$(abspath $(SRCDIR)) modules
	gzip -f $(BUILDDIR)/thermoclock.ko

$(DTBO): $(DTS)
	dtc -@ -I dts -O dtb -o $@ $<

