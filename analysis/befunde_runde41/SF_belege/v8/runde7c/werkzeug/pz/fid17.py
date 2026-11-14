"""Flaechen-Kennung der Flaechen an Ecke 17 (Muster fid.py des Pruefers R3): t49 orange 489, t53 violett 490, t8 grau 491, t60 weiss 488,
q15 gelb 486; Rest wie mark.bauen.  python fid17.py IN OUT"""
import sys, struct
import mark, pl
FAR = {486: 0x03FF, 488: 0x7FFF, 489: 0x021F, 490: 0x4010, 491: 0x4210}
WAHL = {('t', 49): 489, ('t', 53): 490, ('t', 8): 491, ('t', 60): 488, ('q', 15): 486}
blob = open(sys.argv[1], 'rb').read()
lm, kl = mark.bauen(blob)
b = bytearray(lm); E = pl.EMD(blob)
tf, tu, qf, qu = mark.offs(b, 8)
for (kind, k), z in WAHL.items():
    mark.setz(b, (tu if kind == 't' else qu)[k], z)
do = mark.u32(b, 0); tim = mark.u32(b, do + 4 * 8); cw, ch = struct.unpack_from('<2H', b, tim + 8 + 8)
for r, col in FAR.items():
    for k in range(cw):
        o = tim + 20 + 2 * cw * (r - 480) + 2 * k; v = struct.unpack_from('<H', b, o)[0]
        if v: struct.pack_into('<H', b, o, (v & 0x8000) | col)
open(sys.argv[2], 'wb').write(bytes(b)); print('ok')
