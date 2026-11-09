"""Grundlage v2: gemeinsame Hilfen. Alle Zonen in GRUNDLAGE-Koordinaten (Rueckseite.png / birkin_form3_02.png, Faktor 2 zur Vorlage).
Kennungs-Render hochaufgeloest direkt in Grundlage-Koordinaten (Kamera-Matrix mit der Abbildung Vorlage->Grundlage verkettet)."""
import os, sys, json, copy
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
WT = 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form'
K5 = WT + '/analysis/befunde_runde41/form/varianten/K5'
sys.path.insert(0, K5 + '/farbe_lesung/V4F_skripte')
sys.path.insert(0, K5 + '/zonen_nutzer/skripte')
import v4f  # noqa
P, FB = v4f.P, v4f.FB
D = 'C:/workspace/git/reAi_v2/Version_80%/4_Beiner_Zerlegt/'
ARBEIT = os.environ.get('GRUNDLAGE_ARBEIT', HIER)
GRUND = {1: 'Rückseite.png', 2: 'birkin_form3_02.png'}
ASSET = ARBEIT + '/asset'
K5C = ARBEIT + '/k5c'
MATERIAL_JSON = K5 + '/ruhe/messung/material.json'


def lies(p):
    return cv2.imdecode(np.fromfile(p, np.uint8), cv2.IMREAD_UNCHANGED)


VS = json.load(open(ARBEIT + '/versatz.json', encoding='utf-8'))
AB = json.load(open(ARBEIT + '/abbildung.json', encoding='utf-8'))
TEILNAME = {0: 'Hüfte', 1: 'Schwanz 1', 2: 'Schwanz 2', 3: 'Schwanz 3', 4: 'Schwanz 4', 5: 'Schwanz 5', 6: 'Schwanzspitze (Zipfel)',
            7: 'Rücken/Wirbel', 8: 'Oberschenkel hinten links', 9: 'Oberschenkel hinten rechts', 10: 'Brust/Buckel',
            11: 'Oberarm vorn links', 12: 'Unterarm vorn links', 13: 'Hand vorn links', 14: 'Kralle vorn links 14', 15: 'Kralle vorn links 15',
            16: 'Oberarm vorn rechts', 17: 'Unterarm vorn rechts', 18: 'Hand vorn rechts', 19: 'Kralle vorn rechts 19', 20: 'Kralle vorn rechts 20',
            21: 'Hals', 22: 'Kopf (Oberkiefer)', 23: 'Unterkiefer', 24: 'Unterschenkel hinten links', 25: 'Fuß hinten links',
            26: 'Unterschenkel hinten rechts', 27: 'Fuß hinten rechts', 28: 'Kralle vorn links 28', 29: 'Kralle vorn rechts 29',
            30: 'Kralle vorn links 30', 31: 'Kralle vorn rechts 31'}


def s_t(n):
    a = AB[str(n)]['aehnlich']
    return float(a['massstab']), np.array(a['versatz'], float)


def v_aus_g(n, xg, yg):
    """Grundlage (Pixelmitte ganzzahlig) -> Vorlage (Pixelmitte ganzzahlig)."""
    s, t = s_t(n)
    return s * np.asarray(xg, float) + t[0], s * np.asarray(yg, float) + t[1]


def g_aus_v(n, xv, yv):
    s, t = s_t(n)
    return (np.asarray(xv, float) - t[0]) / s, (np.asarray(yv, float) - t[1]) / s


def grund_rgba(n):
    return lies(D + GRUND[n])


def grund_auf_weiss(n):
    B = grund_rgba(n); a = B[..., 3:4] / 255.0
    return (B[..., :3][..., ::-1] * a + 255 * (1 - a)).astype(np.uint8)  # RGB


def grund_lab(n):
    rgb = grund_auf_weiss(n).astype(np.float32) / 255.0
    return cv2.cvtColor(rgb, cv2.COLOR_RGB2LAB)


def figur(n):
    return grund_rgba(n)[..., 3] > 128


def vorlage(n):
    return v4f.original(n)


def vorlage_in_g(n):
    """Video (Vorlage) auf Grundlage-Raster gezogen (RGB)."""
    s, t = s_t(n); B = grund_rgba(n); h, w = B.shape[:2]
    M = np.array([[s, 0, t[0]], [0, s, t[1]]], np.float32)
    return cv2.warpAffine(vorlage(n), M, (w, h), flags=cv2.INTER_LINEAR | cv2.WARP_INVERSE_MAP)


def g_matrix(n):
    """Kamera-Abbildung (Port-Pixel -> Vorlage, projektiv) verkettet mit Vorlage -> Grundlage.
    Konvention: rastern tastet Pixel i bei i+0,5; die Aehnlichkeit gilt fuer Pixelmitten auf ganzen Zahlen."""
    kam = P.kamera(n)
    Mv = np.asarray(kam.abbildung['matrix'], float).reshape(3, 3)
    s, t = s_t(n)
    # kontinuierlich: V = s*(g-0.5) + t + 0.5  ->  g = (V - 0.5 - t)/s + 0.5
    Ainv = np.array([[1 / s, 0, (-0.5 - t[0]) / s + 0.5], [0, 1 / s, (-0.5 - t[1]) / s + 0.5], [0, 0, 1]], float)
    return Ainv @ Mv, kam


def kand(ordner):
    return v4f.kand(ordner)


def render_g(k, n, meshes_ohne=(), mo=None, licht=True):
    """Kennungs-Render in Grundlage-Koordinaten. k = Kandidat; mo optional ersetztes Modell (gleiche Pose-Daten)."""
    mo = mo or k.mo
    lage, gier, clip, bild = k.stand(n)
    Mg, kam = g_matrix(n)
    kf = k.kf(clip, bild)
    li = None
    if licht:
        li, _ = P.licht_fuer(n, k.licht)
    tris = v4f.dreiecke_id(mo, kf, lage, gier, kam, li)
    if meshes_ohne:
        tris = [t for t in tris if t[5][0] not in meshes_ohne]
    h, w = grund_rgba(n).shape[:2]
    ziel = np.full((h, w, 3), 0, np.uint8)
    K = v4f.rastern_id(tris, k.tex, ziel, dict(matrix=Mg.tolist()))
    return ziel, K


def lade_render(name, n, erzeuge):
    p = ARBEIT + '/render_%s_%d.npz' % (name, n)
    if os.path.exists(p):
        d = dict(np.load(p))
        if 'u' in d:  # Runde 2: Texel-Kennung (u, v, CLUT-Zeile) muss mit gespeichert sein
            return d['rgb'], d
    rgb, K = erzeuge()
    np.savez_compressed(p, rgb=rgb, **{a: K[a] for a in ('mesh', 'flaeche', 'maske', 'u', 'v', 'zeile')})
    d = dict(np.load(p)); return d['rgb'], d


def fl_schluessel(mesh, flaeche):
    """Kennung -> 'mesh|viereck|index' (wie material.json)."""
    return '%d|%d|%d' % (mesh, flaeche // 100000, flaeche % 100000)


def material_listen():
    m = json.load(open(MATERIAL_JSON, encoding='utf-8'))['material']
    return {k: set(v['liste']) for k, v in m.items()}


def alle_flaechen(mo):
    out = []
    for b, m in enumerate(mo.meshes):
        out += ['%d|0|%d' % (b, i) for i in range(len(m.tf))] + ['%d|1|%d' % (b, i) for i in range(len(m.qf))]
    return out
