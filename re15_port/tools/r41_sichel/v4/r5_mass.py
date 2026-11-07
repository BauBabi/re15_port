"""SF V4 Runde 5 (Bauer): neue Masse fuer die Entscheide O7-O10 des Orchestrators (Pruefung V4 r4) - Riegel und Pruefer benutzen
dieselben Funktionen (riegel_v4.py ruft sie auf).

  ausreisser   O9 "keine Farbe, die im Original-Bild an der Stelle nicht vorkommt; Riegel 'keine Ausreisserfarben' (Abstand jedes Texels zur
               Farbmenge der Region im Original) fuer alle Koerper-Meshes": je Koerper-Mesh (Ids 0-15) und Messansicht (Pose "135" gegen
               Vorlage 1 entblendet, Pose "85" gegen Vorlage 2) der Abstand (dE76, Lab) jedes sichtbaren Port-Bildpunkts (Schirm in der
               Eichung V = a + b x P, ohne Weichzeichnung: es geht um einzelne Texel) zur naechsten Farbe der Original-Region. Original-Region
               = die Quellen der Koerperfarben (r2_koerper_farben.REGIONEN, Boxen/Polygon der Ansicht fuer diese Ids) ohne Hintergrund-Gelb
               (r2_koerper_farben.gelb_grenze); Kopf (Id 9) = Kopfbox des Messblatts (Abschnitt 2/3). Id 15 (Sichel (B)-links) ist im Original
               nicht abgrenzbar und behaelt die Referenz-2-Farben (r2_koerper_farben) - ohne Region, ausgegeben, nicht bewertet.
  schwanz_achse O8: Lab entlang der Schwanz-Mittellinie (t2_mass_v4, Saat am Ruecken) - Naht = groesster Sprung dE76 ueber 3 px der
               Bogenlaenge (Verfahren des Sicht-Pruefers V4 r4 schwanz_naehte.py, dort spaltenweise im waagerechten Teil, hier entlang der
               ganzen Achse); Stationen Wurzel / Mitte / Biegung / senkrecht / Endstueck an den Anteilen der Bogenlaenge zwischen den
               Punkten Beginn - Biegung - Knick - Ende (Original: Nutzerpunkte Messblatt 1a und die Biegung nach t2_mass_v4).
  schwanz_rot  O8 "Rotanteil wie im Original": Anteil a* > 12 im waagerechten Teil (x 284-356, y < 262, Verfahren des Sicht-Pruefers V4 r4
               schwanz_farbe_waagerecht.py).
Keine RE-Werte: die Sichel-Form gibt es auf der Disc nicht; alle Ziele sind Messungen am 80%-Video (Vorlagen n1485 / n1435).
"""
import os, sys
import numpy as np, cv2
from scipy import ndimage as nd
from scipy.spatial import cKDTree
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import t2_mass_v4 as T2M
import r4_mass as R4

AF_DE = 10.0     # Ausreisser: weiter als 10 dE76 von JEDER Farbe der Original-Region. Bezug: die Regionsriegel (F, FS) erlauben dE00 5 zum
                 # Regionsmittel; 10 dE76 entspricht bei diesen dunklen Farben etwa dE00 5-7. Das Original selbst (Nachbarbilder n1484/n1486
                 # gegen n1485, n1434/n1436 gegen n1435) hat 0,0000 Anteil ueber 10 dE76 (p99,9 2,7-4,4; Protokoll r5_mass __main__)
AF_ANTEIL = 0.01  # je Mesh und Ansicht hoechstens 1 % Ausreisser-Bildpunkte (Original 0,0000; ein Prozent fuer Kanten, an denen Port- und
                  # Original-Figur verschieden liegen)


