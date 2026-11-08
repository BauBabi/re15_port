"""Messer V4 Feinschliff (2026-11-08): Hauptmessung -> messung_v4f.json.

    python messen.py <cache-Ordner> <aus.json>

Voraussetzung: blick_alle.py und ketten.py haben den Cache gefuellt (Szene Bild 1/2 je Stand, Kennungs-Puffer, Videofilter Saaten 15/12345/7,
Kette T = p_zonen.rueck(video_filter(...))).
Teile:
 1 Details: helle Flecken der Vorlage (T-L* > 40 Bild 1 / > 28 Bild 2, in den Nutzer-Teilmasken, 2 px vom Rand, ohne Logo/Boden/Weisskappe),
   je Fleck Lage, Lab (Video roh und T), Modellort (Mesh, Gelenkkoerper, Zeile) am Asset und an K4, Ketten-L* aller Staende auf denselben Pixeln.
 2 Regionen der Nutzerpunkte: Flaechen, Zeilen, Eintraege, Mitnutzer, exklusive Texel; Krallen Anzahl/Form/Farbe.
 3 Helligkeit: mittleres L* Figur und Regionen, mit und ohne Videofilter, Faktor je Region/Zeile per Bisektion.
 4 Zeilenbudget.
"""
import collections
import json
import sys

import cv2
import numpy as np

import v4f
import ketten as KT
import gelenke as G
import referenzen as RF

PZ = v4f.PZ
LOGO = np.zeros((480, 640), bool)
LOGO[428:, :262] = True
NT = v4f.FORM + '/nutzer_teile/maske_%s.png'
NAMEN_STAND = [nm for nm, _o in v4f.STAENDE]


def r1(x):
    return [round(float(v), 1) for v in np.atleast_1d(x)]


def nutzer_union(n):
    um = np.zeros((480, 640), bool)
    for _a, m in PZ.regionen(n):
        um |= m
    return um


def lab_mittel(T, m):
    return r1(v4f.lab(np.clip(T[m], 0, 255)).mean(0)) if m.any() else None


def ketten_lab(cache, nm, n, m):
    """Mittel ueber 3 Saaten der Kette T (Lab) auf Pixelmenge m."""
    T = KT.kette(cache, nm, n)['T']
    L = [v4f.lab(np.clip(t[m], 0, 255)).mean(0) for t in T]
    return r1(np.mean(L, 0)) if m.any() else None


def modellort(K, m, gel):
    c = collections.Counter()
    zc = collections.Counter()
    for y, x in zip(*np.nonzero(m & K['maske'])):
        key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
        c[gel.get(key, 'Mesh %d' % key[0])] += 1
        zc[int(K['zeile'][y, x])] += 1
    return dict(teile=c.most_common(5), zeilen=dict(zc.most_common(4)), modell_px=int((m & K['maske']).sum()))


def albedo_lab(ordner, K, m):
    k = v4f.kand(ordner)
    mm = m & K['maske']
    if not mm.any():
        return None
    rgb = k.tex.lut[K['zeile'][mm].clip(0), K['eintrag'][mm].clip(0)][:, :3]
    return r1(v4f.lab(rgb).mean(0))


