#!/usr/bin/env python3
"""Runde 42 Strang "gegenstaende", Stufe 2: Item-Bild (ITPS 112x72) der Blue Master Keycard 0x47 mit dem Namen "MICHAEL"
-> engine/src/gen/gegenstaende_r42_karte.inc

    python re15_port/tools/r42_gegenstaende/michaelkarte.py [--check] [--messbild <png>]

Warum (Konzept T-09, Uebergabe 11-17 "Kartenbild Michael"): Die Notiz in 4010 (msg 39 @0x183D) legt die Karte zu "Michael", die
Beschreibung ist seit r42/story angeglichen (re15_story_klein_r42.h). Das Bild (ITPS.ITP Block 0x47 @0xD5000) traegt aber noch den
zweizeiligen Namen "GORO / MACIDA" unter dem Foto (dieselbe Karte wie Disc-Block 0x36 der 40-%-Disc, dort grau).

Nur Original-Material:
  * Karte, Farben: der Block selbst (CLUT +0x14 unveraendert, jedes neue Pixel = naechster Eintrag DIESER CLUT).
  * Buchstaben: die Spielschrift DATA/TEX.TIM (Schriftseite x 256..511, Zelle 16x16 ab y 32, Code-Tabelle wie
    render_pc.c re15_msgfont_*; Codes M 0x29, I 0x25, C 0x1F, H 0x24, A 0x1D, E 0x21, L 0x28 = Grossbuchstaben ab A 0x1D,
    belegt durch "Goro" 0x23 0x4b 0x4e 0x4b in der Beschreibung @0x800c5e53, test_r42_klein.c).
  * Lage, Hoehe, Breite und Schwaerze der Schrift: am Original GEMESSEN (Zeile "GORO"), nicht gewaehlt:
      - Kartenkante unten = Ausgleichsgerade durch die Hell->Dunkel-Uebergaenge der Spalten 60..90 (Schwelle 8 von 0..31),
      - Zeilen getrennt am Minimum der Tinte quer zur Kante; Anfang/Ende der Zeile 1 laengs der Kante,
      - Laengsmass so, dass "GORO" in der Spielschrift genau die gemessene Laenge der Original-Zeile hat,
      - Hoehe und Lage quer: kleinste Fehlerquadrate zwischen "GORO" in der Spielschrift und der Original-Zeile (0.1-px-Raster),
      - Tintenmenge je Buchstabe = die der Original-Zeile "GORO" (Summe Hintergrund minus Pixel).
  * Hintergrund unter dem alten Namen: Profil quer zur Kante aus den Spalten 76..79 (rechts neben der Schrift, links vom Logo).
PORT-WAHL: ein Name in einer Zeile ("MICHAEL" auf der Lage von "GORO"); die Zeile "MACIDA" entfaellt, weil der Text keinen
Nachnamen kennt ("It belongs to someone named \"Michael.\"").
Die Datei ITPS.ITP bleibt byte-true; der Port setzt das Bild beim Laden ein (re15_gegenstaende_r42_bild_einsetzen).
"""
import math, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
PSX = os.path.join(PORT, 'shared_assets', 'PSX')
ZIEL = os.path.join(PORT, 'engine', 'src', 'gen', 'gegenstaende_r42_karte.inc')
ID = 0x47
W, H = 112, 72
NAME = 'MICHAEL'
CODES = {'A': 0x1D, 'C': 0x1F, 'D': 0x20, 'E': 0x21, 'G': 0x23, 'H': 0x24, 'I': 0x25, 'L': 0x28, 'M': 0x29, 'O': 0x2B, 'R': 0x2E}
# Bereich der alten Schrift (Bildkoordinaten): Pixelmitte x 53 bis 77, quer 1.5 .. 11.5 Pixel ueber der Kante.
S_X0, S_X1 = 53.0, 77.0
D_LO, D_HI = 1.5, 11.5
REF_X = (76, 77, 78, 79)


