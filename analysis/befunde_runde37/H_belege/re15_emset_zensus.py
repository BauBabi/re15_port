import sys, struct, importlib.util, os, glob, collections
spec = importlib.util.spec_from_file_location("aot", r"C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv/re15_port/tools/aot_sce_census.py")
m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
ROOT = r"C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv/re15_port/shared_assets/PSX"
rows = []
# eigener Walk: collect() nimmt nur AOTs; Sce_em_set (0x44, 20 B) selbst aufsammeln
orig_collect = m.collect
def coll(data, pc, op, room, script, installs):
    if op == 0x44:
        installs.append(dict(room=room, script=script, off=pc, op=op, raw=bytes(data[pc:pc+20])))
    else:
        orig_collect(data, pc, op, room, script, installs)
m.collect = coll
for f in sorted(glob.glob(os.path.join(ROOT, "STAGE*", "ROOM*.RDT"))):
    room = os.path.basename(f)[4:8]
    data = open(f, "rb").read()
    if len(data) < 0x48: continue
    inst = []; stats = dict(walked=0, total=0, residue=0, stops=[])
    main_s, sub_s = m.u32(data, 0x40), m.u32(data, 0x44)
    for (sec, tag) in ((main_s, "main"), (sub_s, "sub")):
        if sec == 0 or sec >= len(data): continue
        send = m.rdt_section_end(data, sec)
        for (o, e, idx) in m.section_regions(data, sec, send):
            m.walk(data, sec + o, sec + e, room, f"{tag}{idx:02d}", inst, stats)
    for e in inst:
        if e["op"] != 0x44: continue
        r = e["raw"]; typ = r[2]; g = r[3]
        x, y, z = struct.unpack_from('<3h', r, 8); rot = struct.unpack_from('<h', r, 16)[0]
        rows.append((room, e["script"], e["off"], r[1], typ, g, r[6], r[7], x, y, z, rot))
mode = sys.argv[1] if len(sys.argv) > 1 else "sum"
Z = {0x10, 0x11, 0x12, 0x13, 0x16, 0x18}
if mode == "sum":
    c = collections.Counter(); cn = collections.Counter()
    for r in rows:
        if r[4] in Z:
            c[r[5]] += 1; cn[r[5] & 0x0f] += 1
    print("RE1.5 Zombie-Sce_em_set (Typ 0x10/11/12/13/16/18), grid-Byte pc[3]:")
    for g in sorted(c): print("  0x%02x: %d" % (g, c[g]))
    print("nach Nibble (grid & 0x0f):", dict(sorted(cn.items())), "gesamt", sum(c.values()))
else:
    for r in rows:
        if r[4] in Z and (mode == "all" or r[0].startswith(mode)):
            print("%s %s @0x%05X slot %d typ 0x%02x grid 0x%02x p6 0x%02x p7 0x%02x pos(%d,%d,%d) rot %d" % r)
