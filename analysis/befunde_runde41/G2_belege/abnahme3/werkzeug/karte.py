# Karte: Gator-Wurzel mit Kanister (gator_trag.log) farbig nach Zeit, Stillstands-Fenster (30 B. netto < 300) rot, Spieler grau.
import re, sys, math
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt
run, out = sys.argv[1], sys.argv[2]
rx = re.compile(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\) takt=\((-?\d+),(-?\d+)\) nach_klemmen=\((-?\d+),(-?\d+)\) spieler=\((-?\d+),(-?\d+)\)")
R = [tuple(int(v) for v in m.groups()) for m in (rx.match(l) for l in open(run + "/gator_trag.log")) if m]
fig, ax = plt.subplots(1, 2, figsize=(14, 7))
a = ax[0]
a.add_patch(plt.Rectangle((-8900, -27000), 16100, 21600, fill=False, color="k"))
a.add_patch(plt.Rectangle((-1700, -20000), 3550, 7550, color="#bbb"))
a.add_patch(plt.Rectangle((1850, -18100), 5350, 3600, color="#ddd"))
sc = a.scatter([r[5] for r in R], [r[6] for r in R], c=[r[0] for r in R], s=1, cmap="viridis")
a.scatter([r[11] for r in R][::30], [r[12] for r in R][::30], s=3, c="#888")
st = set()
for i in range(len(R) - 30):
    if R[i + 30][0] - R[i][0] == 30 and math.hypot(R[i + 30][5] - R[i][5], R[i + 30][6] - R[i][6]) < 300: st.update(range(i, i + 31))
a.scatter([R[i][5] for i in st], [R[i][6] for i in st], s=4, c="r")
a.set_aspect("equal"); a.set_title("%s: Wurzel mit Kanister (Farbe = Bild), rot = 30 B. netto < 300, grau = Spieler" % run.split("/")[-1], fontsize=8)
plt.colorbar(sc, ax=a, label="Bild")
b = ax[1]
f = [r[0] / 30 for r in R]
b.plot(f, [r[5] for r in R], lw=0.6, label="x"); b.plot(f, [r[6] for r in R], lw=0.6, label="z")
b.plot(f, [math.hypot(r[5] - r[11], r[6] - r[12]) for r in R], lw=0.6, label="Abstand Wurzel-Spieler")
b.set_xlabel("s (Raumbild/30)"); b.legend(fontsize=7); b.set_title("Lage ueber die Zeit", fontsize=8)
plt.tight_layout(); plt.savefig(out, dpi=90)