def rgb(v):
    return ((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3)


def lade():
    d = open(os.path.join(PSX, 'ITEM', 'ITPS.ITP'), 'rb').read()
    o = ID * 0x3000
    clut = [struct.unpack_from('<H', d, o + 0x14 + 2 * k)[0] for k in range(256)]
    img = bytearray(d[o + 0x220:o + 0x220 + W * H])
    return clut, img


def schrift():
    d = open(os.path.join(PSX, 'DATA', 'TEX.TIM'), 'rb').read()
    fl = struct.unpack_from('<I', d, 4)[0]
    assert fl & 8 and (fl & 7) == 0, 'TEX.TIM 4bpp mit CLUT erwartet'
    o = 8 + struct.unpack_from('<I', d, 8)[0]
    iw, ih = struct.unpack_from('<HH', d, o + 8)
    pitch = iw * 2
    pix = d[o + 12:o + 12 + pitch * ih]
    glyph = {}
    for ch, code in CODES.items():
        ax, ay = 256 + (code & 15) * 16, (code >> 4) * 16 + 32
        g = [[0] * 16 for _ in range(16)]
        for y in range(16):
            for x in range(16):
                b = pix[(ay + y) * pitch + (ax + x) // 2]
                g[y][x] = 1 if ((b >> 4) if (ax + x) & 1 else (b & 15)) else 0
        xs = [x for y in range(16) for x in range(16) if g[y][x]]
        ys = [y for y in range(16) for x in range(16) if g[y][x]]
        glyph[ch] = (g, min(xs), max(xs), min(ys), max(ys))
    return glyph


def lum(c):
    r, g, b = c
    return (3 * r + 6 * g + b) / 10.0


def main():
    clut, img = lade()
    pal = [rgb(v) for v in clut]
    L = lambda x, y: lum(pal[img[y * W + x]])
    # 1) Kartenkante unten: Uebergang >= 8*8 -> < 8*8 (Helligkeit in 0..255, Schwelle 64 = Stufe 8 von 31)
    pts = []
    for x in range(60, 91):
        for y in range(30, H - 1):
            a, b = L(x, y), L(x, y + 1)
            if a >= 64 and b < 64 and all(L(x, yy) < 32 for yy in range(y + 1, min(H, y + 6))):
                pts.append((x, y + (a - 64) / (a - b)))
                break
    n = len(pts)
    mx = sum(p[0] for p in pts) / n; my = sum(p[1] for p in pts) / n
    m = sum((p[0] - mx) * (p[1] - my) for p in pts) / sum((p[0] - mx) ** 2 for p in pts)
    c = my - m * mx
    nrm = math.sqrt(1 + m * m)
    ux, uy = 1 / nrm, m / nrm                       # laengs der Kante (nach rechts oben)

    def sd(x, y):                                   # (laengs, quer ueber der Kante)
        return x * ux + y * uy, ((m * x + c) - y) / nrm

    # 2) Hintergrundprofil quer zur Kante aus den Referenzspalten
    prof = []
    for x in REF_X:
        for y in range(25, H):
            s, d = sd(x, y)
            if 0.0 <= d <= D_HI + 3:
                prof.append((d, pal[img[y * W + x]]))
    prof.sort()

    def hg(d):
        near = sorted(prof, key=lambda p: abs(p[0] - d))[:6]
        wsum = sum(1.0 / (0.25 + abs(p[0] - d)) for p in near)
        return tuple(sum(p[1][k] / (0.25 + abs(p[0] - d)) for p in near) / wsum for k in range(3))

    # Kartenflaeche = nicht der blaue Fotorahmen (dort B - R >= 16: gemessen 16/144, 96/160, 136/160 links der Schrift; Tinte
    # und Kartenweiss haben R >= B, z. B. 104/96, 200/192)
    karte = lambda x, y: pal[img[y * W + x]][2] - pal[img[y * W + x]][0] < 16
    bereich = []
    for y in range(H):
        for x in range(W):
            s, d = sd(x + 0.5, y + 0.5)
            if S_X0 <= x + 0.5 <= S_X1 and D_LO <= d <= D_HI and karte(x, y):
                bereich.append((x, y, s, d))
    # 3) Tinte der Originalzeile 1 ("GORO") messen: Pixel des Bereichs mit d > Trennlinie zwischen den Zeilen
    tinte = {}
    for x, y, s, d in bereich:
        dunkel = lum(hg(d)) - L(x, y)
        if dunkel > 0:
            tinte[(x, y)] = (dunkel, s, d)
    # Zeilentrennung: Minimum der Tintenmenge quer zur Kante (Histogramm 0.5 px)
    hist = {}
    for dk, s, d in tinte.values():
        hist[int(d * 2)] = hist.get(int(d * 2), 0) + dk
    kandid = [k for k in hist if 8 <= k <= 14]
    trenn = min(kandid, key=lambda k: hist[k]) / 2.0 + 0.25
    z1 = [(dk, s, d) for dk, s, d in tinte.values() if d > trenn and dk > 8]
    mass1 = sum(t[0] for t in z1)
    d_mitte = sum(t[0] * t[2] for t in z1) / mass1
    sz = [t[1] for t in z1 if t[0] > 40]
    z1_s0, z1_s1 = min(sz), max(sz) + 1
    ink = sorted((L(x, y), pal[img[y * W + x]]) for (x, y), (dk, s, d) in tinte.items() if d > trenn)[0:5]
    ink_rgb = tuple(sum(p[1][k] for p in ink) / len(ink) for k in range(3))
    dunkel_voll = lambda d: lum(hg(d)) - lum(ink_rgb)
    # 4) Spielschrift auf die gemessene Zeile legen
    glyph = schrift()
    gy0 = min(g[3] for g in glyph.values()); gy1 = max(g[4] for g in glyph.values()) + 1

    def deckung(text, x, y, s_anf, sx, d_oben, sy):
        k = 0
        for j in range(8):
            for i in range(8):
                s, d = sd(x + (i + 0.5) / 8, y + (j + 0.5) / 8)
                fx = (s - s_anf) / sx
                fy = (d_oben - d) / sy + gy0
                if fx < 0 or fy < gy0 or fy >= gy1:
                    continue
                pen = 0.0
                for ch in text:
                    g, x0, x1, _, _ = glyph[ch]
                    w = x1 - x0 + 1
                    if pen <= fx < pen + w:
                        if g[int(fy)][int(fx - pen) + x0]:
                            k += 1
                        break
                    pen += w + 1
        return k / 64.0

    def breite(t):
        return sum(glyph[ch][2] - glyph[ch][1] + 1 for ch in t) + (len(t) - 1)

    def schwaerze(covs, ds, ziel):
        lo, hi = 0.05, 40.0
        for _ in range(50):
            kk = (lo + hi) / 2
            mass = sum(min(1.0, kk * cv) * dunkel_voll(d) for cv, d in zip(covs, ds))
            lo, hi = (kk, hi) if mass < ziel else (lo, kk)
        return (lo + hi) / 2

    # Anpassung: "GORO" in der Spielschrift so legen und skalieren, dass seine Tinte der Original-Zeile 1 am naechsten kommt
    # (kleinste Fehlerquadrate ueber alle Pixel der Zeile 1; Laenge = gemessene Laenge, Hoehe und Lage quer im 0.1-px-Raster).
    sx = (z1_s1 - z1_s0) / breite('GORO')
    z1_px = [(x, y, d) for x, y, s, d in bereich if d > trenn]
    beob = [max(0.0, lum(hg(d)) - L(x, y)) for x, y, d in z1_px]
    best = None
    for hz in range(18, 46):
        hoehe_v = hz / 10.0
        for oz in range(80, 111):
            d_ob = oz / 10.0
            sy_v = hoehe_v / (gy1 - gy0)
            covs = [deckung('GORO', x, y, z1_s0, sx, d_ob, sy_v) for x, y, d in z1_px]
            kk = schwaerze(covs, [d for x, y, d in z1_px], mass1)
            err = sum((min(1.0, kk * cv) * dunkel_voll(d) - o) ** 2 for cv, (x, y, d), o in zip(covs, z1_px, beob))
            if best is None or err < best[0]:
                best = (err, hoehe_v, d_ob)
    _, hoehe, d_oben = best
    sy = hoehe / (gy1 - gy0)
    d_unten, d_top = d_oben - hoehe, d_oben
    cov = {(x, y): deckung(NAME, x, y, z1_s0, sx, d_oben, sy) for x, y, s, d in bereich}
    # Tintenmenge je Buchstabe wie in "GORO"
    ziel = mass1 / 4 * len(NAME)
    dunkel_voll = lambda d: lum(hg(d)) - lum(ink_rgb)
    lo, hi = 0.1, 20.0
    for _ in range(60):
        k = (lo + hi) / 2
        mass = sum(min(1.0, k * cov[(x, y)]) * dunkel_voll(d) for x, y, s, d in bereich)
        lo, hi = (k, hi) if mass < ziel else (lo, k)
    k = (lo + hi) / 2
    # 5) Bild bauen: Bereich = Hintergrund + Tinte, auf die CLUT des Blocks gerundet (Eintrag 0 = durchsichtig, nie gewaehlt)
    neu = bytearray(img)
    def naechster(c):
        best, bi = None, 1
        for i in range(1, 256):
            p = pal[i]
            e = (p[0] - c[0]) ** 2 + (p[1] - c[1]) ** 2 + (p[2] - c[2]) ** 2
            if best is None or e < best:
                best, bi = e, i
        return bi
    for x, y, s, d in bereich:
        a = min(1.0, k * cov[(x, y)])
        b = hg(d)
        neu[y * W + x] = naechster(tuple(b[i] * (1 - a) + ink_rgb[i] * a for i in range(3)))
    info = dict(kante='y = %.4f x + %.3f (%d Spalten)' % (m, c, n), trenn=trenn, d_mitte=d_mitte, d_zeile=(d_unten, d_top), hoehe=hoehe, sx=sx, sy=sy,
                goro_s=(z1_s0, z1_s1), tinte_goro=mass1, k=k, pixel=len(bereich),
                geaendert=sum(1 for i in range(W * H) if neu[i] != img[i]))
    return clut, img, neu, info, pal


def main_cli():
    clut, alt, neu, info, pal = main()
    for kk, v in info.items():
        print('%-11s %s' % (kk, v))
    out = ['/* Generiert von re15_port/tools/r42_gegenstaende/michaelkarte.py - nicht von Hand aendern. Item-Bild 0x47 (Blue Master',
           ' * Keycard, ITPS.ITP Block 0x47 @0xD5000, Bild +0x220) mit "MICHAEL" statt "GORO / MACIDA"; CLUT des Blocks unveraendert.',
           ' * Messung: Kartenkante %s, Zeile 1 quer %.1f..%.1f px ueber der Kante, Laengsmass %.4f px je Schriftpixel,' % (
               info['kante'], info['d_zeile'][0], info['d_zeile'][1], info['sx']),
           ' * Tinte "GORO" %.0f, %d Pixel im Bereich, davon %d geaendert. */' % (info['tinte_goro'], info['pixel'], info['geaendert']),
           'static const uint8_t k_g42_karte47[%d] = {' % (W * H)]
    for i in range(0, W * H, 28):
        out.append('    ' + ','.join('%d' % v for v in neu[i:i + 28]) + ',')
    out.append('};')
    text = chr(10).join(out) + chr(10)
    if '--messbild' in sys.argv:
        from PIL import Image, ImageDraw
        S = 6
        m = Image.new('RGB', (2 * (W * S + 10) + 10, 2 * (H * S + 30) + 40), (40, 40, 40)); dr = ImageDraw.Draw(m)
        dr.rectangle([0, 0, m.width, 20], fill=(160, 0, 0))
        dr.text((6, 5), 'Messbild - kein Ergebnis  (ITPS 0x47 alt | neu, x6 und x2)', fill=(255, 255, 255))
        for k, b in enumerate((alt, neu)):
            im = Image.new('RGB', (W, H)); im.putdata([pal[v] for v in b])
            m.paste(im.resize((W * S, H * S), Image.NEAREST), (10 + k * (W * S + 10), 30))
            m.paste(im.resize((W * 2, H * 2), Image.BILINEAR), (10 + k * (W * S + 10), 40 + H * S))
        m.save(sys.argv[sys.argv.index('--messbild') + 1])
    if '--check' in sys.argv:
        alt_t = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt_t == text else 'VERALTET'); sys.exit(0 if alt_t == text else 1)
    open(ZIEL, 'w', newline=chr(10)).write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main_cli()
