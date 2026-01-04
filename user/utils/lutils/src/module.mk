obj-y += $(call mapout, main.o)

dirs-y := cmd

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

