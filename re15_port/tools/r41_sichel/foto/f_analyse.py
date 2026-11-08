"""SF Messblatt Fotovorlage, Schritte 3-6 (Modellseite + Zuordnung): Variante V5r4 in der gefundenen Pose (pose_<b>.json,
bester Feinwert) gegen Foto 1/2. Ausgabe sfv4/foto/analyse.json + Masken/Bilder fuer f_tafeln.py.
Farben im Videoraum: Modell -> G.videoraum (Gauss 1,5, V = a + b P), Foto 1 entblendet, Foto 2 roh. Rot = a* >= 10
(Figur-Median a* Foto 1 3,4 / Foto 2 4,5; P90 10,6 / 12,1).
"""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
import f_rast as FR
S = os.path.join(G.SFV4, 'foto')
ROT = 10.0
GRAU = 6.0
TEILE = {0: 'Becken', 1: 'Oberschenkel (W)-rechts', 2: 'Unterschenkel (W)-rechts', 3: 'Fuss (W)-rechts',
         4: 'Oberschenkel (W)-links', 5: 'Unterschenkel (W)-links', 6: 'Fuss (W)-links', 7: 'Bauch/Huefte',
         9: 'Kopf', 10: 'Oberarm (W)-links', 11: 'Unterarm (W)-links', 12: 'Klauenhand (W)-links',
         13: 'Oberarm (W)-rechts (Sichelarm)', 14: 'Unterarm (W)-rechts (Sichelarm)', 15: 'Sichel'}
for _k in range(16, 23):
    TEILE[_k] = 'Schwanz'


def teil(bone, lok):
    """Koerperteil je Pixel; Knochen 8 nach Knochenraum (Achsen aus skel.rel: -y oben (Kopf 9 bei y -971), +x vorn
    (Kopf x +111, Buckel bis x -1075), +z (W)-links (Arm 10 bei z +943, Arm 13 bei z -1136))."""
    if bone != 8:
        return TEILE.get(int(bone), '?')
    x, y, z = lok
    if y < -500 and z > 450:
        return 'Schulter (W)-links'
    if y < -500 and z < -450:
        return 'Schulter (W)-rechts'
    if x >= -100:
        return 'Brust'
    return 'Ruecken/Buckel'


GLEICH = 0.003


def pose_von(b):
    """Gewaehlte Vergleichsansicht: bester Gesamtwert; Gleichstand (Gesamtwert innerhalb GLEICH = 0,003 vom Besten)
    -> hoechste Silhouetten-IoU (Foto 2: c12/b48/g853 IoU 0,7465 statt c11/b7/g293 IoU 0,7332 bei 0,7100 gegen 0,7122;
    letztere legt den Sichel-Unterarm 2857 px in sicheren Hintergrund, erstere 585 px)."""
    F = json.load(open(os.path.join(S, 'pose_%d.json' % b)))['fein']
    if b == 1:
        # Vorn/hinten: Foto 1 zeigt die Klauenhand (W)-links Bild-rechts (rote Hand x 482-511, y 340-367) und die Sichel
        # Bild-links -> nur Posen mit Klauenhand-Anker >= 0,5. Verworfen: c7/b1/g3380 (297 Grad, Rueckansicht, Anker 0).
        F = [f for f in F if f['teile'].get('klauenhand', 0) >= 0.5]
    top = F[0]['gesamt']
    return max((f for f in F if f['gesamt'] >= top - GLEICH), key=lambda f: f['iou'])


def welt(mo, pose, b, P):
    """Knochenraum-Punkte -> Videopunkte."""
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    R, T = mo.pose(kf)
    Pm = (np.asarray(P, float) @ (R[b].astype(float) / 4096.0).T) + T[b].astype(float)
    return b2.projiziere(pose, Pm)


def uv_besitzer(mo):
    """Je (zeile, v, u): Menge der Flaechen (bone, key), deren UV-Polygon den Texel bedeckt (Mehrfachnutzung)."""
    own = {}
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        for uvs, vi in ((m.tuv, 0), (m.quv, 1)):
            for fi, uv in enumerate(uvs):
                po, zeile = (uv[5] & 0xF) * 128, (uv[2] >> 6) & 0x1FF
                pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if vi else [])
                p = np.array(pts, np.int32)
                if vi:
                    p = p[[0, 1, 3, 2]]
                x0, y0 = p.min(0); x1, y1 = p.max(0)
                mk = np.zeros((y1 - y0 + 1, x1 - x0 + 1), np.uint8)
                cv2.fillConvexPoly(mk, p - [x0, y0], 1)
                ys, xs = np.nonzero(mk)
                key = (b, ('v%d' if vi else 'd%d') % fi)
                for y, x in zip(ys + y0, xs + x0):
                    own.setdefault((int(zeile), int(y), int(x)), set()).add(key)
    return own


def r1(a, n=1):
    return [round(float(q), n) for q in np.ravel(a)]


