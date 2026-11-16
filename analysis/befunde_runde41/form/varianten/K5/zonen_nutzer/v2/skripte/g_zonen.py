"""Grundlage v2 (Korrektor Runde 1): Zonen bauen, auf Modellflaechen abbilden, zonen_v2.json schreiben.
Alle Masken in Grundlage-Koordinaten; Pixelzahlen zusaetzlich im Vorlage-Massstab (x s^2)."""
import json, sys
import numpy as np, cv2
from scipy import ndimage as ndi
from g_basis import *
import g_bau
from g_fleck import fleck, wachsen, richtung
from g_spec import ZONEN, MODELLZONEN_KOPF

ka = kand(ASSET); kc = kand(K5C)
REND = {n: lade_render('asset', n, lambda n=n: render_g(ka, n))[1] for n in (1, 2)}
REND_K5C = {n: lade_render('k5c', n, lambda n=n: render_g(kc, n))[1] for n in (1, 2)}
REND_OK = lade_render('asset_ohnekopf', 2, lambda: render_g(ka, 2, meshes_ohne=(22, 23)))[1]
S2 = {n: s_t(n)[0] ** 2 for n in (1, 2)}
ML = material_listen()
ALLE = alle_flaechen(ka.mo)


def bild_von(lf):
    return 1 if VS[lf]['grund'].startswith('R') else 2


def nach_g(m_l, lf):
    n = bild_von(lf); dx = VS[lf]['dx']; h, w = grund_rgba(n).shape[:2]
    return m_l[:h, dx:dx + w].copy()


def pose_welt(mo, kf):
    R, T = mo.pose(kf)
    out = {}
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        for vi, (verts, faces) in enumerate(((m.tv, m.tf), (m.qv, m.qf))):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], float)
            w = (R[b].astype(float) @ v.T).T / 4096.0 + T[b].astype(float)
            out[(b, vi)] = (w, faces, v)
    return out


def modell_mengen():
    """Flaechenmengen nach Modellbedeutung (Schluessel 'mesh|viereck|index')."""
    M = {k: set(v) for k, v in ML.items()}
    alle23 = {f for f in ALLE if f.startswith('23|')}
    # Runde 3 (Pruefer r2 Nr. 11): die material.json-Liste 'Maulinneres' ist eine FARBliste (Texturzeile 0 rot), keine Lageliste.
    # Maulinneres nach LAGE aus g_maul.py (Flaechen, die erst durch das Oeffnen des Mauls sichtbar werden; Zaehne ausgenommen).
    ML_LAGE = json.load(open(ARBEIT + '/maul_lage.json', encoding='utf-8'))
    M['Maulinneres_lage'] = set(ML_LAGE['maulinneres_lage'])
    # Augenhoehle wie Runde 2: Flaechen der Farbliste in Augenhoehe (Flaechenmitte Knochen-y < -200, Kopf 22)
    au = set()
    for f in M['Maulinneres']:
        b, vi, i = map(int, f.split('|')); m = ka.mo.meshes[b]
        verts, faces = (m.tv, m.tf) if vi == 0 else (m.qv, m.qf)
        y = np.mean([verts[k][1] for k in faces[i][1::2]])
        if b == 22 and y < -200:
            au.add(f)
    M['Augenhoehle'] = au
    # Rest der Farbliste: Kopf 22 -> Zone 45 (aussen, Haut nicht belegt); Unterkiefer 23 -> Unterkiefer aussen (Zone 31)
    M['Maulfarbe_aussen'] = {f for f in M['Maulinneres'] if f.startswith('22|')} - au - M['Maulinneres_lage']
    M['Unterkiefer_aussen'] = alle23 - M['Maulinneres_lage'] - M['Zahn']  # mit Unterlippe und den 23-Flaechen der Farbliste
    # Runde 4 (Pruefer r3 Nr. 5): Oberlippe nach LAGE (g_maul.py: Rand der Maul-Oeffnung am Kopf 22, auch bei geschlossenem Maul sichtbar),
    # nicht mehr aus der Farbliste 'Lippensaum' (die enthielt Gaumenflaechen, z. B. 22|1|40 und 49)
    M['Lippe_oben'] = set(ML_LAGE['oberlippe_lage']) - M['Maulinneres_lage'] - M['Zahn'] - au
    # Hals-Unterseite: Flaechennormale (Pose Bild 2, nach aussen orientiert) zeigt nach unten (PSX +y), n_y >= 0,5
    lage, gier, clip, bild = ka.stand(2)
    W = pose_welt(ka.mo, ka.kf(clip, bild))
    unten = set(); zehen = {}
    pts21 = np.vstack([W[k][0] for k in W if k[0] == 21]); c21 = pts21.mean(0)
    for (b, vi), (w, faces, v) in W.items():
        if b == 21:
            for fi, g in enumerate(faces):
                ie = list(g[1::2])
                p = w[ie]; n_ = np.cross(p[1] - p[0], p[2] - p[0]); n_ /= (np.linalg.norm(n_) + 1e-9)
                if np.dot(p.mean(0) - c21, n_) < 0:
                    n_ = -n_
                if n_[1] >= 0.5:
                    unten.add('%d|%d|%d' % (b, vi, fi))
        if b in (25, 27):
            xmax = max(float(np.vstack([W[(b, j)][2] for j in (0, 1) if (b, j) in W])[:, 0].max()), 1)
            for fi, g in enumerate(faces):
                ie = list(g[1::2])
                if v[ie][:, 0].mean() >= 0.75 * xmax:
                    zehen.setdefault(b, set()).add('%d|%d|%d' % (b, vi, fi))
    M['Hals_unten'] = unten
    M['Zehen25'] = zehen.get(25, set()); M['Zehen27'] = zehen.get(27, set())
    return M


