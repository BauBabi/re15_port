"""Abnahme 1 Spur G2 (Runde 34 Nacht) — UNABHAENGIGER Auswerter: Port-Bilder gegen den
ORIGINAL-Bildspeicher (DuckStation-Staende), ohne das Log des Ports als Orakel.

Referenzen (nur Lesen): zwei saubere Original-Staende ROOM1150 Cut 2 aus der Ermittlung
(Dossier G2 §3.5): AN = Records Gruppen 6..11 Byte0 = 1 (c2_w12.7.sav), AUS = Byte0 = 0
(c2_w12.sav). Aus beiden wird das Schrift-Rechteck x139..216 y25..40 (320x240) in 15 Bit gelesen
(VRAM-Puffer y=0; Stub-Pruefung @0x80026e4c == 0x03e00008). "Schriftpunkte" = Punkte, an denen
Original-AN und Original-AUS um mehr als 2 Stufen (5 Bit) auseinanderliegen.

Je Port-Bild (RE15_FRAMEDUMP, Fenster-Skala k = Breite/320, Mittelpunkt je k*k-Block, >> 3):
  AN   = alle Schriftpunkte bitgleich (5 Bit) zum Original-AN
  AUS  = alle Schriftpunkte hoechstens 2 Stufen vom Original-AUS (BSS-Dekodierung Port vs PSX)
  sonst "?" (anderer Cut / Blende / Menue / Mischzustand)
Ausgabe: Zustand je Bild und die Laeufe gleichen Zustands mit Laenge.

    C:/Python310/python.exe re15_port/tools/r34n_g/abn1_blink_eval.py <an.sav> <aus.sav> <praefix> <von> <bis> [schritt]
"""
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

X0, Y0, X1, Y1 = 139, 25, 216, 40


def orig_box(path):
    r = re15_ss.Ram(path)
    stub = r.u32(0x80026e4c)
    if stub != 0x03e00008:
        raise SystemExit("%s: EXE gepatcht (%08x) — unbrauchbar" % (path, stub))
    if r.u16(0x800b0fe4) != 2:
        raise SystemExit("%s: nicht Cut 2" % path)
    vb = r.vram_base
    rows = []
    for y in range(Y0, Y1 + 1):
        o = vb + (y * 1024 + X0) * 2
        rows.append(np.frombuffer(r.blob[o:o + (X1 - X0 + 1) * 2], dtype="<u2"))
    a = np.stack(rows).astype(np.int32)
    return np.stack([a & 31, (a >> 5) & 31, (a >> 10) & 31], -1)


def port_box(path):
    with open(path, "rb") as f:
        data = f.read()
    # P6-Kopf: "P6\n<w> <h>\n255\n"
    parts = data.split(b"\n", 3)
    w, h = map(int, parts[1].split())
    px = np.frombuffer(parts[3][:w * h * 3], np.uint8).reshape(h, w, 3)
    k = w // 320
    ys = np.arange(Y0, Y1 + 1) * k + k // 2
    xs = np.arange(X0, X1 + 1) * k + k // 2
    return (px[np.ix_(ys, xs)].astype(np.int32) >> 3)


def main():
    if len(sys.argv) < 6:
        print(__doc__)
        return 2
    an, aus = orig_box(sys.argv[1]), orig_box(sys.argv[2])
    schrift = np.abs(an - aus).max(-1) > 2
    n = int(schrift.sum())
    print("Original: %d Schriftpunkte (AN vs AUS > 2 Stufen) im Rechteck x%d..%d y%d..%d"
          % (n, X0, X1, Y0, Y1))
    praefix, von, bis = sys.argv[3], int(sys.argv[4]), int(sys.argv[5])
    schritt = int(sys.argv[6]) if len(sys.argv) > 6 else 1
    zust = []
    for f in range(von, bis + 1, schritt):
        p = "%s%06d.ppm" % (praefix, f)
        if not os.path.exists(p):
            zust.append((f, "-", 0, 0))
            continue
        b = port_box(p)
        gl_an = int((np.all(b == an, -1) & schrift).sum())
        nah_aus = int(((np.abs(b - aus).max(-1) <= 2) & schrift).sum())
        z = "AN" if gl_an == n else ("AUS" if nah_aus == n else "?")
        zust.append((f, z, gl_an, nah_aus))
    for f, z, a, b in zust:
        print("F%06d %-3s  Schriftpunkte bitgleich Original-AN %3d/%d, <=2 Stufen Original-AUS %3d/%d"
              % (f, z, a, n, b, n))
    print("Laeufe:")
    start = None
    for i, (f, z, _, _) in enumerate(zust):
        if start is None:
            start = i
        if i + 1 == len(zust) or zust[i + 1][1] != z:
            print("  %-3s F%d..F%d  %d Bilder" % (z, zust[start][0], f, (i - start + 1) * schritt))
            start = None
    return 0


if __name__ == "__main__":
    sys.exit(main())
