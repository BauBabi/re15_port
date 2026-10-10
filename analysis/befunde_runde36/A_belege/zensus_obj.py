import subprocess, sys, glob, os, re
root = sys.argv[1]
tool = os.path.join(root, "re15_port/tools/scd_dump_room.py")
rows = []
for f in sorted(glob.glob(os.path.join(root, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    try:
        out = subprocess.run([sys.executable, "-I", tool, f], capture_output=True, text=True, timeout=60).stdout
    except Exception as e:
        continue
    room = os.path.basename(f)[:-4]
    for line in out.splitlines():
        if "Obj_model_set" not in line: continue
        m = re.search(r"Obj_model_set\s+((?:[0-9a-f]{2} ){8})", line)
        if not m: continue
        b = m.group(1).split()
        obj, typ, band, pc6, pc7 = int(b[1],16), int(b[2],16), int(b[4],16), int(b[6],16), int(b[7],16)
        flags = (pc6 | (pc7 << 8)) | 1
        if typ == 4 or (flags & 0x100):
            sec = "sub" if "sub" in line else "main"
            rows.append((room, obj, typ, band, flags))
seen = set()
for r in rows:
    if r in seen: continue
    seen.add(r)
    print("%s obj=%d type=%d band=%d flags=0x%04X %s" % (r[0], r[1], r[2], r[3], r[4], "KLETTERBIT" if r[4] & 0x100 else "ohne-0x100"))
