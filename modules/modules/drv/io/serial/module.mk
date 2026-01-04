dirs-y :=

dirs-$(CONFIG_MOD_DRV_IO_SERIAL_8250) += 8250.mod

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))
