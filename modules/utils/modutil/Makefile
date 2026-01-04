MAINDIR    = .

LJSON      = $(MAINDIR)/lambda-json

SRC        = $(MAINDIR)/src
INC        = $(MAINDIR)/inc

SRCS       = $(wildcard $(SRC)/*.c)

OBJS       = $(patsubst %.c,%.o,$(SRCS))
LJSON_LIB  = $(LJSON)/libljson.a

CFLAGS    += -Wall -Wextra -Werror -I$(LJSON)/inc -I$(INC) -g

ifeq ($(CC), clang)
CFLAGS    += -Weverything -Wno-padded
endif

OUT        = modutil

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJS) $(LJSON_LIB)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	@$(CC) -o $(OUT) $(OBJS) $(LJSON_LIB)

$(LJSON_LIB):
	@cd $(LJSON); make

# gcc:
%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<


clean:
	@rm -f $(OBJS) $(OUT)
