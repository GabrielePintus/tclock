CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic
PREFIX ?= $(HOME)/.local
VERSION = 0.1.1
ZIG ?= zig
RELEASE_CFLAGS = -std=c11 -Os -Wall -Wextra -Wpedantic -static -s
LINUX_BINARIES = dist/tclock-linux-x86_64 dist/tclock-linux-arm64

tclock: tclock.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) tclock.c -o $@ $(LDLIBS)

.PHONY: clean install uninstall dist dist-linux
install: tclock
	install -d "$(DESTDIR)$(PREFIX)/bin"
	install -m 755 tclock "$(DESTDIR)$(PREFIX)/bin/tclock"

uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/bin/tclock"

dist/tclock-linux-x86_64: tclock.c Makefile
	mkdir -p dist
	$(ZIG) cc $(RELEASE_CFLAGS) -target x86_64-linux-musl -mcpu=baseline tclock.c -o $@

dist/tclock-linux-arm64: tclock.c Makefile
	mkdir -p dist
	$(ZIG) cc $(RELEASE_CFLAGS) -target aarch64-linux-musl -mcpu=baseline tclock.c -o $@

dist: dist-linux

dist-linux: $(LINUX_BINARIES)
	@set -eu; for arch in x86_64 arm64; do \
		name="tclock-$(VERSION)-linux-$$arch"; \
		mkdir -p "dist/$$name"; \
		cp "dist/tclock-linux-$$arch" "dist/$$name/tclock"; \
		cp README.md LICENSE "dist/$$name/"; \
		cp -R assets "dist/$$name/"; \
		printf '%s\n' "tclock $(VERSION) — Linux $$arch" \
			'Statically linked with musl; no glibc dependency.' \
			'Install: install -Dm755 tclock "$$HOME/.local/bin/tclock"' \
			> "dist/$$name/INSTALL.txt"; \
		tar -czf "dist/$$name.tar.gz" -C dist "$$name"; \
	done
	cd dist && sha256sum tclock-linux-x86_64 tclock-linux-arm64 \
		tclock-$(VERSION)-linux-x86_64.tar.gz tclock-$(VERSION)-linux-arm64.tar.gz > SHA256SUMS

clean:
	rm -f tclock $(LINUX_BINARIES)
