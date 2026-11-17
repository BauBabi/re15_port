"""E4: Referenz Frau (Typ 0x4D) in Bild 2. ROOM5090 Schnitt 11 hat nur Umgebungslicht (83,78,78), alle Lichtfarben 0 (RDT Licht @0x55c):
Schirmfarbe = Textur x Konstante, unabhaengig von Pose und Normale. Darum ist die Frau eine exakte Referenz, nur die Lage der Pixel ist
geschaetzt (Pose der Vorlage unbekannt, Lage aus kamera.md (1166,0,-25211)).
Gegenprobe A: Modell am Hintergrund gefittet (e3, Variante ridge) -> Frau vorhergesagt gegen Video.
Gegenprobe B: nur Kanalverstaerkung k (3 Werte) an der Frau gefittet, Rest fest -> Hintergrund getestet.
Ausgang daten/e4_frau.json, Scratch e4_frau.npz."""
import json, sys, numpy as np, cv2
from scipy.optimize import least_squares
import ek, em
FV = ek.FV
V = ek.video(2)
J = json.load(open(ek.DATEN + '/e3_modell_bild2.json'))
z = np.load(ek.SCR + '/e3_b2.npz')
E, M = z['E'], z['M']
mo, tex, _ = ek.frau()
LAGE = (1166, 0, -25211)
BOX = (175, 310, 130, 220)          # y0, y1, x0, x1: Frau ohne Beine (Feuer ab y ~ 310)
y0, y1, x0, x1 = BOX
vl = cv2.GaussianBlur(cv2.cvtColor(V, cv2.COLOR_RGB2GRAY), (0, 0), 2)


def ncc(a, b):
    a = a - a.mean(); b = b - b.mean()
    return float((a * b).sum() / np.sqrt((a * a).sum() * (b * b).sum() + 1e-9))


best = None
for clip, bild in ((0, 0), (0, 8), (0, 16), (0, 24), (2, 0), (2, 25)):
    for g in (3712, 3840, 3968, 0, 128, 256, 384):
        s, m, sh, alb = ek.figur(mo, tex, ek.kf(mo, clip, bild), LAGE, g, 2)
        al = cv2.GaussianBlur(cv2.cvtColor(alb, cv2.COLOR_RGB2GRAY).astype(np.float32) * m, (0, 0), 2)
        for dy in range(-16, 17, 2):
            for dx in range(-14, 15, 2):
                A = np.roll(al, (dy, dx), (0, 1))[y0:y1, x0:x1]
                c = ncc(A, vl[y0:y1, x0:x1])
                if best is None or c > best[0]:
                    best = (c, clip, bild, g, dx, dy)
c, clip, bild, g, dx, dy = best
print('beste Lage', best)
s, m, sh, alb = ek.figur(mo, tex, ek.kf(mo, clip, bild), LAGE, g, 2)
sch = lambda a: np.roll(a, (dy, dx), (0, 1))
s, m, sh, alb = sch(s), sch(m), sch(sh), sch(alb)
comp = E.copy()
comp[m] = s[m]
# Klassen aus der Albedo der Textur (Lab)
L = ek.lab(alb.astype(np.float32))
C = np.hypot(L[..., 1], L[..., 2])
ero = lambda q, r=2: cv2.erode(q.astype(np.uint8), np.ones((2 * r + 1, 2 * r + 1), np.uint8)) > 0
gueltig = ero(m, 2) & (np.arange(480)[:, None] < y1) & (V.max(2) < 220)
klassen = {
    'Kittel (Albedo L* > 65, C* < 14)': gueltig & ero((L[..., 0] > 65) & (C < 14), 2),
    'dunkel: Oberteil (Albedo L* < 30)': gueltig & ero(L[..., 0] < 30, 2),
    'Kopf: Haar und Haut (oberste 34 px der Figur)': gueltig & (np.arange(480)[:, None] < np.nonzero(m)[0].min() + 34) & ~ero((L[..., 0] > 65) & (C < 14), 0),
}


