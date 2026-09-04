# -*- coding: utf-8 -*-
"""Die gemalten RE1.5-TUERSCHWENKE in den Kartenkacheln finden - und ein BILD dazu.

WARUM: seit die Karte das Original-Kartenmaterial zeichnet (v0.5.4), kommen die vom
Kuenstler HINEINGEMALTEN RE1.5-Tuerschwenke mit (Nutzer 2026-09-04). Ueber die Farbe sind
sie nicht zu fassen - sie sind in der WANDFARBE gemalt (Palettenindex 4, an allen 13
Blaettern nachgemessen: er traegt ueberall die zweitlaengsten Laeufe nach dem
Hintergrund-Index 0).

UEBER DIE LAUFLAENGE SCHON - ABER NICHT MIT EINER ABSOLUTEN SCHWELLE.
BEFUND §27 hat "Lauf <= 4, wachsen ueber Nachbarn mit Lauf <= 10" an EINEM Blatt gemessen
(6/6 Symbole, kein Wandpixel). Game-weit angewandt frisst das auf Blatt 8 ein Viertel der
Waende (§41) - dort sind die Waende von Natur aus kurz: laengster Lauf 32 gegen 47..112
auf den anderen. Die Schwelle muss RELATIV zur Wandlaenge des Blattes sein.

Aufruf:
    python re15_port/tools/tuersymbole.py            # Zahlen je Blatt
    python re15_port/tools/tuersymbole.py --bilder   # + PNG je Blatt zur Sichtpruefung
"""
import io
import os
import re
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
WAND = 4
DATA = os.path.join(HIER, '..', 'shared_assets', 'PSX', 'DATA')


def _blob():
    src = io.open(os.path.join(HIER, '..', 'engine', 'src', 're15_inv_ui_tables.c'),
                  encoding='utf-8', errors='replace').read()
    i = src.index('re15_inv_map_blob[')
    j = src.index('};', i)
    return bytes(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})', src[i:j]))


BL = _blob()
BASE = 0x800762A0


def _rd(a, n):
    return BL[a - BASE:a - BASE + n]


def rechtecke(pg):
    cnt = struct.unpack_from('<H', _rd(0x80076840 + pg * 8, 2))[0]
    lp = struct.unpack_from('<I', _rd(0x80076844 + pg * 8, 4))[0]
    aus = []
    for i in range(cnt):
        a = lp + i * 12
        x, y, w, h = struct.unpack_from('<hhhh', _rd(a, 8), 0)
        u, v = _rd(a + 8, 1)[0], _rd(a + 10, 1)[0]
        aus.append((x, y, w, h, u, v))
    return aus


def kachel(pg):
    d = open(os.path.join(DATA, 'MAP%02X.PIX' % (pg + 1)), 'rb').read()
    return [[((d[y * 128 + (u >> 1)] >> 4) if (u & 1) else (d[y * 128 + (u >> 1)] & 0xF))
             for u in range(256)] for y in range(256)]


def _laeufe(px, ben):
    lauf = [[0] * 256 for _ in range(256)]
    for y in range(256):
        for x in range(256):
            if not ben[y][x] or px[y][x] != WAND:
                continue
            lx = 0
            k = x
            while k < 256 and px[y][k] == WAND:
                lx += 1
                k += 1
            k = x - 1
            while k >= 0 and px[y][k] == WAND:
                lx += 1
                k -= 1
            ly = 0
            k = y
            while k < 256 and px[k][x] == WAND:
                ly += 1
                k += 1
            k = y - 1
            while k >= 0 and px[k][x] == WAND:
                ly += 1
                k -= 1
            lauf[y][x] = lx if lx > ly else ly
    return lauf


def erkennen(pg, s_saat=4, s_wachs=10):
    """Schwellen als ANTEIL der laengsten Wand des Blattes, nicht als feste Pixel.
    Auf Blatt 4 (laengste Wand 47) ergibt das Saat <= 6 / Wachstum <= 15 - dieselbe
    Groessenordnung wie die 4/10 aus §27, die dort gemessen wurden."""
    px = kachel(pg)
    ben = [[0] * 256 for _ in range(256)]
    for (x, y, w, h, u, v) in rechtecke(pg):
        for iy in range(v, min(256, v + h)):
            for ix in range(u, min(256, u + w)):
                ben[iy][ix] = 1
    lauf = _laeufe(px, ben)
    maxlauf = 0
    for y in range(256):
        for x in range(256):
            if lauf[y][x] > maxlauf:
                maxlauf = lauf[y][x]
    # ⛔ ABSOLUT, WIE IN §27 GEMESSEN. Eine blattrelative Schwelle wurde probiert und
    # ist SCHLECHTER: auf Blaettern mit langen Waenden wird sie zu gross (Blatt 12:
    # 63 % der Wand entfernt statt 4,8 %). Die Groesse eines gemalten Tuerschwenks
    # haengt nicht von der Laenge der Waende ab - sie ist eine feste Zeichenkonvention
    # (13 Pixel, §27).
    weg = [[0] * 256 for _ in range(256)]
    for y in range(256):
        for x in range(256):
            if ben[y][x] and px[y][x] == WAND and 0 < lauf[y][x] <= s_saat:
                weg[y][x] = 1
    for _ in range(24):
        ge = 0
        for y in range(1, 255):
            for x in range(1, 255):
                if weg[y][x] or not ben[y][x] or px[y][x] != WAND:
                    continue
                if lauf[y][x] > s_wachs:
                    continue
                if weg[y - 1][x] or weg[y + 1][x] or weg[y][x - 1] or weg[y][x + 1]:
                    weg[y][x] = 1
                    ge = 1
        if not ge:
            break
    return weg, lauf, px, ben, maxlauf, s_saat, s_wachs


