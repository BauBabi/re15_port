"""SF V4 Originaltexturen Runde 2: Gitter ueber die Kopf-PORT-WAHLen von ref2tex_v4.py (Hauthelligkeit vorn, Augenhof) am Riegel
riegel_ref2tex.tk4 (T-K4 / T-K5, Abnahme gegen Referenz 2).

    python ref2tex_gitter.py   -> analysis/befunde_runde41/SF_belege/v4/ot2/ref2tex_gitter.txt

Je Gitterpunkt: Anzahl roter T-K4/T-K5-Zeilen, K1 (Augenkontrast), S3 (Haarkante seitlich) und die uebrigen roten Zeilen. Die uebrigen
Konstanten wie in ref2tex_v4.py. Kein RE-Befund; Messverfahren fuer die PORT-WAHL.
"""
import os, sys, itertools
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import numpy as np
import v4_gemein as G, b2, ref2tex_v4 as RT, birkin_formen as BF, riegel_ref2tex as RR

AUS = os.path.join(G.BAUM, 'analysis', 'befunde_runde41', 'SF_belege', 'v4', 'ot2', 'ref2tex_gitter.txt')
GITTER = dict(S_VORN=[1.2, 1.25], HOF_F=[0.62, 0.68, 0.74], HOF_R=[8.0, 11.0, 14.0], HOF_MIN=[0.0, 29.0])


def main():
    A = RT.atlas()
    namen = [z[0] for z in RT.E.ZONEN]
    P = G.posen(); zk = RT.zonenkarte()
    blob, _ = RT.bauen(); mo = b2.modell(blob)
    form = BF._variante_v4(); kp, sw = form['kopf_v4'], form['schwanz_kette']
    src_v = RT.quelle_kopf(A, zk['kopf_vorn'][0].shape, (0, 0)); src_s = RT.quelle_kopf(A, zk['kopf_seite'][0].shape, (20, 6))
    schwanz = RT.schwanz_textur_r2(A, RT.schwanz_spanne(mo, sw))
    w_v, w_s = RT.augen_gewicht(mo, kp, P, (zk['kopf_vorn'][0].shape, zk['kopf_seite'][0].shape))
    hv = RT.haar_vorn(mo, kp, P, zk['kopf_vorn'][0].shape); hs = RT.haar_seite(mo, kp, P, zk['kopf_seite'][0].shape)
    nv_ = RT.fenster_texel(mo, kp, P, '135', 'KV3', 0, zk['kopf_vorn'][0].shape) & ~hv
    alt = {k: getattr(RT, k) for k in GITTER}
    os.makedirs(os.path.dirname(AUS), exist_ok=True)
    with open(AUS, 'w', encoding='utf-8') as fo:
        fo.write('Gitter %s; fest: %s\n' % (GITTER, {k: getattr(RT, k) for k in ('S_SEITE', 'HAAR_F', 'HAAR_F_V', 'HOF_F_S', 'KNIE', 'KNIE_F',
                                                                                    'HAUT_L2', 'KINN_F', 'NASE_F')}))
        for werte in itertools.product(*GITTER.values()):
            par = dict(zip(GITTER.keys(), werte))
            for k, v in par.items():
                setattr(RT, k, v)
            K, _used, haar_l, _nv = RT.klassen_r2(A)
            bv = RT.kopf_bild_r2(A, K, zk['kopf_vorn'][0], w_v, src_v, namen, 'vorn', RT.S_VORN, haar_l, hv, nv_)
            bs = RT.kopf_bild_r2(A, K, zk['kopf_seite'][0], w_s, src_s, namen, 'seite', RT.S_SEITE, haar_l, hs)
            RT.setze(mo, kp, sw, dict(kopf_vorn=bv, kopf_seite=bs, schwanz=schwanz))
            z, _ = RR.tk4(mo, P)
            rot = [x['mass'].replace('T-K4 ', '') + ': ' + x['ist'] for x in z if not x['ok']]
            ki = {x['mass'].split()[1]: x['ist'] for x in z if x['mass'].startswith('T-K4')}
            zeile = '%s rot %d | K1 %s | S3 %s | %s' % (par, len(rot), ki.get('K1'), ki.get('S3'),
                                                       ' | '.join(r for r in rot if not r.startswith(('K1', 'S3'))))
            print(zeile, flush=True); fo.write(zeile + '\n'); fo.flush()
    for k, v in alt.items():
        setattr(RT, k, v)
    print('geschrieben', AUS)


if __name__ == '__main__':
    main()
