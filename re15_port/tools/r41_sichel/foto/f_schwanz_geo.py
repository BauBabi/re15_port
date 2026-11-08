"""Schwanz-Geometrie der Variante V5r4 im Knochenraum (posenunabhaengig): je Glied 16-22 Laenge (Achse +y bis zum
Kindgelenk, skel.rel[k+1]), Radius r = sqrt(x^2 + z^2) der Ecken in Anfangs-/Mitte-/End-Zehntel, Querschnitt quer
(x) und tief (z) getrennt; Sprung an jeder Gliedgrenze = r_anfang(k+1) - r_ende(k). Wurzel: Glied 16 Anfang gegen die
Rumpfecken (Knochen 8) im Umkreis 400 um das Gelenk 16 (skel.rel[16] im Raum von 8)."""
import os, sys, json, numpy as np
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'v4'))
import v4_gemein as G, b2
S = os.path.join(G.SFV4, 'foto')


def ecken(m):
    P = [x[:3] for x in m.tv] + [x[:3] for x in m.qv]
    return np.array(P, float)


def main(pfad=G.EMD_V4, name='V5r4'):
    mo = b2.modell(pfad)
    R = {}
    for k in range(16, 23):
        P = ecken(mo.meshes[k])
        L = float(mo.skel.rel[k + 1][1]) if k < 22 else float(P[:, 1].max())
        t = P[:, 1] / L
        r = np.hypot(P[:, 0], P[:, 2])
        def zone(a, b):
            s = (t >= a) & (t <= b)
            if not s.any(): return None
            return dict(r_max=round(float(r[s].max()), 1), x=[round(float(P[s, 0].min()), 1), round(float(P[s, 0].max()), 1)],
                        z=[round(float(P[s, 2].min()), 1), round(float(P[s, 2].max()), 1)], n=int(s.sum()))
        R[k] = dict(laenge=L, ecken=len(P), t_bereich=[round(float(t.min()), 2), round(float(t.max()), 2)],
                    anfang=zone(-0.5, 0.15), mitte=zone(0.4, 0.6), ende=zone(0.85, 1.5), r_max=round(float(r.max()), 1))
    spr = {}
    for k in range(16, 22):
        a, e = R[k + 1]['anfang'], R[k]['ende']
        if a and e:
            spr['%d/%d' % (k, k + 1)] = dict(r_ende=e['r_max'], r_anfang_naechstes=a['r_max'],
                                            sprung=round(a['r_max'] - e['r_max'], 1),
                                            sprung_rel=round((a['r_max'] - e['r_max']) / max(e['r_max'], 1), 3))
    # Wurzel: Rumpfecken (8) nahe Gelenk 16
    P8 = ecken(mo.meshes[8]); j = np.array(mo.skel.rel[16], float)
    d = np.linalg.norm(P8 - j, axis=1)
    R['wurzel'] = dict(gelenk16_im_rumpf=j.tolist(), rumpfecken_im_umkreis_400=int((d < 400).sum()),
                       naechste_rumpfecke=round(float(d.min()), 1), r16_anfang=R[16]['anfang'])
    R['spruenge'] = spr
    json.dump(R, open(os.path.join(S, 'schwanz_geo_%s.json' % name), 'w'), indent=1)
    for k in range(16, 23): print(k, R[k]['laenge'], R[k]['t_bereich'], R[k]['anfang'] and R[k]['anfang']['r_max'], R[k]['mitte'] and R[k]['mitte']['r_max'], R[k]['ende'] and R[k]['ende']['r_max'])
    print(json.dumps(spr)); print(R['wurzel'])


if __name__ == '__main__':
    main()
