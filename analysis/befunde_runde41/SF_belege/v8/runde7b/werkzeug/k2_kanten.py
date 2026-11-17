import sys, json, os
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8')
import v8_gemein as V
import numpy as np, kopf_orig_bau as KB, adern_v8 as AV
from unterseite_r5 import _zeile_oder_none, ring_ecke, SCHWANZ_ZEILE
def kanten_liste(blob):
    """Mass des Zahlen-Pruefers (Runde 5, p4kante.py) nachgebaut: jede Kante, an der im selben Mesh eine Schwanz-Flaeche der Fleisch-Zeile an
    eine der Zeile 487 stoesst (gleiche Ecken-Orte): 9 Proben je Kante, 1 Texel nach innen, unbeleuchtet; dL / da* (Fleisch-Zeile minus 487),
    mit der Kantenlaenge gewichtet; getrennt nach grauer Nachbarflaeche Viereck (Mantel) / Dreieck (Ring-Deckel, Dorn)."""
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    zeile = _zeile_oder_none(t)
    H, W = t['pix'].shape

    def probe(z, a, b, mitte):
        ein = mitte - (a + b) / 2.0
        ein = ein / (np.linalg.norm(ein) + 1e-9)
        c = []
        for s_ in np.linspace(0.1, 0.9, 9):
            q = a + (b - a) * s_ + ein
            u = int(np.clip(round(q[0]), 0, W - 1)); v = int(np.clip(round(q[1]), 0, H - 1))
            c.append(int(t['clut'][z - 480][t['pix'][v, u]]))
        return AV.lab15(np.array(c))
    res = []
    for b in V.SCHWANZ:
        m = ms[b]
        kanten = {}
        for kind, F, U in (('d', m['tf'], m['tuv']), ('v', m['qf'], m['quv'])):
            for k, (fc, uv) in enumerate(zip(F, U)):
                po = (uv[5] & 0xF) * 128; z = (uv[2] >> 6) & 0x1FF
                nk = len(fc) // 2
                tx = [(uv[3 * i] + po, uv[3 * i + 1]) for i in range(nk)]
                Pk = [tuple(m['v'][i][:3]) for i in fc[1::2]]
                if kind == 'v':
                    Pk = [Pk[0], Pk[1], Pk[3], Pk[2]]; tx = [tx[0], tx[1], tx[3], tx[2]]
                tx = np.array(tx, float)
                for i in range(nk):
                    a, c = Pk[i], Pk[(i + 1) % nk]
                    if a == c:
                        continue
                    key = (a, c) if a < c else (c, a)
                    ta, tc = (tx[i], tx[(i + 1) % nk]) if a < c else (tx[(i + 1) % nk], tx[i])
                    art = 'd' if (kind == 'd' and not all(ring_ecke(b, i_) for i_ in fc[1::2])) else 'v'
                    kanten.setdefault(key, []).append((art + str(k), z, ta, tc, tx.mean(0)))
        for key, lst in sorted(kanten.items()):
            f5 = [x for x in lst if x[1] == zeile]; f7 = [x for x in lst if x[1] == SCHWANZ_ZEILE]
            if f5 and f7:
                A = probe(zeile, f5[0][2], f5[0][3], f5[0][4]); B = probe(SCHWANZ_ZEILE, f7[0][2], f7[0][3], f7[0][4])
                lang = float(np.linalg.norm(np.array(key[0], float) - np.array(key[1], float)))
                res.append((b, f5[0][0], f7[0][0], lang, float(A[:, 0].mean() - B[:, 0].mean()), float(A[:, 1].mean() - B[:, 1].mean())))
    return res
    out = {}
    for art, sel in (('alle', res), ('mantel', [r for r in res if r[2][0] == 'v']), ('deckel', [r for r in res if r[2][0] == 'd'])):
        l = sum(r[3] for r in sel)
        out[art] = dict(kanten=len(sel), laenge=round(l), da=round(sum(r[3] * r[5] for r in sel) / l, 2) if l else 0.0,
                        dL=round(sum(r[3] * r[4] for r in sel) / l, 2) if l else 0.0,
                        da_max=round(max([abs(r[5]) for r in sel]), 1) if sel else 0.0)
    return out



for p in sys.argv[1:]:
    r = kanten_liste(open(p,'rb').read())
    g = [x for x in r if abs(x[5]) >= 15]
    print(p, 'Kanten 495/487', len(r), 'davon da* >= 15:', len(g), [(x[0], x[1], x[2], round(x[5],1), round(x[3])) for x in g])
