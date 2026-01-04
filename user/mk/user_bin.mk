BIN_TARGET := user$(BIN_MODULE)-$(BIN_NAME)

BIN_LDFLAGS := -T $(LIBDIR)/linker.ld

include mk/reset_vars.mk

include $(BIN_SOURCE)src/module.mk

BIN_PATH := $(call src2build,$(BIN_SOURCE))$(BIN_NAME)

utils-y += $(BIN_PATH)

BIN_OBJS := $(obj-y)
$(BIN_PATH) $(BIN_TARGET)-clean: BIN_OBJS := $(obj-y)
$(BIN_PATH) $(BIN_TARGET)-clean: BIN_PATH := $(BIN_PATH)

$(BIN_PATH): TARGET_CFLAGS := $(cflags-y) $(BIN_CFLAGS) -I $(BIN_SOURCE)/inc

$(BIN_PATH): $(CRT0) $(BIN_OBJS) $(LIB_ARCHIVE)
	@echo -e "\033[33m  \033[1mLD $@\033[0m"
	$(Q) $(LD) $(BIN_LDFLAGS) -o $@ $^

.PHONY: $(BIN_TARGET) $(BIN_TARGET)-clean

$(BIN_TARGET): $(BIN_PATH)

$(BIN_TARGET)-clean:
	$(Q) rm -f $(BIN_OBJS) $(BIN_PATH)

