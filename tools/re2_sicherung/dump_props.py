#!/usr/bin/env python3
"""dump_props.py - alle Prop-Slots eines RDT auf einmal ausschneiden und rendern.

Aufruf: dump_props.py <ROOM....RDT> <zielordner> [--png]
"""
import sys, os
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rdt_props, md1_view

path, outdir = sys.argv[1], sys.argv[2]
do_png = "--png" in sys.argv
os.makedirs(outdir, exist_ok=True)
room = os.path.splitext(os.path.basename(path))[0]
d, n, tbl, props = rdt_props.parse(path)
print("%s nOmodel=%d Tabelle@0x%X" % (room, n, tbl))
for p in props:
    pre = os.path.join(outdir, "%s_prop%02d" % (room, p["slot"]))
    open(pre + ".tim", "wb").write(d[p["tim_off"]:p["tim_off"] + p["tim_size"]])
    open(pre + ".md1", "wb").write(d[p["md1_off"]:p["md1_off"] + p["md1_size"]])
    md1 = d[p["md1_off"]:p["md1_off"] + p["md1_size"]]
    try:
        h, tris, quads = md1_view.geometry(md1)
        allv = [v for f, _ in tris + quads for v in f]
        bb = ("x %d..%d y %d..%d z %d..%d" % (
            min(v[0] for v in allv), max(v[0] for v in allv),
            min(v[1] for v in allv), max(v[1] for v in allv),
            min(v[2] for v in allv), max(v[2] for v in allv))) if allv else "leer"
        print("  Slot %2d  MD1 @0x%06X %5dB  %d Tri %d Quad  %s" % (
            p["slot"], p["md1_off"], p["md1_size"], len(tris), len(quads), bb))
        if do_png:
            md1_view.render(md1, pre + ".tim", pre + ".png", 320)
    except Exception as e:
        print("  Slot %2d  MD1-Parse fehlgeschlagen: %s" % (p["slot"], e))
