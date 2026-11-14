#!/usr/bin/env python3
"""Riegel der K5g3-Geometrie-Stufe (Orchestrator-Entscheide G1, G2, G3 vom 2026-11-14). Exit 1 bei Rot.

Jeder Riegel hat mindestens eine Gegenprobe, die ROT sein MUSS; ist sie gruen, meldet der Riegel STUMPF (Exit 1).

  python riegel_k5g3.py            -> Protokoll auf stdout und K5/k5g/K5g3/geo/riegel_k5g3.txt (+ .json)
  python riegel_k5g3.py textur [<Variante>] [--spiel <json>] [--neu_spiel]
                                   -> Textur-Riegel (E3, V1-V6, O3-O5; riegel_k5g3_tex.py), Protokoll <Variante>/riegel_k5g3_textur.txt (+ .json)

R1 Generator     Datei == kf_k5g3_geo.geometrie_bytes(), zweiter Lauf byte-gleich      | Gegenprobe: ohne Krallen-Stufe
R2 Byte-Probe    K5e_f1-Posen + K5e_f1-Krallen + B4-Tabelle == KAND.EMD (sha f78e5983)  | Gegenprobe: volle Geometrie
R3 Spalt         Netze 10/16/21/22: Ecken/Normalen/Flaechen/CLUT/Seite == B4-Tabelle (G1, sha 99a4f142), UV == uvfein(B4)
                                                                                         | Gegenproben: K5e_f1; B4-UV ohne uvfein
R4 Krallen       19/20/28/29 Ecken+Normalen == K5c, Flaechen/UV/CLUT/Seite == K5e_f1   | Gegenproben: K5e_f1; K5c-Netze ganz
R5 Rest          alle anderen Netze und die Knochen == K5e_f1                           | Gegenprobe: Netz 31 eine Ecke +1
R6 EDD/EMR       Sektionen 0-6 byte-gleich K5e_f1 und K5c                               | Gegenprobe: ein Bit in emr1
R7 Posen         Pool == K5c (keine Posen-Datei) bzw. == Posen-Datei; FORM.EMD zieht nach | Gegenproben: Probe-Posen-Datei; Key 300
R8 B4-Grenzen    Messbank-Urteil des Gesamtmodells (urteil4, voller Lauf) nicht schlechter als B4 allein | Gegenprobe: Urteil B3
R9 Dreiecke      max gezeichnet je Bild (Messbank, alle Clips/Ansichten) + Spielfigur + Waffe + Effekte <= 16384 - Reserve;
                 im Spiel gezaehlt (G3-Zaehler) <= 16384 - Reserve, verworfen 0   | Gegenprobe: alte Grenze 8192
R10 UV-Stufe     (Runde 2) Kinder der Netze 10/16/21/22: (a) keine Ecke ausserhalb der UV-Flaeche der Mutter ausser den gemeldeten
                 B4-Kindern, wo im Gebiet nur eine Linie moeglich ist; (b) Linien-Dreiecke (UV-Flaeche 0) je Netz <= B4 und gesamt
                 <= 1/4 von B4; (c) Augen-Flaechen (Zone 29) lesen dieselben Augen-Texel wie K5e_f1   | Gegenprobe: B4-UV ohne uvfein
"""
import copy
import hashlib
import json
import os
import sys

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g3_geo as K  # noqa: E402

FE, G = K.FE, K.G
K5 = K.K5
GEO = os.path.join(K5, 'k5g', 'K5g3', 'geo')
MESS = os.path.join(GEO, 'messung')
K5C_EMD = os.path.join(K5, 'einbau_k5c', 'BAHNSTEIG_FORM.EMD')
SEK06 = ('morph', 'edd1', 'emr1', 'edd2', 'emr2', 'edd3', 'emr3')
GRENZE, RESERVE = 16384, 2048                        # G3: render_pc.c TEXTRI_QUEUE_MAX; Reserve = ein Viertel der alten Grenze
SPIELERIN, WAFFE, EFFEKTE = 773, 86, 1000            # PL08 773 / W 86 (riegel_k5g2_geo, bank.py Z.57); Effekte: Rahmen fuer Feuer/Huelsen/Blut
ZEILEN, ERG = [], {}


