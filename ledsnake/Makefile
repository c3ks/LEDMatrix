# ledsnake
# (c) 2007 Enno Boland

include config.mk

TARGET = ledsnake
SRC = ledsnake.c
OBJ = ${SRC:.c=.o}

all: options ${TARGET}

options:
	@echo ledsnake build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

${TARGET}: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ $@.c

clean:
	@echo cleaning
	@rm -f -- ${TARGET} ${OBJ}

.PHONY: all options clean
