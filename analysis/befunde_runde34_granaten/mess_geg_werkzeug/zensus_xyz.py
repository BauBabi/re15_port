import os, sys, glob, struct
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as W
CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
want = set(int(x, 16) for x in sys.argv[1].split(','))
rooms = set(sys.argv[2].split(',')) if len(sys.argv) > 2 else None
for st in range(1, 7):
    for p in sorted(glob.glob(os.path.join(CD, 'STAGE%d' % st, 'ROOM*.RDT'))):
        name = os.path.basename(p)[4:8]
        if rooms and name not in rooms: continue
        d = open(p, 'rb').read()
        try: reg = W.regionen(d)
        except Exception: continue
        for (tag, idx), ops in sorted(reg.items(), key=lambda kv: str(kv[0])):
            for pc, op, sz in ops:
                if op == 0x44 and d[pc+2] in want:
                    x, y, z = struct.unpack_from('<hhh', d, pc+8)
                    dr = struct.unpack_from('<h', d, pc+16)[0]
                    print(f'ROOM{name} {tag}{idx} @0x{pc:04x} slot={d[pc+1]} typ=0x{d[pc+2]:02x} grid=0x{d[pc+3]:02x} persist=0x{d[pc+7]:02x} pos=({x},{y},{z}) dir={dr}')
