"""Nachbesserung 3 (N2): aus welchen Raeumen fuehren Tueren in einen Raum? (Erreichbarkeit von Kartenzustaenden)
tuer_quellen.py <ziel-hex-ohne-variante> ...   z.B. 121 122  -> alle Door_aot_set (0x3B, main00 + sub) der STAGE1-RDTs
mit Ziel ROOM<ziel>0/1. Walker/Laengen: scd_laengen.py (= G39abn2/tools/scd1210.py) (scd_vm.c scd_opcode_size_at); Ziel = Bytes q+8/q+9
(Stage-1, Raum), q = o + 14 (bzw. + 22 mit Bit 0x80 in Byte 3)."""
import struct, os, sys
BAUM = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '..'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scd_laengen import size, u16, u32   # Kopie von G39abn2/tools/scd1210.py (Abnahme 2), Laengen = scd_vm.c
P = os.path.join(BAUM, 're15_port', 'shared_assets', 'PSX', 'STAGE1') + os.sep
src = {}
for fn in sorted(os.listdir(P)):
    if not (fn.startswith('ROOM') and fn.endswith('.RDT')): continue
    b = open(P + fn, 'rb').read()
    if len(b) < 0x60: continue
    main, sub, ext = u32(b, 0x40), u32(b, 0x44), u32(b, 0x48)
    for (base, nxt) in ((main, sub), (sub, ext if ext > sub else len(b))):
        if not (0 < base < len(b)): continue
        n = u16(b, base) // 2
        offs = [u16(b, base + 2 * i) for i in range(n)]
        for i, of in enumerate(offs):
            o = base + of; en = base + offs[i + 1] if i + 1 < n else nxt
            while o < en and o < len(b):
                k = size(b, o)
                if k < 0: break
                if b[o] == 0x3B:
                    q = o + (22 if b[o + 3] & 0x80 else 14)
                    st, rm = b[q + 8], b[q + 9]
                    src.setdefault('%X%02X' % (st + 1, rm), set()).add((fn[4:8], '0x%05X' % o))
                o += k
for z in sys.argv[1:] or ['121', '122']:
    print('Ziel ROOM%s0/1:' % z, sorted(src.get(z.upper(), [])))
