"""Runde 2 (Nutzer 2026-11-09 "komplette quader ... viel zu ungenau"): Flecken TEXELGENAU statt ganzer Flaechen (T1) und geteilte Texel (T2).

T1  Die Stelle im Nutzerbild (Nutzerumriss, sonst der gemessene Fleck neben dem Linienende; Maske in Grundlage-Koordinaten) wird in der
    ausgerichteten Pose ueber einen Kennungs-Render auf die Modelloberflaeche abgebildet. Der Render rechnet je Bildpunkt baryzentrisch
    (v4f.rastern_id: w0/w1/w2 an der Pixelmitte) die Texturkoordinate u/v und die CLUT-Zeile aus. Abtastung: Ausschnitt um die Stelle,
    F = 5 Abtastpunkte je Grundlage-Pixel und Achse (ungerade, damit die Grundlage-Pixelmitte selbst einer der Abtastpunkte ist); jeder
    Abtastpunkt gehoert zu genau dem Grundlage-Pixel, in dem er liegt (keine Vergroesserung der Stelle).
    Lueckenschluss (einmal, minimal): ein Texel, das NICHT getroffen wurde, kommt nur dazu, wenn alle 4 Nachbartexel (oben/unten/links/rechts)
    getroffen wurden UND das Texel im UV-Bereich einer Traegerflaeche liegt. Kein Glaetten, kein Vergroessern.
    Teilfilter: nur bei Zonen mit eigenem Teilfilter (Augen: Kopf 22; "großer fleischfarbener Teil": Brust 10 und Hals 21, Render OHNE Kopf 22 /
    Unterkiefer 23, Z4). Was dadurch wegfaellt, steht als "verworfen" in der Ausgabe.
T2  UV-Bereich jeder Modellflaeche (Abtastung 4 x 4 je Texel, Texel = floor(u), floor(v) wie im Render). Andere Flaechen, deren UV-Bereich ein
    Fleck-Texel enthaelt, zeigen den Fleck AUCH. Gezaehlt in Bild 1, Bild 2 (Grundlage-Render) und 5 Rundum-Ansichten.
Ausgabe: ARBEIT/texel.json, ARBEIT/texel_masken.npz (je Zone: Texel-Bitmap 256 x 512)."""
import json
import numpy as np, cv2
from g_basis import *

F = 5
TW, TH = 512, 256
ROH = json.load(open(ARBEIT + '/zonen_v2_roh.json', encoding='utf-8'))
ZR = {e['id']: e for e in ROH}
MS = dict(np.load(ARBEIT + '/zonen_v2_masken.npz'))
ka = kand(ASSET)
S2 = {n: s_t(n)[0] ** 2 for n in (1, 2)}
# Zonen, die eine Stelle INNERHALB eines Teils beschreiben (T1); (id, Teilfilter, Render ohne Meshes)
TEXEL_ZONEN = [
    ('wunde_hinten', None, ()), ('musterung_rechts_hinten', None, ()), ('musterung_links_hinten', None, ()),
    ('schwanz_muster', None, ()), ('zipfel_musterung', None, ()), ('rotes_fleisch_zipfel', None, ()),
    ('weisse_musterung_bein_hinten_links', None, ()), ('weisse_musterung_bein_hinten_links_strich2', None, ()),
    ('braeunliche_musterung_bein_hinten_links', None, ()), ('weisse_musterung_bein_hinten_rechts', None, ()),
    ('braeunliche_musterung_bein_hinten_rechts', None, ()), ('weisse_musterung_vorderbein_rechts', None, ()),
    ('fleisch_vorn', (10, 21), (22, 23)), ('augen_vorn', (22,), ()),
]
RUNDUM = [('von vorn', 90, 0), ('von hinten', -90, 0), ('von links (Wesen)', 180, 0), ('von rechts (Wesen)', 0, 0), ('schräg von oben', 45, 40)]
_tris = {}


def tris(n, ohne=()):
    k = (n, tuple(ohne))
    if k not in _tris:
        lage, gier, clip, bild = ka.stand(n)
        Mg, kam = g_matrix(n)
        li, _ = P.licht_fuer(n, ka.licht)
        t = v4f.dreiecke_id(ka.mo, ka.kf(clip, bild), lage, gier, kam, li)
        _tris[k] = [x for x in t if x[5][0] not in ohne]
    return _tris[k]


def render_box(n, box, ohne=()):
    """Kennungs-Render des Grundlage-Ausschnitts box=(x0,y0,x1,y1) mit F Abtastpunkten je Grundlage-Pixel und Achse."""
    x0, y0, x1, y1 = box
    Mg, _ = g_matrix(n)
    S = np.array([[F, 0, -x0 * F], [0, F, -y0 * F], [0, 0, 1]], float)
    ziel = np.zeros(((y1 - y0) * F, (x1 - x0) * F, 3), np.uint8)
    K = v4f.rastern_id(tris(n, ohne), ka.tex, ziel, dict(matrix=(S @ Mg).tolist()))
    K['rgb'] = ziel
    return K


