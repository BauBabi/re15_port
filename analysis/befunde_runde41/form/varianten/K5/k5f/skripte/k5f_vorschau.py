"""K5f-VORSCHAU (nur Messung, scratch; kein Asset): kf_k5e mit ersetzter Rampe.
Haut: (L*, a*, b*) der G4-Originalrampe x f  (= Skalierung im linearen Raum fuer L* > 8: Y ~ (L*)^3, a*/b* skalieren mit), Knie ab L* 75.
hell (L* >= 52,4): wie Haut, durch das Knie gedrueckt.  Ader (RV6): L* rangtreu auf [Haut-p10', q x Haut-p50'], G4-Farbton, a* <= Haut-a*-Median'.
Gelenk: Haut x rg (L*), Buntheit x cg.  Fleisch: Haut x rf (L*), Ton hf, C* cf.  Maulinneres-Aussenflaechen (6) -> Haut.
F1-Zeile: Kern bytegleich, Haut-Eintraege = Hautzeile 3, Fleischrand = Hauttransformation."""
import sys, json, math, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator')
import numpy as np
import kf_k5e as E, kf_g4 as KG

AUSSEN_MAUL = [(22, 1, 1), (22, 1, 8), (22, 1, 9), (22, 1, 194), (22, 1, 201), (22, 1, 209)]
P = dict(cgr_max=4.8, cgr=0.41, grau=True, h=-22.9, s=0.284, kappa=0.61, hell='mal', f=2.0, knie=75.0, q=0.74, cg=0.41, rg=1.469, rf=1.51, hf=-8.0, cf=18.0, aussen_maul_haut=True, ader_c='a_haut')
STAT = {}

def knie(L, k):
    L = np.asarray(L, float)
    return np.where(L <= k, L, k + (100 - k) * np.tanh((L - k) / (100 - k)))

def ziel_haut(Lab, f, k):
    L2 = knie(Lab[:, 0] * f, k)
    s = np.where(Lab[:, 0] > 1e-6, L2 / np.maximum(Lab[:, 0], 1e-6), f)
    return np.stack([L2, Lab[:, 1] * s, Lab[:, 2] * s], 1)

def k5f_rampe(g4zeile, cnt, art, f):
    """v2: EIN Farbton je Material, Buntheit proportional zur Helligkeit (C* = s x L*: eine Chromatizitaet, Helligkeitsmuster der G4-Rampe
    multiplikativ). Haut L' = knie(f x L). Ader rangtreu [Haut-p10', q x Haut-p50'], C = kappa s L'. hell: 'mal' = knie(f x L), 'mitte' = K5e-Regel."""
    L, haut, hell, ader = E.klassen(g4zeile)
    z0 = np.array(g4zeile) != 0
    cnt = np.asarray(cnt, float)
    Ln = knie(L[:, 0] * f, P['knie'])
    hg = haut & (cnt > 0)
    if not hg.any(): hg = haut
    w_ = cnt[hg] if cnt[hg].sum() > 0 else np.ones(int(hg.sum()))
    p10 = E.gew_quantil(Ln[hg], w_, .1); p50 = E.gew_quantil(Ln[hg], w_, .5)
    if P['hell'] == 'mitte' and hell.any():
        Ln[hell] = p50 + 0.1 * (L[hell, 0] - KG.R4_L_HELL)
    ag = ader & (cnt > 0)
    if not ag.any(): ag = ader
    if ag.any():
        v0, v1 = float(L[ag, 0].min()), float(L[ag, 0].max())
        lo, hi = p10, P['q'] * p50
        Ln[ader] = np.clip(lo + (L[ader, 0] - v0) / max(v1 - v0, 1e-6) * (hi - lo), min(lo, hi), hi)
    s = np.where(ader, P['kappa'] * P['s'], P['s'])
    h = P['h']
    if art == 'gelenk':
        Ln = knie(Ln * P['rg'], P['knie']); s = s * P['cg']
    if art == 'grau':
        s = s * P['cgr']
    if art == 'fleisch':
        Ln = knie(Ln * P['rf'], P['knie']); h = P['hf']
        s = np.where(ader, P['kappa'], 1.0) * P['cf'] / np.maximum(Ln, 1e-6)      # Fleisch: C* konstant cf (K5e-Satz), L multiplikativ
    C = s * Ln
    if art == 'grau':
        C = np.minimum(C, P['cgr_max'])      # Grau (Lippe/Unterkiefer/Krallen): C* <= 4,8 = K5e-Gelenk-Buntheit ('weisslich/grau')
    cm = KG.r4_cmax(Ln, h)
    C = np.where(cm > 1e-6, np.minimum(C, cm * np.tanh(C / np.maximum(cm, 1e-6))), 0.0)
    hr = math.radians(h)
    Z = np.stack([Ln, C * math.cos(hr), C * math.sin(hr)], 1)
    rgb = KG.rgb_aus_lab(Z)
    neu = list(g4zeile)
    for i in range(256):
        if not z0[i]:
            continue
        wn = E.runde15(rgb[i], Z[i], -1 if ader[i] else 0) | (g4zeile[i] & 0x8000)
        neu[i] = wn if wn != 0 else 0x8000
    return neu, dict(haut_p10_p50=[round(p10, 2), round(p50, 2)])

