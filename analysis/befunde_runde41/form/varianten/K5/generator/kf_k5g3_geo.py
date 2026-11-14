#!/usr/bin/env python3
"""K5g3 Geometrie-Stufe (Orchestrator-Entscheide G1/G2 vom 2026-11-14) - EIN Aufruf baut die fertige Geometrie der Riesenform.

Eingaenge (alle im Repo, mit sha256 geprueft):
  K5e_f1   K5/ruhe/K5e_f1/BAHNSTEIG_FORM.EMD (sha f277916d..., = Spiel-Asset re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD)
  G1 Spalt generator/k5g3_spalt_b4_tabelle.json = Kopie von scratchpad k5g2/spalt/kand_b4/TABELLE.json (sha 99a4f142...), Netze 10, 16, 21, 22
           (Kandidat B4, vom Nutzer am 14.11. abgenommen). Eingesetzt mit kf_k5g2_geo.geometrie_form(stufen=('mulde',)) - derselbe Weg, den
           die Messbank (spalt/bank.py emd) fuer KAND.EMD geht.
  G2 Krallen generator/k5g3_krallen_k5c.json: Ecken und Normalen der rechten Krallen 19, 20, 28, 29 byte-gleich aus K5c
           (K5/einbau_k5c/BAHNSTEIG_FORM.EMD, sha 1ab56076..., vom Nutzer am 07.11. abgenommen); Flaechen/UV/CLUT/Seite bleiben K5e_f1.
           Netz 31 ist in K5c und K5e_f1 gleich (nur geprueft).
  Posen    vorerst die von K5c. Gemessen: Sektionen 0-6 von K5c (Morph, EDD/EMR aller drei Baenke) sind Byte fuer Byte die von K5e_f1.
           OPTIONAL eine Posen-Datei (Vorgabe generator/krallen_posen.json, sonst --posen <datei>): {"keys": {"<key>": {"alt": {"<knochen>":
           [x,y,z]}, "neu": {...}}}}, nur Keys 398-425 und 542-556 (Lage der rechten Hand in Bild 2, Nutzer-Entscheid offen), nur Knochen des
           rechten Arms (16 und alles darunter: 16, 17, 18, 19, 20, 29, 31). 'alt' muss der Wert in K5c sein (modulo 4096), sonst Abbruch.
  uvfein   (Runde 2, Befund "gestreckte Farbstreifen aus UV-entarteten Dreiecken"): UV der Spalt-Kinder neu gewaehlt, kf_k5g3_uv.uv_fein -
           nur UV; Ecken, Normalen, Flaechen-Indizes, CLUT, Seite bleiben die der B4-Tabelle. Die Tabelle mit diesen UV schreibt `bauen` nach
           K5/k5g/K5g3/geo/spalt_uv_tabelle.json (GEO3_SPALT_UV) - die nimmt die Textur-Stufe fuer mulde_auf_form statt GEO3_SPALT.
Stufen: posen -> krallen -> spalt -> uvfein (Sektion 2 / Netze 19, 20, 28, 29 / Netze 10, 16, 21, 22 / UV der Kinder in 10, 16, 21, 22).
Kein Zufall, keine Uhr: zweiter Lauf ergibt dieselben Bytes (uvfein rechnet in float64 nur die Auswahl, das Ergebnis ist ganzzahlig).

Aufruf:  python kf_k5g3_geo.py bauen [--posen <datei>] [<ziel.EMD>]   -> K5/k5g/K5g3/geo/BAHNSTEIG_FORM.EMD (+ FORM.EMD daneben)
         python kf_k5g3_geo.py pruef                                 -> zwei Laeufe + Datei byte-gleich, Byte-Probe gegen KAND.EMD (Exit 1 sonst)
"""
import hashlib
import json
import os
import sys

sys.dont_write_bytecode = True
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g2_geo as G  # noqa: E402
import kf_k5g3_uv as UVF  # noqa: E402

