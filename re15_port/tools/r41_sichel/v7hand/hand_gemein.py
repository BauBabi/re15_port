"""SF V7 linke Hand (Bauer, 2026-11-08): gemeinsame Helfer fuer Messung, Bau, Riegel und Leisten.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Masse = Bildmessung an den Originalfotos
(Version_80%/Beschreibung1_Vorlage.png = Video-Bild f1485 in SF_belege/roh/alle, Spielkamera 50A0 Cut 12) und am Modell (Spielkamera,
RDT-Licht, f_rast). Bildnummern hier = SF_belege/roh/alle/fNNNN (Messblatt-Zaehlung; sfv4/orig/nNNNN ist um 1 versetzt: n(N-1) = fN,
gemessen: Beschreibung1_Vorlage gegen f1485 mittlere Abweichung 0,15, gegen orig n1485 1,86).

Nutzer 2026-11-08 (woertlich, nach Ansicht der V6-Bilder): "Beim 2 Beiner Birkin - fast perfekt, aber die linke Hand von ihn stimmt
immer noch nicht. Erstens sieht man auf den Original Bildern ganz klar, das alle Krallen bis auf die Daumen Kralle rot sind, die Daumen
Kralle ist grau. und 2. Sieht man auf den Original Bildern das Offenbar JEDE der Krallen im Original laenger ist...." (linke Hand =
Wesen-links = Klauenhand, Mesh 12).

Krallen im Modell (Mesh 12, gemessen an EM30_V6.EMD und EM30_V5k.EMD, Topologie gleich): vier geschlossene Doppelpyramiden aus je 6
Dreiecken und 5 Ecken (zwei Spitzen + Ring aus 3 Ecken); keine Ecke und keine Normale teilen sie mit anderen Flaechen (krallen()):
  Daumen  T: Ecken 18-22, Spitze 20 (am Daumen, Ring x ~ +190; Flaechen d0 d1 d3 d4 d6 d7)
  Finger  A: Ecken 45-49, Spitze 47 / C: 40-44, Spitze 42 / B: 27-31, Spitze 29 (unten an der Handflaeche bzw. am Kastenfinger)
Die V6-"Krallenflaechen" (alle Ecken y >= 330, 13 Flaechen) waren die unteren Haelften von A und C, ganz B und die Kappe v16 des
Kastenfingers; der Daumen fehlte (Ecken y 219-343).
"""
import os, sys
import numpy as np, cv2
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
R41 = os.path.dirname(HIER)
for _p in (os.path.join(R41, 'v6drei'), HIER):
    if _p not in sys.path:
        sys.path.insert(0, _p)
import drei_gemein as DG          # noqa: E402

G, V, b2, BB = DG.G, DG.V, DG.b2, DG.BB
VID = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle', 'f%04d.png')
H1 = 302.0                         # Messblatt: H1 = 302 (G.H_ORIG[1]); wie Messblatt 6 auch fuer die Nachbarbilder
ROT_BILDER = tuple(range(1482, 1489))     # Foto 1 = f1485 +- 3
DAUMEN_BILDER = (1475, 1476, 1477)        # Messblatt 6: heller Keil f1476 +- 1
ROT_FENSTER = (482, 338, 506, 371)        # x0, y0, x1, y1 (Hand Bild-rechts; Messblatt 6: rote Hand x 482-511 y 340-367)
DAUMEN_FENSTER = (478, 340, 495, 365)
Z_HAND, Z_DAUMEN, Z_FINGER = 490, 491, 492


# ------------------------------------------------------------------ Foto
def foto_rot(n, amin=5.0, prom=1.0, sigma=0.7):
    """Rote Krallenstreifen in Bild fN: Zeilenmaxima von L* (Gauss 0,7) mit Prominenz >= 1 gegen +-2 px und a* >= 5 im Fenster, ueber
    Zeilen verkettet (Luecke <= 1 Zeile, Versatz <= 1 px); Ketten >= 5 Zeilen. Je Kette Anfang, Ende, Laenge (px), Mittel a*/L*/b*."""
    im = cv2.cvtColor(cv2.imread(VID % n), cv2.COLOR_BGR2RGB).astype(float)
    Lab = G.lab(im); L = ndimage.gaussian_filter(Lab[..., 0], sigma); a = Lab[..., 1]
    x0, y0, x1, y1 = ROT_FENSTER
    ketten = []
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            r = L[y, x - 2:x + 3]
            if not (L[y, x] >= L[y, x - 1] and L[y, x] > L[y, x + 1] and L[y, x] - r.min() >= prom and a[y, x] >= amin):
                continue
            best = None
            for c in ketten:
                ly, lx = c[-1]
                if 1 <= y - ly <= 2 and abs(x - lx) <= 1 and (best is None or abs(x - lx) < abs(x - best[-1][1])):
                    best = c
            if best is not None:
                best.append((y, x))
            else:
                ketten.append([(y, x)])
    out = []
    for c in ketten:
        if c[-1][0] - c[0][0] < 5:
            continue
        out.append(dict(anfang=[c[0][1], c[0][0]], ende=[c[-1][1], c[-1][0]],
                        px=round(float(np.hypot(c[-1][0] - c[0][0], c[-1][1] - c[0][1])), 1),
                        a=round(float(np.mean([a[y, x] for y, x in c])), 2), L=round(float(np.mean([Lab[y, x, 0] for y, x in c])), 2),
                        b=round(float(np.mean([Lab[y, x, 2] for y, x in c])), 2)))
    return out


