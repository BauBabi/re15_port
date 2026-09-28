#!/usr/bin/env python
"""Runde 30 karten-marken (Fortsetzung): die Karten-CLUT-Zeilen BEIDER Spiele mit
Datei-Offset je Eintrag, und die Frage des Auftrags: unterscheiden sich die Zeilen in
{1,12,13,14}, und was steht in RE1.5s Zeile 21 in 12/13/14?
  RE1.5: re15_port/shared_assets/PSX/DATA/TEX.TIM, CLUT-Block x=256 y=480 w=32 h=24
         (Kopf selbst gelesen), Zeile 21 = VRAM-Y 501, 4bpp nutzt die ersten 16 Eintraege
  RE2:   info/re2leon/COMMON/DATA/ST0.TIM, zweites TIM @0x10820, CLUT x=256 y=480 w=16 h=21
         (Kopf selbst gelesen), Zeilen k=8/11/12 = CLUT-Y 498/501/502 laut Kommentar des Ports
         -> hier wird k aus dem Kopf und aus den bekannten Eintrag-1-Offsets nachgerechnet.
Aufruf: python r30_clut_zeilen.py"""
import struct

def rgb(w):
    return ((w & 31) << 3, ((w >> 5) & 31) << 3, ((w >> 10) & 31) << 3, w >> 15)

def tim_clut(d, base):
    magic, flags = struct.unpack_from('<II', d, base)
    assert magic == 0x10, hex(magic)
    assert flags & 8, 'kein CLUT'
    blen, x, y, w, h = struct.unpack_from('<IHHHH', d, base + 8)
    return base + 20, x, y, w, h, flags, blen

def zeile(d, name, off0, w, k, nur=None):
    o = off0 + 2 * w * k
    ws = struct.unpack_from('<16H', d, o)
    print("  %s Zeile k=%d (Zeilenindex; VRAM-Y je Spiel s. Dossier; 480+k=%d) @Datei 0x%05X:" % (name, k, 480 + k, o))
    for i, v in enumerate(ws):
        if nur is not None and i not in nur:
            continue
        r, g, b, stp = rgb(v)
        print("     Eintrag %2d @0x%05X = 0x%04X  RGB (%3d,%3d,%3d) STP %d" % (i, o + 2 * i, v, r, g, b, stp))
    return ws

def main():
    t = open('re15_port/shared_assets/PSX/DATA/TEX.TIM', 'rb').read()
    o, x, y, w, h, fl, bl = tim_clut(t, 0)
    print("RE1.5 TEX.TIM: flags 0x%X, CLUT-Block len 0x%X, x=%d y=%d w=%d h=%d, Daten @0x%X" % (fl, bl, x, y, w, h, o))
    r21 = zeile(t, 'RE1.5', o, w, 21)
    # Gegenprobe: dieselbe Zeile im Original-Baum
    t0 = open('info/Re1.5/PSX/DATA/TEX.TIM', 'rb').read()
    print("  info/Re1.5/PSX/DATA/TEX.TIM bytegleich zu shared_assets: %s" % (t0 == t))

    s = open('info/re2leon/COMMON/DATA/ST0.TIM', 'rb').read()
    o2, x2, y2, w2, h2, fl2, bl2 = tim_clut(s, 0x10820)
    print("\nRE2 ST0.TIM zweites TIM @0x10820: flags 0x%X, CLUT len 0x%X, x=%d y=%d w=%d h=%d, Daten @0x%X" % (fl2, bl2, x2, y2, w2, h2, o2))
    rows = {}
    for k in (8, 11, 12):
        rows[k] = zeile(s, 'RE2', o2, w2, k, nur=(1, 4, 12, 13, 14))
    print("\n  Unterschiede der drei RE2-Zeilen (Eintragsnummern):")
    for a, b in ((8, 11), (11, 12), (8, 12)):
        print("     k=%d gegen k=%d: %s" % (a, b, [i for i in range(16) if rows[a][i] != rows[b][i]]))
    print("\n  RE1.5 Zeile 21 gegen RE2 k=11 (besucht): abweichende Eintraege %s"
          % [i for i in range(16) if r21[i] != rows[11][i]])
    # Kommt 0xD902 (RE2-Blau) irgendwo in RE1.5s TEX.TIM-CLUT vor?
    n = 0
    for i in range(w * h):
        v = struct.unpack_from('<H', t, o + 2 * i)[0]
        if (v & 0x7FFF) == (0xD902 & 0x7FFF):
            n += 1
            print("  0x%04X in TEX.TIM-CLUT Zeile %d Eintrag %d @0x%05X" % (v, i // w, i % w, o + 2 * i))
    print("  RE2-Blau 0xD902 (ohne STP verglichen) in RE1.5s TEX.TIM-CLUT: %d Treffer" % n)

if __name__ == '__main__':
    main()
