"""Auftrag 3: Krallen-Vorschlag als Geometrie-Kandidat (nicht im Asset).
vorn links: 28 mit K5c-Geometrie zurueck. vorn rechts: 31 bleibt; 19 mit K5c-Lage zurueck und genau EIN Laengenfaktor entlang der eigenen
Achse (Wurzel -> Spitze) aus der gemessenen Nutzer-Spitze; 20 und 29 nur um ihre Wurzel (Knochen-Ursprung an der Hand) gedreht, Laenge wie K5c,
bis sie in Bild 2 ganz in der vom Schriftzug verdeckten Flaeche liegen und kein Pixel ueber sichtbarem Hintergrund (Annahme nach Nutzer-Wortlaut
"vorne rechts hat er wahrscheinlich auch 4 krallen. Man sieht es nur nicht wegen der Schrift."). hinten unveraendert.
Runde 2: 20 und 29 bleiben eingezogen (Asset), weil keine Drehung die Annahme erfuellt (Pruefer r1 Nr. 12); Frage an den Nutzer."""
import copy, json, os, struct, sys, math
import numpy as np, cv2
from scipy import ndimage as ndi
from scipy.optimize import minimize
from scipy.spatial.transform import Rotation as Rot
from g_basis import *
sys.path.insert(0, WT + "/re15_port/tools")
import form_emd as FE
import emd_ansichtsblatt as EA

OUT = sys.argv[1] if len(sys.argv) > 1 else ARBEIT + '/krallen_kandidat'
VAR = os.environ.get('KRALLEN_VARIANTE', 'A')  # A: ganz im Schriftzug (Auftrag); B: nur kein Pixel ueber sichtbarem Hintergrund
os.makedirs(OUT, exist_ok=True)
ka = kand(ASSET); kc = kand(K5C)
lage2, gier2, clip2, bild2 = ka.stand(2)
kam2 = P.kamera(2)
kf2 = ka.kf(clip2, bild2)


# ------------------------------------------------------------ Masken Bild 2 (Vorlage)
def wasserzeichen():
    v = vorlage(2); lab = cv2.cvtColor(v.astype(np.float32) / 255, cv2.COLOR_RGB2LAB)
    L, C = lab[..., 0], np.hypot(lab[..., 1], lab[..., 2])
    hell = (L >= 75) & (C <= 30)
    blk = np.zeros_like(hell); blk[434:480, 0:262] = True
    g = cv2.dilate((hell & blk).astype(np.uint8), np.ones((3, 3), np.uint8))
    W = cv2.morphologyEx(g, cv2.MORPH_CLOSE, np.ones((9, 9), np.uint8)) > 0
    return ndi.binary_fill_holes(W) & blk


def figur_v(n):
    s, t = s_t(n); A = (grund_rgba(n)[..., 3] > 128).astype(np.float32)
    M = np.array([[s, 0, t[0]], [0, s, t[1]]], np.float32)
    return cv2.warpAffine(A, M, (640, 480), flags=cv2.INTER_AREA) >= 0.5


WZ = wasserzeichen()
FIG2 = figur_v(2); FIG1 = figur_v(1)
BG2 = ~FIG2 & ~WZ
BG1 = ~FIG1


# ------------------------------------------------------------ Projektion (Nachbau v4f.dreiecke_id fuer ein Mesh)
def projiziere(mo, kf, lage, gier, kam, b, tv, qv, mesh):
    s_, c_ = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c_, 0, s_], [0, 4096, 0], [-s_, 0, c_]], dtype=np.int64)
    R, T = mo.pose(kf)
    W = (Y @ R[b]) >> 12
    t = ((Y @ T[b]) >> 12) + np.array(lage, dtype=np.int64)
    VW = (kam.V @ W) >> 12
    vT = ((kam.V @ t) >> 12) + kam.t
    aus = []
    for verts, faces, viereck in ((tv, mesh.tf, 0), (qv, mesh.qf, 1)):
        if not faces:
            continue
        v = np.asarray(verts, dtype=np.int64)[:, :3]
        q = (VW @ v.T) >> 12
        vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
        sx, sy = FB._gte(vx, vy, vz, kam.h)
        for fi, g in enumerate(faces):
            ie = g[1::2]
            if any(vz[i] < 64 for i in ie):
                continue
            p = [(int(sx[i]), int(sy[i])) for i in ie]
            m0 = FB._mac0(p[0], p[1], p[2])
            if (m0 < 0) if viereck else (m0 <= 0):
                continue
            idx = ((0, 1, 3), (0, 3, 2)) if viereck else ((0, 1, 2),)
            for a, bb, cc in idx:
                tief = (int(vz[ie[a]]) + int(vz[ie[bb]]) + int(vz[ie[cc]])) / 3.0
                x, y = FB.abbilden(np.array([p[a][0], p[bb][0], p[cc][0]], float) + FB.PHASE,
                                   np.array([p[a][1], p[bb][1], p[cc][1]], float) + FB.PHASE, kam.abbildung)
                aus.append((tief, np.stack([x, y], 1)))
    return aus


