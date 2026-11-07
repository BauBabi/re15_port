"""Riegel SF V5 Kopf aus RE2 (Bauer, 2026-11-07) - Exit 1, wenn eine Zeile rot ist.

    python riegel_re2kopf.py <EMD> [--basis EMD_ot3] [--lauf LAUFORDNER] [--json aus.json] [--md aus.md] [--txt aus.txt] [--kein-exit]

  T-K1  Koerper = Referenz 2 (riegel_ref2tex.tk1: Meshes 0-8, 10-15 bytegleich, CLUT-Zeilen 480-485 bytegleich, Koerper-Texel bytegleich).
  T-K2s Schwanz-Texel (Meshes 16-22) zeigen Ref2-Farben (riegel_ref2tex.tk2 nur Schwanz); Kopf ausgenommen (RE2-Palette, T-R1).
  T-K3  Schwanz ohne Quernaht/Bloecke (riegel_ref2tex.tk3, unveraendert Runde 3).
  T-R0  Aenderungsumfang gegen die Basis (EM30_V4ot3): Sektionen 0-6 bytegleich; MD1 nur Mesh 9 anders; Schwanz-Meshes 16-22 bytegleich;
        Texel ausserhalb der alten Kopfflaechen (TPage 128 v 160-223) und der neuen Orte (G2 / G3) bytegleich.
  T-R1  Kopf byte-true RE2: Mesh 9 = G2 Mesh 3 (kind 0x31) bis auf Skalierung/Drehung/Versatz (Ecken max. Rundungsabstand <= 1 E., Normalen
        gedreht <= 2 E., Flaechen-Indizes gleich, UVs = Quelle + Versatz bis auf die Maulflaechen (Zahl)); Texel im G2-Ort = G2-TIM bis auf
        die angedunkelten (Zahl, selbe Zeile); G3-Kachel bytegleich; CLUT-Zeilen 486/487 bytegleich den RE2-Zeilen (15-Bit-Rohwerte).
  T-R2  Kopfbox K10, Auge K4, Maul K7, Auge seitlich S2, Haarkante KS2 in den Messblatt-Fenstern (kopf_re2_mess).
  T-R3  Zaehne am Render sichtbar: Zahn-Texel (G3-Kachel L* > 60, Chroma < 20) in Pose 135 >= 20 Bildpunkte (Marker-Render) und helle
        unbunte Bildpunkte im Maulfenster des rohen Renders >= 5.
  T-R4  CLUT-Zeilen <= 16 (Port-Texturhoehe = 256 x Zeilen <= 4096, SDL_render.c:1266-1270).
  Form  riegel_ref2tex.form: G1, M1-M4, T1-T7, TA, TK, Boden (mit --lauf auch T7a-d).
Gegenprobe: die Basis (ot3) muss an T-R1 ROT sein (python riegel_re2kopf.py EM30_V4ot3.EMD --kein-exit).
"""
import os, sys, json, argparse, hashlib
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.dirname(os.path.dirname(HIER))
sys.path.insert(0, HIER); sys.path.insert(0, TOOLS); sys.path.insert(0, os.path.join(TOOLS, 'r41_sichel', 'v4'))
import v4_gemein as G
import b2
import emd_ansichtsblatt as EA
import birkin_formen as BF
import riegel_v4 as RV
import riegel_ref2tex as RR
import ref2tex_v4 as RT
import re2_leser as RL
import kopf_re2_bau as KB
import kopf_re2_mess as KM

zeile = RV.zeile
SOLL = dict(K10=((0.14, 0.18), (0.15, 0.20)), K4=((0.035, 0.065), (0.07, 0.12)), K7=((0.3, 0.7), (0.55, 1.0)),
            S2=((0.03, 0.07), (0.76, 0.82)), KS2_DL=5.0, ZAEHNE=20, HELL=5)


