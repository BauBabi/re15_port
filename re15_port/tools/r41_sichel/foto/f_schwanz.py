"""SF Messblatt Fotovorlage, Schritt 5: Schwanz in Foto 2 (n1435, roh) gegen V5r4 in der gefundenen Pose 2.

Foto: Maske = Bewegungs-/Chroma-Maske der Leser V4 (sfv4/schwanz_maske.npy | B_L3_masken.npz 'Mc'), x >= 270, y < 346.
Mittellinie = Nutzerpunkte Beginn (357,243) -> Biegung (295,252, Messblatt V4 Abschnitt 4) -> Knick (290,308) -> Ende (295,343);
11 Stationen gleichen Bogenabstands; je Station Lauf der Maske entlang der Normalen (+-40 px), der die Station enthaelt
(sonst der naechste Lauf): Breite, Mitte, Lab (roh). Bezug H2 = 253 (Messblatt V4).
Modell: Gelenkkette 16..22 + Spitze (b2.kette_punkte) projiziert = Mittellinie; Stationen gleich; Maske = sichtbare Ids 16-22.
Gliedgrenzen: Breite 4 px vor/nach jedem Gelenk entlang der Kette, Knickwinkel der Kette im Bild.
"""
import os, sys, json, numpy as np, cv2
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import v4_gemein as G, b2
import birkin_buehne as BB
import f_rast as FR
S = os.path.join(G.SFV4, 'foto')
H2 = G.H_ORIG[2]
NUTZER = [(357, 243), (295, 252), (290, 308), (295, 343)]


def stationen(poly, n=11):
    P = np.array(poly, float)
    seg = np.linalg.norm(np.diff(P, axis=0), axis=1); cum = np.r_[0, np.cumsum(seg)]
    out = []
    for f in np.linspace(0, 1, n):
        s = f * cum[-1]; i = min(int(np.searchsorted(cum, s, 'right') - 1), len(seg) - 1)
        t = (s - cum[i]) / seg[i]; p = P[i] + t * (P[i + 1] - P[i])
        d = (P[i + 1] - P[i]) / seg[i]
        out.append((f, p, d, s))
    return out, cum[-1]


def lauf(maske, p, d, r=40):
    n = np.array([-d[1], d[0]])
    ts = np.arange(-r, r + 0.5, 0.5)
    q = p[None] + ts[:, None] * n[None]
    xi, yi = np.round(q[:, 0]).astype(int), np.round(q[:, 1]).astype(int)
    ok = (xi >= 0) & (xi < 640) & (yi >= 0) & (yi < 480)
    v = np.zeros(len(ts), bool); v[ok] = maske[yi[ok], xi[ok]]
    if not v.any():
        return None
    runs = []; i = 0
    while i < len(v):
        if v[i]:
            j = i
            while j + 1 < len(v) and v[j + 1]: j += 1
            runs.append((ts[i], ts[j])); i = j + 1
        else:
            i += 1
    best = min(runs, key=lambda ab: 0 if ab[0] <= 0 <= ab[1] else min(abs(ab[0]), abs(ab[1])))
    a, b_ = best
    c = p + (a + b_) / 2 * n
    pts = p[None] + np.arange(a, b_ + 0.01, 0.5)[:, None] * n[None]
    return dict(breite=float(b_ - a + 0.5), mitte=c, pts=np.round(pts).astype(int))


def messe(maske, poly, img_lab, n=11):
    st, L = stationen(poly, n)
    R = []
    for f, p, d, s in st:
        l = lauf(maske, p, d)
        if l is None:
            R.append(dict(f=round(f, 2), punkt=G.r1(p), breite=None)); continue
        pts = l['pts']
        lab_ = img_lab[pts[:, 1], pts[:, 0]].mean(0) if img_lab is not None else None
        R.append(dict(f=round(f, 2), punkt=G.r1(p), mitte=G.r1(l['mitte']), breite_px=l['breite'],
                      breite_H=round(l['breite'] / H2, 3), lab=G.r1(lab_) if lab_ is not None else None))
    return R, L


