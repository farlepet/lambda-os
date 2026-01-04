MAINDIR    := $(CURDIR)
KERNELDIR  := $(MAINDIR)/kernel
LBOOTDIR   := $(MAINDIR)/lboot
BUILDDIR   := $(MAINDIR)/build
INITRDDIR  := $(BUILDDIR)/initrd
USERDIR    := $(MAINDIR)/user
MODULESDIR := $(MAINDIR)/modules

MDIR = $(dir $(lastword $(MAKEFILE_LIST)))

include .config

#KERNSRC     = $(shell find $(KERNELDIR) -type f \( -iname *.h -o -iname *.c \))
CPIOFILES   = $(shell find $(INITRDDIR))

INITRD      = $(BUILDDIR)/initrd.cpio
INITRD_LZOP = $(BUILDDIR)/initrd.cpio.lzo

LBOOT_BASE = $(LBOOTDIR)/boot.img



export CFLAGS
export LDFLAGS
export VERBOSE

VERBOSE = 0
ifeq ($(VERBOSE), 1)
Q =
else
Q = @
endif

include mk/compiler.mk

include $(KERNELDIR)/kernel.mk
include $(USERDIR)/user.mk
include $(MODULESDIR)/module.mk

include mk/media.mk

.PHONY: menuconfig clean pop-initrd

$(LBOOT_BASE):
	$(Q) cd $(LBOOTDIR) && $(MAKE)

$(INITRD): pop-initrd $(CPIOFILES)
	@echo -e "\033[33m  \033[1mGenerating InitCPIO\033[0m"
	$(Q) cp -rT rootfs $(INITRDDIR)
	$(Q) cd $(INITRDDIR) && find . | cpio -o -v -O$(INITRD) &> /dev/null
	$(Q) cp $(INITRD) $(KERNELDIR)/initrd.cpio

$(INITRD_LZOP): $(INITRD)
	@echo -e "\033[33m  \033[1mCompressing InitCPIO\033[0m"
	$(Q) lzop -9 -f -o $@ $<
	$(Q) cp $(INITRD_LZOP) $(KERNELDIR)/initrd.cpio.lzo

menuconfig:
	$(Q) menuconfig

clean: user-clean kernel-clean
	$(Q) rm -f $(INITRD) $(ISO) $(FLOPPY) $(KERNEL)
	$(Q) rm -rf $(INITRDDIR)/bin
	$(Q) cd $(LBOOTDIR) && $(MAKE) clean


pop-initrd: $(user-bin-y) $(modules-y)
	$(Q) mkdir -p $(INITRDDIR)/bin
	$(Q) cp $(user-bin-y) $(INITRDDIR)/bin
# Add links (TODO: Make this cleaner)
	$(Q) cd $(INITRDDIR)/bin && rm -f cat ls echo && \
		ln -s lutils cat && \
		ln -s lutils ls  && \
		ln -s lutils echo
# Copy in build modules
	$(Q) mkdir -p $(INITRDDIR)/modules
	$(Q) cp $(modules-y) $(INITRDDIR)/modules

.config: | .defconfig
	@echo -e "\033[32m\033[1mCopying default .config\033[0m"
	$(Q) cp $| $@

