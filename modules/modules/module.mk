dirs-y:= drv \
         fs \
         tty

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