def tiefenbild(mo, kf, lage, gier, kam, ohne):
    """Maler-Tiefe je Pixel (Mitteltiefe des zuletzt gezeichneten Dreiecks), gleiche Reihenfolge wie rastern."""
    tris = v4f.dreiecke_id(mo, kf, lage, gier, kam, None)
    Z = np.full((480, 640), np.inf, np.float32)
    for tief, p, uv, zeile, farbe, tag in sorted(tris, key=lambda t: -t[0]):
        if tag[0] in ohne:
            continue
        x, y = FB.abbilden(np.array([q[0] + FB.PHASE for q in p], float), np.array([q[1] + FB.PHASE for q in p], float), kam.abbildung)
        m = dreieck_maske(np.stack([x, y], 1))
        if m is None:
            continue
        (y0, y1, x0, x1), mm = m
        Z[y0:y1, x0:x1][mm] = tief
    return Z


def dreieck_maske(pts):
    """Pixelmitten (i+0,5) innerhalb des Dreiecks (wie rastern_id), Rueckgabe (Fenster, Maske)."""
    x, y = pts[:, 0], pts[:, 1]
    fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
    if abs(fl) < 1e-9:
        return None
    x0 = max(int(np.floor(x.min())), 0); x1 = min(int(np.ceil(x.max())) + 1, 640)
    y0 = max(int(np.floor(y.min())), 0); y1 = min(int(np.ceil(y.max())) + 1, 480)
    if x0 >= x1 or y0 >= y1:
        return None
    PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
    w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
    w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
    w0 = 1 - w1 - w2
    return (y0, y1, x0, x1), (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)


def sichtbar(tris, Z):
    m = np.zeros((480, 640), bool)
    for tief, pts in tris:
        r = dreieck_maske(pts)
        if r is None:
            continue
        (y0, y1, x0, x1), mm = r
        m[y0:y1, x0:x1] |= mm & (tief < Z[y0:y1, x0:x1])
    return m


def ganz(tris):
    m = np.zeros((480, 640), bool)
    for tief, pts in tris:
        r = dreieck_maske(pts)
        if r is not None:
            (y0, y1, x0, x1), mm = r; m[y0:y1, x0:x1] |= mm
    return m


# ------------------------------------------------------------ Geometrie
def ecken(mesh):
    return np.array([x[:3] for x in mesh.tv], float), np.array([x[:3] for x in mesh.qv], float)


def normalen(mesh):
    return np.array([x[:3] for x in mesh.tn], float), np.array([x[:3] for x in mesh.qn], float)


def spitze(mesh):
    tv, qv = ecken(mesh); a = np.vstack([tv, qv]); i = np.linalg.norm(a, axis=1).argmax(); return a[i]


def als_int(a):
    return [tuple(int(round(v)) for v in r) + (0,) for r in a]


def drehe(mesh, Rm):
    tv, qv = ecken(mesh); tn, qn = normalen(mesh)
    m = copy.deepcopy(mesh)
    m.tv = als_int(tv @ Rm.T); m.qv = als_int(qv @ Rm.T)
    m.tn = als_int(tn @ Rm.T); m.qn = als_int(qn @ Rm.T)
    return m


