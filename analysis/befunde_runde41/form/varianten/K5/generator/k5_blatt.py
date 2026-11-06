#!/usr/bin/env python3
"""Variante K5: Bildleisten fuer den Nutzer (deutsch, ohne Umlaute wie im Repo).

    python k5_blatt.py leiste  AUS.png NAME=DIR          je Variante: Bild 2 Original | Variante in Kamera 5090 Cut 11 mit L1-L8 | Ueberlagerung |
                                                          Bild 1 Original | Variante in Bild-1-Kamera
    python k5_blatt.py vergleich AUS.png NAME=DIR ...    alle Varianten nebeneinander (Bild 2, Kopfbereich) + Tabelle R1-R10
    python k5_blatt.py rundum AUS.png NAME=DIR           6 Blickwinkel ganze Figur (Pose Bild 2) + Kopf nah (Pose Bild 2 und Ruhe)

Render: form_pruefung.Kandidat.rendern (Kamera, Licht, Hintergrund wie die Pruefung), gefiltert mit form_video.video_filter. Messwerte aus mass.json
(k5_mass.py) im Variantenordner."""
import json
import math
import os
import sys

import cv2
import numpy as np

import k5_kern as C

KK = C.KK
sys.path.insert(0, KK.TOOLS)
import form_pruefung as P  # noqa: E402
import form_buehne as FB  # noqa: E402
import form_video as FV  # noqa: E402

REGELN = ('R1', 'R2', 'R3', 'R4', 'R5', 'R6', 'R7', 'R8', 'R9', 'R10')
KURZ = {'R1': 'Augen sichtbar', 'R2': 'Augenlinie', 'R3': 'OK-Spitze', 'R4': 'Mittellinie', 'R5': 'UK-Lappen', 'R6': 'Maulwinkel l (W)',
        'R7': 'UK rechts (B)', 'R8': 'Umriss/IoU', 'R9': 'Boden frei B1', 'R10': 'Schwanz oben B1'}
B2 = (110, 130, 510, 480)
B1 = (150, 160, 470, 480)
KOPF2 = (170, 225, 370, 425)
GRUEN, ROT = (90, 220, 90), (70, 70, 255)


def schrift(a, t, x, y, g=0.5, f=(255, 255, 255), d=1):
    cv2.putText(a, t, (int(x), int(y)), cv2.FONT_HERSHEY_SIMPLEX, g, (0, 0, 0), d + 2, cv2.LINE_AA)
    cv2.putText(a, t, (int(x), int(y)), cv2.FONT_HERSHEY_SIMPLEX, g, f, d, cv2.LINE_AA)


def ausschnitt(a, box, h):
    x0, y0, x1, y1 = box
    s = h / float(y1 - y0)
    return cv2.resize(np.ascontiguousarray(a[y0:y1, x0:x1]), (int(round((x1 - x0) * s)), h), interpolation=cv2.INTER_NEAREST), s


def lade(spec):
    nm, d = spec.split('=', 1)
    m = json.load(open(os.path.join(d, 'mass.json'), encoding='utf-8'))
    kj = os.path.join(d, 'kopf.json')
    kopf = json.load(open(kj, encoding='utf-8')) if os.path.exists(kj) else None
    return nm, d, P.Kandidat(os.path.join(d, 'form.json')), m, kopf


def renders(k, n):
    sauber, alb, m, _ = k.rendern(n)
    fil = FV.video_filter(sauber, bild=n, entzerren=False, rauschen=False)
    return sauber[:, :, ::-1].copy(), fil[:, :, ::-1].copy(), m


