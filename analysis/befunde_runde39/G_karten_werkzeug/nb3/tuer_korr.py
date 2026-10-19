"""Nachbesserung 3 (N2): welche Tuermarken (s_map_marks, re15_map_zones.h) liegen auf der ERSATZLINIE einer
Doppelwand-Korrektur (s_korr, karte_r39.c)? = Balken quer direkt neben den geloeschten Pixeln (+-1 Spalte bzw. Zeile),
laengs von ihnen ueberdeckt. Gilt die Korrektur nicht (Traeger ungezeichnet), steht die eigene Linie des korrigierten
Rechtecks noch - dann gehoert die Tuer dorthin (RE2: jede Raum-Kachel traegt die Tuer auf ihrer EIGENEN Wand, re2_tueren.py)."""
import re, os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from schwenk import rects
SRC = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..', 're15_port', 'engine', 'src')
h = open(os.path.join(SRC, 're15_map_zones.h'), encoding='utf-8', errors='replace').read()
blk = h[h.index('static const re15_map_mark_t s_map_marks[]'):]
blk = blk[:blk.index('\n};')]
blk = re.sub(r'/\*.*?\*/', '', blk, flags=re.S)
marks = [tuple(int(v) for v in m.groups()) for m in re.finditer(
    r'\{\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', blk)]
k = open(os.path.join(SRC, 'karte_r39.c'), encoding='utf-8', errors='replace').read()
kb = k[k.index('static const r39_korr_t s_korr[]'):]
kb = re.sub(r'/\*.*?\*/', '', kb[:kb.index('\n};')], flags=re.S)
korr = []
for m in re.finditer(r'\{\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*([A-Z0-9_()]+)\s*\}', kb):
    korr.append(tuple(int(v) for v in m.groups()[:7]) + (m.group(8),))
print('%d Marken, davon %d Tueren; %d Korrekturen' % (len(marks), sum(1 for m in marks if m[4] <= 3), len(korr)))
for (p, rc, mx, my, kind, zid, zid2, ap) in marks:
    if kind > 3: continue
    senk = kind in (1, 3)
    bar = [(mx, my + d) for d in range(-2, 3)] if senk else [(mx + d, my) for d in range(-2, 3)]
    for (kp, kr, c, r, w, hh, neu, tr) in korr:
        if kp != p or neu == 4: continue
        x, y, rw, rh, u, v = rects(p)[kr]
        pts = {(x + c + i, y + r + j) for j in range(hh) for i in range(w)}
        for d in (-1, 1):
            neben = [(bx + d, by) if senk else (bx, by + d) for (bx, by) in bar]
            n = sum(1 for q in neben if q in pts)
            if n:
                print('Tuer {%d,%d,%d,%d,%d,%d,%d,%d}: %d/5 Balkenpixel direkt %s neben Korrektur Blatt %d r%d Spalte %d Zeile %d '
                      '%dx%d neu %d Traeger %s (geloescht x%d..%d y%d..%d)' % (
                          p, rc, mx, my, kind, zid, zid2, ap, n, ('rechts' if d > 0 else 'links') if senk else ('unter' if d > 0 else 'ueber'),
                          kp, kr, c, r, w, hh, neu, tr, x + c, x + c + w - 1, y + r, y + r + hh - 1))
        # liegt der Balken selbst AUF geloeschten Pixeln?
        n0 = sum(1 for q in bar if q in pts)
        if n0:
            print('Tuer {%d,%d,%d,%d,%d,%d,%d,%d}: %d/5 Balkenpixel AUF Korrektur Blatt %d r%d (neu %d, Traeger %s)' % (
                p, rc, mx, my, kind, zid, zid2, ap, n0, kp, kr, neu, tr))
