"""SF V5 wie Foto - Koerper (Bauer, 2026-11-08): gemeinsame Messungen fuer Bau, Riegel und Leisten.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Alles ist Bildmessung an den
Originalfotos (Version_80%/Beschreibung1_Vorlage.png = n1485 = Foto 1, Beschreibung2_Vorlage.png = n1435 = Foto 2, Spielkamera
50A0 Cut 12) nach analysis/befunde_runde41/SF_MESSBLATT_FOTO.md (Commit a909bd2cd) und am Modell im Renderer v4_gemein.zeichne.

Posen (v6_gemein.posen, Messblatt Abschnitt 2): P1 = Foto 1 (Clip 20 Bild 139 Gier 1422), P2 = Foto 2 (Clip 12 Bild 48 Gier 861),
"135" = Kopf von vorn.

Schwanzbreite je Station (T-F1): Querschnitt = Durchmesser des groessten Inkreises (2 x Distanztransformation) auf der Normalen
durch die Station (+-25 px), Foto UND Modell mit derselben Funktion. Grund (gemessen, Bauer 2026-11-08): der Lauf entlang der Normalen
(Messblatt Abschnitt 5) misst an der Biegung quer durch die Ecke (Foto f 0,4: 57 px = 0,225 H, f 0,5: 21 px) und haengt damit von
der Lage der Station zur Ecke ab, nicht vom Querschnitt. Stationen: Foto = Nutzerlinie Beginn (357,243) -> Biegung (295,252) ->
Knick (290,308) -> Ende (295,343) (f_schwanz.NUTZER), Modell = projizierte Gelenkkette ab Gelenk 17 (= Beginn, Ende von Id 16) bis
zur Spitze (b2.kette_punkte[1:]); je 11 Stationen gleichen Bogenabstands, bewertet f 0,1-0,9 (an f 0 und 1 endet die Maske, dort
misst der Inkreis die halbe Breite). Foto-Maske = sfv4/foto/schwanz_foto_maske.npy (Messblatt, Leser-V4-Maske, x >= 270, y < 346).
Foto-Sollwert = gleitender Median ueber 3 Stationen (Maskenrauschen: f 0,7 / 0,8 / 0,9 roh 0,033 / 0,055 / 0,033 H).
"""
import os, sys, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
R41 = os.path.dirname(HIER)
for _p in (os.path.join(R41, 'v6kopf'), os.path.join(R41, 'foto'), os.path.join(R41, 'v4'), HIER):
    if _p not in sys.path:
        sys.path.insert(0, _p)
import v6_gemein as V            # noqa: E402
import f_schwanz as FS           # noqa: E402
import birkin_buehne as BB       # noqa: E402

G, b2 = V.G, V.b2
SFV4 = G.SFV4
DATEN = V.DATEN
H2 = 253.0
STATIONEN = [round(0.1 * i, 1) for i in range(11)]
BEWERTET = [round(0.1 * i, 1) for i in range(1, 10)]
SCHWANZ = G.SCHWANZ


def foto_maske():
    return np.load(os.path.join(SFV4, 'foto', 'schwanz_foto_maske.npy'))


def inkreis(M, poly, H, n=11, r=25):
    """Je Station (f, Durchmesser/H, Punkt des Inkreismittelpunkts, Durchmesser px)."""
    D = cv2.distanceTransform(M.astype(np.uint8), cv2.DIST_L2, 5)
    st, _L = FS.stationen(poly, n)
    out = []
    for f, p, d, _s in st:
        nrm = np.array([-d[1], d[0]])
        best, bp = 0.0, None
        for t in np.arange(-r, r + 0.5, 0.5):
            q = p + t * nrm
            x, y = int(round(q[0])), int(round(q[1]))
            if 0 <= x < 640 and 0 <= y < 480 and D[y, x] > best:
                best, bp = float(D[y, x]), (x, y)
        out.append(dict(f=round(float(f), 1), d_H=round(2 * best / H, 4), d_px=round(2 * best, 2), mitte=bp, punkt=[round(float(v), 1) for v in p]))
    return out


