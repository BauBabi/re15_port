# -*- coding: utf-8 -*-
"""Pruefer Spielmodell: Krater = am Knick sichtbare, nach innen zeigende Flaechen.
Hohl: Flaeche, deren Aussennormale zur Mittellinie des eigenen Teils zeigt (n * radial < -0,3), Mittellinie aus den Ecken in 8 Scheiben entlang der Teilachse.
Sichtbar: Blick von aussen auf den Knick (Maler wie Buehne), nur Eltern und Kind. Kraterflaeche relativ zur Querschnittsflaeche des Glieds (pi r^2, r = halbe kleinere Gliedbreite).
Rohbauten in der Bindepose ueber den Winkel, RE2 in echten Bildern p50/p90/p99 wie u_biege.
Dazu: falsche Lichtkanten, Symmetrie, Flaechenbudget."""
import cv2
from u_biege import *


def hohl_flags(form, b, achse):
    """Rueckgabe: je Dreieck True, wenn hohl; Flaecheninhalt je Dreieck."""
    m = form['meshes'][b]
    V = np.array(m['ecken'], float).reshape(-1, 3)
    F = dreiecke(m)
    if len(F) == 0:
        return np.zeros(0, bool), np.zeros(0)
    a = V[:, achse]
    lo, hi = a.min(), a.max()
    bins = np.clip(((a - lo) / max(hi - lo, 1) * 8).astype(int), 0, 7)
    mi = np.zeros((8, 3))
    for i in range(8):
        s = V[bins == i]
        mi[i] = s.mean(0) if len(s) else np.nan
    # Luecken fuellen
    for i in range(8):
        if np.isnan(mi[i]).any():
            j = [x for x in range(8) if not np.isnan(mi[x]).any()]
            mi[i] = mi[min(j, key=lambda x: abs(x - i))]
    p = V[F]
    n = -np.cross(p[:, 1] - p[:, 0], p[:, 2] - p[:, 0])
    fl = 0.5 * np.linalg.norm(n, axis=1)
    n = n / np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-9)
    c = p.mean(1)
    ix = np.clip(((c[:, achse] - lo) / max(hi - lo, 1) * 8).astype(int), 0, 7)
    r = c - mi[ix]
    r[:, achse] = 0
    d = np.linalg.norm(r, axis=1, keepdims=True)
    rd = r / np.maximum(d, 1e-9)
    # Wand zaehlt nur, wenn die Flaeche nicht auf der Achse sitzt
    return ((n * rd).sum(1) < -0.3) & (d[:, 0] > 25), fl


def achse_von(form, b):
    """Teilachse: groesste Ausdehnung im Knochenraum, x oder y."""
    V = np.array(form['meshes'][b]['ecken'], float).reshape(-1, 3)
    e = V.max(0) - V.min(0)
    return int(np.argmax(e[:2]))


def krater_bild(form, teile, flags, R, T, A, J, s, gross):
    """Maler: je Dreieck fern zuerst. Rueckgabe: Maske hohl, Maske Figur."""
    L = []
    for b in teile:
        m = form['meshes'][b]
        V = np.array(m['ecken'], float).reshape(-1, 3)
        F = dreiecke(m)
        if len(F) == 0:
            continue
        q = ((V @ R[b].T + T[b]) - J) @ A.T
        xy = q[:, :2] * s + gross / 2
        p = xy[F]
        d = (p[:, 1, 0] - p[:, 0, 0]) * (p[:, 2, 1] - p[:, 0, 1]) - (p[:, 2, 0] - p[:, 0, 0]) * (p[:, 1, 1] - p[:, 0, 1])
        z = q[F, 2].mean(1)
        for i in range(len(F)):
            if d[i] > 0:
                L.append((z[i], p[i], bool(flags[b][i])))
    a = np.zeros((gross, gross), np.uint8)
    for z, p, h in sorted(L, key=lambda t: -t[0]):
        cv2.fillConvexPoly(a, np.round(p * 16).astype(np.int32), 2 if h else 1, lineType=cv2.LINE_8, shift=4)
    return a


def aussen_ansicht(u1, u2, nn):
    o = u1 - u2
    o = o / np.linalg.norm(o)
    vor = -(o * 0.82 + nn * 0.57)
    vor = vor / np.linalg.norm(vor)
    unten = u1 - (u1 @ vor) * vor
    unten = unten / np.linalg.norm(unten)
    return np.stack([np.cross(unten, vor), unten, vor])


def krater_mess(form, R, T, p, c, A_b, J, E, n_w, flags, r_glied):
    u1 = (J - A_b) / np.linalg.norm(J - A_b)
    u2 = (E - J) / np.linalg.norm(E - J)
    s, gross = 0.5, 1400
    q0 = -1.0
    for n2 in (n_w, -n_w):
        A = aussen_ansicht(u1, u2, n2 / np.linalg.norm(n2))
        a = krater_bild(form, [p, c], flags, R, T, A, J, s, gross)
        q0 = max(q0, float((a == 2).sum()) / (s * s) / (math.pi * r_glied ** 2))
    return q0