def _regionen_quelle(ansicht):
    import r2_koerper_farben as RK
    orig = G.vorlage(1 if ansicht == '135' else 2)
    Lo = G.lab(orig)
    gelb = RK.gelb_grenze()
    je_id = {}
    for reg in RK.REGIONEN:
        name, ans, idl, boxen = reg[:4]
        teil = reg[6] if len(reg) > 6 else None
        if ans != ansicht:
            continue
        m = np.zeros((480, 640), bool)
        if teil is not None and teil[0] == 'kacheln':
            m |= RK.oberkoerper_flaeche()
        for (x0, y0, x1, y1) in boxen:
            m[y0:y1 + 1, x0:x1 + 1] = True
        m &= Lo[..., 2] <= gelb
        for b in idl:
            je_id[b] = je_id.get(b, np.zeros((480, 640), bool)) | m
    kb = G.KB_ORIG[1 if ansicht == '135' else 2]
    k = np.zeros((480, 640), bool); k[kb[1]:kb[3] + 1, kb[0]:kb[2] + 1] = True
    je_id[9] = k & (Lo[..., 2] <= gelb)
    out = {b: Lo[m] for b, m in je_id.items()}
    if ansicht == '85':          # Beine nur vorn als Region (r2_koerper_farben 'beine'); seitlich gilt dieselbe Farbmenge
        q = _regionen_quelle('135')
        for b in (2, 3, 5, 6):
            out.setdefault(b, q[b])
    return out


AF_UMKREIS = 15  # Region zusaetzlich = Originalbildpunkte bis 15 px um die Bildpunkte des Meshes (die Port-Figur liegt bis 0,05 H = 13-18 px
                 # neben der Original-Figur: Pose "135" IoU 0,62, Messblatt Abschnitt 9 Punkt 5)


def regionen_mengen(ids, ansicht):
    """O9: Farbmenge (Lab-Tabelle) je Id: Quellregion der Koerperfarben (_regionen_quelle) vereinigt mit dem Umkreis AF_UMKREIS um die
    Bildpunkte des Meshes im Original, beides ohne Hintergrund-Gelb. Id 15 ohne Menge (s. Kopf)."""
    import r2_koerper_farben as RK
    Lo = G.lab(G.vorlage(1 if ansicht == '135' else 2))
    gelb = Lo[..., 2] <= RK.gelb_grenze()
    q = _regionen_quelle(ansicht)
    out = {}
    for b in range(15):
        m = ids == b
        if m.sum() < 30:
            continue
        um = nd.binary_dilation(m, iterations=AF_UMKREIS) & gelb
        out[b] = np.vstack([Lo[um]] + ([q[b]] if b in q and q[b] is not None and len(q[b]) else []))
    return out


def ausreisser(img, ids, ansicht):
    """O9: je Id (0-15) Anteil der Bildpunkte mit Abstand > AF_DE zur Farbmenge der Original-Region; gelbe Bildpunkte (b* > 20, a* < 12,
    L* > 25, Riegelvorschlag Sicht-Pruefer V4 r4 Abschnitt 3) gezaehlt."""
    Pl = G.lab(np.clip(G.EA_ + G.EB_ * np.asarray(img, float), 0, 255))
    q = regionen_mengen(ids, ansicht)
    out = {}
    for b in range(16):
        m = ids == b
        if m.sum() < 30:
            continue
        gelb = int(((Pl[..., 2] > 20) & (Pl[..., 1] < 12) & (Pl[..., 0] > 25) & m).sum())
        if b not in q or len(q[b]) < 30:
            out[b] = dict(n=int(m.sum()), anteil=None, gelb=gelb)
            continue
        d, _ = cKDTree(q[b]).query(Pl[m])
        out[b] = dict(n=int(m.sum()), anteil=float((d > AF_DE).mean()), p99=float(np.percentile(d, 99)), gelb=gelb)
    return out


def ausreisser_original():
    """Gegenprobe des Verfahrens am Original: Nachbarbilder gegen die Vorlage (Region = Figurbox, ohne Gelb)."""
    sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'nb2neu'))
    os.environ.setdefault('NB6_RUNS', os.path.join(G.SFV4, 'r'))
    import c6
    import r2_koerper_farben as RK
    out = {}
    for h, b, box in ((134, 1, (300, 140, 500, 300)), (136, 1, (300, 140, 500, 300)), (84, 2, (280, 185, 470, 345)), (86, 2, (280, 185, 470, 345))):
        v = cv2.cvtColor(c6.vid(h), cv2.COLOR_BGR2RGB).astype(float)
        if b == 1:
            v = np.clip(G.SW + (v - G.SW) / G.K1, 0, 255)
        O, V = G.lab(G.vorlage(b)), G.lab(v)
        x0, y0, x1, y1 = box
        r = np.zeros((480, 640), bool); r[y0:y1, x0:x1] = True; r &= O[..., 2] <= RK.gelb_grenze()
        d, _ = cKDTree(O[r]).query(V[r])
        out[h] = dict(anteil=float((d > AF_DE).mean()), p99=float(np.percentile(d, 99)), p999=float(np.percentile(d, 99.9)))
    return out


