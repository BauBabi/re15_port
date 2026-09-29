#!/usr/bin/env python3
"""zensus_mass.py - Aehnlichkeitsmass entzerrtes Blatt <-> RE2-Tuerarchiv (R31 T1, Schritt 4).

NUR ZWEITBELEG. Die Zuordnung Tuer -> Archiv trifft eine spaetere Welle am Bild.
Alles hier ist frei gewaehlt (PORT-WAHL, keine Original-Adresse); es steuert kein Spielverhalten.

Texturen: info/re2leon/COMMON/DOOR/DOORxx.DO2, TIM 128x256 8 bpp (04_tuerkatalog §1.2).
Blattbereich: das Standardblatt (mesh0, 12 Dreiecke) belegt auf BEIDEN Grossflaechen
u 0..126, v 0..217 (MD1 von DOOR00/03/0B gelesen: Flaeche x=-145 und x=+143 je
(126,217) (0,0) (0,217) / (126,217) (126,0) (0,0)); von der Gegenseite gesehen erscheint die
Textur gespiegelt. Verglichen wird deshalb immer mit Bild UND Spiegelbild (Maximum).

Ein Archiv wird vertreten durch (a) seinen Textur-Blattbereich und (b) alle entzerrten
Ausschnitte derselben Tuer aus RE2-Raumhintergruenden (Door_aot_set pc+26 == Archiv) - (b)
liegt im selben Bildbereich wie die RE1.5-Ausschnitte (vorgerenderter, beleuchteter Hintergrund).
Wert eines Archivs = Maximum ueber seine Vertreter.

Vorbereitung jedes Bildes: mittlere 80 % der Breite / 90 % der Hoehe (Rand = Zarge und
Lagefehler), Helligkeit auf das 2..98-Perzentil gespreizt.
Merkmale: farbe (Chromatizitaet (r,g)/(r+g+b), Mittel + Streuung auf 4 x 6 Feldern),
struktur (Helligkeit 16 x 28, Mittel 0 / Streuung 1, Pearson), gradient (8 Richtungen auf
4 x 7 Feldern, Kosinus). Wert = Mittel der drei Einzelwerte in 0..1.

Aufruf:
  python re15_port/tools/tueren/zensus_mass.py validieren
"""
import json
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L     # noqa: E402

BLATT_U, BLATT_V = 127, 218      # u 0..126, v 0..217
ENTZ_W, ENTZ_H = 128, 218
# Archive ohne senkrechtes Tuerblatt (04_tuerkatalog §2: Treppenlaeufe 0E/0F/12, Leiter 16,
# Schott/Klappe 1E/33/35, Stahlplatte 1F, Bodenluke 28, Klappe 2B, Hubbuehne 2D)
NICHT_BLATT = {0x0E, 0x0F, 0x12, 0x16, 0x1E, 0x1F, 0x28, 0x2B, 0x2D, 0x33, 0x35}


# ----------------------------------------------------------------------------
# Texturen
# ----------------------------------------------------------------------------
_TEX = None


def texturen():
    """{nr: dict(rgba 256x128x4, blatt 218x128x3 (u 0..126 auf 128 gestreckt), gleich_wie)}"""
    global _TEX
    if _TEX is not None:
        return _TEX
    import glob
    from PIL import Image
    import do2_format as F
    aus = {}
    for p in sorted(glob.glob(os.path.join(L.REPO, "info", "re2leon", "COMMON", "DOOR", "DOOR*.DO2"))):
        nr = int(os.path.basename(p)[4:6], 16)
        do2 = F.Do2.lesen(open(p, "rb").read())
        tim, _ = F.Tim.lesen(do2.tim)
        rgba = np.asarray(tim.als_bild(), np.uint8)
        blatt = np.asarray(Image.fromarray(rgba[0:BLATT_V, 0:BLATT_U, :3]).resize((ENTZ_W, ENTZ_H), Image.BILINEAR))
        aus[nr] = dict(nr=nr, pfad=L.rel(p), rgba=rgba, blatt=blatt)
    for nr, t in aus.items():
        t["gleich_wie"] = min(m for m, u in aus.items()
                              if np.array_equal(u["rgba"][0:BLATT_V, 0:BLATT_U], t["rgba"][0:BLATT_V, 0:BLATT_U]))
    _TEX = aus
    return aus


