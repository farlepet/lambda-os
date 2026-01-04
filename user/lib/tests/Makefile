MAINDIR    = $(CURDIR)
LIB        = $(MAINDIR)/..
LIBINC     = $(LIB)/inc

SRCS       = $(wildcard */*.c) $(wildcard */*/*.c)

OBJS       = $(patsubst %.c,%.o,$(SRCS))
OUT        = $(patsubst %.c,%,$(SRCS))

CFLAGS    += -m32 -nostdlib -nostdinc -ffreestanding -Wall -Wextra -Werror -I$(LIBINC)
LDFLAGS    = -melf_i386 -T ../linker.ld

all: $(OBJS) $(OUT)

# gcc:
%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<

%: %.o
	@echo -e "\033[33m  \033[1mLD\033[21m    \033[34m$<\033[0m"
	@$(LD) $(LDFLAGS) -o $@ $(LIB)/crt/crt0.o $< $(LIB)/liblambda.a

clean:
	@rm -f $(OUT) $(OBJS)
