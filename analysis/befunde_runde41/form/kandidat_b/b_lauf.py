#!/usr/bin/env python3
"""Kandidat B: ein Durchgang. Fit + Quad-Netze -> EMD, TIM, Kandidat-JSON, Pruefung.
    python b_lauf.py --fit zw/fit2.pt --quad zw/quad2 --aus zw/v2
"""
import argparse, json, math, os, sys, time
import numpy as np, cv2, torch
from b_geom import *
import b_teile as T
import b_bau as BB
import b_mal as M
import b_mesh as BM
sys.path.insert(0, BAUM + '/re15_port/tools')
import form_emd as FE

THETA = 25.0                     # Azimut der Naht in Pose 1, gemessen: Z1 25-29, Z2 21, Z3 28, Z4 22 Grad


def ry_engine(deg):
    a = math.radians(deg)
    c, s = math.cos(a), math.sin(a)
    return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])


def teiler(c, theta, front=260.0, hinten=-520.0, ring=85.0):
    d = np.array([math.cos(math.radians(theta)), 0, math.sin(math.radians(theta))])

    def f(mi):
        s = (mi - c) @ d
        x = mi[:, 0] - c[0]
        return [(T.IDX['rumpf'], s > front - ring), (T.IDX['rumpf_f'], x < hinten + ring), (T.IDX['rumpf_m'], (s < front + ring) & (x > hinten - ring))]
    return f


def lauf(fit_pfad, quad, aus, malen=None, pruefen=True, rand=2, dichte=None, ruhe_w=None, theta=None, lage1=None, gier1=None, snap=None, leise=False):
    os.makedirs(aus, exist_ok=True)
    fit = BM.lade(fit_pfad)
    J = np.asarray(fit['J'], float)
    c = np.asarray(T.KUPPEL_MITTE, float)
    if theta is None:
        d = (fit['naht']['P'][2:9] - c).mean(0) if 'naht' in fit else np.array([math.cos(math.radians(THETA)), 0, math.sin(math.radians(THETA))])
        theta = math.degrees(math.atan2(d[2], d[0]))
    print('Nahtazimut %.1f Grad' % theta)
    netze = BB.lade_netze(quad, teiler(c, theta), snap)
    n = len(T.KNOCHEN)
    Mm = {T.IDX[k]: ry_engine(-theta) for k in ('rumpf', 'maul_l', 'maul_r')}
    w1 = np.zeros((n, 3))
    w2 = np.asarray(fit['w2'], float)
    wr = np.zeros((n, 3)) if ruhe_w is None else np.asarray(ruhe_w, float)
    B, H = 384, 256
    import b_pack
    tex, dd = b_pack.packe(netze, B, H, rand=1, dichte_je=dichte or {'rumpf': 1.2, 'maul_l': 1.3, 'maul_r': 1.3, 'fl_ober': 0.7, 'fl_unter': 0.7, 'fl_fuss': 0.7, 'fl_zeh': 0.7})
    print('Atlas-Dichte %.4f Texel je Einheit' % dd)
    pos, nrm, bone = BB.texel_karten(netze, tex, B, H)
    belegt = bone >= 0
    R1, T1 = BB.fk(w1, J, T.ELTERN, 0.0)
    R2, T2 = BB.fk(w2, J, T.ELTERN, fit['dy2'])
    Rr, Tr = BB.fk(wr, J, T.ELTERN, 0.0)
    # Ruhe: Rumpf und Naht bleiben wie in Pose 1, kein Verdrehen
    posen = [('pose1', [dict(R=R1, T=T1)]), ('pose2', [dict(R=R2, T=T2)]), ('ruhe', [dict(R=Rr, T=Tr)])]
    form = BB.form_bauen(netze, tex, J, posen, Mm)
    wy = [cl['bilder'][0]['wurzel'][1] for cl in form['clips']]
    lage1 = O1 if lage1 is None else np.array(lage1, float)
    gier1 = G1 if gier1 is None else int(gier1)
    lage2 = np.asarray(fit['lage2'], float)
    gier2 = int(round(fit['gier2'])) % 4096
    R1, T1 = BB.fk(w1, J, T.ELTERN, wy[0] - J[0][1])
    R2, T2 = BB.fk(w2, J, T.ELTERN, wy[1] - J[0][1])
    s1 = M.Sicht(1, K1, R1, T1, J, lage1, gier1)
    s2 = M.Sicht(2, K2, R2, T2, J, lage2, gier2)
    s1.tiefenkarte(netze)
    s2.tiefenkarte(netze)
    xy1, si1 = s1.texel(pos, nrm, bone)
    xy2, si2 = s2.texel(pos, nrm, bone)
    sichten = {1: dict(xy=xy1, sicht=si1), 2: dict(xy=xy2, sicht=si2)}
    if malen is None:
        rgb = M.haut(pos, nrm, belegt)
        rgb = M.stellen_malen(rgb, sichten)
    else:
        rgb = malen(pos, nrm, bone, belegt, sichten, netze, dict(s1=s1, s2=s2, fit=fit, J=J))
    rgb = BB.rand_fuellen(rgb, belegt, 3)
    px, clut = M.quantisieren(rgb, belegt)
    cv2.imwrite(os.path.join(aus, 'textur.png'), cv2.resize(np.clip(rgb, 0, 255).astype(np.uint8)[:, :, ::-1], None, fx=2, fy=2, interpolation=cv2.INTER_NEAREST))
    with open(os.path.join(aus, 'FORM.EMD'), 'wb') as h:
        h.write(FE.schreibe_emd(form))
    with open(os.path.join(aus, 'FORM.TIM'), 'wb') as h:
        h.write(FE.tim_schreiben(px, [clut]))
    FE.speichern(form, os.path.join(aus, 'form.json'))
    kj = dict(name='B', emd='FORM.EMD', tim='FORM.TIM', bild1=dict(lage=[int(round(q)) for q in lage1], gier=gier1), bild2=dict(lage=[int(round(q)) for q in lage2], gier=gier2), ruhe=dict(clip=2, bild=0))
    pfad = os.path.join(aus, 'kandidat.json')
    with open(pfad, 'w') as h:
        json.dump(kj, h, indent=1)
    F = sum(len(m['dreiecke']) + len(m['vierecke']) for m in form['meshes'])
    print('Flaechen', F, 'Knochen', len(form['knochen']), 'Lage2', kj['bild2'])
    print(' '.join('%d:%d' % (i, len(m['dreiecke']) + len(m['vierecke'])) for i, m in enumerate(form['meshes'])))
    if pruefen:
        BB.pruefe(pfad, aus)
        zeile(aus)
    return form


