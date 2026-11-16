"""SF V8 Runde 5 P5: Luecke zwischen Ruecken und Kopf IN DER VORLAGE - Nachmessung des Bauers (Messblatt V8 Abschnitt 11 "Soll neu").

    python vorlage_luecke_r5.py [--ref EMD_V7] [--aus TXT] [--bild PNG]

Auftrag Runde 5: "Der Bauer misst die Vorlage selbst nach (eigener Lauf, Schwellen 6/9/12, Nachbarbilder) und schreibt Messung und Soll
ins Messblatt". Mass (Kastenmass, eigener Lauf des Bauers; gleiche Art wie die Messung des Orchestrators sfv8/r5p5/vorlage_tiefe2.py):
  - Figurmaske der Vorlage = dE (CIELAB) gegen den Konsens-Hintergrund m4_bgT (sfv8/m4_bgT.npy, wie Messblatt m7_trimap) > Schwelle, je
    Bild gemittelt ueber n-1..n+1 und mit Gauss 1,0 geglaettet; zur Probe auch das Einzelbild ohne Mittelung.
  - Kasten = Kopf (Knochen 9) der Referenz V7 in derselben Pose + RAND px nach links / oben / rechts, + 5 px nach unten (PV fuer
    n1437-n1441, P2 = Foto 2 fuer n1433-n1437; Kopf Mesh 9 ist in allen Staenden bytegleich V7).
  - Kerbe = konvexe Huelle der Maske im Kasten ohne die Maske, nur Stuecke in der oberen Haelfte des Kastens; Tiefe eines Bildpunkts =
    Abstand zum freien Hintergrund ausserhalb der Kerbe (wie luecke_r5.tiefe). Je Stueck dazu der Abstand zum Kopf der Referenz:
    "am Kopf" = Stueck beruehrt den um 2 px geweiteten Kopf (Uebergang Huegel/Kopf); "nah" = innerhalb luecke_r5.NAH (12 px); sonst Huegel.
Verworfen (gemessen, steht im Messblatt): das Riegelmass luecke_aus direkt auf die Vorlage (Knochen vom naechsten Bildpunkt der Referenz
uebernommen) - die Figurmaske der Vorlage fehlt am dunklen Gesicht vor dunklem Hintergrund, die Huelle um den Kopf zaehlt dann den ganzen
Raum vor dem Gesicht (n1435 dE > 9: 15 352 px). Das Kastenmass mit oberer Haelfte bleibt davon frei.
Alles Messung an Videobildern der Vorlage (Bildnummern n = Datei f(n+1).png, nur gelesen); kein RE-Wert, die Form gibt es auf keiner Disc.
"""
import os, sys, argparse
import numpy as np, cv2
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import r3_gemein as R3                # noqa: E402
import f_rast as FR                   # noqa: E402
import v4_gemein as G                 # noqa: E402
import luecke_r5 as L                 # noqa: E402

ROH = os.path.join(V.BAUM.replace('r41_sichel_v8', 'r41_sichel'), 'analysis', 'befunde_runde41', 'SF_belege', 'roh', 'alle')   # nur gelesen
SCHWELLEN = (6, 9, 12)                # dE; Auftrag Runde 5 "Schwellen 6/9/12"
BILDER = (('PV', 1437, 1441), ('P2', 1433, 1437))   # Pose der Referenz je Bildbereich (Messblatt V8 Abschnitt 1: n1439 = PV, n1435 = Foto 2)
RAND = 22                             # px um den Kopf (wie vorlage_tiefe2.py; halbe Kopfbreite PV 15 px + Unschaerfe); Messfenster, kein Modellwert
AM_KOPF = 2                           # px: Stueck beruehrt den Kopf der Referenz (Deckung Vorlage/Modell auf etwa 2 px, Messblatt V8 Abschnitt 1)
AUSFALL = 10.0                        # px: Stuecke mit groesserer Tiefe sind Maskenausfall am Kastenrand (n1439 dE > 12: 4 px mit Tiefe 16,0)


def de_bild(n, mittel=True):
    ks = (n - 1, n, n + 1) if mittel else (n,)
    F = np.mean([cv2.imread(os.path.join(ROH, 'f%04d.png' % (k + 1)))[..., ::-1].astype(float) for k in ks], 0)
    bgT = np.load(os.path.join(V.SFV8, 'm4_bgT.npy'))
    dE = np.sqrt(((G.lab(F) - G.lab(bgT)) ** 2).sum(2))
    return cv2.GaussianBlur(dE.astype(np.float32), (0, 0), 1.0)[:480]


def kasten(H):
    ys, xs = np.nonzero(H)
    return (int(xs.min()) - RAND, int(ys.min()) - RAND, int(xs.max()) + RAND, int(ys.max()) + 5)


def kerben(S, box, H):
    """-> (Liste dict(px, x, y, tief, max, kopf), Maske aller Stuecke)."""
    x0, y0, x1, y1 = box
    sub = S[y0:y1, x0:x1].astype(np.uint8)
    Gm = np.zeros(S.shape, bool)
    if not sub.any():
        return [], Gm
    hull = np.zeros_like(sub)
    cv2.fillConvexPoly(hull, cv2.convexHull(np.argwhere(sub)[:, ::-1].astype(np.int32)), 1)
    lab, n = ndimage.label((hull > 0) & (sub == 0))
    for i in range(1, n + 1):
        c = lab == i
        if np.nonzero(c)[0].mean() < (y1 - y0) * 0.5:
            Gm[y0:y1, x0:x1] |= c
    if not Gm.any():
        return [], Gm
    d = ndimage.distance_transform_edt(~(~S & ~Gm))
    dk = ndimage.distance_transform_edt(~H)
    lab, n = ndimage.label(Gm)
    out = []
    for i in range(1, n + 1):
        c = lab == i
        ys, xs = np.nonzero(c)
        out.append(dict(px=int(c.sum()), x=int(xs.mean()), y=int(ys.mean()), tief=int((d[c] >= L.TIEF_AB).sum()),
                        max=round(float(d[c].max()), 1), kopf=round(float(dk[c].min()), 1)))
    return sorted(out, key=lambda s: -s['px']), Gm