def log(s=''):
    print(s)
    ZEILEN.append(s)


def sha(b):
    return hashlib.sha256(b).hexdigest()


def riegel(name, fn, gegen):
    """fn() -> (gut, text); gegen: Liste (name, fn) - jede muss ROT sein."""
    try:
        gut, text = fn()
    except (SystemExit, OSError, KeyError) as e:
        gut, text = False, 'Abbruch: %r' % (e,)
    g_erg = []
    stumpf = False
    for gn, gf in gegen:
        try:
            gg, gt = gf()
        except (SystemExit, OSError, KeyError) as e:
            gg, gt = False, 'Abbruch: %r' % (e,)
        g_erg.append((gn, gg, gt))
        stumpf = stumpf or gg
    urteil = 'STUMPF' if stumpf else ('GRUEN' if gut else 'ROT')
    log('%-6s %s | %s' % (urteil, name, text))
    for gn, gg, gt in g_erg:
        log('         Gegenprobe "%s": %s | %s' % (gn, 'gruen (FALSCH)' if gg else 'rot wie verlangt', gt))
    ERG[name] = dict(urteil=urteil, text=text, gegenproben=[dict(name=a, gruen=b, text=c) for a, b, c in g_erg])
    return urteil == 'GRUEN', len(g_erg) - sum(1 for _a, b, _c in g_erg if b)


# ------------------------------------------------------------------ Daten
EMD = open(K.GEO3_ZIEL, 'rb').read()
FORMEMD = open(os.path.join(GEO, 'FORM.EMD'), 'rb').read()
A = open(K.GEO_BASIS, 'rb').read()
C = open(K5C_EMD, 'rb').read()
FG, FA, FC = FE.lese_emd(EMD), FE.lese_emd(A), FE.lese_emd(C)
TAB = json.load(open(K.GEO3_SPALT, encoding='utf-8'))
TUV, UVBER = K.spalt_uv()                              # uvfein(B4) im Speicher
FB4UV = K.geometrie_form(stufen=('posen', 'krallen', 'spalt'))[1]   # Gegenprobe: B4-UV ohne uvfein
import kf_k5g3_uv as UVF  # noqa: E402


def form_von(b):
    return FE.lese_emd(b)


# R1
def r1(datei=EMD, stufen=K.STUFEN):
    a, b = K.geometrie_bytes(stufen=stufen), K.geometrie_bytes(stufen=stufen)
    return a == b == datei, 'zwei Laeufe gleich %s, == Datei %s, sha256 %s' % (a == b, a == datei, sha(a)[:16])


# R2
def r2(b=None):
    b = K.geometrie_bytes(stufen=('spalt',), posen='') if b is None else b
    return sha(b) == K.KAND_B4_SHA, 'sha256 %s (KAND.EMD %s)' % (sha(b)[:16], K.KAND_B4_SHA[:16])


# R3
def _ohne_uv(m):
    return dict(ecken=m['ecken'], normalen=m['normalen'],
                fl=[[(f['e'], f['n'], f['clut'], f['page']) for f in m[a]] for a in ('dreiecke', 'vierecke')])


def _nur_uv(m):
    return [[f['uv'] for f in m[a]] for a in ('dreiecke', 'vierecke')]


def r3(f=FG):
    t_sha = sha(open(K.GEO3_SPALT, 'rb').read())
    ab = [t for t in K.SPALT_TEILE if _ohne_uv(f['meshes'][t]) != _ohne_uv(TAB['teile'][str(t)])]
    au = [t for t in K.SPALT_TEILE if _nur_uv(f['meshes'][t]) != _nur_uv(TUV['teile'][str(t)])]
    nd = {t: '%d/%d/%d' % (len(f['meshes'][t]['ecken']), len(f['meshes'][t]['dreiecke']), len(f['meshes'][t]['vierecke'])) for t in K.SPALT_TEILE}
    return (not ab and not au and t_sha == K.GEO3_SPALT_SHA), ('Tabelle sha %s; Netze mit anderer Geometrie als B4: %s; Netze mit anderer UV als '
                                                             'uvfein(B4): %s; Ecken/Dreiecke/Vierecke %s' % (t_sha[:16], ab or 'keine', au or 'keine', nd))


