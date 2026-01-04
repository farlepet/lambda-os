
KERNEL_INC  = $(KERNELDIR)/inc
KERNEL_ARCH = $(KERNELDIR)/arch/$(ARCH)/inc

MODBUILDDIR := $(BUILDDIR)/modules


MODUTIL_DIR := $(MODULESDIR)/utils/modutil
MODUTIL     := $(MODUTIL_DIR)/modutil

MODULE_CFLAGS  := $(KERNEL_CFLAGS) \
              -iquote $(MODULESDIR)
MODULE_ASFLAGS := $(KERNEL_ASFLAGS)

modules-y :=

include $(MODULESDIR)/modules/module.mk

$(MODUTIL):
	@cd $(MODUTIL_DIR); $(MAKE)

$(MODBUILDDIR)/%.o: $(MODULESDIR)/%.c
	@echo -e "\033[32m    \033[1mCC    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(MODULE_CFLAGS) $(TARGET_CFLAGS) -MMD -MP -c -o $@ $<

$(MODBUILDDIR)/%.o: $(MODULESDIR)/%.s
	@echo -e "\033[32m    \033[1mAS    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(AS) $(MODULE_ASFLAGS) $(TARGET_ASFLAGS) -c -o $@ $<


.PHONY: modules modules-modutil modules-clean modules-modutil-clean

modules: $(modules-y)

modules-modutil: $(MODUTIL)

modules-clean: modules-modutil-clean
	@rm -f $(OBJS) $(MODOUT)
	@rm -f $(MODDIR)/_modhead.c $(MODDIR)/_modhead.o

modules-modutil-clean:
	@cd $(MODUTIL_DIR); $(MAKE) clean

# TODO: Deps

