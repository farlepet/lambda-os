MOD_NAME   := io.serial.8250
MOD_SOURCE := $(MDIR)
MOD_CFLAGS :=

obj-y := $(call mapout, 8250.o main.o)

include modules/mk/module.mk