def flaeche_seite():
    """(mesh, flaeche) -> Textur-Seite (uv[5] & 0xF) und CLUT-Zeile relativ."""
    out = {}
    for b, m in enumerate(ka.mo.meshes):
        for vi, uvs in ((0, m.tuv), (1, m.quv)):
            for fi, uv in enumerate(uvs):
                out[(b, vi * 100000 + fi)] = (uv[5] & 0xF, ((uv[2] >> 6) & 0x1FF) - ka.tex.clut_y)
    return out


SEITE = flaeche_seite()


def uv_bereiche():
    """je Flaeche die Texel-Indizes (v*512+u), die eine Abtastung im UV-Dreieck trifft (4 x 4 Proben je Texel)."""
    out = {}
    for b, m in enumerate(ka.mo.meshes):
        for vi, uvs in ((0, m.tuv), (1, m.quv)):
            for fi, uv in enumerate(uvs):
                po = (uv[5] & 0xF) * 128
                if vi:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    dr = [(tx[0], tx[1], tx[3]), (tx[0], tx[3], tx[2])]
                else:
                    dr = [((uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]))]
                s = set()
                for d in dr:
                    p = np.array(d, float); x, y = p[:, 0], p[:, 1]
                    fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
                    u0, u1, v0, v1 = int(x.min()), int(np.ceil(x.max())) + 1, int(y.min()), int(np.ceil(y.max())) + 1
                    PX, PY = np.meshgrid(np.arange(u0 * 4, u1 * 4) / 4 + 0.125, np.arange(v0 * 4, v1 * 4) / 4 + 0.125)
                    if abs(fl) < 1e-9:  # entartet: nur die Ecken
                        s |= {int(yy) * TW + int(xx) for xx, yy in p}
                        continue
                    w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
                    w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
                    inn = (w1 >= 0) & (w2 >= 0) & (1 - w1 - w2 >= 0)
                    s |= set((np.clip(PY[inn].astype(int), 0, TH - 1) * TW + np.clip(PX[inn].astype(int), 0, TW - 1)).tolist())
                out[(b, vi * 100000 + fi)] = np.array(sorted(s), np.int64)
    return out


UVB = uv_bereiche()
UVS = {f: set(a.tolist()) for f, a in UVB.items()}


def fl_txt(k):
    return fl_schluessel(k[0], k[1])


