LIBBUILDDIR = $(USERBUILDDIR)/lib

include mk/reset_vars.mk

include $(LIBDIR)/crt/module.mk
include $(LIBDIR)/src/module.mk

LIBOBJS := $(obj-y)
LIBDEPS := $(filter %.d,$(patsubst %.o,%.d,$(LIBOBJS)))

cflags-y += -I$(LIBDIR)/inc \
            -nostdlib -nostdinc -ffreestanding \
            -fno-stack-protector \
            -Wall -Wextra -Werror -O2 \
            -pipe -fdata-sections -ffunction-sections

LIBCFLAGS := $(cflags-y)

LIB_ARCHIVE := $(LIBBUILDDIR)/liblambda.a

$(LIB_ARCHIVE): $(LIBOBJS) $(CRT0)
	@echo -e "\033[33m  \033[1mCombining Objects\033[0m"
	$(Q) $(AR) rsc $@ $(LIBOBJS)

$(LIBBUILDDIR)/%.o: $(LIBDIR)/%.c
	@echo -e "\033[32m    \033[1mCC\033[21m    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(LIBCFLAGS) -MMD -MP -c -o $@ $<


.PHONY: lib lib-clean

lib: $(LIB_ARCHIVE)

lib-clean:
	$(Q) rm -f $(LIBOBJS) $(LIB_ARCHIVE)

-include $(LIBDEPS)