# R4
def r4(f=FG):
    fl = []
    for t in K.KRALLEN_K5C:
        m, c, a = f['meshes'][t], FC['meshes'][t], FA['meshes'][t]
        if m['ecken'] != c['ecken']:
            fl.append('%d Ecken != K5c' % t)
        if m['normalen'] != c['normalen']:
            fl.append('%d Normalen != K5c' % t)
        if m['dreiecke'] != a['dreiecke'] or m['vierecke'] != a['vierecke']:
            fl.append('%d Flaechen/UV/CLUT/Seite != K5e_f1' % t)
    if f['meshes'][31] != FA['meshes'][31] or FA['meshes'][31]['ecken'] != FC['meshes'][31]['ecken']:
        fl.append('31 nicht K5e_f1 (= K5c-Ecken)')
    return not fl, 'K5c sha %s; %s' % (sha(C)[:16], '; '.join(fl) if fl else '19/20/28/29 Ecken+Normalen = K5c, Flaechen = K5e_f1; 31 = K5e_f1 = K5c')


# R5
def r5(f=FG):
    frei = set(K.SPALT_TEILE) | set(K.KRALLEN_K5C)
    ab = [t for t in range(len(FA['meshes'])) if t not in frei and f['meshes'][t] != FA['meshes'][t]]
    kn = f['knochen'] == FA['knochen'] and len(f['meshes']) == len(FA['meshes'])
    return (not ab and kn), '%d Netze geprueft; abweichend %s; Knochen/Netzzahl gleich %s' % (
        len(FA['meshes']) - len(frei), ab or 'keine', kn)


# R6
def r6(b=EMD):
    s, sa, sc = FE.sektionen(b), FE.sektionen(A), FE.sektionen(C)
    ab = [n for n in SEK06 if not (s[n] == sa[n] == sc[n])]
    return not ab, 'Sektionen 0-6 (%d Byte) gleich K5e_f1 und K5c: %s' % (sum(len(s[n]) for n in SEK06), 'ja' if not ab else 'NEIN %s' % ab)


# R7
def r7(f=FG, fe=FORMEMD, posen=None):
    fl = []
    if posen is None:
        if f['pool'] != FC['pool'] or f['clips'] != FC['clips']:
            fl.append('Pool/Clips != K5c')
        art = 'keine Posen-Datei -> Pool == K5c'
    else:
        soll = copy.deepcopy(FC['pool'])
        n = 0
        for key, e in posen['keys'].items():
            for t, w in e['neu'].items():
                soll[int(key)]['winkel'][int(t)] = [(int(x) & 0xFFF) - 4096 if (int(x) & 0xFFF) > 2047 else int(x) & 0xFFF for x in w]
                n += 1
        if f['pool'] != soll:
            fl.append('Pool != K5c + Posen-Datei')
        art = 'Posen-Datei mit %d Tripeln -> Pool == K5c + Datei' % n
    ff = FE.lese_emd(fe)
    for k, kb in K.FORM_KEYS:
        if ff['pool'][k]['winkel'] != f['pool'][kb]['winkel']:
            fl.append('FORM.EMD Key %d != Key %d' % (k, kb))
    if ff['meshes'] != f['meshes']:
        fl.append('FORM.EMD MD1 != Geometrie')
    return not fl, '%s; FORM.EMD Key 0/1 = Key 11/542, MD1 gleich: %s' % (art, '; '.join(fl) if fl else 'ja')


def probe_posen():
    """Probe-Posen-Datei: Key 542 Knochen 17 um +64 gedreht (nur fuer die Gegenprobe/den Mechanismus, nicht gespeichert)."""
    alt = FC['pool'][542]['winkel'][17]
    return {'keys': {'542': {'alt': {'17': [int(x) & 0xFFF for x in alt]}, 'neu': {'17': [(int(alt[0]) + 64) & 0xFFF, int(alt[1]) & 0xFFF, int(alt[2]) & 0xFFF]}}}}


def r7_mechanik():
    """Posen-Datei wirkt genau auf den genannten Tripel (Generator im Speicher, Datei in GEO/messung/_probe_posen.json)."""
    p = os.path.join(MESS, '_probe_posen.json')
    os.makedirs(MESS, exist_ok=True)
    json.dump(probe_posen(), open(p, 'w', encoding='utf-8'))
    try:
        b = K.geometrie_bytes(posen=p)
    finally:
        os.remove(p)
    f = FE.lese_emd(b)
    return f, K.form_emd_bytes(b)


