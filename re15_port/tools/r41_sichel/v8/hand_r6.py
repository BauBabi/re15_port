"""SF V8 Runde 6a Stufe h) linke Hand (Mesh 12) und linker Unterarm (Mesh 11) nach dem Messblatt Hand (P6).

Nutzer 2026-11-10 (woertlich, bindend): "Irgendwie sieht die linke Hand vom 2 Beiner Birkin Modell immer noch komisch aus, im Vergleich
zum Original Material. Ich weiß nicht genau was es ist... Sind die Krallen zu dünn? Überlappen sie? Irgendwas passt nicht...." und
"Bei drei_posen.png was du gepostet hast, sieht man es ganz eindeutig. Die Vorlage hat eine breite Hand mit breiten Unterarmt. Dadurch
wirkt es natürlich. Bei uns ist die Hand ganz klein und zierlich und wirkt dadurch verkümmert."
Messblatt: analysis/befunde_runde41/SF_MESSBLATT_HAND.md (Commit a43dc024c), Werkzeug hand_mass.py.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Jede Zahl in hand_v8.json ist PORT-WAHL
aus dem Messblatt (Breitenprofil der Vorlage birkin20-24) bzw. aus hand_suche.py (Varianten gegen dieses Profil).

Was die Stufe tut (nur Ecken und Normalen von Mesh 11 und 12; Flaechen, UV, CLUT-Zeilen 490 / 491 / 492, Skelett und Clips bleiben):
  Mesh 12 = ORIGINAL-Klauenhand des RE1.5-Birkin (CDEMD0.EMS Typ 0x30, Mesh 12) je Achse skaliert ("skala"), um die Handachse y gedreht
     ("dreh_grad": der Handruecken +z zeigt danach auf Azimut dreh_grad um y, x -> (cos, 0, -sin)), entlang y verschoben ("schub_y").
     Vor dem Skalieren: Krallenspitzen ("krallen"), Handansatz Ecken 1-4 ("wurzelring": Hoehe y, Radialfaktor) und "ebnen" (eine Ecke in
     ihrer Hoehe y in die Ebene dreier Ecken - gegen den Schlitz eines windschiefen Vierecks, Riegel S9 (b); Quelle in hand_v8.json).
     Die bisherige Hand war das Original x 0,55 / 0,54 / 0,55 (tools/birkin_formen.py FORMEN hangar skal 12) mit in V7 verlaengerten
     Krallenspitzen; die Original-Krallen sind in der neuen Groesse laenger als diese (Messblatt Abschnitt 6).
  Mesh 11 = Original-Unterarm x "grund" (1,229 / 0,80 / 1,229 wie bisher, birkin_formen.py), danach je Ecke ein Radialfaktor f(y) und eine
     neue Hoehe y' aus den Stuetzstellen "verlauf" [y, y', f] (stueckweise linear ueber die bisherige Hoehe y).
Normalen: mit der Inverstransponierten der jeweiligen Abbildung (Hand: R * S^-1; Unterarm: Jacobi-Matrix von (f x, y', f z)), Laenge 4096.
"""
import json, math
import numpy as np
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402

HAND, UNTERARM = 12, 11
KRALLEN = {'T': (20, [18, 19, 21]), 'A': (47, [45, 46, 48]), 'B': (29, [27, 28, 30]), 'C': (42, [40, 41, 43])}   # Spitze, Ring (hand_gemein.krallen)


def ry(grad):
    c, s = math.cos(math.radians(grad)), math.sin(math.radians(grad))
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def rz(grad):
    c, s = math.cos(math.radians(grad)), math.sin(math.radians(grad))
    return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


def om_flaechen(_c=None):
    """Flaechen der Original-Hand (Mesh 12) als Liste (Normalen-Indizes, Ecken-Indizes)."""
    om = original()[HAND]
    return [(list(f[0::2]), list(f[1::2])) for f in list(om['tf']) + list(om['qf'])]


def _normalen_nur(fl, ecken):
    """Normalen-Indizes, die ausschliesslich an Ecken aus <ecken> haengen (Runde 7a)."""
    an, fremd = set(), set()
    for ns, vs in fl:
        for n, v in zip(ns, vs):
            (an if v in ecken else fremd).add(n)
    return sorted(an - fremd)


def original():
    return KB.teile(BF.lade_em30())[2]