def tk1b(A, R):
    """T-K1b wie riegel_ref2tex, aber weitere Zeilen duerfen auch die RE2-Zeilen des Kopfes sein (486 = G2 Zeile 0, 487 = G3 Zeile 0; T-R1d prueft
    sie bytegleich) - die Zeilen 480-485 muessen bytegleich Referenz 2 sein."""
    import struct
    gleich = A['clut'].shape[0] >= 6 and bool((A['clut'][:6] == R['clut'][:6]).all())
    erlaubt = {}
    for kind, nm in ((KB.G2, 'G2 0x31 Zeile 0'), (KB.G3, 'G3 0x33 Zeile 0')):
        t = RL.rec(kind, 2)[2]; cw_, ch_ = struct.unpack_from('<HH', t, 16)
        erlaubt[nm] = np.frombuffer(t[20:20 + 2 * cw_], '<u2')
    weitere = []
    ok = gleich
    for zi in range(6, A['clut'].shape[0]):
        kopie = [480 + j for j in range(R['clut'].shape[0]) if (A['clut'][zi] == R['clut'][j]).all()]
        re2 = [nm for nm, w in erlaubt.items() if (A['clut'][zi] == w).all()]
        weitere.append('%d=%s' % (480 + zi, ('Kopie %s' % kopie) if kopie else (re2[0] if re2 else 'NEU')))
        ok &= bool(kopie or re2)
    return zeile('T-K1b CLUT = Ref2', 'Zeilen 480-485 bytegleich; weitere Zeilen nur als Kopie einer Ref2-Zeile oder als RE2-Kopfzeile (G2 / G3, T-R1d)',
                 'Zeilen 480-485 %s; TIM %d Zeilen; weitere %s' % ('gleich' if gleich else 'ABWEICHEND', A['clut'].shape[0], weitere or 'keine'), ok)


def tk2_schwanz(A, R):
    """T-K2 nur fuer den Schwanz (Meshes 16-22): Farben in der Ref2-Palette, Zeile = Ref2-Zeile oder Kopie."""
    ref = set(int(x) for x in np.unique(R['clut'] & 0x7FFF))
    mk = RR._flaechen_maske(A['ms'], list(G.SCHWANZ), A['tex'].shape)
    neu = ges = 0; zeilen_ok = True; z_ = []
    for z, m in mk.items():
        zi = z - 480
        kopie = [480 + j for j in range(R['clut'].shape[0]) if 0 <= zi < A['clut'].shape[0] and (A['clut'][zi] == R['clut'][j]).all()]
        zeilen_ok &= bool(kopie)
        werte = A['clut'][zi][A['tex'][m]] & 0x7FFF
        neu += int(sum(1 for w in werte if int(w) not in ref)); ges += int(m.sum()); z_.append('%d (%s)' % (z, kopie or 'NEU'))
    return [zeile('T-K2s Schwanz Ref2-Farben', 'Meshes 16-22: jeder Texel 15-Bit-Wert in der Ref2-CLUT; Zeile = Ref2-Zeile oder Kopie',
                  '%d von %d Texeln ausserhalb; Zeilen %s' % (neu, ges, ', '.join(z_)), neu == 0 and zeilen_ok)]


def tr0(emd, basis):
    a, v = open(emd, 'rb').read(), open(basis, 'rb').read()
    da, dv = EA.emd_dir(a), EA.emd_dir(v)
    gleich = [i for i in range(7) if a[da[i]:da[i + 1]] == v[dv[i]:dv[i + 1]]]
    ma, mv = EA.md1_parse(a[da[7]:da[8]]), EA.md1_parse(v[dv[7]:dv[8]])
    import machart_v3 as MV
    andere = [b for b in range(min(len(ma), len(mv))) if b != KB.KOPF and MV.mesh_bytes(ma[b]) != MV.mesh_bytes(mv[b])]
    A, V = RT.atlas(emd), RT.atlas(basis)
    clut_alt = A['clut'].shape[0] >= 6 and bool((A['clut'][:6] == V['clut'][:6]).all())
    diff = A['tex'] != V['tex']
    erlaubt = np.zeros(diff.shape, bool)
    kp = BF._variante_v4()['kopf_v4']; pb = 128 * (kp['page'] & 0xF)
    for ans in ('vorn', 'seite'):
        u0, u1 = kp[ans]['u']; v0, v1 = kp[ans]['v']; erlaubt[v0:v1 + 1, pb + u0:pb + u1 + 1] = True
    o = KB.G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']; erlaubt[o['v0']:o['v0'] + 90, x0:x0 + 126] = True
    o = KB.G3_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']
    erlaubt[o['v0']:o['v0'] + o['v'][1] - o['v'][0] + 1, x0:x0 + o['u'][1] - o['u'][0] + 1] = True
    aussen = int((diff & ~erlaubt).sum())
    return [zeile('T-R0 Aenderungsumfang gegen %s' % os.path.basename(basis), 'Sektionen 0-6 bytegleich; MD1 nur Mesh 9 anders; CLUT 480-485 gleich; '
                  'Texel nur in alten Kopfflaechen und G2-/G3-Orten geaendert',
                  'Sektionen gleich %s; andere Meshes abweichend %s; Meshes %d -> %d; CLUT 480-485 %s; Texel ausserhalb %d' % (
                      gleich, andere or 'keine', len(mv), len(ma), 'gleich' if clut_alt else 'ABWEICHEND', aussen),
                  gleich == list(range(7)) and not andere and clut_alt and aussen == 0 and len(ma) == len(mv))]


