FLOPPY      = $(BUILDDIR)/lambda-os.img
ISO         = $(BUILDDIR)/lambda-os.iso

floppy: $(FLOPPY)

cdrom: $(ISO)

# TODO: Allow choosing decompressed initrd
$(FLOPPY): $(STRIPKERNEL) $(INITRD) $(INITRD_LZOP) $(LBOOT_BASE)
	$(Q) rm -f $@
	$(Q) cp $(LBOOT_BASE) $@
	$(Q) mcopy -D oO -i $@ build/lboot.cfg ::/LBOOT/LBOOT.CFG
	$(Q) #mcopy -D oO -i $@ $(INITRD)       ::/INITRD
	$(Q) mcopy -D oO -i $@ $(INITRD_LZOP)  ::/INITRD
	$(Q) mcopy -D oO -i $@ $(STRIPKERNEL)  ::/KERNEL.ELF

$(ISO): $(STRIPKERNEL) $(INITRD) $(BUILDDIR)/CD/boot/grub/stage2_eltorito
	$(Q) cp $(INITRD) $(STRIPKERNEL) $(BUILDDIR)/CD/
	$(Q) grub-mkrescue -o $@ $(BUILDDIR)/CD


emu: $(ISO)
	$(Q) qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot

emu-floppy: $(FLOPPY)
	$(Q) qemu-system-i386 -fda $(FLOPPY) -serial stdio -machine pc -no-reboot

emu-floppy-dbg: $(FLOPPY)
	$(Q) qemu-system-i386 -fda $(FLOPPY) -serial stdio -machine pc -no-reboot -gdb tcp::1234 -S

emu-floppy-slow: $(FLOPPY)
	$(Q) qemu-system-i386 -drive file=$(FLOPPY),if=floppy,format=raw,bps=12500 \
	                      -serial stdio -machine pc -no-reboot

emu-debug: $(ISO)
	$(Q) qemu-system-i386 -cdrom $(ISO) -serial stdio -machine pc -no-reboot -gdb tcp::1234 -S