def zeilen(par, F, mat, quell, cl_k5c, cl_g4):
    Z = par['zeilen']; f = P['f']
    zeile_je, nutz = {}, {}
    for k, (nm, s) in E.material_seite(F, mat, quell).items():
        if nm == 'Maulinneres' and P['aussen_maul_haut'] and k in AUSSEN_MAUL:
            nm = 'Haut'
        if nm == 'Haut' and P['grau'] and (k in GRAU or k[0] == 23): z, gr = Z['grau'][s], ('grau', s)
        elif nm == 'Haut': z, gr = Z['haut'][s], ('haut', s)
        elif nm == 'Gelenk': z, gr = Z['gelenk'][s], ('gelenk', s)
        elif nm.startswith('Fleisch'): z, gr = Z['fleisch'], ('fleisch', 1)
        elif nm in ('Auge', 'Maulinneres'): z, gr = Z['g4_0'], None
        elif nm == 'Zahn': z, gr = Z['zahn'], None
        else: z, gr = Z['f1'], None
        zeile_je[k] = z
        if gr:
            n = nutz.setdefault(gr, np.zeros(256)); np.add.at(n, F[k][2], 1)
    worte = [None] * E.NZ; info = {}
    worte[Z['g4_0']] = [int(x) for x in cl_g4[0]]
    worte[Z['zahn']] = [int(x) for x in cl_k5c[7]]
    for s in range(3):
        g4z = [int(x) for x in cl_g4[s]]
        worte[Z['haut'][s]], info['haut%d' % s] = k5f_rampe(g4z, nutz[('haut', s)], 'haut', f)
        worte[Z['gelenk'][s]], info['gelenk%d' % s] = k5f_rampe(g4z, nutz.get(('gelenk', s), np.ones(256)), 'gelenk', f)
    if P['grau']:
        for s in range(3):
            worte[Z['grau'][s]], info['grau%d' % s] = k5f_rampe([int(x) for x in cl_g4[s]], nutz.get(('grau', s), np.ones(256)), 'grau', f)
    worte[Z['fleisch']], info['fleisch1'] = k5f_rampe([int(x) for x in cl_g4[1]], nutz[('fleisch', 1)], 'fleisch', f)
    # F1: Kern bytegleich K5c-5; uebrige gelesene Eintraege: G4-Seite-2-Haut -> Hautzeile 3, sonst Hauttransformation des K5c-Worts
    z5 = [int(x) for x in cl_k5c[5]]
    kern = set(E.f1_kern_eintraege())
    px = E.FE.tim_lesen(open(os.path.join(E.BASIS, 'FORM.TIM'), 'rb').read())['pixel']
    gel = np.zeros(256, int)
    for k, v in mat.items():
        if v == 'F1': np.add.at(gel, F[k][2], 1)
    g4z2 = [int(x) for x in cl_g4[2]]
    neu = list(z5); um = [i for i in range(256) if gel[i] > 0 and i not in kern and z5[i] != 0]
    Lz = KG.r4_eintraege(z5)[0]
    for i in um:
        if z5[i] == g4z2[i]:
            neu[i] = worte[Z['haut'][2]][i]
        else:
            l2 = knie(Lz[i:i + 1, 0] * f, P['knie']); hr = math.radians(P['h'])
            t = np.stack([l2, P['s'] * l2 * math.cos(hr), P['s'] * l2 * math.sin(hr)], 1)
            neu[i] = (E.runde15(KG.rgb_aus_lab(t)[0], t[0], 0) | (z5[i] & 0x8000)) or 0x8000
    worte[Z['f1']] = neu
    info['f1'] = dict(umgefaerbt=len(um), kern=len(kern))
    STAT.update(info)
    return worte, zeile_je, info, {'%s%d' % gr: int(n.sum()) for gr, n in sorted(nutz.items())}, nutz

E.zeilen = zeilen
_mj = json.load(open(E.MATERIAL, encoding='utf-8'))['material']
GRAU = set(tuple(int(x) for x in s.split('|')) for nm in ('Lippensaum', 'Maul_Band', 'Kralle') for s in _mj[nm]['liste'])

if __name__ == '__main__':
    aus = sys.argv[1]
    for kv in sys.argv[2:]:
        k, v = kv.split('='); P[k] = type(P[k])(v) if not isinstance(P[k], bool) else v in ('1', 'True', 'true')
    par = json.load(open(r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/K5e_f1/k5e_f1_par.json', encoding='utf-8'))
    par['name'] = 'K5f-Vorschau ' + json.dumps(P)
    if P['grau']:
        E.NZ = 13; par['zeilen']['grau'] = [10, 11, 12]
    b, w, z = E.bauen(par, aus)
    json.dump(dict(P=P, stat=STAT, sha=b['sha256'], flaechen_je_zeile=b['flaechen_je_zeile']), open(aus + '/vorschau.json', 'w'), indent=1)
    print(json.dumps(dict(P=P, stat=STAT, fz=b['flaechen_je_zeile']), indent=None)[:900])
