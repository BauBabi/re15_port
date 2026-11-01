#!/usr/bin/env python3
"""Ansichtsblaetter je Fassung mit dem Werkzeug re15_port/tools/emd_ansichtsblatt.py (unveraendert aufgerufen).
Blob kommt aus lib_fassungen. Fusszeile des Werkzeugs (Gorilla-Text) wird abgeschnitten, Texturkachel zeigt 3 CLUT-Seiten."""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_fassungen import *
from PIL import Image
AUS = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

def geometrie_getrennt(self, keyframe):
    """Wie EA.Modell.geometrie, aber Vertex- und Normalen-Index getrennt gezaehlt (D36: Normalenzahl != Vertexzahl)."""
    R, T = self.pose(keyframe)
    V, N, F = [], [], []
    for b in range(min(self.skel.bone_count, len(self.meshes))):
        m = self.meshes[b]
        rot = R[b].astype(np.float64) / 4096.0
        tr = T[b].astype(np.float64)
        def xf(vs):
            return np.array([[v[0], v[1], v[2]] for v in vs], float) @ rot.T + tr if vs else np.zeros((0, 3))
        def xn(ns):
            if not ns:
                return np.zeros((0, 3))
            w = np.array([[v[0], v[1], v[2]] for v in ns], float) @ rot.T
            ln = np.linalg.norm(w, axis=1, keepdims=True); ln[ln == 0] = 1
            return w / ln
        tvb, tnb = len(V), len(N); V.extend(xf(m.tv)); N.extend(xn(m.tn))
        qvb, qnb = len(V), len(N); V.extend(xf(m.qv)); N.extend(xn(m.qn))
        for i, (n0, v0, n1, v1, n2, v2) in enumerate(m.tf):
            uv = m.tuv[i]
            F.append(dict(v=(tvb+v0, tvb+v1, tvb+v2), n=(tnb+n0, tnb+n1, tnb+n2),
                          uv=((uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])), clut=uv[2], page=uv[5], bone=b))
        for i, (n0, v0, n1, v1, n2, v2, n3, v3) in enumerate(m.qf):
            uv = m.quv[i]
            a, bb = (uv[0], uv[1]), (uv[3], uv[4])
            c, dd = (uv[6], uv[7]), (uv[9], uv[10])
            F.append(dict(v=(qvb+v0, qvb+v1, qvb+v2), n=(qnb+n0, qnb+n1, qnb+n2), uv=(a, bb, c), clut=uv[2], page=uv[5], bone=b))
            F.append(dict(v=(qvb+v1, qvb+v3, qvb+v2), n=(qnb+n1, qnb+n3, qnb+n2), uv=(bb, dd, c), clut=uv[2], page=uv[5], bone=b))
    return np.array(V), np.array(N), F

def modell(b, typ, titel, unter, tim=None):
    mo = EA.Modell.__new__(EA.Modell)
    d = EA.emd_dir(b)
    mo.blob, mo.typ, mo.blob_index, mo.blob_len = b, typ, 0, len(b)
    if len(d) >= 9:
        mo.meshes = EA.md1_parse(b[d[7]:d[8]])
        t = b[d[8]:]
    else:
        mo.meshes = EA.md1_parse(b[d[7]:])
        t = tim
    mo.skel = EA.emr_parse(b[d[2]:d[3]])
    mo.clips, mo.frames = EA.edd_parse(b[d[1]:d[2]])
    mo.tex, mo.pal, mo.clut_y = EA.tim_decode(t)
    mo.texh, mo.texw = mo.tex.shape
    mo.titel, mo.untertitel = titel, unter + ' (%d)'
    mo.hinweis = 'Darstellung wie Werkzeug: Orthogonalprojektion, Z-Buffer, Lambert; Daten unveraendert.'
    def tb(self=mo):
        w = self.texw // self.pal.shape[0]
        return np.concatenate([self.pal[i][self.tex[:, i*w:(i+1)*w]] for i in range(self.pal.shape[0])], axis=1)
    mo.texturblatt = tb
    import types
    mo.geometrie = types.MethodType(geometrie_getrennt, mo)
    return mo

def blatt(mo, clip, name):
    p = os.path.join(AUS, name)
    EA.blatt(mo, clip, p)
    im = Image.open(p)
    w, h = im.size
    im.crop((0, 0, w, h - EA.PAD*0 - 160 - 10)).save(p)   # Fusszeile (Gorilla-Text) ab
    print('->', p)

LISTE = sys.argv[1:] or ['D0', 'D1', 'M1', 'M0x', 'D36', 'R30', 'R31', 'R33', 'R34', 'R36', 'Z0a', 'Z0c', 'Z0d']
for n in LISTE:
    if n in FASSUNGEN:
        pf, s, l, t = FASSUNGEN[n]
        b = blob(n)
        typ = 0x30
        tn = t
        ut = ('Disc ' if 'Version_40' in pf else 'Mod ') + f'{os.path.basename(pf)} Sektor {s}, {l} B'
        mo = modell(b, typ, f'{n}: {tn}', ut)
    else:
        kind = int(n[1:], 16)
        e, se, le = re2_rec(kind, 3); ti, st, lt = re2_rec(kind, 2)
        mo = modell(e, kind, f'{n}: RE2 kind 0x{kind:02X}', f'shared_assets/RE2/CDEMD0.EMS EMD Sektor {se}, TIM Sektor {st}', ti)
    try:
        blatt(mo, 0, f'blatt_{n}.png')
    except Exception as ex:
        import traceback; traceback.print_exc()
        print('FEHLER', n, ex)
