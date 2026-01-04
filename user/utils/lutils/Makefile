MAINDIR    = $(CURDIR)
LIB        = $(MAINDIR)/../lib
LIBINC     = $(LIB)/inc

SRC        = $(MAINDIR)/src
INC        = $(MAINDIR)/inc

SRCS       = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c)

OBJS       = $(patsubst %.c,%.o,$(SRCS))

CFLAGS    += -m32 -nostdlib -nostdinc -Wall -Wextra -Werror -fno-stack-protector -I$(LIBINC) -I$(INC) -g
LDFLAGS    = -melf_i386 -T $(LIB)/linker.ld

ifeq ($(patsubst %-gcc,,$(CC)),)
CFLAGS    += -Wno-builtin-declaration-mismatch
else
CFLAGS    += -Wno-incompatible-library-redeclaration 
endif

OUT        = lutils

link:   $(OBJS)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	@$(LD) $(LDFLAGS) -o $(OUT) $(LIB)/crt/crt0.o $(OBJS) $(LIB)/liblambda.a

all: link


# gcc:
%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<


clean:
	@rm -f $(OBJS) $(OUT) $(OUT).bin
