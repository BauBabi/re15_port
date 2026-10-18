# Projektion Welt -> Kartenpixel mit der Formel FUN_800473f8 (@0x8004741c-0x80047528) wie re15_map_zone_marker
import sys
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/analysis/befunde_runde39/G_karten_werkzeug')
from zeile_fit import tdiv
def t(w, s): return tdiv((((w + 32000) * 10 * s) >> 20) + 5, 10)
def proj(x, z, ox, oy, sx, sy, fx, fz):
    px = (-t(x, sx) if fx else t(x, sx)) + ox
    py = (t(z, sy) if fz else -t(z, sy)) + oy
    return px, py
if __name__ == '__main__':
    ox, oy, sx, sy, fx, fz = map(int, sys.argv[1:7])
    pts = sys.argv[7:]
    for i in range(0, len(pts), 2):
        x, z = int(pts[i]), int(pts[i+1])
        print((x, z), '->', proj(x, z, ox, oy, sx, sy, fx, fz))
