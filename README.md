# tclock

A small, colorful terminal clock written in C. Large digits, adjustable size,
custom colors, and a layout that stays centered as you resize your terminal.

![tclock running in a terminal with orange digits](assets/tclock.gif)

## Usage

```sh
tclock
tclock --color orange --no-date --no-hint
tclock --size medium --12-hour --no-seconds
tclock --width 6 --scale 3 --color '#89B4FA'
```

The preview uses `tclock --color orange --no-date --no-hint`.
Press **q**, **Esc**, or **Ctrl+C** to quit and restore your terminal.
Use your terminal application's fullscreen shortcut for a fullscreen clock.

### Options

| Option | What it does |
| --- | --- |
| `--size small\|medium\|large` | Set a size preset; default: `large`. |
| `--width N` | Columns per font pixel, from 1 to 40; default: 6. |
| `--scale N` | Rows per font pixel, from 1 to 20; default: 3. |
| `--color COLOR` | A named or hex color; default: `cyan`. |
| `--no-seconds` | Show only hours and minutes. |
| `--no-date` | Hide the date. |
| `--no-hint` | Hide the quit hint; quit keys still work. |
| `--12-hour` | Use 12-hour time with an AM/PM label. |
| `--utc` | Show UTC instead of local time. |
| `-h`, `--help` | Show all options. |

Sizes shrink automatically to fit. The `small`, `medium`, and `large` presets
set width/scale to 2/1, 4/2, and 6/3. Options apply left to right, so place
individual size overrides after a preset.

### Colors

Choose `cyan`, `blue`, `green`, `yellow`, `red`, `magenta`, `white`, `orange`,
or `grey` (`gray` also works). Custom colors accept six hexadecimal digits,
with an optional `#`:

```sh
tclock --color orange
tclock --color '#FF8800'
tclock --color 89B4FA
```

Quote values starting with `#` so your shell does not treat them as comments.
Orange, grey, and custom colors require a terminal with true-color support.

## Installation

### Prebuilt binary · Linux

Download [x86_64](https://github.com/GabrielePintus/tclock/releases/latest/download/tclock-linux-x86_64)
or [ARM64](https://github.com/GabrielePintus/tclock/releases/latest/download/tclock-linux-arm64)
from [Releases](https://github.com/GabrielePintus/tclock/releases).
Then, **from the download directory**, run:

```sh
mkdir -p "$HOME/.local/bin"
install -m 755 tclock-linux-x86_64 "$HOME/.local/bin/tclock"
```

Both binaries are statically linked with musl, with no glibc dependency,
and support Alpine Linux. For ARM64, use `tclock-linux-arm64` in the command above.

Archives include the same binary as `tclock`, plus documentation and a license.
Checksums are provided in `SHA256SUMS`.

### From source · Linux / macOS

Requires a C compiler and Make; no additional libraries.

```sh
git clone https://github.com/GabrielePintus/tclock.git
cd tclock
make install
```

Already in the project directory? Just run `make install`. To build and try it
without installing, use `make && ./tclock`.

Both methods install to `~/.local/bin`. Ensure it is in your `PATH`, or run
`~/.local/bin/tclock` directly. Remove the installed binary to uninstall;
source builds also provide `make uninstall` and a custom `PREFIX=/your/prefix`.

<details>
<summary>Cross-compilation and releases</summary>

With [Zig](https://ziglang.org/download/) installed (tested with 0.17.1),
`make dist` builds static Linux x86_64 and ARM64 binaries, archives, and
`SHA256SUMS` in `dist/`. Use `ZIG=/path/to/zig` to select the compiler.
Normal builds use the system C compiler and do not require Zig.

To publish, update `VERSION`, commit, and push the matching `vX.Y.Z` tag.
GitHub Actions builds, checks, and publishes the assets. You can also select
**Actions → Linux release → Run workflow** with an existing tag.

</details>

## License

[MIT](LICENSE) © Gabriele Pintus.