def klassen_mass(P):
    out = {}
    for k, q in klassen.items():
        if q.sum() < 10:
            out[k] = dict(pixel=int(q.sum()))
            continue
        mv, mp = np.median(V[q], 0), np.median(P[q], 0)
        d = ek.de76(P[q], V[q])
        out[k] = dict(pixel=int(q.sum()), video_median=[round(float(x), 1) for x in mv], modell_median=[round(float(x), 1) for x in mp],
                      dE76_der_mediane=round(float(ek.de76(mp, mv)), 2), dE76_pixel_median=round(float(np.median(d)), 2),
                      dE76_pixel_p90=round(float(np.percentile(d, 90)), 2),
                      spiel_e_median=[round(float(x), 1) for x in np.median(s[q].astype(float), 0)],
                      textur_median=[round(float(x), 1) for x in np.median(alb[q].astype(float), 0)])
    return out


aus = dict(lage=dict(ncc=round(c, 4), clip=clip, bild=bild, gier=g, versatz_px=[dx, dy], weltlage=LAGE),
           schattierung_disc=[int(x) for x in np.median(sh[m], 0)], box=BOX)
X = em.optik(em.licht(comp))
aus['A_hintergrund_fit_frau_test'] = {}
for art, th in J['alle'].items():
    P = em.kurve(X, np.array(th))
    alle = gueltig & np.any([q for q in klassen.values()], 0)
    d = ek.de76(P[alle], V[alle])
    aus['A_hintergrund_fit_frau_test'][art] = dict(gesamt=dict(pixel=int(alle.sum()), dE76_median=round(float(np.median(d)), 2),
                                                               dE76_p90=round(float(np.percentile(d, 90)), 2)), klassen=klassen_mass(P))
# Bestand form_video.video_filter
FVb = FV.video_filter(np.clip(comp + .5, 0, 255).astype(np.uint8), bild=2, entzerren=False, rauschen=False).astype(np.float32)
alle = gueltig & np.any([q for q in klassen.values()], 0)
d = ek.de76(FVb[alle], V[alle])
aus['A_bestand_video_filter'] = dict(gesamt=dict(pixel=int(alle.sum()), dE76_median=round(float(np.median(d)), 2), dE76_p90=round(float(np.percentile(d, 90)), 2)),
                                     klassen=klassen_mass(FVb))
# B: nur k an der Frau
th0 = np.array(J['th'])
ys, xs = np.nonzero(alle)
Xs = X  # ganzes Bild wegen Chroma


def resk(lk):
    th = np.r_[th0, lk]
    return (em.kurve(X, th)[ys, xs] - V[ys, xs]).ravel()


rk = least_squares(resk, np.zeros(3), loss='soft_l1', f_scale=6.0, max_nfev=60)
k = np.exp(rk.x)
thk = np.r_[th0, rk.x]
Pk = em.kurve(X, thk)
d = ek.de76(Pk[M], V[M])
d0 = ek.de76(em.kurve(X, th0)[M], V[M])
aus['B_frau_fit_k'] = dict(k=[round(float(x), 4) for x in k], frau_klassen=klassen_mass(Pk),
                           hintergrund_test=dict(dE76_median=round(float(np.median(d)), 2), dE76_p90=round(float(np.percentile(d, 90)), 2)),
                           hintergrund_mit_k1=dict(dE76_median=round(float(np.median(d0)), 2), dE76_p90=round(float(np.percentile(d0, 90)), 2)))
json.dump(aus, open(ek.DATEN + '/e4_frau.json', 'w'), indent=1, ensure_ascii=False)
np.savez_compressed(ek.SCR + '/e4_frau.npz', comp=comp, m=m, P=em.kurve(X, th0).astype(np.float32), Pk=Pk.astype(np.float32), alb=alb, s=s,
                    **{'k%d' % i: q for i, q in enumerate(klassen.values())})
print(json.dumps(aus, indent=1, ensure_ascii=False))