def strecke(mesh, f):
    u = spitze(mesh); u = u / np.linalg.norm(u)
    tv, qv = ecken(mesh); tn, qn = normalen(mesh)
    S = np.eye(3) + (f - 1) * np.outer(u, u)
    Sn = np.eye(3) + (1 / f - 1) * np.outer(u, u)

    def nn(n):
        r = n @ Sn.T; return r / (np.linalg.norm(r, axis=1, keepdims=True) + 1e-9) * 4096
    m = copy.deepcopy(mesh)
    m.tv = als_int(tv @ S.T); m.qv = als_int(qv @ S.T); m.tn = als_int(nn(tn)); m.qn = als_int(nn(qn))
    return m


def proj_punkt(mo, b, p, n=2):
    k = ka; lage, gier, clip, bild = k.stand(n); kam = P.kamera(n); kf = k.kf(clip, bild)
    s_, c_ = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c_, 0, s_], [0, 4096, 0], [-s_, 0, c_]], dtype=np.int64)
    R, T = mo.pose(kf)
    W = (Y @ R[b]) >> 12; t = ((Y @ T[b]) >> 12) + np.array(lage, dtype=np.int64)
    VW = (kam.V @ W) >> 12; vT = ((kam.V @ t) >> 12) + kam.t
    q = (VW @ np.asarray(p, dtype=np.int64).reshape(3, 1)) >> 12
    vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
    sx, sy = FB._gte(vx, vy, vz, kam.h)
    x, y = FB.abbilden(np.array(sx, float) + FB.PHASE, np.array(sy, float) + FB.PHASE, kam.abbildung)
    return float(x[0]) - 0.5, float(y[0]) - 0.5  # Pixelmitte ganzzahlig (wie Nutzer-/Vorlagenkoordinaten)


def welt(mo, kf, b, pts):
    R, T = mo.pose(kf)
    return (R[b].astype(float) @ np.asarray(pts, float).T).T / 4096.0 + T[b].astype(float)


def hand_dreiecke(mo, kf, b=18):
    m = mo.meshes[b]; out = []
    for verts, faces, viereck in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
        if not faces:
            continue
        w = welt(mo, kf, b, np.array([x[:3] for x in verts], float))
        for g in faces:
            ie = list(g[1::2])
            if viereck:
                out += [w[[ie[0], ie[1], ie[3]]], w[[ie[0], ie[3], ie[2]]]]
            else:
                out.append(w[ie])
    return np.array(out)


def windungszahl(pkt, T):
    """verallgemeinerte Windungszahl (Jacobson 2013) fuer Punkte pkt (N x 3) und Dreiecke T (M x 3 x 3)."""
    a = T[None, :, 0, :] - pkt[:, None, :]; b = T[None, :, 1, :] - pkt[:, None, :]; c = T[None, :, 2, :] - pkt[:, None, :]
    la, lb, lc = (np.linalg.norm(x, axis=2) for x in (a, b, c))
    det = np.einsum('nmi,nmi->nm', a, np.cross(b, c))
    den = la * lb * lc + np.einsum('nmi,nmi->nm', a, b) * lc + np.einsum('nmi,nmi->nm', b, c) * la + np.einsum('nmi,nmi->nm', c, a) * lb
    return np.arctan2(det, den).sum(1) / (2 * np.pi)


def durchdringung(mo, kf, b, mesh, Hd, wurzel_r=200.0):
    tv, qv = ecken(mesh); a = np.unique(np.vstack([tv, qv]), axis=0)
    a = a[np.linalg.norm(a, axis=1) > wurzel_r]  # Wurzel sitzt planmaessig in der Hand
    w = welt(mo, kf, b, a)
    wz = windungszahl(w, Hd)
    return int((np.abs(wz) > 0.5).sum()), int(len(a))


