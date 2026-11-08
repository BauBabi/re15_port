"""SF V6 Kopf: gemeinsame Helfer (Posen, Render, Kopfausschnitte) fuer Bau-Pruefung, Riegel und Leisten.

Posen (Messblatt Fotovorlage SF_MESSBLATT_FOTO.md Abschnitt 2, f_analyse.pose_von; Daten SF_belege/foto/daten/pose_<b>.json):
  P1 = Foto 1 (Clip 20, Bild 139, Gier 1422), P2 = Foto 2 (Clip 12, Bild 48, Gier 861), dazu "135" (posen_v4) fuer Kopf/Maul von
  vorn (Messblatt: kein Kandidat bringt den Modellkopf in Foto 1 an die Foto-Kopfstelle; Kopf von vorn zusaetzlich Pose 135).
"""
import os, sys, json
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
R41 = os.path.dirname(HIER)
sys.path.insert(0, os.path.join(R41, 'v4')); sys.path.insert(0, os.path.join(R41, 'foto')); sys.path.insert(0, HIER)
import v4_gemein as G          # noqa: E402
import b2                       # noqa: E402

BAUM = G.BAUM
DATEN = os.path.join(BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'foto', 'daten')
GLEICH = 0.003


def pose_von(b):
    """wie f_analyse.pose_von, aber aus den committeten Daten (SF_belege/foto/daten/pose_<b>.json)."""
    F = json.load(open(os.path.join(DATEN, 'pose_%d.json' % b)))['fein']
    if b == 1:
        F = [f for f in F if f['teile'].get('klauenhand', 0) >= 0.5]
    top = F[0]['gesamt']
    p = max((f for f in F if f['gesamt'] >= top - GLEICH), key=lambda f: f['iou'])
    return {k: p[k] for k in ('clip', 'bild', 'gier', 'x', 'z')}


def posen():
    return {'P1': pose_von(1), 'P2': pose_von(2), '135': G.posen()['135']}


FOTO_VON = {'P1': 1, 'P2': 2, '135': 1}


def modell(pfad_oder_blob):
    return b2.modell(pfad_oder_blob)


def render(mo, pose, licht=True):
    img, ids = G.zeichne(mo, pose, licht=licht)
    return img, ids


def box_um(ids, b=9, rand=12):
    ys, xs = np.nonzero(ids == b)
    if not len(xs):
        return None
    return (max(int(xs.min()) - rand, 0), max(int(ys.min()) - rand, 0), min(int(xs.max()) + rand, 639), min(int(ys.max()) + rand, 479))


def uv_karte(mo, pose):
    """Je Bildpunkt der Kopfflaechen (Knochen 9): Texel x, Texel y (im TIM) und CLUT-Zeile; sonst -1. Drei Renders ohne Licht mit
    Kodier-Textur (Index = x bzw. y, Zeilen = Identitaet, Alpha 255) bzw. Kodier-Palette (alle Eintraege einer Zeile = Zeilennummer
    - 480, Alpha wie das Original); Verdeckung durch den Koerper wie im Bild."""
    lut0, tex0 = mo.lut, mo.tex
    out = []
    try:
        for art in ('x', 'y', 'zeile'):
            lut = lut0.copy()
            if art == 'zeile':
                for z in range(lut.shape[0]):
                    lut[z, :, :3] = z
                mo.tex = tex0
            else:
                lut[:, :, 0] = lut[:, :, 1] = lut[:, :, 2] = np.arange(256)[None]; lut[:, :, 3] = 255
                mo.tex = np.clip(np.indices(tex0.shape)[1 if art == 'x' else 0], 0, 255).astype(np.uint8)
            mo.lut = lut
            img, ids = G.zeichne(mo, pose, licht=False)
            out.append(np.where(ids == 9, img[..., 0].astype(int), -1))
    finally:
        mo.lut, mo.tex = lut0, tex0
    U, Vv, Z = out
    Z = np.where(Z >= 0, Z + mo.clut_y, -1)
    return U, Vv, Z


def kontur(ids):
    """Obere Kontur (Knochen 0-15) von der Buckelkuppe vor dem Kopf bis zur Kopf-Vorderkante (Bild-rechts, Pose P2): Kuppe, groesster
    Wiederanstieg (spaeterer Punkt hoeher als der tiefste fruehere), Abstand ueber der Foto-2-Kontur (kopf_foto.json), Kopf-Oberkante."""
    kopf = ids == 9
    xs = np.nonzero(kopf.any(0))[0]
    x_vorn = int(xs.max()); xk0 = int(xs.min())
    spalten = {}
    for x in range(max(xk0 - 80, 0), x_vorn + 1):
        col = np.nonzero((ids[:, x] >= 0) & (ids[:, x] < 16))[0]
        if len(col):
            spalten[x] = (int(col.min()), int(ids[col.min(), x]))
    vor = [x for x in spalten if x < xk0]
    xc = min(vor, key=lambda x: (spalten[x][0], -x))
    xs_ = [x for x in sorted(spalten) if x >= xc]
    best, stelle, mx = 0, (xc, spalten[xc][0], xc, spalten[xc][0]), None
    tief = (xc, spalten[xc][0])
    for x in xs_:
        y = spalten[x][0]
        if y > tief[1]:
            tief = (x, y)
        if tief[1] - y > best:
            best, stelle = tief[1] - y, (tief[0], tief[1], x, y)
    fk = json.load(open(os.path.join(DATEN, 'kopf_foto.json')))['obere_kontur']['2']
    ue = (0, -1, -1)
    for x in range(xc - 20, xk0):
        if str(x) in fk and fk[str(x)] is not None and x in spalten:
            dd = int(fk[str(x)]) - spalten[x][0]
            if dd > ue[0]:
                ue = (dd, x, spalten[x][1])
    xo = min((x for x in spalten if spalten[x][1] == 9), key=lambda x: spalten[x][0])
    return dict(kuppe=(xc, spalten[xc][0]), x_vorn=x_vorn, wiederanstieg=int(best), stelle=[int(v) for v in stelle], ueber_foto=ue,
                kopf_oben=(xo, spalten[xo][0], fk.get(str(xo))))
