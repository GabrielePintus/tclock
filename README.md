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

## Install a prebuilt release

The [latest release](https://github.com/GabrielePintus/tclock/releases/latest)
provides two ready-to-run Linux binaries. No compiler is needed.

| Architecture (`uname -m`) | Download |
| --- | --- |
| `x86_64` — Intel/AMD 64-bit PCs | [tclock-linux-x86_64](https://github.com/GabrielePintus/tclock/releases/latest/download/tclock-linux-x86_64) |
| `aarch64` / `arm64` — ARM64, including Raspberry Pi with a 64-bit OS | [tclock-linux-arm64](https://github.com/GabrielePintus/tclock/releases/latest/download/tclock-linux-arm64) |

Starting with v0.1.1, both binaries are statically linked with musl: they have
no glibc dependency and can also run on Alpine Linux. Version 0.1.0's x86_64
binary requires glibc 2.35 or newer. For macOS or 32-bit Linux, build from source.

Download [tclock-linux-x86_64](https://github.com/GabrielePintus/tclock/releases/latest/download/tclock-linux-x86_64),
then install it:

```sh
mkdir -p "$HOME/.local/bin"
install -m 755 tclock-linux-x86_64 "$HOME/.local/bin/tclock"
tclock --color orange --no-date --no-hint
```

For ARM64, replace `tclock-linux-x86_64` with `tclock-linux-arm64` in the commands.
Run these commands in the directory containing the downloaded file. Ensure
`~/.local/bin` is in your `PATH`; otherwise run `~/.local/bin/tclock` directly.
You can also try the downloaded file without installing it:

```sh
chmod +x tclock-linux-x86_64
./tclock-linux-x86_64
```

The release also includes a `.tar.gz` archive with the same executable,
documentation, and license. Extract it, enter the extracted directory, and run
`./tclock` or install it with `install -m 755 tclock "$HOME/.local/bin/tclock"`.
`SHA256SUMS` lists checksums for both downloads. To check downloaded assets on Linux,
place that file beside them and run `sha256sum --check --ignore-missing SHA256SUMS`.

## Build and install from source

Requires a C compiler and Make on Linux or macOS. No additional libraries are
needed.

```sh
git clone https://github.com/GabrielePintus/tclock.git
cd tclock
make
./tclock --color orange --no-date --no-hint
```

Install to `~/.local/bin` so you can use `tclock` from any directory:

```sh
make install
tclock --help
```

Ensure `~/.local/bin` is in your `PATH`. To select another installation prefix,
use `make install PREFIX=/your/prefix`. To remove a source installation, run
`make uninstall` with the same prefix. For a manually installed release binary,
remove `~/.local/bin/tclock`.

Without Make, compile directly:

```sh
cc -std=c11 -O2 -Wall -Wextra -Wpedantic tclock.c -o tclock
```

<details>
<summary>Creating a release</summary>

Install [Zig](https://ziglang.org/download/) (tested with 0.17.1) to build release
binaries. Zig is only needed for cross-compilation; normal `make` and
`make install` still use your system C compiler.

```sh
make dist
# Or specify the path to Zig:
make dist ZIG=/path/to/zig
```

This cross-compiles static musl binaries for Linux x86_64 and ARM64 from the
same source. It leaves the local `./tclock` executable unchanged. The `dist/`
directory contains both binaries, an archive per architecture, and `SHA256SUMS`.
The ARM64 executable requires an ARM64 machine or emulator to run.

To publish a new version, update `VERSION` in the Makefile and commit it.
Push the matching tag (for example, `git tag v0.1.1` followed by
`git push origin v0.1.1`). The Linux release workflow builds both architectures,
checks them (ARM64 through QEMU), and publishes the four assets and `SHA256SUMS`
to a GitHub release automatically.
You can also run **Linux release → Run workflow** from the repository's
**Actions** tab and enter an existing tag. GitHub Actions must be enabled for
the repository.

</details>

## License

[MIT](LICENSE) © Gabriele Pintus.
