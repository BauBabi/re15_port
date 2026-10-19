#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Runde 39 Spur T "titelversion", Nachbesserung 1 - Titel-Stempel des Vorprojekts nachmessen (liest nur).

Nutzer (AUFTRAG.md, Nachtrag 14): "Ich glaube dafuer gibt es auch schon ein Skript iregndwo." Das Vorprojekt
(C:/workspace/git/reAi, Skript scripts/patch_save_final.py - existiert nicht mehr) hat bei jeder Version eine Zeile
"v<Version> <Datum>" in DATA/TITLEU.TIM der gepatchten Disc geschrieben. Erhalten ist nur das Ergebnis:
info/Re1.5/re15_save_final.bin (Hauptbaum, ungetrackt; CLAUDE.md: gepatchtes Disc-Image des Vorprojekts).
Dossier: analysis/befunde_runde39/T_titelversion.md Abschnitte 1.3, 7, 8.

Was das Skript misst:
  * ISO9660-Weg /PSX/DATA/TITLEU.TIM in der BIN (MODE2/2352 laut .cue, Nutzdaten ab Byte 24 je Sektor) -> LBA, Groesse
  * Unterschied zur Auslieferung (re15_port/shared_assets/PSX/DATA/TITLEU.TIM, bytegleich mit info/Re1.5/PSX):
    Bytes, Pixel, bbox; was mit der alten Beschriftung (Maske wie Dossier 1.1) und mit der Hintergrundkunst geschah
  * die gestempelte Zeile als ASCII und je Glyphe das Bitmuster, verglichen mit den Glyphen des Ports
    (ERGAENZT aus glyphen_bauen.py; alte Zeile aus Dossier 1.2)
  * optional ein Vergleichsbild Auslieferung / Vorprojekt / Port (--bild, --port-tim; Port-TIM aus anwenden_tim.c)

Aufruf: python re15_port/tools/r39_titelversion/vorprojekt_titel.py <re15_save_final.bin>
          [--bild aus.png --port-tim TITLEU_port.TIM --port-text "Ver.0.8.27"]