def foto_daumen(n, lmin=24.0, amax=8.0):
    """Grauer Keil (Daumenkralle nach Nutzer) in Bild fN: groesste 8er-Komponente L* >= 24, a* < 8 im Fenster (wie f_krallen, Messblatt
    6); Laenge entlang der Hauptachse (SVD), Lab-Mittel."""
    im = cv2.cvtColor(cv2.imread(VID % n), cv2.COLOR_BGR2RGB).astype(float)
    Lab = G.lab(im)
    x0, y0, x1, y1 = DAUMEN_FENSTER
    w = np.zeros(Lab.shape[:2], bool); w[y0:y1 + 1, x0:x1 + 1] = True
    m = w & (Lab[..., 0] >= lmin) & (Lab[..., 1] < amax)
    k, lb, st, _ = cv2.connectedComponentsWithStats(m.astype(np.uint8), 8)
    if k < 2:
        return None
    i = 1 + int(np.argmax(st[1:, 4])); ys, xs = np.nonzero(lb == i)
    P = np.c_[xs, ys].astype(float); c = P.mean(0)
    ax = np.linalg.svd(P - c)[2][0]
    t = (P - c) @ ax
    return dict(px_flaeche=int(len(xs)), box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())],
                achse=[round(float(v), 2) for v in ax], px=round(float(t.max() - t.min() + 1), 1),
                lab=[round(float(v), 2) for v in Lab[lb == i].mean(0)])


def foto_mass():
    rot = {n: foto_rot(n) for n in ROT_BILDER}
    daumen = {n: foto_daumen(n) for n in DAUMEN_BILDER}
    rl = [max(k, key=lambda c: c['px']) for k in rot.values() if k]
    dl = [d for d in daumen.values() if d]
    fpx = float(np.median([c['px'] for c in rl])); dpx = float(np.median([d['px'] for d in dl]))
    return dict(rot_bilder=rot, daumen_bilder=daumen, rot_streifen_je_bild=[len(k) for k in rot.values()],
                finger_px=fpx, finger_H=round(fpx / H1, 4),
                finger_a=round(float(np.median([c['a'] for c in rl])), 2), finger_L=round(float(np.median([c['L'] for c in rl])), 2),
                finger_b=round(float(np.median([c['b'] for c in rl])), 2),
                daumen_px=dpx, daumen_H=round(dpx / H1, 4),
                daumen_lab=[round(float(np.median([d['lab'][i] for d in dl])), 2) for i in range(3)])


# ------------------------------------------------------------------ Modell
def krallen(m):
    """Doppelpyramiden von Mesh 12 (Mesh-dict mit 'v', 'tf', 'qf'): Liste dict(name, flaechen, ecken, spitze, wurzel, ring, unten,
    normalen). name 'T' = Daumen (groesstes Ring-x), sonst A/B/C nach Spitzen-Ecke (47/29/42)."""
    P = np.array([v[:3] for v in m['v']], float)
    fl = [('d%d' % i, f) for i, f in enumerate(m['tf'])] + [('v%d' % i, f) for i, f in enumerate(m['qf'])]
    par = list(range(len(P)))

    def fd(a):
        while par[a] != a:
            a = par[a]
        return a
    for _k, f in fl:
        vi = f[1::2]
        for x in vi[1:]:
            par[fd(x)] = fd(vi[0])
    comp = {}
    for k, f in fl:
        comp.setdefault(fd(f[1]), []).append((k, f))
    out = []
    for fs in comp.values():
        ecken = sorted({v for _k, f in fs for v in f[1::2]})
        if len(fs) != 6 or len(ecken) != 5 or any(len(f) != 6 for _k, f in fs):
            continue
        zahl = {e: sum(e in f[1::2] for _k, f in fs) for e in ecken}
        apex = [e for e in ecken if zahl[e] == 3]
        ring = [e for e in ecken if zahl[e] == 4]
        assert len(apex) == 2 and len(ring) == 3
        spitze = max(apex, key=lambda e: P[e, 1]); wurzel = min(apex, key=lambda e: P[e, 1])
        out.append(dict(flaechen=sorted(k for k, _f in fs), ecken=ecken, spitze=spitze, wurzel=wurzel, ring=ring,
                        unten=sorted(k for k, f in fs if spitze in f[1::2]),
                        normalen=sorted({n for _k, f in fs for n in f[0::2]})))
    daumen = max(out, key=lambda k: P[k['ring'], 0].mean())
    namen = {47: 'A', 42: 'C', 29: 'B'}
    for k in out:
        k['name'] = 'T' if k is daumen else namen.get(k['spitze'], 'F%d' % k['spitze'])
    return sorted(out, key=lambda k: k['name'])


def laenge_bild(mo, pose, kralle, Hp=None):
    """Projizierte Laenge Ring-Mitte -> Spitze in der Pose: (px, H, H_port)."""
    import f_analyse as FA
    P = np.array([x[:3] for x in mo.meshes[12].tv], float)
    c = P[kralle['ring']].mean(0)
    pp = FA.welt(mo, pose, 12, [c, P[kralle['spitze']]])
    if Hp is None:
        _i, _d, iv = G.zeichne(mo, pose, voll=True)
        Hp = G.h_port(iv)[2]
    px = float(np.linalg.norm(pp[1] - pp[0]))
    return px, px / Hp, Hp


def pixel(mo, pose, flaechen):
    """f_rast-Render (Spielkamera, RDT-Licht, BSS): Maske der Pixel von Knochen 12 mit Flaechenschluessel in flaechen, Lab (Videoraum),
    Render-Dict."""
    import f_rast as FR
    A = FR.mit_bg(mo, pose)
    fl = A['flaeche'][:480]
    sel = (A['bone'][:480] == 12) & np.isin(fl, list(flaechen))
    Lm = G.lab(G.videoraum(A['img'][:480]))
    return sel, Lm, A
