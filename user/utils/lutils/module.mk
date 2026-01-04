BIN_NAME   := lutils
BIN_SOURCE := $(MDIR)
BIN_CFLAGS :=

include user/mk/user_bin.mk

LUTILS_BIN := $(call mapout,lutils)

