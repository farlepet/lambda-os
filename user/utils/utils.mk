UTILSOBJS :=

LINITDIR := $(UTILSDIR)/linit
LSHELLDIR := $(UTILSDIR)/lshell
LUTILSDIR := $(UTILSDIR)/lutils

UTILSBUILDDIR := $(BUILDSUBDIR)

utils-y :=

include $(LINITDIR)/module.mk
include $(LSHELLDIR)/module.mk
include $(LUTILSDIR)/module.mk

user-bin-y := $(utils-y)

.PHONY: utils utils-clean

UTILS_CFLAGS := -I$(LIBDIR)/inc \
                -nostdlib -nostdinc -ffreestanding \
                -fno-stack-protector \
                -Wall -Wextra -Werror -O2 \
                -pipe -fdata-sections -ffunction-sections

UTILSDEPS := $(filter %.d,$(patsubst %.o,%.d,$(UTILSOBJS)))

UTILS_LDFLAGS := -T $(LIBDIR)/linker.ld

$(UTILSBUILDDIR)%.o: $(UTILSDIR)/%.c
	@echo -e "\033[32m    \033[1mCC\033[21m    \033[34m$<\033[0m"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(UTILS_CFLAGS) -MMD -MP -c -o $@ $<

utils: $(utils-y)

utils-clean: utils-linit-clean utils-lshell-clean utils-lutils-clean

-include $(UTILSDEPS)