# ----------------------------------------------------------------------------
# Vorbereitung und Merkmale
# ----------------------------------------------------------------------------
def vorbereiten(rgb):
    from PIL import Image
    h, w = rgb.shape[:2]
    y0, x0 = int(h * 0.05), int(w * 0.10)
    a = np.asarray(Image.fromarray(rgb[y0:h - y0, x0:w - x0]).resize((ENTZ_W, ENTZ_H), Image.BILINEAR), float)
    lo, hi = np.percentile(a, 2), np.percentile(a, 98)
    return (np.clip((a - lo) / max(1.0, hi - lo), 0, 1) * 255).astype(np.uint8)


def _felder(h, w, nx, ny):
    ys = np.linspace(0, h, ny + 1).astype(int)
    xs = np.linspace(0, w, nx + 1).astype(int)
    return [(ys[j], ys[j + 1], xs[i], xs[i + 1]) for j in range(ny) for i in range(nx)]


def merkmale(rgb):
    """rgb 218x128x3 uint8 (vorbereitet) -> (farbe 48, struktur 448 normiert, gradient 224 normiert)."""
    from PIL import Image
    a = rgb.astype(float)
    s = a.sum(axis=2) + 1e-6
    chrom = np.stack([a[..., 0] / s, a[..., 1] / s], -1)
    farbe = []
    for (y0, y1, x0, x1) in _felder(a.shape[0], a.shape[1], 4, 6):
        c = chrom[y0:y1, x0:x1].reshape(-1, 2)
        farbe.extend(list(c.mean(axis=0)) + list(c.std(axis=0)))
    lum = a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114
    klein = np.asarray(Image.fromarray(lum.astype(np.float32)).resize((16, 28), Image.BILINEAR), float).ravel()
    st = (klein - klein.mean()) / (klein.std() + 1e-6)
    gy, gx = np.gradient(lum)
    mag = np.hypot(gx, gy)
    ang = (np.arctan2(gy, gx) % np.pi) / np.pi * 8
    hog = []
    for (y0, y1, x0, x1) in _felder(lum.shape[0], lum.shape[1], 4, 7):
        hog.extend(np.bincount(np.minimum(ang[y0:y1, x0:x1].astype(int).ravel(), 7),
                               weights=mag[y0:y1, x0:x1].ravel(), minlength=8))
    hog = np.array(hog)
    return np.array(farbe), st, hog / (np.linalg.norm(hog) + 1e-6)


class Referenz(object):
    """Alle Vertreter aller Archive als Matrizen (je Vertreter Bild und Spiegelbild)."""

    def __init__(self, mit_ausschnitten=True):
        from PIL import Image
        tex = texturen()
        self.info, F, S, G = [], [], [], []

        def dazu(rgb, archiv, art, quelle, raum):
            for sp in (False, True):
                f, s, g = merkmale(vorbereiten(rgb[:, ::-1] if sp else rgb))
                F.append(f)
                S.append(s)
                G.append(g)
                self.info.append(dict(archiv=archiv, art=art, quelle=quelle, raum=raum, gespiegelt=sp))
        for nr, t in sorted(tex.items()):
            dazu(t["blatt"], nr, "textur", t["pfad"], None)
        if mit_ausschnitten:
            for v in json.load(open(os.path.join(L.AUS, "re2_seiten.json"))):
                for g in v.get("gewaehlt") or []:
                    rgb = np.asarray(Image.open(os.path.join(L.REPO, g["entzerrt"])).convert("RGB"))
                    dazu(rgb, v["archiv"], "re2-ausschnitt", "%s ROOM%s c%d" % (v["id"], v["raum"], g["cut"]), v["raum"])
        self.F, self.S, self.G = np.array(F), np.array(S), np.array(G)
        self.archiv = np.array([i["archiv"] for i in self.info])
        self.raum = np.array([i["raum"] or "" for i in self.info])

    def werte(self, rgb, ausschluss_raum=None, nur_textur=False):
        """-> [dict(archiv, wert, farbe, struktur, gradient, vertreter, gespiegelt)] absteigend.
        ausschluss_raum: ein Raumname oder eine Menge von Raumnamen."""
        f, s, g = merkmale(vorbereiten(rgb))
        wf = np.exp(-np.linalg.norm(self.F - f, axis=1) / 0.15)
        ws = (self.S.dot(s) / len(s) + 1) / 2
        wg = self.G.dot(g)
        w = (wf + ws + wg) / 3
        erlaubt = np.ones(len(w), bool)
        if ausschluss_raum:
            for r in ([ausschluss_raum] if isinstance(ausschluss_raum, str) else ausschluss_raum):
                erlaubt &= self.raum != r
        if nur_textur:
            erlaubt &= np.array([i["art"] == "textur" for i in self.info])
        w = np.where(erlaubt, w, -1)
        best = {}
        for i in np.argsort(-w):
            if w[i] < 0:
                break
            a = int(self.archiv[i])
            if a not in best:
                inf = self.info[i]
                best[a] = dict(archiv=a, wert=round(float(w[i]), 4), farbe=round(float(wf[i]), 3),
                               struktur=round(float(ws[i]), 3), gradient=round(float(wg[i]), 3),
                               vertreter=inf["art"] + ":" + inf["quelle"], gespiegelt=inf["gespiegelt"])
        return sorted(best.values(), key=lambda r: -r["wert"])


