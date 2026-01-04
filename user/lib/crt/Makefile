MAINDIR    = $(CURDIR)


CRT0SRC  = $(wildcard $(MAINDIR)/crt0/*.s)
CRT0OBJS = $(patsubst %.s,%.o,$(CRT0SRC))
CRT0OUT  = $(MAINDIR)/crt0.o

OBJS = $(CRT0OBJS)

LDFLAGS = -melf_i386 -r
ASFLAGS = -m32
#AS      = gcc


link:   $(OBJS)
	@echo -e "\033[33m  \033[1mLinking CRT0\033[0m"
	@$(LD) $(LDFLAGS) -o $(CRT0OUT) $(CRT0OBJS)


all: link


clean:
	@rm -f $(OBJS) $(CRT0OUT)



%.o: %.s
	@echo -e "\033[32m    \033[1mAS\033[21m    \033[34m$<\033[0m"
	@$(CC) $(ASFLAGS) -c -o $@ $<
