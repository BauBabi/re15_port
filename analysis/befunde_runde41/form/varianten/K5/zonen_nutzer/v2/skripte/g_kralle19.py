"""Runde 3 (Pruefer r2 Nr. 12): Kralle 19 und Ihre "Form links im Bild" (vorne_rechte_Hand_und_Kralle.png).
A  Auftrag (K5c-Lage, genau EIN Laengenfaktor entlang der eigenen Achse): gemessen, wie viel davon in Ihrer Form, in Ihrem Hand-Umriss und
   ueber sichtbarer Hand liegt; Durchdringung der Hand 18 mit einem Wurzelradius, der zur gekuerzten Laenge passt (200 x Faktor).
B  Gegenprobe "Lage an Ihre Form gebunden": K5c-Kralle 19 um ihre Wurzel (Knochen-Ursprung an der Hand) gedreht (2 Winkel, -90..90 Grad in
   3-Grad-Schritten, senkrecht zur K5c-Achse) und mit einem Laengenfaktor so, dass die Spitze <= 2 px neben Ihrem Strichende liegt; je
   Loesung dieselben Messgroessen. Nur gemessen; was in den Kandidaten kommt, steht in g_krallen.py.
Aufruf: python g_kralle19.py <ausgabe.json>"""
import sys, json, os
_ziel = sys.argv[1]
sys.argv = [sys.argv[0], os.path.join(os.path.dirname(_ziel), '_kk_unbenutzt')]
import numpy as np, cv2
from scipy.spatial.transform import Rotation as Rot
import g_krallen as G
from g_basis import *

ka, kc = G.ka, G.kc
MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))
NUTZER_SPITZE = np.array([202.96, 457.33])  # Strichende Ihrer Form (Vorlage), wie g_krallen.py


def in_v(mg):
    s, t = s_t(2)
    M = np.array([[s, 0, t[0]], [0, s, t[1]]], np.float32)
    return cv2.warpAffine(mg.astype(np.float32), M, (640, 480), flags=cv2.INTER_LINEAR) >= 0.5


FORM = in_v(MS['krallen_vorn_rechts_form3']); HAND = in_v(MS['hand_vorn_rechts'])
lage, gier, clip, bild = ka.stand(2); kam = P.kamera(2); kf = ka.kf(clip, bild)
OHNE = (14, 15, 28, 30, 19, 20, 29, 31)
Zb = G.tiefenbild(ka.mo, kf, lage, gier, kam, ohne=OHNE)
# sichtbare Hand 18 heute (Asset) als Maske
_t18 = G.projiziere(ka.mo, kf, lage, gier, kam, 18, *G.ecken(ka.mo.meshes[18]), ka.mo.meshes[18])
HAND18 = G.sichtbar(_t18, G.tiefenbild(ka.mo, kf, lage, gier, kam, ohne=OHNE + (18,)))
HD = G.hand_dreiecke(ka.mo, kf, 18)


def mass(mesh, f):
    sm = G.sichtbar(G.projiziere(ka.mo, kf, lage, gier, kam, 19, *G.ecken(mesh), mesh), Zb)
    pen, nv = G.durchdringung(ka.mo, kf, 19, mesh, HD, wurzel_r=200.0 * f)
    tip = G.proj_punkt(ka.mo, 19, G.spitze(mesh))
    return dict(sichtbar_px=int(sm.sum()), in_ihrer_form_px=int((sm & FORM).sum()), ihre_form_px=int(FORM.sum()),
                in_ihrem_handumriss_px=int((sm & HAND).sum()), verdeckt_sichtbare_hand18_px=int((sm & HAND18).sum()),
                ueber_hintergrund_px=int((sm & G.BG2).sum()), im_schriftzug_px=int((sm & G.WZ).sum()),
                ecken_in_hand18=pen, ecken_geprueft=nv, wurzelradius=round(200.0 * f, 1),
                spitze_vorlage=[round(tip[0], 1), round(tip[1], 1)], spitze_abstand_px=round(float(np.hypot(*(np.array(tip) - NUTZER_SPITZE))), 2)), sm


