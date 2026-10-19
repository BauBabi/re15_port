#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Runde 39 Spur T "titelversion" - Glyphen-Tabelle fuer die Versionszeile im Titelbild.

Nutzer (AUFTRAG.md, Nachtrag 14): "bei dieser und zukuenftig neuen Versionen passe mir bitte die Version im Titelscreen an.
Entferne das bestehende." Dossier: analysis/befunde_runde39/T_titelversion.md.

Quelle: re15_port/shared_assets/PSX/DATA/TITLEU.TIM (320x240, 16 bpp, Pixel (x,y) bei Datei-Byte-Offset 0x14 + 2*(y*320+x)).
Die Zeile "Ver.2025.01.25" (y 24..33) ist eine Pixelschrift: Raster 1,12 px (28/25) je Schrift-Pixel, jedes Schrift-Pixel als
Quadrat 1,52 px (38/25), oberste Schrift-Zeile ab y 24,88, Wert = round(25 * Flaechendeckung) (Messung: Dossier 1.2).

Erzeugt re15_port/engine/src/gen/titelversion_r39_glyphen.inc:
  * AUSGESCHNITTEN aus TITLEU.TIM (Werte bytegleich, Herkunft je Glyphe): V e r . 0 1 2 5
  * ERGAENZT im selben Raster mit derselben Schattierung: 3 4 6 7 8 9 d v
    Form: PORT-WAHL (5x7-Bitmuster der HD44780-A00-Schrift, aus der "0" und "2" der Titelzeile bitgleich stammen);
    Schattierung: Phase der "0"(x233)/"5"(x277)/"r"(x214) (Tintenkante links bei .60); je Bildpixel zuerst der WERT
    derselben lokalen Lage in diesen drei Original-Glyphen (gleiche Spalte/Zeile im Raster + gleiche Nachbarschaft der
    Schrift-Pixel), sonst das Flaechenmodell. Die Zahl der Pixel je Quelle schreibt das Skript in den Kommentar.

Aufruf (aus dem Repo): python re15_port/tools/r39_titelversion/glyphen_bauen.py [--pruefen]
  --pruefen: nichts schreiben, nur gegen die vorhandene .inc vergleichen (Rueckgabe 1 bei Abweichung).
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
TIM = os.path.join(PORT, 'shared_assets', 'PSX', 'DATA', 'TITLEU.TIM')
AUS = os.path.join(PORT, 'engine', 'src', 'gen', 'titelversion_r39_glyphen.inc')

S = 25          # Teilpunkte je Pixel und Achse (Tintenkanten liegen auf 0,04-px-Stufen, Dossier 1.2)
RASTER = 28     # 1,12 px je Schrift-Pixel (in 1/25 px)
QUADRAT = 38    # 1,52 px Kantenlaenge je Schrift-Pixel (in 1/25 px)
OBEN = 622      # 24,88 px = obere Tintenkante von Schrift-Zeile 0 (in 1/25 px)
ZEILE0 = 24     # erste Bildzeile der Kacheln (Halo-Zeile ueber dem Kopfbalken)
ZEILEN = 10     # Bildzeilen 24..33


def tim_lesen():
    d = open(TIM, 'rb').read()
    magic, flag = struct.unpack_from('<II', d, 0)
    _groesse, _vx, _vy, w, h = struct.unpack_from('<IHHHH', d, 8)
    assert magic == 0x10 and flag == 2 and (w, h) == (320, 240), 'TITLEU.TIM: unerwarteter Kopf'
    px = struct.unpack_from('<%dH' % (w * h), d, 20)
    return px, w


def wert(px, w, x, y):
    c = px[y * w + x]
    r, g, b = c & 31, (c >> 5) & 31, (c >> 10) & 31
    assert r == g == b, 'Pixel (%d,%d) nicht grau: %04x' % (x, y, c)
    return r


def muster(zeilen):
    return [[1 if ch == '#' else 0 for ch in z] for z in zeilen]


# --- Ausgeschnittene Glyphen ------------------------------------------------------------------------------------------
# (Zeichen, erste Spalte, letzte Spalte, Tintenkante links in 1/25 px relativ zur ersten Spalte, Bitmuster-Breite,
#  {Spalte: Bildzeilen, die zum NACHBARN gehoeren}, Herkunftstext)
# Tintenkante: Modell-Anpassung tools/r39_titelversion/messen.py (Dossier 1.2, Phase auf dem 1/25-px-Raster);
# Tintenbreite = (n-1)*1,12 + 1,52 px.
AUSSCHNITT = [
    ('V', 198, 207, 20, 7, {207: range(27, 34)},
     'V aus "Ver." x198..207 (Spalte 207 nur Zeilen 24..26; 27..33 = linke Kante des "e")'),
    ('e', 207, 213, 21, 5, {207: range(24, 27)}, 'e aus "Ver." x207..213 (Spalte 207 nur Zeilen 27..33)'),
    ('r', 214, 220, 15, 5, {}, 'r aus "Ver." x214..220'),
    ('.', 221, 223, 8, 2, {}, '. aus "Ver." x221..223 (gleiche Werte wie der Punkt bei x267..269)'),
    ('0', 233, 239, 15, 5, {}, '0 aus "2025" x233..239'),
    ('1', 264, 266, 0, 2, {}, '1 aus ".01." x264..266'),
    ('2', 226, 232, 21, 5, {}, '2 aus "2025" x226..232 (bitgleich mit der "2" bei x270..276)'),
    ('5', 277, 283, 15, 5, {}, '5 aus ".25" x277..283 (letztes Zeichen der Zeile)'),
]