def foto_breiten():
    roh = inkreis(foto_maske(), FS.NUTZER, H2)
    w = [r['d_H'] for r in roh]
    med = [float(np.median(w[max(i - 1, 0):i + 2])) for i in range(len(w))]
    for r, m in zip(roh, med):
        r['soll_H'] = round(m, 4)
    return roh


def kette_bild(mo, pose):
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    return b2.projiziere(pose, b2.kette_punkte(mo, kf))


def modell_breiten(mo, pose, ids=None, H=None):
    if ids is None:
        _img, ids, iv = G.zeichne(mo, pose, voll=True)
        H = G.h_port(iv)[2]
    kp = kette_bild(mo, pose)
    return inkreis(np.isin(ids, SCHWANZ), kp[1:], H), kp


def gelenk_f(kp):
    """f (Bogenanteil ab Gelenk 17 bis zur Spitze) jedes projizierten Gelenks kp[1:]."""
    q = np.asarray(kp[1:], float)
    s = np.r_[0, np.cumsum(np.linalg.norm(np.diff(q, axis=0), axis=1))]
    return s / s[-1]


# ------------------------------------------------------------------ Rot-Karte (T-F7; Messblatt Abschnitt 4: Rot = a* >= 10)
ROT = 10.0
REGIONEN = {1: {'Kopf (Kopfbox V4)': (368, 139, 416, 191), 'Brust Mitte (B-Mitte)': (360, 192, 450, 245),
                'Schulter/Oberarm Bild-links = (W)-rechts': (296, 175, 355, 250), 'Schulter/Oberarm Bild-rechts = (W)-links': (455, 178, 512, 265),
                'Unterarm/Hand Bild-rechts = (W)-links': (478, 265, 512, 372), 'Bauch/Huefte': (360, 250, 445, 300)},
            2: {'Kopf (Kopfbox V4)': (412, 190, 467, 263), 'Buckel/Ruecken oben': (330, 186, 410, 225),
                'Schulter/Oberarm Mitte = (W)-rechts': (360, 228, 430, 300), 'Schwanz Bogen': (280, 222, 335, 262)}}   # = f_zusatz.REG


def trimap(b):
    return np.load(os.path.join(SFV4, 'foto', 'trimap_%d.npy' % b))


def rot_karte(mo, b, pose=None):
    """Rot-Anteil Foto gegen Modell je fester Region (Messblatt 4.1, f_zusatz.REG) und je Koerperteil (Id-Puffer + f_analyse.teil,
    Messblatt 4.2: Pixel, die im Foto sicher Figur sind UND im Modell belegt). Foto 1 entblendet, Foto 2 roh; Modell Videoraum."""
    import f_rast as FR
    import f_analyse as FA
    pose = pose or V.posen()['P%d' % b]
    A = FR.mit_bg(mo, pose)
    bone = A['bone'][:480]; lok = A['lok'][:480]
    Lm = G.lab(G.videoraum(A['img'][:480])); Lf = G.lab(G.vorlage(b)); T = trimap(b)
    rf = Lf[..., 1] >= ROT; rm = Lm[..., 1] >= ROT
    reg = {}
    for n, (x0, y0, x1, y1) in REGIONEN[b].items():
        box = np.zeros(T.shape, bool); box[y0:y1 + 1, x0:x1 + 1] = True
        mf = box & (T == 1); mm = box & (bone >= 0)
        reg[n] = dict(foto=round(float(rf[mf].mean()), 3) if mf.any() else None, modell=round(float(rm[mm].mean()), 3) if mm.any() else None,
                      foto_px=int(mf.sum()), modell_px=int(mm.sum()))
    tk = np.full(bone.shape, '', object)
    for y, x in zip(*np.nonzero(bone >= 0)):
        tk[y, x] = FA.teil(bone[y, x], lok[y, x])
    beide = (T == 1) & (bone >= 0)
    teile = {}
    for t_ in sorted(set(tk[beide])):
        m = beide & (tk == t_)
        if m.sum() < 15:
            continue
        teile[t_] = dict(px=int(m.sum()), foto=round(float(rf[m].mean()), 3), modell=round(float(rm[m].mean()), 3))
    return dict(regionen=reg, teile=teile, rot_foto=rf & (T == 1), rot_modell=rm & (bone >= 0), bone=bone, img=A['img'][:480])