def eich_breite(form, b, ach):
    V = np.array(form['meshes'][b]['ecken'], float).reshape(-1, 3)
    q = [i for i in range(3) if i != ach]
    return float(np.percentile(V[:, q[0]], 95) - np.percentile(V[:, q[0]], 5) + np.percentile(V[:, q[1]], 95) - np.percentile(V[:, q[1]], 5)) / 4


def falsche_kanten(form):
    """Kanten, an denen die gespeicherten Normalen stark springen, obwohl die Flaechen fast gleich liegen. Laenge relativ zur Gesamtkantenlaenge."""
    n_b = len(form['knochen'])
    gesamt = falsch = 0.0
    hart = 0.0
    for m in form['meshes'][:n_b]:
        V = np.array(m['ecken'], float)
        N = np.array(m['normalen'], float)
        if len(V) == 0 or len(N) == 0:
            continue
        fl = [(tuple(f['e']), tuple(f['n'])) for f in m['dreiecke']]
        for f in m['vierecke']:
            e, nn = f['e'], f['n']
            fl.append(((e[0], e[1], e[3]), (nn[0], nn[1], nn[3])))
            fl.append(((e[0], e[3], e[2]), (nn[0], nn[3], nn[2])))
        rund = {tuple(np.round(v, 0)): i for i, v in enumerate(V)}
        ki = [rund[tuple(np.round(v, 0))] for v in V]
        kant = {}
        for e, nn in fl:
            p = V[list(e)]
            g = -np.cross(p[1] - p[0], p[2] - p[0])
            g = g / max(np.linalg.norm(g), 1e-9)
            for i in range(3):
                a, b = ki[e[i]], ki[e[(i + 1) % 3]]
                k = (a, b) if a < b else (b, a)
                nnk = {a: nn[i], b: nn[(i + 1) % 3]}
                kant.setdefault(k, []).append((g, nnk))
        for k, l in kant.items():
            if len(l) != 2:
                continue
            la = float(np.linalg.norm(V[k[0]] - V[k[1]]))
            gesamt += la
            dih = math.degrees(math.acos(np.clip(l[0][0] @ l[1][0], -1, 1)))
            spr = 0.0
            for x in k:
                a, b = N[l[0][1][x]], N[l[1][1][x]]
                la_, lb_ = np.linalg.norm(a), np.linalg.norm(b)
                if la_ > 1 and lb_ > 1:
                    spr = max(spr, math.degrees(math.acos(np.clip(a @ b / la_ / lb_, -1, 1))))
            if spr > 25:
                hart += la
                if dih < 15:
                    falsch += la
    return hart / max(gesamt, 1), falsch / max(gesamt, 1)


def symmetrie(form):
    """Mittlerer Abstand der Ecken zum Spiegelbild, Paare links/rechts und Mittelteile, relativ zur Teilgroesse."""
    from scipy.spatial import cKDTree
    P = [('oberarm', 'oberarm_l', 'oberarm_r'), ('unterarm', 'unterarm_l', 'unterarm_r'), ('hand', 'hand_l', 'hand_r'), ('oberschenkel', 'oberschenkel_l', 'oberschenkel_r'),
         ('unterschenkel', 'unterschenkel_l', 'unterschenkel_r'), ('fuss', 'fuss_l', 'fuss_r')]
    aus = {}
    S = np.array([1, 1, -1.0])
    for nm, a, b in P:
        Va = np.array(form['meshes'][I[a]]['ecken'], float).reshape(-1, 3)
        Vb = np.array(form['meshes'][I[b]]['ecken'], float).reshape(-1, 3) * S
        d1 = cKDTree(Vb).query(Va)[0].mean()
        d2 = cKDTree(Va).query(Vb)[0].mean()
        aus[nm] = round(float((d1 + d2) / 2), 1)
    for nm in ('huefte', 'wirbel', 'brust', 'hals', 'kopf'):
        V = np.array(form['meshes'][I[nm]]['ecken'], float).reshape(-1, 3)
        d = cKDTree(V * S).query(V)[0].mean()
        aus[nm] = round(float(d), 1)
    return aus