# R10
AUGEN_FL = ((22, 0, 82), (22, 0, 83), (22, 1, 112), (22, 1, 113), (22, 1, 152))      # Zone 29 (zonen_v2.json)


def _augen_texel():
    z = json.load(open(os.path.join(K5, 'zonen_nutzer', 'v2', 'zonen_v2.json'), encoding='utf-8'))
    e = [x for x in z['zonen'] if x['nr'] == '29'][0]
    return {(u, v) for _s, _z, u, v in e['texel']['liste']}


def _texel(tris, page):
    import numpy as np
    out = set()
    for tr in tris:
        P = UVF.PROBEN @ np.array(tr, float)
        out |= {(int(np.floor(x + 1e-9)) + (int(page) & 15) * 128, int(np.floor(y + 1e-9))) for x, y in P}
    return out


def r10(f=FG):
    ART = UVF.ART
    aug = _augen_texel()
    aussen, aussen_b4fremd = 0, []
    her = TAB['herkunft']
    for t, a, i, mt, ma, mi in her:
        mf = FA['meshes'][mt][ART[ma]][mi]
        k = f['meshes'][t][ART[a]][i]
        if a != 0 or (k['uv'] == mf['uv'] and a == ma):
            continue
        tris = UVF.render_dreiecke(mf, ma)
        if not all(UVF.im_gebiet(tuple(p), tris) for p in k['uv']):
            aussen += 1
            if k['uv'] != TAB['teile'][str(t)][ART[a]][i]['uv']:
                aussen_b4fremd.append('%d|%d|%d' % (t, a, i))
    erlaubt = UVBER['gruende'].get('aussen, im Gebiet nur Linie -> B4 bleibt', 0)
    lin = UVF.linien_flaechen(f['meshes'])
    lb4 = UVF.linien_flaechen({int(k): v for k, v in TAB['teile'].items()})
    la = UVF.linien_flaechen(FA['meshes'])
    aug_soll, aug_ist = [], []
    for mt, ma, mi in AUGEN_FL:
        mf = FA['meshes'][mt][ART[ma]][mi]
        aug_soll.append(len(_texel(UVF.render_dreiecke(mf, ma), mf['page']) & aug))
        ks = [(t, a, i) for t, a, i, x, y, z in her if (x, y, z) == (mt, ma, mi)]
        tk = set()
        for t, a, i in ks:
            k = f['meshes'][t][ART[a]][i]
            tk |= _texel(UVF.render_dreiecke(k, a), k['page'])
        aug_ist.append(len(tk & aug))
    fl = []
    if aussen > erlaubt or aussen_b4fremd:
        fl.append('Kinder ausserhalb der Mutter-UV %d (erlaubt %d), davon nicht B4: %s' % (aussen, erlaubt, aussen_b4fremd[:5]))
    if any(lin[t] > lb4[t] for t in lin) or sum(lin.values()) * 4 > sum(lb4.values()):
        fl.append('Linien-Dreiecke %s gegen B4 %s' % (lin, lb4))
    if aug_ist != aug_soll:
        fl.append('Augen-Texel %s statt %s' % (aug_ist, aug_soll))
    return not fl, ('Kinder ausserhalb der Mutter-UV %d (B4 2711; erlaubt die %d gemeldeten B4-Kinder); Linien-Dreiecke je Netz %s, gesamt %d '
                    '(B4 %s = %d, K5e_f1 %s = %d); Augen-Texel je Zone-29-Mutter %s (K5e_f1 %s); gew. Fehlrate %.4f (B4 %.4f)%s'
                    % (aussen, erlaubt, lin, sum(lin.values()), lb4, sum(lb4.values()), la, sum(la.values()), aug_ist, aug_soll,
                       UVBER['nachher']['fehlrate_gew'], UVBER['vorher']['fehlrate_gew'], ('; ROT: ' + '; '.join(fl)) if fl else ''))


# R8
def lade_urteil(p):
    d = json.load(open(p, encoding='utf-8'))
    u = {e['nr']: e for e in d['urteil2']}
    h = {e['nr']: e for e in d['hart']}
    return d, u, h


