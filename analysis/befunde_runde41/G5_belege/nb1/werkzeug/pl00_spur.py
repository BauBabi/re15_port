import struct
d = open(r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_g5auftritt/re15_port/shared_assets/PSX/PLD/PL00.PLD","rb").read()
diroff, n = struct.unpack_from("<II", d, 0)
dirs = struct.unpack_from("<%dI" % n, d, diroff)
print("dirs", [hex(x) for x in dirs])
edd = dirs[0]; emr = dirs[1]
nclips = struct.unpack_from("<H", d, edd+2)[0] // 4
foff = struct.unpack_from("<H", d, emr+2)[0]; w = struct.unpack_from("<I", d, emr+4)[0]; esz = (w >> 18) * 4
print("clips", nclips, "esz", esz, "foff", hex(foff))
for c in range(nclips):
    cnt, off = struct.unpack_from("<HH", d, edd+4*c)
    sp = []
    for f in range(cnt):
        kf = struct.unpack_from("<I", d, edd+off+4*f)[0] & 0xfff
        a = emr + foff + esz*kf
        sp.append(struct.unpack_from("<6h", d, a))
    if sp:
        print("clip %2d frames %3d first %s last %s" % (c, cnt, sp[0], sp[-1]))
