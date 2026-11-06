# Leser B L3: Schwanz in Bild 2 (n1435). Textlesung (Polygonzug aus den drei Linienenden) vs. Masken.
# Maske M1 (rote Flaeche): Chroma R-(G+B)/2 >= 6 im Fenster x 262..362, y 200..300, Komponente durch (300,255).
# Maske M2 (bewegter Teil unten): |L(n1435) - L(n1439)| >= 10 (Grauwert-Mittel RGB), Fenster x 272..312, y 286..352.
#   n1439 = gleiche Kamera, unterer Teil dort weg (gemessen: Spalte x292 y322..344 in 1435 = 32/30/26/24/35, in 1439/1441 = 42..46/16/17/30/70).
import sys, json, numpy as np, cv2
from PIL import Image, ImageDraw
from scipy import ndimage as ndi
from skimage.morphology import skeletonize
from skimage.graph import route_through_array
import b_common as C
V, S, BEL = sys.argv[1:4]
L1 = json.load(open(f'{S}/B_L1.json'))
z = {q['text'].split()[0] + ('_' + q['text'].split()[1] if q['text'].startswith('Schwanz ') else ''): q for q in L1['2']['zuordnung']}
BEG = tuple(z['Schwanz_Beginn']['ende']); KN = tuple(z['Schwanz_Knick,']['ende']); END = tuple(z['Schwanzspitze']['ende'])
H2 = float(sys.argv[4])
img = np.asarray(Image.open(f'{V}/Beschreibung2_Vorlage.png').convert('RGB')).astype(float)
Ch = img[..., 0] - (img[..., 1] + img[..., 2]) / 2
win1 = np.zeros((480, 640), bool); win1[200:301, 262:363] = True
M1 = (Ch >= 6) & win1
M1 = ndi.binary_opening(M1, iterations=1)
lab, _ = ndi.label(M1); M1 = lab == lab[255, 300]
b = C.bild(S, 1439); Lb = b.mean(2); La = img.mean(2)
win2 = np.zeros((480, 640), bool); win2[286:353, 272:313] = True
M2 = (np.abs(La - Lb) >= 10) & win2
M2 = ndi.binary_closing(M2, iterations=2) & win2
lab2, n2 = ndi.label(M2); sizes = ndi.sum(M2, lab2, range(1, n2 + 1))
M2 = np.isin(lab2, [i + 1 for i, s in enumerate(sizes) if s >= 15])
M = M1 | M2
Mc = ndi.binary_closing(M, iterations=3)
# Achse: kuerzester Weg durch die Maske (Kosten 1/(1+DT)) von Beginn-nah nach Ende-nah
DT = ndi.distance_transform_edt(Mc)
def nah(p):
    ys, xs = np.nonzero(Mc); i = np.argmin((xs - p[0]) ** 2 + (ys - p[1]) ** 2); return (int(ys[i]), int(xs[i])), float(np.sqrt((xs[i] - p[0]) ** 2 + (ys[i] - p[1]) ** 2))
(pb, db), (pk, dk), (pe, de) = nah(BEG), nah(KN), nah(END)
cost = np.where(Mc, 1.0 / (1.0 + DT), 1e3)
pfad1, _ = route_through_array(cost, pb, pk, fully_connected=True, geometric=True)
pfad2, _ = route_through_array(cost, pk, pe, fully_connected=True, geometric=True)
def laenge(p): p = np.array(p, float); return float(np.sum(np.hypot(*np.diff(p, axis=0).T)))
def stationen(pfad, fr):
    p = np.array(pfad, float); s = np.r_[0, np.cumsum(np.hypot(*np.diff(p, axis=0).T))]; out = []
    for f in fr:
        i = int(np.argmin(np.abs(s - f * s[-1]))); y, x = pfad[i]
        # Breite senkrecht zur Achse: Richtung aus +-4 Punkten, Schnitt durch Mc
        j0, j1 = max(0, i - 4), min(len(pfad) - 1, i + 4)
        d = np.array(pfad[j1], float) - np.array(pfad[j0], float); d /= (np.linalg.norm(d) + 1e-9); nrm = np.array([-d[1], d[0]])  # (dy,dx)->Normale
        w = 0
        for sgn in (1, -1):
            t = 0.0
            while True:
                yy, xx = y + sgn * (t + 0.5) * nrm[0], x + sgn * (t + 0.5) * nrm[1]
                if not (0 <= int(round(yy)) < 480 and 0 <= int(round(xx)) < 640) or not Mc[int(round(yy)), int(round(xx))]: break
                t += 0.5
            w += t
        out.append(dict(f=f, x=int(x), y=int(y), breite_px=round(w + 1, 1), breite_H=round((w + 1) / H2, 3),
                        breite_2DT_px=round(2 * float(DT[y, x]), 1), achse_richtung_bild=[round(float(d[1]), 2), round(float(d[0]), 2)]))
    return out
