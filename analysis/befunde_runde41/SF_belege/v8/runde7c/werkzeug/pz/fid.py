"""Pruefer R3: Flaechen-Kennlauf. Wie mark.bauen, zusaetzlich bekommen ausgewaehlte Mesh-8-Flaechen eigene Zeilen/Farben.
Zeilen 488-491 sind nach mark.bauen unbenutzt (alle Flaechen haengen auf 480-486) und haben keinen Eintrag 0 (mark.py Kopf).
  486 gelb   0x03FF  tf79, tf80 (neue Haelften von qf17)
  488 weiss  0x7FFF  qf17
  489 orange 0x021F  uebrige Flaechen mit Ecke 17
  490 violett 0x4010 uebrige Flaechen mit Ecke 30
  491 grau   0x4210  uebrige Flaechen mit Ecke 46
Aufruf: python fid.py IN.EMD OUT.EMD"""
import sys, struct
import mark, pl
FAR = {486: 0x03FF, 488: 0x7FFF, 489: 0x021F, 490: 0x4010, 491: 0x4210}
blob = open(sys.argv[1], 'rb').read()
lm, kl = mark.bauen(blob)
b = bytearray(lm); E = pl.EMD(blob); m = E.meshes[8]
tf, tu, qf, qu = mark.offs(b, 8)
def zeile(kind, k, vs):
    if kind == 't' and k in (79, 80): return 486
    if kind == 'q' and k == 17: return 488
    if 17 in vs: return 489
    if 30 in vs: return 490
    if 46 in vs: return 491
    return None
n = {}
for kind, F, U in (('t', m['tf'], tu), ('q', m['qf'], qu)):
    for k, f in enumerate(F):
        z = zeile(kind, k, f[1::2])
        if z: mark.setz(b, U[k], z); n.setdefault(z, []).append(kind + str(k))
do = mark.u32(b, 0); tim = mark.u32(b, do + 4 * 8); cw, ch = struct.unpack_from('<2H', b, tim + 8 + 8)
for r, col in FAR.items():
    for k in range(cw):
        o = tim + 20 + 2 * cw * (r - 480) + 2 * k; v = struct.unpack_from('<H', b, o)[0]
        if v: struct.pack_into('<H', b, o, (v & 0x8000) | col)
open(sys.argv[2], 'wb').write(bytes(b)); print(n)
