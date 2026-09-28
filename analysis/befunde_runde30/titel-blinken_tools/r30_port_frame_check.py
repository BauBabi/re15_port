#!/usr/bin/env python3
"""r30_port_frame_check.py - Runde 30 / Thema D.  ABNAHME-WERKZEUG fuer den Bau-Agenten.

Prueft ein Bild des Titelmenues (gdigrab des ECHTEN Port-Fensters, beliebiger ganzzahliger
Massstab, oder ein Original-Bildpuffer) pixelgenau gegen das ORIGINAL-MODELL der
Menuezeilen. Das Modell rechnet ausschliesslich aus den ausgelieferten Dateien
  DATA/TITLEU.TIM (Hintergrund) und DATA/TMOJI.TIM (Zeilen-Textur + CLUT)
und ist gegen 12 Original-Bildpuffer aus 6 Savestates geprueft (0 abweichende Pixel,
build/r30_titel-blinken/orig_row_sim.txt).

Modell (TITLE.BIN, Zeichner FUN_801027a0):
  je Zeile zwei Durchgaenge; gezeichnet wird ZUERST der subtraktive bei y+1
  (Texpage 0xe10000d5, @0x80102800-04 | 0x40 @0x8010281c; +0x10000 aufs xy-Wort
  @0x80102810-14), DANACH der additive bei y (0xe10000b5, | 0x20 @0x80102824) -
  AddPrim haengt vorn ein, der zuletzt eingehaengte Durchgang wird zuerst gezeichnet.
  Befehl 0x66808080 (@0x80102830-38) = texturiertes Rechteck, moduliert.
  aktive Zeile : CLUT 0x7fc0 (Deskriptor +0), Farbbytes = Pulswert (@0x80102844-58)
  inaktive     : CLUT 0x7fcc (Deskriptor +4), Farbbytes 0x80
  Modulation   : kanal5 = min(31, (texel5 * farbe8) >> 7)      (psx-spx "Modulation")
  Zeilen       : y = 0x85 / 0x99 / 0xad, x = 0x20, Breite 0x100, Hoehe 0x10
                 (@0x80102bb0-f8; Deskriptor 0x801028ac + 16*n: v = 16*n, 0x00100100)

Aufruf:
  r30_port_frame_check.py <bild.png> [cursor]          cursor 0|1|2 (Vorgabe 0)
Ausgabe: je Zeile die Zahl abweichender Pixel; fuer die aktive Zeile der Pulswert
(0x80..0xBE, Schritt 2) mit der besten Deckung. ABNAHME = 0 abweichende Pixel.
Rueckgabe 0 = alle vier Regionen decken sich, 1 = Abweichung.
"""
import sys, struct
import numpy as np
from PIL import Image

ASSETS = r"C:\workspace\git\reAi_v2\re15_port\shared_assets\PSX\DATA"
ROWY = [0x85, 0x99, 0xad]

def load_tim16(path):
    d = open(path, "rb").read()
    blen, x, y, w, h = struct.unpack_from("<IHHHH", d, 8)
    return np.frombuffer(d, dtype="<u2", count=w * h, offset=20).reshape(h, w)

def load_tim8(path):
    d = open(path, "rb").read()
    clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, 8)
    clut = np.frombuffer(d, dtype="<u2", count=cw * ch, offset=20)
    o = 8 + clen
    blen, x, y, w, h = struct.unpack_from("<IHHHH", d, o)
    pix = np.frombuffer(d, dtype=np.uint8, count=w * 2 * h, offset=o + 12).reshape(h, w * 2)
    return pix, clut

def split(c):
    return np.stack([c & 31, (c >> 5) & 31, (c >> 10) & 31], axis=-1).astype(np.int32)

def sim_strip(bg, pix, clut, y0, v0, h, clut_base, val):
    """liefert die Region y0..y0+h (h+1 Zeilen), x 0x20..0x11f als 5-Bit-RGB"""
    out = split(bg[y0:y0 + h + 1, 0x20:0x120].astype(np.int32))
    idx = pix[v0:v0 + h, 0:256].astype(np.int32) + clut_base
    # CLUT-Zeile ist 256 breit; Indizes darueber laegen im VRAM rechts daneben
    col = np.where(idx < clut.size, clut[np.minimum(idx, clut.size - 1)], 0).astype(np.int32)
    vis = (col != 0)[..., None]
    f = np.minimum(31, (split(col) * val) >> 7)
    sub = out.copy()
    sub[1:h + 1] = np.where(vis, np.maximum(0, sub[1:h + 1] - f), sub[1:h + 1])     # zuerst: B-F bei y+1
    add = sub.copy()
    add[0:h] = np.where(vis, np.minimum(31, add[0:h] + f), add[0:h])               # danach: B+F bei y
    return add

def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    cursor = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    im = Image.open(sys.argv[1]).convert("RGB")
    if im.size != (320, 240):
        if im.size[0] % 320 or im.size[1] % 240:
            print("Bildgroesse %s ist kein ganzzahliges Vielfaches von 320x240" % (im.size,)); return 2
        k = im.size[0] // 320
        im = im.resize((320, 240), Image.NEAREST)
        print("Bild um Faktor %d verkleinert (naechster Nachbar)" % k)
    got = np.array(im).astype(np.int32) >> 3
    bg = load_tim16(ASSETS + r"\TITLEU.TIM")
    pix, clut = load_tim8(ASSETS + r"\TMOJI.TIM")
    fail = 0
    for row in range(3):
        y0 = ROWY[row]; v0 = 16 * (row + 1)
        reg = got[y0:y0 + 17, 0x20:0x120]
        if row == cursor:
            best = None
            for val in range(0x80, 0xc0, 2):
                s = sim_strip(bg, pix, clut, y0, v0, 16, 0, val)
                bad = int((s != reg).any(axis=-1).sum())
                if best is None or bad < best[0]: best = (bad, val)
            print("Zeile %d AKTIV   : beste Deckung bei Pulswert 0x%02x (Faktor %.4f): %d von %d Pixeln weichen ab" % (
                row, best[1], best[1] / 128.0, best[0], reg.shape[0] * reg.shape[1]))
            fail |= (best[0] != 0)
        else:
            s = sim_strip(bg, pix, clut, y0, v0, 16, 192, 0x80)
            bad = int((s != reg).any(axis=-1).sum())
            print("Zeile %d inaktiv : %d von %d Pixeln weichen ab" % (row, bad, reg.shape[0] * reg.shape[1]))
            fail |= (bad != 0)
    # Copyright: FUN_80102948, Wort @0x80102a0c = 0x7fc05200 (CLUT 0x7fc0, v=0x52), 0x00140100, y=0xc8
    s = sim_strip(bg, pix, clut, 0xc8, 0x52, 20, 0, 0x80)
    reg = got[0xc8:0xc8 + 21, 0x20:0x120]
    bad = int((s != reg).any(axis=-1).sum())
    print("Copyright        : %d von %d Pixeln weichen ab" % (bad, reg.shape[0] * reg.shape[1]))
    fail |= (bad != 0)
    print("ERGEBNIS: %s" % ("ABWEICHUNG" if fail else "deckt sich pixelgenau mit dem Original-Modell"))
    return 1 if fail else 0

if __name__ == "__main__":
    sys.exit(main())
