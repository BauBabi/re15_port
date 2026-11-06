import struct
P = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/re15_port/shared_assets/PSX/PLD/"
edd = open(P+"PL00W01.EDD","rb").read(); emr = open(P+"PL00W01.EMR","rb").read()
nclips = struct.unpack_from("<H", edd, 2)[0] // 4
foff = struct.unpack_from("<H", emr, 2)[0]; w = struct.unpack_from("<I", emr, 4)[0]; esz = (w >> 18) * 4
print("clips", nclips, "esz", esz)
for c in range(nclips):
    cnt, off = struct.unpack_from("<HH", edd, 4*c)
    sp = []
    for f in range(cnt):
        kf = struct.unpack_from("<I", edd, off+4*f)[0] & 0xfff
        a = foff + esz*kf
        sp.append(struct.unpack_from("<6h", emr, a))
    if sp: print("clip %2d frames %3d first %s last %s" % (c, cnt, sp[0], sp[-1]))