# ------------------------------------------------------------------ 1 Details
def flecken(cache, n, schwelle, mindest):
    O = KT.original(cache, n)
    To = O['T'][0]
    Lo = v4f.lab(np.clip(To, 0, 255))
    um = nutzer_union(n)
    ume = cv2.erode(um.astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    spot = (Lo[..., 0] > schwelle) & ume & ~LOGO & ~PZ.BODEN & ~O['clip']
    nl, lab, st, ce = cv2.connectedComponentsWithStats(spot.astype(np.uint8), connectivity=8)
    return [(lab == i, st[i], ce[i]) for i in range(1, nl) if st[i, 4] >= mindest], To


def details(cache):
    gel = {nm: G.gelenk_flaechen(o)[0] for nm, o in v4f.STAENDE}
    vid = {n: v4f.original(n) for n in (1, 2)}
    aus = {}
    masken = {}
    for n, schwelle, mindest in ((1, 40.0, 8), (2, 28.0, 10)):
        fl, To = flecken(cache, n, schwelle, mindest)
        Ks = {nm: KT.lade(cache, nm)[n]['K'] for nm in NAMEN_STAND}
        liste = []
        for i, (m, st, ce) in enumerate(fl):
            e = dict(id='B%d-F%02d' % (n, i + 1), px=int(st[4]), mitte=[int(round(ce[0])), int(round(ce[1]))],
                     bbox=[int(st[0]), int(st[1]), int(st[0] + st[2] - 1), int(st[1] + st[3] - 1)],
                     lab_video=lab_mittel(vid[n].astype(float), m), lab_T=lab_mittel(To, m),
                     ort_asset=modellort(Ks['Asset K5c'], m, gel['Asset K5c']), ort_K4=modellort(Ks['K4'], m, gel['K4']),
                     albedo_asset=albedo_lab(v4f.ST['Asset K5c'], Ks['Asset K5c'], m), albedo_K4=albedo_lab(v4f.ST['K4'], Ks['K4'], m),
                     kette={nm: ketten_lab(cache, nm, n, m) for nm in NAMEN_STAND})
            liste.append(e)
            masken[e['id']] = m
        aus['bild%d' % n] = dict(schwelle_T_L=schwelle, mindest_px=mindest, flecken=liste)
    return aus, masken


# ------------------------------------------------------------------ 2 Regionen am Modell
def flaechen_menge(F, sel):
    return [i for i, f in enumerate(F) if sel(f)]


def region_bericht(ordner, F, tex, fl, idx_region, name):
    """Zeilen/Eintraege der Region, Mitnutzer je Eintrag (Flaechen ausserhalb der Region in derselben Zeile), exklusive Texel."""
    R = set(idx_region)
    k = v4f.kand(ordner)
    lut = k.tex.lut
    zeilen = collections.Counter(F[i]['zeile'] for i in R)
    eintr = {}
    for i in R:
        f = F[i]
        u, c = np.unique(f['idx'], return_counts=True)
        for e_, c_ in zip(u, c):
            d = eintr.setdefault((f['zeile'], int(e_)), [0, set()])
            d[0] += int(c_)
            d[1].add(i)
    zeilen_info = {}
    for z in sorted(zeilen):
        ez = sorted(e for (zz, e) in eintr if zz == z)
        nur_region = [e for e in ez if fl[z][e] <= R]
        geteilt = [e for e in ez if not fl[z][e] <= R]
        fremd = set()
        for e in geteilt:
            fremd |= fl[z][e] - R
        mit = collections.Counter(F[j]['mesh'] for j in fremd)
        tex_reg = sum(eintr[(z, e)][0] for e in ez)
        L = v4f.lab(lut[z][ez][:, :3])
        w = np.array([eintr[(z, e)][0] for e in ez], float)
        zeilen_info[z] = dict(flaechen_region=zeilen[z], flaechen_zeile=len({j for e in range(256) for j in fl[z][e]}),
                              eintraege=len(ez), eintraege_nur_region=len(nur_region), eintraege_geteilt=len(geteilt),
                              texel_region=int(tex_reg), texel_zeile=int(tex[z].sum()),
                              mitnutzer_meshes=dict(sorted(mit.items())),
                              albedo_lab_texelgewichtet=r1((L * w[:, None]).sum(0) / w.sum()),
                              rot_anteil_albedo=round(100.0 * float(w[(L[:, 1] > 15) & (np.hypot(L[:, 1], L[:, 2]) > 15)].sum() / w.sum()), 1),
                              eintraege_liste=ez)
    # exklusive Texel: TIM-Texel, die nur von Regionsflaechen bedeckt sind
    H, W = k.tex.px.shape
    deck = np.zeros((H, W), np.int32)
    deck_r = np.zeros((H, W), np.int32)
    for i, f in enumerate(F):
        deck[f['ys'], f['xs']] += 1
        if i in R:
            deck_r[f['ys'], f['xs']] += 1
    reg = deck_r > 0
    excl = reg & (deck == deck_r)
    return dict(name=name, flaechen=len(R), meshes=sorted({F[i]['mesh'] for i in R}), zeilen=zeilen_info,
                texel_bedeckt=int(reg.sum()), texel_exklusiv=int(excl.sum()), texel_geteilt=int((reg & ~excl).sum()))


def flaechen_an_pixeln(K, m, F):
    """Flaechenindizes (in F) der am Pixelset m sichtbaren Flaechen, mit Pixelzahl."""
    pos = {(f['mesh'], f['viereck'], f['fi']): i for i, f in enumerate(F)}
    c = collections.Counter()
    for y, x in zip(*np.nonzero(m & K['maske'])):
        key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
        c[pos[key]] += 1
    return c


def lippe_flaechen(ordner, F):
    """Lippe = Kopf-/Kiefer-Flaechen (Meshes 22/23), die in einer der 10 Kopfansichten (Clip 1 Maul offen, Clip 2 Ruhe; 5 Winkel) innerhalb 4 px
    an Zahn-Pixel (Zeile 7) grenzen, ohne die Zahnflaechen selbst. Rueckgabe Flaechenindizes mit Pixelzahl und rote Pixel (Albedo a* > 15, C* > 15)."""
    k = v4f.kand(ordner)
    pos = {(f['mesh'], f['viereck'], f['fi']): i for i, f in enumerate(F)}
    c, rot = collections.Counter(), collections.Counter()
    for clip in (1, 2):
        for az, el in ((60, 10), (0, 5), (80, -30), (90, 0), (120, 10)):
            _a, K = v4f.rundum_bild(ordner, clip, 0, az, el, 300, teile=(22, 23))
            zahn = (K['zeile'] == 7) & K['maske']
            nah = cv2.dilate(zahn.astype(np.uint8), np.ones((9, 9), np.uint8)) > 0
            m = nah & K['maske'] & ~zahn
            rgb = k.tex.lut[K['zeile'].clip(0), K['eintrag'].clip(0)][..., :3]
            L = v4f.lab(rgb)
            r = m & (L[..., 1] > 15) & (np.hypot(L[..., 1], L[..., 2]) > 15)
            for y, x in zip(*np.nonzero(m)):
                key = (int(K['mesh'][y, x]), int(K['flaeche'][y, x] >= 100000), int(K['flaeche'][y, x] % 100000))
                c[pos[key]] += 1
                if r[y, x]:
                    rot[pos[key]] += 1
    return c, rot


def krallen(cache, aus_det):
    """Anzahl, Form, Farbe der Vorderkrallen (Bild 2)."""
    import krallen_spitzen as KS
    res = {}
    Ka = KT.lade(cache, 'Asset K5c')[2]['K']
    K4 = KT.lade(cache, 'K4')[2]['K']
    O = KT.original(cache, 2)
    To = O['T'][0]
    vid = v4f.original(2).astype(float)
    pts = KS.modell_spitzen(v4f.ST['Asset K5c'])
    for hand, (maske, hm, klauen) in KS.HAND.items():
        um = cv2.imread(NT % maske, 0) > 0
        mitte = pts[hm].mean(0)
        sp = KS.umriss_spitzen(um, mitte)
        kl = {}
        for kb in klauen:
            m = (Ka['mesh'] == kb)
            q = pts[kb]
            i = int(np.argmax(np.linalg.norm(q - mitte, axis=1)))
            ys, xs = np.nonzero(m)
            mo = m & um & ~LOGO & ~O['clip']
            kl[kb] = dict(sichtbar_px=int(m.sum()), px_ausserhalb_nutzerumriss=int((m & ~um).sum()), spitze=r1(q[i]),
                          sichtbare_ausdehnung_px=round(float(np.hypot(xs.max() - xs.min(), ys.max() - ys.min())), 1) if m.any() else 0,
                          naechste_umriss_spitze=min(([round(float(np.hypot(q[i][0] - p[0], q[i][1] - p[1])), 1), p] for _w, p in sp),
                                                     default=None),
                          vorlage_lab_T=lab_mittel(To, mo), vorlage_lab_video=lab_mittel(vid, mo), px_messung=int(mo.sum()),
                          asset_kette_lab=ketten_lab(cache, 'Asset K5c', 2, mo), k4_kette_lab=ketten_lab(cache, 'K4', 2, mo))
        hand_m = (Ka['mesh'] == hm) & um & ~LOGO & ~O['clip']
        k4_kl = np.isin(K4['mesh'], klauen) & um & ~LOGO & ~O['clip']
        alle_kl = np.isin(Ka['mesh'], klauen) & um & ~LOGO & ~O['clip']
        res[hand] = dict(nutzermaske=maske, umriss_spitzen=sp, klauen=kl,
                         klauen_gesamt=dict(vorlage_lab_T=lab_mittel(To, alle_kl), asset_kette_lab=ketten_lab(cache, 'Asset K5c', 2, alle_kl),
                                            k4_kette_lab_k4pixel=ketten_lab(cache, 'K4', 2, k4_kl), px=int(alle_kl.sum())),
                         hand=dict(vorlage_lab_T=lab_mittel(To, hand_m), asset_kette_lab=ketten_lab(cache, 'Asset K5c', 2, hand_m), px=int(hand_m.sum())))
    return res


# ------------------------------------------------------------------ 3 Helligkeit
def bisekt(fun, ziel, lo=0.3, hi=8.0, it=16):
    flo, fhi = fun(lo), fun(hi)
    if not (flo <= ziel <= fhi):
        return None
    for _ in range(it):
        mid = 0.5 * (lo + hi)
        if fun(mid) < ziel:
            lo = mid
        else:
            hi = mid
    return round(0.5 * (lo + hi), 3)


def helligkeit(cache):
    res = {}
    for n in (1, 2):
        O = KT.original(cache, n)
        Lo = v4f.lab(np.clip(O['T'][0], 0, 255))
        eo = O['e']
        Le = v4f.lab(np.clip(eo, 0, 255))
        Lv = v4f.lab(v4f.original(n).astype(float))
        regs = PZ.regionen(n)
        um = nutzer_union(n)
        d = KT.lade(cache, 'Asset K5c')[n]
        Ka, sauber = d['K'], d['sauber']
        fig = um & Ka['maske'] & ~O['clip']
        R = {}
        for nm in NAMEN_STAND + ['Original']:
            if nm == 'Original':
                LT, LV, LS = Lo, Lv, Le
            else:
                dd = KT.lade(cache, nm)[n]
                LT = np.mean([v4f.lab(np.clip(t, 0, 255)) for t in KT.kette(cache, nm, n)['T']], 0)
                LV = np.mean([v4f.lab(dd['video'][s].astype(float)) for s in v4f.SAATEN], 0)
                LS = v4f.lab(dd['sauber'].astype(float))
            figm = fig
            z = dict(figur=dict(L_T=round(float(LT[figm][:, 0].mean()), 2), L_video=round(float(LV[figm][:, 0].mean()), 2),
                                L_ohne_filter=round(float(LS[figm][:, 0].mean()), 2), px=int(figm.sum())))
            for a, m in regs:
                mm = m & figm
                z[a] = dict(L_T=round(float(LT[mm][:, 0].mean()), 2), L_video=round(float(LV[mm][:, 0].mean()), 2),
                            L_ohne_filter=round(float(LS[mm][:, 0].mean()), 2), px=int(mm.sum()))
            R[nm] = z
        # Faktor am Asset: Modellpixel (bzw. Region/Zeile) im Disc-Licht-Bild sauber mit f skaliert (linear wie Textur x Licht / 255), dann
        # mit Filter: video_filter (Saat 15) und mittleres L* des Videos auf den Messpixeln gegen das Vorlagen-Video (gleiche Pixel);
        # ohne Filter: sauber gegen video_umkehr(Vorlage). Pruefung am Endfaktor zusaetzlich in T (p_zonen.rueck) und mit 3 Saaten.
        hg = KT.lade(cache, 'Asset K5c')[n]['sauber']

        def skal(f, maske_skal):
            s_ = hg.astype(np.float32).copy()
            s_[maske_skal] = np.clip(s_[maske_skal] * f, 0, 255)
            return s_

        def mess_V(f, maske_skal, maske_mess, saat=15):
            v = v4f.video(skal(f, maske_skal).astype(np.uint8), n, saat)
            return float(v4f.lab(v[maske_mess].astype(float))[:, 0].mean())

        def mess_T(f, maske_skal, maske_mess, saat=15):
            v = v4f.video(skal(f, maske_skal).astype(np.uint8), n, saat)
            T = PZ.rueck(v, n)[0]
            return float(v4f.lab(np.clip(T[maske_mess], 0, 255))[:, 0].mean())

        def mess_S(f, maske_skal, maske_mess):
            return float(v4f.lab(skal(f, maske_skal)[maske_mess])[:, 0].mean())

        def eintrag(maske_skal, mm, pruef_T=False):
            zV, zS, zT = float(Lv[mm][:, 0].mean()), float(Le[mm][:, 0].mean()), float(Lo[mm][:, 0].mean())
            e = dict(px=int(mm.sum()), vorlage_L_video=round(zV, 2), asset_L_video=round(mess_V(1.0, maske_skal, mm), 2),
                     vorlage_L_ohne=round(zS, 2), asset_L_ohne=round(mess_S(1.0, maske_skal, mm), 2), vorlage_L_T=round(zT, 2),
                     mit_filter=bisekt(lambda f: mess_V(f, maske_skal, mm), zV), ohne_filter=bisekt(lambda f: mess_S(f, maske_skal, mm), zS))
            if pruef_T and e['mit_filter']:
                f_ = e['mit_filter']
                e['pruef_bei_f_L_video_3_saaten'] = round(float(np.mean([mess_V(f_, maske_skal, mm, s_) for s_ in v4f.SAATEN])), 2)
                e['pruef_bei_f_L_T_saat15'] = round(mess_T(f_, maske_skal, mm), 2)
            return e
        fak = {}
        modell = Ka['maske']
        fak['figur'] = eintrag(modell, fig, True)
        for a, m in regs:
            mm = m & fig
            if mm.sum() < 50:
                continue
            fak[a] = eintrag(modell & cv2.dilate(m.astype(np.uint8), np.ones((7, 7), np.uint8)).astype(bool), mm, True)
        fz = {}
        for z in range(16):
            mz = fig & (Ka['zeile'] == z)
            if mz.sum() < 80:
                continue
            fz[z] = eintrag(Ka['maske'] & (Ka['zeile'] == z), mz)
        res['bild%d' % n] = dict(L=R, faktor=fak, faktor_je_zeile=fz)
    return res


def spielfenster(ablage):
    """Echtes Fenster (Tafel 11): Figurmaske aus form_buehne (Raum-Kamera, Pose wie Spiellauf), per deckung() verschoben; mittleres L* im Fenster.
    Original: Video-Standbild, Nutzer-Teilmasken."""
    FB = v4f.FB
    E = v4f.K5 + '/einbau/spiel/'
    H = v4f.K5 + '/farbe/spiel_v3/'
    AS = v4f.WT + '/re15_port/shared_assets/RE2/BAHNSTEIG_FORM.'
    K4A = v4f.K5 + '/../K4/BAHNSTEIG_FORM.'
    faelle = [('5080 Nachstellung Bild 1', '5080', 1, (-20711, -15984, 756), [('Asset K5c', AS, 0, 11, E + 'einbau_pose1_nachstellung_r5080.png'),
                                                                               ('K4', K4A, 0, 0, H + 'heute_spiel_pose1_nachstellung_r5080.png')], 1),
              ('5090 Pose Bild 2', '5090', 11, (941, -23776, 3874), [('Asset K5c', AS, 12, 0, E + 'einbau_pose2_r5090_s11.png'),
                                                                      ('K4', K4A, 1, 0, H + 'heute_spiel_pose2_r5090_s11.png')], 2)]
    res = {}
    for titel, raum, schnitt, (x, z, gier), staende, n in faelle:
        R = FB.Raum(raum)
        s = R.schnitt(schnitt)
        kam = FB.Kamera.aus_rid(s['pos'], s['ziel'], s['fov'])
        licht = R.licht(schnitt)
        r = {}
        for nm, pfad, clip, bild, png in staende:
            emd = open(pfad + 'EMD', 'rb').read()
            tim = open(pfad + 'TIM', 'rb').read()
            render, maske = FB.buehne(emd, tim, clip, bild, (x, 0, z), gier, kam, licht, None)
            sp = FB.bild_laden(png)
            dk = FB.deckung(render, maske, sp)
            m = np.roll(np.roll(maske, dk['dy'], 0), dk['dx'], 1)
            L = v4f.lab(np.asarray(sp, float))
            r[nm] = dict(bild=png.split('/')[-1], px=int(m.sum()), versatz=[dk['dx'], dk['dy']], abstand_deckung=round(float(dk['abstand']), 1),
                         L_mittel=round(float(L[m][:, 0].mean()), 2), L_p90=round(float(np.percentile(L[m][:, 0], 90)), 2),
                         lab_mittel=r1(L[m].mean(0)))
            np.save(ablage + '/spielmaske_%s_%s.npy' % (raum, nm.replace(' ', '_')), m)
        o = v4f.original(n).astype(float)
        um = nutzer_union(n) & ~LOGO
        Lo = v4f.lab(o)
        r['Original (Video-Standbild)'] = dict(px=int(um.sum()), L_mittel=round(float(Lo[um][:, 0].mean()), 2),
                                               L_p90=round(float(np.percentile(Lo[um][:, 0], 90)), 2), lab_mittel=r1(Lo[um].mean(0)))
        res[titel] = r
    return res


def main():
    cache, aus = sys.argv[1], sys.argv[2]
    import os
    zw = aus.replace('.json', '_teil1.json')
    if os.path.exists(zw):
        out = json.load(open(zw, encoding='utf-8'))
        zh = aus.replace('.json', '_teil2.json')
        if os.path.exists(zh):
            out['helligkeit'] = json.load(open(zh, encoding='utf-8'))
        else:
            out['helligkeit'] = helligkeit(cache)
            json.dump(out['helligkeit'], open(zh, 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=str)
        print('Helligkeit fertig', flush=True)
        out['spielfenster'] = spielfenster(os.path.dirname(aus))
        print('Spielfenster fertig', flush=True)
        json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
        print('fertig', aus)
        return
    out = {}
    det, masken = details(cache)
    out['details'] = det
    np.savez_compressed(aus.replace('.json', '_flecken.npz'), **{k: v for k, v in masken.items()})
    print('Details fertig', flush=True)
    # Regionen
    oa = v4f.ST['Asset K5c']
    F, tex, fl, me = RF.tabelle(oa)
    gel, _n = G.gelenk_flaechen(oa)
    pos = {(f['mesh'], f['viereck'], f['fi']): i for i, f in enumerate(F)}
    gel_idx = collections.defaultdict(list)
    for key, nm in gel.items():
        gel_idx[nm].append(pos[key])
    reg = {}
    reg['Krallen vorn (Meshes 14,15,28,30 / 19,20,29,31)'] = region_bericht(oa, F, tex, fl, flaechen_menge(F, lambda f: f['mesh'] in (14, 15, 28, 30, 19, 20, 29, 31)), 'Krallen')
    reg['Schwanzspitze (Mesh 6)'] = region_bericht(oa, F, tex, fl, flaechen_menge(F, lambda f: f['mesh'] == 6), 'Schwanzspitze')
    Ka1 = KT.lade(cache, 'Asset K5c')[1]['K']
    Ka2 = KT.lade(cache, 'Asset K5c')[2]['K']
    fm = np.load(aus.replace('.json', '_flecken.npz'))
    # Schwanzspitze nach Vorlage: sichtbare Flaechen an B1-Fleck Schwanzspitze
    for e in det['bild1']['flecken']:
        if e['ort_asset']['teile'] and e['ort_asset']['teile'][0][0] == 'Mesh 6':
            c = flaechen_an_pixeln(Ka1, fm[e['id']], F)
            reg['Schwanzspitze, am Vorlagenfleck %s sichtbar' % e['id']] = region_bericht(oa, F, tex, fl, list(c), 'Schwanzspitze sichtbar')
    # Gelenke
    for nm in sorted(gel_idx):
        reg['Gelenkkoerper ' + nm] = region_bericht(oa, F, tex, fl, gel_idx[nm], nm)
    alle_gel = [i for v in gel_idx.values() for i in v]
    reg['Gelenkkoerper alle (31)'] = region_bericht(oa, F, tex, fl, alle_gel, 'alle Gelenkkoerper')
    # Gelenk-Flecken Bild 1/2: sichtbare Flaechen an den hellen Flecken ausser F1-Leuchtstelle und Schwanzspitze
    for n, Ka in ((1, Ka1), (2, Ka2)):
        c = collections.Counter()
        for e in det['bild%d' % n]['flecken']:
            t0 = e['ort_asset']['teile'][0][0] if e['ort_asset']['teile'] else ''
            if t0 in ('Mesh 6',) or (n == 1 and e['ort_asset']['zeilen'].get(5, 0) > 0.5 * max(e['ort_asset']['modell_px'], 1)):
                continue
            c.update(flaechen_an_pixeln(Ka, fm[e['id']], F))
        reg['helle Flecken Bild %d (ohne F1, Schwanzspitze)' % n] = region_bericht(oa, F, tex, fl, list(c), 'Flecken Bild %d' % n)
    # Buckel Bild 2: B2-Flecken auf Mesh 10 oberhalb y 215
    c = collections.Counter()
    for e in det['bild2']['flecken']:
        if e['mitte'][1] < 215 and e['mitte'][0] < 335 and e['ort_asset']['teile'] and str(e['ort_asset']['teile'][0][0]).startswith('Mesh 10'):
            c.update(flaechen_an_pixeln(Ka2, fm[e['id']], F))
    reg['Buckel vorn (Bild 2, Mesh 10 an den Flecken)'] = region_bericht(oa, F, tex, fl, list(c), 'Buckel')
    # Lippe
    lc, lrot = lippe_flaechen(oa, F)
    reg['Lippe (Kopf/Kiefer an den Zaehnen, alle)'] = region_bericht(oa, F, tex, fl, list(lc), 'Lippe')
    reg['Lippe, Flaechen mit roten Albedo-Pixeln'] = region_bericht(oa, F, tex, fl, [i for i in lrot if lrot[i] >= 3], 'Lippe rot')
    reg['Lippe, rote Flaechen je Zeile'] = {int(z): c_ for z, c_ in collections.Counter(F[i]['zeile'] for i in lrot if lrot[i] >= 3).items()}
    out['regionen'] = reg
    out['krallen'] = krallen(cache, det)
    lut = v4f.kand(oa).tex.lut
    bud = {}
    for z in range(tex.shape[0]):
        ben = np.nonzero(tex[z])[0]
        bud[z] = dict(flaechen=len({j for e in range(256) for j in fl[z][e]}), meshes=sorted({m for e in ben for m in me[z][e]}),
                      eintraege_benutzt=int(len(ben)), eintraege_frei=int(256 - len(ben)), eintrag0_transparent=bool(lut[z][0][3] == 0))
    out['budget'] = bud
    json.dump(out, open(zw, 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('Regionen fertig', flush=True)
    out['helligkeit'] = helligkeit(cache)
    print('Helligkeit fertig', flush=True)
    out['spielfenster'] = spielfenster()
    print('Spielfenster fertig', flush=True)
    json.dump(out, open(aus, 'w', encoding='utf-8'), indent=1, ensure_ascii=False, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    print('fertig', aus)


if __name__ == '__main__':
    main()
