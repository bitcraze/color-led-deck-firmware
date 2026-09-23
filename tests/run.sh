#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
test_binary=$(mktemp /tmp/color-led-protocol-test.XXXXXX)
trap 'rm -f "$test_binary"' EXIT HUP INT TERM
${CC:-cc} -std=c11 -Wall -Wextra -Werror -g \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Itests/stubs -ICore/Inc \
  Core/Src/i2c_protocol.c tests/test_i2c_protocol.c -o "$test_binary"
"$test_binary"
