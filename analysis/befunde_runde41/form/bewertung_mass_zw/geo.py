#!/usr/bin/env python3
"""Bewertung Linse MESSUNG: Geometrie-Pruefung der drei Kandidaten. Liest nur, schreibt nur nach bewertung_mass_zw.

Y zeigt nach unten, tiefster Punkt = y 0. Alle Werte Spieleinheiten, Aktorraum.
"""
import json, math, os, sys
import numpy as np
FORM = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form'
TOOLS = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/re15_port/tools'
sys.path.insert(0, TOOLS)
import form_emd as FE

NAMEN = {
 'a': ['becken','huefte_r','huefte_l','fr_ober','fr_unter','fr_fuss','fl_ober','fl_unter','fl_fuss','rumpf_mitte','rumpf','nl_ober','nl_unter','nl_hand','nl_innen','nl_daumen','nr_ober','nr_unter','nr_hand','nr_daumen','hals','kopf','kiefer'],
 'b': ['rumpf_f','fl_ober','fl_unter','fl_fuss','fl_zeh','fr_ober','fr_unter','fr_fuss','fr_zeh','rumpf_m','rumpf','nl_ober','nl_unter','nl_faust','nl_innen','maul_l','nr_ober','nr_unter','nr_faust','nr_innen','maul_r'],
 'c': ['rumpf_h','fr_huefte','fr_bein','fr_fuss','fr_zeh','fl_huefte','fl_bein','fl_fuss','fl_zeh','rumpf_m','rumpf_v','nr_ober','nr_unter','nr_hand','hals','vorsprung','nl_ober','nl_kugel_o','nl_kugel_u','nl_unter','kiefer'],
}
# Bein-Enden: Knochen, deren tiefster Punkt je Pose der Bodenkontakt ist
BEINE = {
 'a': dict(FR=[5], FL=[8], NL=[13,15], NR=[18,19], NL_innen=[14]),
 'b': dict(FR=[7,8], FL=[3,4], NL=[13], NR=[18], NL_innen=[14], NR_innen=[19]),
 'c': dict(FR=[3,4], FL=[7,8], NL=[18], NR=[13], NL_innen=[19]),
}


def lade(k):
    d = os.path.join(FORM, 'kandidat_' + k)
    emd = open(d + '/FORM.EMD', 'rb').read()
    f = FE.lese_emd(emd)
    return f, emd


def lage(k):
    if k == 'b':
        j = json.load(open(FORM + '/bewertung_mass_zw/b_eingabe.json'))
    else:
        j = json.load(open(FORM + '/kandidat_%s/lage.json' % k))
    return j


def mesh_arrays(f, b):
    m = f['meshes'][b]
    v = np.array(m['ecken'], float) if m['ecken'] else np.zeros((0, 3))
    n = np.array(m['normalen'], float) if m['normalen'] else np.zeros((0, 3))
    tri, ntri = [], []
    for d in m['dreiecke']:
        tri.append(d['e'])
        ntri.append(d['n'])
    for q in m['vierecke']:
        a, b_, c, d = q['e']
        na, nb, nc, nd = q['n']
        tri.append([a, b_, d]); ntri.append([na, nb, nd])
        tri.append([a, d, c]); ntri.append([na, nd, nc])
    return v, n, np.array(tri, int).reshape(-1, 3), np.array(ntri, int).reshape(-1, 3)


def pose(f, ci):
    R, T = FE.pose_aus_bild(f, f['clips'][ci]['bilder'][0])
    return [r.astype(float) / 4096.0 for r in R], [t.astype(float) for t in T]


def welt(f, ci, b):
    R, T = pose(f, ci)
    v = mesh_arrays(f, b)[0]
    return v @ R[b].T + T[b]


def weg_winkel(Ra, Rb):
    Q = Ra.T @ Rb
    c = max(-1.0, min(1.0, (np.trace(Q) - 1) / 2))
    return math.degrees(math.acos(c)), Q


def lokale(f, ci):
    R, T = pose(f, ci)
    L = []
    for i, k in enumerate(f['knochen']):
        p = k['eltern']
        L.append(R[i] if p < 0 else R[p].T @ R[i])
    return L, R, T


