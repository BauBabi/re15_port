"""SCA-Zellen eines Raums mit DATEI-OFFSET (RDT +0x20 = Zeiger, Kopf 24 B, 12 B je Zelle:
u16 width, u16 density, s16 x, s16 z, u8 type, u8 u0, u8 u1, u8 floor - re15_rdt.h re15_sca_entry_t).
sca_liste.py <hexraum> [band] -> je Zelle: @Offset x..x+w z..z+d typ u0 floor (Dubletten markiert)."""
import sys, os, struct
ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..')
def zellen(rid):
    p = os.path.join(ROOT, 're15_port', 'shared_assets', 'PSX', f'STAGE{rid >> 12}', f'ROOM{rid:04X}.RDT')
    d = open(p, 'rb').read()
    s = struct.unpack_from('<I', d, 0x20)[0]
    n = sum(struct.unpack_from('<5I', d, s + 4))
    out = []
    for i in range(n):
        o = s + 24 + 12 * i
        w, dep, x, z = struct.unpack_from('<HHhh', d, o)
        typ, u0, u1, flr = struct.unpack_from('<BBBB', d, o + 8)
        out.append(dict(off=o, x=x, z=z, w=w, d=dep, typ=typ, u0=u0, u1=u1, flr=flr))
    return s, out
if __name__ == '__main__':
    rid = int(sys.argv[1], 16); band = int(sys.argv[2]) if len(sys.argv) > 2 else None
    s, zs = zellen(rid)
    print(f'ROOM{rid:04X}: SCA @0x{s:X}, {len(zs)} Zellen')
    gesehen = {}
    for c in zs:
        if band is not None and (c['flr'] >> 4) != band: continue
        k = (c['x'], c['z'], c['w'], c['d'], c['typ'], c['flr'])
        dup = f" (= @0x{gesehen[k]:X})" if k in gesehen else ''
        gesehen.setdefault(k, c['off'])
        print(f"  @0x{c['off']:05X}: x {c['x']:6d}..{c['x']+c['w']:6d} z {c['z']:6d}..{c['z']+c['d']:6d} typ {c['typ']} u0 {c['u0']:02x} floor {c['flr']:02x}{dup}")
