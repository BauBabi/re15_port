# Nachbesserung 2: braucht flut1210.pgm im Arbeitsordner = probe_r39_karten flut 1210 0 -25500 -21700 450 100 flut1210.pgm
# Nachbildung des Kartenpfads fuer ROOM1210 Zone 0 (Zeile 177/140/2496/2250, Klemmung rect 3 reserve 4) an jedem
# Rasterpunkt des Flut-Bilds flut1210.pgm; zaehlt Punkte mit r3-Texel 0 (wie probe markerflaeche auf_zone).
import sys, struct, collections
sys.path.insert(0, '.')
from proj import px, py
d = open('flut1210.pgm', 'rb').read()
hdr = d.split(b'\n', 4)
kom = hdr[1].decode(); x0 = int(kom.split()[2]); z0 = int(kom.split()[4]); st = int(kom.split()[6])
nx, nz = map(int, hdr[2].split()); body = hdr[4]
pix = open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_karten/re15_port/shared_assets/PSX/DATA/MAP02.PIX', 'rb').read()
def tex(u, v): b = pix[v*128 + u//2]; return (b & 15) if u % 2 == 0 else (b >> 4)
RX, RY, RW, RH, U, V = 187, 70, 64, 80, 96, 48
kammer = lambda x, z: x <= -22128 and -26696 <= z <= -19898 and x >= -27358
aus = collections.Counter(); bsp = {}
n = 0
for gz in range(nz):
    for gx in range(nx):
        if not body[gz*nx + gx]: continue
        wx = x0 + gx*st; wz = z0 + gz*st; n += 1
        if kammer(wx, wz): continue
        mx = min(max(px(wx), RX+4), RX+RW-4); my = min(max(py(wz), RY+4), RY+RH-4)
        if tex(U + mx - RX, V + my - RY) == 0:
            aus[(mx, my)] += 1; bsp.setdefault((mx, my), (wx, wz))
print('Punkte', n, 'Zone 0 ausserhalb', sum(aus.values()))
for k, v in sorted(aus.items()): print(' Marker', k, 'Punkte', v, 'z.B. Welt', bsp[k])
