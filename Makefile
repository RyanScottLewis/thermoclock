include /usr/share/rive/make/env.make

#DTS     = $(shell find dts -type f -name '*.dts')
#DTBO    = $(patsubst dts/%.dts, $(BUILD)/%.dtbo, $(DTS))
#KO      = $(patsubst src/%.c, $(BUILD)/%.ko.gz, $(SOURCES))

C    = $(filter %.c,$(SOURCES))
DTS  = $(filter %.dts,$(SOURCES))
DTBO = $(DTS:$(SRCDIR)/%.dts=$(BUILDDIR)/%.dtbo)
KO   = $(C:$(SRCDIR)/%.c=$(BUILDDIR)/%.ko.gz)

ARCH      = $(shell uname -r)
BUILDDIR := $(BUILDDIR)/$(ARCH)
MODDIR    = /lib/modules/$(ARCH)

DEST_KO   = $(DESTDIR)/$(MODDIR)/kernel/drivers/rypi
DEST_DTBO = $(DESTDIR)/boot/overlays

include /usr/share/rive/make/build.make
include /usr/share/rive/make/install.make
include /usr/share/rive/make/clean.make
include /usr/share/rive/make/list.make

build:: $(DTBO) $(KO)

install::
	$(call install,$(DTBO),$(DEST_DTBO))
	$(call install,$(KO),$(DEST_KO))

uninstall::
	$(call uninstall,$(DTBO),$(DEST_DTBO))
	$(call uninstall,$(KO),$(DEST_KO))

$(KO): $(C)
	touch $(BUILDDIR)/Makefile # I don't like this. But, it works.
	make -C $(MODDIR)/build M=$(abspath $(BUILDDIR)) src=$(abspath $(SRCDIR)) modules
	gzip -f $(BUILDDIR)/thermoclock.ko

$(DTBO): $(DTS)
	dtc -@ -I dts -O dtb -o $@ $<

