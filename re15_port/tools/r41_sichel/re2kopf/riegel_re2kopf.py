"""Riegel SF V5 Kopf aus RE2 (Bauer, Runde 2, 2026-11-07) - Exit 1, wenn eine Zeile rot ist.

    python riegel_re2kopf.py <EMD> [--basis EMD_ot3] [--lauf LAUFORDNER] [--prot PROT.json] [--json aus.json] [--md aus.md] [--txt aus.txt] [--kein-exit]

  T-K1  Koerper = Referenz 2 (riegel_ref2tex.tk1: Meshes 0-8, 10-15 bytegleich, CLUT-Zeilen 480-485 bytegleich, Koerper-Texel bytegleich).
  T-K2s Schwanz-Texel (Meshes 16-22) zeigen Ref2-Farben (riegel_ref2tex.tk2 nur Schwanz); Kopf ausgenommen (RE2-Palette, T-R1).
  T-K3  Schwanz ohne Quernaht/Bloecke (riegel_ref2tex.tk3, unveraendert Runde 3).
  T-R0  Aenderungsumfang gegen die Basis (EM30_V4ot3): Sektionen 0-6 bytegleich; MD1 nur Mesh 9 anders; Schwanz-Meshes 16-22 bytegleich;
        Texel ausserhalb der alten Kopfflaechen (TPage 128 v 160-223) und der neuen Orte (G2 / G3) bytegleich.
  T-R1  Kopf byte-true RE2: a) Mesh 9 = G2 Mesh 3 (kind 0x31) bis auf Skalierung/Drehung/Versatz (Ecken <= 1 E., Normalen <= 2 E., Flaechen-
        Indizes gleich, UVs = Quelle + Versatz ausser Maulflaechen; CLUT-Zeile je Flaeche nach der Zonenregel des Protokolls); b) Texel im
        G2-Ort BYTEGLEICH der G2-TIM (Runde 2: kein Andunkeln mehr); c) G3-Kachel bytegleich; d) CLUT-Zeile 486 / 488 = G2-Zeile 0 nach der
        dokumentierten Palettentransformation (L* x k, Chroma x g, L* <= lmax; 15-Bit-Worte bytegleich der Nachrechnung), 487 = G3-Zeile 0.
  T-R2  Kopfbox K10, Auge K4 (Marker), Maul K7, Auge seitlich S2, Haarkante KS2 (Messblatt-Fenster, kopf_re2_mess); dazu Runde 2:
        K1k Auge sichtbar nach KI-Lesung D3 (dunkle Pupille: Augenfaecher T19/T20/T41 im Videoraum dunkler als die Umgebung, Glanzkern heller
        als der Faecher), K5 dunkle Seite Bild-rechts (Haelften-Median rechts <= 0,85 x links, Videoraum und RDT-Licht; Fenster KV2 L* in
        0,62-0,90 x Kopf-Median und <= Ref2-Kopf P50 - 8, a* > 0, b* > 0), K6 zweites Auge verdeckt (Spiegelbox des Faechers <= Kopf-Median),
        T-K9e Kopf L* P50 >= 23 und Kopf/Rumpf >= 0,9 (roher Render mit RDT-Licht, Pose 135).
  T-R3  Zaehne am Render sichtbar: Zahn-Texel (G3-Kachel L* > 60, Chroma < 20) in Pose 135 >= 20 Bildpunkte (Marker-Render) und helle
        unbunte Bildpunkte im Maulfenster des rohen Renders >= 5.
  T-R4  CLUT-Zeilen <= 16 (Port-Texturhoehe = 256 x Zeilen <= 4096, SDL_render.c:1266-1270).
  INFO  Messblatt-Methode K1/K3/K4 (hellster Kopfpunkt = Augenfleck) und der Lichtfaktor des Augenfaechers; KV1 a* (K9); Kopf/Brust.
  Form  riegel_ref2tex.form: G1, M1-M4, T1-T7, TA, TK, Boden (mit --lauf auch T7a-d).
Gegenprobe: die Basis (ot3) muss an T-R1 ROT sein (python riegel_re2kopf.py EM30_V4ot3.EMD --kein-exit).

Runde 3 (Nutzer 2026-11-08: Feuerflecken/helle Kratzer weg, Seite einfach grau; kein spitzer Schnabel, vorn abgerundet wie das Originalfoto):
  T-R1a/d pruefen gegen kopf_re2_bau.PARAM_R3 (Kappe + Palettenregel r3), wenn das Protokoll keine Parameter bringt.
  T-V1  Kopf-Farbe am Render Pose 135 / 85 (RDT-Licht), Kopfpixel ohne Augenfaecher (T19/T20/T41) und ohne Maulflaechen: Anteil a* > 12
        <= 2 %; Anteil heller Kratzer = L* > lokaler Median (9 x 9, nur Pixel derselben Zone grau/dunkel) + 8 <= 2 %; b* P10 >= -1.
        Der Anteil gegen den Kopf-Median bzw. Zonen-Median steht als Ausgabe dabei: gegen den Kopf-Median ist jede Lesart mit dunkler Haarseite
        (K5) rot (graue Seite ganz ueber Median + 8), gegen den Zonen-Median zaehlt in Pose 85 die Lichtseite (n.l der oberen Vorderflaechen;
        dieselben Pixel ohne Licht liegen bei Texel-L* <= M + 6) - beides misst Licht/Zonen, nicht Kratzer.
  T-V2  Form: a) Vorsprung der vordersten Kopfecke (max x im G2-Raum, Ecken von Mesh 9 zurueckgerechnet) gegen die Kappe (Ellipsoid-Anpassung
        an die Kopfecken, iterativ ohne r > 1,15) <= 3 % der Kopfhoehe; b) keine Delle/kein neuer Vorsprung: bewegte Ecken und ihre Nachbarn liegen
        radial nicht weiter unter/ueber dem Mittel ihrer Nachbarecken als das Relief der G2-Quelle (P75 von |Abweichung| ueber alle 48 Ecken,
        mindestens 3 % der Kopfhoehe), es sei denn, das war an der Quelle schon so (Zuwachs <= 1 %); c) Silhouette seitlich gegen
        Version_80%/Beschreibung2_Vorlage.png: Zeilen 186-217 (Hintergrund = weisser Pfosten x 466-482), Vorderkante je Zeile im Foto = rechtestes
        x in 450-482 mit |L*Foto - L*BSS (Videoraum)| > 9, im Render = rechtestes Figurpixel; IoU der Streifen 440..Vorderkante (Zahl) und
        groesster Ueberstand; Soll IoU >= IoU der Vorgaengervariante (--vorher, Standard EM30_V4re2 = V5 Runde 2).
  Gegenprobe V5 Runde 2 (EM30_V4re2.EMD mit --prot kopf_re2_protokoll.json): T-V1 und T-V2 ROT.
"""
import os, sys, json, argparse, hashlib, struct
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
            S2=((0.03, 0.07), (0.76, 0.82)), KS2_DL=5.0, ZAEHNE=20, HELL=5,
            K1K_DL=6.0,          # Augenfaecher (Videoraum) mindestens 6 L* dunkler als die Kopf-Umgebung (25 x 25 um den Faecher)
            K1K_GLANZ=8.0,       # Glanzkern (G2-Augenkern) mindestens 8 L* heller als der Faecher-Median (RDT-Licht)
            K5_REL=0.90,         # rechte Haelfte <= 0,90 x linke Haelfte (Median L*; die Haelften mischen Zonen, Maul und Augenhoehle - Zonenwerte in der INFO-Zeile)
            K5_BAND=(0.62, 0.90), K5_REF2_DL=8.0, K9E_P50=23.0, K9E_VERH=0.9)