def marken_ziel(a, box, s, beschriften=True):
    """Ziele L1-L8 (gruen) in einen Ausschnitt zeichnen."""
    x0, y0 = box[0], box[1]
    q = lambda p: (int(round((p[0] - x0) * s)), int(round((p[1] - y0) * s)))
    cv2.line(a, q(C.L2), q(C.L1), (0, 200, 0), 1, cv2.LINE_AA)
    cv2.line(a, q(((C.L1[0] + C.L2[0]) / 2, 275)), q(C.L4), (0, 200, 0), 1, cv2.LINE_AA)
    cv2.line(a, q(C.L7), q(C.L4), (0, 160, 0), 1, cv2.LINE_AA)
    cv2.line(a, q(C.L7), q(C.L6_NUTZER), (0, 160, 0), 1, cv2.LINE_AA)
    cv2.line(a, q(C.L8_KNICK), q(C.L8), (0, 160, 0), 1, cv2.LINE_AA)
    for nm, p in (('L1', C.L1), ('L2', C.L2), ('L4', C.L4), ('L6', C.L6_NUTZER), ('L7', C.L7), ('L8', C.L8)):
        cv2.drawMarker(a, q(p), (0, 255, 0), cv2.MARKER_CROSS, 10, 2)
        if beschriften:
            schrift(a, nm, q(p)[0] + 5, q(p)[1] - 4, 0.42, (120, 255, 120))
    cs, _ = cv2.findContours(C.lappen_maske().astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cs = [np.round((c - np.array([x0, y0])) * s).astype(np.int32) for c in cs]
    cv2.drawContours(a, cs, -1, (0, 160, 0), 1)


def marken_modell(a, box, s, lm):
    x0, y0 = box[0], box[1]
    q = lambda p: (int(round((p[0] - x0) * s)), int(round((p[1] - y0) * s)))
    for nm in ('auge_l', 'auge_r', 'ok_spitze', 'uk_spitze', 'maulwinkel_l'):
        if nm in lm:
            cv2.circle(a, q(lm[nm]), 6, (0, 230, 255), 2, cv2.LINE_AA)
    if 'auge_l' in lm and 'auge_r' in lm:
        cv2.line(a, q(lm['auge_r']), q(lm['auge_l']), (0, 230, 255), 1, cv2.LINE_AA)


def umrisse(a, maske, farbe, box, s):
    cs, _ = cv2.findContours(maske.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cs = [np.round((c - np.array([box[0], box[1]])) * s).astype(np.int32) for c in cs]
    cv2.drawContours(a, cs, -1, farbe, 1, cv2.LINE_AA)


def reihe(kacheln, rand=6, grund=24):
    h = max(k.shape[0] for k in kacheln)
    w = sum(k.shape[1] for k in kacheln) + rand * (len(kacheln) - 1)
    out = np.full((h, w, 3), grund, np.uint8)
    x = 0
    for k in kacheln:
        out[:k.shape[0], x:x + k.shape[1]] = k
        x += k.shape[1] + rand
    return out


def stapel(zeilen, rand=6, grund=24):
    w = max(z.shape[1] for z in zeilen)
    h = sum(z.shape[0] for z in zeilen) + rand * (len(zeilen) - 1)
    out = np.full((h, w, 3), grund, np.uint8)
    y = 0
    for z in zeilen:
        out[y:y + z.shape[0], :z.shape[1]] = z
        y += z.shape[0] + rand
    return out


def kopfzeile(text, breite, h=34, g=0.62, f=(230, 230, 120)):
    a = np.full((h, breite, 3), 24, np.uint8)
    schrift(a, text, 8, h - 11, g, f)
    return a


def beschreibung(nm, kopf, m):
    if not kopf:
        return '%s: K4 heute (Spiel-Asset)' % nm
    f4 = {'kurz': 'F4 kurz (Schnauze %.2f S, nach unten)', 'lang': 'F4 lang (Schnauze %.2f S, zur Kamera)'}[kopf['f4']] % kopf['schnauze_e']
    f5 = {'hals': 'F5 Hals (Hals %d statt 503, Rumpf J)' % round(kopf['halslaenge']), 'rumpf': 'F5 Rumpf (Hals 503, Rumpf Bild 2 gedreht)'}[kopf['f5']]
    n_ok = sum(1 for r in REGELN if m[r]['ok'])
    return '%s: %s, %s, Augenabstand S %d | R1-R10: %d/10 gruen' % (nm, f4, f5, round(kopf['kopf']['S']), n_ok)


# ───────────────────────── Leiste je Variante
def blatt_leiste(aus, spec, h=480):
    nm, d, k, m, kopf = lade(spec)
    o2 = P.original(2)[:, :, ::-1].copy()
    o1 = P.original(1)[:, :, ::-1].copy()
    s2, f2, m2 = renders(k, 2)
    s1, f1, m1 = renders(k, 1)
    t1, s = ausschnitt(o2, B2, h)
    schrift(t1, 'Bild 2 Original (5090 Cut 11)', 8, 22)
    t2, s = ausschnitt(s2, B2, h)
    marken_ziel(t2, B2, s)
    marken_modell(t2, B2, s, m['_lm_bild'])
    schrift(t2, '%s in Kamera 5090 Cut 11 (Render ohne Filter)' % nm, 8, 22)
    schrift(t2, 'gruen + = Ziel L1-L8, gelb o = Modell', 8, 44, 0.45, (200, 255, 200))
    ov = o2.copy().astype(np.float32)
    ov[m2] = 0.5 * ov[m2] + 0.5 * s2[m2].astype(np.float32)
    t3, s = ausschnitt(ov.astype(np.uint8), B2, h)
    umrisse(t3, KK.maske('oberkiefer'), (0, 140, 255), B2, s)
    umrisse(t3, KK.maske('unterkiefer'), (255, 80, 220), B2, s)
    umrisse(t3, m2, (255, 255, 255), B2, s)
    schrift(t3, 'Ueberlagerung 50 Prozent auf Original', 8, 22)
    schrift(t3, 'weiss Modellumriss, orange/rosa Nutzer-Teile OK/UK', 8, 44, 0.45, (230, 230, 230))
    t4, s = ausschnitt(o1, B1, h)
    schrift(t4, 'Bild 1 Original (Bahnsteig)', 8, 22)
    bl, br = C.boden_masken()
    t5, s = ausschnitt(s1, B1, h)
    umrisse(t5, bl | br, (60, 60, 255), B1, s)
    schrift(t5, '%s in Bild-1-Kamera (Render ohne Filter)' % nm, 8, 22)
    schrift(t5, 'rot umrandet = Boden (R9)', 8, 44, 0.45, (200, 200, 255))
    r = reihe([t1, t2, t3, t4, t5])
    zeile = np.full((58, r.shape[1], 3), 24, np.uint8)
    for i, kk in enumerate(REGELN):
        v = m[kk]
        t = '%s %s: %s' % (kk, 'ok' if v['ok'] else 'NEIN', v['wert'])
        schrift(zeile, t[:62], 8 + (i % 5) * int(r.shape[1] / 5), 22 + 26 * (i // 5), 0.45, GRUEN if v['ok'] else ROT)
    cv2.imwrite(aus, stapel([kopfzeile(beschreibung(nm, kopf, m), r.shape[1]), r, zeile]))


# ───────────────────────── Vergleichstafel
def blatt_vergleich(aus, specs, h=520):
    o2 = P.original(2)[:, :, ::-1].copy()
    ks = []
    t, s = ausschnitt(o2, KOPF2, h)
    marken_ziel(t, KOPF2, s)
    schrift(t, 'Bild 2 Original + Ziele L1-L8', 8, 22)
    ks.append(t)
    daten = []
    for sp in specs:
        nm, d, k, m, kopf = lade(sp)
        _s2, f2, _m2 = renders(k, 2)
        t, s = ausschnitt(f2, KOPF2, h)
        marken_ziel(t, KOPF2, s, beschriften=False)
        marken_modell(t, KOPF2, s, m['_lm_bild'])
        schrift(t, nm, 8, 24, 0.7)
        if kopf:
            schrift(t, 'F4 %s (%.2f S), F5 %s' % (kopf['f4'], kopf['schnauze_e'], kopf['f5']), 8, 48, 0.48, (230, 230, 160))
            schrift(t, 'S %d, Hals %d' % (round(kopf['kopf']['S']), round(kopf['halslaenge'])), 8, 70, 0.48, (230, 230, 160))
        else:
            schrift(t, 'heute im Spiel-Asset', 8, 48, 0.48, (230, 230, 160))
        ks.append(t)
        daten.append((nm, m))
    r = reihe(ks)
    # Tabelle R1-R10
    zh = 30
    sp0 = 210
    spw = int((r.shape[1] - sp0) / len(daten))
    tab = np.full((zh * (len(REGELN) + 1) + 10, r.shape[1], 3), 24, np.uint8)
    schrift(tab, 'Regel (FORMBLATT_V2 Abschn. 6)', 8, 22, 0.5, (230, 230, 120))
    for j, (nm, m) in enumerate(daten):
        schrift(tab, nm, sp0 + j * spw + 6, 22, 0.55, (230, 230, 120))
    for i, kk in enumerate(REGELN):
        y = zh * (i + 1) + 22
        schrift(tab, '%s %s' % (kk, KURZ[kk]), 8, y, 0.5)
        for j, (nm, m) in enumerate(daten):
            v = m[kk]
            x = sp0 + j * spw
            cv2.rectangle(tab, (x + 2, y - 19), (x + spw - 4, y + 7), (40, 90, 40) if v['ok'] else (40, 40, 110), -1)
            schrift(tab, v['wert'][:int(spw / 8.2)], x + 6, y, 0.43)
    kopf = kopfzeile('Vergleich Bild 2 (Kamera 5090 Cut 11, Kopfbereich 2,6-fach, Videofilter): Ziele gruen, Modell-Landmarken gelb; '
                     'Tabelle: gruen = Regel erfuellt, rot = nicht', r.shape[1])
    cv2.imwrite(aus, stapel([kopf, r, tab]))


# ───────────────────────── Rundum
def blick_bild(k, kf, A, mitte, s, gross=420, knochen=None):
    """Wie form_pruefung.rundum_dreiecke, aber mit freier Blickmatrix A (Zeilen rechts, unten, vorn), Mitte und Massstab; knochen = nur diese Teile."""
    mo = k.mo
    R, T = mo.pose(kf)
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    aus = []
    for b in range(min(mo.skel.bone_count, len(mo.meshes))):
        if knochen is not None and b not in knochen:
            continue
        m = mo.meshes[b]
        Wm = R[b].astype(float) / 4096.0
        t = T[b].astype(float)
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ Wm.T) + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ Wm.T
            q = (v - mitte) @ A.T
            sx = gross / 2 + q[:, 0] * s
            sy = gross / 2 + q[:, 1] * s
            hell = np.clip(0.46 + 0.86 * np.maximum(nn @ L, 0), 0, 1) * 255.0
            for g, uv in zip(faces, uvs):
                ie, inn = g[1::2], g[0::2]
                p = [(float(sx[i]), float(sy[i])) for i in ie]
                m0 = FB._mac0(p[0], p[1], p[2])
                if (m0 < 0) if viereck else (m0 <= 0):
                    continue
                po = (uv[5] & 0xF) * 128
                zeile = (uv[2] >> 6) & 0x1FF
                c = [np.full(3, hell[i]) for i in inn]
                if viereck:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7]), (uv[9] + po, uv[10])]
                    for x, y, zz in ((0, 1, 3), (0, 3, 2)):
                        aus.append((float(q[ie[x], 2] + q[ie[y], 2] + q[ie[zz], 2]) / 3, (p[x], p[y], p[zz]), (tx[x], tx[y], tx[zz]), zeile, (c[x], c[y], c[zz])))
                else:
                    tx = [(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[6] + po, uv[7])]
                    aus.append((float(q[ie[0], 2] + q[ie[1], 2] + q[ie[2], 2]) / 3, tuple(p), tuple(tx), zeile, tuple(c)))
    a = np.full((gross, gross, 3), 92, np.uint8)
    msk = FB.rastern(aus, k.tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    a[msk] = np.clip(np.power(a[msk] / 255.0, 0.5) * 255, 0, 255).astype(np.uint8)
    return a[:, :, ::-1].copy()


def blick_matrix(vor, oben):
    vor = np.asarray(vor, float) / np.linalg.norm(vor)
    unten = -np.asarray(oben, float)
    unten = unten - (unten @ vor) * vor
    unten /= np.linalg.norm(unten)
    rechts = np.cross(unten, vor)
    return np.stack([rechts, unten, vor])


def blatt_rundum(aus, spec, gross=420):
    nm, d, k, m, kopf = lade(spec)
    zeilen = []
    for clip, titel in ((1, 'Pose Bild 2'), (2, 'Ruhe')):
        kf = k.kf(clip, 0)
        R, T = k.mo.pose(kf)
        alle = P.pose_punkte(k.mo, kf)
        mitte = (alle.max(0) + alle.min(0)) / 2
        s = gross * 0.46 / float(np.linalg.norm(alle - mitte, axis=1).max())
        if clip == 1:
            ks = []
            for t, az, el in (('vorn', 90, 0), ('3/4 vorn rechts', 45, 15), ('rechts (W)', 0, 0), ('hinten', -90, 0), ('links (W)', 180, 0),
                              ('3/4 vorn links oben', 135, 30)):
                a, e = math.radians(az), math.radians(el)
                vor = np.array([-math.sin(a) * math.cos(e), math.sin(e), math.cos(a) * math.cos(e)])
                ks.append(blick_bild(k, kf, blick_matrix(vor, [0, -1, 0]), mitte, s, gross))
                schrift(ks[-1], 'ganz, %s: %s' % (titel, t), 8, 22, 0.5)
            zeilen.append(reihe(ks))
        # Kopf nah: Blick im Kopfraum (x Schnauze, y unten, z links (W)); Mitte = halbe Schnauzenlaenge
        Rk = R[22].astype(float) / 4096.0
        L = kopf['kopf']['L'] if kopf else 1620.0
        km = T[22].astype(float) + Rk @ np.array([0.45 * L, 80.0, 0.0])
        hx, hy, hz = Rk[:, 0], Rk[:, 1], Rk[:, 2]
        sk = gross * 0.42 / (0.62 * L + 400.0)
        ks = []
        for t, vor, oben in (('Kopf von vorn', -hx, -hy), ('Kopf 3/4 vorn links (W)', -hx - hz * 0.9 + 0.2 * hy, -hy),
                             ('Kopf Seite links (W)', -hz, -hy), ('Kopf Seite rechts (W)', hz, -hy), ('Kopf von oben', hy, hx),
                             ('Kopf von unten', -hy, hx)):
            ks.append(blick_bild(k, kf, blick_matrix(vor, oben), km, sk, gross, knochen=(21, 22, 23)))
            schrift(ks[-1], '%s, %s' % (t, titel), 8, 22, 0.5)
        zeilen.append(reihe(ks))
    r = stapel(zeilen)
    kopf_t = ('%s: Rundum (Licht nur fuers Blatt, aufgehellt Gamma 0,5). Zeile 1 ganze Figur Pose Bild 2; Zeile 2/3 nur Hals/Kopf/Kiefer nah, '
              'Pose Bild 2 (Maul offen) und Ruhe; Seiten aus Sicht des Wesens (W)' % nm)
    cv2.imwrite(aus, stapel([kopfzeile(kopf_t, r.shape[1]), r]))


if __name__ == '__main__':
    art = sys.argv[1]
    if art == 'leiste':
        blatt_leiste(sys.argv[2], sys.argv[3])
    elif art == 'vergleich':
        blatt_vergleich(sys.argv[2], sys.argv[3:])
    elif art == 'rundum':
        blatt_rundum(sys.argv[2], sys.argv[3])
