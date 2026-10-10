import subprocess, sys, glob, os, re, struct
root = sys.argv[1]; want_stage = int(sys.argv[2]); want_room = int(sys.argv[3], 16)
tool = os.path.join(root, "re15_port/tools/scd_dump_room.py")
for f in sorted(glob.glob(os.path.join(root, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    out = subprocess.run([sys.executable, "-I", tool, f], capture_output=True, text=True, timeout=60).stdout
    for line in out.splitlines():
        if "Door_aot_set" not in line: continue
        m = re.search(r"Door_aot_set\s+((?:[0-9a-f]{2} ){32})", line)
        if not m: continue
        b = bytes(int(x, 16) for x in m.group(1).split())
        nx, ny, nz, nd = struct.unpack_from('<hhhh', b, 14)
        st, rm, cut, nfl = b[22], b[23], b[24], b[25]
        if st == want_stage and rm == want_room:
            print(os.path.basename(f), line.split()[0], "slot=%d floor=%d -> stage=%d room=%02X cut=%d nfloor=%d next=(%d,%d,%d) dir=%d" % (b[1], b[4], st, rm, cut, nfl, nx, ny, nz, nd))