def zeile(aus):
    R = json.load(open(os.path.join(aus, 'bericht.json'), encoding='utf-8'))
    for n in (1, 2):
        B = R['bild%d' % n]
        z = lambda L: '%d/%d' % (sum(1 for x in L if x['zaehlt'] and x['ok']), sum(1 for x in L if x['zaehlt']))
        print('Bild %d IoU %.3f weiss %.3f schwarz %d | Punkte %s Flaechen %s Zeichnung %s Verbote %s | MAE %.1f NCC %.3f' % (n, B['umriss']['iou'], B['umriss']['weiss_gedeckt'], B['umriss']['schwarz_getroffen'], z(B['punkte']), z(B['flaechen']), z(B['zeichnung']), z(B['verbote']), B['mae_weiss'], B['ncc_weiss']))
        f = []
        for nm in ('punkte', 'flaechen', 'zeichnung', 'verbote'):
            for x in B[nm]:
                if x['zaehlt'] and not x['ok']:
                    d = x.get('abstand')
                    f.append('%s%s' % (x['id'], '' if d is None else '(%+.0f/%.0f)' % (d, x['grenze'])))
        print('   fehlt:', ' '.join(f))
    print('Rahmen:', R['kennzahlen']['rahmen'], 'Warnungen', len(R['kennzahlen']['warnungen']))


if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--fit', required=True)
    ap.add_argument('--quad', required=True)
    ap.add_argument('--aus', required=True)
    a = ap.parse_args()
    t0 = time.time()
    lauf(a.fit, a.quad, a.aus)
    print('%.0fs' % (time.time() - t0))
