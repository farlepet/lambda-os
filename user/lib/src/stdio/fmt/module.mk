obj-y += $(call mapout, printf.o \
                        sprintf.o)

dirs-y := helpers

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

