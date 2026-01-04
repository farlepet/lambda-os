obj-y += $(call mapout, getchar.o \
                        gets.o \
                        putchar.o \
                        puts.o)

dirs-y := fmt

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

