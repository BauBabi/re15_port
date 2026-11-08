"""SF V5 wie Foto - Koerper: Suche der Schwanz-Radien (schwanz_form.setzen) unter allen Schwanz-Riegeln zugleich.

    python schwanz_suche.py [--saat N] [--schritte N] [--start r1,r2,r3,r4,r5,r6,bt,ba,bo] [--aus JSON]

Ziel (Strafe 0 = alles mit Marge gruen):
  T-F1  Inkreis-Durchmesser je Station f 0,1-0,9 in Pose P2 gegen das Foto (v5f_gemein.foto_breiten) +-20 % (Marge: 17 %).
  T-F5  Profil ohne Huckel im Knochenraum: Steigungsknick der Mantellinie je Gelenk <= 15 Grad (Marge 13); Gelenkringe deckungsgleich
        und bauch nie dicker als Gelenk 2 sind Bauregeln (schwanz_form.setzen).
  Riegel V4 in Pose 85 (riegel_v4.schwanz / schwanz_form): T1, T2 (Lage + Drehung), T3, T4, TA, T5, TK; Marge 10 % der Toleranz.
Parameter: r1, r2, r3, r4, r5, r6 (= r7), bauch t / anteil / aussen; r0 = r1 x 315 / 281 (Verhaeltnis V4).
Kein RE-Befund (die Sichel-Form gibt es auf keiner Disc).
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v5f_gemein as F
import schwanz_form as SF
import riegel_v4 as RV
import t2_mass_v4 as T2M
import r4_mass as R4
G = F.G
LAENGE = {16: 624.0, 17: 440.0, 18: 887.0, 19: 524.0, 20: 531.0, 21: 243.0, 22: 242.0}
KNICK_MAX = 15.0
# bauch-Grenzen (Lauf 2: anteil bis 1,4 = bauch bis 40 % ueber die Gerade r2 -> r3 hinaus, solange der Steigungsknick <= 15 Grad bleibt)
BA_MAX = float(os.environ.get('V5F_BA_MAX', '1.0'))
BO_MAX = float(os.environ.get('V5F_BO_MAX', '0.05'))
NAMEN = ['r1', 'r2', 'r3', 'r4', 'r5', 'r6', 'bt', 'ba', 'bo']


def profil_knicke(rad):
    """Steigungsknick (Grad) der Mantellinie an jedem Gelenk 1..6 und am bauch (Knochenraum, Gliedlaengen der Ruhe-Kette)."""
    seg = []
    for i in range(7):
        L = LAENGE[16 + i]; a, e = rad[i], rad[i + 1]
        if i == 2 and rad.get('bauch'):
            b = rad['bauch']; t = b['t']; lin = (1 - t) * a + t * e; rb = lin + b['anteil'] * (a - lin)
            seg += [(t * L, a, rb, 'Glied 18 bis bauch'), ((1 - t) * L, rb, e, 'Glied 18 ab bauch')]
        else:
            seg.append((L, a, e, 'Glied %d' % (16 + i)))
    w = [np.degrees(np.arctan2(e - a, L)) for L, a, e, _ in seg]
    return [dict(zwischen='%s / %s' % (seg[k][3], seg[k + 1][3]), knick=round(float(abs(w[k + 1] - w[k])), 2)) for k in range(len(seg) - 1)]


def alt_riegel(mo):
    """Normierte Abweichungen (|ist - soll| / Toleranz; <= 1 = gruen) der Riegel-V4-Schwanzzeilen in Pose 85."""
    P = G.posen()
    img, ids, iv = G.zeichne(mo, P['85'], voll=True)
    ok, so, H = G.h_port(iv)
    s = RV.schwanz_mass_h(img, ids, ok, so)
    to = T2M.original(); tp = T2M.port(ids, s['H'])
    tl = R4.t2_lage_port(ids, s['H'], s['beginn'])
    v = {}
    v['T1x'] = abs(s['T1'][0] - 0.15) / 0.03; v['T1y'] = abs(s['T1'][1] - 0.767) / 0.03
    v['T2x'] = abs(tl['dx'] + 0.245) / 0.03; v['T2y'] = abs(tl['dy'] - 0.036) / 0.03
    v['T2d'] = abs((tp['drehung'] if tp else 0) - to['ziel']) / to['tol']
    v['T3x'] = abs(s['T3'][0] + 0.265) / 0.03; v['T3y'] = abs(s['T3'][1] - 0.257) / 0.03
    v['T4x'] = abs(s['T4'][0] + 0.245) / 0.03; v['T4y'] = abs(s['T4'][1] - 0.397) / 0.03
    v['T4s'] = abs(s['ende_ueber_sohle'] - 0.37) / 0.03
    for k, (p, q) in dict(B=((357, 243), s['beginn']), K=((290, 308), s['knick']), E=((295, 343), s['ende'])).items():
        v['TA' + k + 'x'] = abs(q[0] - p[0]) / s['H'] / 0.03; v['TA' + k + 'y'] = abs(q[1] - p[1]) / s['H'] / 0.03
    soll = {0.1: (0.03, 0.015), 0.3: (0.065, 0.02), 0.5: (0.125, 0.025), 0.7: (0.14, 0.025)}
    for k, (w, t) in soll.items():
        v['T5_%.1f' % k] = abs(s['T5'][k] - w) / t
    reihe = [s['T5'][k] for k in sorted(s['T5']) if 0.1 <= k <= 0.7 + 1e-9]
    v['T5ab'] = max([reihe[i] - reihe[i + 1] for i in range(len(reihe) - 1)] + [0.0]) / 0.01
    fp = R4.form_port(img, ids)
    v['TKrms'] = 3.02 / max(fp['kontur']['rms'], 1e-3)
    ko = R4.form_original()[0]['kontur']['kante']; kp_ = fp['kontur']['kante']
    v['TKkante'] = max((abs(kp_[x] - ko[x]) / H / 0.03 if kp_[x] is not None else 9.0) for x in R4.X_KANTE)
    return v


def par_rad(p):
    d = dict(zip(NAMEN, [float(x) for x in p]))
    return {0: d['r1'] * 315.0 / 281.0, 1: d['r1'], 2: d['r2'], 3: d['r3'], 4: d['r4'], 5: d['r5'], 6: d['r6'], 7: d['r6'],
            'bauch': dict(t=d['bt'], anteil=d['ba'], aussen=d['bo'])}


def gueltig(p):
    d = dict(zip(NAMEN, p))
    return (d['r1'] >= d['r2'] >= d['r3'] >= d['r4'] >= d['r6'] >= d['r5'] > 20 and 0.05 <= d['bt'] <= 0.6 and 0.0 <= d['ba'] <= BA_MAX
            and 0.0 <= d['bo'] <= BO_MAX)


def bewerte(meshes, packen, par):
    rad = par_rad(par)
    ms = SF.setzen(meshes, rad)
    mo = F.V.modell(packen(ms))
    mb, _kp = F.modell_breiten(mo, F.V.posen()['P2'])
    soll = F.foto_breiten()
    tf1 = max(abs(m['d_H'] / s['soll_H'] - 1) for m, s in zip(mb, soll) if 0.1 <= m['f'] <= 0.9)
    kn = max(k['knick'] for k in profil_knicke(rad))
    v = alt_riegel(mo)
    strafe = (max(0.0, tf1 / 0.17 - 1) ** 2 * 4 + max(0.0, kn / 13.0 - 1) ** 2 * 4
              + sum(max(0.0, x - 0.9) ** 2 for x in v.values()))
    return strafe, dict(tf1=tf1, knick=kn, alt={k: round(x, 2) for k, x in v.items()}, breiten=[m['d_H'] for m in mb])


def main():
    import kopf_orig_bau as KB
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=os.path.join(KB.AUS, 'EM30_V5k.EMD'))
    ap.add_argument('--saat', type=int, default=1); ap.add_argument('--schritte', type=int, default=150)
    ap.add_argument('--start', default='279,279,195,177,62,72,0.15,0.6,0.011'); ap.add_argument('--aus')
    a = ap.parse_args()
    b = open(a.basis, 'rb').read(); d, _do, ms, tim = KB.teile(b)
    pk = lambda m: KB.packen(b, d, m, tim)
    rng = np.random.default_rng(a.saat)
    p = np.array([float(x) for x in a.start.split(',')])
    skala = np.array([12, 12, 12, 12, 5, 5, 0.05, 0.15, 0.006])
    best, info = bewerte(ms, pk, p)
    print('start %.3f' % best, 'tf1 %.3f knick %.1f' % (info['tf1'], info['knick']), {k: v for k, v in info['alt'].items() if v > 0.9}, flush=True)
    log = []
    for it in range(a.schritte):
        q = p + rng.normal(0, 1, len(p)) * skala * (rng.random(len(p)) < 0.4)
        if not gueltig(q):
            continue
        s, inf = bewerte(ms, pk, q)
        if s < best:
            best, p, info = s, q, inf
            print(it, '%.4f' % s, [round(float(x), 3) for x in p], 'tf1 %.3f knick %.1f' % (inf['tf1'], inf['knick']),
                  {k: v for k, v in inf['alt'].items() if v > 0.9}, flush=True)
            log.append(dict(schritt=it, strafe=s, par=dict(zip(NAMEN, [float(x) for x in p])), info=inf))
        if best == 0:
            break
    print('ENDE %.4f' % best, dict(zip(NAMEN, [round(float(x), 4) for x in p])), info)
    if a.aus:
        json.dump(dict(saat=a.saat, start=a.start, ende=dict(zip(NAMEN, [float(x) for x in p])), strafe=best, info=info, verlauf=log),
                  open(a.aus, 'w'), indent=1, default=float)


if __name__ == '__main__':
    main()
