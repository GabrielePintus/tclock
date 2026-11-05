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
	@test "$$(uname -s)-$$(uname -m)" = Linux-x86_64 || \
		{ echo 'Binary packaging currently supports Linux x86_64 only.' >&2; exit 1; }
	mkdir -p dist/tclock-$(VERSION)-linux-x86_64
	cp tclock dist/tclock-linux-x86_64
	cp tclock README.md LICENSE dist/tclock-$(VERSION)-linux-x86_64/
	cp -R assets dist/tclock-$(VERSION)-linux-x86_64/
	printf '%s\n' 'tclock $(VERSION) — Linux x86_64' \
		'Requires glibc 2.35 or newer.' \
		'Install: install -Dm755 tclock "$$HOME/.local/bin/tclock"' \
		> dist/tclock-$(VERSION)-linux-x86_64/INSTALL.txt
	tar -czf dist/tclock-$(VERSION)-linux-x86_64.tar.gz -C dist tclock-$(VERSION)-linux-x86_64
	cd dist && sha256sum tclock-linux-x86_64 tclock-$(VERSION)-linux-x86_64.tar.gz > SHA256SUMS

clean:
	rm -f tclock
