"""Nachbesserung 1: Textur der Gelenkkoerper von K3 (Nutzerbefund nach dem Einbau: "Texturen vollkommen kaputt", grau-weisse,
karierte Bloecke an Hals, Schulter, Gelenken, Kopf).

    python s3_n1_textur.py pruefen [EMD TIM]        Kennzahlen: Seiten/CLUT-Zeilen je Flaeche, Gelenkkoerper-Flaechen, helle Fremdtexel
    python s3_n1_textur.py bauen AUS_DIR            K3_fit mit neuer UV der Gelenkkoerper -> AUS_DIR/FORM.EMD/.TIM/form.json

Ursache (gemessen, s. nachbesserung1_asset.md): kein Seiten- oder CLUT-Fehler im Port. Port (render_pc.c: Texel-x = u + (page & 0xF) * 128,
Texel-y ueber die CLUT-Zeile) und Offline-Buehne (form_buehne.dreiecke/rastern, dieselbe Regel) zeigen dieselben Flecken. Die 31
Gelenkkoerper von K3 (k_naht.kugel) uebernehmen fuer JEDE ihrer 40 Flaechen die UV-Ecken der naechsten Flaeche des Eltern-/Kindmeshes,
unveraendert und in beliebiger Drehung: aus einem Hautstueck mit weissem Fleck oder Kante werden so 40 aneinandergesetzte, gedrehte
Kopien - ein Schachbrett aus Fleckstuecken. Neu: jeder Gelenkkoerper nimmt EINEN Hauttexel seiner Umgebung (Median der Farbe der
12 naechsten Originalflaechen desselben Meshes, gleiche Seite und CLUT-Zeile wie diese), alle seine Flaechen zeigen ein 2 x 2-Texel-Stueck
dort: gleichmaessige Haut der Umgebung, die Schattierung macht das Licht. Kein RE-Wert (Textur eines neuen Modells).
"""
import os, sys, json, copy, shutil, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import s3_rig as S          # noqa: E402
import form_emd as FE       # noqa: E402
sys.path.insert(0, os.path.join(S.FORM_DIR, 'varianten', 'K_alligator', 'generator'))
import k_naht as KN         # noqa: E402

K3 = S.K3_DIR
# Fassung 'gemischt' (gewaehlt): jeder Koerper ein zusammenhaengendes Hautstueck ('flaeche'), ausser wo ein Koerper im Screenshot eine
# gemalte Stelle von J zudeckt: dort setzt er die Haut der Unterlage fort ('projektion'). Ellbogen r liegt in Bild 1 genau auf dem
# weissen Dreieck Z5 (E1/E3/E14, Kennungsbild der Kamera von Bild 1: 169 von 169 Pixeln um die Sollmitte (403,289)); als Hautstueck
# verlor Bild 1 die Pruefpunkte b1.kappe_spitze/b1.kappe_basis.
KOERPER_ART = {'Ellbogen r': 'projektion'}   # Hals/Bein r als Projektion brachten Q3/Z4 nicht zurueck (gemessen), daher nur Ellbogen r
PROJ_NACHBARN = False   # mit Partner (Kind/Eltern) verloren Z5 und ein Pruefpunkt in Bild 1 (gemessen)
_RIG = S.Rig(os.path.join(S.K3_DIR, 'FORM.EMD'))
JE_KOERPER = dict(dreiecke=2 * KN.KUGEL_U, vierecke=(KN.KUGEL_V - 2) * KN.KUGEL_U, ecken=(KN.KUGEL_V - 1) * KN.KUGEL_U + 2)
NAECHSTE = 12


def farbe15(w):
    return np.array([(w & 31) << 3, ((w >> 5) & 31) << 3, ((w >> 10) & 31) << 3], float)


class Tex:
    def __init__(self, tim_bytes):
        t = FE.tim_lesen(tim_bytes)
        self.px, self.cluts, self.cy = t['pixel'], t['cluts'], t['clut_y']
        self.h, self.w = self.px.shape

    def texel(self, u, v, page, clut):
        x = int(u) + (int(page) & 0xF) * 128
        y = int(v)
        z = ((int(clut) >> 6) & 0x1FF) - self.cy
        if not (0 <= x < self.w and 0 <= y < self.h and 0 <= z < len(self.cluts)):
            return None
        return farbe15(self.cluts[z][self.px[y, x]])


