# Door_aot_set (0x3B) aller RDTs einer Stage: Ziel (stage,room,cut) und Position; nur lesen.
import sys, os, struct, glob, subprocess
TOOL = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/tools/scd_dump_room.py"
root = sys.argv[1]; muster = sys.argv[2] if len(sys.argv) > 2 else "ROOM*.RDT"
for p in sorted(glob.glob(os.path.join(root, muster))):
    txt = subprocess.run([sys.executable, TOOL, p], capture_output=True, text=True).stdout
    for line in txt.splitlines():
        if "Door_aot_set" in line:
            hexs = line.split("Door_aot_set")[1].split("slot=")[0].split()
            b = bytes(int(h, 16) for h in hexs)
            off = line.split()[0]
            nl = b[22:] if (b[3] & 0x80) else b[14:]
            x, z, w, dd = struct.unpack_from("<hhhh", b, 6)
            nx, ny, nz, ndir = struct.unpack_from("<hhhh", nl, 0)
            print(os.path.basename(p), off, "slot", b[1], "sce", b[2], "rect", (x, z, w, dd), "-> stage", nl[8], "room %02X" % nl[9], "cut", nl[10], "pos", (nx, ny, nz), "dir", ndir)
