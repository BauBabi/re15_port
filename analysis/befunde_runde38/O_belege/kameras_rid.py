import struct, sys
base = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_montage/re15_port/shared_assets/PSX/STAGE1/'
for room, cuts in (('1130',[0]),('1040',[1]),('1030',[6,7,12]),('11C0',[13])):
    d = open(base + 'ROOM%s.RDT' % room, 'rb').read()
    ncut = d[1]
    cs = struct.unpack_from('<I', d, 0x24)[0]
    print('ROOM%s nCut=%d RDT+0x24 camera_start=0x%X' % (room, ncut, cs))
    for c in cuts:
        o = cs + c*32
        flag, fov, px, py, pz, tx, ty, tz, pri = struct.unpack_from('<HHiiiiiiI', d, o)
        raw = d[o+4:o+28].hex(' ')
        print('  Cut %2d @file 0x%05X: flag=%d fov=%d pos=(%d,%d,%d) target=(%d,%d,%d)  bytes+4..+27: %s' % (c, o, flag, fov, px, py, pz, tx, ty, tz, raw))
