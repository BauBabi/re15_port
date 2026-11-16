"""Synthetische Gegenproben fuer R8h (3) aus dem 8g-Asset (nur Messkopien, nie Spiel-Asset):
  T = ein Texel im Feld C1 (Seite 0, u 60 v 53) auf einen anderen Palettenindex -> Texturbild nicht mehr bytegleich (neue Farbe am Schwanz)
  U = eine Ruecken-Flaeche (Mesh 17, erstes Viereck auf 487) auf ein 4x4-Feld der Seite 0 Zeile 493, das KEINE Original-Flaeche benutzt"""
import sys, numpy as np
T=r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel'
sys.path[:0]=[T+'/v8',T+'/v6kopf',T+'/v4']
import kopf_orig_bau as KB, adern_r4 as A4
src, dst_t, dst_u = sys.argv[1:4]
blob = open(src,'rb').read()
d,o,ms,tim = KB.teile(blob); t = KB.tim_teile(tim)
alt = int(t['pix'][53, 60]); t['pix'][53, 60] = (alt + 1) % 256
neu = KB.tim_packen(t); assert len(neu) == len(tim)
b = bytearray(blob); b[d[8]:d[8]+len(tim)] = neu; open(dst_t,'wb').write(b)
print('T: Texel Seite 0 u 60 v 53 Index %d -> %d' % (alt, (alt+1)%256))
nz = A4.nutzung(None, 480)[:, 0:128]
frei = None
for v0 in range(0, 252):
    for u0 in range(0, 124):
        if not nz[v0:v0+4, u0:u0+4].any():
            frei = (u0, v0); break
    if frei: break
d,o,ms,tim = KB.teile(blob)
m = ms[17]
for i, uv in enumerate(m['quv']):
    if ((uv[2] >> 6) & 0x1FF) == 487:
        u0, v0 = frei
        x = list(uv); x[0],x[1],x[3],x[4],x[6],x[7],x[9],x[10] = u0,v0,u0+3,v0,u0,v0+3,u0+3,v0+3
        x[2] = 493 << 6; x[5] = 128 + 0
        m['quv'][i] = x; print('U: Mesh 17 Viereck %d von 487 auf Seite 0 Zeile 493 u %d-%d v %d-%d (ungenutzt)' % (i, u0, u0+3, v0, v0+3)); break
open(dst_u,'wb').write(KB.packen(blob, d, ms, tim))
