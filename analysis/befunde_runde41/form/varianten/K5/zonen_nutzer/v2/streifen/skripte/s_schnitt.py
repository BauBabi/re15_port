"""Bild 2 (Spiel-Asset K5e_f1, Clip 12 Bild 0): welche Flaechen bilden die Keile auf dem Buckel, warum, wie tief.
Ausgabe: AUS/schnitt_bild2.json + Konsole."""
import json
from collections import Counter, defaultdict
from s_basis import *  # noqa: F401,F403
import s_geo

KOPF, KIEFER, HALS, BRUST = 22, 23, 21, 10


def name(tag):
    b, fi, q, h = tag
    return '%d %s, %s %d%s' % (b, TEILNAME[b], 'Viereck' if q else 'Dreieck', fi, (' Haelfte %d' % h) if q else '')


def orient_vorzeichen(tris, kam):
    """Vorzeichen s, so dass s * cross(V1-V0, V2-V0) im Sichtraum zur Kamera zeigt (fuer sichtbare = vorn liegende Flaechen)."""
    s = []
    for t in tris:
        V = t[5]
        n = np.cross(V[1] - V[0], V[2] - V[0])
        s.append(np.sign(-(n @ V.mean(0))))
    s = np.array(s)
    return s


def auswertung(ordner, clip=None, bild=None, tag_name='K5e_f1'):
    k, tris, Mg, kam = szene(ordner, 2, clip, bild)
    H, W = grund_rgba(2).shape[:2]
    R = rastern_beide(tris, k.tex, H, W, Mg)
    tm = tri_mesh(tris)
    ma = np.where(R['maler_i'] >= 0, tm[np.maximum(R['maler_i'], 0)], -1)
    mb = np.where(R['puffer_i'] >= 0, tm[np.maximum(R['puffer_i'], 0)], -1)
    erg = {'kandidat': tag_name, 'dreiecke': len(tris)}
    sv = orient_vorzeichen(tris, kam)
    erg['orientierung'] = {'sichtbare_dreiecke': int(len(sv)), 'zur_kamera': int((sv > 0).sum()), 'von_kamera_weg': int((sv < 0).sum())}
    # (1) Keile: Kopf/Kiefer gemalt, Brust/Hals liegt vorn
    keil = np.isin(ma, (KOPF, KIEFER)) & np.isin(mb, (BRUST, HALS))
    gegen = np.isin(ma, (BRUST, HALS)) & np.isin(mb, (KOPF, KIEFER))
    erg['keil_px'] = int(keil.sum()); erg['gegen_px'] = int(gegen.sum())
    je = Counter(R['maler_i'][keil].tolist())
    liste = []
    for i, nn in je.most_common():
        sel = keil & (R['maler_i'] == i)
        vorn = Counter(R['puffer_i'][sel].tolist()).most_common(3)
        dz = (R['maler_z'] - R['puffer_z'])[sel]
        ys, xs = np.nonzero(sel)
        liste.append(dict(flaeche=name(tris[i][4]), px=int(nn), mitteltiefe=round(tris[i][0], 1),
                          ecken_vz=[int(v) for v in tris[i][5][:, 2]],
                          vorn_liegt=[dict(flaeche=name(tris[j][4]), px=int(c), mitteltiefe=round(tris[j][0], 1),
                                           ecken_vz=[int(v) for v in tris[j][5][:, 2]]) for j, c in vorn],
                          tiefe_hinter_vorderflaeche_median=float(np.median(dz)), tiefe_max=float(dz.max()),
                          bbox_grundlage=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())]))
    erg['keil_flaechen'] = liste
    # Paare aller Ordnungsfehler, nach Teilen
    diff = (R['maler_i'] != R['puffer_i']) & (R['maler_i'] >= 0)
    erg['ordnungsfehler_px_gesamt'] = int(diff.sum()); erg['figur_px'] = int((R['maler_i'] >= 0).sum())
    c = Counter(zip(ma[diff].tolist(), mb[diff].tolist()))
    erg['ordnungsfehler_paare'] = [dict(gemalt=TEILNAME[a], vorn=TEILNAME[b], px=int(n)) for (a, b), n in c.most_common(15)]
    # (2) Schnitt im Raum (Sichtraum Bild 2, starr zu Modellraum)
    vt = defaultdict(list)
    for i, t in enumerate(tris):
        vt[t[4][0]].append(i)
    # alle Dreiecke (auch Rueckseiten) im Modellraum
    wt = welt_dreiecke(k.mo, k.kf(*(k.stand(2)[2:] if clip is None else (clip, bild))))
    T = defaultdict(list); TG = defaultdict(list)
    for tag, V in wt:
        T[tag[0]].append(V); TG[tag[0]].append(tag)
    schn = {}
    for a, b in ((KOPF, BRUST), (KOPF, HALS), (KIEFER, HALS), (KIEFER, BRUST), (HALS, BRUST), (KOPF, KIEFER)):
        pr = s_geo.tri_schnitte(np.array(T[a]), np.array(T[b]))
        fl_a = sorted({name(TG[a][i]) for (i, j), _ in pr}); fl_b = sorted({name(TG[b][j]) for (i, j), _ in pr})
        schn['%s x %s' % (TEILNAME[a], TEILNAME[b])] = dict(paare=len(pr), flaechen_a=len(fl_a), flaechen_b=len(fl_b),
                                                            beispiel_a=fl_a[:12], beispiel_b=fl_b[:12])
    erg['schnitt_im_raum'] = schn
    # (3) Ecken von Kopf/Kiefer innerhalb Brust/Hals: Lot in die Flaeche, auf der Rueckseite der naechsten Flaeche
    # Aussen-Normale im Modellraum: Vorzeichen aus der Bild-2-Sicht (sichtbare Flaechen zeigen zur Kamera) und det(V*Y)
    detVY = np.sign(np.linalg.det(np.asarray(kam.V, float)))
    s_aussen = (1 if (sv > 0).sum() > (sv < 0).sum() else -1) * detVY
    erg['aussen_vorzeichen_modellraum'] = float(s_aussen)
    ecken = {}
    for a in (KOPF, KIEFER):
        P = np.unique(np.array(T[a]).reshape(-1, 3), axis=0)
        for b in (BRUST, HALS):
            TB = np.array(T[b]); A_, B_, C_ = TB[:, 0], TB[:, 1], TB[:, 2]
            d2, X, innen = s_geo.naechster_punkt(P, A_, B_, C_)
            j = d2.argmin(1); d = np.sqrt(d2[np.arange(len(P)), j]); Xj = X[np.arange(len(P)), j]
            n = s_aussen * np.cross(B_ - A_, C_ - A_)[j]
            n /= np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9)
            sd = ((P - Xj) * n).sum(1)
            drin = (sd < 0) & innen[np.arange(len(P)), j]
            ecken['%s in %s' % (TEILNAME[a], TEILNAME[b])] = dict(ecken=int(len(P)), innen=int(drin.sum()),
                                                                   tiefe_max=float(d[drin].max()) if drin.any() else 0.0,
                                                                   tiefe_median=float(np.median(d[drin])) if drin.any() else 0.0,
                                                                   ueber_20=int((d[drin] > 20).sum()), ueber_50=int((d[drin] > 50).sum()))
    erg['ecken_innen'] = ecken
    return erg, R, tris


if __name__ == '__main__':
    erg, R, tris = auswertung(ASSET)
    np.savez_compressed(AUS + '/r_K5e_f1_2.npz', **{a: R[a] for a in ('maler_i', 'puffer_i', 'maler_z', 'puffer_z', 'rgb_maler', 'rgb_puffer')},
                        tm=tri_mesh(tris), mitteltiefe=np.array([t[0] for t in tris]))
    json.dump(erg, open(AUS + '/schnitt_bild2.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print(json.dumps({k: v for k, v in erg.items() if k != 'keil_flaechen'}, ensure_ascii=False, indent=1))
    for e in erg['keil_flaechen'][:15]:
        print(e['px'], e['flaeche'], 'Mitteltiefe', e['mitteltiefe'], e['ecken_vz'], '| vorn:', [(v['flaeche'], v['px'], v['mitteltiefe'], v['ecken_vz']) for v in e['vorn_liegt'][:2]],
              '| dahinter median %.0f max %.0f' % (e['tiefe_hinter_vorderflaeche_median'], e['tiefe_max']), e['bbox_grundlage'])
