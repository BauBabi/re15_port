import subprocess, sys, glob, os, re, struct
root = sys.argv[1]
tool = os.path.join(root, "re15_port/tools/scd_dump_room.py")
tot = 0; null = 0; klein = []
for f in sorted(glob.glob(os.path.join(root, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    out = subprocess.run([sys.executable, "-I", tool, f], capture_output=True, text=True, timeout=60).stdout
    for line in out.splitlines():
        m = re.search(r"Obj_model_set\s+((?:[0-9a-f]{2} ){34})", line)
        if not m: continue
        b = bytes(int(x, 16) for x in m.group(1).split())
        hx, hy, hz = struct.unpack_from('<HHH', b, 28)
        tot += 1
        if hx == 0 and hz == 0: null += 1; continue
        if hx < 112 or hz < 112:
            klein.append((os.path.basename(f)[:-4], b[1], b[2], b[4], (b[6] | b[7] << 8) | 1, hx, hy, hz))
print("Obj_model_set-Records gesamt:", tot, " davon h(x,z)=0:", null, " davon 0<hx<112 oder 0<hz<112 (Achse invertiert):", len(klein))
for k in klein: print("  %s obj=%d typ=%d band=%d flags=0x%04X h=(%d,%d,%d)" % k)
