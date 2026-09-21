#!/usr/bin/env python3
"""prop_census.py - ALLE Prop-Modelle aller RE2-Raeume ausschneiden, entdoppeln, rendern.

Das ist die SOLLZAHL fuer die Frage "gibt es ein Modell von X": jedes
Objektmodell, das RE2 (Leon-Disc) in einem Raum aufstellen kann, steht in der
Modelltabelle RDT+0x30 seines Raums (nOmodel = RDT+0x02). Wer alle Tabellen
aller RDTs liest, hat alle Prop-Modelle des Spiels.

Zusaetzlich: die Gegner-/Figurenmodelle in COMMON/BIN/EM*.  Die werden hier
NICHT erfasst — sie sind Figuren, keine Einrichtung.

Aufruf:
    prop_census.py <zielordner> [--re15] [--no-png]
        ohne --re15: RE2-Leon  info/re2leon/PL0/RDT/ROOM*.RDT
        mit  --re15: RE1.5     info/Re1.5/PSX/STAGE*/ROOM*.RDT
"""
import sys, os, glob, hashlib, collections
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import rdt_props, md1_view
from PIL import Image, ImageDraw

REPO = os.path.abspath(os.path.join(HERE, "..", ".."))

out = sys.argv[1]
do_png = "--no-png" not in sys.argv
os.makedirs(out, exist_ok=True)

if "--re15" in sys.argv:
    pattern = os.path.join(REPO, "info", "Re1.5", "PSX", "STAGE*", "ROOM*.RDT")
else:
    pattern = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT")

seen = {}           # md5(md1) -> dict
placements = 0
rooms = sorted(glob.glob(pattern))
for path in rooms:
    room = os.path.splitext(os.path.basename(path))[0]
    try:
        d, n, tbl, props = rdt_props.parse(path)
    except Exception as e:
        print("!! %s: %s" % (room, e)); continue
    for p in props:
        if p["md1_size"] <= 16:
            continue
        md1 = d[p["md1_off"]:p["md1_off"] + p["md1_size"]]
        tim = d[p["tim_off"]:p["tim_off"] + p["tim_size"]]
        h = hashlib.md5(md1).hexdigest()[:12]
        placements += 1
        if h not in seen:
            seen[h] = dict(md1=md1, tim=tim, rooms=[], size=len(md1))
        seen[h]["rooms"].append("%s#%d" % (room, p["slot"]))

print("Raeume: %d   Platzierungen: %d   VERSCHIEDENE Prop-MD1: %d"
      % (len(rooms), placements, len(seen)))

order = sorted(seen.items(), key=lambda kv: (-len(kv[1]["rooms"]), kv[0]))
idx = {}
for i, (h, v) in enumerate(order):
    pre = os.path.join(out, "prop%03d_%s" % (i, h))
    open(pre + ".md1", "wb").write(v["md1"])
    open(pre + ".tim", "wb").write(v["tim"])
    idx[h] = (i, pre)

with open(os.path.join(out, "_index.txt"), "w") as f:
    f.write("# %d Raeume, %d Platzierungen, %d verschiedene Prop-MD1\n"
            % (len(rooms), placements, len(seen)))
    for h, v in order:
        i = idx[h][0]
        f.write("prop%03d  md5=%s  %6d B  %2d Raeume  %s\n"
                % (i, h, v["size"], len(v["rooms"]), " ".join(v["rooms"][:12])))
print("Index:", os.path.join(out, "_index.txt"))

if not do_png:
    sys.exit(0)

S, COLS, PER = 176, 8, 48
sheet_n = 0
tmp = os.path.join(out, "_t.png")
for start in range(0, len(order), PER):
    chunk = order[start:start + PER]
    rows = (len(chunk) + COLS - 1) // COLS
    img = Image.new("RGB", (S * COLS, (S + 14) * rows), (12, 12, 16))
    dr = ImageDraw.Draw(img)
    for j, (h, v) in enumerate(chunk):
        i, pre = idx[h]
        try:
            md1_view.render(v["md1"], pre + ".tim", tmp, S, 0.7, 0.45)
            r, c = divmod(j, COLS)
            img.paste(Image.open(tmp), (c * S, r * (S + 14) + 14))
            dr.text((c * S + 2, r * (S + 14) + 2),
                    "%d x%d %dB" % (i, len(v["rooms"]), v["size"]), fill=(240, 240, 240))
        except Exception as e:
            r, c = divmod(j, COLS)
            dr.text((c * S + 2, r * (S + 14) + 20), "%d FEHLER" % i, fill=(255, 90, 90))
    p = os.path.join(out, "_sheet%02d.png" % sheet_n)
    img.save(p); print("Bogen:", p, img.size)
    sheet_n += 1
if os.path.exists(tmp):
    os.remove(tmp)