FAN = [19, 20, 41]


def _rows(p):
    mo2, mo3 = RL.modell(KB.G2), RL.modell(KB.G3)
    return KB.zeilen_neu(p, mo2, mo3)


def tk1b(A, R, p):
    """T-K1b wie riegel_ref2tex, aber weitere Zeilen duerfen die RE2-Kopfzeilen sein (486 = G2 Zeile 0 grau, 487 = G3 Zeile 0, 488 = G2 Zeile 0
    dunkel; T-R1d prueft sie bytegleich der Nachrechnung) - die Zeilen 480-485 muessen bytegleich Referenz 2 sein."""
    gleich = A['clut'].shape[0] >= 6 and bool((A['clut'][:6] == R['clut'][:6]).all())
    zg, z3, zd = _rows(p)
    if p['pal'].get('regel') == 'r3':
        erlaubt = {'G2 0x31 Zeile 0 grau (Regel r3)': zg, 'G3 0x33 Zeile 0': z3, 'G2 0x31 Zeile 0 dunkel (Regel r3)': zd}
    else:
        erlaubt = {'G2 0x31 Zeile 0 grau (L* x %.2f)' % p['pal']['grau']['k']: zg, 'G3 0x33 Zeile 0': z3,
                   'G2 0x31 Zeile 0 dunkel (L* x %.2f, <= %.0f)' % (p['pal']['dunkel']['k'], p['pal']['dunkel']['lmax']): zd}
    weitere = []
    ok = gleich
    for zi in range(6, A['clut'].shape[0]):
        kopie = [480 + j for j in range(R['clut'].shape[0]) if (A['clut'][zi] == R['clut'][j]).all()]
        re2 = [nm for nm, w in erlaubt.items() if (A['clut'][zi] == w).all()]
        weitere.append('%d=%s' % (480 + zi, ('Kopie %s' % kopie) if kopie else (re2[0] if re2 else 'NEU')))
        ok &= bool(kopie or re2)
    return zeile('T-K1b CLUT = Ref2', 'Zeilen 480-485 bytegleich; weitere Zeilen nur als Kopie einer Ref2-Zeile oder als RE2-Kopfzeile (G2 grau / G3 / G2 dunkel, T-R1d)',
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
    p = (prot or {}).get('param') or KB.PARAM_R3
    zonen = (prot or {}).get('zonen')
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
        # Runde 3: Soll-Ecken = gerundete Ecken (Kappe, kopf_re2_bau.runden), Soll-Normalen = Normalen der bewegten Ecken neu gerechnet;
        # Zonen und UVs an den QUELL-Ecken V (unveraendert)
        V, V2, N, finfo = KB.g2_geometrie(p)
        Q = (V2 * sk) @ R.T + t
        dv = np.abs(np.array([v[:3] for v in m9.tv], float) - Q).max()
        Nn = (N / sk) @ R.T
        Ln = np.linalg.norm(N, axis=1, keepdims=True); L2 = np.linalg.norm(Nn, axis=1, keepdims=True); L2[L2 == 0] = 1
        dn = np.abs(np.array([n[:3] for n in m9.tn], float) - Nn / L2 * Ln).max()
        fl = list(m9.tf) == list(g.tf) and list(m9.qf) == list(g.qf)
        du, dv_ = KB.G2_ORT['u0'] - KB.G2_ORT['u'][0], KB.G2_ORT['v0'] - KB.G2_ORT['v'][0]
        uv_ab = 0; maul = 0; zone_ab = 0; rows = dict(grau=0, dunkel=0)
        zn = p.get('zonen', KB.PARAM['zonen'])
        for kind, A_, B_, faces, n in (('T', m9.tuv, g.tuv, g.tf, 3), ('Q', m9.quv, g.quv, g.qf, 4)):
            for i, (a, b) in enumerate(zip(A_, B_)):
                if (a[2] >> 6) == KB.ZEILE_G3:
                    maul += 1; continue
                row = a[2] >> 6
                if not all(a[3 * k] == b[3 * k] + du and a[3 * k + 1] == b[3 * k + 1] + dv_ for k in range(n)) or row not in (KB.ZEILE_G2, KB.ZEILE_DUNKEL):
                    uv_ab += 1; continue
                soll = KB.zone_flaeche(V, [faces[i][2 * k + 1] for k in range(n)], list(b), n, zn, kind, i)
                ist = 'dunkel' if row == KB.ZEILE_DUNKEL else 'grau'
                rows[ist] += 1
                if soll != ist:
                    zone_ab += 1
        form_txt = ('; Kappe: %d Ecken gerundet %s, %d Normalen neu' % (len(finfo['bewegt']), finfo['bewegt'], len(finfo['normalen_neu']))) if finfo else '; ohne Kappe (Runde 2)'
        Z.append(zeile('T-R1a Kopf-Geometrie = G2 Mesh 3 (0x31)', 'Ecken = Rz Ry Rx S runden(v) + t (Abstand <= 1 E.), Normalen gedreht (<= 2 E.), Flaechen-Indizes gleich, '
                       'UV = Quelle + (%d, %d) ausser Maulflaechen (%d Dreiecke, G3-Kachel); CLUT-Zeile je Flaeche = Zonenregel (n . c > %.1f mit n = %s | u %d-%d | Spitze Q%s -> 488)' % (
                           du, dv_, len(KB.MAUL_T) + len(KB.MAUL_Q), zn['ebene_d'], tuple(zn['ebene_n']), zn['nase_u'][0], zn['nase_u'][1], list(zn.get('spitze_Q', ()))),
                       '%d Ecken / %d Normalen / %d Tri / %d Quad; Ecken max %.2f E., Normalen max %.2f, Flaechen %s, UV abweichend %d, Maul %d; Zeilen grau %d / dunkel %d, Zone abweichend %d%s' % (
                           len(m9.tv), len(m9.tn), len(m9.tf), len(m9.qf), dv, dn, 'gleich' if fl else 'ABWEICHEND', uv_ab, maul, rows['grau'], rows['dunkel'], zone_ab, form_txt),
                       dv <= 1.0 and dn <= 2.0 and fl and uv_ab == 0 and zone_ab == 0 and maul == len(KB.MAUL_T) + len(KB.MAUL_Q)))
    else:
        Z.append(zeile('T-R1a Kopf-Geometrie = G2 Mesh 3 (0x31)', 'Ecken / Normalen / Flaechen wie G2 Mesh 3 (48 / 48 / 56 Tri / 14 Quad)',
                       '%d Ecken / %d Normalen / %d Tri / %d Quad' % (len(m9.tv), len(m9.tn), len(m9.tf), len(m9.qf)), False))
    # Texel
    o = KB.G2_ORT; x0 = 128 * (o['page'] & 0xF) + o['u0']
    neu = tex[o['v0']:o['v0'] + 90, x0:x0 + 126] if tex.shape[0] >= o['v0'] + 90 else np.zeros((90, 126), np.uint8)
    quelle = mo2.tex[o['v'][0]:o['v'][1] + 1, o['u'][0]:o['u'][1] + 1]
    abw = int((neu != quelle).sum())
    Z.append(zeile('T-R1b Kopf-Texel = G2-TIM (u 0-125 v 0-89)', 'bytegleich (Runde 2: kein Andunkeln, Helligkeit nur ueber die Palette)',
                   '%d von %d Texeln abweichend (%d belegt)' % (abw, quelle.size, int((quelle != 0).sum())), abw == 0))
    o3 = KB.G3_ORT; x3 = 128 * (o3['page'] & 0xF) + o3['u0']
    hk, wk = o3['v'][1] - o3['v'][0] + 1, o3['u'][1] - o3['u'][0] + 1
    k_neu = tex[o3['v0']:o3['v0'] + hk, x3:x3 + wk] if tex.shape[0] >= o3['v0'] + hk else np.zeros((hk, wk), np.uint8)
    k_q = mo3.tex[o3['v'][0]:o3['v'][1] + 1, o3['u'][0]:o3['u'][1] + 1]
    Z.append(zeile('T-R1c Zahn-Kachel = G3-TIM (u %d-%d v %d-%d)' % (o3['u'] + o3['v']), 'bytegleich', '%d von %d Texeln abweichend' % (int((k_neu != k_q).sum()), k_q.size),
                   bool((k_neu == k_q).all())))
    if p['pal'].get('regel') == 'r3':
        Z.append(tr1d_r3(p, c, ch, mo2, mo3))
        return Z
    zg, z3, zd = KB.zeilen_neu(p, mo2, mo3)
    q2 = KB.clut_roh(mo2)[0]
    okg = ch > KB.ZEILE_G2 - 480 and bool((c[KB.ZEILE_G2 - 480] == zg).all())
    ok3 = ch > KB.ZEILE_G3 - 480 and bool((c[KB.ZEILE_G3 - 480] == z3).all())
    okd = ch > KB.ZEILE_DUNKEL - 480 and bool((c[KB.ZEILE_DUNKEL - 480] == zd).all())
    # Nachweis der Transformation: Lab der belegten Eintraege (Quelle -> grau / dunkel), Farbton erhalten (Winkel)
    idx = np.unique(quelle[quelle != 0])
    lq, lg, ld = (KB.lab_pal(KB.clut15_rgb(z))[idx] for z in (q2, zg, zd))
    pg, pd = p['pal']['grau'], p['pal']['dunkel']
    # Farbton: Zielwerte der Transformation VOR der 5-Bit-Rundung (L* x k, a*/b* x g, Gamut nur ueber die Chroma) gegen die Quelle; die
    # Rundung auf 5 Bit je Kanal (PSX-CLUT) wird getrennt als dE76 ausgewiesen (Schritt 8/255 -> hoechstens ~6)
    ziel = np.stack([np.clip(np.minimum(lq[:, 0] * pg['k'], pg['lmax']), 0, 100), lq[:, 1] * pg['g'], lq[:, 2] * pg['g']], 1)
    ziel = KB.lab_gamut(ziel)
    hq, hz = np.degrees(np.arctan2(lq[:, 2], lq[:, 1])), np.degrees(np.arctan2(ziel[:, 2], ziel[:, 1]))
    cq, cz, cg = np.hypot(lq[:, 1], lq[:, 2]), np.hypot(ziel[:, 1], ziel[:, 2]), np.hypot(lg[:, 1], lg[:, 2])
    dh = np.abs((hz - hq + 180) % 360 - 180)[(cq > 10) & (cz > 1)]
    dq = np.linalg.norm(lg - ziel, axis=1)
    gamut = float((cz < cq - 0.5).mean())
    Z.append(zeile('T-R1d CLUT-Zeilen 486 / 487 / 488 = RE2 (Palettentransformation)',
                   '486 = G2 Zeile 0 mit L* x %.2f (Chroma x %.2f, L* <= %.0f); 487 = G3 Zeile 0 bytegleich; 488 = G2 Zeile 0 mit L* x %.2f, Chroma x %.2f, L* <= %.0f; '
                   '15-Bit-Worte bytegleich der Nachrechnung (kopf_re2_bau.zeile_transform); Farbton der grauen Zeile erhalten (Farbwinkel der Zielwerte vor der 5-Bit-Rundung '
                   'p90 <= 1 Grad bei Quell-Chroma > 10; sRGB-Gamut nur ueber die Chroma); 5-Bit-Rundung dE76 max <= 7' % (
                       pg['k'], pg['g'], pg['lmax'], pd['k'], pd['g'], pd['lmax']),
                   'TIM %d Zeilen; 486 %s; 487 %s; 488 %s; Eintraege (belegt %d) Lab p50 Quelle %s -> grau %s -> dunkel %s; Farbwinkel p90 %.2f Grad; Rundung dE76 p90 %.1f max %.1f; '
                   'Gamut-reduzierte Eintraege %.2f (Chroma p50 %.1f -> %.1f)' % (
                       ch, 'gleich' if okg else 'ABWEICHEND', 'gleich' if ok3 else 'ABWEICHEND', 'gleich' if okd else 'ABWEICHEND', len(idx),
                       '/'.join('%.1f' % v for v in np.median(lq, 0)), '/'.join('%.1f' % v for v in np.median(lg, 0)), '/'.join('%.1f' % v for v in np.median(ld, 0)),
                       float(np.percentile(dh, 90)) if len(dh) else 0.0, float(np.percentile(dq, 90)), float(dq.max()), gamut, float(np.median(cq)), float(np.median(cg))),
                   okg and ok3 and okd and (not len(dh) or float(np.percentile(dh, 90)) <= 1.0) and float(dq.max()) <= 7.0))
    return Z


def tr1d_r3(p, c, ch, mo2, mo3):
    """T-R1d fuer die Palettenregel r3: Zeilen bytegleich der Nachrechnung und die Regel an den belegten Eintraegen erfuellt."""
    zg, z3, zd = KB.zeilen_neu(p, mo2, mo3)
    _zg, _zd, pr = KB.zeilen_r3(p, mo2)
    okg = ch > KB.ZEILE_G2 - 480 and bool((c[KB.ZEILE_G2 - 480] == zg).all())
    ok3 = ch > KB.ZEILE_G3 - 480 and bool((c[KB.ZEILE_G3 - 480] == z3).all())
    okd = ch > KB.ZEILE_DUNKEL - 480 and bool((c[KB.ZEILE_DUNKEL - 480] == zd).all())
    zm, kern, tex, labq = KB.zonen_texel(p, mo2)
    Cq = np.hypot(labq[:, 1], labq[:, 2])
    glanz = set(pr['glanz_eintraege'])
    teile = []; ok = okg and ok3 and okd
    for nm, z, zone in (('grau', zg, zm['grau']), ('dunkel', zd, zm['dunkel'])):
        pz = p['pal'][nm]; M = pr[nm]['M']
        lab = KB.lab_pal(KB.clut15_rgb(z))
        idx = np.unique(tex[zone])
        ng = np.array([i for i in idx if i not in glanz])
        hell = int((lab[ng, 0] > M + pz['d_hoch'] + 1.0).sum())                       # +1: 5-Bit-Rundung
        ader = ng[Cq[ng] > pz['c_ader']]
        ader_hell = int((lab[ader, 0] > M + 1.0).sum())
        da, db = float(np.abs(lab[ng, 1] - pz['a_t']).max()), float(np.abs(lab[ng, 2] - pz['b_t']).max())
        de = float(np.linalg.norm(lab[ng] - pr[nm]['ziel'][ng], axis=1).max())          # 5-Bit-Rundung gegen den Zielwert der Regel
        amax, bmin = float(lab[ng, 1].max()), float(lab[ng, 2].min())
        texel_l = lab[tex[zone], 0]; tg = np.array([t not in glanz for t in tex[zone]])
        teile.append('%s: M %.1f (k %.3f), belegt %d, heller als M+%.0f: %d, Adern (Quell-C > %.0f) %d davon heller als M: %d, |a*-%.1f| max %.1f, |b*-%.1f| max %.1f, '
                     'Rundung dE76 max %.1f, a* max %.1f, b* min %.1f, Texel L* P50/P99 %.1f/%.1f' % (
                         nm, M, pr[nm]['k'], len(idx), pz['d_hoch'], hell, pz['c_ader'], len(ader), ader_hell, pz['a_t'], da, pz['b_t'], db, de, amax, bmin,
                         float(np.median(texel_l[tg])), float(np.percentile(texel_l[tg], 99))))
        ok &= hell == 0 and ader_hell == 0 and de <= 7.0 and amax <= 8.0 and bmin >= -0.5
    lg = KB.lab_pal(KB.clut15_rgb(zg))[sorted(glanz)] if glanz else np.zeros((0, 3))
    teile.append('Glanz %s L* %s' % (sorted(glanz), np.round(lg[:, 0], 1).tolist()))
    return zeile('T-R1d CLUT-Zeilen 486 / 487 / 488 = RE2 (Palettenregel r3)',
                 '486 / 488 = G2 Zeile 0 nach Regel r3 (kopf_re2_bau.zeile_r3; 15-Bit-Worte bytegleich der Nachrechnung), 487 = G3 Zeile 0 bytegleich; an den '
                 'belegten Eintraegen der Zone (ohne Glanzpunkt des Auges): kein Eintrag heller als M + d_hoch (+1 Rundung), kein Ader-Eintrag (Quell-Chroma > c_ader) '
                 'heller als M (+1), Farbton a*/b* je Zeile = a_t/b_t (5-Bit-Rundung dE76 <= 7 gegen den Zielwert), a* <= 8 (kein Rosa/Rot), b* >= -0,5 (kein Lila/Blau)',
                 'TIM %d Zeilen; 486 %s; 487 %s; 488 %s; %s' % (ch, 'gleich' if okg else 'ABWEICHEND', 'gleich' if ok3 else 'ABWEICHEND', 'gleich' if okd else 'ABWEICHEND',
                                                                 '; '.join(teile)), ok)


# ------------------------------------------------------------------------------------------------------------ Runde 3: T-V1 / T-V2
TV1 = dict(A_MAX=12.0, A_ANTEIL=0.02, DL=8.0, L_ANTEIL=0.02, B_P10=-1.0, FENSTER=9)
TV2 = dict(VORSPRUNG=0.03, DELLE=0.03, ZUWACHS=0.01, RELIEF_P=75, ZEILEN=(186, 217), X0=440, FOTO_X=(450, 482), FOTO_DL=9.0)


def tv1(mo, P, prot):
    """T-V1 Kopf-Farbe am Render (RDT-Licht), Kopfpixel ohne Augenfaecher und Maulflaechen."""
    zon = (prot or {}).get('zonen')
    Z = []
    for h in ('135', '85'):
        img, ids = G.zeichne(mo, P[h]); Lab = G.lab(img.astype(float)); k = ids == KB.KOPF
        _i, idf = G.zeichne(KM.teil(mo, FAN), P[h], licht=False); fan = (idf == KB.KOPF) & k
        _i, idm = G.zeichne(KM.teil(mo, KB.MAUL_T, KB.MAUL_Q), P[h], licht=False); maul = (idm == KB.KOPF) & k
        m = k & ~fan & ~maul
        L, a, b = Lab[..., 0][m], Lab[..., 1][m], Lab[..., 2][m]
        anteil_a = float((a > TV1['A_MAX']).mean()) if m.any() else 1.0
        med = float(np.median(L)) if m.any() else 0.0
        anteil_l_kopf = float((L > med + TV1['DL']).mean()) if m.any() else 1.0
        hell = np.zeros(m.shape, bool); hell_lok = np.zeros(m.shape, bool); zt = []
        zmasken = []
        if zon:
            for nm, tt, qq in (('grau', zon['grau_T'], zon['grau_Q']), ('dunkel', zon['dunkel_T'], zon['dunkel_Q'])):
                _i, idz = G.zeichne(KM.teil(mo, tt, qq), P[h], licht=False); zmasken.append((nm, (idz == KB.KOPF) & m))
        else:
            zmasken.append(('Kopf', m))
        for nm, mz in zmasken:
            if not mz.any():
                continue
            zmed = float(np.median(Lab[..., 0][mz]))
            hell |= mz & (Lab[..., 0] > zmed + TV1['DL'])
            ys, xs = np.nonzero(mz); r_ = TV1['FENSTER'] // 2
            for y_, x_ in zip(ys, xs):            # lokaler Median: Pixel derselben Zone im Fenster FENSTER x FENSTER
                w_ = mz[max(y_ - r_, 0):y_ + r_ + 1, max(x_ - r_, 0):x_ + r_ + 1]
                lw = Lab[max(y_ - r_, 0):y_ + r_ + 1, max(x_ - r_, 0):x_ + r_ + 1, 0][w_]
                if Lab[y_, x_, 0] > np.median(lw) + TV1['DL']:
                    hell_lok[y_, x_] = True
            zt.append('%s %d px Median %.1f' % (nm, int(mz.sum()), zmed))
        anteil_l = float(hell_lok[m].mean()) if m.any() else 1.0
        anteil_zone = float(hell[m].mean()) if m.any() else 1.0
        b10 = float(np.percentile(b, 10)) if m.any() else -99.0
        ok = anteil_a <= TV1['A_ANTEIL'] and anteil_l <= TV1['L_ANTEIL'] and b10 >= TV1['B_P10']
        Z.append(zeile('T-V1 Kopf-Farbe Pose %s (RDT-Licht)' % h,
                       'Kopfpixel ohne Augenfaecher/Maul: Anteil a* > %.0f <= %.0f %%; Anteil heller Kratzer L* > lokaler Median (%dx%d, gleiche Zone grau/dunkel) + %.0f '
                       '<= %.0f %%; b* P10 >= %.0f (Zonen- und Kopf-Median-Anteil nur Ausgabe: die Haarseite ist nach K5 dunkler, die Lichtseite nach n.l heller)' % (
                           TV1['A_MAX'], 100 * TV1['A_ANTEIL'], TV1['FENSTER'], TV1['FENSTER'], TV1['DL'], 100 * TV1['L_ANTEIL'], TV1['B_P10']),
                       '%d px; a* > 12: %.1f %%; L* > lokaler Median + 8: %.1f %%; > Zonen-Median + 8: %.1f %% (%s); > Kopf-Median %.1f + 8: %.1f %%; L* P50/P99 %.1f/%.1f; '
                       'a* P50/P99 %.1f/%.1f; b* P10/P50 %.1f/%.1f' % (int(m.sum()), 100 * anteil_a, 100 * anteil_l, 100 * anteil_zone, ', '.join(zt) or 'ohne Zonen', med, 100 * anteil_l_kopf,
                                                med, float(np.percentile(L, 99)) if m.any() else 0, float(np.median(a)) if m.any() else 0,
                                                float(np.percentile(a, 99)) if m.any() else 0, b10, float(np.median(b)) if m.any() else 0), ok))
    return Z


def g2_ecken(emd, p):
    """Ecken von Mesh 9 zurueck in den unskalierten G2-Raum (Umkehrung von Q = (V S) R^T + t)."""
    blob = open(emd, 'rb').read(); d = EA.emd_dir(blob)
    m9 = EA.md1_parse(blob[d[7]:d[8]])[KB.KOPF]
    Q = np.array([v[:3] for v in m9.tv], float)
    sk = np.array(p['skal'], float); R = KB.rot(p); t = np.array(p['versatz'], float)
    return ((Q - t) @ R) / sk


def _nachbarn(m):
    nb = {}
    for faces, n in ((m.tf, 3), (m.qf, 4)):
        for f in faces:
            vi = [f[2 * k + 1] for k in range(n)]
            for a_ in vi:
                nb.setdefault(a_, set()).update(set(vi) - {a_})
    return nb


def foto_vorderkante():
    """Vorderkante des Kopfes im Foto Beschreibung2_Vorlage je Zeile (TV2['ZEILEN']): rechtestes x in FOTO_X mit |L* Foto - L* BSS| > FOTO_DL."""
    v = G.vorlage(2); bg = G.videoraum(G.hintergrund())
    dl = np.abs(G.lab(v)[..., 0] - G.lab(bg)[..., 0])
    out = []
    for y in range(TV2['ZEILEN'][0], TV2['ZEILEN'][1] + 1):
        xs = [x for x in range(TV2['FOTO_X'][0], TV2['FOTO_X'][1] + 1) if dl[y, x] > TV2['FOTO_DL']]
        out.append(max(xs) if xs else TV2['X0'])
    return np.array(out)


def silhouette(mo, P, ff=None):
    ff = foto_vorderkante() if ff is None else ff
    _img, ids = G.zeichne(mo, P['85'])
    fr = []
    for y in range(TV2['ZEILEN'][0], TV2['ZEILEN'][1] + 1):
        xs = np.nonzero(ids[y, TV2['X0']:TV2['FOTO_X'][1] + 11] >= 0)[0]
        fr.append(TV2['X0'] + int(xs.max()) if len(xs) else TV2['X0'])
    fr = np.array(fr)
    inter = (np.minimum(fr, ff) - TV2['X0']).sum(); uni = (np.maximum(fr, ff) - TV2['X0']).sum()
    k9 = np.nonzero((ids == KB.KOPF).any(0))[0]
    return dict(iou=float(inter / max(uni, 1)), ueber_max=int((fr - ff).max()), ueber_mittel=float((fr - ff).mean()), foto=ff.tolist(), render=fr.tolist(),
                kopf_vorderkante=int(k9.max()) if len(k9) else -1)


def tv2(emd, mo, P, prot, vorher=None, prot_vorher=None):
    """T-V2 Form: Vorsprung gegen die Kappe, Delle/Vorsprung der bewegten Ecken, Silhouette seitlich gegen das Foto."""
    p = (prot or {}).get('param') or KB.PARAM_R3
    g = RL.modell(KB.G2).meshes[3]
    V0 = np.array([v[:3] for v in g.tv], float)
    V = g2_ecken(emd, p)
    Hk = float(V[:, 1].max() - V[:, 1].min())
    c, ax, r, raus, it = KB.kappe(V, (p.get('form') or KB.PARAM_R3['form'])['trimm'])
    i0 = int(np.argmax(V[:, 0]))
    vor = float(np.linalg.norm(V[i0] - c) * (1 - 1 / r[i0])) / Hk
    Z = [zeile('T-V2a Vorsprung vorderste Kopfecke', 'Ecke mit max x (G2-Raum, Mesh 9 zurueckgerechnet) gegen die Kappe (Ellipsoid-Anpassung an die Kopfecken, iterativ ohne r > 1,15): '
               'radialer Ueberstand <= %.0f %% der Kopfhoehe' % (100 * TV2['VORSPRUNG']),
               'Ecke %d (x %.0f) r %.3f -> %.1f E. = %.1f %% von %.0f E.; Kappe c %s ax %s, %d Schritte, ausgeschlossen %s' % (
                   i0, V[i0, 0], r[i0], vor * Hk, 100 * vor, Hk, np.round(c, 1).tolist(), np.round(ax, 1).tolist(), it, raus),
               vor <= TV2['VORSPRUNG'])]
    # b) Delle / neuer Vorsprung an den bewegten Ecken
    bew = [int(i) for i in np.nonzero(np.linalg.norm(V - V0, axis=1) > 1.0)[0]]
    nb = _nachbarn(g)
    r0 = np.linalg.norm((V0 - c) / ax, axis=1)

    def abw(W, rW, i):
        ring = sorted(nb.get(i, ()))
        rr = np.linalg.norm((W[ring] - c) / ax, axis=1)
        return float((rW[i] - rr.mean()) * np.linalg.norm(W[i] - c) / max(rW[i], 1e-9) / Hk)
    # gepruefte Ecken: bewegte Ecken und ihre Nachbarn (deren Umgebung sich geaendert hat)
    pruef = sorted(set(bew) | set().union(*[nb.get(i, set()) for i in bew])) if bew else []
    dev = {i: abw(V, r, i) for i in pruef}
    dev0 = {i: abw(V0, r0, i) for i in pruef}
    # Relief der unveraenderten G2-Quelle (alle 48 Ecken, gleiche Kappe): P75 von |Abweichung| = Grenze fuer "neu" (eine Unebenheit, die
    # kleiner ist als die von drei Vierteln der Quell-Ecken, ist keine neue Form); mindestens TV2['DELLE']
    rel = float(np.percentile(np.abs([abw(V0, r0, i) for i in range(len(V0))]), TV2['RELIEF_P']))
    grenze = max(TV2['DELLE'], rel)
    delle = {i: d_ for i, d_ in dev.items() if d_ < -grenze and d_ < dev0[i] - TV2['ZUWACHS']}
    neu_vor = {i: d_ for i, d_ in dev.items() if d_ > grenze and d_ > dev0[i] + TV2['ZUWACHS']}
    dmin = min(dev.values()) if dev else 0.0; dmax = max(dev.values()) if dev else 0.0
    Z.append(zeile('T-V2b keine Delle / kein neuer Vorsprung', 'bewegte Ecken (> 1 E. gegen G2) und ihre Nachbarn: radialer Abstand zum Mittel der Nachbarecken '
                   '(Kappenmass, Anteil der Kopfhoehe); Grenze = max(%.0f %%, Relief der G2-Quelle = P%d von |Abweichung| ueber alle 48 Ecken); Delle = unter -Grenze und um > %.0f %% '
                   'tiefer als an der Quelle; neuer Vorsprung = ueber +Grenze und um > %.0f %% hoeher als an der Quelle; beides 0' % (
                       100 * TV2['DELLE'], TV2['RELIEF_P'], 100 * TV2['ZUWACHS'], 100 * TV2['ZUWACHS']),
                   'Grenze %.1f %% (Relief P%d %.1f %%); %d Ecken bewegt %s, %d geprueft; min %.1f %% (Ecke %s), max %.1f %% (Ecke %s, Quelle %.1f %%); Dellen %s; neue Vorspruenge %s' % (
                       100 * grenze, TV2['RELIEF_P'], 100 * rel,
                       len(bew), bew, len(pruef), 100 * dmin, min(dev, key=dev.get) if dev else '-', 100 * dmax, max(dev, key=dev.get) if dev else '-',
                       100 * dev0[max(dev, key=dev.get)] if dev else 0.0, {k: round(100 * v, 1) for k, v in delle.items()} or 'keine',
                       {k: round(100 * v, 1) for k, v in neu_vor.items()} or 'keine'),
                   not delle and not neu_vor))
    # c) Silhouette seitlich
    ff = foto_vorderkante()
    s_ = silhouette(mo, P, ff)
    sv = None
    if vorher and os.path.exists(vorher) and os.path.abspath(vorher) != os.path.abspath(emd):
        sv = silhouette(b2.modell(vorher), P, ff)
    ok = sv is None or s_['iou'] >= sv['iou']
    Z.append(zeile('T-V2c Silhouette seitlich gegen Beschreibung2_Vorlage', 'Zeilen %d-%d (Hintergrund weisser Pfosten): IoU der Streifen %d..Vorderkante Foto/Render (Pose 85) '
                   '>= IoU der Vorgaengervariante; Foto-Vorderkante = rechtestes x in %d-%d mit |dL*| > %.0f gegen das BSS im Videoraum' % (
                       TV2['ZEILEN'][0], TV2['ZEILEN'][1], TV2['X0'], TV2['FOTO_X'][0], TV2['FOTO_X'][1], TV2['FOTO_DL']),
                   'IoU %.3f%s; Ueberstand Render ueber Foto max %d px / Mittel %.1f px; Foto-Vorderkante x %d-%d (P50 %d); Kopf-Vorderkante Render (Id 9) x %d' % (
                       s_['iou'], (' (vorher %s: %.3f, Ueberstand max %d, Kopf-Vorderkante %d)' % (os.path.basename(vorher), sv['iou'], sv['ueber_max'], sv['kopf_vorderkante'])) if sv else '',
                       s_['ueber_max'], s_['ueber_mittel'], min(ff), max(ff), int(np.median(ff)), s_['kopf_vorderkante']), ok))
    return Z, dict(silhouette=s_, vorher=sv, abweichung=dev, vorsprung=vor)


_ref2 = {}


def ref2_mass(P):
    """Ref2-Kopf im selben Render: Kopf-Median (roh mit Licht und Videoraum) Pose 135, Kopf/Rumpf."""
    if not _ref2:
        mo = b2.modell(G.REF2)
        img, ids = G.zeichne(mo, P['135'])
        L = G.lab(img.astype(float))[..., 0]; Lv = G.lab(G.videoraum(img))[..., 0]
        k, r8 = ids == 9, ids == 8
        _ref2.update(p50=float(np.median(L[k])), p50_v=float(np.median(Lv[k])), verh=float(L[k].mean() / L[r8].mean()), rumpf=float(np.median(L[r8])))
    return _ref2


def kopf_r2(mo, M, P, zonen=None):
    """Runde-2-Masse am Render Pose 135: Auge als dunkle Pupille (Faecher), Haelften, KV1/KV2, K6, T-K9e, Messblatt-Methode (INFO)."""
    a = M['135']; img, ids = a['img'], a['ids']
    ir, _ = G.zeichne(mo, P['135'], licht=False)
    L = G.lab(img.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
    V = G.videoraum(img); LabV = G.lab(V); Lv = LabV[..., 0]
    k, r8 = ids == 9, ids == 8; kb = a['kopfbox']; x0, y0, x1, y1 = kb; xm = 0.5 * (x0 + x1); H = a['H']
    X = np.arange(640)[None, :].repeat(480, 0)
    li, re_ = k & (X < xm), k & (X >= xm)
    R2 = ref2_mass(P)
    Z = []
    # Auge (KI-Lesung D3): Faecher T19/T20/T41
    _i, idf = G.zeichne(KM.teil(mo, FAN), P['135'], licht=False)
    fan = (idf == 9) & k
    if fan.any():
        ys, xs = np.nonzero(fan); cx, cy = int(xs.mean()), int(ys.mean())
        umg = k & ~fan
        umg[:max(cy - 12, 0)] = False; umg[cy + 13:] = False; umg[:, :max(cx - 12, 0)] = False; umg[:, cx + 13:] = False
        core = a['ma'] if a.get('ma') is not None and a['ma'].any() else fan
        fv, uv = float(np.median(Lv[fan])), float(np.median(Lv[umg])) if umg.any() else float('nan')
        fl, gl = float(np.median(L[fan])), float(L[core].max())
        fak = float(np.median(L[fan] / np.maximum(Lr[fan], 1)))
        Z.append(zeile('T-R2 K1k Auge sichtbar (KI D3: dunkle Pupille mit Glanz)',
                       'Augenfaecher T19/T20/T41 (Videoraum) L* <= Umgebung (Kopf 25x25 um den Faecher) - %.0f; Glanzkern (G2-Augenkern) L* max >= Faecher-Median (RDT-Licht) + %.0f' % (
                           SOLL['K1K_DL'], SOLL['K1K_GLANZ']),
                       'Faecher %d px (%.3f x %.3f H) L* %.1f gegen Umgebung %.1f (Videoraum); RDT-Licht Faecher %.1f, Glanzkern max %.1f; Lichtfaktor Faecher %.2f' % (
                           fan.sum(), (xs.ptp() + 1) / H, (ys.ptp() + 1) / H, fv, uv, fl, gl, fak),
                       fv <= uv - SOLL['K1K_DL'] and gl >= fl + SOLL['K1K_GLANZ']))
        bx0, bx1 = int(xs.min()), int(xs.max()); by0, by1 = int(ys.min()), int(ys.max())
        sp = (int(round(2 * xm - bx1)), by0, int(round(2 * xm - bx0)), by1)
        l6 = float(RV.lab1(G.mittel(V, sp))[0]); pv = float(np.median(Lv[k]))
        Z.append(zeile('T-R2 K6 zweites Auge verdeckt', 'Spiegelbox des Augenfaechers an der Kopfmitte (Videoraum): L* <= Kopf-Median (%.1f); kein Glanzkern (Messblatt: Original 12,9)' % pv,
                       'L* %.1f (Box %s)' % (l6, sp), l6 <= pv))
    else:
        Z.append(zeile('T-R2 K1k Auge sichtbar (KI D3)', 'Augenfaecher sichtbar', 'Faecher nicht sichtbar', False))
    # Haelften und KV2
    hl, hr = float(np.median(L[li])), float(np.median(L[re_])); vl, vr = float(np.median(Lv[li])), float(np.median(Lv[re_]))
    pv = float(np.median(Lv[k]))
    kv2, _b = RV.fenster_lab(V, kb, RV.FENSTER['KV2']); kv1, _b1 = RV.fenster_lab(V, kb, RV.FENSTER['KV1']); kv3, _b3 = RV.fenster_lab(V, kb, RV.FENSTER['KV3'])
    lo, hi = SOLL['K5_BAND'][0] * pv, SOLL['K5_BAND'][1] * pv
    # Zonen am Bild: graue / dunkle / Maul-Flaechen (Protokoll) -> Median L* mit Licht
    zinfo = ''
    if zonen:
        zw = {}
        for nm, tt, qq in (('grau', zonen['grau_T'], zonen['grau_Q']), ('dunkel', zonen['dunkel_T'], zonen['dunkel_Q']), ('maul', zonen['maul_T'], zonen['maul_Q'])):
            _i, idz = G.zeichne(KM.teil(mo, tt, qq), P['135'], licht=False); mz_ = (idz == 9) & k
            zw[nm] = (int(mz_.sum()), float(np.median(L[mz_])) if mz_.any() else float('nan'), float(np.median(Lv[mz_])) if mz_.any() else float('nan'))
        zinfo = '; Zonen (px, L* Licht, Videoraum): ' + ', '.join('%s %d / %.1f / %.1f' % (nm, *w) for nm, w in zw.items())
    Z.append(zeile('T-R2 K5 dunkle Seite Bild-rechts (Haare)', 'Kopf-Haelften an der Kopfbox-Mitte: Median L* rechts <= %.2f x links (RDT-Licht und Videoraum); Fenster KV2 (Videoraum) '
                   'L* <= Ref2-Kopf P50 %.1f - %.0f (Messblatt R2), a* > 0, b* > 0 (braeunlich); Band %.2f-%.2f x Kopf-Median (%.1f-%.1f) nur Ausgabe (das Fenster KV2 '
                   'schneidet das Maul mit den Zaehnen)' % (SOLL['K5_REL'], R2['p50_v'], SOLL['K5_REF2_DL'], SOLL['K5_BAND'][0], SOLL['K5_BAND'][1], lo, hi),
                   'links/rechts RDT-Licht %.1f / %.1f (%.2f), Videoraum %.1f / %.1f (%.2f); KV2 %s; KV3 (Nase) %s%s' % (
                       hl, hr, hr / hl if hl else 0, vl, vr, vr / vl if vl else 0, '/'.join('%.1f' % v for v in kv2), '/'.join('%.1f' % v for v in kv3), zinfo),
                   hr <= SOLL['K5_REL'] * hl and vr <= SOLL['K5_REL'] * vl and kv2[0] <= R2['p50_v'] - SOLL['K5_REF2_DL'] and kv2[1] > 0 and kv2[2] > 0))
    p50 = float(np.median(L[k])); verh = float(L[k].mean() / L[r8].mean())
    Z.append(zeile('T-R2 T-K9e ganzer Kopf (Pose 135)', 'L* P50 >= %.0f; Kopf/Rumpf (Mittel L*, Id 8) >= %.1f (Ref2 %.1f / %.2f; Vorlagen Kopf/Brust Video 1,59, KI 1,6-2,2)' % (
        SOLL['K9E_P50'], SOLL['K9E_VERH'], R2['p50'], R2['verh']),
        'L* P50 %.1f (Videoraum %.1f); Kopf/Rumpf %.2f (Rumpf P50 %.1f)' % (p50, pv, verh, float(np.median(L[r8]))), p50 >= SOLL['K9E_P50'] and verh >= SOLL['K9E_VERH']))
    Z.append(zeile('INFO K9 Kuppe rot (KV1)', 'Messblatt a* >= 11 (Original 15,4); G2-Kuppe hat rote Adern auf grauem Grund - nur Ausgabe',
                   'KV1 %s' % '/'.join('%.1f' % v for v in kv1), True, info=True))
    # Messblatt-Methode (hellster Kopfpunkt) als INFO mit Lichtbeleg
    b = RV.kopf_blob(V, LabV, k, kb)
    dxk = (xm - b['sp'][0]) / H; dyk = (b['sp'][1] - y0) / H
    mz = a.get('mz'); in_zahn = bool(mz is not None and mz.any() and mz[int(round(b['sp'][1])), int(round(b['sp'][0]))]) if mz is not None else False
    Z.append(zeile('INFO Messblatt K1/K3/K4 (hellster Kopfpunkt)', 'Messblatt: der hellste Kopfpunkt ist der Augenfleck (Video). Im RDT-Licht 50A0/12 liegt der Augenfaecher (Normale -z, (W)-rechts) im '
                   'Umgebungslicht (Faktor 0,24 = 63/255 von ncct, re2kopf_r2/diag1.json), das Maul (Normale +x) im Hauptlicht (0,81) - ein weisser Texel wuerde hier hoechstens L* 26 erreichen, '
                   'die Zaehne liegen bei 55; Drehungen (yaw -65, roll -20) heben den Faecher nur auf 0,47-0,51 und brechen K4/K10/KS2 (grid_rot/grid_roll.log). Nur Ausgabe.',
                   'hellster Fleck Schwerpunkt (%d, %d) L* %.1f, Kontrast %.1f, %d x %d px = %.3f x %.3f H, K4 %.3f / %.3f H, auf den Zaehnen: %s' % (
                       int(round(b['sp'][0])), int(round(b['sp'][1])), b['ext'], b['kontrast'], b['wh'][0], b['wh'][1], b['wh'][0] / H, b['wh'][1] / H, dxk, dyk, in_zahn), True, info=True))
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


def pruefen(emd, basis=None, lauf=None, prot=None, vorher=None):
    P = G.posen()
    p = (prot or {}).get('param') or KB.PARAM_R3
    R = RT.atlas(G.REF2); A = RT.atlas(emd)
    mo = b2.modell(emd)
    Z = []
    for z in RR.tk1(A, R):
        if z['mass'].startswith('T-K1b'):
            z = tk1b(A, R, p)
        Z.append(z)
    Z += tk2_schwanz(A, R)
    z3, i3 = RR.tk3(mo, P, A); Z += z3
    if basis:
        Z += tr0(emd, basis)
    Z += tr1(emd, prot)
    M = KM.messen(mo, P, bilder=True)
    hell_unten(mo, M)
    Z += tr2_tr3(mo, M)
    Z += kopf_r2(mo, M, P, (prot or {}).get('zonen'))
    Z += tv1(mo, P, prot)
    z2, i2 = tv2(emd, mo, P, prot, vorher)
    Z += z2
    Z += tr4(emd)
    zf, st = RR.form(emd, mo, P, lauf); Z += zf
    info = dict(tk3={k: v for k, v in i3.items() if k != 'bild'}, mess={h: {k: v for k, v in r.items() if k not in ('img', 'ids', 'ma', 'mz')} for h, r in M.items()},
                schwanz={k: v for k, v in st.items() if k not in ('C', 'breite', 'maske')}, ref2=dict(_ref2), form=i2)
    return Z, info, M


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd'); ap.add_argument('--basis', default=KB.BASIS); ap.add_argument('--lauf')
    ap.add_argument('--prot', help='Protokoll-JSON des Baus (Parameter, Zonen)')
    ap.add_argument('--vorher', default=os.path.join(G.SFV4, 'emd', 'EM30_V4re2.EMD'), help='Vorgaengervariante fuer T-V2c (Standard V5 Runde 2)')
    ap.add_argument('--json'); ap.add_argument('--md'); ap.add_argument('--txt'); ap.add_argument('--kein-exit', action='store_true')
    a = ap.parse_args()
    prot = json.load(open(a.prot)) if a.prot and os.path.exists(a.prot) else None
    Z, info, _M = pruefen(a.emd, a.basis if a.basis and os.path.exists(a.basis) and os.path.abspath(a.basis) != os.path.abspath(a.emd) else None, a.lauf, prot, a.vorher)
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
