MDIR = $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += $(MDIR)cbuff.o \
         $(MDIR)cobuff.o \
         $(MDIR)llist.o

