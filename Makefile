MAINDIR    = $(CURDIR)
KERNELDIR  = $(MAINDIR)/lambda-kern
BUILDDIR   = $(MAINDIR)/build
INITRDDIR  = $(BUILDDIR)/initrd

KERNSRC     = $(shell find $(KERNELDIR)/kernel -type f \( -iname *.h -o \
                                                          -iname *.c -o \
														  -iname *.h \))
KERNEL      = $(KERNELDIR)/build/lambda.kern
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
	$(Q) grub-mkrescue -o $(ISO) $(BUILDDIR)/CD

$(FLOPPY): $(STRIPKERNEL) $(INITRD)
	$(Q) rm -f $(FLOPPY)
	$(Q) mkdosfs -C $(FLOPPY) 1440
	$(Q) mcopy -i $(FLOPPY) build/syslinux.cfg ::/
	$(Q) mcopy -i $(FLOPPY) /usr/lib/syslinux/bios/mboot.c32 ::/
	$(Q) mcopy -i $(FLOPPY) /usr/lib/syslinux/bios/libcom32.c32 ::/
	$(Q) mcopy -i $(FLOPPY) $(INITRD) ::/
	$(Q) mcopy -i $(FLOPPY) $(STRIPKERNEL) ::/lambda.kern
	$(Q) syslinux -i $(FLOPPY)

$(STRIPKERNEL): $(KERNEL)
	$(Q) $(STRIP) $(KERNEL) -o $(STRIPKERNEL)

$(KERNEL): $(INITRD) $(KERNSRC)
	$(Q) cd $(KERNELDIR); $(MAKE) build/lambda.kern

$(INITRD): $(CPIOFILES)
	@echo -e "\033[33m  \033[1mGenerating InitCPIO\033[0m"
	$(Q) cd $(INITRDDIR); find . | cpio -o -v -O$(INITRD) &> /dev/null
	$(Q) cp $(INITRD) $(KERNELDIR)/initrd.cpio

emu:
	$(Q) qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot

emu-debug:
	$(Q) qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot -gdb tcp::1234 -S


clean:
	$(Q) rm -f $(INITRD) $(ISO) $(FLOPPY)
	$(Q) cd $(KERNELDIR); $(MAKE) clean