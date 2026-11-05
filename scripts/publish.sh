#!/bin/sh
# Run once from a machine with GitHub CLI authentication and network access.
set -eu
cd "$(dirname "$0")/.."
gh auth status
version=$(sed -n 's/^VERSION = //p' Makefile)
owner=$(gh api user --jq .login)
repo="$owner/tclock"
if git rev-parse --show-toplevel >/dev/null 2>&1; then
    echo 'This first-publication script expects a folder without a Git repository.' >&2
    exit 1
fi
if gh repo view "$repo" >/dev/null 2>&1; then
    echo "Repository $repo already exists; stopping to avoid changing it." >&2
    exit 1
fi
make
make dist
./tclock --help >/dev/null
git init -b main
git add tclock.c Makefile README.md LICENSE RELEASE_NOTES.md .gitignore scripts
git commit -m "Release tclock $version"
gh repo create "$repo" --public --source . --remote origin --push \
    --description 'A colorful, resizable terminal clock in C'
git tag "v$version"
git push origin "v$version"
gh release create "v$version" --repo "$repo" --verify-tag \
    --title "tclock $version" --notes-file RELEASE_NOTES.md \
    dist/tclock-linux-x86_64 "dist/tclock-$version-linux-x86_64.tar.gz" dist/SHA256SUMS
