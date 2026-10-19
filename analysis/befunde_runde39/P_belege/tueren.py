import sys, os, glob, struct
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041/re15_port/tools')
import scd_walk_lib as L
base = r'C:/workspace/git/reAi_v2/.claude/worktrees/r39_johnruf1041/re15_port/shared_assets/PSX'
# list all Door_aot_set (and Aot_reset to sce 2) in given rooms, or all doors targeting a room
mode = sys.argv[1]
arg = sys.argv[2]
for p in sorted(glob.glob(base + '/STAGE*/ROOM*.RDT')):
    nm = os.path.basename(p)[4:8]
    d = open(p, 'rb').read()
    try:
        regs = L.regionen(d)
    except Exception:
        continue
    for (tag, idx), ops in sorted(regs.items()):
        for pc, op, sz in ops:
            if op != 0x3B:
                continue
            slot = d[pc+1]; sat = d[pc+3]
            x, z, w, dd = struct.unpack_from('<hhHH', d, pc+6)
            nx, ny, nz, ndir = struct.unpack_from('<hhhh', d, pc+14)
            st, rm, cut, nfl = d[pc+22], d[pc+23], d[pc+24], d[pc+25]
            tgt = '%d%02X%d' % (st+1, rm, 0)
            line = 'ROOM%s %s%02d @0x%05X slot %d -> stage %d room 0x%02X cut %d (ROOM%d%02Xx) rect(%d,%d,%d,%d) next(%d,%d,%d) dir %d bytes+22..31 %s' % (nm, tag, idx, pc, slot, st, rm, cut, st+1, rm, x, z, w, dd, nx, ny, nz, ndir, d[pc+22:pc+32].hex(' '))
            if mode == 'from' and nm.upper() in arg.upper().split(','):
                print(line)
            if mode == 'to':
                s_, r_ = arg.split(':')
                if st == int(s_, 0) and rm == int(r_, 0):
                    print(line)
