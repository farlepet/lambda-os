MAINDIR    = ../../../

LJSON      = $(MAINDIR)/lib/lambda-json

SRC        = src
INC        = inc

SRCS       = $(wildcard $(SRC)/*.c)

OBJS       = $(patsubst %.c,%.o,$(SRCS))
LJSON_LIB  = $(LJSON)/libljson.a

CFLAGS    += -Wall -Wextra -Werror -I$(LJSON)/inc -I$(INC) -g

OUT        = modutil

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJS) $(LJSON_LIB)
	@echo -e "\033[33m  \033[1mLinking sources\033[0m"
	${Q} $(HOST_CC) -o $(OUT) $(OBJS) $(LJSON_LIB)

$(LJSON_LIB):
	@cd $(LJSON); $(MAKE) CC="$(HOST_CC)" AR="$(HOST_AR)"

# gcc:
%.o: %.c
	@echo -e "\033[32m  \033[1mCC\033[21m    \033[34m$<\033[0m"
	${Q} $(HOST_CC) $(CFLAGS) -c -o $@ $<


clean:
	${Q} rm -f $(OBJS) $(OUT)
