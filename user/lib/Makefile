MAINDIR    = $(CURDIR)
SRC        = $(MAINDIR)/src
INC        = $(MAINDIR)/inc

SRCS       = $(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c) $(wildcard $(SRC)/*/*/*.c) $(wildcard $(SRC)/*/*/*/*.c)

OBJS       = $(patsubst %.c,%.o,$(SRCS))

CFLAGS    += -m32 -march=i586 \
             -nostdlib -nostdinc -ffreestanding \
			 -Wall -Wextra -Werror \
			 -fno-stack-protector \
			 -I $(INC)
LDFLAGS    = -melf_i386 -T ../../linker_test.ld

OUT        = liblambda.a
CRT0       = $(MAINDIR)/crt0.o

#--target=elf32-i386

link:   $(OBJS) $(CRT0)
	@echo -e "\033[33m  \033[1mCombining Objects\033[0m"
	$(AR) rvsc  $(OUT) $(OBJS) &> /dev/null

$(CRT0):
	@cd crt; $(MAKE)

all: link


# gcc:
%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<


clean:
	@rm -f $(OBJS) $(OUT)
