MAINDIR    = $(CURDIR)
MODULESDIR = $(MAINDIR)/modules

OUTDIR      = $(MAINDIR)/out
MODULES     = $(shell find $(MODULESDIR) -type d -iname "*.mod")
MODULE_OBJS = $(addprefix $(OUTDIR)/,$(subst /,.,$(subst $(MODULESDIR)/,,$(MODULES))))

MODULE_SRCS = $(foreach mod, $(MODULES), $(wildcard $(mod)/*.c))
MODULE_COBJ = $(patsubst %.c,%.o,$(MODULE_SRCS))

export MODULESDIR

.PHONY: all clean

all: utils/modutil/modutil $(OUTDIR) $(MODULE_OBJS)

.SECONDEXPANSION:
$(OUTDIR)/%.mod: $(MODULESDIR)/$$(subst .,/,%).mod
	@echo -e "\033[32m\033[1mMOD\033[21m   \033[34m$@\033[0m"
	@if [ -f "$</Makefile" ]; then                                    \
		$(MAKE) -s -f "$</Makefile" MODDIR=$< MODOUT=$@ OUTDIR=$(OUTDIR); \
	else                                                             \
		$(MAKE) -s -f module.mk MODDIR=$< MODOUT=$@ OUTDIR=$(OUTDIR);     \
	fi

utils/modutil/modutil: $(wildcard utils/modutil/src/*.c) $(wildcard utils/modutil/inc/*.h)
	@echo -e "\033[32m\033[1mMODUTIL\033[0m"
	@cd utils/modutil; $(MAKE)

$(OUTDIR):
	@mkdir $(OUTDIR)

clean:
	@rm -f $(MODULE_OBJS)
	@rm -f $(MODULE_COBJ)
	@cd utils/modutil; $(MAKE) clean
