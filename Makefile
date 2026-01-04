MAINDIR    = $(CURDIR)
KERNELDIR  = $(MAINDIR)/kernel
LBOOTDIR   = $(MAINDIR)/lboot
BUILDDIR   = $(MAINDIR)/build
INITRDDIR  = $(BUILDDIR)/initrd

# TODO: Move to root
include $(KERNELDIR)/.config

KERNSRC     = $(shell find $(KERNELDIR)/kernel -type f \( -iname *.h -o -iname *.c \))
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

include $(KERNELDIR)/kernel.mk

include mk/media.mk
include mk/compiler.mk


.PHONY: menuconfig clean pop-initrd clean-user

$(LBOOT_BASE):
	$(Q) cd $(LBOOTDIR) && $(MAKE)

#$(KERNEL): $(INITRD_LZOP) $(KERNSRC)
#	$(Q) cd $(KERNELDIR) && $(MAKE) build/x86/i486/pc/lambda.kern

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
	$(Q) cd $(KERNELDIR); menuconfig

clean: clean-user kernel-clean
	$(Q) rm -f $(INITRD) $(ISO) $(FLOPPY) $(KERNEL)
	$(Q) rm -rf $(INITRDDIR)/bin
	$(Q) cd $(LBOOTDIR) && $(MAKE) clean

LLIB_DIR=$(MAINDIR)/user/lib
LINIT_DIR=$(MAINDIR)/user/utils/linit
LSHELL_DIR=$(MAINDIR)/user/utils/lshell
LUTILS_DIR=$(MAINDIR)/user/utils/lutils

LLIB=$(LLIB_DIR)/liblambda.a

LINIT=$(INITRDDIR)/bin/linit
LSHELL=$(INITRDDIR)/bin/lshell
LUTILS=$(INITRDDIR)/bin/lutils

LLIBSRC=   $(shell find $(LLIBDIR)   -type f \( -iname *.h -o -iname *.c \))
LINITSRC=  $(shell find $(LINITDIR)  -type f \( -iname *.h -o -iname *.c \))
LSHELLSRC= $(shell find $(LSHELLDIR) -type f \( -iname *.h -o -iname *.c \))
LUTILSSRC= $(shell find $(LUTILSDIR) -type f \( -iname *.h -o -iname *.c \))

$(LLIB): $(LLIBSRC)
	@cd $(LLIB_DIR) && $(MAKE)

$(LINIT): $(LINITSRC) $(LLIB) $(INITRDDIR)/bin
	@cd $(LINIT_DIR) && $(MAKE) LIB=$(LLIB_DIR) OUT=$@

$(LSHELL): $(LSHELLSRC) $(LLIB) $(INITRDDIR)/bin
	@cd $(LSHELL_DIR) && $(MAKE) LIB=$(LLIB_DIR) OUT=$@

$(LUTILS): $(LUTILSSRC) $(LLIB) $(INITRDDIR)/bin
	@cd $(LUTILS_DIR) && $(MAKE) LIB=$(LLIB_DIR) OUT=$@
	@cd $(INITRDDIR)/bin && rm -f cat  && ln -s lutils cat
	@cd $(INITRDDIR)/bin && rm -f ls   && ln -s lutils ls
	@cd $(INITRDDIR)/bin && rm -f echo && ln -s lutils echo

$(INITRDDIR)/bin:
	mkdir -p $@

pop-initrd: $(LINIT) $(LSHELL) $(LUTILS)

clean-user:
	$(Q) cd $(LLIB_DIR)   && $(MAKE) clean
	$(Q) cd $(LINIT_DIR)  && $(MAKE) clean
	$(Q) cd $(LSHELL_DIR) && $(MAKE) clean
	$(Q) cd $(LUTILS_DIR) && $(MAKE) clean
	$(Q) rm -rf $(INITRDDIR)/bin

