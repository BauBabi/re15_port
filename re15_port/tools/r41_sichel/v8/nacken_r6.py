"""SF V8 Runde 6 Stufe n) Nackenstueck an Mesh 8: schliesst die Kerbe zwischen Buckel-Vorderkante und Hinterkopf (P5-neu).

    python nacken_r6.py --suche --basis EM30_V7.EMD [--par nacken_v8.json]   (Haut-Ort je Stueck neu bestimmen und in die Datei schreiben)

Nutzer 2026-11-09 (woertlich): "es gibt immer noch eine Lücke zwischen Rücken und Kopf beim 2 beiner Birkin. Das ist in der Vorlage so
wie es aussieht nicht so." Frueher: "Auch der obere Rücken geht im original screenshot direkt mehr oder weniger in den Kopf über." und
Darstellung.png (weiss) "smooth straight to head without further hills....".
Auftrag Runde 6 (P5-neu, nach den Pruefern der Runde 5: Kragen um den Kopf, die ganze Frontansicht lang): die in Runde 5 angehobenen
Ecken gehen vorn zurueck auf Runde 4 (ruecken_v8.json), die Kerbe schliesst ein kleines GESCHLOSSENES Nackenstueck an Mesh 8, dessen
vorderes Ende IM Hinterkopf liegt und dessen Seiten in Schultern und Rumpf eintauchen. Es ersetzt den Einsatz der Runde 5 (nacken_r5.py).
Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; jede Zahl ist PORT-WAHL mit Messung (Dossier SF_sichel.md "V8 Runde 6").

Was das ist: je Stueck aus nacken_v8.json "koerper" die konvexe Huelle seiner "punkte" (Knochenraum 8: x vorn, y < 0 oben, z > 0 (W)-links), angehaengt an
Mesh 8 als neue Ecken und Dreiecke; die bisherigen Ecken, Flaechen und Nummern bleiben. Umlaufsinn wie Mesh 8 ((v1 - v0) x (v2 - v0) zeigt
nach innen), Eckennormalen = flaechengewichtetes Mittel der Aussennormalen (stetig ueber den ganzen Koerper).
Haut: Original-Texel der Rueckenhaut (Zeile 493 = Original-EM30-Zeile 480, Seite 0) in natuerlicher Texeldichte: die Flaechen, die nach
"blick" zeigen (Winkel bis 60 Grad), liegen in EINER ebenen Karte (Projektion laengs "blick", Massstab = Median Texel je Einheit der
Dreiecke von Mesh 8) - benachbarte Flaechen tragen damit zusammenhaengende Haut ohne Naht; steilere Flaechen (verdeckt im Rumpf oder im
Kopf) bekommen je eine winkeltreue Abwicklung im selben Kartenfeld. Der Ort der Karte ("uv_ort", "drehung") kommt aus --suche: alle Texel
unter allen Flaechen samt 1 Texel Rand = Original-Texel, von Original-Flaechen der Zeile 480 benutzt, ohne Feld C, nicht fast weiss
(adern_r5.erlaubt) und nicht hell-blaeulich (hell_karte: B >= R - 6, B > 120, R > 110 - das Mass des Sicht-Pruefers r5 fuer den Keil);
unter den zulaessigen Orten der mit dem kleinsten Farbabstand (Lab) zum Mittel der angrenzenden Buckelflaechen ("nachbarn").
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402,F401  (setzt die Suchpfade)
import kopf_orig_bau as KB            # noqa: E402

STEIL_COS = 0.5         # Flaechen mit cos(Winkel zu "blick") >= 0,5 liegen in der Karte (Streckung <= 2 = Soll des Riegels S19)
RAND = 1                # Texel Rand um jede Flaeche (der Rasterer liest an Kanten bis 1 Texel ausserhalb, wie adern_r5.WEISS_RAND)
FARBGRENZE = 1.5                # darueber kein Schub (die Flaeche bleibt, ihre hellen Einzeltexel auch); PORT-WAHL, am Riegel S14 (a) gemessen:
                                # mit 4,0 (v15 Abstand 3,92, d66 2,84, d49 2,07) Takt 364 dL 3,83 / da* 2,98; mit 2,0 (d8 Abstand 1,52) dL 3,69 -
                                # Soll dL <= 3,61 (V7 2,11 + 1,5), da* <= 2,93; mit 1,5 dL 3,03 / da* 2,51 (gruen)
FARBTREU = 1.5                  # Schub einzelner Flaechen ohne "nachbarn": Lab-Abstand des Flaechenmittels zum bisherigen hoechstens 1,5
                                # (= riegel_v8.S14_KANTE_RAND, der Rand des Kanten-Sprungs dL); PORT-WAHL aus Messung (Versuch 3d: Schuebe
                                # mit Abstand 6,7-16,5 hoben S14 (a) dL von 2,6 auf 4,3-5,9 und legten v24 auf fast weisse Texel, 448 px)


def koerper(punkte):
    """-> (Ecken P, Aussennormalen N je Ecke, Dreiecke T mit Umlaufsinn nach innen, Aussennormale je Dreieck)."""
    from scipy.spatial import ConvexHull
    P = np.array(punkte, float)
    h = ConvexHull(P)
    auf = sorted(int(i) for i in set(h.vertices.tolist()))      # Punkte im Inneren der Huelle fallen weg (Reihenfolge der Liste bleibt)
    nr = {i: k for k, i in enumerate(auf)}
    P = P[auf]
    sim = [[nr[int(i)] for i in s] for s in h.simplices]
    c = P.mean(0)
    T, FN = [], []
    for s in sorted(sorted(s) for s in sim):
        a, b, d = s
        g = np.cross(P[b] - P[a], P[d] - P[a])
        if g @ (P[a] - c) > 0:              # g zeigt nach aussen -> drehen (Mesh 8: nach innen)
            b, d = d, b
            g = -g
        T.append([a, b, d]); FN.append(-g)
    FN = np.array(FN)
    N = np.zeros_like(P)
    for f, n in zip(T, FN):
        for i in f:
            N[i] += n                        # |n| = 2 x Flaeche: flaechengewichtet
    N /= np.linalg.norm(N, axis=1)[:, None]
    return P, N, T, FN / np.linalg.norm(FN, axis=1)[:, None]


def dichte(m, bis=None):
    """Median Texel je Einheit ueber die Dreiecke von Mesh 8 (wie nacken_r5.dichte); bis = nur die ersten Dreiecke (ohne das Nackenstueck)."""
    P = np.array([v[:3] for v in m['v']], float)
    out = []
    for f, uv in list(zip(m['tf'], m['tuv']))[:bis]:
        a, b, c = P[f[1]], P[f[3]], P[f[5]]
        fr = 0.5 * np.linalg.norm(np.cross(b - a, c - a))
        fu = 0.5 * abs((uv[3] - uv[0]) * (uv[7] - uv[1]) - (uv[4] - uv[1]) * (uv[6] - uv[0]))
        if fr > 0 and fu > 0:
            out.append(np.sqrt(fu / fr))
    return float(np.median(out))


def karte(P, T, FN, blick, s):
    """-> je Dreieck 3 Kartenpunkte (float, Ursprung = Kartenmitte) und ob steil. Karte: Projektion laengs blick; steil: eigene Abwicklung."""
    n = np.array(blick, float); n /= np.linalg.norm(n)
    eu = np.array([0.0, 0.0, 1.0]); eu -= (eu @ n) * n; eu /= np.linalg.norm(eu)
    ev = np.cross(n, eu)
    E = np.stack([P @ eu, P @ ev], 1) * s
    E -= (E.max(0) + E.min(0)) / 2
    out, steil = [], []
    for f, fn in zip(T, FN):
        if fn @ n >= STEIL_COS:
            out.append(E[f]); steil.append(False)
        else:
            Q = P[f]
            x = (Q[1] - Q[0]) / np.linalg.norm(Q[1] - Q[0])
            g = np.cross(Q[1] - Q[0], Q[2] - Q[0])
            y = np.cross(g / np.linalg.norm(g), x)
            A = np.array([[(q - Q[0]) @ x, (q - Q[0]) @ y] for q in Q]) * s
            out.append(A - A.mean(0)); steil.append(True)
    return out, steil


def uv_von(E, ort, drehung, zeile, seite):
    """Kartenpunkte -> ganze Texel am Ort (Mitte) mit Drehung k x 90 Grad; -> uv (9 Werte, CLUT-Feld = Zeile * 64, Seitenfeld = Seite)."""
    k = drehung % 4
    c, s_ = (1, 0, -1, 0)[k], (0, 1, 0, -1)[k]
    R = np.array([[c, -s_], [s_, c]], float)
    e = E @ R.T + np.array(ort, float)
    e = [[int(round(u)), int(round(v))] for u, v in e]
    return [e[0][0], e[0][1], zeile * 64, e[1][0], e[1][1], seite, e[2][0], e[2][1], 0]


def hell_karte(k):
    """Original-Zeile 480 + k: Texel, deren Farbe das Hell-Mass des Sicht-Pruefers r5 erfuellt (B >= R - 6, B > 120, R > 110; 8 Bit)."""
    import adern_r4 as A4
    _MO, TO = A4.original()
    c = np.array(TO['clut'][k]) & 0x7FFF
    r, b = (c & 31) * 8, ((c // 1024) & 31) * 8
    return ((b >= r - 6) & (b > 120) & (r > 110))[np.array(TO['pix'])]


def weiss_karte(k):
    """Original-Zeile 480 + k: fast weisse Texel (L > 75, Chroma < 12 - Mass riegel_v8.weiss_px / Sicht-Pruefer r4), unbeleuchtet."""
    import adern_v8 as AV
    import adern_r4 as A4
    _MO, TO = A4.original()
    lab = AV.lab15(np.array(TO['clut'][k]) & 0x7FFF)
    return ((lab[:, 0] > 75) & (np.hypot(lab[:, 1], lab[:, 2]) < 12))[np.array(TO['pix'])]


def sauber(k):
    """Zulaessige Texel fuer neue / umgelegte Haut: adern_r5.erlaubt ohne hell-blaeuliche Texel samt RAND."""
    import adern_r5 as A5
    from scipy import ndimage
    h = ndimage.binary_dilation(hell_karte(k) | weiss_karte(k), structure=np.ones((3, 3)), iterations=RAND)
    return A5.erlaubt(k, np.ones((256, 384), bool)) & ~h


def maske(uv, seite):
    from scipy import ndimage
    mk = KB.raster(uv, 3 if len(uv) == 9 else 4, seite * 128)
    return ndimage.binary_dilation(mk, structure=np.ones((3, 3)), iterations=RAND)


def nachbar_lab(m, t, nachbarn):
    import adern_v8 as AV
    L = []
    for key in nachbarn:
        uv = AV.flaeche(m, key)
        z = (uv[2] // 64) & 0x1FF
        mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
        L.append(AV.lab15(np.array(t['clut'][z - 480])[np.array(t['pix'])[mk]] & 0x7FFF))
    return np.concatenate(L).mean(0)


def _koerper_liste(par):
    """nacken_v8.json: "koerper" = Liste von Stuecken (je punkte, blick, nachbarn, uv_ort, drehung); Zeile und Seite gelten fuer alle."""
    return par['koerper']


def ort_suche(m, t, par, kp, s, log=print):
    """Bester Ort der Karte EINES Stuecks: -> dict(uv_ort, drehung, abstand, kandidaten). Deterministisch (Reihenfolge Drehung, v, u;
    strikt kleiner). s = Texel je Einheit (dichte von Mesh 8 ohne die Nackenstuecke)."""
    import adern_v8 as AV
    import adern_r4 as A4
    P, N, T, FN = koerper(kp['punkte'])
    E, _steil = karte(P, T, FN, kp['blick'], s)
    k = par['zeile'] - 493
    ok = sauber(k)
    _MO, TO = A4.original()
    lab = AV.lab15(np.array(TO['clut'][k])[np.array(TO['pix'])] & 0x7FFF).reshape(256, 384, 3)
    ziel = nachbar_lab(m, t, kp['nachbarn'])
    best, n = None, 0
    for dreh in (0, 1, 2, 3):
        m0 = np.zeros((256, 384), bool)
        for e in E:
            m0 |= maske(uv_von(e, (64 + par['seite'] * 128, 128), dreh, par['zeile'], par['seite']), par['seite'])
        ys, xs = np.nonzero(m0)
        for dv in range(-int(ys.min()), 256 - int(ys.max())):
            for du in range(par['seite'] * 128 - int(xs.min()), par['seite'] * 128 + 128 - int(xs.max())):
                if not ok[ys + dv, xs + du].all():
                    continue
                n += 1
                d = float(np.linalg.norm(lab[ys + dv, xs + du].mean(0) - ziel))
                if best is None or d < best[0] - 1e-9:
                    best = (d, 64 + par['seite'] * 128 + du, 128 + dv, dreh)
    assert best is not None, 'kein zulaessiger Ort fuer die Karte'
    log('nacken_r6 ort_suche %s: %d zulaessige Orte, bester Abstand %.2f bei (%d, %d) Drehung %d; Ziel Lab %s'
        % (kp.get('name'), n, best[0], best[1], best[2], best[3], np.round(ziel, 1).tolist()))
    return dict(uv_ort=[best[1], best[2]], drehung=best[3], abstand=round(best[0], 2), kandidaten=n, ziel_lab=np.round(ziel, 1).tolist())


def schub_suche(m, t, par, h, log=print):
    """P5 (d) heller Keil: neuer Ort (nur Schub, Form und Texeldichte bleiben) fuer EINE Flaeche, deren Haut als Ganzes blasser ist als
    ihre Umgebung: alle Texel samt Rand zulaessig (sauber), kleinster Farbabstand (Lab) zum Mittel der "nachbarn"; bei Gleichstand der
    kleinere Schub. Gesucht wird am Stand VOR dem Schub (Stufen s r w a u). -> dict(schub, abstand, kandidaten, vorher, ziel_lab)."""
    import adern_v8 as AV
    import adern_r4 as A4
    uv = AV.flaeche(m, h['flaeche'])
    z, seite = (uv[2] // 64) & 0x1FF, uv[5] & 0xF
    k = z - 493
    ok = sauber(k)
    _MO, TO = A4.original()
    lab = AV.lab15(np.array(TO['clut'][k])[np.array(TO['pix'])] & 0x7FFF).reshape(256, 384, 3)
    ziel = nachbar_lab(m, t, h['nachbarn']) if h.get('nachbarn') else None     # ohne "nachbarn": nur der kleinste Schub (einzelne Texel)
    ys, xs = np.nonzero(maske(uv, seite))
    kern_y, kern_x = np.nonzero(KB.raster(uv, 3 if len(uv) == 9 else 4, seite * 128))
    if ziel is None:
        ziel = lab[kern_y, kern_x].mean(0)                                    # Farbe bleibt: Abstand zum eigenen Mittel, auf 1 gerundet
    vorher = float(np.linalg.norm(lab[kern_y, kern_x].mean(0) - nachbar_lab(m, t, h['nachbarn']))) if h.get('nachbarn') else 0.0
    us = [uv[3 * i] for i in range(3 if len(uv) == 9 else 4)]; vs = [uv[3 * i + 1] for i in range(3 if len(uv) == 9 else 4)]
    best, n = None, 0
    for dv in range(max(-int(ys.min()), -min(vs)), min(256 - int(ys.max()), 256 - max(vs))):
        for du in range(max(seite * 128 - int(xs.min()), -min(us)), min(seite * 128 + 128 - int(xs.max()), 256 - max(us))):
            if not ok[ys + dv, xs + du].all():
                continue
            n += 1
            d = float(np.linalg.norm(lab[kern_y + dv, kern_x + du].mean(0) - ziel))
            if h.get('nachbarn'):
                d = (0, round(d, 2), du * du + dv * dv)
            elif d <= FARBTREU:           # einzelne Texel: die Flaeche behaelt ihre Farbe (Riegel S14 a/c) - kleinster Schub
                d = (0, du * du + dv * dv, round(d, 2))
            else:                         # kein farbtreuer Schub: der farbnaechste (nur bis FARBGRENZE)
                d = (1, round(d, 2), du * du + dv * dv)
            if best is None or d < best[0]:
                best = (d, du, dv)
    if best is not None and best[0][0] == 1 and best[0][1] > FARBGRENZE:
        best = None
    if best is None:
        log('nacken_r6 schub_suche %s: kein farbtreuer Schub (Lab-Abstand zum eigenen Mittel <= %.1f) - Flaeche bleibt' % (h['flaeche'], FARBGRENZE))
        return None
    ab = best[0][1] if (h.get('nachbarn') or best[0][0] == 1) else best[0][2]
    log('nacken_r6 schub_suche %s: %d zulaessige Schuebe, Abstand vorher %.2f, gewaehlt %.2f bei Schub (%d, %d); Ziel Lab %s'
        % (h['flaeche'], n, vorher, ab, best[1], best[2], np.round(ziel, 1).tolist()))
    return dict(schub=[best[1], best[2]], abstand=ab, kandidaten=n, vorher=round(vorher, 2), ziel_lab=np.round(ziel, 1).tolist())


def haut_neu(ms, t, par, prot):
    """Schuebe aus nacken_v8.json "haut_neu" anwenden (vor den Nackenstuecken)."""
    import adern_v8 as AV
    m = ms[8]
    out = []
    for h in par.get('haut_neu', []):
        uv = AV.flaeche(m, h['flaeche'])
        AV.schiebe(uv, int(h['schub'][0]), int(h['schub'][1]))
        seite = uv[5] & 0xF
        assert not (maske(uv, seite) & ~sauber(((uv[2] // 64) & 0x1FF) - 493)).any(), ('Texel ausserhalb der zulaessigen Haut', h)
        out.append(dict(flaeche=h['flaeche'], schub=list(h['schub']), uv=list(uv)))
    prot['nacken_haut_neu'] = out


def anwenden(ms, t, par, prot):
    m = ms[8]
    haut_neu(ms, t, par, prot)
    s = dichte(m)                              # vor dem Anhaengen: Mesh 8 ohne die Nackenstuecke
    ok = sauber(par['zeile'] - 493)
    v_erst, d_erst = len(m['v']), len(m['tf'])
    stuecke = []
    for kp in _koerper_liste(par):
        P, N, T, FN = koerper(kp['punkte'])
        E, steil = karte(P, T, FN, kp['blick'], s)
        v0, n0 = len(m['v']), len(m['n'])
        for p, n in zip(P, N):
            m['v'].append([int(round(x)) for x in p] + [0])
            m['n'].append([int(round(x * 4096)) for x in n] + [0])
        uvs = []
        for f, e in zip(T, E):
            uv = uv_von(e, kp['uv_ort'], kp['drehung'], par['zeile'], par['seite'])
            assert not (maske(uv, par['seite']) & ~ok).any(), ('Texel ausserhalb der zulaessigen Haut', kp.get('name'), f, uv)
            m['tf'].append([n0 + f[0], v0 + f[0], n0 + f[1], v0 + f[1], n0 + f[2], v0 + f[2]])
            m['tuv'].append(uv)
            uvs.append(uv)
        stuecke.append(dict(name=kp.get('name'), ecken=len(P), dreiecke=len(T), erste_ecke=v0, steil=int(sum(steil)),
                            uv_ort=list(kp['uv_ort']), drehung=kp['drehung'], uv=uvs))
    prot['nacken'] = dict(art='huelle_r6', ecken=len(m['v']) - v_erst, dreiecke=len(m['tf']) - d_erst, vierecke=0, erste_ecke=v_erst,
                          erstes_dreieck=d_erst, zeile=par['zeile'], seite=par['seite'], texel_je_einheit=round(s, 5), stuecke=stuecke,
                          quelle=par.get('quelle'))


def main():
    """--suche: Haut-Ort je Stueck neu bestimmen (am Stand der Stufen s r w a u) und in die Parameterdatei schreiben."""
    ap = argparse.ArgumentParser()
    ap.add_argument('--suche', action='store_true')
    ap.add_argument('--par', default=os.path.join(HIER, 'nacken_v8.json'))
    ap.add_argument('--basis', default=V.ASSET)
    a = ap.parse_args()
    par = json.load(open(a.par, encoding='utf-8'))
    if not a.suche:
        for kp in _koerper_liste(par):
            P, N, T, FN = koerper(kp['punkte'])
            print('%s: %d Ecken, %d Dreiecke, Ort %s Drehung %s' % (kp.get('name'), len(P), len(T), kp.get('uv_ort'), kp.get('drehung')))
        return
    import v8_bau as VB
    blob, _prot = VB.bauen(a.basis, 'srwau')
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    bleibt = []
    for h in par.get('haut_neu', []):
        e = schub_suche(ms[8], t, par, h)
        if e is None:
            bleibt.append(h)
            continue
        h.update(schub=e['schub'], abstand=e['abstand'], kandidaten=e['kandidaten'], vorher=e['vorher'], ziel_lab=e['ziel_lab'])
    if bleibt:
        par['haut_neu'] = [h for h in par['haut_neu'] if h not in bleibt]
        par['haut_ohne_schub'] = sorted(set(par.get('haut_ohne_schub', [])) | set(h['flaeche'] for h in bleibt))
    s = dichte(ms[8])
    for kp in _koerper_liste(par):
        e = ort_suche(ms[8], t, par, kp, s)
        kp.update(uv_ort=e['uv_ort'], drehung=e['drehung'], abstand=e['abstand'], kandidaten=e['kandidaten'], ziel_lab=e['ziel_lab'])
    open(a.par, 'w', encoding='utf-8', newline='\n').write(json.dumps(par, indent=1, ensure_ascii=False) + '\n')
    print('geschrieben', a.par)


if __name__ == '__main__':
    main()
