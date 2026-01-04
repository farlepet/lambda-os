dirs-y := mman \
          stat \
          wait

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