# ------------------------------------------------------------ Bau
def main():
    mo_a = ka.mo; mo_c = kc.mo
    mo = copy.deepcopy(mo_a)
    ergebnis = dict(regel=__doc__.strip(), wasserzeichen_px_vorlage=int(WZ.sum()))
    # vorn links: 28 zurueck
    mo.meshes[28] = copy.deepcopy(mo_c.meshes[28])
    # vorn rechts: 19 mit K5c-Lage, ein Laengenfaktor aus der Nutzer-Spitze
    nutzer_spitze = (202.96, 457.33)  # Vorlage; Form 3 endet bei 301/613 (beschriftet) = Grundlage 19/613
    tip19 = spitze(mo_c.meshes[19])
    tip_k5c = proj_punkt(mo_a, 19, tip19)
    fs = np.linspace(0.30, 1.20, 361)
    d = [np.hypot(*(np.array(proj_punkt(mo_a, 19, tip19 * f)) - nutzer_spitze)) for f in fs]
    i = int(np.argmin(d)); f = float(fs[i])
    for _ in range(3):  # Feinsuche
        fs2 = np.linspace(f - 0.005, f + 0.005, 41)
        d2 = [np.hypot(*(np.array(proj_punkt(mo_a, 19, tip19 * g)) - nutzer_spitze)) for g in fs2]
        f = float(fs2[int(np.argmin(d2))])
    mo.meshes[19] = strecke(mo_c.meshes[19], f)
    tip_neu = proj_punkt(mo, 19, spitze(mo.meshes[19]))
    ergebnis['kralle19'] = dict(laengenfaktor=round(f, 4), achse='Knochen-Ursprung (Wurzel an der Hand) -> fernste Ecke (Spitze)',
                                spitze_k5c_vorlage=[round(tip_k5c[0], 1), round(tip_k5c[1], 1)], nutzer_spitze_vorlage=list(nutzer_spitze),
                                spitze_vorschlag_vorlage=[round(tip_neu[0], 1), round(tip_neu[1], 1)],
                                rest_abstand_px=round(float(np.hypot(tip_neu[0] - nutzer_spitze[0], tip_neu[1] - nutzer_spitze[1])), 2),
                                laenge_k5c=round(float(np.linalg.norm(tip19)), 1), laenge_vorschlag=round(float(np.linalg.norm(tip19) * f), 1))
    print('19', ergebnis['kralle19'])
    # 20 / 29: Runde 2 (Pruefer r1 Nr. 12): bleiben eingezogen wie im Spiel-Asset, bis der Nutzer Frage 4 beantwortet.
    # Runde 1 hatte 6000 Drehungen um die Wurzel geprueft: keine lag ganz im Schriftzug ohne Pixel ueber Hintergrund und ohne Handdurchdringung.
    ergebnis['kralle20_29'] = dict(stand='eingezogen wie im Spiel-Asset (unveraendert)',
                                   grund='Drehen um die Wurzel erfuellt die Annahme nicht (Runde 1: bestes Ergebnis 135 / 12 px ueber Hintergrund, 2 / 1 Ecken in der Hand, '
                                         'in Bild 1 330 / 683 px ueber Hintergrund); Entscheidung beim Nutzer (Frage)')
    # Durchdringung der eigenen Hand (Ecken ausserhalb r 200 um die Wurzel, verallgemeinerte Windungszahl > 0,5)
    ergebnis['durchdringung'] = {}
    for b, hand in ((19, 18), (28, 13)):
        pen, nv = durchdringung(mo_a, kf2, b, mo.meshes[b], hand_dreiecke(mo_a, kf2, hand))
        ergebnis['durchdringung'][str(b)] = dict(hand=hand, ecken_in_der_hand=pen, ecken_geprueft=nv)
    # EMD schreiben: Asset-EMD, nur Ecken/Normalen der Meshes 19, 20, 28, 29 ersetzt
    emd = bytearray(open(ASSET + '/BAHNSTEIG_FORM.EMD', 'rb').read())
    dir_off, dd = FE.verzeichnis(bytes(emd)); md1 = dd[7]
    for b in (19, 28):
        (tvo, tvc, tno, tnc, tfo, tfc, tuo, qvo, qvc, qno, qnc, qfo, qfc, quo) = struct.unpack_from('<14I', emd, md1 + 12 + b * 56)
        m = mo.meshes[b]
        assert len(m.tv) == tvc and len(m.qv) == qvc and len(m.tn) == tnc and len(m.qn) == qnc
        for off, arr in ((tvo, m.tv), (tno, m.tn), (qvo, m.qv), (qno, m.qn)):
            for k_, x in enumerate(arr):
                struct.pack_into('<4h', emd, md1 + 12 + off + k_ * 8, *[int(v) for v in x[:4]])
    open(OUT + '/BAHNSTEIG_FORM.EMD', 'wb').write(bytes(emd))
    open(OUT + '/BAHNSTEIG_FORM.TIM', 'wb').write(open(ASSET + '/BAHNSTEIG_FORM.TIM', 'rb').read())
    j = json.load(open(ASSET + '/form.json', encoding='utf-8')); j['name'] = 'Krallen-Kandidat v2 (nicht im Asset)'
    json.dump(j, open(OUT + '/form.json', 'w', encoding='utf-8'), ensure_ascii=False)
    # Gegenprobe: neu einlesen, nur die vier Meshes verschieden
    kk = P.Kandidat(OUT + '/form.json')
    diff = [b for b in range(len(kk.mo.meshes)) if ecken(kk.mo.meshes[b])[0].tolist() != ecken(ka.mo.meshes[b])[0].tolist()
            or ecken(kk.mo.meshes[b])[1].tolist() != ecken(ka.mo.meshes[b])[1].tolist()]
    roh_a = open(ASSET + '/BAHNSTEIG_FORM.EMD', 'rb').read()
    ergebnis['emd'] = dict(datei='BAHNSTEIG_FORM.EMD', meshes_mit_anderen_ecken=diff, bytes_verschieden=int(sum(1 for x, y in zip(roh_a, emd) if x != y)),
                           laenge_gleich=len(roh_a) == len(emd))
    ergebnis['ecken'] = {str(b): dict(tv=[list(x[:3]) for x in mo.meshes[b].tv], qv=[list(x[:3]) for x in mo.meshes[b].qv]) for b in (19, 28)}
    # Bild 1: liegt der Vorschlag dort ueber Hintergrund?
    kf1 = ka.kf(*ka.stand(1)[2:]); l1, g1 = ka.stand(1)[:2]; kam1 = P.kamera(1)
    Z1 = tiefenbild(mo, kf1, l1, g1, kam1, ohne=(19, 20, 28, 29))
    b1 = {}
    for b in (19, 20, 28, 29):
        for name, mm in (('asset', ka.mo), ('k5c', mo_c), ('vorschlag', mo)):
            tv, qv = ecken(mm.meshes[b])
            sm = sichtbar(projiziere(mo_a, kf1, l1, g1, kam1, b, tv, qv, mm.meshes[b]), Z1)
            b1.setdefault(str(b), {})[name] = dict(sichtbar_px=int(sm.sum()), ueber_hintergrund_px=int((sm & BG1).sum()))
    ergebnis['bild1_kontrolle'] = b1
    # Bild 2 je Stand
    Zb = tiefenbild(ka.mo, kf2, lage2, gier2, kam2, ohne=(14, 15, 28, 30, 19, 20, 29, 31))
    b2 = {}; masken = {}
    for name, mm in (('asset', ka.mo), ('k5c', mo_c), ('vorschlag', mo)):
        for b in (14, 15, 28, 30, 19, 20, 29, 31):
            tv, qv = ecken(mm.meshes[b])
            sm = sichtbar(projiziere(mo_a, kf2, lage2, gier2, kam2, b, tv, qv, mm.meshes[b]), Zb)
            b2.setdefault(name, {})[str(b)] = dict(sichtbar_px=int(sm.sum()), ueber_hintergrund_px=int((sm & BG2).sum()), im_schriftzug_px=int((sm & WZ).sum()))
            masken['%s_%d' % (name, b)] = sm
    ergebnis['bild2_je_stand'] = b2
    np.savez_compressed(OUT + '/krallen_masken_bild2.npz', wasserzeichen=WZ, hintergrund=BG2, figur=FIG2, **masken)
    json.dump(ergebnis, open(OUT + '/kandidat.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print(json.dumps({k: v for k, v in ergebnis.items() if k not in ('ecken', 'regel')}, ensure_ascii=False, indent=1))


if __name__ == '__main__':
    main()