def koerper_je_mesh(form, naht):
    """Mesh -> Liste der Gelenkkoerper (Name) in Anbau-Reihenfolge (k_naht.schliessen: Reihenfolge von naht['gelenke'])."""
    out = {}
    for nm, (kind, fq, fn, ax) in naht['gelenke'].items():
        if fq <= 0 or fn <= 0:
            continue
        b = kind if nm.startswith('Muffe ') else form['knochen'][kind]['eltern']
        out.setdefault(b, []).append(nm)
    return out


def flaechen_mitte(mesh, f):
    V = np.array(mesh['ecken'], float)
    return V[f['e']].mean(0)


def proben(mesh, f, tex):
    """Texelfarben einer Flaeche: Mitte und drei Punkte zwischen Mitte und Ecken (UV-Raum)."""
    uv = np.array(f['uv'][:3] if len(f['e']) == 3 else f['uv'], float)
    m = uv.mean(0)
    out = []
    for p in [m] + [m + 0.5 * (q - m) for q in uv]:
        c = tex.texel(round(p[0]), round(p[1]), f['page'], f['clut'])
        if c is not None:
            out.append((c, (int(round(p[0])), int(round(p[1])))))
    return out


def neu_uv(form, naht, tex, art='gemischt'):
    form = copy.deepcopy(form)
    km = koerper_je_mesh(form, naht)
    bericht = {}
    for b, namen in km.items():
        m = form['meshes'][b]
        nk = len(namen)
        nd, nv, ne = nk * JE_KOERPER['dreiecke'], nk * JE_KOERPER['vierecke'], nk * JE_KOERPER['ecken']
        orig_d, orig_v = m['dreiecke'][:-nd], m['vierecke'][:-nv]
        orig = [(f, 3) for f in orig_d] + [(f, 4) for f in orig_v]
        mitten = np.array([flaechen_mitte(m, f) for f, _ in orig])
        V = np.array(m['ecken'], float)
        e0 = len(V) - ne
        for i, nm in enumerate(namen):
            mitte = V[e0 + i * JE_KOERPER['ecken']: e0 + (i + 1) * JE_KOERPER['ecken']].mean(0)
            idx = np.argsort(np.linalg.norm(mitten - mitte, axis=1))[:NAECHSTE]
            # dominierende Seite/CLUT der Umgebung, darin der Texel, der dem Farbmedian am naechsten liegt
            schl = {}
            for j in idx:
                f = orig[j][0]
                schl.setdefault((f['page'], f['clut']), []).append(f)
            (page, clut), fs = max(schl.items(), key=lambda kv: len(kv[1]))
            P = [(c, uv) for f in fs for c, uv in proben(m, f, tex)]
            C = np.array([c for c, _ in P])
            med = np.median(C, axis=0)
            fd = m['dreiecke'][len(orig_d) + i * JE_KOERPER['dreiecke']: len(orig_d) + (i + 1) * JE_KOERPER['dreiecke']]
            fv = m['vierecke'][len(orig_v) + i * JE_KOERPER['vierecke']: len(orig_v) + (i + 1) * JE_KOERPER['vierecke']]
            art_k = KOERPER_ART.get(nm, art) if art == 'gemischt' else art
            if art_k == 'projektion':
                # Haut der Unterlage fortsetzen: jede Koerperflaeche nimmt die naechste Originalflaeche (Mitte zu Mitte) und bildet ihre
                # Ecken mit deren ebener Abbildung Raum -> UV ab (Ecke auf die Ebene projiziert, baryzentrisch, Ueberstand je Gewicht
                # bis -0,5). Benachbarte Koerperflaechen ueber derselben Unterlage teilen damit ihre UV-Kanten; was die Unterlage zeigt
                # (Haut, weisser Fleck Z5 am Ellbogen), setzt sich auf dem Koerper fort, statt als gedrehtes Stueck wiederholt zu werden.
                quellen = []
                nachbarn = [(b, orig, V)]
                if PROJ_NACHBARN:
                    # auch die Flaechen des Gelenkpartners (Kind bei Kugeln, Eltern bei Muffen), in der Ruhepose in diesen Knochenraum
                    # gebracht: liegt der Koerper ueber dem Partner (Oberarm vor der Brust), zeigt er dessen Haut (Band, Fleck)
                    kind_ = naht['gelenke'][nm][0]
                    partner = form['knochen'][kind_]['eltern'] if nm.startswith('Muffe ') else kind_
                    if partner != b:
                        Rr, Tr = _RIG.fk(_RIG.ruhe)
                        mp = form['meshes'][partner]
                        npk = len(km.get(partner, []))
                        po = (mp['dreiecke'][:len(mp['dreiecke']) - npk * JE_KOERPER['dreiecke']] +
                              mp['vierecke'][:len(mp['vierecke']) - npk * JE_KOERPER['vierecke']])
                        Vp = np.array(mp['ecken'], float)
                        Vp = ((Vp @ Rr[partner].T + Tr[partner]) - Tr[b]) @ Rr[b]
                        nachbarn.append((partner, [(f, len(f['e'])) for f in po], Vp))
                for _bb, fl, VV in nachbarn:
                    for f, _k in fl:
                        E = VV[f['e']]
                        UV = np.array(f['uv'][:len(f['e'])], float)
                        halb = [(0, 1, 2)] if len(f['e']) == 3 else [(0, 1, 3), (0, 3, 2)]
                        for h in halb:
                            quellen.append((E[list(h)], UV[list(h)], f))
                qm = np.array([q[0].mean(0) for q in quellen])

                def abbilden(X, q):
                    P, UV, _f = q
                    e1, e2 = P[1] - P[0], P[2] - P[0]
                    n = np.cross(e1, e2)
                    if np.linalg.norm(n) < 1e-9:
                        return UV.mean(0)
                    d = X - P[0]
                    d = d - (d @ n) / (n @ n) * n
                    M = np.array([[e1 @ e1, e1 @ e2], [e1 @ e2, e2 @ e2]])
                    b_ = np.linalg.solve(M, np.array([d @ e1, d @ e2]))
                    w = np.array([1 - b_.sum(), b_[0], b_[1]])
                    w = np.maximum(w, -0.5)
                    w = w / w.sum()
                    return w @ UV
                for f in fd + fv:
                    c = V[f['e']].mean(0)
                    q = quellen[int(np.argmin(np.linalg.norm(qm - c, axis=1)))]
                    rand = tex.w - 1 - (int(q[2]['page']) & 0xF) * 128
                    uvs = []
                    for e in f['e']:
                        u_, v_ = abbilden(V[e], q)
                        uvs.append([int(min(max(round(u_), 0), min(127, rand))), int(min(max(round(v_), 0), tex.h - 1))])
                    f['uv'], f['page'], f['clut'] = uvs, q[2]['page'], q[2]['clut']
                bericht[nm] = dict(mesh=b, art='projektion', flaechen=len(fd) + len(fv))
                continue
            if art_k == 'texel':
                k = int(np.argmin(np.linalg.norm(C - med, axis=1)))
                u, v = P[k][1]
                u = min(max(u, 0), 126)
                v = min(max(v, 0), 254)
                tri = [[u, v], [u + 1, v], [u, v + 1]]
                quad = [[u, v], [u + 1, v], [u, v + 1], [u + 1, v + 1]]
                for f in fd:
                    f['uv'], f['page'], f['clut'] = [list(x) for x in tri], page, clut
                for f in fv:
                    f['uv'], f['page'], f['clut'] = [list(x) for x in quad], page, clut
                bericht[nm] = dict(mesh=b, seite=page & 0xF, clut_zeile=((clut >> 6) & 0x1FF) - tex.cy, texel=[u, v],
                                   farbe=[int(x) for x in C[k]], median=[int(x) for x in med], flaechen_umgebung=len(fs))
                continue
            # art 'flaeche': zusammenhaengendes Hautstueck W x H Texel; Laengen-/Breitengrad des Ellipsoids -> u/v, Nachbarflaechen teilen
            # ihre UV-Kanten (nur an der Laengsnaht j = U-1 -> 0 springt die Textur). Groesse nach dem Koerper (rund 60 Einheiten je Texel,
            # J-Haut 20-40), 12-32 Texel. Lage: unter den Mitten der naechsten Flaechen das Rechteck, dessen Texel der Umgebungshaut in
            # Farbe und Koernung am naechsten liegen, ohne helle Ausreisser (weisse Flecken, Kanten: Y > Median + 40), ohne bunte Texel
            # (rosa Wundbaender: Buntheit > Umgebung + 30) und ohne Index 0.
            ecken = V[e0 + i * JE_KOERPER['ecken']: e0 + (i + 1) * JE_KOERPER['ecken']]
            ring = ecken[:KN.KUGEL_U]
            umfang = float(np.linalg.norm(np.roll(ring, -1, 0) - ring, axis=1).sum())
            laenge = float(np.linalg.norm(ecken[-2] - ecken[-1]))
            W = int(min(32, max(12, round(umfang / 60.0))))
            H = int(min(32, max(12, round(laenge / 60.0))))
            yl = 0.299 * med[0] + 0.587 * med[1] + 0.114 * med[2]
            ys = np.array([0.299 * c[0] + 0.587 * c[1] + 0.114 * c[2] for f in fs for c in texel_der_flaeche(f, tex)])
            y_streu = float(ys.std()) if len(ys) else 0.0
            mitten_uv = [np.array(f['uv'][:len(f['e'])], float).mean(0) for f in fs]
            best = None
            # Stueckgroesse: wie oben, notfalls kleiner (3/4, 1/2, mind. 8 Texel), bis ein Stueck ganz ohne Fleck gefunden ist
            for teil in (1.0, 0.75, 0.5, 0.35):
                Wt, Ht = max(8, int(round(W * teil))), max(8, int(round(H * teil)))
                kand = set()
                for uv in mitten_uv:
                    for du in range(-16, 17, 4):
                        for dv in range(-16, 17, 4):
                            kand.add((int(min(max(round(uv[0] - Wt / 2 + du), 0), 127 - Wt)),
                                      int(min(max(round(uv[1] - Ht / 2 + dv), 0), tex.h - 1 - Ht))))
                for u0, v0 in sorted(kand):
                    T_ = [tex.texel(uu, vv, page, clut) for vv in range(v0, v0 + Ht + 1) for uu in range(u0, u0 + Wt + 1)]
                    if any(t is None for t in T_):
                        continue
                    T_ = np.array(T_)
                    Y = 0.299 * T_[:, 0] + 0.587 * T_[:, 1] + 0.114 * T_[:, 2]
                    x0 = (u0 + (int(page) & 0xF) * 128)
                    idx0 = float((tex.px[v0:v0 + Ht + 1, x0:x0 + Wt + 1] == 0).mean())
                    chroma = T_.max(1) - T_.min(1)
                    wert = (float(np.abs(T_.mean(0) - med).mean()) + 0.5 * abs(float(Y.std()) - y_streu) + 1000.0 * idx0)
                    # harte Grenze: kein einziger weisser/bunter Texel im Stueck (sonst sitzt ein Fleck mitten im Gelenk)
                    if Y.max() > yl + 60 or chroma.max() > (med.max() - med.min()) + 40:
                        wert += 10000.0
                    if best is None or wert < best[0]:
                        best = (wert, u0, v0, T_, Wt, Ht)
                if best[0] < 10000.0:
                    break
            _w, u0, v0, T_, W, H = best
            U, Vn = KN.KUGEL_U, KN.KUGEL_V
            k0 = e0 + i * JE_KOERPER['ecken']

            def gitter(e):
                k = e - k0
                if k < (Vn - 1) * U:
                    return (k // U) + 1, k % U
                return (0, None) if k == (Vn - 1) * U else (Vn, None)
            for f in fd + fv:
                g = [gitter(e) for e in f['e']]
                js = [j for _i, j in g if j is not None]
                if js and max(js) == U - 1 and min(js) == 0:
                    g = [(ii, (U if j == 0 else j)) if j is not None else (ii, j) for ii, j in g]
                    js = [j for _i, j in g if j is not None]
                jm = sum(js) / float(len(js))
                f['uv'] = [[int(round(u0 + W * ((j if j is not None else jm) / float(U)))), int(round(v0 + H * (ii / float(Vn))))]
                           for ii, j in g]
                f['page'], f['clut'] = page, clut
            bericht[nm] = dict(mesh=b, seite=page & 0xF, clut_zeile=((clut >> 6) & 0x1FF) - tex.cy, rechteck=[u0, v0, W, H],
                               farbe_mittel=[int(x) for x in T_.mean(0)], median_umgebung=[int(x) for x in med], flaechen_umgebung=len(fs))
    # Naht der Zylinder-UV von Kopf/Kiefer (k_form, Seite 3): u = 128 an der Wickelkante liegt einen Texel rechts der TIM (x 512 bei
    # Breite 512). Port und Buehne lesen dort ausserhalb der Textur. Auf den letzten Texel der Seite klemmen.
    geklemmt = 0
    for m in form['meshes']:
        for f in m['dreiecke'] + m['vierecke']:
            rand = tex.w - 1 - (int(f['page']) & 0xF) * 128
            for q in f['uv']:
                if q[0] > rand:
                    q[0] = rand
                    geklemmt += 1
    bericht['_uv_geklemmt'] = geklemmt
    return form, bericht


def texel_der_flaeche(f, tex, n=4):
    """Texelfarben auf einem Gitter im UV-Dreieck/Viereck der Flaeche (baryzentrisch)."""
    uv = np.array(f['uv'][:len(f['e'])], float)
    tris = [uv[[0, 1, 2]]] if len(uv) == 3 else [uv[[0, 1, 3]], uv[[0, 3, 2]]]
    out = []
    for t in tris:
        for a_ in range(n + 1):
            for b_ in range(n + 1 - a_):
                w = np.array([a_, b_, n - a_ - b_], float) / n
                p = w @ t
                c = tex.texel(int(p[0]), int(p[1]), f['page'], f['clut'])
                if c is not None:
                    out.append(c)
    return out


def vergleich(form, naht, tex):
    """Je Gelenkkoerper: Farbe (Mittel) und Koernung (Streuung der Helligkeit) seiner Texel gegen die 12 naechsten Originalflaechen
    desselben Meshes. Rueckgabe Mittelwerte ueber alle Koerper: Farbabstand (RGB, Mittel |d|), Koernungsverhaeltnis Koerper/Umgebung."""
    km = koerper_je_mesh(form, naht)
    fa, kv, n = [], [], 0
    for b, namen in km.items():
        m = form['meshes'][b]
        nk = len(namen)
        nd, nv, ne = nk * JE_KOERPER['dreiecke'], nk * JE_KOERPER['vierecke'], nk * JE_KOERPER['ecken']
        orig = m['dreiecke'][:-nd] + m['vierecke'][:-nv]
        mitten = np.array([flaechen_mitte(m, f) for f in orig])
        V = np.array(m['ecken'], float)
        e0 = len(V) - ne
        for i in range(nk):
            mitte = V[e0 + i * JE_KOERPER['ecken']: e0 + (i + 1) * JE_KOERPER['ecken']].mean(0)
            idx = np.argsort(np.linalg.norm(mitten - mitte, axis=1))[:NAECHSTE]
            U_ = np.array([c for j in idx for c in texel_der_flaeche(orig[j], tex)])
            fs = (m['dreiecke'][len(m['dreiecke']) - nd + i * JE_KOERPER['dreiecke']: len(m['dreiecke']) - nd + (i + 1) * JE_KOERPER['dreiecke']] +
                  m['vierecke'][len(m['vierecke']) - nv + i * JE_KOERPER['vierecke']: len(m['vierecke']) - nv + (i + 1) * JE_KOERPER['vierecke']])
            K_ = np.array([c for f in fs for c in texel_der_flaeche(f, tex)])
            yu = U_ @ np.array([0.299, 0.587, 0.114]); yk = K_ @ np.array([0.299, 0.587, 0.114])
            fa.append(float(np.abs(K_.mean(0) - U_.mean(0)).mean()))
            kv.append(float(yk.std() / max(yu.std(), 1e-6)))
            n += 1
    return dict(koerper=n, farbabstand_mittel=round(float(np.mean(fa)), 1), farbabstand_max=round(float(np.max(fa)), 1),
                koernung_koerper_durch_umgebung_mittel=round(float(np.mean(kv)), 2),
                koernung_min_max=[round(float(np.min(kv)), 2), round(float(np.max(kv)), 2)])


def pruefen(emd, tim, naht=None):
    """Seite/CLUT je Flaeche (Texel innerhalb TIM, Zeile vorhanden), und je Gelenkkoerper: wie viele verschiedene UV-Stuecke, Helligkeitsspanne
    der Texel (hell = weisser Fleck), gegen den Median seiner Umgebung."""
    form = FE.lese_emd(emd)
    tex = Tex(tim)
    falsch, ges = 0, 0
    for m in form['meshes']:
        for f in m['dreiecke'] + m['vierecke']:
            ges += 1
            uv = f['uv'][:len(f['e'])]
            if any(tex.texel(u, v, f['page'], f['clut']) is None for u, v in uv):
                falsch += 1
    out = dict(flaechen=ges, texel_ausserhalb_tim_oder_ohne_clut=falsch)
    if naht:
        km = koerper_je_mesh(form, naht)
        stuecke, spanne = [], []
        for b, namen in km.items():
            m = form['meshes'][b]
            nk = len(namen)
            fs = m['dreiecke'][-nk * JE_KOERPER['dreiecke']:] + m['vierecke'][-nk * JE_KOERPER['vierecke']:]
            stuecke.append(len(set((tuple(map(tuple, f['uv'])), f['page'], f['clut']) for f in fs)))
            Y = []
            for f in fs:
                for c, _ in proben(m, f, tex):
                    Y.append(0.299 * c[0] + 0.587 * c[1] + 0.114 * c[2])
            spanne.append(float(np.percentile(Y, 95) - np.percentile(Y, 5)) if Y else 0.0)
        out.update(gelenkkoerper_meshes=len(km), verschiedene_uv_stuecke_je_mesh=stuecke,
                   helligkeit_p95_minus_p5_je_mesh=[round(x) for x in spanne], vergleich_umgebung=vergleich(form, naht, tex))
    return out


def main():
    a = argparse.ArgumentParser()
    a.add_argument('was', choices=('pruefen', 'bauen'))
    a.add_argument('pfade', nargs='*')
    a.add_argument('--art', default='gemischt', choices=('gemischt', 'projektion', 'flaeche', 'texel'))
    g = a.parse_args()
    naht = json.load(open(os.path.join(K3, 'naht.json'), encoding='utf-8'))
    if g.was == 'pruefen':
        emd = g.pfade[0] if g.pfade else os.path.join(K3, 'FORM.EMD')
        tim = g.pfade[1] if len(g.pfade) > 1 else os.path.join(K3, 'FORM.TIM')
        print(json.dumps(pruefen(open(emd, 'rb').read(), open(tim, 'rb').read(), naht)))
        return
    aus = g.pfade[0]
    form = FE.lese_emd(open(os.path.join(K3, 'FORM.EMD'), 'rb').read())
    tim = open(os.path.join(K3, 'FORM.TIM'), 'rb').read()
    neu, ber = neu_uv(form, naht, Tex(tim), g.art)
    os.makedirs(aus, exist_ok=True)
    emd = FE.schreibe_emd(neu)
    open(os.path.join(aus, 'FORM.EMD'), 'wb').write(emd)
    shutil.copy(os.path.join(K3, 'FORM.TIM'), os.path.join(aus, 'FORM.TIM'))
    fj = json.load(open(os.path.join(K3, 'form.json'), encoding='utf-8'))
    fj['name'] = os.path.basename(os.path.normpath(aus))
    json.dump(fj, open(os.path.join(aus, 'form.json'), 'w', encoding='utf-8'), indent=1)
    shutil.copy(os.path.join(K3, 'naht.json'), os.path.join(aus, 'naht.json'))
    json.dump(dict(quelle='varianten/K_alligator/K3_fit (Meshes, TIM, Posen), nur UV/Seite/CLUT der 31 Gelenkkoerper neu', koerper=ber),
              open(os.path.join(aus, 'textur_gelenkkoerper.json'), 'w', encoding='utf-8'), indent=1)
    print('vorher', json.dumps(pruefen(open(os.path.join(K3, 'FORM.EMD'), 'rb').read(), tim, naht)))
    print('nachher', json.dumps(pruefen(emd, tim, naht)))


if __name__ == '__main__':
    main()
