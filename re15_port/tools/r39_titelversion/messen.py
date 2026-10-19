#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Runde 39 Spur T "titelversion" - Messung der alten Beschriftung in DATA/TITLEU.TIM (Dossier
analysis/befunde_runde39/T_titelversion.md, Abschnitte 1.1 und 1.2). Liest nur, schreibt nichts.

  1.1  8-zusammenhaengende Komponenten der Nicht-Schwarz-Pixel im Kasten x188..295 y4..35, Maske (graue Komponenten
       mit Maximum >= 20), Werte, Ring um die Maske, Rand des Kastens
  1.2  Pixelschrift: Bitmuster und Phase je Glyphe der Zeile "Ver.2025.01.25" per Bit-Abstieg gegen das Flaechenmodell
       (Raster 28/25 px, Quadrat 38/25 px, oberste Schrift-Zeile 622/25 px, Wert = round(25 x Deckung))

Aufruf (aus dem Repo): python re15_port/tools/r39_titelversion/messen.py [--ohne-anpassung]
"""
import collections
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
TIM = os.path.normpath(os.path.join(HIER, '..', '..', 'shared_assets', 'PSX', 'DATA', 'TITLEU.TIM'))
W, H = 320, 240
X0, X1, Y0, Y1 = 188, 295, 4, 35
S, RASTER, QUADRAT, OBEN = 25, 28, 38, 622


def laden():
    d = open(TIM, 'rb').read()
    assert struct.unpack_from('<II', d, 0) == (0x10, 2) and struct.unpack_from('<HH', d, 0x10) == (W, H)
    return struct.unpack_from('<%dH' % (W * H), d, 0x14)


def kanaele(c):
    return c & 31, (c >> 5) & 31, (c >> 10) & 31


def teil_maske(px):
    print('== 1.1 Komponenten im Kasten x%d..%d y%d..%d ==' % (X0, X1, Y0, Y1))
    rand = sum(1 for y in range(Y0, Y1 + 1) for x in range(X0, X1 + 1)
               if (x in (X0, X1) or y in (Y0, Y1)) and px[y * W + x] & 0x7fff)
    print('Nicht-Schwarz-Pixel auf dem Kastenrand:', rand)
    gesehen = set()
    maske = set()
    for y in range(Y0, Y1 + 1):
        for x in range(X0, X1 + 1):
            if (x, y) in gesehen or not px[y * W + x] & 0x7fff:
                continue
            stapel, teil = [(x, y)], []
            gesehen.add((x, y))
            while stapel:
                a, b = stapel.pop()
                teil.append((a, b))
                for dx in (-1, 0, 1):
                    for dy in (-1, 0, 1):
                        n = (a + dx, b + dy)
                        if X0 <= n[0] <= X1 and Y0 <= n[1] <= Y1 and n not in gesehen and px[n[1] * W + n[0]] & 0x7fff:
                            gesehen.add(n)
                            stapel.append(n)
            werte = [kanaele(px[b * W + a]) for a, b in teil]
            grau = all(r == g == bl for r, g, bl in werte)
            mx = max(max(v) for v in werte)
            xs = [a for a, _ in teil]
            ys = [b for _, b in teil]
            behalten = grau and mx >= 20
            print('  bbox x%d..%d y%d..%d  %4d Pixel  grau=%d  max=%2d  -> %s' % (
                min(xs), max(xs), min(ys), max(ys), len(teil), grau, mx, 'MASKE' if behalten else 'bleibt'))
            if behalten:
                maske.update(teil)
    xs = [a for a, _ in maske]
    ys = [b for _, b in maske]
    print('Maske: %d Pixel, bbox x%d..%d y%d..%d, Zeile y<=23: %d, y>=24: %d' % (
        len(maske), min(xs), max(xs), min(ys), max(ys), sum(1 for _, b in maske if b <= 23),
        sum(1 for _, b in maske if b >= 24)))
    print('Werte:', sorted(collections.Counter(kanaele(px[b * W + a])[0] for a, b in maske).items()))
    print('STP:', dict(collections.Counter(px[b * W + a] >> 15 for a, b in maske)))
    ring = {}
    for a, b in maske:
        for dx in (-1, 0, 1):
            for dy in (-1, 0, 1):
                n = (a + dx, b + dy)
                if n not in maske:
                    ring[n] = px[n[1] * W + n[0]]
    print('Ring (8-Nachbarn ausserhalb der Maske): %d Pixel, Werte %s' % (
        len(ring), dict(collections.Counter('0x%04X' % v for v in ring.values()))))


def modell(bm, phase, x0, breite):
    """Werte der Bildspalten x0..x0+breite-1, Zeilen 24..33; Tinte links bei x0 + phase/25 (Teilzeilen-Raster)."""
    out = []
    for r in range(24, 34):
        zaehl = [0] * breite
        for sy in range(S):
            y = r * S + sy
            belegt = [0] * (breite * S)
            for j, z in enumerate(bm):
                if OBEN + j * RASTER <= y < OBEN + j * RASTER + QUADRAT:
                    for i, ch in enumerate(z):
                        if ch == '#':
                            for x in range(max(0, phase + i * RASTER), min(breite * S, phase + i * RASTER + QUADRAT)):
                                belegt[x] = 1
            for c in range(breite):
                zaehl[c] += sum(belegt[c * S:(c + 1) * S])
        out.append([(n * 25 + 312) // 625 for n in zaehl])
    return out


def sse(bm, phase, x0, breite, ist):
    m = modell([''.join(z) for z in bm], phase, x0, breite)
    return (sum((m[r][c] - ist[r][c]) ** 2 for r in range(10) for c in range(breite)),
            max(abs(m[r][c] - ist[r][c]) for r in range(10) for c in range(breite)))


def teil_anpassung(px):
    print('== 1.2 Bitmuster + Phase je Glyphe (Bit-Abstieg gegen das Flaechenmodell) ==')
    faelle = [('0', 233, 5), ('0', 257, 5), ('2', 226, 5), ('2', 240, 5), ('5', 247, 5), ('5', 277, 5),
              ('V', 198, 7), ('e', 207, 5), ('r', 214, 5), ('1', 264, 2), ('.', 221, 2), ('.', 253, 2)]
    for ch, x0, nb in faelle:
        breite = {7: 10, 5: 7, 2: 3}[nb] if ch != '.' else 4
        cols = range(x0, x0 + breite)
        ist = [[kanaele(px[r * W + x])[0] for x in cols] for r in range(24, 34)]
        if ch == 'V':                       # Spalte 207 ab Zeile 27 gehoert zum "e"
            for r in range(27 - 24, 10):
                ist[r][9] = 0
        if ch == 'e':                       # Spalte 207 bis Zeile 26 gehoert zum "V"
            for r in range(0, 3):
                ist[r][0] = 0
        def start(phase):
            return [['#' if ist[min(9, int((OBEN + j * RASTER + 19) / S) - 24)][min(breite - 1, int((phase + i * RASTER + 19) / S))] >= 15
                     else '.' for i in range(nb)] for j in range(7)]
        # jede der 25 Phasen (Tintenkante links auf dem 1/25-px-Raster): Startmuster, dann Bit-Abstieg
        vor = [(0, ph) for ph in range(25)]
        bester = None
        for _, phase in vor:
            bm = start(phase)
            fehler = sse(bm, phase, x0, breite, ist)[0]
            besser = True
            while besser:
                besser = False
                for j in range(7):
                    for i in range(nb):
                        bm[j][i] = '.' if bm[j][i] == '#' else '#'
                        e = sse(bm, phase, x0, breite, ist)[0]
                        if e < fehler:
                            fehler = e
                            besser = True
                        else:
                            bm[j][i] = '.' if bm[j][i] == '#' else '#'
            e, mx = sse(bm, phase, x0, breite, ist)
            if bester is None or e < bester[0]:
                bester = (e, mx, phase, [''.join(z) for z in bm])
        e, mx, phase, bm = bester
        print('  %s (x%d)  Tinte links %d,%02d  SSE %3d  max %d  %s' % (ch, x0, x0 + phase // 25, (phase % 25) * 4, e, mx, ' '.join(bm)))


def main():
    px = laden()
    teil_maske(px)
    if '--ohne-anpassung' not in sys.argv:
        teil_anpassung(px)
    return 0


if __name__ == '__main__':
    sys.exit(main())