def _txt(st):
    return ', '.join('%d/%d/%.1f@(%d,%d)k%.0f' % (s['px'], s['tief'], s['max'], s['x'], s['y'], s['kopf']) for s in st if s['px'] >= 5) or '-'


def messen(blob):
    """-> (Textzeilen, Kacheln fuer bildreihe, beste Werte)."""
    mo = V.modell(blob)
    zeilen = ['Vorlage, Kerbe am Uebergang Huegel/Kopf (Kastenmass, Bauer Runde 5); Referenz fuer den Kopf: %s' % V.sha(blob)[:8],
              'Je Stueck: px / tiefer als %.1f px / groesste Tiefe @ (x,y) k Abstand zum Kopf der Referenz; nur Stuecke >= 5 px' % L.TIEF_AB]
    kacheln = []
    best = {}
    for nm, n0, n1 in BILDER:
        pose = V.PV if nm == 'PV' else R3.P2
        b_ref = FR.zeichne(mo, pose, None, licht=False)['bone'][:480]
        H = b_ref == 9
        box = kasten(H)
        st, _ = kerben(b_ref >= 0, box, H)
        zeilen.append('%s Kasten %s; Referenz selbst: %s' % (nm, box, _txt(st)))
        for n in range(n0, n1 + 1):
            for mittel in (True, False):
                dE = de_bild(n, mittel)
                for s_ in SCHWELLEN:
                    st, Gm = kerben(dE > s_, box, H)
                    zeilen.append('n%d %s dE>%-2d %s | %s' % (n, nm, s_, 'n-1..n+1' if mittel else 'einzeln ', _txt(st)))
                    for art, grenze in (('am Kopf', AM_KOPF), ('nah', L.NAH)):
                        for s in st:
                            if s['kopf'] <= grenze and s['max'] < AUSFALL:
                                g = best.setdefault((art, s_, mittel), [0, 0, 0.0, ''])
                                if s['px'] > g[0]:
                                    g[0], g[3] = s['px'], 'n%d %s (%d,%d)' % (n, nm, s['x'], s['y'])
                                g[1], g[2] = max(g[1], s['tief']), max(g[2], s['max'])
                    if mittel and s_ == 9 and n in (1439, 1435, 1433):
                        kacheln.append((n, nm, box, Gm, dE > s_, [s for s in st if s['kopf'] <= AM_KOPF]))
    zeilen.append('Groesste Werte je Stueck (px / tief / groesste Tiefe; je Wert das Maximum ueber alle Bilder und Stuecke der Art):')
    for (art, s_, mittel), g in sorted(best.items(), key=lambda kv: (kv[0][0], not kv[0][2], kv[0][1])):
        zeilen.append('  %-7s dE>%-2d %s: %d px / %d tief / %.1f px (groesstes Stueck %s)'
                      % (art, s_, 'n-1..n+1' if mittel else 'einzeln ', g[0], g[1], g[2], g[3]))
    zeilen.append('Soll des Riegels S16 (luecke_r5.py): Summe <= %d px, tiefer als %.1f px <= %d, groesste Tiefe <= %.1f px'
                  % (L.SOLL_SUMME, L.TIEF_AB, L.SOLL_TIEF, L.SOLL_MAXTIEFE))
    return zeilen, kacheln, best


def bildreihe(kacheln, f=4):
    """Vorlage roh | Vorlage mit Maskenrand (gelb) und den Kerben der oberen Kastenhaelfte (tuerkis), je Bild; Ausschnitt = Kasten."""
    ks = []
    for n, nm, box, Gm, S, st in kacheln:
        vb = cv2.imread(os.path.join(ROH, 'f%04d.png' % (n + 1)))[..., ::-1][:480].copy()
        m = vb.copy()
        m[S & ~(cv2.erode(S.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)] = (255, 255, 0)
        m[Gm] = (0, 255, 255)
        x0, y0, x1, y1 = box
        t2 = 'dE>9 am Kopf: ' + (', '.join('%d/%d/%.1f' % (s['px'], s['tief'], s['max']) for s in st) or '-')
        for im, t in ((vb, 'Vorlage n%d (%s)' % (n, nm)), (m, t2)):
            k = cv2.resize(np.ascontiguousarray(im[y0:y1, x0:x1]), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
            cv2.putText(k, t, (6, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 3, cv2.LINE_AA)
            cv2.putText(k, t, (6, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 1, cv2.LINE_AA)
            ks.append(k)
    h = max(k.shape[0] for k in ks)
    return np.hstack([np.vstack([k, np.zeros((h - k.shape[0], k.shape[1], 3), np.uint8)]) for k in ks])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--ref', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--aus'); ap.add_argument('--bild')
    a = ap.parse_args()
    zeilen, kacheln, _best = messen(open(a.ref, 'rb').read())
    txt = '\n'.join(zeilen) + '\n'
    if a.aus:
        open(a.aus, 'w', encoding='utf-8', newline='\n').write(txt)
    print(txt)
    if a.bild:
        cv2.imwrite(a.bild, bildreihe(kacheln)[..., ::-1])


if __name__ == '__main__':
    main()
