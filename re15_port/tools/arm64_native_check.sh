#!/usr/bin/env bash
# =============================================================================
# arm64_native_check.sh — laeuft IN einem linux/arm64-Container (echtes aarch64).
#
#   MSYS_NO_PATHCONV=1 docker run --rm --platform linux/arm64 \
#       -v "/c/workspace/git/reAi_v2":/src debian:12 \
#       bash /src/re15_port/tools/arm64_native_check.sh
#
# Was gemessen wird (nicht modelliert):
#   1. Die ABI-Fakten des Ziels: sizeof(long)/(void*), Vorzeichen von `char`,
#      Schiebeverhalten negativer Werte.
#   2. Die Engine-Bibliothek nativ fuer aarch64 uebersetzen (-Wcast-align=strict).
#   3. align_probe LAUFEN LASSEN: die echten Parser ueber die echten Assets,
#      Ausrichtung jedes herausgereichten Zeigers.
#   4. So viele Unit-/Property-Tests wie moeglich nativ bauen UND ausfuehren.
# =============================================================================
set -uo pipefail
export DEBIAN_FRONTEND=noninteractive

echo "### uname: $(uname -m)   ###"
apt-get update -qq >/dev/null 2>&1
apt-get install -y -qq --no-install-recommends build-essential >/dev/null 2>&1 \
    || { echo "APT-FAIL"; exit 1; }
gcc --version | head -1

SRC=/src/re15_port
OUT=/tmp/arm64
mkdir -p "$OUT/obj" "$OUT/bin"

echo
echo "=== 1) ABI-Fakten des Ziels ================================================"
cat > /tmp/abi.c <<'CEOF'
#include <stdio.h>
#include <stdint.h>
int main(void){
    char c = (char)-1;
    int  neg = -4097;
    long lneg = -4097L;
    printf("sizeof(long)=%zu sizeof(void*)=%zu sizeof(int)=%zu\n",
           sizeof(long), sizeof(void*), sizeof(int));
    printf("plain char signed? %s   ((char)-1 < 0) = %d\n",
           (c < 0) ? "JA" : "NEIN (unsigned - ARM-Vorgabe)", (int)(c < 0));
#ifdef __CHAR_UNSIGNED__
    printf("__CHAR_UNSIGNED__ ist definiert\n");
#else
    printf("__CHAR_UNSIGNED__ ist NICHT definiert\n");
#endif
    printf("(-4097) >> 12 = %d   (arithmetisch waere -2)\n", neg >> 12);
    printf("(-4097L)>> 12 = %ld  (arithmetisch waere -2)\n", lneg >> 12);
    printf("Byte-Reihenfolge: ");
    { uint32_t v = 0x01020304u; unsigned char *p = (unsigned char*)&v;
      printf("%02X %02X %02X %02X  -> %s\n", p[0],p[1],p[2],p[3],
             (p[0]==4) ? "LITTLE-ENDIAN" : "BIG-ENDIAN"); }
    return 0;
}
CEOF
gcc -O2 /tmp/abi.c -o /tmp/abi && /tmp/abi

echo
echo "=== 2) Engine nativ fuer aarch64 uebersetzen (-Wcast-align=strict) ========="
: > /tmp/arm_warn.txt
rc=0
for f in "$SRC"/engine/src/*.c; do
  gcc -std=c11 -O2 -I"$SRC/include" -DRE15_PLATFORM_PC=1 \
      "-DRE15_ASSETS_PATH=$SRC/shared_assets/PSX" \
      -Wall -Wextra -Wcast-align=strict \
      -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-missing-field-initializers \
      -c "$f" -o "$OUT/obj/$(basename "$f" .c).o" 2>>/tmp/arm_warn.txt || rc=1
done
echo "engine-Objekte: $(ls "$OUT/obj" | wc -l)   compile-rc=$rc"
echo "Fehler:          $(grep -c 'error:' /tmp/arm_warn.txt)"
grep 'error:' /tmp/arm_warn.txt | head -10
echo "cast-align:      $(grep -c 'cast increases required alignment' /tmp/arm_warn.txt)"
grep 'cast increases required alignment' /tmp/arm_warn.txt | sed "s#$SRC/##"
echo "sonstige Warnungen (ohne cast-align): $(grep -c 'warning:' /tmp/arm_warn.txt)"
grep 'warning:' /tmp/arm_warn.txt | grep -v 'cast-align' | sed "s#$SRC/##" | sort | uniq -c | sort -rn | head -20
ar rcs "$OUT/libre15_engine.a" "$OUT"/obj/*.o

echo
echo "=== 3) test_support + align_probe nativ bauen und LAUFEN LASSEN ============"
gcc -std=c11 -O2 -I"$SRC/include" -DRE15_PLATFORM_PC=1 \
    "-DRE15_ASSET_PSX_DIR=\"$SRC/shared_assets/PSX\"" \
    -c "$SRC/tests/test_support.c" -o "$OUT/test_support.o" 2>&1 | head -5
gcc -std=c11 -O2 -I"$SRC/include" -DRE15_PLATFORM_PC=1 \
    -c "$SRC/platform/pc/src/skeleton_trig_pc.c" -o "$OUT/trig.o" 2>&1 | head -5
ar rcs "$OUT/libre15_test_support.a" "$OUT/test_support.o" "$OUT/trig.o"

gcc -std=c11 -O2 -I"$SRC/include" -DRE15_PLATFORM_PC=1 \
    "$SRC/tools/align_probe/align_probe.c" \
    "$OUT/libre15_engine.a" "$OUT/libre15_test_support.a" -lm -o "$OUT/align_probe" 2>&1 | head -10
if [ -x "$OUT/align_probe" ]; then
    ( cd "$SRC" && "$OUT/align_probe" shared_assets )
    echo "align_probe exit=$?"
else
    echo "align_probe konnte nicht gebaut werden"
fi

echo
echo "=== 4) Unit-/Property-Tests nativ bauen und ausfuehren ====================="
built=0; failed_build=0; passed=0; failed=0
: > /tmp/arm_testfail.txt
for t in "$SRC"/tests/unit/*.c "$SRC"/tests/property/*.c "$SRC"/tests/integration/*.c; do
  n="$(basename "$t" .c)"
  if gcc -std=c11 -O2 -I"$SRC/include" -I"$SRC/platform/pc/src" -DRE15_PLATFORM_PC=1 \
        "-DRE15_ASSET_PSX_DIR=\"$SRC/shared_assets/PSX\"" \
        "$t" "$OUT/libre15_engine.a" "$OUT/libre15_test_support.a" -lm \
        -o "$OUT/bin/$n" 2>/dev/null; then
      built=$((built+1))
      if ( cd "$SRC" && timeout 60 "$OUT/bin/$n" >/tmp/t.out 2>&1 ); then
          passed=$((passed+1))
      else
          failed=$((failed+1))
          { echo "--- $n (exit $?)"; tail -5 /tmp/t.out; } >> /tmp/arm_testfail.txt
      fi
  else
      failed_build=$((failed_build+1))
      echo "$n" >> /tmp/arm_nolink.txt
  fi
done
echo "gebaut: $built   nicht linkbar (zusaetzliche Quellen noetig): $failed_build"
echo "AUSGEFUEHRT: $passed bestanden, $failed durchgefallen"
if [ "$failed" -gt 0 ]; then
    echo "--- durchgefallene Tests -------------------------------------------"
    head -120 /tmp/arm_testfail.txt
fi
echo "ARM64-NATIVE-CHECK-DONE"
