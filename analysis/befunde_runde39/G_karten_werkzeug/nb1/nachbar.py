# M3: je Kachel-Korrektur (Wand -> Koerper/durchsichtig) die Rechtecke, deren Wandtexel die ERSATZLINIE tragen.
import sys, struct, numpy as np
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/analysis/befunde_runde39/G_karten_werkzeug')
from schwenk import rects
from kachel import pix
KORR = [
 (2,0, 7,76,1,10,1), (2,1,35,32,1,16,1), (2,2,16,49,1,8,1), (2,2,16,67,1,11,1), (2,3,51,1,1,34,1),
 (2,4,66,20,1,1,1), (2,7,0,1,1,15,1), (2,7,15,1,1,15,1), (2,8,0,1,1,15,1), (2,8,15,1,1,15,1), (2,9,9,1,1,8,1),
 (3,5,40,7,1,1,0),
 (4,4,0,1,1,38,0),(4,4,1,1,1,38,4),(4,4,8,40,1,38,0),(4,4,9,39,1,39,4),
 (1,3,0,2,1,5,1),(1,4,0,1,1,16,1),(1,5,0,1,1,16,1),
]
def tex_at(T, R, ri, X, Y):
    x, y, w, h, u, v = R[ri]
    if not (x <= X < x + w and y <= Y < y + h): return None
    return int(T[(v + Y - y) & 255, (u + X - x) & 255])
for (p, ri, c, r, w, h, neu) in KORR:
    R = rects(p); T = pix(p)
    x, y, rw, rh, u, v = R[ri]
    pts = [(x + c + i, y + r + j) for j in range(h) for i in range(w)]
    alt = [tex_at(T, R, ri, X, Y) for X, Y in pts]
    print(f'Blatt {p} r{ri} ({x},{y},{rw},{rh}) Spalte {c} Zeile {r} {w}x{h} -> {neu}: Bild x{x+c}..{x+c+w-1} y{y+r}..{y+r+h-1}, alt-Texel {sorted(set(alt))}')
    if neu == 4: continue
    # Ersatzlinie: die Nachbarspalte/-zeile links/rechts/oben/unten, Wand (4) in einem ANDEREN Rechteck
    for dx, dy in ((-1,0),(1,0),(0,-1),(0,1)):
        traeger = {}
        for X, Y in pts:
            for rj in range(len(R)):
                if rj == ri: continue
                t = tex_at(T, R, rj, X + dx, Y + dy)
                if t == 4: traeger.setdefault(rj, []).append(Y if dx else X)
        if traeger:
            print(f'   Ersatz ({dx:+d},{dy:+d}): ' + ', '.join(f'r{rj} {R[rj][:4]} deckt {len(v)}/{len(pts)} ({min(v)}..{max(v)})' for rj, v in traeger.items()))