def tr1(emd, prot=None):
    """Kopf byte-true RE2 (bis auf die dokumentierte Transformation)."""
    p = (prot or {}).get('param') or KB.PARAM
    blob = open(emd, 'rb').read(); d = EA.emd_dir(blob)
    ms = EA.md1_parse(blob[d[7]:d[8]]); m9 = ms[KB.KOPF]
    mo2 = RL.modell(KB.G2); g = mo2.meshes[3]
    mo3 = RL.modell(KB.G3)
    tex, _pal, _cy = EA.tim_decode(blob[d[8]:])
    _cx, _cy2, _cw, ch, c, _e = BF._clut(blob[d[8]:])
    Z = []
    ok_n = len(m9.tv) == len(g.tv) and len(m9.tn) == len(g.tn) and len(m9.tf) == len(g.tf) and len(m9.qf) == len(g.qf)
    if ok_n:
        sk = np.array(p['skal'], float); R = KB.rot(p); t = np.array(p['versatz'], float)
        V = np.array([v[:3] for v in g.tv], float); Q = (V * sk) @ R.T + t
        dv = np.abs(np.array([v[:3] for v in m9.tv], float) - Q).max()
        N = np.array([n[:3] for n in g.tn], float); Nn = (N / sk) @ R.T
        Ln = np.linalg.norm(N, axis=1, keepdims=True); L2 = np.linalg.norm(Nn, axis=1, keepdims=True); L2[L2 == 0] = 1
        dn = np.abs(np.array([n[:3] for n in m9.tn], float) - Nn / L2 * Ln).max()
        fl = list(m9.tf) == list(g.tf) and list(m9.qf) == list(g.qf)
        du, dv_ = KB.G2_ORT['u0'] - KB.G2_ORT['u'][0], KB.G2_ORT['v0'] - KB.G2_ORT['v'][0]
        uv_ab = 0; maul = 0
        for i, (a, b) in enumerate(list(zip(m9.tuv, g.tuv))):
            if (a[2] >> 6) == KB.ZEILE_G3:
                maul += 1; continue
            if not all(a[3 * k] == b[3 * k] + du and a[3 * k + 1] == b[3 * k + 1] + dv_ for k in range(3)) or (a[2] >> 6) != KB.ZEILE_G2:
                uv_ab += 1
        for a, b in zip(m9.quv, g.quv):
            if (a[2] >> 6) == KB.ZEILE_G3:
                maul += 1; continue
            if not all(a[3 * k] == b[3 * k] + du and a[3 * k + 1] == b[3 * k + 1] + dv_ for k in range(4)) or (a[2] >> 6) != KB.ZEILE_G2:
                uv_ab += 1
        Z.append(zeile('T-R1a Kopf-Geometrie = G2 Mesh 3 (0x31)', 'Ecken = Rz Ry S v + t (Abstand <= 1 E.), Normalen gedreht (<= 2 E.), Flaechen-Indizes gleich, '
                       'UV = Quelle + (%d, %d) ausser Maulflaechen (%d Dreiecke, G3-Kachel)' % (du, dv_, len(KB.MAUL_T) + len(KB.MAUL_Q)),
                       '%d Ecken / %d Normalen / %d Tri / %d Quad; Ecken max %.2f E., Normalen max %.2f, Flaechen %s, UV abweichend %d, Maul %d' % (
                           len(m9.tv), len(m9.tn), len(m9.tf), len(m9.qf), dv, dn, 'gleich' if fl else 'ABWEICHEND', uv_ab, maul),
                       dv <= 1.0 and dn <= 2.0 and fl and uv_ab == 0 and maul == len(KB.MAUL_T) + len(KB.MAUL_Q)))
    else:
        Z.append(zeile('T-R1a Kopf-Geometrie = G2 Mesh 3 (0x31)', 'Ecken / Normalen / Flaechen wie G2 Mesh 3 (48 / 48 / 56 Tri / 14 Quad)',
                       '%d Ecken / %d Normalen / %d Tri / %d Quad' % (len(m9.tv), len(m9.tn), len(m9.tf), len(m9.qf)), False))
    # Texel
    o = KB.G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']
    neu = tex[o['v0']:o['v0'] + 90, x0:x0 + 126] if tex.shape[0] >= o['v0'] + 90 else np.zeros((90, 126), np.uint8)
    quelle = mo2.tex[o['v'][0]:o['v'][1] + 1, o['u'][0]:o['u'][1] + 1]
    abw = neu != quelle
    pal2 = mo2.pal[0]
    z2 = KM._lab(pal2)
    selbe = int(sum(1 for y, x in zip(*np.nonzero(abw)) if neu[y, x] != 0 and (pal2[neu[y, x]] != 0).any()))
    dunkler = int(sum(1 for y, x in zip(*np.nonzero(abw)) if z2[neu[y, x], 0] <= z2[quelle[y, x], 0]))
    haar_prot = (prot or {}).get('haar', {})
    Z.append(zeile('T-R1b Kopf-Texel = G2-TIM (u 0-125 v 0-89) bis auf Andunkeln', 'abweichende Texel = angedunkelte (Protokoll %s), alle mit Eintrag derselben G2-Zeile, '
                   'kein Texel heller als die Quelle' % haar_prot.get('texel_geaendert', '?'),
                   '%d von %d Texeln abweichend; davon Eintrag der G2-Zeile %d; dunkler/gleich %d; unveraendert %d' % (
                       int(abw.sum()), quelle.size, selbe, dunkler, int((~abw).sum())),
                   int(abw.sum()) == selbe == dunkler and (haar_prot.get('texel_geaendert') in (None, int(abw.sum())))))
    o3 = KB.G3_ORT; x3 = 128 * (o3['page'] & 0xF) + o3['u0']
    hk, wk = o3['v'][1] - o3['v'][0] + 1, o3['u'][1] - o3['u'][0] + 1
    k_neu = tex[o3['v0']:o3['v0'] + hk, x3:x3 + wk] if tex.shape[0] >= o3['v0'] + hk else np.zeros((hk, wk), np.uint8)
    k_q = mo3.tex[o3['v'][0]:o3['v'][1] + 1, o3['u'][0]:o3['u'][1] + 1]
    Z.append(zeile('T-R1c Zahn-Kachel = G3-TIM (u %d-%d v %d-%d)' % (o3['u'] + o3['v']), 'bytegleich', '%d von %d Texeln abweichend' % (int((k_neu != k_q).sum()), k_q.size),
                   bool((k_neu == k_q).all())))

    def clut_roh(mo):
        import struct
        t = mo.tim; cs_, = struct.unpack_from('<I', t, 8); cw_, ch_ = struct.unpack_from('<HH', t, 16)
        return np.frombuffer(t[20:20 + 2 * cw_ * ch_], '<u2').reshape(ch_, cw_)
    r2, r3 = clut_roh(mo2)[0], clut_roh(mo3)[0]
    ok2 = ch > KB.ZEILE_G2 - 480 and bool((c[KB.ZEILE_G2 - 480] == r2).all())
    ok3 = ch > KB.ZEILE_G3 - 480 and bool((c[KB.ZEILE_G3 - 480] == r3).all())
    Z.append(zeile('T-R1d CLUT-Zeilen 486 / 487 = RE2', 'Zeile 486 = G2 (0x31) CLUT-Zeile 0, Zeile 487 = G3 (0x33) CLUT-Zeile 0, 15-Bit-Rohwerte bytegleich',
                   'TIM %d Zeilen; 486 %s; 487 %s' % (ch, 'gleich' if ok2 else 'ABWEICHEND', 'gleich' if ok3 else 'ABWEICHEND'), ok2 and ok3))
    return Z


