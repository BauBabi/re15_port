"""Zensus: Kurzform-Aot_set (0x2C/0x3B/0x50) mit Breite ODER Tiefe > 32767, je sce. Liest NUR den Arbeitsbaum."""
import glob, os, sys, collections
BAUM = r"C:/workspace/git/reAi_v2/.claude/worktrees/r38_wasser"
sys.path.insert(0, BAUM + "/re15_port/tools")
import aot_sce_census as A  # noqa: E402
PSX = BAUM + "/re15_port/shared_assets/PSX"
treffer = []
ops = collections.Counter()
gesamt = 0
for path in sorted(glob.glob(PSX + "/STAGE*/ROOM*.RDT")):
    room = os.path.basename(path)[4:8]
    data = open(path, "rb").read()
    if len(data) < 0x60:
        continue
    inst = []
    stats = dict(walked=0, total=0, residue=0, stops=[])
    for (sec, tag) in ((A.u32(data, 0x40), "main"), (A.u32(data, 0x44), "sub")):
        if sec == 0 or sec >= len(data):
            continue
        send = A.rdt_section_end(data, sec)
        for (o, e, idx) in A.section_regions(data, sec, send):
            A.walk(data, sec + o, sec + e, room, f"{tag}{idx:02d}", inst, stats)
    for i in inst:
        if not i.get("rect") or i.get("long"):
            continue
        ops[i["op"]] += 1
        gesamt += 1
        x, z, w, d = i["rect"]
        if (w & 0xffff) > 32767 or (d & 0xffff) > 32767:
            treffer.append((room, i["script"], i["off"], i["op"], i.get("sce"), w & 0xffff, d & 0xffff))
print("Kurzform-Saetze mit rect:", gesamt, dict((hex(k), v) for k, v in ops.items()))
for t in treffer:
    print("ROOM%s %s@%#x op=%#x sce=%s breite=%d tiefe=%d" % t)
print("Treffer:", len(treffer), "sce:", collections.Counter(t[4] for t in treffer))