def r8(gk=None, b4=None):
    gk = gk or os.path.join(MESS, 'urteil_gk.json')
    b4 = b4 or os.path.join(MESS, 'urteil_b4.json')
    dg, ug, hg = lade_urteil(gk)
    db, ub, hb = lade_urteil(b4)
    fl, zeilen = [], []

    def le(name, a, b):
        zeilen.append('%s %s/%s' % (name, a, b))
        if a > b:
            fl.append('%s %s > B4 %s' % (name, a, b))

    le('H1a', ug['H1a']['daten']['summe'], ub['H1a']['daten']['summe'])
    le('H1b', ug['H1b']['daten']['groesstes'], ub['H1b']['daten']['groesstes'])
    le('H1c', ug['H1c']['daten']['groesste'][0], ub['H1c']['daten']['groesste'][0])
    le('H1c_rot', len(ug['H1c']['daten']['rot']), len(ub['H1c']['daten']['rot']))
    le('H2_summe', ug['H2']['daten']['ist']['summe'], ub['H2']['daten']['ist']['summe'])
    le('H2_ueber20', ug['H2']['daten']['ist']['ueber20'], ub['H2']['daten']['ist']['ueber20'])
    le('H3b_rot', len(ug['H3b']['daten']['rot']), len(ub['H3b']['daten']['rot']))
    le('H3b_summe', ug['H3b']['daten']['summe'], ub['H3b']['daten']['summe'])
    zg = {z[0]: z for z in ug['H3a']['daten']['zeilen']}
    zb = {z[0]: z for z in ub['H3a']['daten']['zeilen']}
    neu_rot = []
    for k in sorted(zg):
        b4 = zb[k][1] if k in zb else 0
        if zg[k][1] > max(b4, zg[k][3]):                          # Gruppe ueber B4 UND ueber ihrer Grenze (K5e_f1*1,05+50)
            neu_rot.append(k)
            fl.append('H3a %s %s > B4 %s / Grenze %.0f' % (k, zg[k][1], b4, zg[k][3]))
    zeilen.append('H3a Gruppen %d (ueber B4 und Grenze: %d)' % (len(zg), len(neu_rot)))
    for k in ('bild2', 'bild1'):
        a, b, g = ug['H7']['daten']['umriss'][k], ub['H7']['daten']['umriss'][k], ug['H7']['daten']['k5e'][k] - 0.0005
        zeilen.append('H7_%s %.5f/%.5f' % (k, a, b))
        if a < g:
            fl.append('H7 %s %.5f < Grenze %.5f' % (k, a, g))
    # Gruen-Status: jede bei B4 gruene harte Grenze bleibt gruen (H6b ausgenommen: gewollt rot, Krallen G2 - gesondert geprueft)
    for nr, e in hb.items():
        if nr in ('H6b', 'H5'):
            continue
        if e['gut'] and not hg.get(nr, {}).get('gut', False):
            fl.append('%s bei B4 gruen, am Gesamtmodell rot' % nr)
    h6 = hg.get('H6b', {}).get('ist', '')
    if 'fremde Netze geaendert: [19, 20, 28, 29]' not in h6 and 'fremde Netze geaendert: keine' not in h6:
        fl.append('H6b: fremde Netze nicht genau die Krallen 19/20/28/29 (%s)' % h6[:120])
    if not hg.get('H5', {}).get('gut'):
        fl.append('H5: Lauf gehoert nicht zur EMD')
    if dg['sha256'] != sha(EMD) and gk.endswith('urteil_gk.json'):
        fl.append('Urteil gehoert nicht zur Geometrie-EMD (%s)' % dg['sha256'][:16])
    zeilen.append('H4 %s/%s' % (ug['H4']['daten']['meiste'][0], ub['H4']['daten']['meiste'][0]))
    return not fl, ('GK/B4: ' + ', '.join(zeilen) + ('; ROT: ' + '; '.join(fl) if fl else '')), ug


# R8b
KRALLEN_TEILE = ('19', '20', '28', '29', '31')