# --- Ergaenzte Glyphen (Form = PORT-WAHL, HD44780 A00 5x7) -------------------------------------------------------------
ERGAENZT = {
    '3': ['#####', '...#.', '..#..', '...#.', '....#', '#...#', '.###.'],
    '4': ['...#.', '..##.', '.#.#.', '#..#.', '#####', '...#.', '...#.'],
    '6': ['..##.', '.#...', '#....', '####.', '#...#', '#...#', '.###.'],
    '7': ['#####', '....#', '...#.', '..#..', '.#...', '.#...', '.#...'],
    '8': ['.###.', '#...#', '#...#', '.###.', '#...#', '#...#', '.###.'],
    '9': ['.###.', '#...#', '#...#', '.####', '....#', '...#.', '.##..'],
    'd': ['....#', '....#', '.##.#', '#..##', '#...#', '#...#', '.####'],
    'v': ['.....', '.....', '#...#', '#...#', '#...#', '.#.#.', '..#..'],
}
# Lern-Glyphen derselben Phase (Tintenkante links bei erster Spalte + 0,60): Bitmuster aus der Anpassung (Dossier 1.2)
LERN = [
    ('0', 233, muster(['.###.', '#...#', '#..##', '#.#.#', '##..#', '#...#', '.###.'])),
    ('5', 277, muster(['#####', '#....', '####.', '#...#', '....#', '#...#', '.###.'])),
    ('r', 214, muster(['.....', '.....', '#.##.', '##..#', '#....', '#....', '#....'])),
]
PHASE = 15      # 0,60 px in 1/25 px


def zellen(a25, start25):
    """Raster-Indizes, deren Quadrat [start + k*RASTER, +QUADRAT) das Pixel [a25, a25+S) schneidet."""
    return [k for k in range(-2, 9) if start25 + k * RASTER < a25 + S and start25 + k * RASTER + QUADRAT > a25]


def bit(bm, i, j):
    return bm[j][i] if 0 <= j < len(bm) and 0 <= i < len(bm[0]) else 0


def schluessel(bm, c, r):
    """Lokale Lage eines Kachelpixels: (Spalte, Zeile, Schrift-Pixel der schneidenden Zellen + 1 Zelle Rand)."""
    ii = zellen(c * S, PHASE)
    jj = zellen((ZEILE0 + r) * S, OBEN)
    if not ii or not jj:
        return (c, r, ())
    nb = tuple(bit(bm, i, j) for j in range(jj[0] - 1, jj[-1] + 2) for i in range(ii[0] - 1, ii[-1] + 2))
    return (c, r, nb)


