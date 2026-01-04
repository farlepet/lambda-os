dirs-y:= serial

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

