"""Vergleichsblatt (Messbild - kein Ergebnis): Schwanz-Netz (Meshes 16-22) gestreckt, Ansicht von oben (Ruecken), von der Seite (Flanke) und von
unten (Bauch); Spalten = Assets. Gleiche Beleuchtung in allen Zellen (emd_ansichtsblatt.render: Lambert kamerarelativ, Orthogonalprojektion,
Textur aus dem Asset; nicht das Spiellicht).

    python blatt.py <aus.png> <name>=<emd> [<name>=<emd> ...]

Streckung: jedes Glied g im eigenen Knochenraum, um -innen_phi (unterseite_v8.json) um y gedreht (Bauchachse e_h -> +x, Querachse e_q -> +z, wie
riegel_schwanz_r8.meshes_gedreht), um g x 541 entlang y verschoben. Oben = Blick von -x (Ruecken), Seite = Blick von +z (Bauch unten im Bild),
unten = Blick von +x (Bauch).
"""
import sys, json
import numpy as np
from PIL import Image, ImageDraw, ImageFont
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools'
sys.path[:0] = [T, T + '/r41_sichel/v8', T + '/r41_sichel/v6kopf', T + '/r41_sichel/v4', T + '/r41_sichel/v2']
import emd_ansichtsblatt as EA      # noqa: E402
import birkin_buehne as BB          # noqa: E402

PHI = {int(k): float(v) for k, v in json.load(open(T + '/r41_sichel/v8/unterseite_v8.json'))['innen_phi'].items()}
L = 541.0
ANSICHTEN = [('oben (Ruecken)', (0, 1, 0), (0, 0, 1), (-1, 0, 0)),
             ('Seite (Flanke, Bauch unten)', (0, 1, 0), (1, 0, 0), (0, 0, 1)),
             ('unten (Bauch)', (0, 1, 0), (0, 0, -1), (1, 0, 0))]
W, H, SC = 900, 250, 0.185
CX, CY = 30, 125


def geometrie(mo):
    V, N, F = [], [], []
    for b in range(16, 23):
        m = mo.meshes[b]
        p = np.radians(PHI[b])
        R = np.array([[np.cos(p), 0, np.sin(p)], [0, 1, 0], [-np.sin(p), 0, np.cos(p)]])
        off = np.array([0.0, (b - 16) * L, 0.0])

        def xf(vs):
            return np.array([v[:3] for v in vs], float) @ R.T + off

        def xn(ns):
            w = np.array([v[:3] for v in ns], float) @ R.T
            n = np.linalg.norm(w, axis=1, keepdims=True); n[n == 0] = 1
            return w / n
        tvb = len(V); V.extend(xf(m.tv)); N.extend(xn(m.tn))
        qvb = len(V); V.extend(xf(m.qv)); N.extend(xn(m.qn))
        for i, (n0, v0, n1, v1, n2, v2) in enumerate(m.tf):
            uv = m.tuv[i]
            F.append(dict(v=(tvb + v0, tvb + v1, tvb + v2), n=(tvb + n0, tvb + n1, tvb + n2),
                          uv=((uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])), clut=uv[2], page=uv[5], bone=b))
        for i, (n0, v0, n1, v1, n2, v2, n3, v3) in enumerate(m.qf):
            uv = m.quv[i]
            a, bb, c, dd = (uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])
            F.append(dict(v=(qvb + v0, qvb + v1, qvb + v2), n=(qvb + n0, qvb + n1, qvb + n2), uv=(a, bb, c), clut=uv[2], page=uv[5], bone=b))
            F.append(dict(v=(qvb + v1, qvb + v3, qvb + v2), n=(qvb + n1, qvb + n3, qvb + n2), uv=(bb, dd, c), clut=uv[2], page=uv[5], bone=b))
    return np.array(V), np.array(N), F


def zelle(mo, ans):
    V, N, F = geometrie(mo)
    _n, r, d, z = ans
    col, alp = EA.render(mo, V, N, F, r, d, z, W, H, SC, CX, CY)
    bg = np.array([40, 42, 48], np.float32) / 255.0
    img = col * alp[..., None] + bg * (1 - alp[..., None])
    return (np.clip(img, 0, 1) * 255).astype(np.uint8)


def main():
    aus = sys.argv[1]
    spalten = [a.split('=', 1) for a in sys.argv[2:]]
    try:
        fk = ImageFont.truetype('arial.ttf', 26); fs = ImageFont.truetype('arial.ttf', 17)
    except OSError:
        fk = fs = ImageFont.load_default()
    KOPF, LBL, PAD = 70, 30, 12
    BW = len(spalten) * (W + PAD) + PAD
    BH = KOPF + LBL + len(ANSICHTEN) * (H + 24) + PAD
    blatt = Image.new('RGB', (BW, BH), (22, 23, 27))
    dr = ImageDraw.Draw(blatt)
    dr.rectangle([0, 0, BW, KOPF - 8], fill=(150, 30, 30))
    dr.text((PAD, 12), 'Messbild - kein Ergebnis', font=fk, fill=(255, 255, 255))
    dr.text((PAD + 360, 18), 'Schwanz-Netz Meshes 16-22 gestreckt, gleiche Beleuchtung (Lambert kamerarelativ, nicht Spiellicht), Textur des Assets',
            font=fs, fill=(240, 220, 220))
    for s, (name, pfad) in enumerate(spalten):
        mo = BB.modell(open(pfad, 'rb').read())
        x0 = PAD + s * (W + PAD)
        dr.text((x0, KOPF), name, font=fk, fill=(232, 233, 236))
        for a, ans in enumerate(ANSICHTEN):
            y0 = KOPF + LBL + a * (H + 24)
            dr.text((x0, y0), ans[0] + '   (links Wurzel Glied 16, rechts Nadel)', font=fs, fill=(170, 172, 180))
            blatt.paste(Image.fromarray(zelle(mo, ans)), (x0, y0 + 22))
    blatt.save(aus)
    print('ok', aus, blatt.size)


if __name__ == '__main__':
    main()
