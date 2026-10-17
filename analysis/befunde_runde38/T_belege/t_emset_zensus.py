# Runde 38 Spur T: Zensus aller Sce_em_set (SCD 0x44, 20 B) in den RDTs einer Stage.
# Aufruf: python t_emset_zensus.py <stage-praefix z.B. 4> [typ-hex]
import sys, struct, importlib.util, os, glob
BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
spec = importlib.util.spec_from_file_location("aot", os.path.join(BAUM, "re15_port", "tools", "aot_sce_census.py"))
m = importlib.util.module_from_spec(spec); spec.loader.exec_module(m)
ROOT = os.path.join(BAUM, "re15_port", "shared_assets", "PSX")
orig_collect = m.collect
def coll(data, pc, op, room, script, installs):
    if op == 0x44:
        installs.append(dict(room=room, script=script, off=pc, op=op, raw=bytes(data[pc:pc+20])))
    else:
        orig_collect(data, pc, op, room, script, installs)
m.collect = coll
stage = sys.argv[1] if len(sys.argv) > 1 else "4"
want = int(sys.argv[2], 16) if len(sys.argv) > 2 else None
for f in sorted(glob.glob(os.path.join(ROOT, "STAGE" + stage, "ROOM*.RDT"))):
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
        if want is not None and typ != want: continue
        x, y, z = struct.unpack_from('<3h', r, 8); rot = struct.unpack_from('<h', r, 16)[0]
        print("%s %s @0x%05X slot %d typ 0x%02x grid 0x%02x p4 0x%02x p5 0x%02x p6 0x%02x p7 0x%02x pos(%d,%d,%d) rot %d raw %s" %
              (room, e["script"], e["off"], r[1], typ, g, r[4], r[5], r[6], r[7], x, y, z, rot, r.hex()))