def main():
    A_ = json.load(open(os.path.join(S, 'analyse.json')))
    pose = A_['2']['pose']
    Hp = A_['2']['H_port']
    mo = b2.modell(G.EMD_V4)
    v = G.vorlage(2); Lf = G.lab(v)
    TM = np.load(os.path.join(G.SFV4, 'schwanz_maske.npy')) | np.load(os.path.join(G.SFV4, 'B_L3_masken.npz'))['Mc']
    TM[:, :270] = False; TM[346:] = False
    foto, Lf_len = messe(TM, NUTZER, Lf)
    # Modell
    A = FR.mit_bg(mo, pose)
    bone = A['bone'][:480]; img = A['img'][:480]; vm = G.videoraum(img); Lm = G.lab(vm)
    MM = np.isin(bone, G.SCHWANZ)
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    K = b2.kette_punkte(mo, kf)
    kp = b2.projiziere(pose, K)
    modell, Lm_len = messe(MM, kp, Lm)
    for r in modell:
        if r.get('breite_px'): r['breite_H'] = round(r['breite_px'] / Hp, 3)
    # Gliedgrenzen
    gr = []
    for j in range(1, 7):        # Gelenk zwischen Glied 16+j-1 und 16+j = Punkt kp[j]
        a, c = kp[j - 1], kp[j + 1]
        d1 = (kp[j] - a) / np.linalg.norm(kp[j] - a); d2 = (c - kp[j]) / np.linalg.norm(c - kp[j])
        knick = float(np.degrees(np.arccos(np.clip(d1 @ d2, -1, 1))))
        vor = lauf(MM, kp[j] - 4 * d1, d1); nach = lauf(MM, kp[j] + 4 * d2, d2)
        gr.append(dict(grenze='%d/%d' % (15 + j, 16 + j), punkt=G.r1(kp[j]), knick_grad=round(knick, 1),
                       breite_vor_px=vor and vor['breite'], breite_nach_px=nach and nach['breite'],
                       sprung_px=(nach['breite'] - vor['breite']) if (vor and nach) else None,
                       sichtbar_vor=bool(vor), sichtbar_nach=bool(nach)))
    # Wurzel: Gelenk 16 (kp[0]) gegen Rumpf: Breite 4 px hinter dem Gelenk (Schwanz) und Rumpfdicke quer zur Kette davor
    d0 = (kp[1] - kp[0]) / np.linalg.norm(kp[1] - kp[0])
    body = bone >= 0
    w_s = lauf(MM, kp[0] + 4 * d0, d0); w_b = lauf(body, kp[0] - 4 * d0, d0)
    wurzel = dict(punkt=G.r1(kp[0]), breite_schwanz_px=w_s and w_s['breite'], dicke_figur_davor_px=w_b and w_b['breite'],
                  knochen_am_gelenk=int(bone[int(round(kp[0][1])), int(round(kp[0][0]))]))
    # Bezugsfarben
    tk = np.load(os.path.join(S, 'teile_2.npy'), allow_pickle=True)
    T = np.load(os.path.join(S, 'trimap_2.npy'))
    haut = (T == 1) & np.isin(tk, ['Ruecken/Buckel', 'Schulter (W)-rechts', 'Schulter (W)-links', 'Brust']) & (Lf[..., 1] < 6) & ~TM
    fs = TM & (Lf[..., 1] < 99)
    ref = b2.modell(G.REF2)
    A2 = FR.mit_bg(ref, pose)
    vm2 = G.videoraum(A2['img'][:480]); Lm2 = G.lab(vm2)
    sich = A2['bone'][:480] == 15; sich14 = A2['bone'][:480] == 14
    # Texelmittel Mesh 15 (Ref2), ungeleuchtet
    tex = []
    m15 = ref.meshes[15]
    for uvs, vi in ((m15.tuv, 0), (m15.quv, 1)):
        for uv in uvs:
            po, zeile = (uv[5] & 0xF) * 128, (uv[2] >> 6) & 0x1FF
            pts = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])] + ([(uv[9] + po, uv[10])] if vi else [])
            p = np.array(pts, np.int32)
            if vi: p = p[[0, 1, 3, 2]]
            mk = np.zeros((ref.texh, ref.texw), np.uint8); cv2.fillConvexPoly(mk, p, 1)
            z = zeile - ref.clut_y
            c = ref.lut[z][ref.tex[mk > 0]]
            tex.append(c[c[:, 3] > 0][:, :3])
    tex = np.vstack(tex).astype(float)
    farben = dict(
        foto_schwanz_gesamt=G.r1(Lf[TM].mean(0)), foto_schwanz_px=int(TM.sum()),
        foto_schwanz_a_kleiner_6=G.r1(Lf[TM & (Lf[..., 1] < 6)].mean(0)), anteil_a_kleiner_0=round(float((Lf[TM][:, 1] < 0).mean()), 3),
        foto_rumpfhaut_grau=G.r1(Lf[haut].mean(0)), foto_rumpfhaut_px=int(haut.sum()),
        ref2_sichel_mesh15_render=G.r1(Lm2[sich].mean(0)) if sich.any() else None, ref2_sichel_px=int(sich.sum()),
        ref2_sichelunterarm_mesh14_render=G.r1(Lm2[sich14].mean(0)) if sich14.any() else None,
        ref2_sichel_mesh15_texel=G.r1(G.lab(tex.reshape(-1, 1, 3)).reshape(-1, 3).mean(0)),
        modell_schwanz_render=G.r1(Lm[MM].mean(0)) if MM.any() else None, modell_schwanz_px=int(MM.sum()))
    # De00 Foto-Schwanz gegen Bezuege
    fsl = np.array(farben['foto_schwanz_gesamt'])
    for k in ('foto_rumpfhaut_grau', 'ref2_sichel_mesh15_render', 'ref2_sichel_mesh15_texel', 'modell_schwanz_render'):
        if farben.get(k): farben['dE00_schwanz_gegen_' + k] = round(G.de00(fsl, farben[k]), 2)
    # obere Kontur Wurzel -> Ruecken (Foto 2 x 327-400) und Modell gleiche Spalten: Stufen (|dy| >= 3 px zwischen Nachbarspalten)
    ok_ = A_['2']['obere_kontur']
    xs = sorted(int(x) for x in ok_)
    yf = {x: ok_[str(x)]['foto'] for x in xs}; ym = {x: ok_[str(x)]['modell'] for x in xs}
    def stufen(y):
        return [dict(x=x, dy=y[x + 1] - y[x]) for x in xs if x + 1 in y and y[x] is not None and y[x + 1] is not None and abs(y[x + 1] - y[x]) >= 3]
    out = dict(foto=dict(stationen=foto, achslaenge_px=round(Lf_len, 1), achslaenge_H=round(Lf_len / H2, 3), maske_px=int(TM.sum())),
               modell=dict(stationen=modell, achslaenge_px=round(Lm_len, 1), achslaenge_H=round(Lm_len / Hp, 3), kette_bild=[G.r1(q) for q in kp],
                           gliedgrenzen=gr, wurzel=wurzel, H_port=Hp),
               farben=farben, kontur_stufen=dict(foto=stufen(yf), modell=stufen(ym)))
    json.dump(out, open(os.path.join(S, 'schwanz.json'), 'w'), indent=1, default=str)
    np.save(os.path.join(S, 'schwanz_foto_maske.npy'), TM)
    print(json.dumps(out, default=str)[:6000])


if __name__ == '__main__':
    main()