MM = modell_mengen()


def flaechen_px(K, maske=None):
    ok = K['maske'] if maske is None else (K['maske'] & maske)
    m = K['mesh'][ok]; f = K['flaeche'][ok]
    key = m.astype(np.int64) * 1000000 + f
    u, c = np.unique(key, return_counts=True)
    return {fl_schluessel(int(k // 1000000), int(k % 1000000)): int(n) for k, n in zip(u, c)}


TOT = {n: flaechen_px(REND[n]) for n in (1, 2)}
TOT_K5C = {n: flaechen_px(REND_K5C[n]) for n in (1, 2)}


def maske_von_flaechen(K, menge):
    m = K['mesh']; f = K['flaeche']
    key = np.where(K['maske'], m.astype(np.int64) * 1000000 + f, -1)
    ks = np.array([int(s.split('|')[0]) * 1000000 + int(s.split('|')[1]) * 100000 + int(s.split('|')[2]) for s in menge], np.int64)
    return np.isin(key, ks) & K['maske']


def baue(z):
    lf = z['datei']; art, kw = z['bau']; kw = dict(kw); info = {}
    n = bild_von(lf); dx = VS[lf]['dx']
    if art == 'flut':
        m, info = g_bau.flut(lf, **kw)
    elif art == 'huelle':
        m, info = g_bau.huelle(lf, kw['farbe'], kw['box'])
    elif art == 'huelle_komp':
        # Runde 3 (Pruefer r2 Nr. 3): je zusammenhaengender Linienzug der Farbe (>= min_px) eine EIGENE konvexe Huelle (Strich + Inneres);
        # Zeiger-Reste unter min_px fallen weg. Keine gemeinsame Huelle, damit der Spalt zwischen zwei Umrissen NICHT dazukommt.
        M_, fig_, (H_, W_) = g_bau.lade(lf); x0, y0, x1, y1 = kw['box']
        s_ = np.zeros((H_, W_), bool); s_[y0:y1 + 1, x0:x1 + 1] = M_[kw['farbe']][y0:y1 + 1, x0:x1 + 1]
        lb, nl = ndi.label(s_, np.ones((3, 3)))
        m = np.zeros((H_, W_), bool); info = dict(umrisse=[])
        for i in range(1, nl + 1):
            q = lb == i
            if q.sum() < kw['min_px']:
                info.setdefault('verworfen_px', []).append(int(q.sum())); continue
            r = np.zeros((H_, W_), np.uint8); cv2.fillConvexPoly(r, cv2.convexHull(np.argwhere(q)[:, ::-1].astype(np.int32)), 1)
            ys, xs = np.nonzero(q)
            info['umrisse'].append(dict(strich_px=int(q.sum()), huelle_px=int(r.sum()), bbox=[int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())]))
            m |= r > 0
        info['vorher_gemeinsame_huellen_px'] = None
    elif art == 'huelle_n':
        m = None
        for bx in kw['teile']:
            q, _ = g_bau.huelle(lf, kw['farbe'], bx); m = q if m is None else m | q
    elif art == 'loecher':
        m, info = g_bau.loecher(lf, kw['farbe'], kw['box'])
    elif art == 'flut_band':
        a, info = g_bau.flut(lf, kw['waende'], kw['saaten'], box=kw['box'])
        c, _ = g_bau.band(lf, kw['band_farbe'], kw['band_box'], kw['band_r']); m = a & c
    elif art in ('fleck', 'fleck_n', 'wachsen', 'wachsen_n'):
        enden = kw.get('enden') or [kw['ende']]
        h, w = grund_rgba(n).shape[:2]; mg = np.zeros((h, w), bool); info = dict(stellen=[])
        for ung in enden:
            e = g_bau.endpunkt(lf, z['farbe'], ung, r=8)
            if art.startswith('fleck'):
                q, inf = fleck(n, (e[0] - dx, e[1]), kw['klasse'])
            else:
                rv = richtung(lf, z['farbe'], e)
                q, inf = wachsen(n, (e[0] - dx, e[1]), rv, tol=kw.get('tol', 14), r_max=kw.get('r_max', 22), vorlauf=kw.get('vorlauf', 4))
                inf = dict(inf or {}, richtung=[round(float(rv[0]), 2), round(float(rv[1]), 2)])
            info['stellen'].append(dict(linienende_beschriftet=e, linienende_grundlage=[e[0] - dx, e[1]], stelle=inf))
            mg |= q
        return mg, info
    elif art == 'zehen':
        menge = MM['Zehen%d' % kw['mesh']]
        mg = maske_von_flaechen(REND[n], menge)
        info = dict(regel='Flaechen von Fuss %d mit Flaechenmitte (Knochen-x) >= 75 %% der Fusslaenge nach vorn' % kw['mesh'],
                    flaechen=len(menge))
        if kw.get('marken_box'):
            mk, _ = g_bau.band(lf, 'rot', kw['marken_box'], 1, nur_figur=False)
            info['nutzermarken_px_grundlage'] = int(nach_g(mk, lf).sum())
            info['nutzermarken_maske'] = nach_g(mk, lf)
        return mg, info
    else:
        raise ValueError(art)
    return nach_g(m, lf), info


def abschnitte(z, m_g):
    lf = z['datei']; M, fig, _ = g_bau.lade(lf)
    w = nach_g(cv2.dilate(M[z['abschnitte']].astype(np.uint8), np.ones((5, 5), np.uint8)) > 0, lf)
    lab, nl = ndi.label(m_g & ~w)
    teile = [lab == i for i in range(1, nl + 1) if (lab == i).sum() >= 600]
    teile.sort(key=lambda q: np.nonzero(q)[0].mean())
    return [cv2.dilate(q.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & m_g for q in teile]


def schleifen(z):
    """je geschlossene Schleife (loecher) eine Maske (Grundlage)."""
    lf = z['datei']; art, kw = z['bau']; M, fig, (H, W) = g_bau.lade(lf); x0, y0, x1, y1 = kw['box']
    s = np.zeros((H, W), bool); s[y0:y1 + 1, x0:x1 + 1] = M[kw['farbe']][y0:y1 + 1, x0:x1 + 1]
    s = cv2.dilate(s.astype(np.uint8), np.ones((2, 2), np.uint8)) > 0
    inn = ndi.binary_fill_holes(s) & ~s
    lab, nl = ndi.label(inn); out = []
    for i in range(1, nl + 1):
        q = lab == i
        if q.sum() >= 15:
            out.append(nach_g(cv2.dilate(q.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0, lf))
    return out


def auswerten(n, mg, teile, K=None, tot=None, menge=None, stelle=False):
    """Flaechen in der Zone; Zuordnung: Teil-Filter (nur Meshes des benannten Teils), Flaeche >= 50 % in der Zone oder >= 15 % der Zonen-Modellpixel."""
    K = K or REND[n]; tot = tot or TOT[n]
    inz = flaechen_px(K, mg)
    modellpx = sum(inz.values())
    je_mesh = {}
    for f, c in inz.items():
        b = int(f.split('|')[0]); je_mesh[b] = je_mesh.get(b, 0) + c
    if menge is None and stelle:
        # Stellen (Fleck/Farbgebiet): Kern der Stelle (1 px abgetragen, gegen Randversatz), Flaechen mit >= 10 % des Kerns
        lbm, nlm = ndi.label(mg); zu = set()
        for i in range(1, nlm + 1):  # je Stelle (Zusammenhangskomponente) einzeln
            kern = cv2.erode((lbm == i).astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
            kz = flaechen_px(K, kern); ks = max(sum(kz.values()), 1)
            zu |= {f for f, c in kz.items() if (teile is None or int(f.split('|')[0]) in teile) and c >= 0.10 * ks}
        zu = sorted(zu)
    elif menge is None:
        zu = sorted(f for f, c in inz.items() if (teile is None or int(f.split('|')[0]) in teile)
                    and (c / tot[f] >= 0.5 or c >= 0.15 * max(modellpx, 1)))
    else:
        zu = sorted(menge)
    mz = maske_von_flaechen(K, zu) if zu else np.zeros_like(mg)
    iou = float((mz & mg).sum() / max((mz | mg).sum(), 1))
    fremd = {b: c for b, c in je_mesh.items() if teile is not None and b not in teile}
    s2 = S2[n]
    return dict(zone_px_vorlage=round(float(mg.sum() * s2)), modell_px_in_zone_vorlage=round(modellpx * s2),
                deckung_modell=round(modellpx / max(int(mg.sum()), 1), 3),
                meshes_in_zone={str(b): dict(teil=TEILNAME[b], px_vorlage=round(c * s2)) for b, c in sorted(je_mesh.items(), key=lambda t: -t[1])},
                fremd_im_umriss={str(b): dict(teil=TEILNAME[b], px_vorlage=round(c * s2)) for b, c in sorted(fremd.items(), key=lambda t: -t[1])},
                zugeordnet=dict(flaechen=len(zu), sichtbar=len([f for f in zu if f in tot]),
                                px_sichtbar_vorlage=round(float(mz.sum() * s2)),
                                je_mesh={str(b): len([f for f in zu if int(f.split('|')[0]) == b]) for b in sorted({int(f.split('|')[0]) for f in zu})},
                                liste=zu),
                iou_zu_umrandung=round(iou, 3),
                anteil_zone_von_flaechen_gedeckt=round(float((mz & mg).sum() / max(int(mg.sum()), 1)), 3),
                anteil_flaechenpixel_in_zone=round(float((mz & mg).sum() / max(int(mz.sum()), 1)), 3)), mz


def main():
    out = []; masken = {}
    for z in ZONEN:
        n = bild_von(z['datei'])
        mg, info = baue(z)
        nm = info.pop('nutzermarken_maske', None) if isinstance(info, dict) else None
        e = dict(nr=z['nr'], id=z['id'], bild=n, nutzerbild=z['datei'], nutzerwort=z['wort'], linienfarbe=z['farbe'], bau=z['bau'][0],
                 bau_parameter=json.loads(json.dumps(z['bau'][1])), material=z['material'], eltern=z.get('eltern'), auftrag=z.get('z'),
                 bau_info={k: v for k, v in (info or {}).items() if k != 'teile'} if isinstance(info, dict) else {})
        teile = z.get('teile')
        menge = None
        if z['bau'][0] == 'zehen':
            menge = MM['Zehen%d' % z['bau'][1]['mesh']]
        if z.get('modell'):
            menge = set().union(*[MM[k] for k in z['modell']])
        if z['id'] in ('gesicht_vorn', 'kopf_hinten'):
            menge = {f for f in ALLE if int(f.split('|')[0]) in (22, 23)}
        if z['id'] in ('auge_hinten', 'mund_hinten'):
            menge = set()
        if z['id'] == 'rotes_fleisch_zipfel':
            # Runde 3 (Pruefer r2 Nr. 3): Ihr oranger Umriss (Zone 11) hat Vorrang vor dem gemessenen Farbgebiet; Ueberlappung abziehen
            ue = mg & masken['zipfel_musterung']
            e['bau_info']['abgezogen_wegen_zone11_px_grundlage'] = int(ue.sum())
            mg = mg & ~masken['zipfel_musterung']
        ana, mz = auswerten(n, mg, teile, menge=menge, stelle=z['bau'][0] in ('fleck', 'fleck_n', 'wachsen', 'wachsen_n'))
        if z['id'] in ('gesicht_vorn', 'maul_vorn', 'unterkiefer_vorn', 'kopf_hinten', 'auge_hinten', 'mund_hinten', 'fusskrallen_hinten_links',
                       'fusskrallen_hinten_rechts'):
            ana['nach_bildlage'] = auswerten(n, mg, teile)[0]['zugeordnet']
            ana['nach_bildlage'].pop('liste')
        if z.get('ohne_kopf'):
            # Z4: Kopf 22 / Kiefer 23 nie in diese Zone; dafuer die Flaechen von 10/21, die nur der zu hohe Modellkopf verdeckt
            a2, mz2 = auswerten(2, mg, teile, K=REND_OK, tot=flaechen_px(REND_OK))
            hinter = sorted(set(a2['zugeordnet']['liste']) - set(ana['zugeordnet']['liste']))
            ana['hinter_dem_modellkopf'] = dict(regel='Render ohne Kopf 22/Kiefer 23, gleiche Zuordnungsregel; nur Flaechen, die im Normal-Render fehlen',
                                                flaechen=len(hinter), liste=hinter, px_vorlage=round(float(maske_von_flaechen(REND_OK, hinter).sum() * S2[2])))
            ana['kopf_im_umriss_px_vorlage'] = {str(b): round(float(((REND[2]['mesh'] == b) & mg).sum() * S2[2])) for b in (22, 23)}
        if nm is not None:
            ana['nutzermarken_auf_modell'] = dict(px_grundlage=int(nm.sum()), davon_auf_modell=int((nm & REND[n]['maske']).sum()),
                                                  meshes={str(b): int(((REND[n]['mesh'] == b) & nm).sum()) for b in np.unique(REND[n]['mesh'][nm & REND[n]['maske']])})
        if z.get('namensteile'):
            ns = [b for b in z['namensteile']]
            alle_ns = [f for f in ALLE if int(f.split('|')[0]) in ns]
            sicht = set(TOT[n])
            ana['ganzteil'] = dict(meshes=ns, teile=[TEILNAME[b] for b in ns], flaechen_gesamt=len(alle_ns),
                                   davon_in_diesem_bild_sichtbar=len([f for f in alle_ns if f in sicht]),
                                   davon_zugeordnet_sichtbar=len([f for f in ana['zugeordnet']['liste'] if int(f.split('|')[0]) in ns]),
                                   rest_material='Haut (nicht belegt)')
        e['modell'] = ana
        out.append(e); masken[z['id']] = mg; masken[z['id'] + '__modell'] = mz
        print('%-3s %-46s B%d zone %6d modell %6d IoU %.3f zu %3d fremd %s' % (z['nr'], z['id'], n, ana['zone_px_vorlage'], ana['modell_px_in_zone_vorlage'],
              ana['iou_zu_umrandung'], ana['zugeordnet']['flaechen'], {k: v['px_vorlage'] for k, v in ana['fremd_im_umriss'].items()}))
        # Abschnitte
        if z.get('abschnitte'):
            for i, q in enumerate(abschnitte(z, mg)):
                a, mq = auswerten(n, q, teile)
                zid = '%s_abschnitt%d' % (z['id'], i + 1)
                out.append(dict(nr='%s.%d' % (z['nr'], i + 1), id=zid, bild=n, nutzerbild=z['datei'], nutzerwort=z['abschnitt_wort'] + ' (Abschnitt %d von oben)' % (i + 1),
                                linienfarbe=z['abschnitte'], bau='abschnitt', material='Gliederung (kein Material)', eltern=z['id'], modell=a))
                masken[zid] = q; masken[zid + '__modell'] = mq
                print('   %-46s zone %6d IoU %.3f meshes %s' % (zid, a['zone_px_vorlage'], a['iou_zu_umrandung'], {k: v['px_vorlage'] for k, v in a['meshes_in_zone'].items()}))
        if z['id'] in ('krallen_vorn_links',):
            for i, q in enumerate(sorted(schleifen(z), key=lambda q: np.nonzero(q)[1].mean())):
                zid = 'kralle_vorn_links_schleife%d' % (i + 1)
                aa, mq = auswerten(n, q, None); ak, _ = auswerten(n, q, None, K=REND_K5C[2], tot=TOT_K5C[2])
                out.append(dict(nr='39.%d' % (i + 1), id=zid, bild=n, nutzerbild=z['datei'], nutzerwort='Exakt 4 Krallen (Schleife %d von links im Bild)' % (i + 1),
                                linienfarbe='hellblau', bau='schleife', material='Kralle', eltern=z['id'],
                                modell=dict(asset=aa, k5c=ak)))
                masken[zid] = q
                print('   %-46s asset %s k5c %s' % (zid, {k: v['px_vorlage'] for k, v in aa['meshes_in_zone'].items()}, {k: v['px_vorlage'] for k, v in ak['meshes_in_zone'].items()}))
    # Modellzonen Kopf (Z4)
    for nr, zid, wort, mat, key, eltern in MODELLZONEN_KOPF:
        menge = MM[key]
        mg = maske_von_flaechen(REND[2], menge)
        ana, mz = auswerten(2, mg, None, menge=menge)
        if eltern:
            ana['in_nutzerzone_px_vorlage'] = round(float((mz & masken[eltern]).sum() * S2[2]))
            ana['iou_zur_nutzerzone'] = round(float((mz & masken[eltern]).sum() / max((mz | masken[eltern]).sum(), 1)), 3)
        out.append(dict(nr=nr, id=zid, bild=2, nutzerbild='Gesicht vorne.png', nutzerwort=None, modell_beschreibung=wort, linienfarbe=None,
                        bau='modellbedeutung', material=mat, eltern=eltern, modell=ana, modell_quelle=key))
        masken[zid] = mg; masken[zid + '__modell'] = mz
        print('%-3s %-46s flaechen %d sichtbar %d px %d' % (nr, zid, len(menge), ana['zugeordnet']['sichtbar'], ana['zugeordnet']['px_sichtbar_vorlage']),
              'in Nutzerzone', ana.get('in_nutzerzone_px_vorlage'), 'IoU', ana.get('iou_zur_nutzerzone'))
    np.savez_compressed(ARBEIT + '/zonen_v2_masken.npz', **masken)
    json.dump(out, open(ARBEIT + '/zonen_v2_roh.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    json.dump({k: sorted(v) for k, v in MM.items() if k in ('Unterkiefer_aussen', 'Hals_unten', 'Zehen25', 'Zehen27', 'Augenhoehle', 'Maulinneres_lage', 'Maulfarbe_aussen', 'Lippe_oben')},
              open(ARBEIT + '/modellmengen_v2.json', 'w', encoding='utf-8'), indent=0)


if __name__ == '__main__':
    main()