FE = G.FE
K5 = G.K5
GEO_BASIS, GEO_BASIS_TIM, GEO_BASIS_SHA = G.GEO_BASIS, G.GEO_BASIS_TIM, G.GEO_BASIS_SHA
GEO_FORM_BASIS = os.path.join(K5, 'ruhe', 'K5e_f1', 'FORM.EMD')
GEO3_SPALT = os.path.join(HIER, 'k5g3_spalt_b4_tabelle.json')
GEO3_SPALT_SHA = '99a4f142c2d01c5eb8e70dd299c0cd98c3e34268adc8473454dcfb42b4cd6d07'
GEO3_KRALLEN = os.path.join(HIER, 'k5g3_krallen_k5c.json')
GEO3_K5C_SHA = '1ab56076b20ef022c45a4517d944c0829ff93120124b6c7bb3601c7373736041'   # nur Bericht/Riegel; Inhalt steckt in GEO3_KRALLEN
GEO3_POSEN = os.path.join(HIER, 'krallen_posen.json')                                # optional (G2); fehlt heute
GEO3_ZIEL = os.path.join(K5, 'k5g', 'K5g3', 'geo', 'BAHNSTEIG_FORM.EMD')
GEO3_SPALT_UV = os.path.join(K5, 'k5g', 'K5g3', 'geo', 'spalt_uv_tabelle.json')     # B4-Tabelle mit den UV der Stufe uvfein (Textur-Stufe)
KAND_B4_SHA = 'f78e598336872111de14a7b06ec7cb3610c8777897d30fc3b5bc9f12945c1a66'      # spalt/kand_b4/KAND.EMD (Byte-Probe)
SPALT_TEILE = G.MULDEN_TEILE                    # (10, 16, 21, 22)
KRALLEN_K5C = (19, 20, 28, 29)
KRALLE_GLEICH = 31
POSEN_KEYS = tuple(range(398, 426)) + tuple(range(542, 557))
FORM_KEYS = ((0, 11), (1, 542))                 # kleine FORM.EMD: Key 0 = BAHNSTEIG-Key 11 (Bild 1), Key 1 = Key 542 (Bild 2), Key 2 Ruhe
STUFEN = ('posen', 'krallen', 'spalt', 'uvfein')


def sha(b):
    return hashlib.sha256(b).hexdigest()


def arm_rechts(form):
    """Knochen 16 und alle darunter (rechter Arm samt Hand und Krallen)."""
    aus, neu = {16}, True
    while neu:
        neu = False
        for i, k in enumerate(form['knochen']):
            if k['eltern'] in aus and i not in aus:
                aus.add(i)
                neu = True
    return tuple(sorted(aus))


def _u12(x):
    return int(x) & 0xFFF


def posen_form(form, posen):
    """Posen-Datei (G2) in den Pool der Form. Rueckgabe: Liste (key, knochen, alt, neu) der geschriebenen Tripel."""
    erlaubt_k = arm_rechts(form)
    keys = sorted(int(k) for k in posen['keys'])
    fremd = [k for k in keys if k not in POSEN_KEYS]
    if fremd:
        raise SystemExit('Posen-Datei: Keys ausserhalb 398-425 / 542-556: %s' % fremd[:10])
    pool, aus = form['pool'], []
    for key in keys:
        e = posen['keys'][str(key)]
        if sorted(e.get('alt', {})) != sorted(e['neu']):
            raise SystemExit('Posen-Datei Key %d: alt und neu nennen verschiedene Knochen' % key)
        w = pool[key]['winkel']
        for t in sorted(int(x) for x in e['neu']):
            if t not in erlaubt_k:
                raise SystemExit('Posen-Datei Key %d: Knochen %d gehoert nicht zum rechten Arm %s' % (key, t, erlaubt_k))
            ist = [_u12(x) for x in w[t]]
            alt = [_u12(x) for x in e['alt'][str(t)]]
            if ist != alt:
                raise SystemExit('Posen-Datei passt nicht zur Form: Key %d Knochen %d ist %r, "alt" sagt %r' % (key, t, ist, alt))
            neu = [_u12(x) for x in e['neu'][str(t)]]
            w[t] = [n - 4096 if n > 2047 else n for n in neu]
            aus.append((key, t, ist, neu))
    return aus


def krallen_form(form, rezept):
    """G2: Ecken und Normalen der Netze 19, 20, 28, 29 aus K5c; Flaechen bleiben (Eckenzahl muss passen). Netz 31 nur pruefen."""
    for t in KRALLEN_K5C:
        q, m = rezept['teile'][str(t)], form['meshes'][t]
        if len(q['ecken']) != len(m['ecken']) or len(q['normalen']) != len(m['normalen']):
            raise SystemExit('Kralle %d: Ecken-/Normalenzahl von K5c passt nicht zu K5e_f1' % t)
        m['ecken'] = [[int(x) for x in e] for e in q['ecken']]
        m['normalen'] = [[int(x) for x in n] for n in q['normalen']]
    g = rezept['geprueft_gleich'][str(KRALLE_GLEICH)]
    m = form['meshes'][KRALLE_GLEICH]
    if m['ecken'] != g['ecken'] or m['normalen'] != g['normalen']:
        raise SystemExit('Kralle 31: K5e_f1 ist nicht mehr gleich K5c')
    return form


def posen_laden(posen):
    if posen is None:
        posen = GEO3_POSEN if os.path.exists(GEO3_POSEN) else ''
    if not posen:
        return None, None
    return json.load(open(posen, encoding='utf-8')), posen


