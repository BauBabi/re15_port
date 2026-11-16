#!/usr/bin/env python3
"""Riegel "Zaehne bis vorn" (Auftrag Vorderzaehne 16.11.2026). Exit 1 bei Rot. Jede Pruefung hat eine Gegenprobe, die ROT sein MUSS
(Gegenprobe gruen = STUMPF = Rot). Vorgabe-Asset: re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD/.TIM; Gegenprobe-Stand: der abgenommene
K5g3 + R9 (EMD 88c49ce1, TIM c00823b3) aus git (Commit 1ee2cdca4).

  python riegel_k5g3_zaehne.py [<EMD>] -> Protokoll K5/k5g/K5g3/zaehne_vorn/riegel_zaehne.txt (+ .json)

ZV1 Zaehne bis vorn: je Kiefer steht ein Zahn auf der Mittellinie (|z| <= 1) und keine Luecke zwischen Nachbarzaehnen der Reihe (Draufsicht,
    um die Schnauze herum) ist groesser als 1,5 x der Abstand der beiden vordersten alten Zaehne (oben O1-O2 51,2 -> 76,9; unten U1-U2
    103,3 -> 155,0)                                                                          | Gegenprobe 88c49ce1 (Mittelluecke 494 / 424)
ZV2 Nur angehaengt: Sektionen 0-6 und TIM bytegleich 88c49ce1, alle Netze ausser 22/23 gleich, in 22/23 alle alten Ecken/Normalen/
    Dreiecke/Vierecke an ihrer Stelle gleich; neu nur Dreiecke auf neuen Ecken/Normalen, jedes mit CLUT/Seite/UV eines alten Zahndreiecks;
    3 Dreiecke + 4 Ecken + 4 Normalen je Zahn                                                | Gegenprobe 88c49ce1 mit einer alten Ecke +1
ZV3 Kein Durchstich (Zahn-Scan, alle Keys, Ganzzahl-Pose): neue Zaehne liegen nie im Gegenkiefer und treten nie durch ihn (die alten oberen
    beruehren den Unterkiefer in keinem Key), kein neuer unterer steht vor der Nase ueber der Mundlinie, und kein neuer Zahn dringt tiefer in
    einen Zahn der Gegenreihe als die alten Zaehne untereinander im selben Key    | Gegenprobe: derselbe Bau ohne die R6-Kappen (Nennlaengen)
ZV4 Maler (Port-Zeichenweg, Dreiecke nach mittlerer Tiefe ohne Tiefenpuffer, 5 Nah- und 4 Spielansichten, alle Keys): Pixel, an denen der
    Maler einen neuen Zahn zeigt, der Tiefenpuffer aber den GEGENkiefer (= Zahn sichtbar durch Kinn/Schnauze/Lippe gemalt), je Ansicht
    hoechstens so viele wie bei den alten Zaehnen im selben Lauf. Info: Wurzel-Pixel am eigenen Kiefer (Wurzeltiefe wie die alten, R3)
                                                                                             | Gegenprobe: derselbe Bau ohne die R6-Kappen
ZV5 N52 beidseitig und Boden am Asset (kf_k5g3_n52, alle 771 Keys; die neuen Zaehne gehoeren zu den 'fremden' Netzen 22/23). 'Arm in 22/23'
    wird ohne die offenen Zahn-Pyramiden nachgerechnet (die Paritaet zaehlt sie als Volumen; Abnahme-Stand Key 724: 419,6 statt 2,5), dazu:
    kein Arm-Punkt in einem neuen Zahn                                                        | Gegenprobe R9-Abgabe (wie R12/R13)
"""
import copy
import hashlib
import json
import math
import os
import subprocess
import sys

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import numpy as np  # noqa: E402
import kf_k5g2_geo as GG  # noqa: E402
import kf_k5g3_zaehne as ZV  # noqa: E402
import kf_k5g3_zaehne_scan as SC  # noqa: E402

