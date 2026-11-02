# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Biegeprobe an Ellbogen, Knie, Wirbelsaeule, Schwanz.
Winkel aus gerippe.json winkelgrenzen_grad: unterarm z -155..+2, unterschenkel z -2..+165, wirbel z +-50 y +-35, brust z +-55 y +-35, schwanz z +-50 y +-50.
Ausgang: Bindepose (alle Winkel 0), jedes Gelenk einzeln. Rohbauten sind auf dieselben Knochen gebaut, die Messung ist also ein reiner Vergleich der Teile am Gelenk.
Messung an der sichtbaren Silhouette (nur Vorderseiten, wie Typ 0x30 zeichnet), Blick entlang der Gelenkachse, 0,5 px je Einheit:
  dicke  = durchgehende Dicke auf der Winkelhalbierenden durch den Gelenkpunkt / kleinere Gliedbreite. 1 = voll, unter 0,6 = Einschnuerung, 0 = Teile beruehren sich nicht.
  kerbe  = Anteil ungedeckter Pixel im Dreieck (Aussenkante Eltern, Aussenkante Kind, Gelenkpunkt). 0 = keine Kerbe.
RE2-Eichmodelle: dieselbe Messung in echten Clip-Bildern, Bild mit Median-, p90- und p99-Knick."""
import cv2
from u_basis import *
from u_ton import Ton

S_MESS = 0.5


def bindepose(form):
    n = len(form['knochen'])
    R = [np.eye(3) for _ in range(n)]
    T = [None] * n
    for b in range(n):
        p = form['knochen'][b]['eltern']
        v = np.array(form['knochen'][b]['versatz'], float)
        T[b] = v.copy() if p < 0 else T[p] + v
    return R, T


def ansicht(u1, n):
    vor = np.asarray(n, float)
    vor = vor / np.linalg.norm(vor)
    unten = u1 - (u1 @ vor) * vor
    unten = unten / np.linalg.norm(unten)
    rechts = np.cross(unten, vor)
    return np.stack([rechts, unten, vor])


def mac0(p):
    return p[0][0] * p[1][1] + p[1][0] * p[2][1] + p[2][0] * p[0][1] - p[0][0] * p[2][1] - p[1][0] * p[0][1] - p[2][0] * p[1][1]


def maske(form, R, T, teile, A, J, s, halb):
    G = 2 * halb
    M = np.zeros((G, G), np.uint8)
    for b in teile:
        m = form['meshes'][b]
        V = np.array(m['ecken'], float).reshape(-1, 3)
        if len(V) == 0:
            continue
        q = ((V @ R[b].T + T[b]) - J) @ A.T
        xy = q[:, :2] * s + halb
        fp = np.round(xy * 16).astype(np.int32)
        for f in m['dreiecke']:
            e = f['e']
            if mac0(xy[list(e)]) > 0:
                cv2.fillConvexPoly(M, fp[list(e)], 1, lineType=cv2.LINE_8, shift=4)
        for f in m['vierecke']:
            a, b2, c, d = f['e']
            if mac0(xy[[a, b2, c]]) >= 0:
                cv2.fillConvexPoly(M, fp[[a, b2, d]], 1, lineType=cv2.LINE_8, shift=4)
                cv2.fillConvexPoly(M, fp[[a, d, c]], 1, lineType=cv2.LINE_8, shift=4)
    return cv2.morphologyEx(M, cv2.MORPH_CLOSE, np.ones((3, 3), np.uint8))


def lauf(M, x0, d, t_max, schritt=0.5):
    """Laeuft von x0 in Richtung d, Rueckgabe: letzte bedeckte Strecke am Stueck, in Pixel. x0 muss bedeckt sein, sonst 0."""
    G = M.shape[0]
    t = 0.0
    while t < t_max:
        x = x0 + d * t
        i, j = int(round(x[1])), int(round(x[0]))
        if not (0 <= i < G and 0 <= j < G) or M[i, j] == 0:
            return t
        t += schritt
    return t


def quer(M, x0, d, t_max):
    """Breite quer zum Glied. Sucht die bedeckte Strecke auf der Geraden x0 + t d, die x0 am naechsten liegt. Rueckgabe (breite, t_aussen) in Pixel."""
    ts = np.arange(-t_max, t_max, 0.5)
    G = M.shape[0]
    bed = np.zeros(len(ts), bool)
    for k, t in enumerate(ts):
        x = x0 + d * t
        i, j = int(round(x[1])), int(round(x[0]))
        bed[k] = 0 <= i < G and 0 <= j < G and M[i, j] > 0
    if not bed.any():
        return 0.0, 0.0
    k0 = np.where(bed)[0]
    k = k0[np.argmin(np.abs(ts[k0]))]
    a, b = k, k
    while a > 0 and bed[a - 1]:
        a -= 1
    while b < len(ts) - 1 and bed[b + 1]:
        b += 1
    return ts[b] - ts[a], ts[b]


def messen(form, R, T, teile, A_b, J_b, E_b, n_w, aussen=None, S=S_MESS):
    """A_b, J_b, E_b = Punkte: Elternanfang, Gelenk, Kindende (Welt). n_w = Gelenkachse. aussen = Welt-Richtung, nur noetig bei fast geradem Gelenk."""
    u1 = J_b - A_b
    Lp = np.linalg.norm(u1)
    u1 = u1 / Lp
    u2 = E_b - J_b
    Lc = np.linalg.norm(u2)
    u2 = u2 / Lc
    g = math.degrees(math.acos(np.clip(u1 @ u2, -1, 1)))
    Ansicht = ansicht(u1, n_w)
    halb = int(max(Lp, Lc) * S * 1.3) + 40
    M = maske(form, R, T, teile, Ansicht, J_b, S, halb)
    z = np.array([halb, halb], float)
    p1 = (Ansicht @ u1)[:2]
    p1 = p1 / np.linalg.norm(p1)
    p2 = (Ansicht @ u2)[:2]
    p2 = p2 / np.linalg.norm(p2)
    if g > 6:
        o = p1 - p2
    else:
        o = (Ansicht @ aussen)[:2]
    o = o / np.linalg.norm(o)
    # Gliedbreite auf 40 % der Laenge, quer zum Glied, in Aussenrichtung
    d1 = o - (o @ p1) * p1
    d1 = d1 / np.linalg.norm(d1)
    d2 = o - (o @ p2) * p2
    d2 = d2 / np.linalg.norm(d2)
    tm = 900 * S
    xp = z - p1 * (0.4 * Lp * S)
    xc = z + p2 * (0.4 * Lc * S)
    Wp, tp = quer(M, xp, d1, tm)
    Wc, tc = quer(M, xc, d2, tm)
    # Winkelhalbierende durch den Gelenkpunkt: durchgehende Dicke
    Wb, _ = quer(M, z, o, tm)
    Wm = max(min([w for w in (Wp, Wc) if w > 0] or [1.0]), 1.0)
    dicke = Wb / Wm
    # Kerbe: Dreieck Aussenkante Eltern, Aussenkante Kind, Gelenkpunkt
    P = xp + d1 * tp
    C = xc + d2 * tc
    Dr = np.zeros(M.shape, np.uint8)
    cv2.fillConvexPoly(Dr, np.round(np.array([P, C, z]) * 16).astype(np.int32), 1, lineType=cv2.LINE_8, shift=4)
    fl = int(Dr.sum())
    ungedeckt = int(((Dr > 0) & (M == 0)).sum())
    kerbe = ungedeckt / max(fl, 1)
    return dict(g=round(g, 1), dicke=round(float(dicke), 2), kerbe=round(float(kerbe), 3), Wp=round(Wp / S), Wc=round(Wc / S), Wb=round(Wb / S), luecke_px=ungedeckt)


def tris_kamera(mo_meshes, nbones, R, T, A, mitte, s, gross):
    """Wie form_pruefung.rundum_dreiecke, aber freie Kamera und freie Pose."""
    aus = []
    L = A.T @ (np.array([-.45, -.6, -.66]) / np.linalg.norm([-.45, -.6, -.66]))
    for b in range(min(nbones, len(mo_meshes))):
        m = mo_meshes[b]
        W = R[b]
        t = T[b]
        for (verts, norms, faces, uvs, viereck) in ((m.tv, m.tn, m.tf, m.tuv, 0), (m.qv, m.qn, m.qf, m.quv, 1)):
            if not faces:
                continue
            v = (np.array([x[:3] for x in verts], float) @ W.T) + t
            nn = (np.array([x[:3] for x in norms], float) / 4096.0) @ W.T
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
    return aus


def tafel(mo, tex, R, T, A, mitte, s, gross, ton=True, grund=58):
    a = np.full((gross, gross, 3), grund, np.uint8)
    t = tris_kamera(mo.meshes, mo.skel.bone_count, R, T, A, mitte, s, gross)
    FB.rastern(t, Ton(tex) if ton else tex, a, dict(ax=1.0, bx=0.0, ay=1.0, by=0.0))
    return a


# Gelenke der Rohbauten: Eltern, Kind, Kindende, Achse (Bindepose), Vorzeichen, Winkel, Blickseite
ELLBOGEN = (0, 30, 60, 90, 120, 150)
RUMPF = (15, 30, 45)
GELENKE = [
    ('ellbogen_l', I['oberarm_l'], I['unterarm_l'], I['hand_l'], 'z', -1, ELLBOGEN, 1),
    ('ellbogen_r', I['oberarm_r'], I['unterarm_r'], I['hand_r'], 'z', -1, ELLBOGEN, -1),
    ('knie_l', I['oberschenkel_l'], I['unterschenkel_l'], I['fuss_l'], 'z', 1, ELLBOGEN, 1),
    ('knie_r', I['oberschenkel_r'], I['unterschenkel_r'], I['fuss_r'], 'z', 1, ELLBOGEN, -1),
]
for vz in (1, -1):
    GELENKE += [
        ('wirbel_z%+d' % vz, I['huefte'], I['wirbel'], I['brust'], 'z', vz, RUMPF, 1),
        ('brust_z%+d' % vz, I['wirbel'], I['brust'], I['hals'], 'z', vz, RUMPF, 1),
        ('wirbel_y%+d' % vz, I['huefte'], I['wirbel'], I['brust'], 'y', vz, RUMPF, 1),
        ('brust_y%+d' % vz, I['wirbel'], I['brust'], I['hals'], 'y', vz, RUMPF, 1),
    ]
    for i in range(1, 6):
        GELENKE.append(('schwanz%d_z%+d' % (i, vz), I['schwanz%d' % i], I['schwanz%d' % (i + 1)], None, 'z', vz, RUMPF, 1))
        GELENKE.append(('schwanz%d_y%+d' % (i, vz), I['schwanz%d' % i], I['schwanz%d' % (i + 1)], None, 'y', vz, RUMPF, 1))
    GELENKE.append(('schwanzwurzel_z%+d' % vz, I['huefte'], I['schwanz1'], I['schwanz2'], 'z', vz, RUMPF, 1))
    GELENKE.append(('schwanzwurzel_y%+d' % vz, I['huefte'], I['schwanz1'], I['schwanz2'], 'y', vz, RUMPF, 1))

ACHSE = {'z': (Rz, np.array([0, 0, 1.0])), 'y': (Ry, np.array([0, 1.0, 0]))}


def kindende(form, c, e, R, T):
    if e is not None:
        return T[e]
    m = form['meshes'][c]
    V = np.array(m['ecken'], float).reshape(-1, 3)
    w = V @ R[c].T + T[c]
    d = np.linalg.norm(w - T[c], axis=1)
    return w[np.argmax(d)]


def eltern_anfang(form, T0, p, c):
    """Anfangspunkt der Elternachse in der Bindepose. Huefte hat keinen Vorgaenger: Fortsetzung der Rumpfachse. Schwanzwurzel: Schwanzachse, die Wurzel sitzt oben auf dem Becken."""
    if c == I['schwanz1']:
        return T0[c] + np.array([700.0, 0, 0])
    if p == I['huefte']:
        return T0[c] - np.array([1100.0, 0, 0])
    return T0[p]


def eine_probe(k, form):
    R0, T0 = bindepose(form)
    aus = {}
    for name, p, c, e, ach, vz, wink, seite in GELENKE:
        f, n = ACHSE[ach]
        teile = [p, c] + ([e] if e is not None else [])
        A0 = eltern_anfang(form, T0, p, c)
        # Aussenrichtung bei geradem Gelenk: gegen die Bewegung des Kindendes
        R1, T1 = gebeugt(form, R0, T0, {c: f(vz * 10)})
        b = kindende(form, c, e, R1, T1) - kindende(form, c, e, R0, T0)
        aussen = -b / np.linalg.norm(b)
        z = []
        for g in wink:
            Rn, Tn = gebeugt(form, R0, T0, {c: f(vz * g)})
            m = messen(form, Rn, Tn, teile, A0, Tn[c], kindende(form, c, e, Rn, Tn), n * seite, aussen=aussen)
            m['soll'] = g
            z.append(m)
        aus[name] = z
    return aus


EICH = {
    'K34': [('ellbogen', 12, 13, 14), ('ellbogen2', 17, 18, 19), ('knie', 2, 3, 4), ('knie2', 6, 7, 8), ('wirbel', 9, 10, 26)],
    'K28': [('ellbogen', 9, 10, 11), ('ellbogen2', 5, 6, 7), ('knie', 13, 14, 15), ('schwanz1', 18, 19, 20), ('schwanz2', 19, 20, 21), ('schwanz3', 20, 21, 22)],
    'K33': [('ellbogen', 18, 19, 21), ('ellbogen2', 11, 12, 13), ('knie', 23, 24, 25), ('knie2', 26, 27, 28), ('wirbel', 1, 2, 3)],
    'K23': [('knie', 2, 3, 4), ('knie2', 16, 17, 18), ('schwanz1', 11, 12, 13), ('schwanz2', 12, 13, 14), ('schwanz3', 13, 14, 15)],
    'K22': [('ellbogen', 4, 5, 6), ('ellbogen2', 11, 12, 13), ('knie', 14, 15, 16), ('knie2', 18, 19, 20)],
}


def bilder_alle(form):
    for c in range(len(form['clips'])):
        for i in range(len(form['clips'][c]['bilder'])):
            yield c, i


def eich_probe(k):
    form = lade(k)
    aus = {}
    Liste = list(bilder_alle(form))
    # Knick je Bild je Gelenk
    Knick = {nm: [] for nm, _, _, _ in EICH[k]}
    for c, i in Liste:
        R, T = pose(form, c, i)
        for nm, p, j, e in EICH[k]:
            u1 = T[j] - T[p]
            u2 = T[e] - T[j]
            g = math.degrees(math.acos(np.clip(u1 @ u2 / np.linalg.norm(u1) / np.linalg.norm(u2), -1, 1)))
            Knick[nm].append(g)
    for nm, p, j, e in EICH[k]:
        a = np.array(Knick[nm])
        v = {}
        for q in (50, 90, 99):
            ziel = np.percentile(a, q)
            ix = int(np.argmin(np.abs(a - ziel)))
            c, i = Liste[ix]
            R, T = pose(form, c, i)
            u1 = (T[j] - T[p])
            u2 = (T[e] - T[j])
            n_w = np.cross(u1, u2)
            if np.linalg.norm(n_w) < 1e-6:
                continue
            m = messen(form, R, T, [p, j] + ([e] if form['meshes'][e]['ecken'] else []), T[p], T[j], T[e], n_w, aussen=None)
            m['clip'] = c
            m['bild'] = i
            m['q'] = q
            v[str(q)] = m
        aus[nm] = dict(p50=round(float(np.percentile(a, 50)), 1), p90=round(float(np.percentile(a, 90)), 1), p99=round(float(np.percentile(a, 99)), 1), mess=v)
        print(k, nm, aus[nm])
    return aus


if __name__ == '__main__':
    erg = {}
    for k in ROHBAU:
        form = lade(k)
        erg[k] = eine_probe(k, form)
        for nm in erg[k]:
            print(k, nm, [(m['soll'], m['dicke'], m['kerbe'], m['Wb']) for m in erg[k][nm]])
    e = {}
    for k in EICH:
        e[k] = eich_probe(k)
    erg['eich'] = e
    json.dump(erg, open(AUS + '/biege.json', 'w'), indent=1)