def geometrie_form(stufen=STUFEN, posen=None, tabelle=GEO3_SPALT, krallen=GEO3_KRALLEN):
    """K5e_f1 -> (Bytes fuer die Sektionen 0-6, Form mit der fertigen Geometrie, Bericht). stufen erlaubt Teilstaende fuer Gegenproben."""
    emd0 = open(GEO_BASIS, 'rb').read()
    if sha(emd0) != GEO_BASIS_SHA:
        raise SystemExit('Basis ist nicht K5e_f1')
    ber = dict(stufen=list(stufen), basis=GEO_BASIS_SHA)
    if 'spalt' in stufen:
        if tabelle == GEO3_SPALT and sha(open(tabelle, 'rb').read()) != GEO3_SPALT_SHA:
            raise SystemExit('Spalt-Tabelle hat nicht die sha256 aus G1 (%s)' % GEO3_SPALT_SHA)
        _e, form = G.geometrie_form(emd0=emd0, tabelle=tabelle, stufen=('mulde',))     # Netze 10, 16, 21, 22 aus der Tabelle (Basis-/Teileprobe dort)
        ber['spalt'] = dict(tabelle=os.path.basename(tabelle), sha256=sha(open(tabelle, 'rb').read()))
        if 'uvfein' in stufen:
            tuv, ub = spalt_uv(tabelle, emd0)
            for t in SPALT_TEILE:
                form['meshes'][t] = tuv['teile'][str(t)]
            ber['uvfein'] = dict((k, v) for k, v in ub.items() if k != 'je_netz')
    elif 'uvfein' in stufen:
        raise SystemExit('Stufe uvfein braucht die Stufe spalt')
    else:
        form = FE.lese_emd(emd0)
    if 'krallen' in stufen:
        rez = json.load(open(krallen, encoding='utf-8'))
        if rez['basis']['sha256'] != GEO_BASIS_SHA or rez['quelle']['sha256'] != GEO3_K5C_SHA:
            raise SystemExit('Krallen-Datei gehoert zu einer anderen Basis oder nicht zu K5c')
        krallen_form(form, rez)
        ber['krallen'] = dict(datei=os.path.basename(krallen), sha256=sha(open(krallen, 'rb').read()), quelle_k5c=rez['quelle']['sha256'])
    sek = emd0
    if 'posen' in stufen:
        p, pfad = posen_laden(posen)
        if p is None:
            ber['posen'] = dict(datei=None, hinweis='keine Posen-Datei: Posen = K5c = K5e_f1 (Sektionen 0-6 byte-gleich)')
        else:
            pf = FE.lese_emd(emd0)                  # Posen auf der K5e_f1-Form schreiben, daraus nur die Sektionen 0-6 nehmen
            geschr = posen_form(pf, p)
            sek = FE.schreibe_emd(pf)
            form['pool'] = pf['pool']
            ber['posen'] = dict(datei=pfad.replace(chr(92), '/'), sha256=sha(open(pfad, 'rb').read()), tripel=len(geschr),
                                keys=sorted(set(k for k, _t, _a, _n in geschr)))
    return sek, form, ber


def spalt_uv(tabelle=GEO3_SPALT, emd0=None):
    """B4-Tabelle -> (Tabelle mit den UV der Stufe uvfein, Bericht). Deterministisch (kf_k5g3_uv.uv_fein)."""
    emd0 = emd0 or open(GEO_BASIS, 'rb').read()
    return UVF.uv_fein(json.load(open(tabelle, encoding='utf-8')), FE.lese_emd(emd0))


def spalt_uv_bytes(tabelle=GEO3_SPALT):
    t, _b = spalt_uv(tabelle)
    return (json.dumps(t, sort_keys=True, separators=(',', ':')) + chr(10)).encode('utf-8')


def geometrie_bytes(stufen=STUFEN, posen=None, tabelle=GEO3_SPALT, krallen=GEO3_KRALLEN):
    sek, form, _b = geometrie_form(stufen=stufen, posen=posen, tabelle=tabelle, krallen=krallen)
    return G.emd_mit_md1(sek, form)


