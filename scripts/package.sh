#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
version=${1:?Specify a version}
case "$version" in *[!0-9.]*|'') echo 'Invalid version' >&2; exit 1;; esac
case "$(uname -s)-$(uname -m)" in
    Linux-x86_64) platform=linux-x86_64 ;;
    *) echo 'Binary packaging currently supports Linux x86_64 only.' >&2; exit 1 ;;
esac
name="tclock-$version-$platform"
mkdir -p "dist/$name"
cp tclock "dist/tclock-$platform"
cp tclock README.md LICENSE "dist/$name/"
printf '%s\n' "tclock $version — Linux x86_64" \
    'Requires glibc 2.35 or newer. Not an Alpine/musl or macOS binary.' \
    'Install: install -Dm755 tclock "$HOME/.local/bin/tclock"' > "dist/$name/INSTALL.txt"
tar -czf "dist/$name.tar.gz" -C dist "$name"
(
    cd dist
    sha256sum "tclock-$platform" "$name.tar.gz" > SHA256SUMS
)
printf 'Release assets prepared in dist/\n'
