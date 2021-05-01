MAINDIR    = $(CURDIR)
KERNELDIR  = $(MAINDIR)/lambda-kern
BUILDDIR   = $(MAINDIR)/build
INITRDDIR  = $(BUILDDIR)/initrd

KERNEL      = $(KERNELDIR)/lambda.kern
STRIPKERNEL = $(BUILDDIR)/lambda.kern
CPIOFILES   = $(shell find $(INITRDDIR))

INITRD     = $(BUILDDIR)/initrd.cpio
FLOPPY     = $(BUILDDIR)/lambda-os.img
ISO        = $(BUILDDIR)/lambda-os.iso

export CC
export AS
export LD
export AR
export STRIP
export CROSS_COMPILE
export CFLAGS
export LDFLAGS
export VERBOSE

cdrom: $(ISO)

floppy: $(FLOPPY)

$(ISO): $(STRIPKERNEL) $(INITRD) $(BUILDDIR)/CD/boot/grub/stage2_eltorito
	@cp $(INITRD) $(BUILDDIR)/CD/
	@grub-mkrescue -o $(ISO) $(BUILDDIR)/CD

$(FLOPPY): $(STRIPKERNEL) $(INITRD)
	rm -f $(FLOPPY)
	mkdosfs -C $(FLOPPY) 1440
	mcopy -i $(FLOPPY) build/syslinux.cfg ::/
	mcopy -i $(FLOPPY) /usr/lib/syslinux/bios/mboot.c32 ::/
	mcopy -i $(FLOPPY) /usr/lib/syslinux/bios/libcom32.c32 ::/
	mcopy -i $(FLOPPY) $(INITRD) ::/
	mcopy -i $(FLOPPY) $(STRIPKERNEL) ::/lambda.kern
	syslinux -i $(FLOPPY)

$(STRIPKERNEL): $(KERNEL)
	@strip $(KERNEL) -o $(STRIPKERNEL)

$(KERNEL): $(INITRD) 
	@cd $(KERNELDIR); $(MAKE) lambda.kern

$(INITRD): $(CPIOFILES)
	@echo -e "\033[33m  \033[1mGenerating InitCPIO\033[0m"
	@cd $(INITRDDIR); find . | cpio -o -v -O$(INITRD) &> /dev/null

emu:
	@qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot

emu-debug:
	@qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot -gdb tcp::1234 -S


clean:
	@rm -f $(INITRD) $(ISO) $(FLOPPY)
	@cd $(KERNELDIR); $(MAKE) clean