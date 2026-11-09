"""Runde 3, Pruefer-Befund 11: Liegen die 21 Flaechen der Zone 30 "geoeffnetes Maul" (und 23|0|3-6) IM Maul oder AUSSEN am Kopf?
Kamera-unabhaengig im Modellraum gemessen:
 - Offenheit: Anteil von 64 Strahlen (Halbkugel um die Aussen-Normale, cos-gewichtet, Start 2 Einheiten vor der Flaechenmitte),
   die kein Dreieck treffen; einmal nur gegen Kopf 22 + Unterkiefer 23 (= was eine Maulhoehle bildet), einmal gegen das ganze Modell.
 - Mittelstrahl: was liegt direkt vor der Flaeche (Teil, Abstand).
 - Lage im Knochenraum (Teil 22: x = Schnauzenrichtung, y = unten positiv, z = Seite).
 - Nachbarn ueber gemeinsame Kanten (gleiche Eckenlage im Teil) und deren Zone in zonen_v2.json.
Aussen = -Kreuzprodukt (s_schnitt.py: 2906/3199 sichtbare Dreiecke). Nur Messung."""
import json
from collections import Counter, defaultdict
from s_basis import *  # noqa: F401,F403
import material_lesen as ML

import subprocess
# Die Pruefer-Befunde r2 beziehen sich auf zonen_v2.json im Stand 41585c197 (Zone 30 mit den 21 Flaechen inkl. der 6 Hals-Streifen-Flaechen).
# Die Grundlage Runde 3 (c8fbee2f6) hat Zone 30 inzwischen neu belegt; gemessen wird hier der gepruefte Stand.
ZONEN_STAND = '41585c197'
ZONEN = json.loads(subprocess.run(['git', '-C', WT, 'show', ZONEN_STAND + ':analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/zonen_v2.json'],
                                  capture_output=True, check=True).stdout.decode('utf-8'))['zonen']
ZONE_VON = {}
for z in ZONEN:
    for f in z['modell_zuordnung']['liste']:
        if f.split('|')[0] in ('22', '23'):
            ZONE_VON.setdefault(f, []).append(z['nr'])
SECHS = ['22|1|1', '22|1|8', '22|1|9', '22|1|194', '22|1|201', '22|1|209']
Z30 = next(z for z in ZONEN if z['nr'] == '30')['modell_zuordnung']['liste']
KIEFER_UNTEN = ['23|0|3', '23|0|4', '23|0|5', '23|0|6']


def zone_liste(nr):
    return next(z for z in ZONEN if z['nr'] == nr)['modell_zuordnung']['liste']


def richtungen(n=64):
    i = np.arange(n) + 0.5
    r = np.sqrt(i / n); phi = i * np.pi * (3 - np.sqrt(5))
    return np.stack([r * np.cos(phi), r * np.sin(phi), np.sqrt(1 - r * r)], 1)   # cos-gewichtet um +z


HK = richtungen()


def flaechen(mo, kf):
    """je Flaeche: schluessel -> (Ecken Modellraum, Aussen-Normale, Mitte, Eckennummern). Dazu alle Dreiecke mit Teil und Schluessel."""
    R, T = mo.pose(kf)
    F = {}; tris = []; teil = []; schl = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        m = mo.meshes[b]
        for (verts, faces, viereck) in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], dtype=np.int64)
            q = (((R[b].astype(np.int64) @ v.T) >> 12).T + T[b].astype(np.int64)).astype(float)
            for fi, g in enumerate(faces):
                ie = list(g[1::2])
                hs = ((0, 1, 3), (0, 3, 2)) if viereck else ((0, 1, 2),)
                n = np.zeros(3)
                for h in hs:
                    A, B, C = q[[ie[h[0]], ie[h[1]], ie[h[2]]]]
                    n -= np.cross(B - A, C - A)
                    tris.append((A, B, C)); teil.append(b); schl.append('%d|%d|%d' % (b, viereck, fi))
                n /= max(np.linalg.norm(n), 1e-9)
                F['%d|%d|%d' % (b, viereck, fi)] = (q[ie], n, q[ie].mean(0), ie)
    return F, np.array(tris), np.array(teil), np.array(schl)


