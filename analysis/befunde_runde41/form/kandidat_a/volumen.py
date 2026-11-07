#!/usr/bin/env python3
"""Kandidat A, Stufe 2: Rundkegel -> SDF je Knochen -> Marching Cubes -> Blender-Reduktion + UV-Inseln.

    python volumen.py zw/passung.json

Rumpf (becken, rumpf_mitte, rumpf) ist EIN glattes Feld, das in drei ueberlappende Scheiben geteilt wird; so bleibt die Kuppel in Pose 1 eine Flaeche ohne Kerben.
Alle anderen Teile sind eigene geschlossene Volumen mit runden Enden, die in den Elternteil hineinragen (kein Skinning, Beugen ohne Loecher).
"""
import json, os, subprocess, sys
import numpy as np
from scipy.ndimage import gaussian_filter
from skimage import measure

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import entwurf as E

BLENDER = r'C:/Program Files/Blender Foundation/Blender 5.2/blender.exe'
VOXEL = 22.0
GLATT = {'becken': 70, 'rumpf_mitte': 70, 'rumpf': 70}   # k der glatten Vereinigung
GLATT_STD = 70
BONES = [k[0] for k in E.KNOCHEN]
IDX = {n: i for i, n in enumerate(BONES)}
RUMPF = ('becken', 'rumpf_mitte', 'rumpf')
FUSS = ('fr_fuss', 'fl_fuss', 'nl_hand', 'nl_daumen', 'nr_hand', 'nr_daumen', 'nl_innen', 'nl_unter', 'nr_unter')   # am Boden abgeschnitten
# Flaechenziel je Teil, Dreiecke vor dem Zusammenlegen zu Vierecken
ZIEL = {'becken': 265, 'rumpf_mitte': 189, 'rumpf': 322, 'huefte_r': 95, 'huefte_l': 66, 'fr_ober': 76, 'fl_ober': 53, 'fr_unter': 85, 'fl_unter': 53, 'fr_fuss': 104, 'fl_fuss': 66, 'nl_ober': 113, 'nl_unter': 123, 'nl_hand': 123, 'nl_innen': 76, 'nl_daumen': 53, 'nr_ober': 104, 'nr_unter': 142, 'nr_hand': 142, 'nr_daumen': 66, 'hals': 123, 'kopf': 245, 'kiefer': 123}
UEBERLAPP = 103.3   # Scheiben des Rumpfs (krumm, damit die Schnittebene nicht auf das Gitter faellt)


def smin(a, b, k):
    h = np.clip(0.5 + 0.5 * (b - a) / k, 0, 1)
    return b + (a - b) * h - k * h * (1 - h)


def sdf_kegel(p, a, b, ra, rb):
    ab = b - a
    l2 = max(float(ab @ ab), 1e-6)
    t = np.clip(((p - a) @ ab) / l2, 0, 1)
    q = p - (a + t[..., None] * ab)
    return np.sqrt((q * q).sum(-1)) - (ra + t * (rb - ra))


def feld(pr, p, k):
    d = None
    for (a, b, ra, rb) in pr:
        e = sdf_kegel(p, a, b, ra, rb)
        d = e if d is None else smin(d, e, k)
    return d


def teil_sdf(name, prims, p):
    """p (...,3), Modellraum, y nach unten."""
    if name in RUMPF:
        d = feld(sum((prims[n] for n in RUMPF), []), p, GLATT[name])
        x = p[..., 0]
        x1, x2 = SCHNITT
        if name == 'rumpf':
            d = np.maximum(d, (x1 - UEBERLAPP) - x)
        elif name == 'rumpf_mitte':
            d = np.maximum(d, np.maximum(x - (x1 + UEBERLAPP), (x2 - UEBERLAPP) - x))
        else:
            d = np.maximum(d, x - (x2 + UEBERLAPP))
        return d
    d = feld(prims[name], p, GLATT_STD)
    if name in FUSS:
        d = np.maximum(d, p[..., 1])            # Boden y = 0
    return d


SCHNITT = (150.0, -650.0)