def krallen_flaechen(m, name):
    """-> (alle Flaechen der Doppelpyramide, Flaechen an der Spitze) als Listen der Dreiecke (n0, v0, n1, v1, n2, v2)."""
    sp, ring = KRALLEN[name]
    alle = [f for f in m['tf'] if set(f[1::2]) & set(ring)]
    ecken = {v for f in alle for v in f[1::2]}
    assert len(alle) == 6 and len(ecken) == 5 and sp in ecken and not any(set(f[1::2]) & ecken for f in m['qf']), 'Kralle %s' % name
    return alle, [f for f in alle if sp in f[1::2]]


def krallen_normalen(m, name):
    """Regel V7 (v7hand/hand_bau.normalen_neu): jede Normale einer Flaeche an der Spitze = Mittel der Einheits-Flaechennormalen ihrer
    Flaechen (Wicklung negiert), Laenge 4096."""
    P = np.array([v[:3] for v in m['v']], float)
    alle, unten = krallen_flaechen(m, name)
    for n in sorted({n for f in unten for n in f[0::2]}):
        s = np.zeros(3)
        for f in alle:
            if n in f[0::2]:
                vi = f[1::2]
                c = -np.cross(P[vi[1]] - P[vi[0]], P[vi[2]] - P[vi[0]])
                s += c / np.linalg.norm(c)
        s = s / np.linalg.norm(s) * 4096
        m['n'][n] = [int(round(x)) for x in s] + [m['n'][n][3]]


def _norm(N):
    N = np.asarray(N, float)
    return N / np.maximum(np.linalg.norm(N, axis=1), 1e-9)[:, None] * 4096.0


def _schreiben(m, P, N):
    assert len(P) == len(m['v']) and len(N) <= len(m['n'])
    for i, p in enumerate(np.rint(P).astype(int)):
        m['v'][i] = [int(p[0]), int(p[1]), int(p[2]), m['v'][i][3]]
    for i, n in enumerate(np.rint(N).astype(int)):
        m['n'][i] = [int(n[0]), int(n[1]), int(n[2]), m['n'][i][3]]


def hand_ecken(om, c):
    """Original-Mesh 12 -> (Ecken, Normalen) nach hand_v8.json "hand"."""
    P = np.array([v[:3] for v in om['v']], float)
    N = np.array([v[:3] for v in om['n']], float)
    P0 = P.copy()
    for name, k in sorted(c.get('krallen', {}).items()):           # Laenge wie V7 (hand_bau): nur die Spitzenecke entlang Ring-Mitte -> Spitze
        sp, ring = KRALLEN[name]
        m = P0[ring].mean(0)
        a = P0[sp] - m
        if k.get('nach'):                                          # Richtung anteilig zur Achse einer Nachbarkralle gedreht (gegen das Kreuzen)
            sp2, ring2 = KRALLEN[k['nach']]
            b = P0[sp2] - P0[ring2].mean(0)
            e = (1.0 - k['anteil']) * a / np.linalg.norm(a) + k['anteil'] * b / np.linalg.norm(b)
            a = e / np.linalg.norm(e) * np.linalg.norm(a)
        P[sp] = m + k['faktor'] * a
        if k.get('ziel') is not None:                              # Runde 7a (K3): Spitze an einen festen Ort (Original-Mass, vor "skala"),
            P[sp] = np.array(k['ziel'], float)                     # damit die drei Fingerkrallen als gemeinsamer Keil enden
        if k.get('ring', 1.0) != 1.0:                              # Runde 6c: Wurzelring der Kralle um seine Mitte geweitet (Dicke an der Wurzel)
            P[ring] = m + k['ring'] * (P0[ring] - m)
        if k.get('wurzel_schub') is not None:                      # Runde 7a (K3): Wurzelring und innere Spitze der Doppelpyramide verschoben
            innen = sorted({v for f in om['tf'] for v in f[1::2] if set(f[1::2]) & set(ring)} - set(ring) - {sp})
            P[list(ring) + innen] += np.array(k['wurzel_schub'], float)
    for g in c.get('gruppen', []):                                 # Runde 7a (K3): eine Eckengruppe (z.B. der Daumen) um einen Punkt "um"
        idx = list(g['ecken'])                                     # (Original-Mass) je Achse skaliert, um z gedreht (Ebene x/y = Handflaeche)
        L = rz(g.get('dreh_z', 0.0)) @ np.diag(np.array(g.get('skala', [1.0, 1.0, 1.0]), float))   # und verschoben; Normalen, die nur
        if g.get('achse') is not None:                             # Runde 7a (K3): statt je Achse entlang einer eigenen Achse "achse" (Original-
            a_ = np.array(g['achse'], float) / np.linalg.norm(g['achse'])   # Mass, z.B. Daumen-Wurzel -> Daumenspitze) mit "laengs" und quer
            L = g.get('laengs', 1.0) * np.outer(a_, a_) + g.get('quer', 1.0) * (np.eye(3) - np.outer(a_, a_))   # dazu mit "quer" skaliert
        um = np.array(g['um'], float)                              # an Ecken der Gruppe haengen, mit der Inverstransponierten
        P[idx] = (P[idx] - um) @ L.T + um + np.array(g.get('schub', [0.0, 0.0, 0.0]), float)
        for n in _normalen_nur(om_flaechen(c), set(idx)):
            N[n] = np.linalg.inv(L).T @ N[n]
    for i, (y, f) in sorted((int(k), v) for k, v in c.get('wurzelring', {}).items()):   # Ecken des Handansatzes: neue Hoehe y, Radialfaktor f
        P[i] = (P[i, 0] * f, y, P[i, 2] * f)
    for e in c.get('ebnen', []):                                   # Ecke auf dem kuerzesten Weg in ihrer Hoehe y in die Ebene dreier Ecken
        a, b, d = (P[j] for j in e['ebene'])
        n = np.cross(b - a, d - a)
        n /= np.linalg.norm(n)
        nh = np.array([n[0], 0.0, n[2]])
        P[e['ecke']] = P[e['ecke']] - np.dot(P[e['ecke']] - a, n) / np.dot(nh, n) * nh
    S = np.array(c['skala'], float)
    R = ry(c['dreh_grad'])
    return (P * S) @ R.T + np.array([0.0, c.get('schub_y', 0.0), 0.0]), _norm((N / S) @ R.T)


