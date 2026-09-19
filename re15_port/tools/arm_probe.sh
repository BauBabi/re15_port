#!/usr/bin/env bash
set -uo pipefail
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq >/dev/null 2>&1
apt-get install -y -qq --no-install-recommends gcc-aarch64-linux-gnu gcc >/dev/null 2>&1 || { echo "APT-FAIL"; exit 1; }
aarch64-linux-gnu-gcc --version | head -1
echo "=== A) plain char default signedness on aarch64 ==="
cat > /tmp/c.c <<'EOF'
int f(void){ char c = (char)0xFF; return c; }
EOF
aarch64-linux-gnu-gcc -O2 -S /tmp/c.c -o /tmp/c.s && grep -E 'mov|ret' /tmp/c.s | head -5
echo "--- __CHAR_UNSIGNED__ defined? ---"
aarch64-linux-gnu-gcc -dM -E - </dev/null | grep -i CHAR_UNSIGNED || echo "(not defined on this aarch64 target)"
gcc -dM -E - </dev/null | grep -i CHAR_UNSIGNED || echo "(x86 host: not defined = signed char)"
echo "=== B) arithmetic right shift of negative value ==="
cat > /tmp/s.c <<'EOF'
int g(int x){ return x >> 12; }
long gl(long x){ return x >> 12; }
EOF
aarch64-linux-gnu-gcc -O2 -S /tmp/s.c -o /tmp/s.s && grep -E 'asr|lsr' /tmp/s.s
echo "=== C) sizeof on aarch64-linux ==="
cat > /tmp/z.c <<'EOF'
#include <stdio.h>
int main(void){ printf("long=%zu ptr=%zu int=%zu char_is_signed=%d\n", sizeof(long), sizeof(void*), sizeof(int), (int)((char)-1 < 0)); return 0; }
EOF
aarch64-linux-gnu-gcc -O2 /tmp/z.c -o /tmp/z 2>&1 | head -3
echo "(static compile only; no qemu run)"
echo "=== D) engine sources for aarch64, -Wcast-align=strict ==="
cd /src/re15_port
for f in engine/src/*.c; do
  aarch64-linux-gnu-gcc -std=c11 -Iinclude -DRE15_PLATFORM_PC=1 "-DRE15_ASSETS_PATH=/src/re15_port/shared_assets/PSX" \
    -Wall -Wextra -Wcast-align=strict -fsyntax-only "$f" 2>&1
done > /tmp/arm_engine.txt
echo "cast-align hits: $(grep -c 'cast increases required alignment' /tmp/arm_engine.txt)"
grep 'cast increases required alignment' /tmp/arm_engine.txt
echo "errors: $(grep -c 'error:' /tmp/arm_engine.txt)"
grep 'error:' /tmp/arm_engine.txt | head -10
echo "=== E) full compile (objects) for aarch64 to prove it builds ==="
mkdir -p /tmp/aobj
rc=0
for f in engine/src/*.c; do
  aarch64-linux-gnu-gcc -std=c11 -O2 -Iinclude -DRE15_PLATFORM_PC=1 "-DRE15_ASSETS_PATH=/src/re15_port/shared_assets/PSX" \
    -c "$f" -o "/tmp/aobj/$(basename $f .c).o" 2>>/tmp/arm_build_err.txt || rc=1
done
echo "engine aarch64 object build rc=$rc  objects=$(ls /tmp/aobj | wc -l)"
head -30 /tmp/arm_build_err.txt 2>/dev/null
echo "=== F) char-signedness sensitive warnings on aarch64 ==="
grep -E 'comparison is always|-Wtype-limits|-Wchar-subscripts|-Wsign-compare' /tmp/arm_engine.txt | head -30
echo "ARM-PROBE-DONE"
