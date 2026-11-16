"""Rote Streifen an der Hals-Unterseite (Spiel-Asset K5e_f1): welche Flaechen tragen warm-rote Texel, sind sie im Tiefenpuffer
sichtbar (echte Aussenflaeche -> Textur/Material) oder nur in der Maler-Ordnung (Durchstossen/Ordnung)?
Rot-Regel wie MESSBLATT_K5F 0.2: Lab a* > 12, C* > 15, Farbton > -10 Grad (gemessen am Texel ohne Licht).
Ansichten: Bild-2-Kamera (Clip 12 Bild 0), Spiel-Griff 5090 Schnitt 11 (Clip 27 Bild 20), und 'von unten' Kameras um die Figur."""
import json
import math
from collections import Counter
import cv2
from s_basis import *  # noqa: F401,F403
import material_lesen as ML  # noqa: E402  (eigene Hilfe: Material je Flaeche)


def rot(rgb):
    L = v4f.lab(rgb.reshape(-1, 3).astype(np.float32)).reshape(rgb.shape)
    a, b = L[..., 1], L[..., 2]
    C = np.hypot(a, b); h = np.degrees(np.arctan2(b, a))
    return (a > 12) & (C > 15) & (h > -10)


def flaechen_rot(k):
    """Je Flaeche (Teil|Viereck|Nr.): Anteil warm-roter Texel in ihrem UV-Dreieck/Viereck."""
    out = {}
    for b, m in enumerate(k.mo.meshes):
        for viereck, uvs in ((0, m.tuv), (1, m.quv)):
            for fi, uv in enumerate(uvs):
                po = (uv[5] & 0xF) * 128
                z = ((uv[2] >> 6) & 0x1FF) - k.tex.clut_y
                z = z if 0 <= z < k.tex.lut.shape[0] else 0
                pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if viereck else [])
                if viereck:
                    pts = [pts[0], pts[1], pts[3], pts[2]]
                msk = np.zeros((k.tex.h, k.tex.w), np.uint8)
                cv2.fillPoly(msk, [np.array(pts, np.int32)], 1)
                ys, xs = np.nonzero(msk)
                if len(xs) == 0:
                    out['%d|%d|%d' % (b, viereck, fi)] = (0.0, z); continue
                rgba = k.tex.lut[z][k.tex.px[ys, xs]]
                ok = rgba[:, 3] > 0
                r = rot(rgba[ok][:, :3][None])[0] if ok.any() else np.zeros(0, bool)
                out['%d|%d|%d' % (b, viereck, fi)] = (float(r.mean()) if len(r) else 0.0, z)
    return out


def kamera_blick(mitte, az, el, abstand=9000, h=208):
    a, e = math.radians(az), math.radians(el)
    pos = (mitte[0] - math.sin(a) * math.cos(e) * abstand, mitte[1] + math.sin(e) * abstand, mitte[2] + math.cos(a) * math.cos(e) * abstand)
    return FB.Kamera.aus_rid([int(round(c)) for c in pos], [int(round(c)) for c in mitte], h=h)


def schluessel(tag):
    b, fi, q, h = tag
    return '%d|%d|%d' % (b, q, fi)


def ansicht(k, kf, lage, gier, kam, frot, H=480, W=640, licht=None):
    tris = dreiecke_3d(k.mo, kf, lage, gier, kam, licht)
    R = rastern_beide(tris, k.tex, H, W, np.diag([2.0, 2.0, 1.0]))
    tm = tri_mesh(tris)
    rm = rot(R['rgb_maler']) & (R['maler_i'] >= 0)
    rp = rot(R['rgb_puffer']) & (R['puffer_i'] >= 0)
    # nur Teile 21/22/23 (Hals, Kopf, Kiefer)
    mm = np.where(R['maler_i'] >= 0, tm[np.maximum(R['maler_i'], 0)], -1)
    mp = np.where(R['puffer_i'] >= 0, tm[np.maximum(R['puffer_i'], 0)], -1)
    rm &= np.isin(mm, (21, 22, 23)); rp &= np.isin(mp, (21, 22, 23))
    fm = Counter(schluessel(tris[i][4]) for i in R['maler_i'][rm].tolist())
    fp = Counter(schluessel(tris[i][4]) for i in R['puffer_i'][rp].tolist())
    return dict(rot_maler=int(rm.sum()), rot_tiefenpuffer=int(rp.sum()), flaechen_maler=dict(fm.most_common(12)),
                flaechen_tiefenpuffer=dict(fp.most_common(12))), R, tris


if __name__ == '__main__':
    k = kand(ASSET)
    frot = flaechen_rot(k)
    mat = ML.material_je_flaeche()
    rote = {f: v for f, v in frot.items() if v[0] >= 0.5}
    erg = {'rote_flaechen_anzahl': len(rote),
           'rote_flaechen_je_teil': dict(Counter(TEILNAME[int(f.split('|')[0])] for f in rote)),
           'rote_flaechen_je_material': dict(Counter(mat.get(f, '?') for f in rote)),
           'rote_flaechen_je_zeile': dict(Counter(int(v[1]) for v in rote.values())),
           'sechs_k5f_flaechen': {f: dict(rotanteil=round(frot[f][0], 3), zeile=frot[f][1], material=mat.get(f, '?'))
                                  for f in ('22|1|1', '22|1|8', '22|1|9', '22|1|194', '22|1|201', '22|1|209')}}
    ansichten = {}
    # Spiel-Griff 5090 Schnitt 11, Clip 27 Bild 20; Bild-2-Pose
    R5090 = FB.Raum('5090'); s = R5090.schnitt(11)
    kam11 = FB.Kamera.aus_rid(s['pos'], s['ziel'], fov=s['fov'])
    for nm, (clip, bild, gier) in (('5090 Schnitt 11, Griff Clip 27 Bild 20', (27, 20, 3874)), ('5090 Schnitt 11, Bild-2-Pose Clip 12 Bild 0', (12, 0, 3874))):
        e, R, tris = ansicht(k, k.kf(clip, bild), (941, 0, -23776), gier, kam11, frot, licht=R5090.licht(11))
        ansichten[nm] = e
        np.savez_compressed(AUS + '/hals_%s.npz' % ('griff' if clip == 27 else 'bild2'), maler_i=R['maler_i'], puffer_i=R['puffer_i'],
                            rgb_maler=R['rgb_maler'], rgb_puffer=R['rgb_puffer'], tm=tri_mesh(tris),
                            tags=np.array([t[4] for t in tris]))
    # von unten (Ruhe-Clip 2 Bild 0 und Clip 12 Bild 0), 8 Richtungen, Hoehe -40 Grad (Kamera unter der Figur)
    for clip in (2, 12):
        su = Counter(); sp = Counter(); rm = rp = 0
        for az in range(0, 360, 45):
            kam = kamera_blick((0, -1200, 0), az, 40)
            e, _R, _t = ansicht(k, k.kf(clip, 0), (0, 0, 0), 0, kam, frot)
            rm += e['rot_maler']; rp += e['rot_tiefenpuffer']
            su.update(e['flaechen_maler']); sp.update(e['flaechen_tiefenpuffer'])
        ansichten['von unten, Clip %d Bild 0, 8 Richtungen' % clip] = dict(rot_maler=rm, rot_tiefenpuffer=rp, flaechen_maler=dict(su.most_common(12)),
                                                                           flaechen_tiefenpuffer=dict(sp.most_common(12)))
    erg['ansichten'] = ansichten
    json.dump(erg, open(AUS + '/hals.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print(json.dumps(erg, ensure_ascii=False, indent=1))