def strahlen(O, D, tris, eps=1e-7):
    """O, D (n x 3) gegen tris (m x 3 x 3): naechstes t (inf = frei) und Index."""
    A, B, C = tris[:, 0], tris[:, 1], tris[:, 2]
    E1 = B - A; E2 = C - A
    best = np.full(len(O), np.inf); idx = np.full(len(O), -1)
    for s in range(0, len(O), 128):
        o = O[s:s + 128, None]; d = D[s:s + 128, None]
        p = np.cross(d, E2[None]); det = (E1[None] * p).sum(-1)
        ok = np.abs(det) > eps; inv = np.where(ok, 1 / np.where(ok, det, 1), 0)
        Tv = o - A[None]; u = (Tv * p).sum(-1) * inv
        qq = np.cross(Tv, E1[None]); v = (d * qq).sum(-1) * inv; t = (E2[None] * qq).sum(-1) * inv
        hit = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (t > 1e-6)
        t = np.where(hit, t, np.inf)
        j = t.argmin(1); bb = t[np.arange(len(j)), j]
        best[s:s + 128] = bb; idx[s:s + 128] = np.where(np.isfinite(bb), j, -1)
    return best, idx


def basis(n):
    a = np.array([1.0, 0, 0]) if abs(n[0]) < 0.9 else np.array([0, 1.0, 0])
    u = np.cross(n, a); u /= np.linalg.norm(u); w = np.cross(n, u)
    return np.stack([u, w, n], 1)


def offenheit(F, tris, teil, schl, liste, nur=None):
    sel = np.ones(len(tris), bool) if nur is None else np.isin(teil, nur)
    T_ = tris[sel]; S_ = schl[sel]; Te = teil[sel]
    O = []; D = []
    for f in liste:
        _q, n, c, _ie = F[f]
        d = HK @ basis(n).T
        O.append(np.repeat((c + 2 * n)[None], len(d), 0)); D.append(d)
    O = np.concatenate(O); D = np.concatenate(D)
    t, j = strahlen(O, D, T_)
    frei = ~np.isfinite(t)
    out = {}
    for k, f in enumerate(liste):
        s = slice(k * len(HK), (k + 1) * len(HK))
        tr = j[s][~frei[s]]
        out[f] = dict(offen=float(frei[s].mean()), getroffen=dict(Counter(int(Te[x]) for x in tr).most_common(3)))
    O2 = np.array([F[f][2] + 2 * F[f][1] for f in liste]); D2 = np.array([F[f][1] for f in liste])
    t2, j2 = strahlen(O2, D2, T_)
    for k, f in enumerate(liste):
        out[f]['vorn'] = None if j2[k] < 0 else dict(teil=int(Te[j2[k]]), flaeche=str(S_[j2[k]]), abstand=round(float(t2[k]), 1))
    return out


def kieferwinkel(mo, kf):
    R, _ = mo.pose(kf)
    M = (R[22].astype(float).T @ R[23].astype(float)) / 4096.0 / 4096.0
    return float(np.degrees(np.arccos(np.clip((np.trace(M) - 1) / 2, -1, 1))))


def ring_lagen(mo, f):
    b, q, i = map(int, f.split('|')); mm = mo.meshes[b]
    g = (mm.qf if q else mm.tf)[i]; vv = mm.qv if q else mm.tv
    L = [tuple(vv[x][:3]) for x in g[1::2]]
    return b, ([L[0], L[1], L[3], L[2]] if q else L)


def nachbarn(mo, liste):
    kante = defaultdict(set)
    for f in alle_flaechen(mo):
        b, L = ring_lagen(mo, f)
        if b not in (22, 23):
            continue
        for e in range(len(L)):
            kante[(b,) + tuple(sorted((L[e], L[(e + 1) % len(L)])))].add(f)
    out = {}
    for f in liste:
        b, L = ring_lagen(mo, f)
        nb = set()
        for e in range(len(L)):
            nb |= kante[(b,) + tuple(sorted((L[e], L[(e + 1) % len(L)])))] - {f}
        out[f] = sorted(nb)
    return out


