First release of tclock: a colorful terminal clock written in C.

- Large digits centered automatically, with adjustable width and scale.
- Named colors and custom RGB hex colors.
- Optional date, seconds, quit hint, 12-hour time, and UTC.
- Updates digits in place and restores the terminal on exit.

Download `tclock-linux-x86_64` for a ready-to-run executable, or the `.tar.gz`
archive for the executable, documentation, and license. `SHA256SUMS` contains
checksums for both downloads.

The prebuilt executable targets **Linux x86_64 with glibc 2.35 or newer**.
For other systems, compile the source with `make`.

```sh
chmod +x tclock-linux-x86_64
install -Dm755 tclock-linux-x86_64 "$HOME/.local/bin/tclock"
tclock --color orange --no-date --no-hint
```