# ---------------------------------------------------------------------------------------------------- Schwanz entlang der Achse (O8)
STATIONEN = (('Wurzel', 0, 0.15), ('Mitte', 0, 0.5), ('Biegung', 1, 0.0), ('senkrecht', 1, 0.5), ('Endstueck', 2, 0.5))
# Naht im waagerechten Teil: Verfahren des Sicht-Pruefers V4 r4 (pruefung_sicht_r4/schwanz_naehte.py): je Spalte x 280-376 die Mitte der
# Maske (y < 262), Lab 3x3, groesster dE76-Sprung ueber 3 Spalten; Grenze = Maximum am Original (3 Leser-Masken)
X_NAHT = (280, 376)


def _achse_lab(V, maske, P):
    """Lab (Mittel 3x3 in der Maske) je Punkt der Mittellinie P."""
    L = G.lab(V)
    out = []
    for x, y in np.round(P).astype(int):
        y0, y1, x0, x1 = max(y - 1, 0), y + 2, max(x - 1, 0), x + 2
        mm = maske[y0:y1, x0:x1]
        out.append(L[y0:y1, x0:x1][mm].mean(0) if mm.any() else L[y, x])
    return np.array(out)


def _stationen(P, s, punkte, lab, r_px, V, maske):
    """Stationen nach Anteil der Bogenlaenge der Mittellinie zwischen den Punkten (Beginn, Biegung, Knick, Ende); Lab = Mittel der Maske im
    Kreis r_px um den Mittellinienpunkt."""
    j = [int(np.argmin(np.linalg.norm(P - np.asarray(p, float), axis=1))) for p in punkte]
    L = G.lab(V)
    yy, xx = np.mgrid[:480, :640]
    out = {}
    for nm, abschnitt, f in STATIONEN:
        sa, sb = s[j[abschnitt]], s[j[abschnitt + 1]]
        c = P[int(np.argmin(np.abs(s - (sa + f * (sb - sa)))))]
        m = maske & (np.hypot(xx - c[0], yy - c[1]) <= r_px)
        out[nm] = dict(punkt=(round(float(c[0]), 1), round(float(c[1]), 1)), lab=L[m].mean(0).tolist() if m.any() else [float('nan')] * 3,
                       a12=float((L[m][:, 1] > 12).mean()) if m.any() else float('nan'))
    return out


def mittellinie_original(t):
    """Mittellinie einer Original-Maske: t2_mass_v4.linie (Verfahren O2) bis zur Unterbrechung der Leser-Maske am Knick (y 296-303), darunter
    die Zeilenmitten der Maske in x 270-305 bis y 345 (Ende (295,343)); 5er-Mittel. Rueckgabe (P, s, Biegung nach t2_mass_v4)."""
    ys, xs = np.nonzero(t)
    P, _s = T2M.linie(t, (int(xs.max()), int(ys[np.argmax(xs)])))
    unten = []
    for y in range(int(P[-1][1]) + 3, 346):
        xx = np.nonzero(t[y, 270:306])[0]
        if len(xx):
            unten.append((270 + xx.mean(), float(y)))
    if unten:
        P = np.vstack([P, np.array(unten)])
    P = np.array([P[max(0, j - 2):j + 3].mean(0) for j in range(len(P))])
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    _w, mitte, _p = T2M.drehung(P, s, T2M.H2, T2M.BEGINN_X_ORIG - 0.10 * T2M.H2)
    return P, s, mitte


