import subprocess, sys, glob, os, re, struct
root = sys.argv[1]; targets = sys.argv[2].split(",")
tool = os.path.join(root, "re15_port/tools/scd_dump_room.py")
dumps = {}
for f in sorted(glob.glob(os.path.join(root, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    dumps[os.path.basename(f)[:-4]] = subprocess.run([sys.executable, "-I", tool, f], capture_output=True, text=True, timeout=60).stdout
for t in targets:
    st = int(t[0]) - 1; rm = int(t[1:3], 16)
    ys = set(); srcs = []
    for name, out in dumps.items():
        for line in out.splitlines():
            m = re.search(r"Door_aot_set\s+((?:[0-9a-f]{2} ){32})", line)
            if not m: continue
            b = bytes(int(x, 16) for x in m.group(1).split())
            nx, ny, nz, nd = struct.unpack_from('<hhhh', b, 14)
            if b[22] == st and b[23] == rm:
                ys.add(ny); srcs.append(name)
    out = dumps.get("ROOM" + t, "")
    sces = sorted(set(int(x) for x in re.findall(r"sce=(\d+)", out)))
    print("ROOM%s: Tueren hinein next_y=%s (aus %d Records), sce im Raum=%s, Treppen(12/13)=%s" % (t, sorted(ys), len(srcs), sces, [s for s in sces if s in (12, 13)]))
