"""PSX-Farbschluessel in vorhandenen Masken-Atlanten nachziehen: CLUT-Eintrag 0x0000 -> 0x8000.

⛔ WARUM (pri-masken-audit.md §2/§4.7): die GPU schluesselt den aufgeloesten TEXEL-WERT, nicht
den Palettenindex — psx-spx graphicsprocessingunitgpu.md Z.1128-1131 ("Color 0000h =
Fully-transparent", "0001h..7FFFh = Non-transparent") und Z.1167-1176 (fuer OPAKE Kommandos ist
"8000h = Non-Transparent Black"). Die Maske ist ein SPRT mit Code 0x64, also opak und ohne ABE
(@0x80039498 setzt CLUT 0x7800; SetSprt 0x8006b704 schreibt Code 0x64). Ein schwarzer
Palettenplatz, der als 0x0000 im CLUT steht, waere auf der PSX ein LOCH in der Maske —
gemessen 12556 Texel allein in ROOM1000 C7.

Der Eingriff ist eine reine Datenkorrektur an der CLUT: Index 0 bleibt 0x0000 (die
Transparenz), jeder ANDERE Eintrag mit Wert 0x0000 bekommt das STP-Bit. Die Bildpunkte
bleiben unveraendert, der PC-Zeichner schluesselt weiter den Index 0 (bg_pc.c
pri_publish_tim) und maskiert Bit 15 beim Auspacken ohnehin weg.

Neu erzeugte Atlanten bekommen das schon beim Bauen (atlas.clut_aus); dieses Werkzeug zieht
die BESTEHENDEN Dateien nach.

Aufruf: python re15_port/tools/maske/clut_schluessel.py [verzeichnis]
"""
import glob
import os
import struct
import sys

STD = "re15_port/shared_assets/PSX/MASKS"


def patch(pfad):
    b = bytearray(open(pfad, "rb").read())
    if len(b) < 20 or struct.unpack_from("<I", b, 0)[0] != 0x10:
        return None
    flag = struct.unpack_from("<I", b, 4)[0]
    if not (flag & 8):
        return None
    blen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", b, 8)
    n = cw * ch
    if 20 + 2 * n > len(b):
        return None
    geaendert = 0
    for i in range(1, n):                    # Index 0 bleibt 0x0000
        o = 20 + 2 * i
        if struct.unpack_from("<H", b, o)[0] == 0:
            struct.pack_into("<H", b, o, 0x8000)
            geaendert += 1
    if geaendert:
        open(pfad, "wb").write(bytes(b))
    return geaendert


def main():
    d = sys.argv[1] if len(sys.argv) > 1 else STD
    ges = 0
    dateien = 0
    for p in sorted(glob.glob(os.path.join(d, "*.TIM"))):
        r = patch(p)
        if r is None:
            print("  uebersprungen (kein 8-bpp-TIM mit CLUT): %s" % os.path.basename(p))
            continue
        dateien += 1
        if r:
            ges += r
            print("  %s: %d Eintraege 0x0000 -> 0x8000" % (os.path.basename(p), r))
    print("%d Atlanten geprueft, %d Eintraege geschluesselt" % (dateien, ges))
    return 0


if __name__ == "__main__":
    sys.exit(main())
