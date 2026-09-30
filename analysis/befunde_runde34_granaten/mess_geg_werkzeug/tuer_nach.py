# tuer_nach.py <stage> <room-hex-2> : alle Door_aot_set-Saetze, deren Ziel (Stage, Raum) passt -> Quelle, Slot, Ziel-Lage, Ziel-Cut
import os, sys, glob, struct
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', '..'))
sys.path.insert(0, os.path.join(REPO, 're15_port', 'tools'))
import scd_walk_lib as W
CD = os.path.join(REPO, 're15_port', 'shared_assets', 'PSX')
zst = int(sys.argv[1]); zr = int(sys.argv[2], 16)
for st in range(1, 7):
    for p in sorted(glob.glob(os.path.join(CD, 'STAGE%d' % st, 'ROOM*.RDT'))):
        d = open(p, 'rb').read()
        try: reg = W.regionen(d)
        except Exception: continue
        for (tag, idx), ops in reg.items():
            for pc, op, sz in ops:
                if op != 0x3B: continue
                vier = (d[pc+3] & 0x80) != 0
                nl = pc + (22 if vier else 14)
                nx, ny, nz, nd = struct.unpack_from('<hhhh', d, nl)
                stg, rm, cut = d[nl+8], d[nl+9], d[nl+10]
                if stg == zst and rm == zr:
                    rx, rz, rw, rd = struct.unpack_from('<hhhh', d, pc+6)
                    print('%s %s%s @0x%04x slot=%d sce=%d rect=(%d,%d,%d,%d) -> Ziel (%d,%d,%d) dir %d Cut %d' % (os.path.basename(p)[:-4], tag, idx, pc, d[pc+1], d[pc+2], rx, rz, rw, rd, nx, ny, nz, nd, cut))
