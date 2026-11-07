"""Pruefer V3: drei Stichproben der Teile-Zuordnung K5c <- G4 mit eigenem Verfahren (nicht Leser A):
Zylinder-Parameter (theta um die Hauptachse, t entlang) je Teil; jede K5c-Flaeche nimmt die affine Abbildung des G4-Dreiecks,
das ihren Schwerpunkt enthaelt, fuer ALLE Ecken (keine Inselsuche je Ecke). UV, CLUT, TPAGE aus G4; Geometrie unveraendert.
Render: eigener Ortho-Renderer, gleiches Licht und gleicher Weltmassstab fuer G4-Teil, K5c-Teil (G4-Texel) und K5c-Teil (heute)."""
import sys, os, math, copy, json
import numpy as np, cv2
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/formfarbe/A')
from lade import *
from PIL import Image, ImageDraw
from szene import lab

M = modelle()
G4, G4TIM, _ = M['G4']; K5, K5TIM, _ = M['K5c']
TG = FB.Textur(G4TIM); TK = FB.Textur(K5TIM)
KF_G = G4.frames[G4.clips[0][0]] & 0xFFF
KF_K = K5.frames[K5.clips[2][0]] & 0xFFF


def welt(mo, kf, bones):
    """Eckpunkte und Flaechen je Knochen in Weltlage (Ruhepose)."""
    R, T = mo.pose(kf)
    out = []
    for b in bones:
        m = mo.meshes[b]
        W = R[b].astype(float) / 4096.0; t = T[b].astype(float)
        for (verts, norms, faces, uvs, q) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = np.array([x[:3] for x in verts], float) @ W.T + t
            out.append((b, q, v, faces, uvs))
    return out


def flaeche(teile):
    s = 0.0
    for b, q, v, faces, uvs in teile:
        for g in faces:
            ie = g[1::2]
            sets = ((0, 1, 3), (0, 3, 2)) if q else ((0, 1, 2),)
            for a, bb, c in sets:
                s += 0.5 * np.linalg.norm(np.cross(v[ie[bb]] - v[ie[a]], v[ie[c]] - v[ie[a]]))
    return s


def rahmen(teile, wurzel):
    P_ = np.vstack([v for _, _, v, _, _ in teile])
    c = P_.mean(0)
    w, U = np.linalg.eigh(np.cov((P_ - c).T))
    a = U[:, -1]
    if np.dot(c - wurzel, a) < 0:
        a = -a
    ref = np.array([0, -1.0, 0]) if abs(a[1]) < 0.7 else np.array([1.0, 0, 0])
    r0 = ref - a * np.dot(ref, a); r0 /= np.linalg.norm(r0); r1 = np.cross(a, r0)
    s = (P_ - c) @ a
    return c, a, r0, r1, s.min(), s.max()


def param(v, fr):
    c, a, r0, r1, smin, smax = fr
    d = v - c
    t = ((d @ a) - smin) / (smax - smin)
    th = np.arctan2(d @ r1, d @ r0)
    return np.stack([th, t], 1)


def quell_dreiecke(teile, fr):
    """G4: Dreiecke mit (theta,t) je Ecke (entfaltet), UV je Ecke (Atlas-x mit Seite), CLUT-Zeile, uv-Rohwerte."""
    D = []
    for b, q, v, faces, uvs in teile:
        pt = param(v, fr)
        for g, uv in zip(faces, uvs):
            ie = g[1::2]
            po = (uv[5] & 0xF) * 128
            tx = [(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])] + ([(uv[9], uv[10])] if q else [])
            sets = ((0, 1, 3), (0, 3, 2)) if q else ((0, 1, 2),)
            for s3 in sets:
                p = pt[[ie[i] for i in s3]].copy()
                for i in (1, 2):
                    p[i, 0] = p[0, 0] + (p[i, 0] - p[0, 0] + math.pi) % (2 * math.pi) - math.pi
                D.append((p, np.array([tx[i] for i in s3], float), uv[2], uv[5]))
    return D


