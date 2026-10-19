"""B1-Daten je Raum: SCA-Huelle, Waende je Band, Tueren (Trigger, Ziel, Spawn, Yaw, Band), Treppen.
Quelle: die RDTs selbst (gen_map_zones.read_rdt, Format RE15_KNOWLEDGE.md / re15_rdt.h)."""
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', 're15_port', 'tools'))
import gen_map_zones as GZ
RAEUME = [0x1180, 0x1230, 0x1190, 0x11B0, 0x11C0, 0x11D0, 0x10A0, 0x1160, 0x1260]
if __name__ == '__main__':
    for rid in (list(map(lambda a: int(a, 16), sys.argv[1:])) or RAEUME):
        r = GZ.read_rdt(rid)
        if not r: print(f'ROOM{rid:04X}: fehlt'); continue
        sca, doors, stairs = r
        xs = [c[0] for c in sca] + [c[0] + c[2] for c in sca]; zs = [c[1] for c in sca] + [c[1] + c[3] for c in sca]
        bands = sorted(set(c[4] >> 4 for c in sca))
        print(f'== ROOM{rid:04X}: {len(sca)} Zellen, Huelle x {min(xs)}..{max(xs)} z {min(zs)}..{max(zs)}, Baender {bands}')
        for d in doors:
            print(f"   Tuer slot {d['slot']:2d} sce {d['sce']} -> ROOM{d['dest']:04X} Band {d['band']} Trigger ({d['rx']},{d['rz']},{d['rw']},{d['rd']}) "
                  f"Mitte ({d['lx']},{d['lz']}) Spawn ({d['nx']},{d['ny']},{d['nz']}) Yaw {d['yaw']}")
        for s in stairs:
            print(f"   Treppe Achse {s['axis']} Band {s['band']} n {s['count']} Mitte ({s['x']},{s['z']})")
