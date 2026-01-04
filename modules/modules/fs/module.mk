dirs-y :=

dirs-$(CONFIG_MOD_FS_DEVFS)   += devfs.mod
dirs-$(CONFIG_MOD_FS_DEVMEM)  += devmem.mod
dirs-$(CONFIG_MOD_FS_DEVNULL) += devnull.mod

include $(patsubst %,$(MDIR)%/module.mk,$(dirs-y))
