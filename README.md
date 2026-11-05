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
provides a ready-to-run binary for **Linux x86_64 with glibc 2.35 or newer**.
No compiler is needed. For macOS, ARM, or Alpine Linux, build from source below.

Download [tclock-linux-x86_64](https://github.com/GabrielePintus/tclock/releases/latest/download/tclock-linux-x86_64),
then install it:

```sh
mkdir -p "$HOME/.local/bin"
install -m 755 tclock-linux-x86_64 "$HOME/.local/bin/tclock"
tclock --color orange --no-date --no-hint
```

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

Update `VERSION` in the Makefile and run `make dist` on Linux x86_64.
The `dist/` directory will contain the executable, archive, and checksums.
Publish those files as assets of a GitHub release with the corresponding version tag.

Build against glibc 2.35 or older to retain the documented compatibility baseline,
and check the resulting executable's glibc requirements before publishing.
Building on a newer system can introduce newer glibc requirements.

</details>

## License

[MIT](LICENSE) © Gabriele Pintus.
