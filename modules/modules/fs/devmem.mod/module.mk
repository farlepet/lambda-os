MOD_NAME   := fs.devmem
MOD_SOURCE := $(MDIR)
MOD_CFLAGS :=

obj-y := $(call mapout, main.o)

include modules/mk/module.mk