"""
import hashlib
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
AUSL = os.path.join(PORT, 'shared_assets', 'PSX', 'DATA', 'TITLEU.TIM')
sys.path.insert(0, HIER)
from glyphen_bauen import ERGAENZT          # noqa: E402  (Formen der ergaenzten Port-Glyphen, PORT-WAHL)

RAW, HDR = 2352, 24
W, H = 320, 240
# Bitmuster der alten Zeile "Ver.2025.01.25" aus der Modell-Anpassung (Dossier 1.2, messen.py)
ALTE_ZEILE = {
    '0': ['.###.', '#...#', '#..##', '#.#.#', '##..#', '#...#', '.###.'],
    '2': ['.###.', '#...#', '....#', '...#.', '..#..', '.#...', '#####'],
    '5': ['#####', '#....', '####.', '#...#', '....#', '#...#', '.###.'],
    '1': ['.#', '##', '.#', '.#', '.#', '.#', '.#'],
}
LESUNG = 'V1.18 2026.04.24'     # Lesung der gestempelten Zeile (ASCII unten); Selbstprobe: gleiche Zeichen = gleiche Muster


def sektor(f, lba):
    f.seek(lba * RAW + HDR)
    return f.read(2048)


def lies(f, lba, n):
    out = bytearray()
    while len(out) < n:
        out += sektor(f, lba)
        lba += 1
    return bytes(out[:n])


def eintraege(f, lba, size):
    d, i, res = lies(f, lba, size), 0, []
    while i < len(d):
        if d[i] == 0:
            i = (i // 2048 + 1) * 2048
            continue
        ext, sz, nl = struct.unpack_from('<I', d, i + 2)[0], struct.unpack_from('<I', d, i + 10)[0], d[i + 32]
        res.append((d[i + 33:i + 33 + nl].decode('ascii', 'replace').split(';')[0].upper(), ext, sz))
        i += d[i]
    return res


def tim_aus_bin(pfad):
    with open(pfad, 'rb') as f:
        pvd = sektor(f, 16)
        if pvd[1:6] != b'CD001':
            raise SystemExit('kein ISO9660 (MODE2/2352) in %s' % pfad)
        lba, size = struct.unpack_from('<I', pvd, 156 + 2)[0], struct.unpack_from('<I', pvd, 156 + 10)[0]
        for teil in ('PSX', 'DATA', 'TITLEU.TIM'):
            treffer = [e for e in eintraege(f, lba, size) if e[0] == teil]
            if not treffer:
                raise SystemExit('nicht gefunden: %s' % teil)
            lba, size = treffer[0][1], treffer[0][2]
        return lba, size, lies(f, lba, size)


def pixel(b):
    return [struct.unpack_from('<H', b, 0x14 + 2 * i)[0] for i in range(W * H)]


def rgb(v):
    return (v & 31, (v >> 5) & 31, (v >> 10) & 31)


def maske_alt(px):
    """Dossier 1.1: graue 8-zusammenhaengende Komponenten im Kasten x188..295 y4..35 mit Maximum >= 20."""
    nz = {(x, y) for y in range(4, 36) for x in range(188, 296) if rgb(px[y * W + x]) != (0, 0, 0)}
    seen, maske = set(), set()
    for p in sorted(nz):
        if p in seen:
            continue
        comp, st = [], [p]
        seen.add(p)
        while st:
            q = st.pop()
            comp.append(q)
            for dx in (-1, 0, 1):
                for dy in (-1, 0, 1):
                    r = (q[0] + dx, q[1] + dy)
                    if r in nz and r not in seen:
                        seen.add(r)
                        st.append(r)
        werte = [rgb(px[y * W + x]) for (x, y) in comp]
        if all(len(set(c)) == 1 for c in werte) and max(c[0] for c in werte) >= 20:
            maske |= set(comp)
    return maske


def bbox(pkte):
    xs, ys = [p[0] for p in pkte], [p[1] for p in pkte]
    return 'x%d..%d y%d..%d' % (min(xs), max(xs), min(ys), max(ys))


def bild(pfad, lba, ap, vp, pp, port_text):
    from PIL import Image, ImageDraw
    bx0, bx1, by0, by1, s, lab = 170, 300, 4, 40, 5, 20
    pw, ph = (bx1 - bx0 + 1) * s, (by1 - by0 + 1) * s
    img = Image.new('RGB', (pw, 3 * (ph + lab)), (40, 40, 40))
    dr = ImageDraw.Draw(img)
    titel = ['Auslieferung DATA/TITLEU.TIM (info/Re1.5/PSX): "Sample" / "Ver.2025.01.25"',
             'Vorprojekt reAi, patch_save_final.py -> re15_save_final.bin LBA %d: "%s"' % (lba, LESUNG),
             'Port jetzt (re15_titelversion_r39_anwenden): "%s"' % port_text]
    for k, px in enumerate((ap, vp, pp)):
        oy = k * (ph + lab)
        dr.text((4, oy + 4), titel[k], fill=(255, 255, 255))
        for y in range(by0, by1 + 1):
            for x in range(bx0, bx1 + 1):
                r, g, b = rgb(px[y * W + x])
                dr.rectangle([(x - bx0) * s, oy + lab + (y - by0) * s, (x - bx0) * s + s - 1,
                              oy + lab + (y - by0) * s + s - 1], fill=(r << 3, g << 3, b << 3))
    img.save(pfad)
    print('Bild: %s (Ausschnitt x%d..%d y%d..%d, %dx)' % (pfad, bx0, bx1, by0, by1, s))


def main():
    a = sys.argv[1:]
    if not a:
        raise SystemExit(__doc__)
    bin_pfad = a[0]
    opt = dict(zip(a[1::2], a[2::2]))
    lba, size, vb = tim_aus_bin(bin_pfad)
    ab = open(AUSL, 'rb').read()
    vp, ap = pixel(vb), pixel(ab)
    print('/PSX/DATA/TITLEU.TIM in %s: LBA %d, %d B, sha256 %s' % (os.path.basename(bin_pfad), lba, size,
                                                                     hashlib.sha256(vb).hexdigest()))
    print('Auslieferung %s: sha256 %s; Kopf gleich: %s' % (os.path.relpath(AUSL, PORT), hashlib.sha256(ab).hexdigest(),
                                                           vb[:20] == ab[:20]))
    diff = [(i % W, i // W) for i in range(W * H) if vp[i] != ap[i]]
    print('verschieden: %d Bytes, %d Pixel, bbox %s' % (sum(1 for x, y in zip(vb, ab) if x != y), len(diff), bbox(diff)))
    x0, x1 = min(p[0] for p in diff), max(p[0] for p in diff)
    y0, y1 = min(p[1] for p in diff), max(p[1] for p in diff)
    m = maske_alt(ap)
    # neue Zeile = alles Nicht-Schwarze im Rechteck der Aenderungen (auch Pixel, die zufaellig den alten Wert behalten)
    neu = {(x, y) for y in range(y0, y1 + 1) for x in range(x0, x1 + 1) if rgb(vp[y * W + x]) != (0, 0, 0)}
    werte = sorted({vp[p[1] * W + p[0]] for p in neu})
    print('alte Beschriftung (Maske Dossier 1.1): %d Pixel; im Vorprojekt 0x8000: %d, von der neuen Zeile belegt: %d'
          % (len(m), sum(1 for (x, y) in m if vp[y * W + x] == 0x8000), sum(1 for p in m if p in neu)))
    kunst = [p for p in diff if p not in m and p not in neu and rgb(ap[p[1] * W + p[0]]) != (0, 0, 0)]
    if kunst:
        print('Hintergrundkunst mitgeloescht: %d Pixel (Werte %d..%d), bbox %s -> 0x8000' % (
            len(kunst), min(rgb(ap[y * W + x])[0] for (x, y) in kunst), max(rgb(ap[y * W + x])[0] for (x, y) in kunst),
            bbox(kunst)))
    n8 = sum(1 for y in range(y0, y1 + 1) for x in range(x0, x1 + 1) if vp[y * W + x] == 0x8000)
    print('Rechteck %s: %d Pixel = 0x8000 %d + neue Zeile %d + anderes %d' % (
        bbox(diff), (x1 - x0 + 1) * (y1 - y0 + 1), n8, len(neu), (x1 - x0 + 1) * (y1 - y0 + 1) - n8 - len(neu)))
    print('neue Zeile: %d Pixel, bbox %s, Werte %s (RGB 5 bit %s)' % (len(neu), bbox(neu), [hex(v) for v in werte],
                                                                       [rgb(v) for v in werte]))
    nx0, nx1 = min(p[0] for p in neu), max(p[0] for p in neu)
    ny0, ny1 = min(p[1] for p in neu), max(p[1] for p in neu)
    print('    ' + ''.join(str(x % 10) for x in range(nx0, nx1 + 1)))
    for y in range(ny0, ny1 + 1):
        print('%3d ' % y + ''.join('#' if (x, y) in neu else '.' for x in range(nx0, nx1 + 1)))
    spalten = [x for x in range(nx0, nx1 + 1) if any((x, y) in neu for y in range(ny0, ny1 + 1))]
    gruppen = []
    for x in spalten:
        if gruppen and x == gruppen[-1][-1] + 1:
            gruppen[-1].append(x)
        else:
            gruppen.append([x])
    zeichen = LESUNG.replace(' ', '')
    if len(gruppen) != len(zeichen):
        raise SystemExit('%d Glyphen, Lesung "%s" hat %d Zeichen' % (len(gruppen), LESUNG, len(zeichen)))
    gesehen, fehler = {}, 0
    print('Glyphen (Lesung "%s"), Muster Zeile %d..%d:' % (LESUNG, ny0, ny1))
    for ch, g in zip(zeichen, gruppen):
        mu = [''.join('#' if (x, y) in neu else '.' for x in range(g[0], g[-1] + 1)) for y in range(ny0, ny1 + 1)]
        if ch in gesehen and gesehen[ch] != mu:
            fehler += 1
        gesehen.setdefault(ch, mu)
        vgl = []
        if ch in ERGAENZT:
            vgl.append("Port ERGAENZT '%s' %s" % (ch, 'BITGLEICH' if ERGAENZT[ch] == mu else 'anders'))
        if ch in ALTE_ZEILE:
            vgl.append("alte Zeile '%s' %s" % (ch, 'BITGLEICH' if ALTE_ZEILE[ch] == mu else 'anders'))
        print('  %s x%d..%d  %s  %s' % (ch, g[0], g[-1], ' '.join(mu), '; '.join(vgl)))
    print('Selbstprobe Lesung (gleiches Zeichen = gleiches Muster): %s' % ('OK' if fehler == 0 else '%d WIDERSPRUECHE' % fehler))
    if '--bild' in opt:
        bild(opt['--bild'], lba, ap, vp, pixel(open(opt['--port-tim'], 'rb').read()), opt.get('--port-text', '?'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
