LIBDIR    := $(USERDIR)/lib
UTILSDIR  := $(USERDIR)/utils

USERBUILDDIR := $(BUILDDIR)/user

# Files to copy into /bin
user-bin-y :=

include $(LIBDIR)/lib.mk
include $(UTILSDIR)/utils.mk

.PHONY: user-clean

user-clean: lib-clean utils-clean
	#$(Q) rm -rf $(INITRDDIR)/bin

