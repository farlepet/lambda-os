MOD_TARGET := modules-$(MOD_NAME)

MOD_PATH   := $(MODBUILDDIR)/modules/$(MOD_NAME).mod
MOD_SHARED := $(subst .mod,.so,$(MOD_PATH))

MOD_REQ     := $(shell $(MODUTIL) deps $(MOD_SOURCE)module.json)
MOD_REQMODS  = $(addprefix $(MODBUILDDIR)/modules/,$(MOD_REQ))
MOD_REQFLAGS = $(addprefix -L,$(MOD_REQMODS))

MOD_JSON     := $(MOD_SOURCE)module.json
MOD_HEAD_SRC := $(call src2build,$(MOD_SOURCE)_modhead.c)
MOD_HEAD     := $(patsubst %.c,%.o,$(MOD_HEAD_SRC))

obj-y += $(MOD_HEAD)

modules-y += $(MOD_PATH)

MOD_OBJS := $(obj-y)
$(MOD_PATH): MOD_PATH := $(MOD_PATH)
$(MOD_PATH): MOD_OBJS := $(obj-y)
$(MOD_PATH): MOD_SHARED := $(MOD_SHARED)
$(MOD_PATH): MOD_REQ := $(MOD_REQ)
$(MOD_PATH): MOD_HEAD := $(MOD_HEAD)
$(MOD_PATH): TARGET_CFLAGS := $(cflags-y) $(MOD_CFLAGS) -I $(MOD_SOURCE)inc

$(MOD_PATH): $(MOD_OBJS) $(MOD_REQMODS)
	@echo -e "\033[33m  \033[1mMOD     \033[34m$@\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) -nostdlib -ffreestanding -fPIC -r -o $@ $(MOD_OBJS)
	$(Q) $(STRIP) -g --strip-unneeded $(MOD_PATH)

$(MOD_HEAD_SRC): MOD_JSON := $(MOD_JSON)
$(MOD_HEAD): MOD_HEAD_SRC := $(MOD_HEAD_SRC)

$(MOD_HEAD_SRC): $(MOD_JSON) $(MODUTIL)
	@echo -e "\033[32m  \033[1mCGEN    \033[34m$@\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(MODUTIL) cgen $< > $@

$(MOD_HEAD): $(MOD_HEAD_SRC)
	@echo -e "\033[32m  \033[1mMHD     \033[34m$@\033[0m"
	$(Q) $(CC) $(MODULE_CFLAGS) $(TARGET_CFLAGS) -c -o $@ $<


.PHONY: $(MOD_TARGET) $(MOD_TARGET)-clean

$(MOD_TARGET): $(MOD_PATH)

$(MOD_TARGET)-clean:
	$(Q) rm -f $(MOD_OBJS) $(MOD_PATH)

