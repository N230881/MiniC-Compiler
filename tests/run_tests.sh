#!/usr/bin/env bash
# Runs every tests/cases/*.mc through ./minicompiler and compares the program
# output (plus any compiler diagnostics on stderr) with tests/expected/*.out.
# Usage: tests/run_tests.sh [--update]
set -u
cd "$(dirname "$0")/.."
COMPILER=./minicompiler
pass=0
fail=0

run_case() {
    local src=$1 stdout stderr
    stderr=$(mktemp)
    stdout=$("$COMPILER" "$src" 2>"$stderr")
    printf '%s\n' "$stdout" | sed -n '/^===OUTPUT===$/,/^===END===$/{/^===/d;p}'
    cat "$stderr"
    rm -f "$stderr"
}

for src in tests/cases/*.mc; do
    name=$(basename "$src" .mc)
    expected=tests/expected/$name.out
    actual=$(run_case "$src")
    if [ "${1:-}" = "--update" ]; then
        printf '%s\n' "$actual" > "$expected"
        echo "updated $expected"
        continue
    fi
    if [ "$actual" = "$(cat "$expected" 2>/dev/null)" ]; then
        echo "PASS  $name"
        pass=$((pass + 1))
    else
        echo "FAIL  $name"
        diff <(cat "$expected" 2>/dev/null) <(printf '%s\n' "$actual") | sed 's/^/      /'
        fail=$((fail + 1))
    fi
done

[ "${1:-}" = "--update" ] && exit 0
echo
echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