def analyse(b, mo, pose, own):
    A = FR.mit_bg(mo, pose)
    img = A['img'][:480]; bone = A['bone'][:480]; lok = A['lok'][:480]
    zl, fl, uu, vv, ix = A['zeile'][:480], A['flaeche'][:480], A['u'][:480], A['v'][:480], A['idx'][:480]
    vm = G.videoraum(img); Lm = G.lab(vm)
    v = G.vorlage(b); Lf = G.lab(v)
    T = np.load(os.path.join(S, 'trimap_%d.npy' % b))
    ok, so, Hp = G.h_port(A['bone'])
    out = dict(pose=pose, H_port=int(Hp), oberkante_port=int(ok), sohle_port=int(so))
    np.save(os.path.join(S, 'ids_%d.npy' % b), bone)
    cv2.imwrite(os.path.join(S, 'modell_%d.png' % b), cv2.cvtColor(img, cv2.COLOR_RGB2BGR))
    # ---------------- Teile-Karte
    tk = np.full(bone.shape, '', object)
    for y, x in zip(*np.nonzero(bone >= 0)):
        tk[y, x] = teil(bone[y, x], lok[y, x])
    np.save(os.path.join(S, 'teile_%d.npy' % b), tk, allow_pickle=True)
    # ---------------- Rot-Karte
    rf = (Lf[..., 1] >= ROT) & (T == 1)
    rm = (Lm[..., 1] >= ROT) & (bone >= 0)
    np.save(os.path.join(S, 'rot_foto_%d.npy' % b), rf); np.save(os.path.join(S, 'rot_modell_%d.npy' % b), rm)
    beide = (T == 1) & (bone >= 0)
    tab = {}
    for t in sorted(set(tk[beide])):
        m = beide & (tk == t)
        n = int(m.sum())
        if n < 15:
            continue
        tab[t] = dict(px=n, foto_rot=round(float(rf[m].mean()), 3), modell_rot=round(float(rm[m].mean()), 3),
                      foto_lab=r1(Lf[m].mean(0)), modell_lab=r1(Lm[m].mean(0)),
                      foto_a50=round(float(np.median(Lf[m][:, 1])), 1), modell_a50=round(float(np.median(Lm[m][:, 1])), 1))
    nur = {}
    for t in sorted(set(tk[bone >= 0])):
        m = (tk == t) & (bone >= 0)
        nur[t] = dict(px=int(m.sum()), modell_rot=round(float(rm[m].mean()), 3))
    out['rot_teile'] = tab
    out['rot_modell_gesamt_je_teil'] = nur
    out['rot_anteil'] = dict(foto=round(float(rf[T == 1].mean()), 3), modell=round(float(rm[bone >= 0].mean()), 3),
                             ueberlapp_foto=round(float(rf[beide].mean()), 3), ueberlapp_modell=round(float(rm[beide].mean()), 3))
    # Abweichungen: Modell rot, Foto grau (a* < 6) an derselben Stelle -> Flaechen/Texel/CLUT
    ab = rm & (T == 1) & (Lf[..., 1] < GRAU)
    np.save(os.path.join(S, 'rot_ab_%d.npy' % b), ab)
    grp = {}
    for y, x in zip(*np.nonzero(ab)):
        k = (int(bone[y, x]), fl[y, x], int(zl[y, x]))
        g = grp.setdefault(k, dict(px=0, teil=tk[y, x], texel=set(), idx=set()))
        g['px'] += 1; g['texel'].add((int(vv[y, x]), int(uu[y, x]))); g['idx'].add(int(ix[y, x]))
    liste = []
    for (bn, key, zeile), g in sorted(grp.items(), key=lambda t: -t[1]['px']):
        if g['px'] < 6:
            continue
        tv = np.array(sorted(g['texel']))
        andere = set()
        for (q_v, q_u) in g['texel']:
            andere |= own.get((zeile, q_v, q_u), set())
        andere.discard((bn, key))
        z = zeile - mo.clut_y
        liste.append(dict(knochen=bn, flaeche=key, teil=g['teil'], px=g['px'], zeile=zeile,
                          u=[int(tv[:, 1].min()), int(tv[:, 1].max())], v=[int(tv[:, 0].min()), int(tv[:, 0].max())],
                          texel=len(tv), clut_idx=sorted(g['idx']),
                          clut_rgb={int(i): mo.lut[z][i][:3].tolist() for i in sorted(g['idx'])} if 0 <= z < mo.lut.shape[0] else {},
                          mitnutzer=sorted(['%d/%s' % a for a in andere])))
    out['rot_abweichung'] = liste
    # ---------------- Kopf
    m9 = bone == 9
    ys9, xs9 = np.nonzero(m9)
    out['kopf_box_modell'] = [int(xs9.min()), int(ys9.min()), int(xs9.max()), int(ys9.max())] if len(xs9) else None
    K = json.load(open(os.path.join(S, 'kopf_foto.json')))
    ex, ey = K['auge'][str(b)]['schwerpunkt']
    xi, yi = int(round(ex)), int(round(ey))
    seite = welt(mo, pose, 9, [[150, -150, -250], [150, -150, 250], [0, 0, 0]])
    out['kopf_seiten_projektion'] = dict(W_rechts=r1(seite[0]), W_links=r1(seite[1]), mitte=r1(seite[2]))
    dR = float(np.hypot(*(seite[0] - [ex, ey]))); dL = float(np.hypot(*(seite[1] - [ex, ey])))
    out['auge_foto'] = dict(punkt=[ex, ey], modell_knochen=int(bone[yi, xi]),
                            modell_lok=[round(float(q)) for q in lok[yi, xi]] if bone[yi, xi] >= 0 else None,
                            modell_zeile=int(zl[yi, xi]), modell_lab=r1(Lm[yi, xi]), modell_lab_5x5=r1(Lm[yi - 2:yi + 3, xi - 2:xi + 3].reshape(-1, 3).mean(0)),
                            foto_lab=K['auge'][str(b)]['lab'], abstand_W_rechts=round(dR, 1), abstand_W_links=round(dL, 1),
                            seite='(W)-rechts' if dR < dL else '(W)-links')
    # Maul (Zeile 487 am Kopf): sichtbar / Fussabdruck ohne Verdeckung / Verdecker
    mm = m9 & (zl == 487)
    F = FR.mit_bg(mo, pose, filt=lambda bn, key, zeile: bn == 9 and zeile == 487)
    fm = (F['bone'][:480] == 9)
    verd = fm & ~mm
    vd = {}
    for y, x in zip(*np.nonzero(verd)):
        k = '%d/%s/z%d' % (bone[y, x], fl[y, x], zl[y, x]); vd[k] = vd.get(k, 0) + 1
    ys, xs = np.nonzero(fm)
    out['maul_modell'] = dict(sichtbar_px=int(mm.sum()), fussabdruck_px=int(fm.sum()),
                              fussabdruck_box=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None,
                              verdeckt_px=int(verd.sum()), verdecker=dict(sorted(vd.items(), key=lambda t: -t[1])[:12]),
                              lab_sichtbar=r1(Lm[mm].mean(0)) if mm.any() else None,
                              hell_sichtbar_px=int((mm & (Lm[..., 0] > 45)).sum()))
    np.save(os.path.join(S, 'maul_sicht_%d.npy' % b), mm); np.save(os.path.join(S, 'maul_fuss_%d.npy' % b), fm)
    # Kopf-Zeilen sichtbar
    kz_ = {}
    for z_ in np.unique(zl[m9]):
        mk = m9 & (zl == z_)
        kz_[int(z_)] = dict(px=int(mk.sum()), lab=r1(Lm[mk].mean(0)))
    out['kopf_zeilen'] = kz_
    if b == 1:
        mz = K['maul_1']; y0, y1 = mz['band_y']; x0, x1 = mz['band_x']
        band = np.zeros_like(m9); band[y0:y1 + 1, x0:x1 + 1] = True
        zb = {}
        for y, x in zip(*np.nonzero(band & (bone >= 0))):
            k = '%d/z%d' % (bone[y, x], zl[y, x]); zb[k] = zb.get(k, 0) + 1
        out['maul_band_foto_im_modell'] = dict(band=[x0, y0, x1, y1], modell_lab=r1(Lm[band].mean(0)), foto_lab=mz['lab_band'], belegung=zb)
    # Haar (Foto) -> Modell
    hf = np.load(os.path.join(S, 'haar_%d.npy' % b))
    hz = {}
    for y, x in zip(*np.nonzero(hf & (bone >= 0))):
        k = '%d/z%d' % (bone[y, x], zl[y, x]); hz[k] = hz.get(k, 0) + 1
    hl = hf & (bone == 9)
    out['haar_foto_im_modell'] = dict(px=int(hf.sum()), belegung=hz,
                                      lok_mittel=[round(float(q)) for q in np.nanmean(lok[hl], 0)] if hl.any() else None,
                                      modell_lab=r1(Lm[hf].mean(0)), foto_lab=r1(Lf[hf].mean(0)))
    # obere Kontur
    ko = K['obere_kontur'][str(b)]
    kz = {}
    for xs_, yf in ko.items():
        x = int(xs_)
        col = np.nonzero(bone[:, x] >= 0)[0]
        if not len(col):
            continue
        ym = int(col.min())
        kz[x] = dict(foto=yf, modell=ym, knochen=int(bone[ym, x]), teil=tk[ym, x],
                     lok=[round(float(q)) for q in lok[ym, x]], d=(None if yf is None else yf - ym))
    out['obere_kontur'] = kz
    return out


def main():
    mo = b2.modell(G.EMD_V4)
    own = uv_besitzer(mo)
    R = {}
    for b in (1, 2):
        p = pose_von(b)
        pose = {k: p[k] for k in ('clip', 'bild', 'gier', 'x', 'z')}
        o = analyse(b, mo, pose, own)
        R[b] = o
        print(b, json.dumps({k: o[k] for k in ('pose', 'H_port', 'auge_foto', 'maul_modell', 'rot_anteil')}, default=str))
    json.dump(R, open(os.path.join(S, 'analyse.json'), 'w'), indent=1, default=str)


if __name__ == '__main__':
    main()
