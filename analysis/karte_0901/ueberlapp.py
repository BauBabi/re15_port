# -*- coding: utf-8 -*-
"""Toleranzfreies Mass: ueberlappt der GEZEICHNETE Port-Stempel das GEMALTE Symbol?

Der Port malt je Tuermarke 13 Pixel (re15_inv_screen.c, SYM[13][2]) im Wandton:
Westwand dx 0..5, dy -2..+2 um (mx,my); die anderen Wandseiten sind Spiegelungen.
Das gemalte Symbol steht im Katalog mit px_w x px_h ab (screen_x, screen_y).
Ueberlappen sich die beiden Kaesten, sieht der Spieler an EINER Stelle ZWEI Tueren."""
import sys, os, math, collections, csv
sys.path.insert(0, r'C:\workspace\git\reAi_v2\analysis\karte_0901')
from sonde2 import *
import sonde2 as S
OUT = []
def P(*a):
    s = ' '.join(str(x) for x in a); print(s); OUT.append(s)

SYMPX = [(0,-2),(1,-2),(2,-2),(3,-2),(4,-2),(5,-2),(5,-1),(5,0),(4,1),(0,2),(1,2),(2,2),(3,2)]
def stempel(mx, my, kind):
    out = []
    for dx, dy in SYMPX:
        if kind == 3: px, py = dx, dy
        elif kind == 1: px, py = -dx, dy
        elif kind == 0: px, py = dy, dx
        else: px, py = dy, -dx
        out.append((mx + px, my + py))
    return out

# gemalte Symbole als Pixelmenge (aus den Kachel-Pixeln des Katalog-Kastens)
SYMBOX = collections.defaultdict(list)
with open(os.path.join(r'C:\workspace\git\reAi_v2', 'analysis', 'kartensymbole', 'symbolkatalog.csv'),
          encoding='utf-8') as f:
    for t in csv.DictReader(f):
        if t['typ'] not in ('TUER', 'TUER2'): continue
        x, y = int(t['screen_x']), int(t['screen_y'])
        w, h = int(t['px_w']), int(t['px_h'])
        SYMBOX[int(t['seite'])].append((x, y, w, h, int(t['rect']), t['maskenbits']))

P("=" * 78)
P("TOLERANZFREIES MASS: PORT-STEMPEL GEGEN GEMALTES SYMBOL")
P("=" * 78)
P("Der Port zeichnet je Tuermarke 13 Pixel in genau dem Wandton des Originals")
P("(re15_inv_screen.c, static const signed char SYM[13][2]) - er ist von einem")
P("gemalten Symbol optisch nicht zu unterscheiden.")
P("")
tu = [m for m in MARKS if m[4] <= 3]
P("Tuermarken im Schnappschuss-Header: %d" % len(tu))
tref = []
for m in MARKS:
    pg, r, mx, my, kind = m[0], m[1], m[2], m[3], m[4]
    if kind > 3: continue
    S_ = set(stempel(mx, my, kind))
    best = None
    for (sx, sy, sw, sh, sr, bits) in SYMBOX.get(pg, ()):
        kasten = set((sx + i, sy + j) for i in range(sw) for j in range(sh))
        ov = len(S_ & kasten)
        if ov and (best is None or ov > best[0]): best = (ov, sx, sy, sw, sh, sr)
    if best: tref.append((m, best))
P("davon ueberlappt der Stempel ein gemaltes Symbol: %d  (%.0f %%)"
  % (len(tref), 100.0 * len(tref) / len(tu)))
P("   im EIGENEN Rechteck: %d      im NACHBAR-Rechteck: %d"
  % (sum(1 for m, b in tref if b[5] == m[1]), sum(1 for m, b in tref if b[5] != m[1])))
P("")
P("   Marke                     ueberlappte Pixel   gemaltes Symbol")
for m, b in sorted(tref, key=lambda t: (t[0][0], t[0][1])):
    P("   S%-2d r%-2d (%3d,%3d) kind%d      %2d von 13     r%-2d (%3d,%3d) %dx%d"
      % (m[0], m[1], m[2], m[3], m[4], b[0], b[5], b[1], b[2], b[3], b[4]))
P("")
P("Gegenprobe - zwei PORT-Stempel, die sich gegenseitig ueberlappen:")
n = 0
for i in range(len(tu)):
    for j in range(i + 1, len(tu)):
        if tu[i][0] != tu[j][0]: continue
        if set(stempel(tu[i][2], tu[i][3], tu[i][4])) & set(stempel(tu[j][2], tu[j][3], tu[j][4])):
            n += 1
            P("   S%-2d r%-2d(%3d,%3d) kind%d  <->  r%-2d(%3d,%3d) kind%d"
              % (tu[i][0], tu[i][1], tu[i][2], tu[i][3], tu[i][4],
                 tu[j][1], tu[j][2], tu[j][3], tu[j][4]))
P("   Summe: %d Paare" % n)
open(r'analysis\karte_0901\_ueberlapp_out.txt', 'w', encoding='utf-8').write('\n'.join(OUT))
