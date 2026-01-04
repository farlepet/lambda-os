#KERNEL     = $(KERNELDIR)/kernel

GIT_VERSION := "$(shell git describe --abbrev=8 --dirty=\* --always --tags)"

# Make sure these variables are empty and setup to be immediate
# Output object file list
obj-y     :=
# Assembler flags
asflags-y :=
# C compiler flags
cflags-y  :=
# Linker flags
ldflags-y :=

# Shared library linking step flags
ldsflags-y :=
# Relocatable object linking step flags
ldoflags-y :=
# Final kernel output linking step flags
ldkflags-y :=


include $(KERNELDIR)/module.mk

ifneq ($(CONFIG_EMBEDDED_INITRD_LZOP),y)
    obj-$(CONFIG_EMBEDDED_INITRD) += initrd.o
endif
obj-$(CONFIG_EMBEDDED_INITRD_LZOP) += initrd.lzo.o

KBUILDDIR   := $(BUILDDIR)/kernel/$(ARCH)/$(CPU)/$(HW)

KERNEL      := $(KBUILDDIR)/lambda.kern
STRIPKERNEL := $(KBUILDDIR)/lambda.kern.strip

OBJS := $(filter %.o,$(patsubst $(KERNELDIR)/%.o,$(KBUILDDIR)/%.o,$(obj-y)))
DEPS := $(filter %.d,$(patsubst %.o,%.d,$(OBJS)))

cflags-y += -I$(KERNELDIR)/inc -I$(KERNELDIR) -I$(KERNELDIR)/arch/$(ARCH)/inc/ \
            -ffreestanding -Wall -Wextra -O2 \
            -pipe -g -fdata-sections -ffunction-sections \
            -DKERNEL_GIT=\"$(GIT_VERSION)\"

KERNEL_CFLAGS := $(cflags-y)
KERNEL_ASFLAGS := $(asflags-y)
KERNEL_LDSFLAGS := $(ldsflags-y)
KERNEL_LDOFLAGS := $(ldoflags-y)
KERNEL_LDKFLAGS := $(ldKflags-y)


.PHONY: kernel clean documentation cppcheck

kernel: $(KERNEL)

kernel-strip: $(STRIPKERNEL)

$(STRIPKERNEL): $(KERNEL)
	$(Q) $(STRIP) $< -o $@

$(KBUILDDIR)/symbols.o: $(KBUILDDIR)/lambda.o
	@echo -e "\033[33m  \033[1mCreating symbol table\033[0m"
	$(Q) scripts/symbols > $(KBUILDDIR)/symbols.c
	$(Q) $(CC) $(KERNEL_CFLAGS) -c -o $(KBUILDDIR)/symbols.o $(KBUILDDIR)/symbols.c

# TODO: Only include this if FEATURE_INITRD_EMBEDDED
$(KBUILDDIR)/initrd.o: initrd.cpio
	@echo -e "\033[33m  \033[1mGenerating embedded InitRD object\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(LD) $(LDARCH) -r -b binary $< -o $@

$(KBUILDDIR)/initrd.lzo.o: initrd.cpio.lzo
	@echo -e "\033[33m  \033[1mGenerating embedded compressed InitRD object\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(LD) $(LDARCH) -r -b binary $< -o $@

$(KBUILDDIR)/lambda.o: $(OBJS)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	$(Q) $(LD) $(KERNEL_LDOFLAGS) -r -o $@ $(OBJS)

$(KBUILDDIR)/lambda.shared: $(KBUILDDIR)/lambda.o
	@echo -e "\033[33m  \033[1mLinking kernel\033[0m"
	$(Q) $(CC) $(KERNEL_LDSFLAGS) -shared -o $@ $< -T $(HWDIR)/hw.ld

$(KERNEL): $(KBUILDDIR)/lambda.o
	@echo -e "\033[33m  \033[1mProducing kernel executable\033[0m"
	$(Q) $(CC) $(KERNEL_LDKFLAGS) -o $@ $< -T $(HWDIR)/hw.ld -nostdlib -lgcc



kernel-clean:
	@echo -e "\033[33m  \033[1mCleaning sources\033[0m"
	$(Q) rm -rf $(KBUILDDIR)
	$(Q) rm -rf doc

kernel-doc:
	@echo -e "\033[32mGenerating documentation\033[0m"
	@doxygen Doxyfile

kernel-cppcheck:
	$(Q) cppcheck --enable=all --suppress=arithOperationsOnVoidPointer --suppress=unusedFunction -I kernel/inc -I kernel/arch/$(ARCH)/inc kernel/

kernel-scan-build:
	@scan-build --use-cc=$(CC) -analyze-headers $(MAKE)


$(KBUILDDIR)/%.o: $(KERNELDIR)/%.c $(KERNELDIR)/.config
	@echo -e "\033[32m    \033[1mCC\033[21m    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(KERNEL_CFLAGS) -MMD -MP -c -o $@ $<

$(KBUILDDIR)/%.o: $(KERNELDIR)/%.s $(KERNELDIR)/.config
	@echo -e "\033[32m    \033[1mAS\033[21m    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(AS) $(KERNEL_ASFLAGS) -c -o $@ $<

-include $(DEPS)