def _stuetz(verlauf, y):
    ya = [k[0] for k in verlauf]
    return float(np.interp(y, ya, [k[1] for k in verlauf])), float(np.interp(y, ya, [k[2] for k in verlauf]))


def unterarm_ecken(om, c):
    """Original-Mesh 11 -> (Ecken, Normalen) nach hand_v8.json "unterarm"."""
    G = np.array(c['grund'], float)
    P = np.array([v[:3] for v in om['v']], float) * G
    N = _norm(np.array([v[:3] for v in om['n']], float) / G)
    P2, N2 = P.copy(), N.copy()
    for i, (x, y, z) in enumerate(P):
        y2, f = _stuetz(c['verlauf'], y)
        ya, fa = _stuetz(c['verlauf'], y - 1.0)
        yb, fb = _stuetz(c['verlauf'], y + 1.0)
        dy, df = (yb - ya) / 2.0, (fb - fa) / 2.0
        P2[i] = (f * x, y2, f * z)
        J = np.array([[f, df * x, 0.0], [0.0, max(dy, 1e-3), 0.0], [0.0, df * z, f]])
        N2[i] = np.linalg.inv(J).T @ N[i]
    return P2, _norm(N2)


def anwenden(ms, t, cfg, prot):
    """ms: Mesh-Liste (KB.teile), t: TIM (unveraendert), cfg: hand_v8.json. Aendert nur Ecken und Normalen von Mesh 11 und 12."""
    if isinstance(cfg, str):
        cfg = json.load(open(cfg, encoding='utf-8'))
    o = original()
    p = dict(quelle='hand_v8.json')
    for nr, fn, key in ((HAND, hand_ecken, 'hand'), (UNTERARM, unterarm_ecken, 'unterarm')):
        m, om = ms[nr], o[nr]
        assert [list(f) for f in m['tf']] == [list(f) for f in om['tf']] and [list(f) for f in m['qf']] == [list(f) for f in om['qf']], \
            'Mesh %d: Flaechen weichen vom Original ab' % nr
        assert len(m['v']) == len(om['v']) and len(m['n']) >= len(om['n'])
        vor = np.array([v[:3] for v in m['v']], float)
        P, N = fn(om, cfg[key])
        _schreiben(m, P, N)
        if nr == HAND:
            for name in sorted(cfg[key].get('krallen', {})):
                krallen_normalen(m, name)
        nach = np.array([v[:3] for v in m['v']], float)
        p[key] = dict(cfg=cfg[key], ecken=len(P), ausdehnung_vorher=(vor.max(0) - vor.min(0)).tolist(),
                      ausdehnung_nachher=(nach.max(0) - nach.min(0)).tolist(),
                      ausdehnung_original=np.ptp(np.array([v[:3] for v in om['v']], float), axis=0).tolist())
    prot['hand'] = p
    return p