def zuordnen(Kteile, Gteile, fr_k, fr_g):
    D = quell_dreiecke(Gteile, fr_g)
    Pq = np.array([d[0] for d in D])            # n,3,2
    neu = {}
    stat = dict(flaechen=0, im_dreieck=0, naechstes=0, kante_gt20=0)
    for b, q, v, faces, uvs in Kteile:
        pt = param(v, fr_k)
        liste = []
        for g, uv in zip(faces, uvs):
            ie = list(g[1::2])
            p = pt[ie].copy()
            for i in range(1, len(ie)):
                p[i, 0] = p[0, 0] + (p[i, 0] - p[0, 0] + math.pi) % (2 * math.pi) - math.pi
            cen = p.mean(0)
            best, bw = None, None
            for sh in (0.0, 2 * math.pi, -2 * math.pi):
                x = cen + np.array([sh, 0])
                A = Pq[:, 1] - Pq[:, 0]; B = Pq[:, 2] - Pq[:, 0]; Cc = x - Pq[:, 0]
                det = A[:, 0] * B[:, 1] - A[:, 1] * B[:, 0]
                ok = np.abs(det) > 1e-12
                l1 = np.where(ok, (Cc[:, 0] * B[:, 1] - Cc[:, 1] * B[:, 0]) / np.where(ok, det, 1), -9)
                l2 = np.where(ok, (A[:, 0] * Cc[:, 1] - A[:, 1] * Cc[:, 0]) / np.where(ok, det, 1), -9)
                inn = ok & (l1 >= -1e-6) & (l2 >= -1e-6) & (l1 + l2 <= 1 + 1e-6)
                if inn.any():
                    best, bw = int(np.nonzero(inn)[0][0]), sh
                    break
            stat['flaechen'] += 1
            if best is None:
                dd = np.linalg.norm(Pq.mean(1) - cen, axis=1)
                best, bw = int(np.argmin(dd)), 0.0
                stat['naechstes'] += 1
            else:
                stat['im_dreieck'] += 1
            pq, tq, clut, tp = D[best]
            A = np.array([pq[1] - pq[0], pq[2] - pq[0]]).T
            Ainv = np.linalg.pinv(A)
            uvn = []
            for i in range(len(ie)):
                l = Ainv @ (p[i] + np.array([bw, 0]) - pq[0])
                u = tq[0] + l[0] * (tq[1] - tq[0]) + l[1] * (tq[2] - tq[0])
                uvn.append(np.clip(np.round(u), 0, [127, 255]).astype(int))
            uvn = np.array(uvn)
            kmax = max(np.abs(uvn[i] - uvn[j]).max() for i in range(len(ie)) for j in range(i + 1, len(ie)))
            stat['kante_gt20'] += int(kmax > 20)
            if q:
                liste.append((int(uvn[0][0]), int(uvn[0][1]), clut, int(uvn[1][0]), int(uvn[1][1]), tp, int(uvn[2][0]), int(uvn[2][1]), 0, int(uvn[3][0]), int(uvn[3][1]), 0))
            else:
                liste.append((int(uvn[0][0]), int(uvn[0][1]), clut, int(uvn[1][0]), int(uvn[1][1]), tp, int(uvn[2][0]), int(uvn[2][1]), 0))
        neu.setdefault(b, {})[q] = liste
    return neu, stat


