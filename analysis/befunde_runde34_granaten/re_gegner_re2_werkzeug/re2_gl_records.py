#!/usr/bin/env python3
"""re2_gl_records.py - Runde 34 (Granaten), Thema re_gegner_re2_familie.

Dump der RE2-Schadensrecords fuer die GL-Attacken-Ids 9 (Explosiv) / 10 (Brand) / 11 (Saeure)
je Gegnertyp, direkt aus info/re2leon/PSX.EXE (EXE-Abbildung 0x800 + addr - t_addr, t_addr aus
dem PS-X-Kopf @0x18 -- derselbe load() wie re2_disasm.py).

Beleg der Lesart (selbst disassembliert, FUN_800470C0 = Projektil-Applier):
  80047218  lbu v0,8(s1)          ; Typ
  8004722c  lw  a1,27272(at)      ; a1 = PTR_DAT_800a6a88[Typ]
  80047230..40  a1 += (id*5*4) - 20  ; Record = Basis + (id-1)*20
  80047244..54  v0 = s6*10 ; lw v1,0(a1) ; srlv v1,v1,v0   ; w0 >> (Klammer*10)
  8004725c  andi v1,v1,0x3ff      ; Schaden 10 Bit
  80047338..4c  lw v0,4(a1) ; srl v0,v0,9 ; andi v0,v0,0x7f ; or a0 ; sb a0,467(s1)
                                   ; +0x1D3 = (+0x1D3 & 0x80) | ((w1>>9) & 0x7f)
Aufruf: python re2_gl_records.py [repo-root]
"""
import struct, sys, os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = sys.argv[1] if len(sys.argv) > 1 else os.path.abspath(os.path.join(HERE, "..", "..", ".."))
exe = open(os.path.join(REPO, "info", "re2leon", "PSX.EXE"), "rb").read()
t_addr = struct.unpack_from("<I", exe, 0x18)[0]


def off(a):
    return a - t_addr + 0x800


def u32(a):
    return struct.unpack_from("<I", exe, off(a))[0]


def s16(a):
    return struct.unpack_from("<h", exe, off(a))[0]


print("t_addr=%08x" % t_addr)
print("--- PTR_DAT_800a6a88[typ] (Schadensrecord-Basis je Gegnertyp)")
bases = {}
for typ in range(0x00, 0x40):
    p = 0x800A6A88 + typ * 4
    b = u32(p)
    bases[typ] = b
    print(" typ %02X @%08X -> %08X" % (typ, p, b))

print()
print("--- Records Id 9/10/11 je distinkter Basis (+ Id 1..19 kompakt)")
seen = {}
for typ in range(0x00, 0x40):
    b = bases[typ]
    if b < t_addr or b >= 0x80100000:
        continue
    if b in seen:
        seen[b].append(typ)
        continue
    seen[b] = [typ]
for b, typs in seen.items():
    print("Basis %08X  (Typen %s)" % (b, " ".join("%02X" % t for t in typs)))
    for i in range(1, 20):
        a = b + (i - 1) * 20
        w0 = u32(a)
        w1 = u32(a + 4)
        mark = " <== GL" if i in (9, 10, 11) else ""
        print("  id%2d @%08X w0=%08X dmg=%d/%d/%d  w1=%08X poise=%d/%d/%d lock(w1>>9&7f)=%d (>>16)=%d (>>23)=%d"
              "  UP[%d,%d] LV[%d,%d] DN[%d,%d]%s" % (
                  i, a, w0, w0 & 0x3ff, (w0 >> 10) & 0x3ff, (w0 >> 20) & 0x3ff, w1,
                  w1 & 7, (w1 >> 3) & 7, (w1 >> 6) & 7, (w1 >> 9) & 0x7f, (w1 >> 16) & 0x7f,
                  (w1 >> 23) & 0x7f,
                  s16(a + 8), s16(a + 10), s16(a + 12), s16(a + 14), s16(a + 16), s16(a + 18), mark))
    # Rohbytes der drei GL-Zeilen
    for i in (9, 10, 11):
        a = b + (i - 1) * 20
        raw = exe[off(a):off(a) + 20]
        print("    raw id%2d @%08X: %s" % (i, a, raw.hex(" ")))