def r8b(gk=None):
    """Jede Verschlechterung aus R8 beruehrt die K5c-Krallen (G2): H3a nur Paare mit 19/20/28/29/31, H6b nur fremde Netze 19/20/28/29,
    H7 Bild 2 = Umriss von K5c allein (+-0,0005; messung/umriss.json). Sonst ROT."""
    g, t, _u = r8(gk=gk)
    fl = []
    teile = t.split('; ROT: ')[1].split('; ') if '; ROT: ' in t else []
    um = json.load(open(os.path.join(MESS, 'umriss.json'), encoding='utf-8'))
    for f in teile:
        if f.startswith('H3a bei B4 gruen') or f.startswith('H6b'):
            continue                                              # Sammelzeile; die Paare stehen einzeln darunter / H6b gewollt (G2)
        if f.startswith('H3a '):
            paar = f.split()[1]
            if not any(x in paar.replace('<->', ' ').replace('T', ' ').split() for x in KRALLEN_TEILE):
                fl.append('ohne Kralle: ' + f)
        elif f.startswith('H7') or f.startswith('H7 bei B4 gruen'):
            if abs(um['K5g3']['bild2'] - um['K5c']['bild2']) > 0.0005 or um['K5g3']['bild1'] < um['K5e_f1']['bild1'] - 0.0005:
                fl.append('H7 nicht durch K5c erklaert: ' + f)
        else:
            fl.append('ohne Kralle: ' + f)
    return (bool(teile) and not fl), ('%d Verschlechterungen, alle an den K5c-Krallen: %s; Umriss Bild 2 K5g3 %.5f / K5c allein %.5f / B4 %.5f'
                                      % (len(teile), 'ja' if not fl else 'NEIN ' + '; '.join(fl), um['K5g3']['bild2'], um['K5c']['bild2'],
                                         um['B4_KAND']['bild2']))


# R9
def r9(grenze=GRENZE, reserve=RESERVE, spiel=None):
    _g, _t, ug = r8()
    m = ug['H4']['daten']['meiste']
    summe = m[0] + SPIELERIN + WAFFE + EFFEKTE
    sp = spiel or json.load(open(os.path.join(MESS, 'spiel_dreiecke.json'), encoding='utf-8'))
    smax = max(l['hoechststand'] for l in sp['laeufe'])
    sverw = sum(l['verworfen'] for l in sp['laeufe'])
    gut = summe <= grenze - reserve and smax <= grenze - reserve and sverw == 0
    return gut, ('Messbank max %d (%s Clip %d Bild %d) + Spielerin %d + Waffe %d + Effekte %d = %d <= %d - %d = %d; Spiel max %d (%s), '
                 'verworfen %d; Reserve Messbank %d, Spiel %d' % (m[0], m[1], m[2], m[3], SPIELERIN, WAFFE, EFFEKTE, summe, grenze, reserve,
                                                               grenze - reserve, smax, ', '.join('%s %d' % (l['lauf'], l['hoechststand']) for l in sp['laeufe']),
                                                               sverw, grenze - summe, grenze - smax))


