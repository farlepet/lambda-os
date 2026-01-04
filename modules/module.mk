# Required variables:
#   MODDIR: Directory of module
#   MODOUT: Module output file
#   OUTDIR: Module output directory
# Optional variables:
#   MODREQ: Module pre-requisites

MAINDIR     = $(CURDIR)

# TODO: Multiple architecture-support
ARCH        = x86
KERNEL      = $(MAINDIR)/../lambda-kern
KERNEL_INC  = $(KERNEL)/kernel/inc
KERNEL_ARCH = $(KERNEL)/kernel/arch/x86/inc

CROSS_COMPILE =
CC            = $(CROSS_COMPILE)gcc
AS            = $(CROSS_COMPILE)gcc

MODUTIL     = $(MAINDIR)/utils/modutil/modutil

MODREQ     := $(shell $(MODUTIL) deps $(MODDIR)/module.json)

MODSHARED   = $(subst .mod,.shared,$(MODOUT))

CSRCS       = $(wildcard $(MODDIR)/*.c)
CSRCS      := $(filter-out $(MODDIR)/_modhead.c, $(CSRCS))
ASSRCS      = $(wildcard $(MODDIR)/*.s)
OBJS        = $(patsubst %.c,%.o,$(CSRCS)) $(patsubst %.s,%.o,$(ASSRCS))

REQMODS     = $(addprefix $(OUTDIR)/,$(MODREQ))
REQFLAGS    = $(addprefix -L,$(REQMODS))

CFLAGS     += -m32 -nostdlib -nostdinc -ffreestanding \
              -Wall -Wextra -Werror -fno-stack-protector \
			  -D ARCH_X86 \
			  -I$(KERNEL_INC) -I$(KERNEL_ARCH)\
			  -iquote $(MODULESDIR)
ASFLAGS    = -m32
LDFLAGS     = -melf_i386 -l:lambda.shared --no-dynamic-linker -pic -L$(KERNEL)/build/x86/ia32/pc -e0x0 -shared

ifeq ($(CC), clang)
CFLAGS    += -Wno-incompatible-library-redeclaration 
else
CFLAGS    += -Wno-builtin-declaration-mismatch
endif

$(MODOUT): $(MODDIR)/_modhead.o $(OBJS) $(REQMODS)
	@echo -e "\033[32m  \033[1mLD\033[21m    \033[34m$@\033[0m"
	@$(LD) $(LDFLAGS) $(REQFLAGS) -o $@ $(OBJS) $(MODDIR)/_modhead.o

.SECONDEXPANSION:
$(OUTDIR)/%.mod: $(MODULESDIR)/$$(subst .,/,%).mod
	@echo -e "\033[32m\033[1mMOD\033[21m   \033[34m$@\033[0m"
	@if [ -f "$</Makefile" ]; then                                    \
		$(MAKE) -s -f "$</Makefile" MODDIR=$< MODOUT=$@ OUTDIR=$(OUTDIR); \
	else                                                             \
		$(MAKE) -s -f module.mk MODDIR=$< MODOUT=$@ OUTDIR=$(OUTDIR);     \
	fi

$(MODDIR)/_modhead.c: $(MODDIR)/module.json
	@echo -e "\033[32m  \033[1mCGEN\033[21m \033[34m$@\033[0m"
	$(MODUTIL) cgen $< > $@


%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	@echo -e "\033[32m  \033[1mAS\033[21m    \033[34m$<\033[0m"
	@$(AS) $(ASFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	@rm -f $(OBJS) $(MODOUT)
	@rm -f $(MODDIR)/_modhead.c $(MODDIR)/_modhead.o