def bericht(k):
    f, emd = lade(k)
    nm = NAMEN[k]
    kn = f['knochen']
    aus = {}
    print('\n================= Kandidat', k.upper())
    wurzeln = [i for i, x in enumerate(kn) if x['eltern'] < 0]
    print('Knochen', len(kn), 'Meshes', len(f['meshes']), 'Wurzeln', wurzeln, 'Clips', len(f['clips']), [len(c['bilder']) for c in f['clips']])
    s = FE.sektionen(emd)
    print('Sektionen Bytes', {a: len(b) for a, b in s.items()})
    for ci in range(3):
        b = f['clips'][ci]['bilder'][0]
        kk = f['pool'][b['kf']]
        print('Clip', ci, 'wurzel', kk['wurzel'], 'spur', kk['spur'])
    print('\nKnochen: name eltern versatz |versatz| ecken flaechen')
    for i, x in enumerate(kn):
        m = f['meshes'][i]
        print('%2d %-11s eltern %-11s versatz %-22s %6.0f  E %3d  F %3d' % (i, nm[i], nm[x['eltern']] if x['eltern'] >= 0 else '-', x['versatz'], np.linalg.norm(x['versatz']), len(m['ecken']), len(m['dreiecke']) + len(m['vierecke'])))
    # Gesamtmasse je Pose
    print('\nMasse je Pose (Aktorraum, x laengs, z quer, h = -y):')
    for ci in range(3):
        P = np.vstack([welt(f, ci, b) for b in range(len(kn)) if len(f['meshes'][b]['ecken'])])
        print(' Clip %d: x %6.0f..%6.0f (%5.0f)  z %6.0f..%6.0f (%5.0f)  h %6.0f..%6.0f' % (ci, P[:, 0].min(), P[:, 0].max(), np.ptp(P[:, 0]), P[:, 2].min(), P[:, 2].max(), np.ptp(P[:, 2]), -P[:, 1].max(), -P[:, 1].min()))
    # Bodenkontakt je Bein
    print('\nFussboden je Pose: Hoehe des tiefsten Punkts je Bein (0 = Boden, + schwebt, - steckt im Boden), Lage x,z im Aktorraum')
    kont = {}
    for ci in range(3):
        z = []
        for nmb, bs in BEINE[k].items():
            P = np.vstack([welt(f, ci, b) for b in bs])
            i = P[:, 1].argmax()
            z.append('%s %+5.0f (%6.0f,%6.0f)' % (nmb, -P[i, 1], P[i, 0], P[i, 2]))
            kont.setdefault(nmb, []).append(float(-P[i, 1]))
        print(' Clip %d: ' % ci + '  '.join(z))
    aus['kontakt'] = kont
    # Gelenkwinkel
    print('\nGelenke: Drehung lokal gegen Clip 0 (Grad)  Clip1  Clip2 | Betrag gegen Bindpose Clip0 Clip1 Clip2')
    L0, R0, T0 = lokale(f, 0)
    L1, R1, T1 = lokale(f, 1)
    L2, R2, T2 = lokale(f, 2)
    gel = []
    for i in range(len(kn)):
        a1 = weg_winkel(L0[i], L1[i])[0]
        a2 = weg_winkel(L0[i], L2[i])[0]
        b0 = weg_winkel(np.eye(3), L0[i])[0]
        b1 = weg_winkel(np.eye(3), L1[i])[0]
        b2 = weg_winkel(np.eye(3), L2[i])[0]
        gel.append((nm[i], a1, a2, b0, b1, b2))
        print(' %2d %-11s  P1>P2 %6.1f   P1>Ruhe %6.1f   | Bind %6.1f %6.1f %6.1f' % (i, nm[i], a1, a2, b0, b1, b2))
    aus['gelenke'] = gel
    print(' Summe Gelenkdrehung P1>P2 %.0f, Median %.1f, Knochen ueber 15 Grad: %d, ueber 40 Grad: %d' % (sum(x[1] for x in gel), np.median([x[1] for x in gel]), sum(x[1] > 15 for x in gel), sum(x[1] > 40 for x in gel)))
    return f, aus


if __name__ == '__main__':
    for k in 'abc':
        bericht(k)