def texel_zone(zid, teile, ohne, mg=None):
    z = ZR[zid]; n = z['bild']; mg = MS[zid] if mg is None else mg
    H, W = mg.shape
    ys, xs = np.nonzero(mg)
    RAND = 12  # Grundlage-px um die Stelle, damit jedes randnahe Texel mit ALLEN seinen Abtastpunkten gezaehlt wird
    box = (max(int(xs.min()) - RAND, 0), max(int(ys.min()) - RAND, 0), min(int(xs.max()) + RAND + 1, W), min(int(ys.max()) + RAND + 1, H))
    K = render_box(n, box, ohne)
    x0, y0, x1, y1 = box
    Mb = np.repeat(np.repeat(mg[y0:y1, x0:x1], F, 0), F, 1)
    modell = K['maske'] & (np.isin(K['mesh'], teile) if teile is not None else True)
    auf = Mb & K['maske']
    ok = auf & modell
    verw = auf & ~ok
    # Mehrheitsregel je (Flaeche, Texel): das Texel gehoert zur Stelle, wenn mindestens die Haelfte der Abtastpunkte, mit denen DIESE Flaeche
    # das Texel im Ausschnitt zeigt, in der Stelle liegt (kein Vergroessern durch angeschnittene Texel am Rand)
    ti_all = (K['v'][modell].astype(np.int64) * TW + K['u'][modell]).astype(np.int64)
    ky_all = K['mesh'][modell].astype(np.int64) * 1000000 + K['flaeche'][modell]
    in_all = Mb[modell]
    zl_all = K['zeile'][modell]
    code = ky_all * (TH * TW) + ti_all
    u_c, inv = np.unique(code, return_inverse=True)
    n_alle = np.bincount(inv); n_in = np.bincount(inv, weights=in_all.astype(float))
    gilt = (n_in / n_alle) >= 0.5
    sel = gilt[inv] & in_all
    idx = ti_all[sel]; key = ky_all[sel]; zl = zl_all[sel]
    angeschnitten = int(len(np.unique(ti_all[in_all & ~gilt[inv]])) - len(np.intersect1d(np.unique(ti_all[in_all & ~gilt[inv]]), np.unique(idx))))
    getroffen = np.zeros(TH * TW, bool); getroffen[idx] = True
    traeger = {}
    for kk, i in zip(key.tolist(), idx.tolist()):
        traeger.setdefault((kk // 1000000, kk % 1000000), set()).add(i)
    # Lueckenschluss (einmal): alle 4 Nachbarn getroffen und im UV-Bereich einer Traegerflaeche
    G = getroffen.reshape(TH, TW)
    nb = np.zeros_like(G)
    nb[1:-1, 1:-1] = G[:-2, 1:-1] & G[2:, 1:-1] & G[1:-1, :-2] & G[1:-1, 2:]
    kandidat = nb & ~G
    imtr = np.zeros(TH * TW, bool)
    for f in traeger:
        imtr[UVB[f]] = True
    zu = kandidat.reshape(-1) & imtr
    T = getroffen | zu
    # je Texel CLUT-Zeile und Seite (von der Traegerflaeche)
    zeile_von = {}
    for i, z_ in zip(idx.tolist(), zl.tolist()):
        zeile_von.setdefault(i, z_)
    zu_idx = np.nonzero(zu)[0]
    for i in zu_idx.tolist():
        for f in traeger:
            if i in UVS[f]:
                zeile_von[i] = SEITE[f][1]; break
    seite_von = {}
    for f, s in traeger.items():
        for i in s:
            seite_von.setdefault(i, SEITE[f][0])
    for i in zu_idx.tolist():
        for f in traeger:
            if i in UVS[f]:
                seite_von.setdefault(i, SEITE[f][0]); break
    tl = [[int(seite_von.get(i, -1)), int(zeile_von.get(i, -1)), int(i % TW), int(i // TW)] for i in np.nonzero(T)[0].tolist()]
    # Flaeche in Video-px: Grundlage-Pixel der Stelle mit sichtbarem Modell (Abtastung Mitte = Index F//2)
    mitte = np.zeros_like(ok); mitte[F // 2::F, F // 2::F] = True
    px_g = int((ok & mitte).sum())
    vm = {}
    for b in np.unique(K['mesh'][verw]):
        vm[str(int(b))] = dict(teil=TEILNAME[int(b)], px_vorlage=round(float(((K['mesh'] == b) & verw & mitte).sum() * S2[n])))
    erg = dict(bild=n, ausschnitt_grundlage=list(box), abtastung_je_grundlagepixel=F, render_ohne_meshes=list(ohne), teilfilter=list(teile) if teile else None,
               stelle_px_vorlage=round(float(mg.sum() * S2[n])), stelle_auf_modell_px_vorlage=round(px_g * S2[n]),
               verworfen_durch_teilfilter=vm, texel_zahl=int(T.sum()), texel_getroffen=int(G.sum()), texel_lueckenschluss=int(zu.sum()), texel_angeschnitten_verworfen=angeschnitten,
               regel_mehrheit='je (Flaeche, Texel): >= 50 % der Abtastpunkte dieser Flaeche auf dem Texel liegen in der Stelle',
               traegerflaechen={fl_txt(f): len(s) for f, s in sorted(traeger.items(), key=lambda t: -len(t[1]))},
               texel=tl)
    return erg, T.reshape(TH, TW), set(traeger)


def texelbild(K):
    return np.where(K['maske'], K['v'].astype(np.int64) * TW + K['u'], -1)


def teilung(T, traeger, R1, R2, RU):
    """T2: andere Flaechen mit Texeln des Flecks; px in Bild 1/2 und Rundum, wo der Fleck dadurch zusaetzlich erscheinen wuerde."""
    Ti = np.nonzero(T.reshape(-1))[0]
    Tset = set(Ti.tolist())
    andere = {}
    for f, a in UVB.items():
        if f in traeger or len(a) == 0:
            continue
        c = int(np.isin(a, Ti).sum())
        if c:
            andere[f] = c
    zeilen_tr = {SEITE[f][1] for f in traeger}

    def zaehle(K, s2=1.0):
        ti = texelbild(K); key = K['mesh'].astype(np.int64) * 1000000 + K['flaeche']
        auch = K['maske'] & np.isin(ti, Ti)
        tr = np.isin(key, [f[0] * 1000000 + f[1] for f in traeger])
        fremd = auch & ~tr
        je = {}
        for kk in np.unique(key[fremd]):
            je[fl_schluessel(int(kk // 1000000), int(kk % 1000000))] = round(float((fremd & (key == kk)).sum() * s2), 1)
        return round(float(fremd.sum() * s2), 1), je, fremd
    b1, j1, _ = zaehle(R1, S2[1]); b2, j2, _ = zaehle(R2, S2[2])
    ru = {}
    for name, K in RU.items():
        p, j, _ = zaehle(K)
        ru[name] = p
    je_mesh = {}
    for f, c in andere.items():
        je_mesh.setdefault(str(f[0]), dict(teil=TEILNAME[f[0]], flaechen=0, geteilte_texel=0, gleiche_clut_zeile=0))
        je_mesh[str(f[0])]['flaechen'] += 1; je_mesh[str(f[0])]['geteilte_texel'] += c
        if SEITE[f][1] in zeilen_tr:
            je_mesh[str(f[0])]['gleiche_clut_zeile'] += 1
    return dict(andere_flaechen=len(andere), andere_flaechen_liste={fl_txt(f): dict(texel=c, clut_zeile=SEITE[f][1]) for f, c in sorted(andere.items(), key=lambda t: -t[1])},
                je_mesh=je_mesh, bild1_px_vorlage=b1, bild1_je_flaeche=j1, bild2_px_vorlage=b2, bild2_je_flaeche=j2, rundum_px=ru,
                brauchen_eigene_texel=sorted(fl_txt(f) for f in andere))


def main():
    R1 = dict(np.load(ARBEIT + '/render_asset_1.npz')); R2 = dict(np.load(ARBEIT + '/render_asset_2.npz'))
    R2o = dict(np.load(ARBEIT + '/render_asset_ohnekopf_2.npz'))
    RU = {}
    for name, az, el in RUNDUM:
        _, K = v4f.rundum_bild(ASSET, 2, 0, az, el, 760, grund=255)
        RU[name] = K
    np.savez_compressed(ARBEIT + '/rundum_kennung.npz', **{'%d_%s' % (i, k): RU[name][k] for i, (name, _, _) in enumerate(RUNDUM)
                                                           for k in ('maske', 'mesh', 'flaeche', 'u', 'v')})
    out = {}; bits = {}
    for zid, teile, ohne in TEXEL_ZONEN:
        e, T, tr = texel_zone(zid, teile, ohne)
        e['geteilte_texel'] = teilung(T, tr, R1, R2, RU)
        # Rueckprobe: Texel-Maske auf den Traegerflaechen im Grundlage-Render gegen die gemessene Stelle (IoU)
        n = e['bild']; RR = R2o if ohne else (R1 if n == 1 else R2)
        ti = texelbild(RR); key = RR['mesh'].astype(np.int64) * 1000000 + RR['flaeche']
        rueck = RR['maske'] & np.isin(ti, np.nonzero(T.reshape(-1))[0]) & np.isin(key, [f[0] * 1000000 + f[1] for f in tr])
        soll = MS[zid] & RR['maske'] & (np.isin(RR['mesh'], teile) if teile else True)
        e['rueckprobe'] = dict(regel='Grundlage-Render: Pixel der Traegerflaechen, deren Texel in der Texel-Maske liegt, gegen Stelle auf dem Modell',
                               iou=round(float((rueck & soll).sum() / max((rueck | soll).sum(), 1)), 3),
                               rueck_px_vorlage=round(float(rueck.sum() * S2[n])), soll_px_vorlage=round(float(soll.sum() * S2[n])),
                               rueck_ausserhalb_stelle_px_vorlage=round(float((rueck & ~cv2.dilate(MS[zid].astype(np.uint8), np.ones((3, 3), np.uint8)).astype(bool)).sum() * S2[n])))
        print('   Rueckprobe', e['rueckprobe'])
        out[zid] = e; bits[zid] = T
        g = e['geteilte_texel']
        print('%-44s texel %5d (getroffen %5d, Luecke %3d) Traeger %3d Stelle %5d auf Modell %5d verworfen %s | andere Fl %3d B1 %6.1f B2 %6.1f rundum %s' % (
            zid, e['texel_zahl'], e['texel_getroffen'], e['texel_lueckenschluss'], len(e['traegerflaechen']), e['stelle_px_vorlage'], e['stelle_auf_modell_px_vorlage'],
            {k: v['px_vorlage'] for k, v in e['verworfen_durch_teilfilter'].items()}, g['andere_flaechen'], g['bild1_px_vorlage'], g['bild2_px_vorlage'],
            {k: int(v) for k, v in g['rundum_px'].items()}))
    meta = dict(regel=__doc__.strip(), texel_format='[Seite, CLUT-Zeile, u, v]; u = Spalte in der TIM-Textur (512 x 256, 8 Bit) einschliesslich Seitenversatz 128 x Seite, '
                                                    'v = Zeile; CLUT-Zeile relativ zur ersten CLUT-Zeile der TIM')
    json.dump(dict(meta=meta, zonen=out), open(ARBEIT + '/texel.json', 'w', encoding='utf-8'), ensure_ascii=False)
    np.savez_compressed(ARBEIT + '/texel_masken.npz', **bits)


if __name__ == '__main__':
    main()

