import sys, glob, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "..", "re15_port", "tools", "r34n_d"))
import rbj_zensus as rz
root = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "..", "re15_port", "shared_assets", "PSX")
files = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
n_raum = n_rbj = n_diff = 0
for f in files:
    n_raum += 1
    d = open(f, "rb").read()
    recs = rz.records(d)
    if not recs: continue
    n_rbj += 1
    marks = [r.get("marker") for r in recs]
    if any(m is None for m in marks):
        print(os.path.basename(f), "Trailerfehler", marks); continue
    # Original FUN_8001b3f8: a3 kumulativ, spaetere Records ueberschreiben
    orig = {}; a3 = 0
    for ri, m in enumerate(marks):
        if m & 1: orig["P"] = ri
        a0 = m >> 1
        while a0:
            if a0 & 1: orig[a3] = ri
            a0 >>= 1; a3 = (a3 + 1)
    # Port rbj_resolve_slot: Gegner i (Slot 1+i) -> erster Record mit Bit (1+i); Spieler: Record 0 (re15_apply_room_cinematic)
    port = {}
    for i in range(31):
        for ri, m in enumerate(marks):
            if m & (1 << (1 + i)): port[i] = ri; break
    e_orig = {k: v for k, v in orig.items() if k != "P"}
    p_orig = orig.get("P", -1); p_port = 0
    if e_orig != port or p_orig != p_port:
        n_diff += 1
        print(os.path.basename(f), "Marker", [hex(m) for m in marks], "Spieler orig rec%d port rec%d" % (p_orig, p_port),
              "Gegner orig", dict(sorted(e_orig.items())), "port", dict(sorted(port.items())))
print("Raeume", n_raum, "mit RBJ", n_rbj, "abweichend", n_diff)
