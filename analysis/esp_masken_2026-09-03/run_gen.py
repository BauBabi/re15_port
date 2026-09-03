"""Treiber: erzeugt die fehlenden Maskensektionen fuer alle Cuts mit Atlas ohne Geometrie."""
import os, sys, glob, struct, json
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from q1_atlas_ist_bg import atlas_rgb, bg_rgb, TBL, u32, u16, ROOT
from q3b_tiefe import groups_masks
from gen_masken import build_cut, pack_section

OUTDIR = "build/masken"
os.makedirs(OUTDIR, exist_ok=True)
report = []
tot_cuts = tot_masks = 0
for rdt in sorted(glob.glob(os.path.join(ROOT,"STAGE*","ROOM*.RDT"))):
    name = os.path.basename(rdt)[:-4]; rid = int(name[4:],16)
    stage = (rid>>12) & 0xF
    if stage not in TBL: continue
    d = open(rdt,'rb').read()
    if len(d) < 0x60: continue
    nCut = d[1]; cam = u32(d,0x24)
    if not cam or cam+32*nCut > len(d): continue
    sections = {}
    for c in range(nCut):
        if groups_masks(d, u32(d, cam+c*32+0x1C)):      # Original hat schon Masken
            continue
        r = build_cut(d, cam, stage, rid, c)
        if not r: continue
        g, m = r
        sections[c] = pack_section(g, m)
        report.append((name, c, len(g), len(m)))
        tot_cuts += 1; tot_masks += len(m)
    if not sections: continue
    # Seitendaten je Raum: Magic, Version, nCut, Offsettabelle, dann die Sektionen
    body = b""; offs = [0]*nCut
    head = 4+4+4+4*nCut
    for c in sorted(sections):
        offs[c] = head + len(body); body += sections[c]
    blob = b"R15M" + struct.pack("<II", 1, nCut) + struct.pack("<%dI"%nCut, *offs) + body
    open(os.path.join(OUTDIR, "%s.MSK"%name), "wb").write(blob)

print("Cuts mit erzeugter Geometrie: %d | Masken gesamt: %d"%(tot_cuts, tot_masks))
by = {}
for n,c,g,m in report: by.setdefault(n, []).append((c,m))
print("Raeume: %d"%len(by))
for n in sorted(by)[:16]:
    print("   %-9s %2d Cuts, %3d Masken"%(n, len(by[n]), sum(m for _,m in by[n])))
json.dump(report, open(os.path.join(OUTDIR,"_report.json"),"w"))