FE = ZV.FE
WT = ZV.WT
ASSET = os.path.join(WT, 're15_port', 'shared_assets', 'RE2', 'BAHNSTEIG_FORM')
AUS = os.path.join(ZV.K5, 'k5g', 'K5g3', 'zaehne_vorn')
REF_COMMIT = '1ee2cdca4'                  # Spiel-Asset = K5g3 + R9 (abgenommen), EMD 88c49ce1, TIM c00823b3
REF_SHA = ('88c49ce17bfdc91af2c40c7ccf14f37d47d1957220289a85227ff14ef6d83610', 'c00823b3')
SCR = os.environ.get('ZAEHNE_SCR', r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/riesenform_zaehne/neu/riegel')
ZEILEN, ERG = [], {}


def sha(b):
    return hashlib.sha256(b).hexdigest()


def log(s=''):
    print(s, flush=True)
    ZEILEN.append(s)


def riegel(name, fn, gegen):
    try:
        gut, text = fn()
    except (SystemExit, OSError, KeyError, ValueError) as e:
        gut, text = False, 'Abbruch: %r' % (e,)
    g_erg = []
    for gn, gf in gegen:
        try:
            gg, gt = gf()
        except (SystemExit, OSError, KeyError, ValueError) as e:
            gg, gt = False, 'Abbruch: %r' % (e,)
        g_erg.append((gn, gg, gt))
    stumpf = any(g for _n, g, _t in g_erg)
    urteil = 'STUMPF' if stumpf else ('GRUEN' if gut else 'ROT')
    log('%-6s %s | %s' % (urteil, name, text))
    for gn, gg, gt in g_erg:
        log('         Gegenprobe "%s": %s | %s' % (gn, 'gruen (FALSCH)' if gg else 'rot wie verlangt', gt))
    ERG[name] = dict(urteil=urteil, text=text, gegenproben=[dict(name=a, gruen=b, text=c) for a, b, c in g_erg])
    return urteil == 'GRUEN'


def ref_dateien():
    os.makedirs(SCR, exist_ok=True)
    out = []
    for ext in ('EMD', 'TIM'):
        p = os.path.join(SCR, 'ref_88c49ce1.' + ext)
        if not os.path.exists(p):
            b = subprocess.run(['git', '-C', WT, 'show', '%s:re15_port/shared_assets/RE2/BAHNSTEIG_FORM.%s' % (REF_COMMIT, ext)],
                               capture_output=True, check=True).stdout
            open(p, 'wb').write(b)
        out.append(p)
    e, t = (open(x, 'rb').read() for x in out)
    if sha(e) != REF_SHA[0] or not sha(t).startswith(REF_SHA[1]):
        raise SystemExit('Gegenprobe-Stand aus git hat nicht EMD 88c49ce1 / TIM c00823b3')
    return out


# ------------------------------------------------------------------ ZV1
def zahngruppen(form, clut_y):
    """Alle Zaehne in 22/23 aus den Flaechen: Dreiecke der Zahnzeile 2, verbunden ueber gemeinsame Ecken-Lagen, genau 3 Dreiecke und 4 Ecken
    (die 8 Kehlen-Deckel der Zeile 2 unter dem Kopf sind keine Pyramide). Basis = Mittel der 3 Ecken ohne die gemeinsame Spitze."""
    out = []
    for t in (ZV.OBEN, ZV.UNTEN):
        m = form['meshes'][t]
        fl = [d for d in m['dreiecke'] if ((int(d['clut']) >> 6) & 0x1FF) - clut_y == ZV.ZAHN_ZEILE]
        par = {}

        def find(a):
            while par.setdefault(a, a) != a:
                par[a] = par[par[a]]
                a = par[a]
            return a
        lage = lambda i: tuple(m['ecken'][i][:3])  # noqa: E731
        for d in fl:
            ps = [lage(i) for i in d['e']]
            for q in ps[1:]:
                par[find(ps[0])] = find(q)
        grp = {}
        for d in fl:
            grp.setdefault(find(lage(d['e'][0])), []).append(d)
        for g in grp.values():
            ps = {}
            for d in g:
                for i in d['e']:
                    ps[lage(i)] = ps.get(lage(i), 0) + 1
            if len(g) != 3 or len(ps) != 4:
                continue
            spitze = max(ps, key=lambda q: ps[q])
            basis = np.mean([q for q in ps if q != spitze], 0)
            out.append(dict(netz=t, basis=basis, spitze=np.array(spitze, float)))
    return out


def zv1(emd, tim):
    form = FE.lese_emd(emd)
    cy = FE.tim_lesen(tim)['clut_y']
    zz = zahngruppen(form, cy)
    p = ZV.vorgabe()
    teile = []
    gut = True
    for t, alt in ((ZV.OBEN, ZV.alte_oben(p)), (ZV.UNTEN, ZV.alte_unten(p))):
        a1 = [z for z in alt if z['seite'] == 1]
        ab = math.hypot(a1[-1]['basis'][0] - a1[-2]['basis'][0], a1[-1]['basis'][2] - a1[-2]['basis'][2])
        B = np.array([z['basis'] for z in zz if z['netz'] == t])
        mitte = bool((np.abs(B[:, 2]) <= 1.0).any())
        # Reihenfolge um die Schnauze: Winkel um einen Punkt weit hinten auf der Mittellinie
        c = np.array([B[:, 0].min() - 1500.0, 0.0])
        w = np.arctan2(B[:, 2] - c[1], B[:, 0] - c[0])
        o = np.argsort(w)
        d = np.hypot(np.diff(B[o, 0]), np.diff(B[o, 2]))
        groesste = float(d.max())
        vorn = float(B[:, 0].max())
        ok = mitte and groesste <= 1.5 * ab
        gut = gut and ok
        teile.append('%s: %d Zaehne, Mitte %s, vorderste Basis x %.0f, groesste Luecke %.1f (Grenze 1,5 x %.1f = %.1f)'
                     % ('oben 22' if t == ZV.OBEN else 'unten 23', len(B), 'ja' if mitte else 'NEIN', vorn, groesste, ab, 1.5 * ab))
    return gut, '; '.join(teile)


# ------------------------------------------------------------------ ZV2


def zv2(emd, tim, ref_emd, ref_tim):
    a, r = FE.lese_emd(emd), FE.lese_emd(ref_emd)
    sa, sr = FE.sektionen(emd), FE.sektionen(ref_emd)
    fl = []
    if any(sa[k] != sr[k] for k in ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3')):
        fl.append('Sektionen 0-6 verschieden')
    if tim != ref_tim:
        fl.append('TIM verschieden')
    if len(a['meshes']) != len(r['meshes']) or a['knochen'] != r['knochen']:
        fl.append('Netzzahl/Knochen verschieden')
    for t in range(len(r['meshes'])):
        if t in (ZV.OBEN, ZV.UNTEN):
            continue
        if a['meshes'][t] != r['meshes'][t]:
            fl.append('Netz %d verschieden' % t)
    n_neu, neu_e = 0, 0
    for t in (ZV.OBEN, ZV.UNTEN):
        ma, mr = a['meshes'][t], r['meshes'][t]
        ne, nn, nd = len(mr['ecken']), len(mr['normalen']), len(mr['dreiecke'])
        if ma['ecken'][:ne] != mr['ecken'] or ma['normalen'][:nn] != mr['normalen'] or ma['dreiecke'][:nd] != mr['dreiecke'] \
                or ma['vierecke'] != mr['vierecke']:
            fl.append('Netz %d: alte Ecken/Normalen/Flaechen nicht an ihrer Stelle gleich' % t)
            continue
        muster = set((json.dumps(d['uv']), int(d['clut']), int(d['page'])) for d in mr['dreiecke']
                     if ((int(d['clut']) >> 6) & 0x1FF) - FE.tim_lesen(ref_tim)['clut_y'] == ZV.ZAHN_ZEILE)
        neu = ma['dreiecke'][nd:]
        de, dn = len(ma['ecken']) - ne, len(ma['normalen']) - nn
        if len(neu) % 3 or de != 4 * (len(neu) // 3) or dn != 4 * (len(neu) // 3):
            fl.append('Netz %d: %d neue Dreiecke, %d Ecken, %d Normalen (nicht 3/4/4 je Zahn)' % (t, len(neu), de, dn))
        for d in neu:
            if min(d['e']) < ne or min(d['n']) < nn:
                fl.append('Netz %d: neues Dreieck auf alter Ecke/Normale' % t)
                break
            if (json.dumps(d['uv']), int(d['clut']), int(d['page'])) not in muster:
                fl.append('Netz %d: neues Dreieck ohne Zahn-UV/CLUT/Seite' % t)
                break
        n_neu += len(neu) // 3
        neu_e += de
    if n_neu == 0:
        fl.append('keine neuen Zaehne')
    return not fl, ('neue Zaehne %d (Dreiecke %d, Ecken %d); Sektionen 0-6, TIM, Netze 0-21/24-31 und alle alten Daten von 22/23 gleich 88c49ce1: %s'
                    % (n_neu, 3 * n_neu, neu_e, 'ja' if not fl else 'NEIN ' + '; '.join(fl[:4])))


# ------------------------------------------------------------------ ZV3
def zv3(emd_pfad, name):
    os.makedirs(SCR, exist_ok=True)
    m, nz = SC.lauf(open(emd_pfad, 'rb').read())
    aus, oben, vn, zahn = [], [], [], []
    for k, e in m.items():
        g = e['gruppen']
        alt_z = max(g.get('alt_oben', {}).get('zahn', 0.0), g.get('alt_unten', {}).get('zahn', 0.0))
        for q in ('neu_oben', 'neu_unten'):
            v = g.get(q)
            if not v:
                continue
            if v['austritt']:
                aus.append((k, q))
            if v.get('vor_nase'):
                vn.append(k)
            if v['zahn'] > alt_z + 1e-9:
                zahn.append((k, q, v['zahn'], alt_z))
        if max(g.get('neu_oben', {}).get('tiefe', 0.0), g.get('neu_unten', {}).get('tiefe', 0.0)) > 0:
            oben.append(k)
    nneu = sum(1 for k, e in m.items() for q in ('neu_oben', 'neu_unten') if q in e['gruppen']) // max(len(m), 1)
    json.dump(dict(emd=emd_pfad.replace(chr(92), '/'), keys={str(k): v for k, v in sorted(m.items())}),
              open(os.path.join(SCR, 'zv3_%s.json' % name), 'w'), indent=0)
    ok = nneu > 0 and not aus and not oben and not vn and not zahn
    return ok, ('%d Keys, %d Zaehne (davon %s neue Gruppen): Austritt neu %d Keys, neue im Gegenkiefer %d Keys, untere vor der Nase %d Keys, '
                'Zahn-in-Zahn ueber alt %d Keys%s' % (len(m), nz, 'mit' if nneu else 'OHNE', len(set(k for k, _q in aus)), len(oben), len(vn),
                                                       len(zahn), (' (z. B. %s)' % zahn[:2]) if zahn else ''))


def bau_ohne_kappen(ref_emd_pfad):
    """Gegenprobe ZV3: Spiel-Stand 88c49ce1 + Vorderzaehne mit Nennlaengen (ohne R6-Kappen)."""
    p = ZV.vorgabe()
    emd = open(ref_emd_pfad, 'rb').read()
    form = FE.lese_emd(emd)
    neue = ZV.neue_reihe(p, ZV.OBEN, kappen={})[0] + ZV.neue_reihe(p, ZV.UNTEN, kappen={})[0]
    ZV.einsetzen(form, p, neue)
    out = os.path.join(SCR, 'gegen_ohne_kappen.EMD')
    open(out, 'wb').write(GG.emd_mit_md1(emd, form))
    return out


# ------------------------------------------------------------------ ZV4
def zv4(emd_pfad, name):
    m = SC.maler_lauf(open(emd_pfad, 'rb').read())
    json.dump(dict(emd=emd_pfad.replace(chr(92), '/'), keys={str(k): v for k, v in sorted(m.items())}),
              open(os.path.join(SCR, 'zv4_%s.json' % name), 'w'), indent=0)
    zs = {}
    for e in m.values():
        for n, v in e.items():
            s = zs.setdefault(n, dict(neu_fremd=0, alt_fremd=0, neu_falsch=0, alt_falsch=0, neu_sicht=0, alt_sicht=0))
            for a in s:
                s[a] += v[a]
    rot, txt = [], []
    for n, s in zs.items():
        txt.append('%s fremd neu %d / alt %d (Wurzel neu %.3f / alt %.3f)' % (n, s['neu_fremd'], s['alt_fremd'],
                                                                           (s['neu_falsch'] - s['neu_fremd']) / max(s['neu_sicht'], 1),
                                                                           (s['alt_falsch'] - s['alt_fremd']) / max(s['alt_sicht'], 1)))
        if s['neu_fremd'] > s['alt_fremd']:
            rot.append(n)
    sicht = sum(s['neu_sicht'] for s in zs.values())
    return not rot and sicht > 0, 'neue Zahnpixel sichtbar %d; %s%s' % (sicht, '; '.join(txt), ('; ROT: %s' % rot) if rot else '')


# ------------------------------------------------------------------ ZV5
def zv5(emd, name):
    """N52 beidseitig + Boden am Asset. Die Strahl-Paritaet von kf_k5g3_n52 nimmt ALLE Dreiecke von 22/23 als Huelle, auch die offenen
    Zahn-Pyramiden (ohne Grundflaeche): ein Strahl durch einen Zahn zaehlt dann als Ein-/Austritt. Gemessen (Abnahme-Stand 88c49ce1, alte Zaehne):
    Key 724 Kralle 19 'in Kopf 22' 419,6 tief, ohne die Zahn-Pyramiden 2,5 (Kopfrohr 2,4). Darum: (a) alle Faelle wie R12/R13 ausser
    'Arm in 22/23'; (b) 'Arm in 22/23' nachgerechnet mit der Huelle OHNE Zahn-Pyramiden, Grenze wie R12 (max(Bezug) + 25); (c) kein Probepunkt des
    Arms in einem NEUEN Zahn (konvexe Pyramide) in irgendeinem Key; die Richtung Zahn -> Arm steckt in 'rueck' (a)."""
    import riegel_k5g3 as RG
    import kf_k5g3_n52 as N52
    refs, _sh = RG.n52_bezuege()
    m = RG.n52_lauf(name, emd)
    faelle, boden, _tief = N52.bewerte(m, refs)
    kz = [f for f in faelle if f[2] == 'vor' and f[3].split('>')[1] in ('22', '23')]
    rest = [f for f in faelle if f not in kz]
    N52._ST.clear()
    N52._init(emd)
    st = N52._ST
    form = st['form']
    zz = SC.zaehne_im_netz(form, ZV.vorgabe())
    pyr = set((z['netz'], fi) for z in zz for fi in z['flaechen'])
    ix = {}
    for t in (22, 23):
        mm = form['meshes'][t]
        tri = [d['e'] for fi, d in enumerate(mm['dreiecke']) if (t, fi) not in pyr]
        for q in mm['vierecke']:
            e = q['e']
            tri += [[e[0], e[1], e[3]], [e[0], e[3], e[2]]]
        ix[t] = np.array(tri, int)
    nach = []
    for f in kz:
        k = f[1]
        g, h = (int(x) for x in f[3].split('>'))
        i, _c, _b = st['first'][k]
        R, T = st['R'][i], st['T'][i]
        d = N52.tiefe52(GG.welt(R, T, g, st['V'][g]), GG.welt(R, T, h, st['V'][h])[ix[h]])
        gr = N52.grenze(refs, k, 'vor', f[3])
        if d > gr:
            nach.append((k, f[3], round(d, 1), round(gr, 1)))
    neu = [z for z in zz if z['gruppe'].startswith('neu')]
    im_zahn = []
    for k in sorted(st['first']):
        i, _c, _b = st['first'][k]
        R, T = st['R'][i], st['T'][i]
        E = [GG.welt(R, T, z['netz'], st['V'][z['netz']][z['ecken']]) for z in neu]
        for g in N52.ARM:
            P = GG.welt(R, T, g, st['V'][g])
            for z, Ez in zip(neu, E):
                if (P.min(0) > Ez.max(0)).any() or (P.max(0) < Ez.min(0)).any():
                    continue
                ab = np.array([P @ n - dd for n, dd in SC._ebenen(Ez)])
                if (ab < 0).all(0).any():
                    im_zahn.append((k, g, z['netz'], z['seite'], z['i']))
    ok = not rest and not boden and not nach and not im_zahn and len(neu) > 0
    return ok, ('%d Keys, %d neue Zaehne: (a) N52 ohne Arm->22/23 ueber max(Bezug)+25: %d, Boden: %d Keys; (b) Arm->22/23 laut Paritaet %d Faelle '
                '(offene Zahn-Pyramiden), ohne Pyramiden nachgerechnet ueber der Grenze: %d%s; (c) Arm-Probepunkte in einem neuen Zahn: %d Faelle%s'
                % (len(m), len(neu), len(rest), len(boden), len(kz), len(nach), (' %s' % nach[:4]) if nach else '', len(im_zahn),
                   (' %s' % im_zahn[:4]) if im_zahn else ''))


def main(argv):
    emd_p = argv[0] if argv else ASSET + '.EMD'
    tim_p = emd_p[:-4] + '.TIM'
    emd, tim = open(emd_p, 'rb').read(), open(tim_p, 'rb').read()
    ref_e, ref_t = ref_dateien()
    re_, rt_ = open(ref_e, 'rb').read(), open(ref_t, 'rb').read()
    log('RIEGEL ZAEHNE BIS VORN  EMD %s  sha256 %s  TIM %s' % (emd_p.replace(chr(92), '/'), sha(emd), sha(tim)[:16]))
    ok = []
    ok.append(riegel('ZV1 Zaehne bis vorn (Mitte besetzt, keine Luecke > 1,5 x Abstand O1-O2 / U1-U2)', lambda: zv1(emd, tim),
                     [('88c49ce1 (abgenommen, ohne Vorderzaehne)', lambda: zv1(re_, rt_))]))

    def ecke_plus1():
        f = FE.lese_emd(emd)
        f['meshes'][22]['ecken'][5][0] += 1
        return zv2(GG.emd_mit_md1(emd, f), tim, re_, rt_)
    ok.append(riegel('ZV2 Nur angehaengt: alles Alte bytegleich 88c49ce1, neu nur Zahndreiecke', lambda: zv2(emd, tim, re_, rt_),
                     [('Asset mit einer alten Ecke von 22 +1', ecke_plus1)]))
    ohne = bau_ohne_kappen(ref_e)
    ok.append(riegel('ZV3 Kein Durchstich der neuen Zaehne (alle Keys)', lambda: zv3(emd_p, 'asset'),
                     [('ohne R6-Kappen (Nennlaengen)', lambda: zv3(ohne, 'ohne_kappen'))]))
    ok.append(riegel('ZV4 Maler: neue Zaehne nicht oefter durch den Gegenkiefer gemalt als die alten (je Ansicht, alle Keys)',
                     lambda: zv4(emd_p, 'asset'), [('ohne R6-Kappen (Nennlaengen)', lambda: zv4(ohne, 'ohne_kappen'))]))
    import riegel_k5g3 as RG
    ok.append(riegel('ZV5 N52 beidseitig + Boden am Asset (alle 771 Keys)', lambda: zv5(emd, 'asset_%s' % sha(emd)[:8]),
                     [('R9-Abgabe (kr4 r9)', lambda: zv5(RG.emd_r9_abgabe(), 'r9_abgabe'))]))
    n_g = sum(1 for e in ERG.values() for g in e['gegenproben'] if not g['gruen'])
    n_gg = sum(len(e['gegenproben']) for e in ERG.values())
    summe = 'RIEGEL ZAEHNE BIS VORN: %d von %d gruen, %d von %d Gegenproben rot wie verlangt' % (sum(ok), len(ok), n_g, n_gg)
    log(summe)
    os.makedirs(AUS, exist_ok=True)
    open(os.path.join(AUS, 'riegel_zaehne.txt'), 'w', encoding='utf-8', newline='\n').write('\n'.join(ZEILEN) + '\n')
    json.dump(dict(emd_sha256=sha(emd), tim_sha256=sha(tim), riegel=ERG, summe=summe), open(os.path.join(AUS, 'riegel_zaehne.json'), 'w',
              encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    return all(ok) and n_g == n_gg


if __name__ == '__main__':
    sys.exit(0 if main(sys.argv[1:]) else 1)
