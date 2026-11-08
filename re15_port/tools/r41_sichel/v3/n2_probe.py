# SF V3 Runde 2: Proben fuer Kopf V3 ohne Dateiaenderung - KOPF_V3/Zeile 486 im Speicher abwandeln, V3 bauen (Scratchpad n2/probe),
# Kopf-Riegel R1-R14 (riegel_v3.kopf) und Spielkamera-Masse (n2_maul_spiel) ausgeben. Belegt die "Probe"-Angaben im Generator.
# Aufruf: python n2_probe.py NAME 'maul.h=0.056' 'zeile.5=13.5,13.7,4.8' 'hell=0.85' ...
#   maul.X / auge.X / zonen.X / seite.X / schulterauge.X = Wert (Python-Literal); zeile.I=L,a,b (absolut Lab) oder zeile.I=ki:L,a,b (x HELL)
import os, sys, json, ast
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'v2')); sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import importlib.util, re
_hell = [a for a in sys.argv[2:] if a.split('=', 1)[0] in ('hell', 'hell_seite') or a.startswith('src:')]
if _hell:
    # KOPF_V3_HELL / KOPF_V3_HELL_SEITE gehen beim Import in die Zeile 486 ein: Modul aus dem abgewandelten Quelltext laden
    _src = open(os.path.join(os.path.dirname(os.path.dirname(HIER)), 'birkin_formen.py'), encoding='utf-8').read()
    for a in sys.argv[2:]:
        if a.startswith('src:'):                        # src:ALT~~NEU = Quelltext-Ersetzung (z. B. Knick-Grenze)
            alt, neu = a[4:].split('~~')
            assert alt in _src, alt
            _src = _src.replace(alt, neu)
            continue
        k, v = a.split('=', 1)
        if k == 'hell':
            _src = re.sub(r'^KOPF_V3_HELL = .*$', 'KOPF_V3_HELL = ' + v, _src, flags=re.M)
        if k == 'hell_seite':
            _src = re.sub(r'^KOPF_V3_HELL_SEITE = .*$', 'KOPF_V3_HELL_SEITE = ' + v, _src, flags=re.M)
    _spec = importlib.util.spec_from_loader('birkin_formen', loader=None)
    BF = importlib.util.module_from_spec(_spec)
    BF.__file__ = os.path.join(os.path.dirname(os.path.dirname(HIER)), 'birkin_formen.py')
    sys.modules['birkin_formen'] = BF
    exec(compile(_src, BF.__file__, 'exec'), BF.__dict__)
else:
    import birkin_formen as BF
OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/n2/probe/'


def anwenden(args):
    tab = dict(BF.KOPF_V3_ZEILE['tabelle'])
    for a in args:
        if a.startswith('src:'):
            continue
        k, v = a.split('=', 1)
        if k in ('hell', 'hell_seite') or a.startswith('src:'):
            continue
        elif k.startswith('zeile.'):
            i = int(k.split('.')[1])
            if v.startswith('ki:'):
                tab[i] = BF._ki(*[float(x) for x in v[3:].split(',')])
            elif v.startswith('rgb:'):
                tab[i] = tuple(int(x) for x in v[4:].split(','))
            else:
                tab[i] = BF._lab_rgb(*[float(x) for x in v.split(',')])
        else:
            gr, sch = k.split('.')
            BF.KOPF_V3[gr] = dict(BF.KOPF_V3[gr], **{sch: ast.literal_eval(v)})
    BF.KOPF_V3_ZEILE['tabelle'] = sorted(tab.items())


if __name__ == '__main__':
    name = sys.argv[1]
    anwenden(sys.argv[2:])
    os.makedirs(OUT, exist_ok=True)
    blob = BF.baue_v3(BF.lade_em30())
    p = OUT + 'EM30_%s.EMD' % name
    open(p, 'wb').write(blob)
    import riegel_v3 as RV, n2_maul_spiel as MS
    k = RV.kopf(p, OUT + name)
    z = RV.zeilen(k, None, None) if False else None
    ka, sa, mu = k.get('kopfauge', {}), k.get('schulterauge', {}), k.get('maul', {})
    print(name, 'R1', k['R1']['kopfaugen'], [f['mitte'] for f in k['R1']['flecken']][:3])
    if ka:
        print('  R3 %.4f x %.4f R4 %.2f R5 %.3f R6 %.3f/%.3f R7 %.3f/%.3f/%s R8 %.3f' % (ka['b_H'], ka['h_H'], ka['bh'], ka['L_auge_haut'], ka['lid_hell'],
              ka['L_lid_haut'], ka['anteil_L80'], ka['anteil_heller_haut'], ka['glanz_rel_box'], ka['L_pup_haut']))
        print('  R2 %s R9 %.3f/%.3f/%.2f' % (k['R2'], sa['L_auge_haut'], sa['anteil_heller_haut'], sa['aequiv_H'] / ka['aequiv_H']))
        print('  R10 %.4f x %.4f R11 %.2f R12 %s R13 %s R14 %d/%d b %.4f L %.1f (Haut %.1f) a %.1f' % (mu['b_H'], mu['h_H'], mu['bh'], k['R12'], k['R13'],
              mu['oben'], mu['unten'], mu['zahn_b_max_H'], mu['lab_zahn'][0] if mu['lab_zahn'] else 0, k['haut_auge'][0],
              mu['lab_zahn'][1] if mu['lab_zahn'] else 0))
    for h in (118, 125):
        z = MS.port(p, h)
        print('  Spiel h%d Feld/Nachbar %.3f Glanz %d schwarz0,25 %s dunkel0,5 h %.4f' % (h, z['feld_nachbar'], z['glanz_px'], z['schwarz_0.25'], z['dunkel_h_H']))