if __name__ == '__main__':
    k = kand(ASSET); mo = k.mo
    mat = ML.material_je_flaeche()
    belegt = set(ZONE_VON)
    gruppen = {
        'Zaehne (Zone 41)': zone_liste('41'),
        'Lippe (Zone 42)': zone_liste('42'),
        'Unterkiefer aussen (Zone 31)': zone_liste('31'),
        'Kopf 22 Haut (nicht in Zonen 29/30/40/41/42)': [f for f in alle_flaechen(mo) if f.startswith('22|') and not set(ZONE_VON.get(f, [])) & {'29', '30', '40', '41', '42'}],
    }
    kw = []
    for c, (f0, nn) in enumerate(mo.clips):
        for b in range(nn):
            kw.append((c, b, round(kieferwinkel(mo, k.kf(c, b)), 2)))
    kwv = np.array([x[2] for x in kw])
    posen = {'Bild-2-Pose Clip 12/0': (12, 0), 'Spiel-Griff Clip 27/20': (27, 20), 'Ruhe Clip 2/0': (2, 0)}
    imax = int(kwv.argmax()); imin = int(kwv.argmin())
    posen['Maul am weitesten offen Clip %d/%d' % kw[imax][:2]] = kw[imax][:2]
    posen['Maul am weitesten zu Clip %d/%d' % kw[imin][:2]] = kw[imin][:2]
    erg = dict(kieferwinkel=dict(min=float(kwv.min()), median=float(np.median(kwv)), max=float(kwv.max()), bilder=len(kw),
                                 je_pose={p: round(kieferwinkel(mo, k.kf(*cb)), 2) for p, cb in posen.items()}),
               posen={}, gruppen={}, lage={}, nachbarn={})
    for f in Z30 + KIEFER_UNTEN:
        b, q, i = map(int, f.split('|')); mm = mo.meshes[b]
        g = (mm.qf if q else mm.tf)[i]; vv = mm.qv if q else mm.tv
        P_ = np.array([vv[x][:3] for x in g[1::2]], float)
        erg['lage'][f] = dict(mitte_knochenraum=[int(round(x)) for x in P_.mean(0)], material=mat.get(f, '?'),
                              zeile=int(((mm.quv if q else mm.tuv)[i][2] >> 6) & 0x1FF), zonen=ZONE_VON.get(f, []))
    nb = nachbarn(mo, Z30 + KIEFER_UNTEN)
    erg['nachbarn'] = {f: [(x, ZONE_VON.get(x, []), mat.get(x, '?')) for x in v] for f, v in nb.items()}
    for p, (c, b) in posen.items():
        F, tris, teil, schl = flaechen(mo, k.kf(c, b))
        erg['posen'][p] = dict(kopf_kiefer=offenheit(F, tris, teil, schl, Z30, nur=(22, 23)),
                               ganz=offenheit(F, tris, teil, schl, Z30))
        if p.startswith('Bild-2') or p.startswith('Ruhe') or 'offen' in p:
            for gn, li in gruppen.items():
                o = offenheit(F, tris, teil, schl, li, nur=(22, 23))
                w = np.array([v['offen'] for v in o.values()])
                erg['gruppen'].setdefault(gn, {})[p] = dict(n=len(w), median=round(float(np.median(w)), 3), p10=round(float(np.percentile(w, 10)), 3),
                                                             p90=round(float(np.percentile(w, 90)), 3))
    json.dump(erg, open(ZIEL + '/messung/r3_lage.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    print(json.dumps(erg['kieferwinkel'], indent=1))
    print(json.dumps(erg['gruppen'], indent=1))
    for p in erg['posen']:
        print(p)
        for f in Z30:
            a = erg['posen'][p]['kopf_kiefer'][f]; g = erg['posen'][p]['ganz'][f]
            print('  %-9s offen kopf+kiefer %.2f ganz %.2f  vorn %s  getroffen %s' % (f, a['offen'], g['offen'], g['vorn'], g['getroffen']))
    for f in Z30 + KIEFER_UNTEN:
        print(f, erg['lage'][f], [x[0] + ':' + ','.join(x[1]) + ':' + x[2] for x in erg['nachbarn'][f]])