# ----------------------------------------------------------------------------
# Validierung an RE2-Tueren mit bekanntem Archiv (Vertreter aus demselben Raum ausgeschlossen)
# ----------------------------------------------------------------------------
def validieren(ausgabe=True):
    from PIL import Image
    tex = texturen()
    ref = Referenz()
    seiten = json.load(open(os.path.join(L.AUS, "re2_seiten.json")))
    proben = [v for v in seiten if v.get("gewaehlt")]
    erg = {}
    # streng: auch die Gegenseite derselben Tuer (Zielraum) ausgeschlossen - so hat eine RE1.5-
    # Tuer nie ein Bild DERSELBEN Tuer in der Referenz
    for name, auswahl, nur_textur, streng in (
            ("alle, Textur+Ausschnitte", lambda v: True, False, False),
            ("Blatt-Archive, Textur+Ausschnitte", lambda v: v["archiv"] not in NICHT_BLATT, False, False),
            ("Blatt-Archive, streng", lambda v: v["archiv"] not in NICHT_BLATT, False, True),
            ("alle, nur Textur", lambda v: True, True, False),
            ("Blatt-Archive, nur Textur", lambda v: v["archiv"] not in NICHT_BLATT, True, False)):
        z = dict(n=0, top1=0, top3=0, top5=0, fehl=[])
        for v in proben:
            if not auswahl(v):
                continue
            g = v["gewaehlt"][0]
            rgb = np.asarray(Image.open(os.path.join(L.REPO, g["entzerrt"])).convert("RGB"))
            aus = {v["raum"], v["ziel_basis"] + "0"} if streng else v["raum"]
            r = ref.werte(rgb, ausschluss_raum=aus, nur_textur=nur_textur)
            kl = [tex[x["archiv"]]["gleich_wie"] for x in r]
            k = tex[v["archiv"]]["gleich_wie"]
            z["n"] += 1
            z["top1"] += kl[:1] == [k]
            z["top3"] += k in kl[:3]
            z["top5"] += k in kl[:5]
            if k not in kl[:3]:
                z["fehl"].append("%s ROOM%s D%02X" % (v["id"], v["raum"], v["archiv"]))
        # nicht erreichbar: Archiv kommt nur in diesem einen Raum vor und hat keine eigene Textur-Treffer-Chance
        from collections import Counter
        haeufig = Counter(tex[v["archiv"]]["gleich_wie"] for v in proben if auswahl(v))
        z["haeufigste3"] = ["DOOR%02X x%d" % kv for kv in haeufig.most_common(3)]
        z["grundrate_top3"] = round(sum(c for _, c in haeufig.most_common(3)) / max(1, z["n"]), 3)
        erg[name] = z
        if ausgabe:
            n = max(1, z["n"])
            print("%-36s n=%3d  Top-1 %3d (%4.1f %%)  Top-3 %3d (%4.1f %%)  Top-5 %3d (%4.1f %%)  "
                  "[Grundrate 'immer die 3 haeufigsten': %.1f %%]" % (
                      name, z["n"], z["top1"], 100.0 * z["top1"] / n, z["top3"], 100.0 * z["top3"] / n,
                      z["top5"], 100.0 * z["top5"] / n, 100.0 * z["grundrate_top3"]))
    return erg


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "validieren":
        e = validieren()
        json.dump(e, open(os.path.join(L.AUS, "mass_validierung.json"), "w"), indent=1)
