"""SF V5 wie Foto - Koerper (Bauer, 2026-11-08): Riegel. Exit 1 bei Rot.

    python riegel_v5f.py --emd EM30_V5f.EMD [--basis EM30_V5k.EMD] [--prot koerper_bau.json] [--aus DIR --name NAME] [--ohne-kopf]

Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc). Posen v6_gemein.posen: P1 = Foto 1, P2 = Foto 2; "85" = posen_v4 (Riegel V4).
Pruefungen (Auftrag Runde 41 SF "V5 wie Foto - Koerper", Nutzer 2026-11-08 Punkte 1/3/5/8/9):
  T-K1   Ausnahmeliste gegen die Basis (V5k): Sektionen 0-6 bytegleich; Texel bytegleich; Meshes 0-15 Ecken/Normalen/Flaechen/UV bytegleich
         bis auf (i) das CLUT-Feld der gelisteten Rot-Flaechen (Zeilen 488/489/490, nur Mesh 8 Brust, 12, 13) und (ii) die Krallenecken
         von Mesh 12 (Flaechen mit allen Ecken y >= 330); Schwanz 16-22: Flaechen/Normalen/UV bytegleich bis auf das CLUT-Feld (487), Ecken
         gelistet; CLUT: 483-486 = Basis, 488/489/490 = Referenz-2-Zeilen 481/480/482 bytegleich, 480-482 nur rote Eintraege (Ref2-a* >= 10)
         geaendert, jeder davon nicht heller und mit C* <= Grau-Spanne, 487 = Zeile 480 (Ref2) ausser den Schwanz-Indizes, die alle
         einen Eintrag der Sichel-Zeile 484 tragen; Zeilen <= 16, Port-Texturhoehe <= 4096.
  T-F1   Schwanzbreite je Station f 0,1-0,9 (Inkreis-Durchmesser, Pose P2, v5f_gemein) innerhalb +-20 % der Foto-Sollbreite.
  T-F5   Schwanz dunkel gruenlich-grau: Mittel Lab (Videoraum, P2, sichtbare Schwanzpixel) a* <= 0, b* >= 0, L* < Rumpfhaut (Foto 2
         grau 23,8 Messblatt 5 und Modell-Rumpf grau Knochen 7/8 a* < 6). Ohne Huckel: (a) Gelenkringe deckungsgleich im Knochenraum
         (Radius Endring k = Anfangsring k+1, Breitensprung <= 8 %), (b) Breitensprung im Bild P2 (Inkreis 2-8 px vor/nach jedem sichtbaren
         Gelenk, linear zum Gelenk fortgesetzt) <= 8 %, (c) Steigungsknick der Mantellinie je Gelenk <= 15 Grad (Knochenraum),
         (d) Stufe Wurzel/Ruecken: obere Kontur am Wechsel Schwanz -> Rumpf in P2 |y(x0) - y(x0 - 1)| <= 2 px (85 nur ausgegeben),
         keine Luecke (Spalte ohne Figur) zwischen Schwanz und Rumpf.
  T-F7   Rot-Karte (a* >= 10): Foto 1 je feste Region (Messblatt 4.1; Id-Zuordnung in P1 nach Messblatt 4.2 nur grob): Brust Mitte,
         Schulter Bild-links, Schulter Bild-rechts, Unterarm/Hand, Bauch je +-5 Prozentpunkte; beide Schulter-Regionen zusaetzlich
         <= 5 % ("rechte Schulter kein Rot", beide Lesarten). Foto 2 je Koerperteil (Id-Puffer, Messblatt 4.2 belastbar, >= 100 px) und
         je Region Buckel / Schulter-Oberarm Mitte je +-5 Prozentpunkte. Kopf (Kopf-Riegel, Kopf nicht Teil dieses Auftrags) und Schwanz
         (T-F5, Nutzer 3 "graeulich") ausgenommen, als INFO ausgegeben.
  T-F8   Krallen (W)-links (Mesh 12): jede Kralle projiziert in P1 0,036 H +-15 % (Messblatt 6), keine kuerzer als in der Basis.
  T-F4   Umriss-IoU gegen die Trimap (f_pose.iou: Figur sicher gegen Modell, Unbekannt zaehlt nicht) P1 >= 0,709, P2 >= 0,758
         (Stand Runde 4, Messblatt 2), gemessen am Render ids >= 0.
  Riegel V4 Schwanz (Pose 85): T1-T4, TA, TK, T5 (riegel_v4.schwanz / schwanz_form); T6/T8/T9/TQ als INFO (alte KI-Ziele).
  Kopf: riegel_v6kopf.pruefen gegen die Basis; T-H1i/ii, T-H2, T-H4, T-H5, T-H6 muessen gruen sein; dessen T-K1-Zeilen (Koerper
         unveraendert) sind hier durch T-K1 ersetzt (INFO).
"""
import os, sys, json, argparse, hashlib
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v5f_gemein as F
import koerper_bau as KBAU
import schwanz_form as SF
import schwanz_suche as SS
import kopf_orig_bau as KB
import riegel_v6kopf as RK
G = F.G
IOU_STAND_EMD = os.path.join(KB.AUS, 'EM30_V5r4.EMD')     # Stand Runde 4 (sha256 38562ced), gleich gemessen
PIXEL_BODEN = 1.0          # T-F5c: Inkreis auf der Binaermaske hat +-0,5 px je Rand; 8 % von 8-12 px liegen darunter
FOTO_RUMPFHAUT_L = 23.8


