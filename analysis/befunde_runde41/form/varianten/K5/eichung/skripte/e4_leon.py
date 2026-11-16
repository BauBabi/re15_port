"""E4c: Referenz Leon (PL00) in Bild 1.

Bild 1 hat keinen Schnitt auf der Disc (kamera.md; Uebersicht der 6 Schnitte von 5080 in tafeln/T1): kein Hintergrund-Pixelpaar.
Leon ist die einzige Pixel-Referenz der Szene. Licht: 5080 Schnitt 2 (Disc), wie die Szenenkette der Form. Lage aus dem Fusspunkt
(125,366) auf dem Boden (kamera_bild1.json); Unsicherheit Gier 1024/1536/2048 und Tiefe +-1000 entlang der Blickachse.

Modell Szene 1: form_video (Kurve fest fuer dunkle Dias, gemessen an 15 Dias, farbe.md) mit Kanalbalance EX, an Leon gefittet
(3 Parameter). Begruendung: e4b zeigt, dass ein Szenen-Fit mit freien Kurven ausserhalb des Farbraums der Fit-Pixel nicht traegt;
die Kurve aus 15 Dias ist der breiter belegte Teil, EX ist laut farbe.md der Teil, der je Dia wechselt.
Gegenprobe: Leon-Klasse auslassen (Uniform / dunkel / Haut-Haar), EX an den anderen fitten, ausgelassene testen.
Ausgang daten/e4_leon.json, daten/e4_modell_bild1.json."""
import json
import numpy as np
import cv2
from scipy.optimize import least_squares
import ek

FV = ek.FV
V = ek.video(1)
Vu8 = np.clip(V + .5, 0, 255).astype(np.uint8)
EX0 = FV.EX[1].copy()
mo, tex, _ = ek.leon()
fuss = np.array(ek.boden(1, (125, 366)))
kam = ek.FK.lade_kamera(1)
blick = np.array(kam['ziel'], float) - np.array(kam['auge'], float)
blick[1] = 0
blick /= np.linalg.norm(blick)
BOX = (185, 380, 70, 170)
y0, y1, x0, x1 = BOX
ero = lambda q, r=2: cv2.erode(q.astype(np.uint8), np.ones((2 * r + 1, 2 * r + 1), np.uint8)) > 0


def umkehr(ex):
    alt = FV.EX[1].copy()
    FV.EX[1] = np.asarray(ex, float)
    try:
        return FV.video_umkehr(Vu8, 1)
    finally:
        FV.EX[1] = alt


def vor(comp, ex):
    return FV.video_filter(np.clip(comp + .5, 0, 255).astype(np.uint8), bild=1, entzerren=False, rauschen=False, ex=ex).astype(np.float32)


def ncc(a, b):
    a = a - a.mean()
    b = b - b.mean()
    return float((a * b).sum() / np.sqrt((a * a).sum() * (b * b).sum() + 1e-9))


def mass(P, q):
    if q.sum() < 5:
        return dict(pixel=int(q.sum()))
    d = ek.de76(P[q], V[q])
    mv, mp = np.median(V[q], 0), np.median(P[q], 0)
    return dict(pixel=int(q.sum()), dE76_median=round(float(np.median(d)), 2), dE76_p90=round(float(np.percentile(d, 90)), 2),
                dE76_der_mediane=round(float(ek.de76(mp, mv)), 2), video_median=[round(float(x), 1) for x in mv],
                modell_median=[round(float(x), 1) for x in mp])


def fit_ex(s, m, q, ex0):
    ys, xs = np.nonzero(q)
    ex = np.asarray(ex0, float)
    for _ in range(2):                                   # Fuellung des unbekannten Hintergrunds aus der Umkehr mit aktuellem EX
        comp = umkehr(ex).astype(np.float32)
        comp[m] = s[m]
        r = least_squares(lambda lx: (vor(comp, np.exp(lx))[ys, xs] - V[ys, xs]).ravel(), np.log(ex), loss='soft_l1', f_scale=6.0,
                          diff_step=0.01, max_nfev=40)
        ex = np.exp(r.x)
    return ex, comp