def schwanz_achse_original():
    """Original: Vorlage 2 roh, 3 Leser-Masken; Punkte Beginn (357,243), Biegung = Fenstermitte nach t2_mass_v4, Knick (290,308), Ende
    (295,343) (Messblatt 1a); Mittellinie mittellinie_original (dieselbe wie r5_schwanz_textur.zuordnung)."""
    o2 = G.vorlage(2)
    out = []
    for t in R4.masken_original():
        P, s, mitte = mittellinie_original(t)
        lab = _achse_lab(o2, nd.binary_erosion(t), P)
        d = np.linalg.norm(lab[3:] - lab[:-3], axis=1)
        st = _stationen(P, s, [(357, 243), tuple(mitte), (290, 308), (295, 343)], lab, 0.02 * T2M.H2, o2, nd.binary_erosion(t))
        out.append(dict(naht=float(d.max()), naht_bei=[round(float(v), 1) for v in P[int(np.argmax(d)) + 1]], stationen=st))
    return out


def schwanz_achse_port(img, ids, H, beginn, knick, ende):
    """Port: Videoraum (Vergleichsregel), Schwanz-Ids; Mittellinie t2_mass_v4.port; Punkte = T1-Beginn, Biegung nach t2_mass_v4, Knick (T3),
    Ende (T4) aus riegel_v4.schwanz_mass_h."""
    r = T2M.port(ids, H)
    if r is None:
        return None
    P = np.asarray(r['P'], float)
    P = np.array([P[max(0, j - 2):j + 3].mean(0) for j in range(len(P))])      # 5er-Mittel wie mittellinie_original
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(P, axis=0), axis=1))]
    V = G.videoraum(img)
    m = nd.binary_erosion(np.isin(ids, G.SCHWANZ))
    lab = _achse_lab(V, m, P)
    d = np.linalg.norm(lab[3:] - lab[:-3], axis=1)
    j0 = int(np.argmin(np.linalg.norm(P - np.asarray(beginn, float), axis=1)))   # Naht ab dem Beginn (davor liegt der Ruecken im Bild)
    jm = j0 + int(np.argmax(d[j0:])) if j0 < len(d) else int(np.argmax(d))
    st = _stationen(P, s, [tuple(beginn), tuple(r['mitte']), tuple(knick), tuple(ende)], lab, 0.02 * H, V, m)
    return dict(naht=float(d[j0:].max()) if j0 < len(d) else float(d.max()), naht_bei=[round(float(v), 1) for v in P[jm + 1]], stationen=st)


def naht_waagerecht(V, maske):
    L = G.lab(V)
    vals = []
    for x in range(X_NAHT[0], X_NAHT[1] + 1):
        ys = np.nonzero(maske[:262, x])[0]
        if len(ys) < 5:
            continue
        y = int((ys.min() + ys.max()) / 2)
        vals.append((x, L[y - 1:y + 2, x - 1:x + 2].reshape(-1, 3).mean(0)))
    if len(vals) < 5:
        return dict(max=float('nan'), bei=None)
    xs = [v[0] for v in vals]; v = np.array([v[1] for v in vals])
    d = np.linalg.norm(v[3:] - v[:-3], axis=1)
    return dict(max=float(d.max()), bei=int(xs[int(np.argmax(d)) + 1]), L=[round(float(a), 1) for a in v[::4, 0]],
                a=[round(float(a), 1) for a in v[::4, 1]], x=xs[::4])


def schwanz_rot(V_or_img, maske, videoraum=True):
    """O8 Rotanteil: Anteil a* > 12 im waagerechten Teil (x 284-356, y < 262), Maske 3x3 erodiert (Sicht-Pruefer V4 r4)."""
    V = G.videoraum(V_or_img) if videoraum else np.asarray(V_or_img, float)
    yy, xx = np.mgrid[:480, :640]
    m = cv2.erode(maske.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool) & (xx >= 284) & (xx <= 356) & (yy < 262)
    L = G.lab(V[m])
    return float((L[:, 1] > 12).mean()) if len(L) else float('nan')


if __name__ == '__main__':
    import json
    print('Ausreisser Original (Nachbarbilder)', json.dumps(ausreisser_original()))
    for d in schwanz_achse_original():
        print('Achse Original', json.dumps(d))
    print('Rot waagerecht Original', [round(schwanz_rot(G.vorlage(2), t, False), 3) for t in R4.masken_original()])
