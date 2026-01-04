obj-y += $(call mapout, sys/syscalls.o)

dirs-y := sys/syscalls

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

