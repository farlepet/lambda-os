LINIT_BIN := $(call mapout,linit)

include mk/reset_vars.mk

include $(LINITDIR)/src/module.mk


utils-y += $(LINIT_BIN)

UTILSOBJS += $(obj-y)
LINITOBJS := $(obj-y)

.PHONY: utils-linit utils-linit-clean

utils-linit: $(LINIT_BIN)

$(LINIT_BIN): $(LINITOBJS) $(LIB_ARCHIVE)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	$(Q) $(LD) $(UTILS_LDFLAGS) -o $@ $(CRT0) $^ $(LIB_ARCHIVE)

utils-linit-clean:
	$(Q) rm -f $(LINIT_BIN) $(LINITOBJS)