FR = [0.1, 0.3, 0.5, 0.7, 0.9]
# Textlesung
def winkel(a, b): return float(np.degrees(np.arctan2(b[1] - a[1], b[0] - a[0])))
v1 = np.array(KN, float) - np.array(BEG, float); v2 = np.array(END, float) - np.array(KN, float)
knick = float(np.degrees(np.arccos(np.dot(v1, v2) / np.linalg.norm(v1) / np.linalg.norm(v2))))
# Abweichung Maske vs Sehne Beginn->Knick
p1 = np.array(pfad1, float)[:, ::-1]
def abst_sehne(P, a, b):
    a = np.array(a, float); b = np.array(b, float); d = (b - a) / np.linalg.norm(b - a); n = np.array([-d[1], d[0]])
    return (P - a) @ n
ab = abst_sehne(p1, BEG, KN)
erg = dict(
    text=dict(beginn=BEG, knick=KN, ende=END,
              seg1=dict(laenge_px=round(float(np.linalg.norm(v1)), 1), laenge_H=round(float(np.linalg.norm(v1)) / H2, 3), richtung_grad_bild=round(winkel(BEG, KN), 1)),
              seg2=dict(laenge_px=round(float(np.linalg.norm(v2)), 1), laenge_H=round(float(np.linalg.norm(v2)) / H2, 3), richtung_grad_bild=round(winkel(KN, END), 1)),
              knickwinkel_richtungsaenderung_grad=round(knick, 1), innenwinkel_grad=round(180 - knick, 1)),
    maske=dict(M1_px=int(M1.sum()), M2_px=int(M2.sum()), anschluss_px=dict(beginn=round(db, 1), knick=round(dk, 1), ende=round(de, 1)),
               achse1_laenge_px=round(laenge(pfad1), 1), achse2_laenge_px=round(laenge(pfad2), 1),
               achse1_max_abweichung_von_sehne_px=round(float(np.abs(ab).max()), 1), achse1_abweichung_vorzeichen=('links-oben der Sehne (Bild)' if ab[np.argmax(np.abs(ab))] > 0 else 'rechts-unten der Sehne (Bild)'),
               stationen_knick_nach_ende=stationen(pfad2, FR), stationen_knick_nach_beginn=stationen(pfad1[::-1], FR)))
json.dump(erg, open(f'{S}/B_L3.json', 'w'), indent=1)
np.savez_compressed(f'{S}/B_L3_masken.npz', M1=M1, M2=M2, Mc=Mc, pfad1=np.array(pfad1), pfad2=np.array(pfad2))
print(json.dumps(erg, indent=1)[:4000])
# Belegbild x3
im = Image.open(f'{V}/Beschreibung2_Vorlage.png').convert('RGB').crop((250, 190, 380, 360))
a = np.asarray(im).astype(float); a = np.clip((a - 12) / 110 * 255, 0, 255).astype(np.uint8)
ov = a.copy(); sub = (slice(190, 360), slice(250, 380))
ov[M1[sub]] = (0.5 * ov[M1[sub]] + 0.5 * np.array([255, 0, 255])).astype(np.uint8)
ov[M2[sub]] = (0.5 * ov[M2[sub]] + 0.5 * np.array([0, 255, 255])).astype(np.uint8)
im = Image.fromarray(ov).resize((130 * 4, 170 * 4), Image.NEAREST); d = ImageDraw.Draw(im)
T = lambda p: ((p[0] - 250 + .5) * 4, (p[1] - 190 + .5) * 4)
d.line([T(BEG), T(KN), T(END)], fill=(255, 255, 0), width=3)
d.line([T((x, y)) for y, x in pfad1 + pfad2], fill=(0, 255, 0), width=2)
for st in erg['maske']['stationen_knick_nach_ende'] + erg['maske']['stationen_knick_nach_beginn']:
    d.ellipse([T((st['x'], st['y']))[0] - 4, T((st['x'], st['y']))[1] - 4, T((st['x'], st['y']))[0] + 4, T((st['x'], st['y']))[1] + 4], outline=(255, 128, 0), width=2)
for p, nm in ((BEG, 'Beginn'), (KN, 'Knick'), (END, 'Ende')):
    d.text((T(p)[0] + 6, T(p)[1] - 6), nm, fill=(255, 255, 0))
d.text((4, 4), 'n1435 x4, Kontrast 12..122. gelb=Textlesung, gruen=Maskenachse,\nmagenta=M1 Chroma>=6, cyan=M2 |1435-1439|>=10, orange=Stationen', fill=(255, 255, 255))
im.save(f'{BEL}/B_L3_schwanz_maske_x4.png')
