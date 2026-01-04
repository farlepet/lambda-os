MOD_NAME   := fs.devfs
MOD_SOURCE := $(MDIR)
MOD_CFLAGS :=

obj-y := $(call mapout, devfs.o main.o)

include modules/mk/module.mk
