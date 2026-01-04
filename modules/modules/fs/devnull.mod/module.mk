MOD_NAME   := fs.devnull
MOD_SOURCE := $(MDIR)
MOD_CFLAGS :=

obj-y := $(call mapout, main.o)

include modules/mk/module.mk