def lade_prims(pfad):
    j = json.load(open(pfad, encoding='utf-8'))
    a, b, ra, rb, pb = [np.array(j[k], float) for k in ('a', 'b', 'ra', 'rb', 'pb')]
    prims = {n: [] for n in BONES}
    for i in range(len(pb)):
        prims[BONES[int(pb[i])]].append((a[i], b[i], abs(ra[i]), abs(rb[i])))
    # Steg zwischen Kugel und Innenglied unter dem Knick-Band Z7 (A22, A23), Rueckprojektion Bild 1 Tiefe 4750, Radius 8 px
    sys.path.insert(0, os.path.normpath(os.path.join(HIER, '..', '..', '..', '..', 're15_port', 'tools')))
    import form_kamera as FK
    k = FK.lade_kamera(1)
    L = np.array(j['lage1'], float)
    w_ = float(j['gier1']) * 2 * np.pi / 4096
    c_, s_ = np.cos(w_), np.sin(w_)
    Y = np.array([[c_, 0, s_], [0, 1, 0], [-s_, 0, c_]])

    def m(x, y, D):
        e, d = FK.strahl(k, np.array([x, y], float))
        return Y.T @ (e + d * (D / (d @ k['R'][2])) - L)
    for (x0, y0), (x1, y1) in (((248, 362), (246, 375)), ((238, 348), (250, 352))):
        prims['nl_unter'].append((m(x0, y0, 4750), m(x1, y1, 4750), 85.0, 85.0))
    return prims, j


# ═══════════════════════════════════════════════════ Schnitt mit den Sehkegeln beider Bilder
def smax(a, b, k):
    return -smin(-a, -b, k)


def bilinear(F, x, y):
    x = np.clip(x, 0, F.shape[1] - 1.001)
    y = np.clip(y, 0, F.shape[0] - 1.001)
    x0, y0 = x.astype(np.int32), y.astype(np.int32)
    fx, fy = x - x0, y - y0
    return (F[y0, x0] * (1 - fx) * (1 - fy) + F[y0, x0 + 1] * fx * (1 - fy) + F[y0 + 1, x0] * (1 - fx) * fy + F[y0 + 1, x0 + 1] * fx * fy)


