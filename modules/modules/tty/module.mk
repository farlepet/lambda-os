dirs-y :=

dirs-$(CONFIG_MOD_TTY_TTY)   += tty.mod

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))