def iou(ids, b):
    T = F.trimap(b); m = ids >= 0
    Fm = T == 1; K = T != 2
    i = (m & Fm).sum(); u = ((m | Fm) & K).sum()
    return float(i / u) if u else 0.0


def pruefen(emd, basis, prot=None, mit_kopf=True):
    blob = open(emd, 'rb').read(); bas = open(basis, 'rb').read(); ref2 = open(KB.REF2, 'rb').read()
    d, _, ms, tim = KB.teile(blob); db, _, mb, timb = KB.teile(bas); _, _, mr, timr = KB.teile(ref2)
    t, tb, tr = KB.tim_teile(tim), KB.tim_teile(timb), KB.tim_teile(timr)
    R = dict(emd=emd, emd_sha256=hashlib.sha256(blob).hexdigest(), basis=basis, basis_sha256=hashlib.sha256(bas).hexdigest(), zeilen=[])

    def zeile_(name, ok, soll, ist, info=False):
        R['zeilen'].append(dict(name=name, ok=bool(ok), soll=soll, ist=ist, info=info))

    # ------------------------------------------------------------------ T-K1
    sek = [i for i in range(7) if blob[d[i]:d[i + 1]] != bas[db[i]:db[i + 1]]]
    zeile_('T-K1a Sektionen 0-6 (EDD, EMR, ...) = Basis', not sek and len(ms) == len(mb), 'bytegleich', 'abweichend %s; Meshes %d/%d' % (sek, len(ms), len(mb)))
    tex_ok = t['pix'].shape == tb['pix'].shape and np.array_equal(t['pix'], tb['pix'])
    zeile_('T-K1b Texel = Basis', tex_ok, 'bytegleich', 'gleich' if tex_ok else 'ABWEICHEND (%d Texel)' % int((t['pix'] != tb['pix']).sum()))
    aus = dict(clut_feld={}, krallen_ecken=[], schwanz_ecken=0)
    geo_fehler = []
    for i in range(len(mb)):
        a, b_ = ms[i], mb[i]
        if a['n'] != b_['n'] or a['tf'] != b_['tf'] or a['qf'] != b_['qf'] or len(a['v']) != len(b_['v']):
            geo_fehler.append('%d: Normalen/Flaechen' % i); continue
        for (k, ua), (_k, ub) in zip(KBAU.flaechen_schluessel(a), KBAU.flaechen_schluessel(b_)):
            if [x for j, x in enumerate(ua) if j != 2] != [x for j, x in enumerate(ub) if j != 2] or (ua[2] & 0x3F) != (ub[2] & 0x3F):
                geo_fehler.append('%d/%s: UV' % (i, k))
            elif ua[2] != ub[2]:
                aus['clut_feld'].setdefault(i, []).append((k, ub[2] >> 6, ua[2] >> 6))
        dv = [j for j in range(len(a['v'])) if a['v'][j] != b_['v'][j]]
        if i in SF_SCHWANZ:
            aus['schwanz_ecken'] += len(dv)
        elif i == 12:
            P = np.array([v[:3] for v in b_['v']], float)
            krallen = set()
            for f in b_['tf'] + b_['qf']:
                ie = list(f[1::2])
                if all(P[j, 1] >= 330 for j in ie):
                    krallen.update(ie)
            fremd = [j for j in dv if j not in krallen]
            aus['krallen_ecken'] = dv
            if fremd:
                geo_fehler.append('12: Ecken ausserhalb der Krallen %s' % fremd[:8])
        elif dv:
            geo_fehler.append('%d: Ecken %s' % (i, dv[:8]))
    # CLUT-Feld-Aenderungen nur: Schwanz -> 487; Mesh 8 (Brust) / 12 / 13 -> 488/489/490 gemaess Herkunftszeile
    KUPPE = KBAU.kuppe_13(bas)[0]
    erlaubt = {480: KBAU.Z_480, 481: KBAU.Z_481, 482: KBAU.Z_482}
    import f_analyse as FA
    falsch = []
    for i, lst in aus['clut_feld'].items():
        P = np.array([v[:3] for v in mb[i]['v']], float)
        fl = dict(zip([k for k, _ in KBAU.flaechen_schluessel(mb[i])], mb[i]['tf'] + mb[i]['qf']))
        for k, alt, neu in lst:
            if i in SF_SCHWANZ:
                ok = neu == KBAU.Z_SCHWANZ
            elif i in (8, 12, 13):
                ok = erlaubt.get(alt) == neu
                if i == 8:
                    ok &= k not in KBAU.SCHULTER_8 and FA.teil(8, P[list(fl[k][1::2])].mean(0)) == 'Brust'
                if i == 13:
                    ok &= k not in KUPPE
            else:
                ok = False
            if not ok:
                falsch.append('%d/%s %d->%d' % (i, k, alt, neu))
    schwanz_alle = all(uv[2] >> 6 == KBAU.Z_SCHWANZ for i in SF_SCHWANZ for _k, uv in KBAU.flaechen_schluessel(ms[i]))
    n_rot = {i: len(v) for i, v in aus['clut_feld'].items() if i not in SF_SCHWANZ}
    zeile_('T-K1c Geometrie/UV = Basis bis auf die Ausnahmeliste', not geo_fehler and not falsch and schwanz_alle,
           'nur CLUT-Feld (Rot-Flaechen 8 Brust/12/13 -> 488-490, Schwanz -> 487), Krallenecken Mesh 12, Schwanzecken 16-22',
           'Fehler %s; falsche CLUT-Felder %s; Schwanz alle 487 %s; Rot-Flaechen je Mesh %s; Krallenecken %d; Schwanzecken %d' % (
               geo_fehler[:6], falsch[:6], schwanz_alle, n_rot, len(aus['krallen_ecken']), aus['schwanz_ecken']))
    c, cb, cr = t['clut'], tb['clut'], tr['clut']
    nz = c.shape[0]
    ok_z = nz == 11 and nz <= 16 and t['ih'] * nz <= 4096 and np.array_equal(c[3:7], cb[3:7])
    ok_k = nz == 11 and np.array_equal(c[8], cr[1]) and np.array_equal(c[9], cr[0]) and np.array_equal(c[10], cr[2])
    pal_fehler, n_grau = [], {}
    for zi in range(3):
        lab_r = KBAU.lab15(cr[zi] & 0x7FFF)
        grau = lab_r[(lab_r[:, 1] < KBAU.GRAU_A) & (lab_r[:, 0] > 0)]
        cmax = float(np.percentile(np.hypot(grau[:, 1], grau[:, 2]), 90))
        dif = np.nonzero(c[zi] != cr[zi])[0]
        n_grau[480 + zi] = int(len(dif))
        for ix in dif:
            ln = KBAU.lab15(int(c[zi][ix]) & 0x7FFF)[0]
            if not (lab_r[ix, 1] >= KBAU.ROT_A and ln[0] <= lab_r[ix, 0] + 1e-6 and np.hypot(ln[1], ln[2]) <= cmax
                    and (int(c[zi][ix]) >> 15) == (int(cr[zi][ix]) >> 15)):
                pal_fehler.append('%d/%d' % (480 + zi, ix))
    # 487
    sw_idx = set()
    for i in SF_SCHWANZ:
        for _k, uv in KBAU.flaechen_schluessel(ms[i]):
            sw_idx.update(int(x) for x in t['pix'][KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)])
    sich = set()
    for _k, uv in KBAU.flaechen_schluessel(mr[15]):
        sich.update(int(cr[4][x]) for x in tr['pix'][KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)])
    z487_rest = nz > 7 and all(c[7][ix] == cr[0][ix] for ix in range(256) if ix not in sw_idx)
    z487_sw = nz > 7 and all(int(c[7][ix]) in sich for ix in sw_idx)
    zeile_('T-K1d CLUT: Ausnahmeliste', ok_z and ok_k and not pal_fehler and z487_rest and z487_sw,
           '11 Zeilen (<= 16, Hoehe <= 4096); 483-486 = Basis; 488/489/490 = Ref2 481/480/482; 480-482 nur rote Eintraege grau (nicht heller, '
           'C* <= P90 der grauen Eintraege derselben Zeile = Spanne der grauen Haut); 487 = Ref2 480 ausser Schwanz-Indizes, diese aus der Sichel-Zeile 484',
           '%d Zeilen, Hoehe %d; 483-486 gleich %s; Kopien gleich %s; geaendert je Zeile %s, Fehler %s; 487 Rest gleich %s, Schwanz-Indizes %d aus Sichel %s' % (
               nz, t['ih'] * nz, bool(np.array_equal(c[3:7], cb[3:7])) if nz >= 7 else False, ok_k, n_grau, pal_fehler[:6], z487_rest, len(sw_idx), z487_sw))
    R['ausnahmeliste'] = dict(rot_flaechen={str(i): [dict(flaeche=k, alt=a_, neu=n_) for k, a_, n_ in v] for i, v in aus['clut_feld'].items() if i not in SF_SCHWANZ},
                              grau_eintraege=n_grau, schwanz_indizes=len(sw_idx), krallen_ecken=aus['krallen_ecken'], schwanz_ecken=aus['schwanz_ecken'])
    # ------------------------------------------------------------------ Renders
    mo = F.V.modell(blob); mob = F.V.modell(bas)
    P = F.V.posen()
    img2, ids2, iv2 = G.zeichne(mo, P['P2'], voll=True); H2p = G.h_port(iv2)[2]
    img1, ids1, iv1 = G.zeichne(mo, P['P1'], voll=True); H1p = G.h_port(iv1)[2]
    # ------------------------------------------------------------------ T-F1
    soll = F.foto_breiten()
    mb_, kp = F.modell_breiten(mo, P['P2'], ids2, H2p)
    st = []
    ok1 = True
    for m_, s_ in zip(mb_, soll):
        if 0.1 <= m_['f'] <= 0.9:
            e = m_['d_H'] / s_['soll_H'] - 1
            ok1 &= abs(e) <= 0.20
            st.append('f %.1f %.3f/%.3f (%+.0f %%)' % (m_['f'], m_['d_H'], s_['soll_H'], 100 * e))
    zeile_('T-F1 Schwanzbreite je Station (Inkreis, P2) +-20 %', ok1, 'Modell/Foto-Soll (Median 3) je f 0,1-0,9, +-20 %', '; '.join(st))
    R['tf1'] = dict(modell=mb_, foto=soll)
    zeile_('INFO T-F1 Foto roh / Modell f 0 und 1', True, '-', 'Foto roh %s; Modell f 0 %.3f, f 1 %.3f' % (
        '/'.join('%.3f' % s_['d_H'] for s_ in soll), mb_[0]['d_H'], mb_[-1]['d_H']), info=True)
    # ------------------------------------------------------------------ T-F5 Farbe
    V2 = G.videoraum(img2); L2 = G.lab(V2)
    sw = np.isin(ids2, G.SCHWANZ)
    swe = cv2.erode(sw.astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)
    rumpf = np.isin(ids2, (7, 8)) & (L2[..., 1] < 6)
    lsw = L2[swe].mean(0) if swe.any() else np.full(3, np.nan)
    lr = L2[rumpf].mean(0) if rumpf.any() else np.full(3, np.nan)
    ok5c = lsw[1] <= 0 and lsw[2] >= 0 and lsw[0] < FOTO_RUMPFHAUT_L and lsw[0] < lr[0]
    zeile_('T-F5a Schwanz dunkel gruenlich-grau (P2)', ok5c, 'a* <= 0, b* >= 0, L* < Rumpfhaut (Foto 23,8; Modell-Rumpf grau)',
           'Schwanz %s (%d px, 1 px erodiert); Modell-Rumpf grau %s; Foto-Schwanz 17,8/4,8/1,5' % ('/'.join('%.1f' % x for x in lsw), int(swe.sum()),
                                                                                                    '/'.join('%.1f' % x for x in lr)))
    # ------------------------------------------------------------------ T-F5 Huckel
    rad = SF.radien_lesen(ms)
    sprung_k = []
    for i in range(6):
        e_ = [x for x in SF.ringe(ms[16 + i]) if len(x[0]) == SF.S_][-1][2]
        a_ = [x for x in SF.ringe(ms[17 + i]) if len(x[0]) == SF.S_][0][2]
        sprung_k.append(abs(a_ - e_) / e_)
    zeile_('T-F5b Gelenkringe deckungsgleich (Knochenraum)', max(sprung_k) <= 0.08, 'Radius Endring k / Anfangsring k+1: Sprung <= 8 %',
           'Spruenge 16/17..21/22: %s %%' % ' '.join('%.1f' % (100 * x) for x in sprung_k))
    # Bild P2
    D = cv2.distanceTransform(sw.astype(np.uint8), cv2.DIST_L2, 5)
    q = np.asarray(kp, float)
    bild = []
    ok5b = True
    for j in range(1, len(q) - 1):
        d1 = (q[j] - q[j - 1]) / np.linalg.norm(q[j] - q[j - 1]); d2 = (q[j + 1] - q[j]) / np.linalg.norm(q[j + 1] - q[j])
        def w(p):
            """Inkreis-Durchmesser am Punkt p (Maximum der Distanz im 5x5-Fenster); None, wenn p nicht im Schwanz liegt oder der Inkreis
            an Koerperpixel (Ids 0-15) stoesst (Breite dort durch Verdeckung begrenzt, kein Querschnitt)."""
            x, y = int(round(p[0])), int(round(p[1]))
            if not (0 <= x < 640 and 0 <= y < 480) or not sw[y, x]:
                return None
            win = D[max(y - 2, 0):y + 3, max(x - 2, 0):x + 3]
            iy, ix = np.unravel_index(int(np.argmax(win)), win.shape)
            cy, cx, r = max(y - 2, 0) + iy, max(x - 2, 0) + ix, float(win.max())
            for a in np.linspace(0, 2 * np.pi, 48, endpoint=False):
                xx, yy = int(round(cx + (r + 1.5) * np.cos(a))), int(round(cy + (r + 1.5) * np.sin(a)))
                if 0 <= xx < 640 and 0 <= yy < 480 and 0 <= ids2[yy, xx] < 16:
                    return None
            return 2 * r
        vor = [(s, w(q[j] - s * d1)) for s in range(2, 9)]; nach = [(s, w(q[j] + s * d2)) for s in range(2, 9)]
        vor = [(s, x) for s, x in vor if x]; nach = [(s, x) for s, x in nach if x]
        if len(vor) < 4 or len(nach) < 4:
            bild.append('%d/%d verdeckt' % (15 + j, 16 + j)); continue
        pv = np.polyfit([-s for s, _ in vor], [x for _, x in vor], 1); pn = np.polyfit([s for s, _ in nach], [x for _, x in nach], 1)
        wv, wn = float(np.polyval(pv, 0)), float(np.polyval(pn, 0))
        spr = abs(wn - wv) / max(wv, 1e-6)
        ok5b &= abs(wn - wv) <= max(0.08 * wv, PIXEL_BODEN)
        bild.append('%d/%d %.1f->%.1f px (%.0f %%)' % (15 + j, 16 + j, wv, wn, 100 * spr))
    zeile_('T-F5c Breitensprung im Bild (P2)', ok5b, 'Inkreis 2-8 px vor/nach jedem sichtbaren Gelenk (Inkreis nicht an Koerperpixeln = nicht verdeckt, je Seite >= 4 Proben), zum Gelenk fortgesetzt: Sprung <= 8 % bzw. <= 1 px (Rasterboden)', '; '.join(bild))
    kn = SF.profil_knicke_geo(ms)
    zeile_('T-F5d Steigungsknick der Mantellinie <= 15 Grad', max(k['knick'] for k in kn) <= 15.0, '<= 15 Grad je Gelenk (Knochenraum)',
           '; '.join('%s %.1f' % (k['zwischen'], k['knick']) for k in kn))
    stufen = []
    ok5e = True
    for pn, ids_ in (('P2', ids2), ('85', G.zeichne(mo, G.posen()['85'])[1])):
        top = {}
        for x in range(250, 470):
            col = np.nonzero(ids_[:, x] >= 0)[0]
            top[x] = (int(col.min()), int(ids_[col.min(), x])) if len(col) else None
        we = [x for x in range(251, 470) if top[x] and top[x - 1] and ((top[x][1] >= 16) != (top[x - 1][1] >= 16))]
        sw_x = [x for x in top if top[x] and top[x][1] >= 16]; ko_x = [x for x in top if top[x] and top[x][1] < 16]
        luecke = sw_x and ko_x and any(top[x] is None for x in range(min(max(sw_x), max(ko_x)), max(min(ko_x), min(sw_x)) + 1)
                                       if min(max(sw_x), max(ko_x)) < max(min(ko_x), min(sw_x)))
        if not we:
            stufen.append('%s kein Wechsel' % pn); ok5e = False; continue
        x0 = we[0]
        dys = [abs(top[x0][0] - top[x0 - 1][0])]
        if pn == 'P2':
            ok5e &= max(dys) <= 2 and not luecke
        stufen.append('%s Wechsel x %d (y %d -> %d), Stufe %d px, Luecke %s' % (pn, x0, top[x0 - 1][0], top[x0][0], max(dys), bool(luecke)))
    zeile_('T-F5e Stufe Wurzel/Ruecken <= 2 px, keine Luecke (P2 = Foto-2-Ansicht)', ok5e,
           'obere Kontur am Wechsel Schwanz -> Rumpf in P2 (Messblatt 2/5); Pose 85 (posen_v4, abgeloest) nur ausgegeben', '; '.join(stufen))
    # ------------------------------------------------------------------ T-F7
    rk = {b: F.rot_karte(mo, b) for b in (1, 2)}
    R['rot_karte'] = {b: dict(regionen=v['regionen'], teile=v['teile']) for b, v in rk.items()}
    for n, v in rk[1]['regionen'].items():
        if n.startswith('Kopf'):
            zeile_('INFO T-F7 Foto 1 Region %s' % n, True, 'Kopf ausgenommen', 'Foto %.1f %% / Modell %.1f %%' % (100 * v['foto'], 100 * v['modell']), info=True)
            continue
        ok = abs(v['modell'] - v['foto']) <= 0.05
        if n.startswith('Schulter'):
            ok &= v['modell'] <= 0.05
        zeile_('T-F7 Foto 1 Region %s' % n, ok, '+-5 Prozentpunkte%s' % (', <= 5 % (kein Rot)' if n.startswith('Schulter') else ''),
               'Foto %.1f %% / Modell %.1f %%' % (100 * v['foto'], 100 * v['modell']))
    for n, v in rk[1]['teile'].items():
        zeile_('INFO T-F7 Foto 1 Teil %s' % n, True, 'Id-Zuordnung P1 nur grob (Messblatt 4.2)', 'Foto %.1f %% / Modell %.1f %% (%d px)' % (
            100 * v['foto'], 100 * v['modell'], v['px']), info=True)
    for n, v in rk[2]['regionen'].items():
        aus_ = n.startswith('Kopf') or n.startswith('Schwanz')
        zeile_(('INFO ' if aus_ else '') + 'T-F7 Foto 2 Region %s' % n, aus_ or abs(v['modell'] - v['foto']) <= 0.05,
               '+-5 Prozentpunkte' if not aus_ else 'ausgenommen', 'Foto %.1f %% / Modell %.1f %%' % (100 * v['foto'], 100 * v['modell']), info=aus_)
    for n, v in rk[2]['teile'].items():
        aus_ = n in ('Kopf', 'Schwanz') or v['px'] < 100
        zeile_(('INFO ' if aus_ else '') + 'T-F7 Foto 2 Teil %s' % n, aus_ or abs(v['modell'] - v['foto']) <= 0.05,
               '+-5 Prozentpunkte' if not aus_ else 'ausgenommen (Kopf/Schwanz/< 100 px)',
               'Foto %.1f %% / Modell %.1f %% (%d px)' % (100 * v['foto'], 100 * v['modell'], v['px']), info=aus_)
    # ------------------------------------------------------------------ T-F8
    import f_krallen as FK
    import f_analyse as FA2
    def krallen_l(m_, Hp):
        out = []
        for Q in FK.krallen(m_):
            tip = Q[np.argmax(Q[:, 1])]; root = Q[np.argmin(Q[:, 1])]
            pp = FA2.welt(m_, P['P1'], 12, [root, tip])
            out.append(float(np.linalg.norm(pp[1] - pp[0])) / Hp)
        return out
    H1b = G.h_port(G.zeichne(mob, P['P1'], voll=True)[2])[2]
    kn_, kb_ = krallen_l(mo, H1p), krallen_l(mob, H1b)
    ok8 = len(kn_) == len(kb_) and all(abs(x / KBAU.KRALLE_SOLL_H - 1) <= 0.15 for x in kn_) and all(x >= y - 1e-4 for x, y in zip(kn_, kb_))
    zeile_('T-F8 Krallen (W)-links Laenge (P1)', ok8, '0,036 H +-15 % je Kralle, keine kuerzer als in der Basis',
           'neu %s H; Basis %s H' % ('/'.join('%.3f' % x for x in kn_), '/'.join('%.3f' % x for x in kb_)))
    # ------------------------------------------------------------------ T-F4
    i1, i2 = iou(ids1, 1), iou(ids2, 2)
    mo4 = F.V.modell(IOU_STAND_EMD)
    s1, s2 = iou(G.zeichne(mo4, P['P1'])[1], 1), iou(G.zeichne(mo4, P['P2'])[1], 2)
    zeile_('T-F4 Umriss-IoU (Trimap) >= Stand Runde 4', i1 >= s1 - 1e-4 and i2 >= s2 - 1e-4,
           'P1 >= %.4f, P2 >= %.4f (V5r4 gleich gemessen: Render ids >= 0; Messblatt mit Umriss-Render 0,709 / 0,758)' % (s1, s2),
           'P1 %.4f, P2 %.4f (Basis V5k %.4f / %.4f)' % (i1, i2, iou(G.zeichne(mob, P['P1'])[1], 1), iou(G.zeichne(mob, P['P2'])[1], 2)))
    # ------------------------------------------------------------------ Riegel V4 Schwanz (Pose 85)
    import riegel_v4 as RV
    P4 = G.posen()
    Zs, _s, _ = RV.schwanz(mo, P4)
    Zf, _ = RV.schwanz_form(mo, P4)
    for z in Zs + Zf:
        nm = z['mass'].split(' ')[0]
        pflicht = nm in ('T1', 'T2', 'T3', 'T4', 'TA', 'T5', 'TK')
        zeile_(('' if pflicht else 'INFO ') + 'V4 ' + z['mass'], z['ok'] or not pflicht, z['soll'], z['ist'] + ('' if pflicht or z['ok'] else ' [ROT, alte KI-Zielwerte]'),
               info=not pflicht)
    # ------------------------------------------------------------------ Kopf
    if mit_kopf:
        RK_ = RK.pruefen(emd, basis)
        for z in RK_['zeilen']:
            if z['name'].startswith('T-K1'):
                zeile_('INFO Kopf-Riegel ' + z['name'], True, z['soll'], z['ist'] + (' [hier durch T-K1 ersetzt]' if not z['ok'] else ''), info=True)
            else:
                zeile_('Kopf ' + z['name'] if not z['info'] else 'INFO Kopf ' + z['name'], z['ok'], z['soll'], z['ist'], info=z['info'])
        R['kopf'] = RK_.get('posen')
    R['rot'] = [z['name'] for z in R['zeilen'] if not z['ok'] and not z['info']]
    return R