if __name__ == '__main__':
    erg = {}
    print('--- Hohlanteil je Glied-Teil: Flaeche hohl / Flaeche Teil')
    for k in ROHBAU:
        form = lade(k)
        n = len(form['knochen'])
        Fl = {}
        for b in range(n):
            if form['meshes'][b]['ecken']:
                Fl[b] = hohl_flags(form, b, achse_von(form, b))[0]
        z = {}
        for nm in ('oberarm_l', 'unterarm_l', 'oberschenkel_l', 'unterschenkel_l', 'fuss_l', 'huefte', 'wirbel', 'brust', 'schwanz1', 'schwanz2', 'schwanz3', 'schwanz4', 'schwanz5'):
            b = I[nm]
            h, fl = hohl_flags(form, b, achse_von(form, b))
            z[nm] = round(float(fl[h].sum() / fl.sum()), 3)
        print(k, z)
        R0, T0 = bindepose(form)
        erg[k] = dict(hohl=z, krater={})
        for name, p, c, e, ach, vz, wink, seite in GELENKE:
            if '_y' in name and ('schwanz' not in name):
                pass
            f, n_ax = ACHSE[ach]
            A0 = eltern_anfang(form, T0, p, c)
            r_gl = eich_breite(form, c, achse_von(form, c))
            z2 = []
            for g in wink:
                if g == 0:
                    continue
                Rn, Tn = gebeugt(form, R0, T0, {c: f(vz * g)})
                E = kindende(form, c, e, Rn, Tn)
                z2.append((g, round(krater_mess(form, Rn, Tn, p, c, A0, Tn[c], E, n_ax, Fl, r_gl), 3)))
            erg[k]['krater'][name] = z2
        for nm in ('ellbogen_l', 'ellbogen_r', 'knie_l', 'knie_r', 'brust_z+1', 'brust_z-1', 'wirbel_z+1', 'wirbel_z-1', 'schwanz2_z+1', 'schwanz4_y+1'):
            print('  ', k, nm, erg[k]['krater'].get(nm))
    print('--- RE2 in echten Bildern')
    e_ = {}
    for k in ():
        form = lade(k)
        Fl = {}
        for b in range(len(form['knochen'])):
            if form['meshes'][b]['ecken']:
                Fl[b] = hohl_flags(form, b, achse_von(form, b))[0]
        d = json.load(open(AUS + '/biege.json'))['eich'][k]
        for nm, p, j, ee in EICH[k]:
            z = []
            for q, m in d[nm]['mess'].items():
                R, T = pose(form, m['clip'], m['bild'])
                r_gl = eich_breite(form, j, achse_von(form, j))
                n_w = np.cross(T[j] - T[p], T[ee] - T[j])
                z.append((q, m['g'], round(krater_mess(form, R, T, p, j, T[p], T[j], T[ee], n_w, Fl, r_gl), 3)))
            print('  ', k, nm, z)
            e_['%s_%s' % (k, nm)] = z
    erg['eich'] = e_
    print('--- Lichtkanten: hart = Normalensprung ueber 25 Grad, falsch = dabei Flaechen unter 15 Grad Knick. Anteil an der Kantenlaenge')
    for k in ('F', 'K34', 'K28'):
        h, f_ = falsche_kanten(lade(k))
        print(' ', k, 'hart %.3f  falsch %.3f' % (h, f_))
        erg.setdefault('kanten', {})[k] = (round(h, 3), round(f_, 3))
    print('--- Symmetrie: mittlerer Abstand zum Spiegelbild in Einheiten')
    for k in ROHBAU:
        s = symmetrie(lade(k))
        print(' ', k, s)
        erg.setdefault('symmetrie', {})[k] = s
    print('--- Flaechen je Gruppe')
    GR = {'Rumpf': ('huefte', 'wirbel', 'brust'), 'Kopf': ('hals', 'kopf', 'kiefer'), 'Schwanz': tuple('schwanz%d' % i for i in range(1, 7)),
          'Arme': ('oberarm_l', 'unterarm_l', 'oberarm_r', 'unterarm_r'), 'Beine': ('oberschenkel_l', 'unterschenkel_l', 'oberschenkel_r', 'unterschenkel_r'),
          'Haende': ('hand_l', 'hand_r') + tuple('klaue_%s%d' % (s, i) for s in 'lr' for i in (1, 2, 3, 4)), 'Fuesse': ('fuss_l', 'fuss_r')}
    for k in ROHBAU:
        form = lade(k)
        z = {g: sum(len(form['meshes'][I[x]]['dreiecke']) + len(form['meshes'][I[x]]['vierecke']) for x in t) for g, t in GR.items()}
        print(' ', k, z)
        erg.setdefault('budget', {})[k] = z
    form = lade('K34')
    z = {}
    for g, t in {'Rumpf': (0, 9, 10), 'Kopf': (26,), 'Beine+Fuesse': (1, 2, 3, 4, 5, 6, 7, 8), 'Arme+Haende': (11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25)}.items():
        z[g] = sum(len(form['meshes'][x]['dreiecke']) + len(form['meshes'][x]['vierecke']) for x in t)
    print('  K34', z)
    json.dump(erg, open(AUS + '/krater.json', 'w'), indent=1, default=str)