def tr2_tr3(mo, M):
    a, b = M['135'], M['85']
    Z = []
    k10 = a.get('K10', (0, 0))
    Z.append(zeile('T-R2 K10 Kopfbox vorn', '0,14-0,18 x 0,15-0,20 H (Messblatt; ot3 0,154 x 0,187)', '%.3f x %.3f H (Box %s, H %.0f)' % (k10[0], k10[1], a.get('kopfbox'), a['H']),
                   SOLL['K10'][0][0] <= k10[0] <= SOLL['K10'][0][1] and SOLL['K10'][1][0] <= k10[1] <= SOLL['K10'][1][1]))
    k4 = a.get('K4')
    Z.append(zeile('T-R2 K4 Augenlage vorn', '(B)-links 0,035-0,065 H; 0,07-0,12 H unter OK (Marker = Kern des (W)-rechten G2-Auges, Flaechen T19/T20/T41)',
                   ('%.3f H links; %.3f H unter OK (%d px)' % (k4[0], k4[1], a['auge']['px'])) if k4 else 'kein Auge sichtbar',
                   bool(k4) and SOLL['K4'][0][0] <= k4[0] <= SOLL['K4'][0][1] and SOLL['K4'][1][0] <= k4[1] <= SOLL['K4'][1][1]))
    z = a['zaehne']
    Z.append(zeile('T-R2 K7 Maul-Lage vorn', 'Zahn-Schwerpunkt bei x 0,3-0,7 / y 0,55-1,0 der Kopfbox (unter dem Auge, Mitte; Leser B: Maul an der Unterseite)',
                   ('%.2f / %.2f (%d px)' % (z['anteil'][0], z['anteil'][1], z['px'])) if z['px'] else 'keine Zaehne sichtbar',
                   bool(z['px']) and SOLL['K7'][0][0] <= z['anteil'][0] <= SOLL['K7'][0][1] and SOLL['K7'][1][0] <= z['anteil'][1] <= SOLL['K7'][1][1]))
    s2 = b.get('S2')
    Z.append(zeile('T-R2 S2 Augenlage seitlich', '0,03-0,07 H hinter der Vorderkante; 0,76-0,82 H ueber der Sohle',
                   ('%.3f H; %.3f H (%d px)' % (s2[0], s2[1], b['auge']['px'])) if s2 else 'kein Auge sichtbar',
                   bool(s2) and SOLL['S2'][0][0] <= s2[0] <= SOLL['S2'][0][1] and SOLL['S2'][1][0] <= s2[1] <= SOLL['S2'][1][1]))
    ks2 = b['KS2']['kopf'][0]; med = b['kopf_p50_v']
    Z.append(zeile('T-R2 KS2 Haarkante seitlich', 'Fenster KS2 (0,84-0,96 / 0,52-0,80 der Kopfbox), nur Kopf-Bildpunkte: L* <= Kopf-Median (Pose 85, Videoraum %.1f) - 5 '
                   '(die Schnauze fuellt die untere Vorderecke der Box nicht, ganzes Fenster nur Ausgabe)' % med,
                   'Kopf L* %.1f (%.0f %% des Fensters; ganzes Fenster %.1f; Box %s)' % (ks2, 100 * b['KS2']['kopf_anteil'], b['KS2']['lab'][0], b['KS2']['box']),
                   ks2 <= med - SOLL['KS2_DL']))
    Z.append(zeile('T-R3 Zaehne sichtbar (Pose 135)', 'Zahn-Texel (G3-Kachel L* > 60, Chroma < 20) >= %d Bildpunkte im Marker-Render; helle unbunte Bildpunkte '
                   '(L* > 45, Chroma < 20) im Kopf unterhalb 0,5 der Kopfbox im rohen Render >= %d' % (SOLL['ZAEHNE'], SOLL['HELL']),
                   '%d Marker-Bildpunkte; %d helle Bildpunkte; Pose 85: %d Marker' % (z['px'], a.get('hell_unten', -1), b['zaehne']['px']),
                   z['px'] >= SOLL['ZAEHNE'] and a.get('hell_unten', 0) >= SOLL['HELL']))
    return Z