def form_emd_bytes(bahn, form_basis=None):
    """Kleine FORM.EMD (3 Keys, Werkzeug-Artefakt der Tafeln/Messungen): MD1 = das der Geometrie; Key 0/1 = BAHNSTEIG-Keys 11/542 der
    Geometrie, und zwar nur die WINKEL (gemessen: in K5e_f1 sind Winkel und Wurzel von FORM-Key 0/1 gleich BAHNSTEIG-Key 11/542; die Spur
    von Key 0 ist 0 statt 3959 - die Posen-Datei aendert nur Winkel), Key 2 (Ruhe) bleibt."""
    fb = open(form_basis or GEO_FORM_BASIS, 'rb').read()
    f0, b0 = FE.lese_emd(fb), FE.lese_emd(open(GEO_BASIS, 'rb').read())
    for k, kb in FORM_KEYS:
        if f0['pool'][k]['winkel'] != b0['pool'][kb]['winkel'] or f0['pool'][k]['wurzel'] != b0['pool'][kb]['wurzel']:
            raise SystemExit('FORM.EMD Key %d ist nicht BAHNSTEIG-Key %d von K5e_f1' % (k, kb))
    bn = FE.lese_emd(bahn)
    if all(bn['pool'][kb]['winkel'] == b0['pool'][kb]['winkel'] for _k, kb in FORM_KEYS):
        sek = fb
    else:
        for k, kb in FORM_KEYS:
            f0['pool'][k]['winkel'] = [list(w) for w in bn['pool'][kb]['winkel']]
        sek = FE.schreibe_emd(f0)
    return G.emd_mit_md1(sek, bn)


def bauen(ziel=None, posen=None, log=print):
    ziel = ziel or GEO3_ZIEL
    sek, form, ber = geometrie_form(posen=posen)
    aus = G.emd_mit_md1(sek, form)
    os.makedirs(os.path.dirname(ziel), exist_ok=True)
    open(ziel, 'wb').write(aus)
    fz = os.path.join(os.path.dirname(ziel), 'FORM.EMD')
    fa = form_emd_bytes(aus)
    open(fz, 'wb').write(fa)
    ber.update(emd=dict(datei=os.path.basename(ziel), bytes=len(aus), sha256=sha(aus)),
               form_emd=dict(datei='FORM.EMD', bytes=len(fa), sha256=sha(fa)),
               tim='K5/ruhe/K5e_f1/BAHNSTEIG_FORM.TIM (sha %s)' % sha(open(GEO_BASIS_TIM, 'rb').read()))
    if 'uvfein' in ber:
        tb = spalt_uv_bytes()
        open(os.path.join(os.path.dirname(ziel), os.path.basename(GEO3_SPALT_UV)), 'wb').write(tb)
        ber['spalt_uv_tabelle'] = dict(datei=os.path.basename(GEO3_SPALT_UV), sha256=sha(tb))
    json.dump(ber, open(os.path.join(os.path.dirname(ziel), 'bau.json'), 'w', encoding='utf-8', newline='\n'), indent=1, sort_keys=True)
    log('EMD %s  %d Byte  sha256 %s' % (ziel.replace(chr(92), '/'), len(aus), sha(aus)))
    log('FORM.EMD %d Byte  sha256 %s' % (len(fa), sha(fa)))
    log('Posen: %s' % ber['posen'])
    return ziel, sha(aus)


def pruef(ziel=None):
    ziel = ziel or GEO3_ZIEL
    a, b = geometrie_bytes(), geometrie_bytes()
    d = open(ziel, 'rb').read() if os.path.exists(ziel) else b''
    gut1 = a == b == d
    print('zwei Laeufe und Datei byte-gleich: %s  sha256 %s' % ('JA' if gut1 else 'NEIN', sha(a)))
    k = geometrie_bytes(stufen=('spalt',), posen='')
    gut2 = sha(k) == KAND_B4_SHA
    print('Byte-Probe K5e_f1-Posen + K5e_f1-Krallen + B4-Tabelle == KAND.EMD (f78e5983...): %s  sha256 %s' % ('JA' if gut2 else 'NEIN', sha(k)))
    tb = spalt_uv_bytes()
    d3 = open(GEO3_SPALT_UV, 'rb').read() if os.path.exists(GEO3_SPALT_UV) else b''
    gut3 = tb == d3
    print('UV-Tabelle (uvfein) == geo/%s: %s  sha256 %s' % (os.path.basename(GEO3_SPALT_UV), 'JA' if gut3 else 'NEIN', sha(tb)))
    return gut1 and gut2 and gut3


if __name__ == '__main__':
    arg = sys.argv[1:]
    was = arg.pop(0) if arg else ''
    pos = None
    if '--posen' in arg:
        i = arg.index('--posen')
        pos = arg[i + 1]
        del arg[i:i + 2]
    if was == 'bauen':
        bauen(arg[0] if arg else None, posen=pos)
    elif was == 'pruef':
        sys.exit(0 if pruef(arg[0] if arg else None) else 1)
    else:
        raise SystemExit('Aufruf: python kf_k5g3_geo.py bauen [--posen <datei>] [<ziel.EMD>] | pruef [<ziel.EMD>]')
