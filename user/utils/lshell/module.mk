LSHELL_BIN := $(call mapout,lshell)

include mk/reset_vars.mk

include $(LSHELLDIR)/src/module.mk


utils-y += $(LSHELL_BIN)

$(LSHELL_BIN): UTILS_CFLAGS += -I $(LSHELLDIR)/inc

UTILSOBJS += $(obj-y)
LSHELLOBJS := $(obj-y)

.PHONY: utils-lshell utils-lshell-clean

utils-lshell: $(LSHELL_BIN)

$(LSHELL_BIN): $(LSHELLOBJS) $(LIB_ARCHIVE)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	$(Q) $(LD) $(UTILS_LDFLAGS) -o $@ $(CRT0) $^ $(LIB_ARCHIVE)

utils-lshell-clean:
	$(Q) rm -f $(LSHELL_BIN) $(LSHELLOBJS)
