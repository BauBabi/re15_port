"""Pruefer V4: Helligkeitsprofil entlang der Nutzerlinie ueber das Linienende hinaus
(t<0 = Richtung Text, t>0 = in Zeigerichtung hinter dem Ende), Vorlagebilder ohne Linien.
Blob um den hellsten Punkt im Fenster hinter dem Ende (Schwelle = Mittel aus Kern und Ring).
Aufruf: python p_kopf.py OUTDIR"""
import sys, json, numpy as np
from PIL import Image
OUT = sys.argv[1]
V = r"C:/workspace/git/reAi_v2/Version_80%/"
def lum(a): return a @ np.array([0.299, 0.587, 0.114])
FAELLE = [  # bild, text, letztes Linienstueck (von, bis=Ende) aus P_linien (Skelett)
    (1, "Auge", (382, 150), (382, 164)),     # Haken: letztes Stueck senkrecht
    (1, "Haare", (659, 138), (404, 160)),
    (1, "Nase", (663, 166), (395, 177)),
    (2, "Auge seitlich", (651, 177), (460, 234)),
    (2, "Haare seitlich", (667, 221), (468, 241)),
]
res = []
for b, t, s, e in FAELLE:
    a = np.asarray(Image.open(V + f"Beschreibung{b}_Vorlage.png").convert("RGB")).astype(float)
    L = lum(a)
    d = np.array(e, float) - np.array(s, float); d /= np.linalg.norm(d)
    prof = []
    for tt in range(-8, 21, 2):
        p = np.array(e) + tt * d
        x, y = int(round(p[0])), int(round(p[1]))
        prof.append((tt, round(float(L[y-1:y+2, x-1:x+2].mean()), 1)))
    ex, ey = e
    kern = float(L[ey-1:ey+2, ex-1:ex+2].mean())
    ring = L[ey-4:ey+5, ex-4:ex+5].copy(); ring[2:7, 2:7] = np.nan
    res.append({"bild": b, "text": t, "ende": e, "richtung": d.round(3).tolist(),
                "kern3x3": round(kern, 1), "ring9x9": round(float(np.nanmean(ring)), 1), "profil": prof})
    print(b, t, e, "kern", round(kern,1), "ring", round(float(np.nanmean(ring)),1))
    print("   profil", prof)
json.dump(res, open(OUT + "/P_kopf_profile.json", "w"), indent=1)