def tr4(emd):
    blob = open(emd, 'rb').read(); d = EA.emd_dir(blob)
    _cx, _cy, _cw, ch, _c, _e = BF._clut(blob[d[8]:])
    _tex, _p, _ = EA.tim_decode(blob[d[8]:])
    return [zeile('T-R4 CLUT-Zeilen', '<= 16 Zeilen; Port-Texturhoehe TIM-Hoehe x Zeilen <= 4096 (render_pc.c ~2598-2606, SDL_render.c:1266-1270)',
                  '%d Zeilen; %d x %d = %d' % (ch, _tex.shape[0], ch, _tex.shape[0] * ch), ch <= 16 and _tex.shape[0] * ch <= 4096)]


def hell_unten(mo, M):
    """helle unbunte Bildpunkte im Kopf unterhalb 0,5 der Kopfbox (roher Render Pose 135)."""
    a = M['135']; img, ids, kb = a['img'], a['ids'], a['kopfbox']
    Lab = G.lab(img.astype(float)); km = ids == KB.KOPF
    ym = 0.5 * (kb[1] + kb[3]); yy = np.arange(img.shape[0])[:, None] * np.ones((1, img.shape[1]))
    m = km & (yy >= ym) & (Lab[..., 0] > 45) & (np.hypot(Lab[..., 1], Lab[..., 2]) < 20)
    a['hell_unten'] = int(m.sum())


