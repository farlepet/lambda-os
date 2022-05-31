MAINDIR    = $(CURDIR)
KERNELDIR  = $(MAINDIR)/lambda-kern
BUILDDIR   = $(MAINDIR)/build
INITRDDIR  = $(BUILDDIR)/initrd

KERNSRC     = $(shell find $(KERNELDIR)/kernel -type f \( -iname *.h -o -iname *.c \))
KERNEL      = $(KERNELDIR)/build/x86/ia32/pc/lambda.kern
STRIPKERNEL = $(BUILDDIR)/lambda.kern
CPIOFILES   = $(shell find $(INITRDDIR))

INITRD     = $(BUILDDIR)/initrd.cpio
FLOPPY     = $(BUILDDIR)/lambda-os.img
ISO        = $(BUILDDIR)/lambda-os.iso

STRIP      = $(CROSS_COMPILE)strip

export CC
export AS
export LD
export AR
export STRIP
export CROSS_COMPILE
export CFLAGS
export LDFLAGS
export VERBOSE

ifeq ($(VERBOSE), 1)
Q =
else
Q = @
endif

# TODO: Split into architecture-dependant includable mk files

cdrom: $(ISO)

floppy: $(FLOPPY)

$(ISO): $(STRIPKERNEL) $(INITRD) $(BUILDDIR)/CD/boot/grub/stage2_eltorito
	$(Q) cp $(INITRD) $(STRIPKERNEL) $(BUILDDIR)/CD/
	$(Q) grub-mkrescue -o $@ $(BUILDDIR)/CD

$(FLOPPY): $(STRIPKERNEL) $(INITRD)
	$(Q) rm -f $@
	$(Q) mkdosfs -C $@ 1440
	$(Q) mcopy -i $@ build/syslinux.cfg ::/
	$(Q) mcopy -i $@ /usr/lib/syslinux/bios/mboot.c32 ::/
	$(Q) mcopy -i $@ /usr/lib/syslinux/bios/libcom32.c32 ::/
	$(Q) mcopy -i $@ $(INITRD) ::/
	$(Q) mcopy -i $@ $(STRIPKERNEL) ::/lambda.kern
	$(Q) syslinux -i $@

$(STRIPKERNEL): $(KERNEL)
	$(Q) $(STRIP) $< -o $@

$(KERNEL): $(INITRD) $(KERNSRC)
	$(Q) cd $(KERNELDIR); $(MAKE) build/x86/ia32/pc/lambda.kern

$(INITRD): pop-initrd $(CPIOFILES)
	@echo -e "\033[33m  \033[1mGenerating InitCPIO\033[0m"
	$(Q) cd $(INITRDDIR); find . | cpio -o -v -O$(INITRD) &> /dev/null
	$(Q) cp $(INITRD) $(KERNELDIR)/initrd.cpio

emu:
	$(Q) qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot

emu-debug:
	$(Q) qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot -gdb tcp::1234 -S

clean: clean-user
	$(Q) rm -f $(INITRD) $(ISO) $(FLOPPY)
	$(Q) rm -rf $(INITRDDIR)/bin
	$(Q) cd $(KERNELDIR); $(MAKE) clean

LLIB_DIR=$(MAINDIR)/lambda-lib
LINIT_DIR=$(MAINDIR)/lambda-lutils/linit
LSHELL_DIR=$(MAINDIR)/lambda-lutils/lshell
LUTILS_DIR=$(MAINDIR)/lambda-lutils/lutils

LLIB=$(LLIB_DIR)/liblambda.a

LINIT=$(INITRDDIR)/bin/linit
LSHELL=$(INITRDDIR)/bin/lshell
LUTILS=$(INITRDDIR)/bin/lutils

LLIBSRC=   $(shell find $(LLIBDIR)   -type f \( -iname *.h -o -iname *.c \))
LINITSRC=  $(shell find $(LINITDIR)  -type f \( -iname *.h -o -iname *.c \))
LSHELLSRC= $(shell find $(LSHELLDIR) -type f \( -iname *.h -o -iname *.c \))
LUTILSSRC= $(shell find $(LUTILSDIR) -type f \( -iname *.h -o -iname *.c \))

$(LLIB): $(LLIBSRC)
	@cd $(LLIB_DIR); $(MAKE)

$(LINIT): $(LINITSRC) $(LLIB) $(INITRDDIR)/bin
	@cd $(LINIT_DIR); $(MAKE) LIB=$(LLIB_DIR) OUT=$@

$(LSHELL): $(LSHELLSRC) $(LLIB) $(INITRDDIR)/bin
	@cd $(LSHELL_DIR); $(MAKE) LIB=$(LLIB_DIR) OUT=$@

$(LUTILS): $(LUTILSSRC) $(LLIB) $(INITRDDIR)/bin
	@cd $(LUTILS_DIR); $(MAKE) LIB=$(LLIB_DIR) OUT=$@
	@cd $(INITRDDIR)/bin; rm -f cat; ln -s lutils cat
	@cd $(INITRDDIR)/bin; rm -f ls;  ln -s lutils ls

$(INITRDDIR)/bin:
	mkdir $@

pop-initrd: $(LINIT) $(LSHELL) $(LUTILS)

clean-user:
	$(Q) cd $(LLIB_DIR);   $(MAKE) clean
	$(Q) cd $(LINIT_DIR);  $(MAKE) clean
	$(Q) cd $(LSHELL_DIR); $(MAKE) clean
	$(Q) cd $(LUTILS_DIR); $(MAKE) clean
	$(Q) rm -rf $(INITRDDIR)/bin

