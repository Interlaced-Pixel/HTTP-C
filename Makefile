CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -g
LDFLAGS ?=

# Sources and objects (support files in subdirectories)
SRCS := $(wildcard *.c */*.c)
OBJS := $(SRCS:.c=.o)

# Binaries to build. If you add new top-level programs, list them here.
BINS := example

all: $(BINS)

# Default example target: link example from its object(s)
example: example.o $(filter-out example.o,$(OBJS))
	$(CC) $(LDFLAGS) -o $@ $^

# Pattern rule: compile C sources to same relative .o paths
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BINS)

distclean: clean
	rm -f tags

install: all
	install -d $(DESTDIR)/usr/local/bin
	install -m 0755 $(BINS) $(DESTDIR)/usr/local/bin

test:
	@if [ -x ./unit_tests ]; then ./unit_tests; else echo "unit_tests binary not found"; exit 1; fi

help:
	@printf "Usage:\n  make [all|example|clean|distclean|install|test|help]\n"

.PHONY: all clean distclean install test help