def main():
    m19 = kc.mo.meshes[19]; tip0 = G.spitze(m19); L0 = np.linalg.norm(tip0); u = tip0 / L0
    erg = dict(regel=__doc__.strip())
    f = json.load(open(ARBEIT + '/krallen_kandidat/kandidat.json', encoding='utf-8'))['kralle19']['laengenfaktor']
    a, sa = mass(G.strecke(m19, f), f)
    erg['A_k5c_lage_ein_faktor'] = dict(laengenfaktor=f, **a)
    k5, _ = mass(m19, 1.0)
    erg['k5c_unveraendert'] = k5
    print('A', erg['A_k5c_lage_ein_faktor']); print('K5c', k5)
    # B: Drehung um die Wurzel + Laengenfaktor, Spitze an Ihrem Strichende
    e1 = np.cross(u, [0, 1.0, 0]); e1 /= np.linalg.norm(e1); e2 = np.cross(u, e1)
    # Projektion vektorisiert (gleiche Ganzzahl-Rechnung wie g_krallen.proj_punkt, Pose einmal)
    s_, c_ = G.EA.sin_q12(gier), G.EA.cos_q12(gier)
    Y = np.array([[c_, 0, s_], [0, 4096, 0], [-s_, 0, c_]], dtype=np.int64)
    R, T = ka.mo.pose(kf)
    W = (Y @ R[19]) >> 12; t = ((Y @ T[19]) >> 12) + np.array(lage, dtype=np.int64)
    VW = (kam.V @ W) >> 12; vT = ((kam.V @ t) >> 12) + kam.t

    def proj_viele(pts):
        q = (VW @ np.asarray(pts, float).astype(np.int64).T) >> 12
        sx, sy = G.FB._gte(q[0] + vT[0], q[1] + vT[1], q[2] + vT[2], kam.h)
        x, y = G.FB.abbilden(np.asarray(sx, float) + G.FB.PHASE, np.asarray(sy, float) + G.FB.PHASE, kam.abbildung)
        return np.stack([x - 0.5, y - 0.5], 1)
    chk = proj_viele([tip0])[0]; ref = G.proj_punkt(ka.mo, 19, tip0)
    assert abs(chk[0] - ref[0]) < 1e-6 and abs(chk[1] - ref[1]) < 1e-6, (chk, ref)
    los = []
    fs = np.linspace(0.15, 1.6, 1451)
    for al in np.arange(-90, 91, 3):
        for be in np.arange(-90, 91, 3):
            Rm = (Rot.from_rotvec(np.radians(al) * e1) * Rot.from_rotvec(np.radians(be) * e2)).as_matrix()
            d = Rm @ u
            pp = proj_viele(d[None, :] * L0 * fs[:, None])
            dd = np.hypot(*(pp - NUTZER_SPITZE).T); i = int(dd.argmin())
            if dd[i] <= 2.0:
                los.append((al, be, float(fs[i]), float(dd[i]), Rm))
    print('Loesungen mit Spitze <= 2 px:', len(los))
    res = []
    for al, be, g, dist, Rm in los:
        mesh = G.strecke(G.drehe(m19, Rm), g)
        m_, _ = mass(mesh, g)
        res.append(dict(winkel=[int(al), int(be)], laengenfaktor=round(float(g), 4), **m_))
    erg['B_loesungen'] = len(res)
    ok = [r for r in res if r['ecken_in_hand18'] == 0 and r['verdeckt_sichtbare_hand18_px'] == 0 and r['ueber_hintergrund_px'] == 0]
    erg['B_ohne_hand_und_hintergrund'] = len(ok)
    key = lambda r: (r['verdeckt_sichtbare_hand18_px'] + r['ueber_hintergrund_px'] + 50 * r['ecken_in_hand18'], -r['in_ihrer_form_px'])
    erg['B_beste'] = sorted(res, key=key)[:10]
    erg['B_meiste_in_form'] = sorted(res, key=lambda r: (-r['in_ihrer_form_px'], key(r)))[:5]
    json.dump(erg, open(_ziel, 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    for r in erg['B_beste'][:5]:
        print('B', r)
    print('ok ohne Hand/Hintergrund', len(ok))


if __name__ == '__main__':
    main()