def lauf(lage, gier):
    # Ausrichtung im Spielraum: Leon ist im Video dunkel vor hellem Zug/Boden, Helligkeits-NCC sucht das Falsche (gemessen: NCC ~ 0).
    # Kosten = mittlerer Abstand |Umkehr(Video) - Render| ueber die Figurpixel (e, 0..255, Summe der Kanaele), kleinster gewinnt.
    best = None
    for clip, bild in ((0, 0), (0, 10), (0, 20), (2, 0)):
        s, m, sh, alb = ek.figur(mo, tex, ek.kf(mo, clip, bild), lage, gier, 1)
        ys, xs = np.nonzero(m)
        sv = s[ys, xs].astype(np.float32)
        for dy in range(-14, 15):
            for dx in range(-14, 15):
                yy, xx = np.clip(ys + dy, 0, 479), np.clip(xs + dx, 0, 639)
                c = -float(np.abs(EV[yy, xx] - sv).sum(1).mean())
                if best is None or c > best[0]:
                    best = (c, clip, bild, dx, dy)
    c, clip, bild, dx, dy = best
    s, m, sh, alb = ek.figur(mo, tex, ek.kf(mo, clip, bild), lage, gier, 1)
    sch = lambda a: np.roll(a, (dy, dx), (0, 1))
    s, m, sh, alb = sch(s), sch(m), sch(sh), sch(alb)
    L = ek.lab(alb.astype(np.float32))
    g = ero(m, 2) & (V.max(2) < 220)
    kl = {'Uniform': g & ero((L[..., 2] < -6) & (L[..., 0] > 20), 1),
          'dunkel': g & ero(L[..., 0] < 22, 1),
          'Haut_Haar': g & ero(L[..., 2] > 8, 1)}
    alle = np.any(list(kl.values()), 0)
    ex, comp = fit_ex(s, m, alle, EX0)
    P = vor(comp, ex)
    P0 = vor(comp, EX0)
    return dict(abstand_e=round(-c, 2), clip=clip, bild=bild, versatz=[dx, dy], ex=[round(float(x), 4) for x in ex],
                gesamt=mass(P, alle), gesamt_ex_farbe_md=mass(P0, alle), klassen={k: mass(P, q) for k, q in kl.items()},
                schattierung_median=[int(x) for x in np.median(sh[m], 0)]), (s, m, kl, comp, ex, alb)


EV = umkehr(EX0).astype(np.float32)
aus = dict(fuss_welt=[round(float(x)) for x in fuss], ex_farbe_md=EX0.round(4).tolist(), laeufe={})
best = None
for tiefe in (-1000, 0, 1000):
    lage = (fuss + tiefe * blick).round().astype(int)
    lage[1] = 0
    for gier in (1024, 1536, 2048):
        r, extra = lauf(tuple(int(x) for x in lage), gier)
        r['lage'] = [int(x) for x in lage]
        aus['laeufe']['tiefe%+d_gier%d' % (tiefe, gier)] = r
        print(tiefe, gier, r['abstand_e'], r['ex'], r['gesamt'].get('dE76_median'), r['gesamt_ex_farbe_md'].get('dE76_median'), flush=True)
        if best is None or -r['abstand_e'] > best[0]:
            best = (-r['abstand_e'], tiefe, gier, extra)
exs = np.array([v['ex'] for v in aus['laeufe'].values()])
aus['ex_streuung'] = dict(min=exs.min(0).round(4).tolist(), max=exs.max(0).round(4).tolist(), median=np.median(exs, 0).round(4).tolist())
aus['gewaehlt'] = dict(tiefe=best[1], gier=best[2], regel='kleinster Abstand Umkehr(Video) - Render ueber die Figurpixel (e)')
s, m, kl, comp, ex, alb = best[3]
# Gegenprobe: Klasse auslassen
aus['gegenprobe_klasse_ausgelassen'] = {}
for k, q in kl.items():
    rest = np.any([v for kk, v in kl.items() if kk != k], 0)
    exk, compk = fit_ex(s, m, rest, ex)
    aus['gegenprobe_klasse_ausgelassen'][k] = dict(ex=[round(float(x), 4) for x in exk], test=mass(vor(compk, exk), q),
                                                   test_mit_ex_farbe_md=mass(vor(compk, EX0), q))
    print('ohne', k, aus['gegenprobe_klasse_ausgelassen'][k], flush=True)
json.dump(dict(bild=1, ex=[float(x) for x in ex], ex_min=exs.min(0).tolist(), ex_max=exs.max(0).tolist(),
               quelle='e4_leon: form_video Bild 1 mit EX an Leon gefittet'), open(ek.DATEN + '/e4_modell_bild1.json', 'w'), indent=1)
json.dump(aus, open(ek.DATEN + '/e4_leon.json', 'w'), indent=1, ensure_ascii=False)
np.savez_compressed(ek.SCR + '/e4_leon.npz', comp=comp, m=m, P=vor(comp, ex), P0=vor(comp, EX0), s=s, alb=alb,
                    **{'k_' + k: q for k, q in kl.items()})
print(json.dumps(aus['ex_streuung']), aus['gewaehlt'])
