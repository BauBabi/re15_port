#!/usr/bin/env python3
"""K5g2 Runde 2, E1 Weg (b'): Unterteilung der Flaechen von Kopf 22 / Unterkiefer 23 / Brust 10 / Hals 21, an denen Maler-Reihenfolge und
Tiefe auseinanderfallen (Keil-Pixel). Gestalt bleibt (neue Ecken liegen auf den alten Kanten, ganzzahlig gerundet), nur die Dreiecke werden
kleiner, damit die Mitteltiefe je Dreieck (render_pc.c:1170 Einfuegesortierung nach s_textri_depth, main.c npc_avg_z = (wz0+wz1+wz2)/3)
naeher an der Tiefe je Bildpunkt liegt.

Begruendung (FARBE.md K5g2 Runde 2): Weg (a) allein laesst in Clip 10/12 die Keile stehen, weil dort die Animation den Kopfkern in die Brust
zieht (Runde 1: Ecken 179-190 wandern 380-510 Einheiten, verletzt "Gesicht unveraendert" und "Kopfgroesse bleibt"); Weg (b) "Vorderflaeche
von Buckel/Hals zuruecknehmen" (STREIFEN_HERKUNFT 5.1) wuerde den Buckel-Umriss in Bild 1/2 aendern. Die Unterteilung aendert keinen Umriss.

Regeln:
 - Viereck mit geteilter Kante -> seine zwei Port-Dreiecke (0,1,3), (0,3,2) (dieselben, die der Port zeichnet, form_buehne.dreiecke).
 - Rot-Gruen-Verfeinerung ohne T-Kreuzung: Dreieck mit 2 oder 3 geteilten Kanten -> alle 3 (4 Kinder); mit 1 -> 2 Kinder; geteilt wird jede
   Kante beidseitig (keine Risse). Neue Ecke = ganzzahlige Mitte (Abrunden zur 0 wie >> 1 der Summe nicht, sondern np.floor((a+b)/2)).
 - UV der Kinder = Mitte der Eltern-UV (abgerundet), CLUT/Seite geerbt; Normale der Mitte = normierte Summe (x 4096).
 - Rueckgabe zusaetzlich die Abstammung: neuer Schluessel -> Schluessel vor der Unterteilung (fuer Textur-Riegel).
"""
import numpy as np


def _mitte_ecke(m, cache, a, b):
    k = (min(a, b), max(a, b))
    if k not in cache:
        pa, pb = np.array(m['ecken'][a], np.int64), np.array(m['ecken'][b], np.int64)
        m['ecken'].append([int(x) for x in np.floor((pa + pb) / 2.0)])
        cache[k] = len(m['ecken']) - 1
    return cache[k]


def _mitte_normale(m, cache, a, b):
    k = (min(a, b), max(a, b))
    if a == b:
        return a
    if k not in cache:
        na, nb = np.array(m['normalen'][a], float), np.array(m['normalen'][b], float)
        s = na + nb
        n = np.linalg.norm(s)
        v = (s / n * 4096.0) if n > 1e-9 else na
        m['normalen'].append([int(x) for x in np.rint(v)])
        cache[k] = len(m['normalen']) - 1
    return cache[k]


