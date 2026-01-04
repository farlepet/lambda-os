obj-y += $(call mapout, libinit.o)

dirs-y := ctype \
          dirent \
          fcntl \
          lambda \
          stdio \
          stdlib \
          string \
          sys \
          unistd \
          wchar

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))

