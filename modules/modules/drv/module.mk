dirs-y:= io

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