def unterteilen(form, auswahl, herkunft=None):
    """auswahl: {teil: set((viereck, fi))} - Flaechen, deren 3 Kanten geteilt werden. herkunft: {(teil, v, fi): urschluessel} oder None.
    Rueckgabe neue herkunft (fuer alle Flaechen der betroffenen Teile; unbetroffene Teile unveraendert)."""
    neu_herkunft = dict(herkunft or {})
    for teil in sorted(auswahl):
        m = form['meshes'][teil]
        hk = {}
        for vi, art in ((0, 'dreiecke'), (1, 'vierecke')):
            for fi in range(len(m[art])):
                hk[(vi, fi)] = (herkunft or {}).get((teil, vi, fi), (teil, vi, fi))
        # Dreiecks-Liste: (e[3], n[3], uv[3], clut, page, ur, aus_viereck)
        tris = []
        quads = []
        for fi, d in enumerate(m['dreiecke']):
            tris.append([list(d['e']), list(d['n']), [list(x) for x in d['uv']], int(d['clut']), int(d['page']), hk[(0, fi)], (0, fi)])
        kanten = set()

        def kanten_von(e):
            return [(min(e[0], e[1]), max(e[0], e[1])), (min(e[1], e[2]), max(e[1], e[2])), (min(e[2], e[0]), max(e[2], e[0]))]
        sel = auswahl[teil]
        for (vi, fi) in sel:
            if vi == 0:
                kanten.update(kanten_von(m['dreiecke'][fi]['e']))
            else:
                q = m['vierecke'][fi]['e']
                for t in ((0, 1, 3), (0, 3, 2)):
                    kanten.update(kanten_von([q[i] for i in t]))
        # Vierecke mit geteilter Kante -> 2 Dreiecke
        for fi, q in enumerate(m['vierecke']):
            e = q['e']
            qk = set()
            for t in ((0, 1, 3), (0, 3, 2)):
                qk.update(kanten_von([e[i] for i in t]))
            if qk & kanten or (1, fi) in sel:
                for t in ((0, 1, 3), (0, 3, 2)):
                    tris.append([[e[i] for i in t], [q['n'][i] for i in t], [list(q['uv'][i]) for i in t], int(q['clut']), int(q['page']),
                                 hk[(1, fi)], (1, fi)])
            else:
                quads.append((q, hk[(1, fi)]))
        # Abschluss: Dreieck mit >= 2 geteilten Kanten -> alle 3
        while True:
            dazu = set()
            for t in tris:
                kk = kanten_von(t[0])
                n = sum(1 for k in kk if k in kanten)
                if n == 2:
                    dazu.update(kk)
            dazu -= kanten
            if not dazu:
                break
            kanten |= dazu
        ce, cn = {}, {}
        neu_tris = []
        for e, nn, uv, clut, page, ur, _alt in tris:
            kk = kanten_von(e)
            geteilt = [k in kanten for k in kk]
            if not any(geteilt):
                neu_tris.append((e, nn, uv, clut, page, ur))
                continue
            me = [_mitte_ecke(m, ce, e[i], e[(i + 1) % 3]) for i in range(3)]
            mn = [_mitte_normale(m, cn, nn[i], nn[(i + 1) % 3]) for i in range(3)]
            mu = [[int(np.floor((uv[i][0] + uv[(i + 1) % 3][0]) / 2.0)), int(np.floor((uv[i][1] + uv[(i + 1) % 3][1]) / 2.0))] for i in range(3)]
            if all(geteilt):
                # (a, ab, ca), (ab, b, bc), (ca, bc, c), (ab, bc, ca)
                kinder = [((e[0], me[0], me[2]), (nn[0], mn[0], mn[2]), (uv[0], mu[0], mu[2])),
                          ((me[0], e[1], me[1]), (mn[0], nn[1], mn[1]), (mu[0], uv[1], mu[1])),
                          ((me[2], me[1], e[2]), (mn[2], mn[1], nn[2]), (mu[2], mu[1], uv[2])),
                          ((me[0], me[1], me[2]), (mn[0], mn[1], mn[2]), (mu[0], mu[1], mu[2]))]
            else:
                i = geteilt.index(True)
                a, b, c = i, (i + 1) % 3, (i + 2) % 3
                kinder = [((e[a], me[i], e[c]), (nn[a], mn[i], nn[c]), (uv[a], mu[i], uv[c])),
                          ((me[i], e[b], e[c]), (mn[i], nn[b], nn[c]), (mu[i], uv[b], uv[c]))]
            for ke, kn, ku in kinder:
                neu_tris.append((list(ke), list(kn), [list(x) for x in ku], clut, page, ur))
        m['dreiecke'] = [dict(e=e, n=nn, uv=uv, clut=clut, page=page) for e, nn, uv, clut, page, _ur in neu_tris]
        m['vierecke'] = [q for q, _ur in quads]
        for fi, t in enumerate(neu_tris):
            neu_herkunft[(teil, 0, fi)] = t[5]
        for fi, (_q, ur) in enumerate(quads):
            neu_herkunft[(teil, 1, fi)] = ur
    return neu_herkunft