def komponenten(weg):
    seen = [[0] * 256 for _ in range(256)]
    aus = []
    for y in range(256):
        for x in range(256):
            if not weg[y][x] or seen[y][x]:
                continue
            st = [(x, y)]
            pix = []
            while st:
                cx, cy = st.pop()
                if cx < 0 or cx > 255 or cy < 0 or cy > 255:
                    continue
                if seen[cy][cx] or not weg[cy][cx]:
                    continue
                seen[cy][cx] = 1
                pix.append((cx, cy))
                # ⛔ ACHTER-NACHBARSCHAFT. Eine DIAGONALE Wand wird als Treppchen
                # gezeichnet; ihre Pixel beruehren sich nur ueber die Ecke. Mit
                # Vierer-Nachbarschaft zerfaellt sie in hunderte Kruemel (Blatt 8: 317),
                # und jeder einzelne sieht aus wie ein Tuerschwenk. Ueber die Ecke
                # verbunden ist sie EINE lange Kette - und faellt an der Ausdehnung auf.
                for ddx in (-1, 0, 1):
                    for ddy in (-1, 0, 1):
                        if ddx or ddy:
                            st.append((cx + ddx, cy + ddy))
            aus.append(pix)
    return aus


def _bild(pg, px, ben, weg):
    try:
        from PIL import Image
    except ImportError:
        return False
    W, H, S = 256, 224, 3
    im = Image.new('RGB', (W * S, H * S), (0, 0, 40))
    pxl = im.load()
    FARBE = {1: (0, 100, 0), 4: (200, 200, 200)}
    for (rx, ry, rw, rh, u, v) in rechtecke(pg):
        for j in range(rh):
            for i in range(rw):
                sx, sy = u + i, v + j
                if sx > 255 or sy > 255:
                    continue
                idx = px[sy][sx]
                if weg[sy][sx]:
                    c = (255, 0, 0)
                elif idx in FARBE:
                    c = FARBE[idx]
                elif idx == 0:
                    continue
                else:
                    c = (70, 70, 110)
                for dy in range(S):
                    for dx in range(S):
                        X, Y = (rx + i) * S + dx, (ry + j) * S + dy
                        if 0 <= X < W * S and 0 <= Y < H * S:
                            pxl[X, Y] = c
    ziel = os.path.join(HIER, '..', '..', 'shots', 'tuersym_blatt%02d.png' % pg)
    if not os.path.isdir(os.path.dirname(ziel)):
        os.makedirs(os.path.dirname(ziel))
    im.save(ziel)
    return True


def main(argv):
    bilder = '--bilder' in argv
    print("%-6s %-8s %-14s %-9s %-8s %s"
          % ("Blatt", "Wandpix", "Schwellen", "entfernt", "Anteil", "Symbole (Groessen)"))
    for pg in range(13):
        weg, lauf, px, ben, maxlauf, ss, sw = erkennen(pg)
        n_wand = 0
        for y in range(256):
            for x in range(256):
                if ben[y][x] and px[y][x] == WAND:
                    n_wand += 1
        ks = komponenten(weg)
        # ⛔ AUSDEHNUNGS-GRENZE. Ein gemalter Tuerschwenk ist eine feste
        # Zeichenkonvention: 13 Pixel in einem 6x5-Kasten (BEFUND §27, SYM[13][2]).
        # Was groesser ist, ist keine Tuer - auf Blatt 8 sind das die diagonalen
        # Korridorwaende, die als Treppchen aus kurzen Laeufen gemalt sind.
        gefiltert = []
        for k in ks:
            bx = max(p[0] for p in k) - min(p[0] for p in k) + 1
            by = max(p[1] for p in k) - min(p[1] for p in k) + 1
            if bx <= 10 and by <= 10:
                gefiltert.append(k)
            else:
                for (cx, cy) in k:
                    weg[cy][cx] = 0
        ks = gefiltert
        n = sum(len(k) for k in ks)
        gr = sorted((len(k) for k in ks), reverse=True)
        print("%-6d %-8d max%-3d S<=%-2d W<=%-3d %-9d %5.1f %%  %2d  %s"
              % (pg, n_wand, maxlauf, ss, sw, n, 100.0 * n / max(1, n_wand),
                 len(ks), gr[:6]))
        if bilder:
            _bild(pg, px, ben, weg)
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
