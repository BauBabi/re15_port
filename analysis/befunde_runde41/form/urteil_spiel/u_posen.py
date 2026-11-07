# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: ganze Koerperposen aus der Ruhe (Clip 2), gleiche Winkel je Rohbau, angelehnt an die Tick-Rollen, dazu Schwanzhieb.
Durchblick = Pixel, die nur eine Rueckseite traegt. Typ 0x30 zeichnet Rueckseiten nicht, dort sieht man den Raum.
Schreibt je Rohbau biegepose_<k>.EMD/.TIM in den Scratchpad fuer die Spielprobe."""
import shutil, sys
import cv2
from u_basis import *

SPEICHER = EICH + '/../biege'


def kette(*n):
    return tuple(I[x] for x in n)


BEIN_R = kette('oberschenkel_r', 'unterschenkel_r', 'fuss_r')
BEIN_L = kette('oberschenkel_l', 'unterschenkel_l', 'fuss_l')
ARM_R = kette('oberarm_r', 'unterarm_r', 'hand_r')
ARM_L = kette('oberarm_l', 'unterarm_l', 'hand_l')
SCHWANZ = kette('schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5', 'schwanz6')
W, B = I['wirbel'], I['brust']


def glied(D, ch, a, c, f, x=0):
    D[ch[0]] = Rz(a) @ Rx(x)
    D[ch[1]] = Rz(c)
    D[ch[2]] = Rz(f)


def posen():
    P = {}
    # Gehen: Diagonale Bein r + Arm l hinten, Bein l + Arm r vorn. Schwanz pendelt.
    D = {}
    glied(D, BEIN_R, 24, -8, -16)
    glied(D, ARM_L, 24, -8, -16)
    glied(D, BEIN_L, -34, 48, -14)
    glied(D, ARM_R, -34, -48, 14)
    D[W] = Ry(5)
    D[B] = Ry(5)
    for s in SCHWANZ:
        D[s] = Ry(-6)
    P['gehen'] = D
    # Hieb: rechter Arm hoch, Rumpf richtet sich auf
    D = {}
    D[W] = Rz(-7)
    D[B] = Rz(-8)
    glied(D, ARM_R, -118, -34, 22, x=12)
    glied(D, ARM_L, 12, -6, 0)
    P['hieb'] = D
    # Biss: Rumpf neigt sich vor, Kiefer auf
    D = {}
    D[W] = Rz(9)
    D[B] = Rz(11)
    glied(D, ARM_R, -30, -30, 0)
    glied(D, ARM_L, -30, -30, 0)
    D[I['hals']] = Rz(-14)
    D[I['kopf']] = Rz(-8)
    D[I['kiefer']] = Rz(32)
    P['biss'] = D
    # Drohen: aufrichten um die Wurzel, Arme hoch
    D = {}
    D[0] = Rz(-26)
    D[W] = Rz(-5)
    D[B] = Rz(-6)
    glied(D, BEIN_R, 26, 0, 0)
    glied(D, BEIN_L, 26, 0, 0)
    glied(D, ARM_R, -70, -40, 10, x=-14)
    glied(D, ARM_L, -70, -40, 10, x=14)
    for s in SCHWANZ:
        D[s] = Rz(8)
    P['drohen'] = D
    # Seitenhieb: Rumpf dreht, linker Arm schwingt quer
    D = {}
    D[B] = Ry(16)
    D[W] = Ry(8)
    glied(D, ARM_L, -60, -25, 0, x=62)
    P['seitenhieb'] = D
    # Schwanzhieb: Schwanz schlaegt quer, je Glied 20 Grad, Rumpf dreht gegen
    D = {}
    D[B] = Ry(-10)
    D[W] = Ry(-10)
    for s in SCHWANZ:
        D[s] = Ry(20)
    P['schwanzhieb'] = D
    return P


def blickachsen(az, el):
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)])
    unten = np.cross(vor, rechts)
    if unten[1] < 0 and abs(el) < 80:
        unten = -unten
    return np.stack([rechts, unten, vor])


def durchblick(W_, tris, gross=900, azs=range(0, 360, 30), els=(-35, 0, 20, 40, 65)):
    """Pixel, die nur Rueckseiten tragen, Oeffnung 3 x 3. Gleicher Massstab wie die vorige Pruefung."""
    alle = np.vstack([w for w in W_ if len(w)])
    mitte = (alle.max(0) + alle.min(0)) / 2
    rad = float(np.linalg.norm(alle - mitte, axis=1).max())
    s = gross * 0.46 / rad
    ke = np.ones((3, 3), np.uint8)
    je = {}
    bester = (0, None, None, None)
    for az in azs:
        for el in els:
            A = blickachsen(az, el)
            vorne = np.zeros((gross, gross), np.uint8)
            hinten = np.zeros((gross, gross), np.uint8)
            for w, F in zip(W_, tris):
                if len(F) == 0:
                    continue
                q = (w - mitte) @ A.T
                xy = np.round(q[:, :2] * s * 16 + gross * 8).astype(np.int32)
                t = xy[F]
                d = (t[:, 1, 0] - t[:, 0, 0]) * (t[:, 2, 1] - t[:, 0, 1]) - (t[:, 2, 0] - t[:, 0, 0]) * (t[:, 1, 1] - t[:, 0, 1])
                for tt, dd in zip(t, d):
                    cv2.fillConvexPoly(vorne if dd > 0 else hinten, tt, 1, lineType=cv2.LINE_8, shift=4)
            loch = (hinten > 0) & (vorne == 0)
            loch = cv2.morphologyEx(loch.astype(np.uint8), cv2.MORPH_OPEN, ke)
            n = int(loch.sum())
            je[(az, el)] = n
            if n > bester[0]:
                bester = (n, (az, el), loch.copy(), vorne.copy())
    return je, bester, s


def neu_form(form, R, T, P):
    neu = FE.neu()
    neu['knochen'] = form['knochen']
    neu['meshes'] = form['meshes']
    neu['tim'] = form['tim']
    # Ruhe zuerst, dann die Posen: Clip 0 Ruhe, 1.. Posen
    for nm, D in [('ruhe', {})] + list(P.items()):
        Rn, Tn = gebeugt(form, R, T, D)
        win = []
        for b in range(len(form['knochen'])):
            p = form['knochen'][b]['eltern']
            L = Rn[b] if p < 0 else Rn[p].T @ Rn[b]
            win.append(FE.winkel_aus_matrix(L))
        FE.clip_dazu(neu, nm, [FE.bild(neu, wurzel=tuple(int(round(c)) for c in Tn[0]), winkel=win)])
    FE.boden_nachfuehren(neu, 0)
    return neu


if __name__ == '__main__':
    os.makedirs(SPEICHER, exist_ok=True)
    P = posen()
    erg = {}
    for k in ROHBAU:
        form = lade(k)
        n = len(form['knochen'])
        R, T = pose(form, 2)
        tris = [dreiecke(m) for m in form['meshes'][:n]]
        neu = neu_form(form, R, T, P)
        emd = FE.schreibe_emd(neu, pruefen=False)
        open(SPEICHER + '/biegepose_%s.EMD' % k, 'wb').write(emd)
        shutil.copyfile(K[k][1], SPEICHER + '/biegepose_%s.TIM' % k)
        mo = FE.modell(emd, open(K[k][1], 'rb').read())
        tex = FB.Textur(open(K[k][1], 'rb').read())
        taf = []
        erg[k] = {}
        print('==', k, 'Clips', [c['name'] for c in neu['clips']])
        for ci, c in enumerate(neu['clips']):
            f0, _ = mo.clips[ci]
            kf = mo.frames[f0] & 0xFFF
            R2, T2 = mo.pose(kf)
            Rf = [r.astype(float) / 4096.0 for r in R2]
            Tf = [t.astype(float) for t in T2]
            Wl = welt(form, Rf, Tf)
            je, bst, s = durchblick(Wl, tris)
            ab0 = sum(v for (az, el), v in je.items() if el >= 0)
            un = sum(v for (az, el), v in je.items() if el < 0)
            low = {}
            for nm, ch in (('bein_r', BEIN_R), ('bein_l', BEIN_L), ('arm_r', ARM_R), ('arm_l', ARM_L)):
                tb = teilbaum(form, ch[0])
                low[nm] = round(max(float(Wl[i][:, 1].max()) for i in tb if len(Wl[i])))
            ty = max(float(w[:, 1].max()) for w in Wl if len(w))
            print('  %-12s Durchblick ab 0 Grad %6d  unten %6d  schlechtester %5d %s  tiefster %5.0f  Glieder %s' % (c['name'], ab0, un, bst[0], bst[1], ty, low))
            erg[k][c['name']] = dict(ab0=ab0, unten=un, bester=[bst[0], str(bst[1])], glieder=low, tiefster=round(ty))
            if bst[2] is not None and bst[0] > 200:
                g = np.zeros(bst[2].shape + (3,), np.uint8)
                g[bst[3] > 0] = (150, 150, 150)
                g[bst[2] > 0] = (255, 0, 0)
                cv2.imwrite(AUS + '/loch_%s_%s.png' % (k, c['name']), g[:, :, ::-1])
            for t, az, el in (('3/4 vorn', 50, 18), ('Seite', 180, 4), ('3/4 hinten', -135, 22)):
                a, _ = bild(mo, tex, kf, az, el, 340, gamma=0.6)
                taf.append(('%s %s %s' % (k, c['name'], t), a))
        FB.montage(taf, 3, AUS + '/posen_%s.png' % k, '%s: Koerperposen aus der Ruhe, gleiche Winkel je Rohbau, Gamma 0,6' % k)
    json.dump(erg, open(AUS + '/posen.json', 'w'), indent=1)