def erlaubt_felder():
    """Vorzeichen-Abstand zum erlaubten Bereich je Bild, in Pixeln, + = draussen. Bild 1: gemessener Soll-Umriss, offene Zonen frei. Bild 2: schwarz der Zielmaske + Linie der Kante."""
    import cv2
    FORM = os.path.normpath(os.path.join(HIER, '..'))
    sys.path.insert(0, os.path.normpath(os.path.join(HIER, '..', '..', '..', '..', 're15_port', 'tools')))
    import form_pruefung as PR
    z1 = cv2.imread(os.path.join(FORM, 'ziel_bild1.png'), 0)
    f1 = cv2.imread(os.path.join(FORM, 'pruefung', 'figur_bild1.png'), 0) > 0
    rand = cv2.distanceTransform((~f1).astype(np.uint8), cv2.DIST_L2, 3)
    ys = np.arange(480)[:, None] * np.ones((1, 640))
    a1 = f1 | ((z1 == 128) & ((rand >= 14) | (ys > 400)))
    a14 = cv2.dilate(PR.poly_maske(PR.merkmale()['A14']['polygon']).astype(np.uint8), np.ones((9, 9), np.uint8)) > 0     # Querband an der Taille ist Figur, nicht Boden (FORMBLATT Z6)
    a1 = a1 | (a14 & (z1 != 0))
    # C5 ist Boden durch die Luecke (Farbfolge wie links, FORMBLATT 1.4), Rundling hat dort seine harte linke Kante
    a1 = a1 & ~PR.poly_maske([(286, 402), (304, 403), (306, 405), (312, 414), (318, 432), (318, 456), (286, 456)])
    z2 = cv2.imread(os.path.join(FORM, 'ziel_bild2.png'), 0)
    a2 = z2 != 0
    R = PR.teil_b2('R')
    kante = R['kante_gelesen'] + R['kante_belegt']
    q = np.array([[238, 228]] + kante + [[470, 206], [470, 110], [238, 110]], float)
    ob = np.zeros((480, 640), np.uint8)
    cv2.fillPoly(ob, [np.round(q).astype(np.int32)], 1)
    h = PR.poly_maske(PR.teil_b2('H')['polygon'])
    a2 = a2 & ~((ob > 0) & ~h)
    aus = {}
    # Soll aus FORMBLATT 7 liegt an zwei Kanten 3-4 px neben dem Umriss-Polygon: dort das Erlaubte bis an die Kante von Abschnitt 7 legen
    a1 = a1 | ((PR.poly_maske([(272, 316), (289, 316), (290, 346), (272, 346)]) | PR.poly_maske([(334, 338), (346, 338), (346, 350), (334, 350)])) & (z1 != 0))
    a1 = a1 & ~PR.poly_maske([(296, 349), (313, 349), (315, 350), (315, 351), (311, 351), (310, 355), (296, 355)])     # Unterkante von L2 an der Taille
    l2 = PR.poly_maske(PR.zone_b1('L2')) & (z1 == 0)
    a1 = a1 & ~(cv2.dilate(l2.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0)
    a22 = np.zeros((480, 640), np.uint8)
    for i in ('A22', 'A23', 'A20'):
        a22 |= cv2.dilate(PR.poly_maske(PR.merkmale()[i]['polygon']).astype(np.uint8), np.ones((5, 5), np.uint8))
    a1 = a1 | ((a22 > 0) & (z1 != 0))        # Knick-Band Z7 ist Figur, der Umriss-Soll laesst dort wie bei A14 ein Loch
    a1 = a1 & ~PR.poly_maske([(196, 326), (225, 326), (225, 345), (196, 345)])     # Kante Bein NL laut Abschnitt 7 x=226 bis y=342, das Polygon laeuft dort 4 px weiter links
    for n, a in ((1, a1), (2, a2)):
        a8 = a.astype(np.uint8)
        d = cv2.distanceTransform(1 - a8, cv2.DIST_L2, 5) - cv2.distanceTransform(a8, cv2.DIST_L2, 5)
        aus[n] = cv2.GaussianBlur(d.astype(np.float32), (0, 0), 0.8)
    return aus


class Kegel:
    def __init__(self, j):
        sys.path.insert(0, os.path.normpath(os.path.join(HIER, '..', '..', '..', '..', 're15_port', 'tools')))
        import form_kamera as FK
        import maler as MA
        self.FK, self.MA = FK, MA
        self.kam = {n: FK.lade_kamera(n) for n in (1, 2)}
        self.F = erlaubt_felder()
        self.g = np.array(j['g'], float)
        self.lage = {n: np.array(j['lage%d' % n], float) for n in (1, 2)}
        self.gier = {n: float(j['gier%d' % n]) for n in (1, 2)}
        self.pose = {1: MA.pose_fk(self.g, np.zeros((len(BONES), 3))), 2: MA.pose_fk(self.g, np.array(j['w2'], float))}

    def carve(self, name, p, d):
        b = IDX[name]
        s = p.shape
        q = p.reshape(-1, 3).astype(np.float64)
        d = d.reshape(-1).copy()
        nah = np.abs(d) < 400      # nur nahe der Flaeche rechnen
        for n in (1, 2):
            R, T = self.pose[n]
            Y = self.MA.yrot(self.gier[n])
            w = ((q[nah] - self.g[b]) @ R[b].T + T[b]) @ Y.T + self.lage[n]
            kam = self.kam[n]
            h = (kam['P'] @ np.c_[w, np.ones(len(w))].T).T
            x, y = h[:, 0] / h[:, 2], h[:, 1] / h[:, 2]
            t = (w - np.asarray(kam['auge'], float)) @ kam['R'][2]
            e = bilinear(self.F[n], x, y) * (t / 425.0)     # Pixel -> Einheiten
            if not (n == 2 and name in ('huefte_r', 'huefte_l', 'fr_ober', 'fr_unter', 'fr_fuss', 'fl_ober', 'fl_unter', 'fl_fuss')):     # Hinterbeine stehen in Bild 2 im Feuer, sie duerfen die Stellen aus Bild 1 nicht verlieren
                d[nah] = smax(d[nah], e, 26.0)
            d[nah] = np.maximum(d[nah], w[:, 1])            # Boden y = 0 in beiden Posen: sonst hebt boden_nachfuehren die ganze Figur
        return d.reshape(s[:-1])


def marsch(name, prims):
    if name in RUMPF:
        pp = sum((prims[n] for n in RUMPF), [])
    else:
        pp = prims[name]
    lo = np.min([np.minimum(a - ra, b - rb) for a, b, ra, rb in pp], 0) - 2.5 * VOXEL
    hi = np.max([np.maximum(a + ra, b + rb) for a, b, ra, rb in pp], 0) + 2.5 * VOXEL
    if name in RUMPF:
        x1, x2 = SCHNITT
        if name == 'rumpf':
            lo[0] = max(lo[0], x1 - UEBERLAPP - 2.37 * VOXEL)
        elif name == 'rumpf_mitte':
            lo[0] = max(lo[0], x2 - UEBERLAPP - 2.37 * VOXEL)
            hi[0] = min(hi[0], x1 + UEBERLAPP + 2 * VOXEL)
        else:
            hi[0] = min(hi[0], x2 + UEBERLAPP + 2 * VOXEL)
    n = np.ceil((hi - lo) / VOXEL).astype(int) + 1
    xs = [lo[i] + np.arange(n[i]) * VOXEL for i in range(3)]
    G = np.stack(np.meshgrid(*xs, indexing='ij'), -1)
    d = teil_sdf(name, prims, G) - AUFBLAEHEN
    if KEGEL is not None:
        d = KEGEL.carve(name, G, d)
        d = gaussian_filter(d, 0.9)        # Kanten des Kegelschnitts runden
    v, f, _n, _w = measure.marching_cubes(d, 0.0, spacing=(VOXEL, VOXEL, VOXEL))
    v = v + lo
    return v, f[:, ::-1]        # Aussenwindung fuer Blender: gegen den Uhrzeigersinn von aussen


def obj_schreiben(pfad, v, f):
    with open(pfad, 'w') as h:
        for q in v:
            h.write('v %.3f %.3f %.3f\n' % tuple(q))
        for t in f:
            h.write('f %d %d %d\n' % (t[0] + 1, t[1] + 1, t[2] + 1))


BLENDER_SKRIPT = r'''
import bpy, bmesh, json, sys, os, math
args = sys.argv[sys.argv.index('--') + 1:]
ordner, ziel_json = args[0], args[1]
ziele = json.load(open(ziel_json))
bpy.ops.wm.read_factory_settings(use_empty=True)
aus = {}
for name, ziel in ziele.items():
    pfad = os.path.join(ordner, name + '.obj')
    bpy.ops.wm.obj_import(filepath=pfad, forward_axis='Y', up_axis='Z')
    ob = bpy.context.selected_objects[0]
    bpy.context.view_layer.objects.active = ob
    me = ob.data
    n0 = len(me.polygons)
    m = ob.modifiers.new('d', 'DECIMATE')
    m.decimate_type = 'COLLAPSE'
    m.ratio = min(1.0, ziel / max(n0, 1))
    bpy.ops.object.modifier_apply(modifier=m.name)
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.join_triangles(bm, faces=bm.faces, angle_face_threshold=math.radians(30), angle_shape_threshold=math.radians(40), cmp_uvs=False, cmp_vcols=False, cmp_seam=False, cmp_sharp=False)
    bm.to_mesh(me)
    bm.free()
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=math.radians(70), island_margin=0.0, area_weight=0.0, correct_aspect=False, scale_to_bounds=False)
    bpy.ops.object.mode_set(mode='OBJECT')
    uvl = me.uv_layers.active.data
    V = [list(v.co) for v in me.vertices]
    F = []
    for p in me.polygons:
        F.append(dict(v=list(p.vertices), uv=[list(uvl[i].uv) for i in p.loop_indices]))
    aus[name] = dict(v=V, f=F)
    print('TEIL', name, n0, '->', len(me.polygons), flush=True)
    bpy.ops.object.delete()
json.dump(aus, open(os.path.join(ordner, 'netze.json'), 'w'))
'''


AUFBLAEHEN = 25.0
KEGEL = None


def main(pfad, ordner=None):
    global KEGEL
    ordner = ordner or os.path.join(HIER, 'zw', 'roh')
    os.makedirs(ordner, exist_ok=True)
    prims, j = lade_prims(pfad)
    KEGEL = Kegel(j)
    for n in BONES:
        v, f = marsch(n, prims)
        obj_schreiben(os.path.join(ordner, n + '.obj'), v, f)
        print(n, len(v), len(f))
    json.dump(ZIEL, open(os.path.join(ordner, 'ziele.json'), 'w'))
    sk = os.path.join(ordner, 'bl.py')
    open(sk, 'w', encoding='utf-8').write(BLENDER_SKRIPT)
    r = subprocess.run([BLENDER, '-b', '-P', sk, '--', ordner, os.path.join(ordner, 'ziele.json')], capture_output=True, text=True, encoding='utf-8', errors='replace')
    print('\n'.join(x for x in r.stdout.splitlines() if x.startswith('TEIL')))
    if not os.path.exists(os.path.join(ordner, 'netze.json')) or r.returncode:
        print(r.stdout[-2000:], r.stderr[-2000:])
        raise SystemExit(1)


if __name__ == '__main__':
    main(sys.argv[1])
