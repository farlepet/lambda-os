LUTILS_BIN := $(call mapout,lutils)

include mk/reset_vars.mk

include $(LUTILSDIR)/src/module.mk


utils-y += $(LUTILS_BIN)

$(LUTILS_BIN): UTILS_CFLAGS += -I $(LUTILSDIR)/inc

UTILSOBJS += $(obj-y)
LUTILSOBJS := $(obj-y)

.PHONY: utils-lutils utils-lutils-clean

utils-lutils: $(LUTILS_BIN)

$(LUTILS_BIN): $(LUTILSOBJS) $(LIB_ARCHIVE)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	$(Q) $(LD) $(UTILS_LDFLAGS) -o $@ $(CRT0) $^ $(LIB_ARCHIVE)

utils-lutils-clean:
	$(Q) rm -f $(LUTILS_BIN) $(LUTILSOBJS)