def main():
    gr, gp = 0, 0
    log('RIEGEL K5g3 GEOMETRIE  EMD %s  sha256 %s' % (K.GEO3_ZIEL.replace(chr(92), '/'), sha(EMD)))
    tests = []
    tests.append(('R1 Generator (Datei == Generator, zweiter Lauf gleich)', r1,
                  [('ohne Krallen-Stufe', lambda: r1(stufen=('posen', 'spalt', 'uvfein')))]))
    tests.append(('R2 Byte-Probe (K5e_f1-Posen/-Krallen + B4 == KAND.EMD)', r2,
                  [('volle Geometrie', lambda: r2(EMD))]))
    tests.append(('R3 Netze 10/16/21/22 == B4-Tabelle (G1), UV == uvfein(B4)', r3, [('K5e_f1', lambda: r3(FA)), ('B4-UV ohne uvfein', lambda: r3(FB4UV))]))

    def k5c_ganz():
        f = copy.deepcopy(FG)
        for t in K.KRALLEN_K5C:
            f['meshes'][t] = copy.deepcopy(FC['meshes'][t])
        return r4(f)
    tests.append(('R4 Krallen 19/20/28/29 = K5c-Ecken/Normalen, Flaechen K5e_f1 (G2)', r4,
                  [('K5e_f1-Krallen', lambda: r4(FA)), ('K5c-Netze ganz (K5c-UV/CLUT)', k5c_ganz)]))

    def netz31():
        f = copy.deepcopy(FG)
        f['meshes'][31]['ecken'][0][0] += 1
        return r5(f)
    tests.append(('R5 alle anderen Netze und Knochen == K5e_f1', r5, [('Netz 31 eine Ecke +1', netz31)]))

    def bit():
        b = bytearray(EMD)
        dir_off, d = FE.verzeichnis(EMD)
        b[d[2] + 100] ^= 1
        return r6(bytes(b))
    tests.append(('R6 EDD/EMR (Sektionen 0-6) byte-gleich', r6, [('ein Bit in emr1', bit)]))

    fpos, fe_pos = r7_mechanik()

    def mech():
        g1, t1 = r7(fpos, fe_pos, posen=probe_posen())
        return g1, 'Mechanik Probe-Datei (Key 542 Knochen 17): ' + t1
    tests.append(('R7 Posen == K5c (keine Posen-Datei); FORM.EMD nachgezogen', r7,
                  [('Probe-Posen-Datei gegen "== K5c"', lambda: r7(fpos, fe_pos)),
                   ('Posen-Datei mit Key 300 (nicht erlaubt)', lambda: K.posen_form(FE.lese_emd(A), {'keys': {'300': {'alt': {'17': [0, 0, 0]}, 'neu': {'17': [1, 0, 0]}}}}) and (True, 'angenommen'))]))
    tests.append(('R7b Posen-Datei wirkt genau auf ihre Tripel (Mechanik)', mech,
                  [('Probe-Datei, aber alt falsch', lambda: K.posen_form(FE.lese_emd(A), {'keys': {'542': {'alt': {'17': [1, 2, 3]}, 'neu': {'17': [1, 2, 3]}}}}) and (True, 'angenommen'))]))
    tests.append(('R8 B4-Grenzen am Gesamtmodell nicht schlechter als B4 allein (Messbank voll, urteil4)', lambda: r8()[:2],
                  [('Urteil von B3 statt GK', lambda: r8(gk=os.path.join(MESS, 'urteil_b3.json'))[:2])]))
    tests.append(('R8b Ursache der R8-Reste: nur die K5c-Krallen (G2)', r8b,
                  [('Urteil von B3 statt GK', lambda: r8b(gk=os.path.join(MESS, 'urteil_b3.json')))]))
    tests.append(('R9 Dreiecke je Bild + Spielfigur + Effekte <= 16384 mit Reserve; im Spiel 0 verworfen (G3)', r9,
                  [('alte Grenze 8192', lambda: r9(grenze=8192))]))
    tests.append(('R10 UV-Stufe uvfein: Kinder in der Mutter-UV, keine Linien-Dreiecke wo vermeidbar, Augen-Texel wie K5e_f1', r10,
                  [('B4-UV ohne uvfein', lambda: r10(FB4UV))]))
    for name, fn, gegen in tests:
        g, n = riegel(name, fn, gegen)
        gr += 1 if g else 0
        gp += n
    ng = sum(len(t[2]) for t in tests)
    zeile = 'RIEGEL K5g3 GEOMETRIE: %d von %d gruen, %d von %d Gegenproben rot wie verlangt' % (gr, len(tests), gp, ng)
    log(zeile)
    ERG['summe'] = zeile
    ERG['emd_sha256'] = sha(EMD)
    open(os.path.join(GEO, 'riegel_k5g3.txt'), 'w', encoding='utf-8', newline='\n').write('\n'.join(ZEILEN) + '\n')
    json.dump(ERG, open(os.path.join(GEO, 'riegel_k5g3.json'), 'w', encoding='utf-8', newline='\n'), indent=1, ensure_ascii=False)
    return gr == len(tests) and gp == ng


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == 'textur':
        # K5g3 Textur-Stufe (Auftrag K5g3 Textur-Bauer, 2026-11-14): E3, V1-V6, O3-O5 - riegel_k5g3_tex.py
        import riegel_k5g3_tex as RT
        sys.exit(0 if RT.main(sys.argv[2:]) else 1)
    sys.exit(0 if main() else 1)
