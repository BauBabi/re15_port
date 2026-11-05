# NB6: RE2-Rumble-Ringe im RAM (Ring A @0x800EAAD8, Ring B @0x800EAC28, je 32 Plaetze a 10 Byte: +0 belegt, +1 Wert, +2 Vorlauf,
# +4 Rest, +6 Schritt, +8 Akku; re15_rumble.h) und die Skript-Tasks (0x800d7860 + i*0x174, +1 aktiv, +0x1c pc).
import sys, struct
sys.path.insert(0, ".")
from re2ram4 import Ram, SN
for n in [int(a) for a in sys.argv[1].split(",")]:
    r = Ram(SN % n)
    print("== s%02d" % n)
    for name, base in (("A", 0x800EAAD8), ("B", 0x800EAC28)):
        for i in range(32):
            o = base + 10 * i
            if r.u8(o):
                print("  Ring %s Platz %2d: Wert %3d Vorlauf %3d Rest %4d Schritt %5d Akku %5d" % (name, i, r.u8(o + 1), r.u8(o + 2) | (r.u8(o + 3) << 8),
                      r.s16(o + 4), r.s16(o + 6), r.u8(o + 8) | (r.u8(o + 9) << 8)))
    rdt = r.u32(0x800ce324)
    for i in range(10):
        t = 0x800d7860 + i * 0x174
        if r.u8(t + 1):
            pc = r.u32(t + 0x1c)
            print("  Task %d aktiv, pc %08x" % (i, pc))
    print("  Aktuatoren 0x800CBC20: %d %d" % (r.u8(0x800CBC20), r.u8(0x800CBC21)))
