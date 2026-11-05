CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
PREFIX ?= $(HOME)/.local
VERSION = 0.1.0

tclock: tclock.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) tclock.c -o $@ $(LDLIBS)

.PHONY: clean install uninstall dist
install: tclock
	install -d "$(DESTDIR)$(PREFIX)/bin"
	install -m 755 tclock "$(DESTDIR)$(PREFIX)/bin/tclock"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/tclock"

dist: tclock
	sh scripts/package.sh $(VERSION)

clean:
	rm -f tclock