SF_SCHWANZ = tuple(range(16, 23))


def bericht(R):
    z = ['Riegel SF V5 wie Foto - Koerper (riegel_v5f.py)', 'EMD %s sha256 %s' % (R['emd'], R['emd_sha256']),
         'Basis %s sha256 %s' % (R['basis'], R['basis_sha256']), '']
    for e in R['zeilen']:
        z.append('%-5s %s | Soll: %s | Ist: %s' % ('INFO' if e['info'] else ('OK' if e['ok'] else 'ROT'), e['name'], e['soll'], e['ist']))
    z.append('')
    z.append('Ergebnis: %d rot von %d (%s)' % (len(R['rot']), sum(1 for e in R['zeilen'] if not e['info']), ', '.join(R['rot']) or '-'))
    return '\n'.join(z)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', required=True); ap.add_argument('--basis', default=KBAU.BASIS)
    ap.add_argument('--aus'); ap.add_argument('--name', default='riegel_v5f'); ap.add_argument('--ohne-kopf', action='store_true')
    a = ap.parse_args()
    R = pruefen(a.emd, a.basis, mit_kopf=not a.ohne_kopf)
    txt = bericht(R)
    print(txt)
    if a.aus:
        os.makedirs(a.aus, exist_ok=True)
        open(os.path.join(a.aus, a.name + '.txt'), 'w', encoding='utf-8').write(txt + '\n')
        json.dump(R, open(os.path.join(a.aus, a.name + '.json'), 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if R['rot'] else 0)


if __name__ == '__main__':
    main()