def pruefen(emd, basis=None, lauf=None, prot=None):
    P = G.posen()
    R = RT.atlas(G.REF2); A = RT.atlas(emd)
    mo = b2.modell(emd)
    Z = []
    for z in RR.tk1(A, R):
        if z['mass'].startswith('T-K1b'):
            z = tk1b(A, R)
        Z.append(z)
    Z += tk2_schwanz(A, R)
    z3, i3 = RR.tk3(mo, P, A); Z += z3
    if basis:
        Z += tr0(emd, basis)
    Z += tr1(emd, prot)
    M = KM.messen(mo, P, bilder=True)
    hell_unten(mo, M)
    Z += tr2_tr3(mo, M)
    Z += tr4(emd)
    zf, st = RR.form(emd, mo, P, lauf); Z += zf
    info = dict(tk3={k: v for k, v in i3.items() if k != 'bild'}, mess={h: {k: v for k, v in r.items() if k not in ('img', 'ids', 'ma', 'mz')} for h, r in M.items()},
                schwanz={k: v for k, v in st.items() if k not in ('C', 'breite', 'maske')})
    return Z, info, M


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd'); ap.add_argument('--basis', default=KB.BASIS); ap.add_argument('--lauf')
    ap.add_argument('--prot', help='Protokoll-JSON des Baus (Parameter, Haar-Umfang)')
    ap.add_argument('--json'); ap.add_argument('--md'); ap.add_argument('--txt'); ap.add_argument('--kein-exit', action='store_true')
    a = ap.parse_args()
    prot = json.load(open(a.prot)) if a.prot and os.path.exists(a.prot) else None
    Z, info, _M = pruefen(a.emd, a.basis if a.basis and os.path.exists(a.basis) and os.path.abspath(a.basis) != os.path.abspath(a.emd) else None, a.lauf, prot)
    rot = [z for z in Z if not z['ok']]
    sha = hashlib.sha256(open(a.emd, 'rb').read()).hexdigest()
    lines = ['EMD %s sha256 %s (Ref2 %s; Basis %s)' % (a.emd, sha[:8], hashlib.sha256(open(G.REF2, 'rb').read()).hexdigest()[:8],
                                                       hashlib.sha256(open(a.basis, 'rb').read()).hexdigest()[:8] if a.basis and os.path.exists(a.basis) else '-')]
    for z in Z:
        lines.append('%-4s %-46s soll: %-60s ist: %s %s' % ('INFO' if z.get('info') else ('OK' if z['ok'] else 'ROT'), z['mass'], z['soll'], z['ist'],
                                                           ('[' + z['note'] + ']') if z.get('note') else ''))
    lines.append('ROT: %d von %d (INFO-Zeilen nicht gezaehlt: %d)' % (len(rot), len([z for z in Z if not z.get('info')]), len([z for z in Z if z.get('info')])))
    print('\n'.join(lines))
    if a.txt:
        open(a.txt, 'w', encoding='utf-8').write('\n'.join(lines) + '\nEXIT=%d\n' % (1 if rot else 0))
    if a.json:
        json.dump(dict(emd=a.emd, sha256=sha, zeilen=Z, info=info), open(a.json, 'w'), indent=1,
                  default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    if a.md:
        with open(a.md, 'w', encoding='utf-8') as f:
            f.write('EMD sha256 %s\n\n| Mass | Soll | Ist | |\n|---|---|---|---|\n' % sha[:8])
            for z in Z:
                f.write('| %s | %s | %s | %s |\n' % (z['mass'], z['soll'], z['ist'] + ((' (' + z['note'] + ')') if z.get('note') else ''),
                                                     'INFO' if z.get('info') else ('OK' if z['ok'] else 'ROT')))
    if rot and not a.kein_exit:
        sys.exit(1)


if __name__ == '__main__':
    main()