def bild(mo, kf, bones, tex, az, el, s, G=300, mitte=None):
    R, T = mo.pose(kf)
    a, e = math.radians(az), math.radians(el)
    vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
    rechts = np.array([math.cos(a), 0.0, math.sin(a)]); unten = np.cross(vor, rechts)
    if unten[1] < 0:
        unten = -unten
    A = np.stack([rechts, unten, vor])
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    tl = []
    for b in bones:
        m = mo.meshes[b]; W = R[b].astype(float) / 4096.0; t = T[b].astype(float)
        for (verts, norms, faces, uvs, q) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if faces:
                v = np.array([x[:3] for x in verts], float) @ W.T + t
                nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ W.T
                nn /= np.maximum(np.linalg.norm(nn, axis=1, keepdims=True), 1e-9)
                tl.append((v, nn, faces, uvs, q))
    allp = np.vstack([x[0] for x in tl])
    if mitte is None:
        mitte = (allp.max(0) + allp.min(0)) / 2
    aus = []
    for v, nn, faces, uvs, q in tl:
        qq = (v - mitte) @ A.T
        sx = G / 2 + qq[:, 0] * s; sy = G / 2 + qq[:, 1] * s
        hell = np.clip(0.35 + 0.65 * np.maximum(nn @ L, 0), 0, 1) * 255.0
        for g, uv in zip(faces, uvs):
            ie, inn = g[1::2], g[0::2]
            p = [(float(sx[i]), float(sy[i])) for i in ie]
            m0 = FB._mac0(p[0], p[1], p[2])
            if (m0 < 0) if q else (m0 <= 0):
                continue
            po = (uv[5] & 0xF) * 128; z = (uv[2] >> 6) & 0x1FF
            c = [np.full(3, hell[i]) for i in inn]
            if q:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                    aus.append((float(qq[ie[x], 2] + qq[ie[y], 2] + qq[ie[zz], 2]) / 3, (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz]), z, (c[x], c[y], c[zz])))
            else:
                tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                aus.append((float(qq[ie[0], 2] + qq[ie[1], 2] + qq[ie[2], 2]) / 3, tuple(p), tuple(tx), z, tuple(c)))
    img = np.full((G, G, 3), 40, np.uint8)
    m = FB.rastern(aus, tex, img, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    return img, np.asarray(m, bool)


def masse(img, m):
    L = lab(img.astype(np.float32))
    X = L[m]
    C = np.hypot(X[:, 1], X[:, 2]); h = np.degrees(np.arctan2(X[:, 2], X[:, 1]))
    rot = (X[:, 1] > 15) & (C > 20) & (h > -40) & (h < 70)
    Li = np.round(L[..., 0])
    pa = []
    for dy, dx in ((0, 1), (1, 0)):
        a = m[:m.shape[0] - dy, :m.shape[1] - dx] & m[dy:, dx:]
        d = np.abs(Li[:Li.shape[0] - dy, :Li.shape[1] - dx] - Li[dy:, dx:])[a]
        pa.append(d)
    d = np.concatenate(pa)
    return dict(px=int(m.sum()), lab=[round(float(x), 1) for x in X.mean(0)], dunkel_L20=round(100 * float((X[:, 0] < 20).mean()), 1),
                rot=round(100 * float(rot.mean()), 1), rot_ton_median=round(float(np.median(h[rot])), 1) if rot.any() else None,
                dL0=round(100 * float((d == 0).mean()), 1), dL_ge10=round(100 * float((d >= 10).mean()), 1))


PROBEN = (('Hinterbein r (W) Oberschenkel', [9], [1]), ('Vorderbein l (W) Unterarm', [12], [12]), ('Kopf + Kiefer', [22, 23], [26]))
if __name__ == '__main__':
    R = {}
    zeilen = []
    for nm, kb, gb in PROBEN:
        Kt = welt(K5, KF_K, kb); Gt = welt(G4, KF_G, gb)
        RK, TKk = K5.pose(KF_K); RG, TGg = G4.pose(KF_G)
        fk = rahmen(Kt, TKk[kb[0]].astype(float)); fg = rahmen(Gt, TGg[gb[0]].astype(float))
        neu, st = zuordnen(Kt, Gt, fk, fg)
        mo2 = copy.deepcopy(K5)
        for b, d in neu.items():
            for q, lst in d.items():
                if q:
                    mo2.meshes[b].quv = lst
                else:
                    mo2.meshes[b].tuv = lst
        aK, aG = flaeche(Kt), flaeche(Gt)
        # Geometrie unveraendert?
        geo = all(mo2.meshes[b].tv == K5.meshes[b].tv and mo2.meshes[b].qv == K5.meshes[b].qv and mo2.meshes[b].tf == K5.meshes[b].tf and mo2.meshes[b].qf == K5.meshes[b].qf
                  and mo2.meshes[b].tn == K5.meshes[b].tn and mo2.meshes[b].qn == K5.meshes[b].qn for b in kb)
        # gleicher Weltmassstab: groesserer Umkreis bestimmt s
        allk = np.vstack([v for _, _, v, _, _ in Kt]); allg = np.vstack([v for _, _, v, _, _ in Gt])
        rad = max(np.linalg.norm(allk - (allk.max(0) + allk.min(0)) / 2, axis=1).max(), np.linalg.norm(allg - (allg.max(0) + allg.min(0)) / 2, axis=1).max())
        s = 300 * 0.46 / rad
        r = dict(K5c=kb, G4=gb, flaeche_K5c=round(aK), flaeche_G4=round(aG), lin_vergroesserung=round(math.sqrt(aK / aG), 2), stat=st,
                 kante_gt20_prozent=round(100 * st['kante_gt20'] / st['flaechen'], 1), geometrie_unveraendert=geo, ansichten={})
        reihe = []
        for az, el in ((0, 10), (-90, 10), (45, 25)):
            ig, mg = bild(G4, KF_G, gb, TG, az, el, s)
            ik, mk_ = bild(mo2, KF_K, kb, TG, az, el, s)
            ih, mh = bild(K5, KF_K, kb, TK, az, el, s)
            r['ansichten']['az%d' % az] = dict(G4=masse(ig, mg), K5c_G4texel=masse(ik, mk_), K5c_heute=masse(ih, mh))
            reihe.append((ig, ik, ih))
        R[nm] = r
        zeilen.append((nm, reihe))
        print(nm, json.dumps({k: v for k, v in r.items() if k != 'ansichten'}, ensure_ascii=False))
        for k, v in r['ansichten'].items():
            print('  ', k, v)
    json.dump(R, open('stich.json', 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    # Belegbild: je Probe 3 Ansichten x (G4-Teil | K5c mit G4-Texeln | K5c heute), gleicher Massstab und gleiches Licht, roh
    G, pad, kopf = 300, 6, 20
    W = pad + 9 * (G + pad); H = 40 + len(zeilen) * (G + kopf + pad)
    b = Image.new('RGB', (W, H), (18, 19, 22)); dr = ImageDraw.Draw(b)
    dr.text((pad, 6), 'Pruefer V3 Stichproben: je Ansicht G4-Quellteil | K5c-Teil mit G4-Texeln (Schwerpunkt-Affin, eigener Code) | K5c heute. Gleicher Weltmassstab, Licht 0,35+0,65 max(0,N.L), roh.', fill=(240, 200, 90))
    dr.text((pad, 22), 'Ansichten: Seite rechts (W) az 0 | hinten az -90 | 3/4 az 45 el 25', fill=(200, 200, 200))
    y = 40
    for nm, reihe in zeilen:
        dr.text((pad, y + 2), nm + '  (Vergroesserung %.2f, Nahtflaechen %.1f %%)' % (R[nm]['lin_vergroesserung'], R[nm]['kante_gt20_prozent']), fill=(230, 230, 230))
        x = pad
        for trio in reihe:
            for im in trio:
                b.paste(Image.fromarray(im), (x, y + kopf)); x += G + pad
        y += G + kopf + pad
    b.save('V3P_1_stichproben.png')
