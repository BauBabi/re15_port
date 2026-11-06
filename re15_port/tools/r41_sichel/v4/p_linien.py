"""Pruefer Messblatt V4: Linienzuordnung der Nutzer-Beschriftung (Beschreibung1/2).
Linienfarbe exakt (237,28,36) (Paint-Rot, nicht geglaettet; Texte sind geglaettet).
Fuer jeden Text-Startpunkt S und jedes linke Linienende E: Belegung der geraden
Strecke S->E (Anteil Abtastpunkte mit Linienpixel im 3x3-Nachbarn) und
geodaetische Erreichbarkeit innerhalb der Linienmaske. Kreuzungen je Permutation.
Aufruf: python p_linien.py OUTDIR"""
import sys, json, itertools
import numpy as np, cv2
from PIL import Image
V = r"C:/workspace/git/reAi_v2/Version_80%/"
OUT = sys.argv[1]

def maske(f):
    a = np.asarray(Image.open(V + f + ".png").convert("RGB")).astype(int)
    return ((a[:, :, 0] == 237) & (a[:, :, 1] == 28) & (a[:, :, 2] == 36)).astype(np.uint8), a

def zhang_suen(m):
    m = m.copy().astype(np.uint8)
    while True:
        changed = False
        for step in (0, 1):
            p = np.pad(m, 1)
            P2, P3, P4, P5 = p[:-2, 1:-1], p[:-2, 2:], p[1:-1, 2:], p[2:, 2:]
            P6, P7, P8, P9 = p[2:, 1:-1], p[2:, :-2], p[1:-1, :-2], p[:-2, :-2]
            nb = [P2, P3, P4, P5, P6, P7, P8, P9]
            B = sum(x.astype(int) for x in nb)
            A = sum(((nb[i] == 0) & (nb[(i + 1) % 8] == 1)).astype(int) for i in range(8))
            if step == 0:
                c = (P2 * P4 * P6 == 0) & (P4 * P6 * P8 == 0)
            else:
                c = (P2 * P4 * P8 == 0) & (P2 * P6 * P8 == 0)
            d = (m == 1) & (B >= 2) & (B <= 6) & (A == 1) & c
            if d.any():
                m[d] = 0; changed = True
        if not changed:
            return m

def endpunkte(sk):
    p = np.pad(sk, 1)
    nb = sum(p[1 + dy:1 + dy + sk.shape[0], 1 + dx:1 + dx + sk.shape[1]].astype(int)
             for dy in (-1, 0, 1) for dx in (-1, 0, 1) if (dy, dx) != (0, 0))
    ys, xs = np.nonzero((sk == 1) & (nb == 1))
    return list(zip(xs.tolist(), ys.tolist()))

def belegung(md, s, e):
    n = int(max(abs(e[0] - s[0]), abs(e[1] - s[1])) * 2) + 1
    xs = np.rint(np.linspace(s[0], e[0], n)).astype(int)
    ys = np.rint(np.linspace(s[1], e[1], n)).astype(int)
    return float(md[ys, xs].mean())

def seg_kreuz(a, b, c, d):
    def o(p, q, r):
        return np.sign((q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0]))
    return o(a, b, c) * o(a, b, d) < 0 and o(c, d, a) * o(c, d, b) < 0

def texte(a, x0):
    """Textzeilen rechts: geglaettete rote Pixel (R>150,G<120), Zeilenblöcke."""
    r = a[:, x0:]
    t = ((r[:, :, 0] > 150) & (r[:, :, 1] < 130) & (r[:, :, 2] < 130)).astype(np.uint8)
    return t

res = {}
for f, x_text in (("Beschreibung1", 670), ("Beschreibung2", 655)):
    m, a = maske(f)
    md = cv2.dilate(m, np.ones((3, 3), np.uint8))
    n, lab, st, _ = cv2.connectedComponentsWithStats(m, connectivity=8)
    linien = [i for i in range(1, n) if st[i][4] >= 300]
    sk = zhang_suen(m)
    eps = endpunkte(sk)
    starts, enden = [], []
    for (x, y) in eps:
        comp = int(lab[y, x]) if m[y, x] else int(lab[max(0,y-1):y+2, max(0,x-1):x+2].max())
        if comp not in linien:
            continue
        (starts if x >= 600 else enden).append((x, y, comp))
    # Zusammenfassen benachbarter Endpunkte (Skelett-Sporne): je Cluster 6 px
    def clust(pts):
        out = []
        for p in sorted(pts):
            for q in out:
                if abs(q[0] - p[0]) <= 6 and abs(q[1] - p[1]) <= 6:
                    break
            else:
                out.append(p)
        return out
    starts = sorted(clust(starts), key=lambda p: p[1])
    enden = clust(enden)
    # extremes Linienpixel je Ende (Spitze in der Maske, nicht Skelett)
    tab = {}
    for s in starts:
        row = {}
        for e in enden:
            row[f"{e[0]},{e[1]}"] = {"gerade": round(belegung(md, s, e), 3),
                                    "gleiche_komponente": s[2] == e[2]}
        tab[f"{s[0]},{s[1]}"] = row
    # Permutationen: beste Gesamtbelegung, Kreuzungen
    perms = []
    for p in itertools.permutations(range(len(enden)), len(starts)):
        sc = [belegung(md, starts[i], enden[p[i]]) for i in range(len(starts))]
        kr = sum(seg_kreuz(starts[i][:2], enden[p[i]][:2], starts[j][:2], enden[p[j]][:2])
                 for i in range(len(starts)) for j in range(i + 1, len(starts)))
        perms.append({"zuordnung": [f"{starts[i][0]},{starts[i][1]}->{enden[p[i]][0]},{enden[p[i]][1]}" for i in range(len(starts))],
                      "mittel": round(float(np.mean(sc)), 3), "min": round(float(min(sc)), 3), "kreuzungen": int(kr)})
    perms.sort(key=lambda d: -d["mittel"])
    res[f] = {"komponenten": {int(i): [int(v) for v in st[i]] for i in linien},
              "starts_y_sortiert": starts, "enden": enden, "matrix": tab, "permutationen": perms}
    print(f, "starts", starts, "\n enden", enden)
    for k, v in tab.items():
        print(" ", k, v)
    for p in perms[:4]:
        print("  P", p)
json.dump(res, open(OUT + "/P_linien.json", "w"), indent=1)