def modell(bm, phase, breite):
    """Flaechenmodell: Kachel `breite` x ZEILEN, Tintenkante links bei `phase`/25 px; Wert = round(25 * Deckung)."""
    zaehl = [[0] * breite for _ in range(ZEILEN)]
    for r in range(ZEILEN):
        for sy in range(S):
            y = (ZEILE0 + r) * S + sy          # Teilzeile (ganzzahlige 1/25-px-Lage)
            belegt = [False] * (breite * S)
            for j in range(len(bm)):
                y0 = OBEN + j * RASTER
                if not (y0 <= y < y0 + QUADRAT):
                    continue
                for i in range(len(bm[0])):
                    if bm[j][i]:
                        a = max(0, phase + i * RASTER)
                        b = min(breite * S, phase + i * RASTER + QUADRAT)
                        for x in range(a, b):
                            belegt[x] = True
            for c in range(breite):
                zaehl[r][c] += sum(belegt[c * S:(c + 1) * S])
    return [[(n * 25 + (S * S) // 2) // (S * S) for n in zeile] for zeile in zaehl]


def main():
    px, w = tim_lesen()
    glyphen = []
    # 1) Ausschnitte
    for ch, x0, x1, tl, nb, fremd, herkunft in AUSSCHNITT:
        breite = x1 - x0 + 1
        kachel = [[0] * breite for _ in range(ZEILEN)]
        for r in range(ZEILEN):
            for c in range(breite):
                x = x0 + c
                y = ZEILE0 + r
                if x in fremd and y in fremd[x]:
                    continue
                kachel[r][c] = wert(px, w, x, y)
        tr = tl + (nb - 1) * RASTER + QUADRAT
        glyphen.append((ch, breite, kachel, tl, tr,
                        'AUSGESCHNITTEN: %s; TITLEU.TIM Datei-Byte 0x%X..0x%X (Zeile y 24), je weitere Zeile +0x280'
                        % (herkunft, 0x14 + 2 * (ZEILE0 * 320 + x0), 0x14 + 2 * (ZEILE0 * 320 + x1) + 1)))
    # 2) Lern-Tabelle: lokale Lage -> Wert, aus den Original-Glyphen derselben Phase
    lern = {}
    widerspruch = set()
    for _ch, x0, bm in LERN:
        for r in range(ZEILEN):
            for c in range(7):
                k = schluessel(bm, c, r)
                v = wert(px, w, x0 + c, ZEILE0 + r)
                if k in lern and lern[k] != v:
                    widerspruch.add(k)
                lern[k] = v
    for k in widerspruch:
        del lern[k]
    # Gegenprobe: das reine Flaechenmodell an den Lern-Glyphen (Dokumentation)
    gegen = []
    for ch, x0, bm in LERN:
        m = modell(bm, PHASE, 7)
        d = [m[r][c] - wert(px, w, x0 + c, ZEILE0 + r) for r in range(ZEILEN) for c in range(7)]
        gegen.append('%s(x%d) SSE %d max %d' % (ch, x0, sum(e * e for e in d), max(abs(e) for e in d)))
    # 3) Ergaenzte Glyphen
    for ch in '346789dv':
        bm = muster(ERGAENZT[ch])
        breite = 7
        m = modell(bm, PHASE, breite)
        kachel = [[0] * breite for _ in range(ZEILEN)]
        n_lern = n_mod = 0
        for r in range(ZEILEN):
            for c in range(breite):
                k = schluessel(bm, c, r)
                if k in lern:
                    kachel[r][c] = lern[k]
                    n_lern += 1 if (lern[k] or m[r][c]) else 0
                else:
                    kachel[r][c] = m[r][c]
                    n_mod += 1 if m[r][c] else 0
        tr = PHASE + 4 * RASTER + QUADRAT
        glyphen.append((ch, breite, kachel, PHASE, tr,
                        'ERGAENZT: Form %s (PORT-WAHL, HD44780 A00), Phase wie 0/5/r (.60); %d Pixel aus derselben '
                        'Lage in 0/5/r, %d aus dem Flaechenmodell' % (' '.join(ERGAENZT[ch]), n_lern, n_mod)))
    # 4) Ausgabe
    z = ['/* AUTOMATISCH ERZEUGT von re15_port/tools/r39_titelversion/glyphen_bauen.py - nicht von Hand aendern.',
         ' * Runde 39 Spur T "titelversion" (Dossier analysis/befunde_runde39/T_titelversion.md).',
         ' * Pixelschrift der Titelzeile "Ver.2025.01.25" aus DATA/TITLEU.TIM: Raster 1,12 px, Quadrat 1,52 px, oberste',
         ' * Schrift-Zeile ab y 24,88, Wert = round(25 * Deckung) (5 bit, Grau r=g=b). Kacheln: Bildzeilen 24..33,',
         ' * Tintenkanten in 1/25 px relativ zur ersten Kachelspalte.',
         ' * Gegenprobe reines Flaechenmodell an den Lern-Glyphen: ' + '; '.join(gegen) + '. */']
    for ch, breite, kachel, tl, tr, herkunft in glyphen:
        name = {'.': 'punkt'}.get(ch, ch)
        z.append('/* %s */' % herkunft)
        z.append('static const uint8_t k_r39t_g_%s[%d] = {' % (name, breite * ZEILEN))
        for r in range(ZEILEN):
            z.append('    ' + ', '.join('%2d' % v for v in kachel[r]) + ',   /* y %d */' % (ZEILE0 + r))
        z.append('};')
    z.append('static const re15_r39t_glyphe_t k_r39t_glyphen[] = {')
    for ch, breite, kachel, tl, tr, herkunft in glyphen:
        name = {'.': 'punkt'}.get(ch, ch)
        quelle = 'RE15_R39T_AUSGESCHNITTEN' if herkunft.startswith('AUS') else 'RE15_R39T_ERGAENZT'
        z.append("    { '%s', %d, %3d, %3d, %s, k_r39t_g_%s }," % (ch, breite, tl, tr, quelle, name))
    z.append('};')
    text = '\n'.join(z) + '\n'
    if '--pruefen' in sys.argv:
        alt = open(AUS, encoding='utf-8').read() if os.path.exists(AUS) else ''
        if alt.replace('\r\n', '\n') != text:
            print('ABWEICHUNG: %s ist nicht der Stand des Generators' % AUS)
            return 1
        print('OK: %s entspricht dem Generator' % AUS)
        return 0
    os.makedirs(os.path.dirname(AUS), exist_ok=True)
    open(AUS, 'w', encoding='utf-8', newline='\n').write(text)
    print('geschrieben: %s (%d Glyphen)' % (AUS, len(glyphen)))
    for g in gegen:
        print('  Gegenprobe Modell:', g)
    for ch, _breite, _kachel, _tl, _tr, herkunft in glyphen:
        print('  %s: %s' % (ch, herkunft))
    return 0


if __name__ == '__main__':
    sys.exit(main())
