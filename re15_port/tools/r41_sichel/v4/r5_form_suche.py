"""SF V4 Runde 5 (Bauer): Schwanzgelenke 2-5, Durchmesser 1-4 und Tiefe 2-3 nach den Befunden der Pruefung V4 r4 nachstellen.

Befunde (SF_PRUEFUNG_V4_r4_zahlen.md 4.1, Entscheid O11 "T3/T5 duerfen nicht schlechter sein als in Runde 3", O8 "Ober- und Unterkontur des
waagerechten Teils nicht gerade (TK)"): T3 dx -0,234 (Grenze -0,235), T5/T5q f 0,5 = 0,095 H (Soll 0,125 +-0,025, Original 0,117-0,121),
TK RMS 1,77 px (Original 3,87-4,71, Riegel >= 3,02).
Nebenbedingungen (Strafe, Marge 0,003 H): alle Bedingungen von r4_form_suche.viol4 (T1-T6/TA, T2 Lage + Drehung nach t2_mass_v4, T5 quer,
Oberkante x 276-292) und dazu TK: RMS der Oberkontur zur Geraden >= Riegelgrenze (r4_mass.oberkontur, riegel_v4.schwanz_form) + 0,2 px.
Gelenk 5 (Knick) ist frei, weil T3 nur am Knick haengt (Runde 4: Gelenke 5-7 nur fuer T4 nachgestellt).
Ziel (klein): wie r4_form_suche - mittlere Abweichung der Ober- und Unterkontur des waagerechten Teils vom Original (Leser-Maske A).
(1+1)-Evolutionsstrategie wie r3/r4_form_suche (Erfolg x1,3, Misserfolg x0,93).

    python r5_form_suche.py <saat> <schritte>     -> je Verbesserung eine Zeile "it N ziel Z pen P {json}" auf stdout
"""
import sys, os, json, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import r4_form_suche as F
import r2_expt as X, v4_gemein as G, b2
import r4_mass as R4
import riegel_v4 as RV

FO = R4.form_original()
RMS_MIN = min(d['kontur']['rms'] for d in FO) - (max(d['kontur']['rms'] for d in FO) - min(d['kontur']['rms'] for d in FO))   # = Riegel TK
base = copy.deepcopy(F.base)


def bau(x):
    g = np.array(base['gelenke'], float); d = np.array(base['durchmesser'], float); t = list(base['tiefe'])
    g[2] = x[0:2]; g[3] = x[2:4]; g[4] = x[4:6]; g[5] = x[6:8]
    dd = np.minimum.accumulate(np.r_[x[8:12]])                 # monoton fallend Gelenk 1 -> 4 (Messblatt Abschnitt 4 Punkt 3)
    d[1:5] = np.maximum(dd, d[5])
    t[2], t[3] = x[12], x[13]
    return dict(gelenke=g.round(4).tolist(), durchmesser=d.round(4).tolist(), tiefe=[round(v, 4) for v in t], seiten=base['seiten'])


def wert(x):
    sw = bau(x); F.BF.SCHWANZ_V4.update(copy.deepcopy(base))
    blob = X.bauen(sw); mo = b2.modell(blob)
    img, ids, iv = G.zeichne(mo, G.posen()['85'], voll=True)
    ok, so, H = G.h_port(iv)
    s = RV.schwanz_mass_h(img, ids, ok, so)
    v, tl, t5 = F.viol4(s, ids, H)
    sw_m = (ids >= 16) & (ids <= 22)
    k = R4.oberkontur(sw_m)
    v = np.r_[v, (RMS_MIN + 0.2 - k['rms']) / 100.0]          # in H-aehnlicher Groesse (1 px ~ 0,004 H)
    pen = 1e4 * np.sum(np.maximum(0, v + 0.003) ** 2)
    zk = F.ziel_kontur(sw_m, H)
    return zk, pen, sw, dict(dreh=round(tl['drehung'], 1), lage=(round(tl['dx'], 3), round(tl['dy'], 3)), T3=[round(c, 3) for c in s['T3']],
                             t5=[round(s['T5'][f], 3) for f in (0.1, 0.3, 0.5, 0.7)],
                             t5q=[round(t5['f'][f], 3) for f in (0.1, 0.3, 0.5, 0.7)], abfall=round(t5['abfall'], 3), tk=round(k['rms'], 2))


# Lauf "bauch" (RE15_R5_BAUCH=1): nur der Zwischenring im Glied 2 (birkin_formen.kette_meshes_v4 'bauch': t, Radiusfaktor, Versatz nach
# aussen) und Gelenk 3 (Lage, Durchmesser); Grundstand = Gelenke 5-7 je 0,0075 / 0,015 H nach hinten (T3) und 11 Seiten (M1: 7 x 22 + 11 +
# 11 + 22 = 198 <= 212)
BAUCH = os.environ.get('RE15_R5_BAUCH') == '1'
GRUND_BAUCH = dict(gelenke=[[0.12, -0.08], [-0.037, -0.04], [-0.1462, -0.0115], [-0.2871, 0.0604], [-0.2456, 0.1649], [-0.268, 0.2852],
                            [-0.2543, 0.3405], [-0.2313, 0.3924]], seiten=11)


def bau_bauch(x):
    g = np.array(GRUND_BAUCH['gelenke'], float); d = np.array(base['durchmesser'], float)
    g[3] = x[3:5]; d[3] = min(max(x[5], d[4]), d[2])
    return dict(gelenke=g.round(4).tolist(), durchmesser=d.round(4).tolist(), seiten=11,
                bauch=dict(glied=2, t=round(float(np.clip(x[0], 0.15, 0.85)), 3), radius=round(float(np.clip(x[1], 0.8, 2.0)), 3),
                           aussen=round(float(np.clip(x[2], -0.8, 0.8)), 3)))


if BAUCH:
    bau = bau_bauch      # noqa: F811


if __name__ == '__main__' and BAUCH:
    rng = np.random.default_rng(int(sys.argv[1]))
    g = np.array(GRUND_BAUCH['gelenke'], float)
    x = np.r_[0.5, 1.2, 0.2, g[3], base['durchmesser'][3]]
    sd = np.r_[0.1, 0.1, 0.1, 0.006, 0.006, 0.008]
    zk, pen, sw, info = wert(x); f = zk + pen
    print('start ziel %.4f pen %.3f %s %s' % (zk, pen, json.dumps(info), json.dumps(sw)), flush=True)
    for it in range(int(sys.argv[2])):
        y = x + rng.normal(0, 1, len(x)) * sd
        try:
            z2, p2, sw2, i2 = wert(y)
        except Exception:
            continue
        if z2 + p2 < f:
            x, f, zk, pen, sw, info = y, z2 + p2, z2, p2, sw2, i2; sd *= 1.3
            print('it %d ziel %.4f pen %.3f %s %s' % (it, zk, pen, json.dumps(info), json.dumps(sw)), flush=True)
        else:
            sd *= 0.93
        sd = np.maximum(sd, np.r_[0.01, 0.01, 0.01, 0.001, 0.001, 0.001])
    sys.exit(0)

if __name__ == '__main__':
    rng = np.random.default_rng(int(sys.argv[1]) if len(sys.argv) > 1 else 1)
    st = json.loads(os.environ['RE15_R5_START']) if os.environ.get('RE15_R5_START') else base
    g = np.array(st['gelenke'], float); d = st['durchmesser']; t = st['tiefe']
    x = np.r_[g[2], g[3], g[4], g[5], d[1], d[2], d[3], d[4], t[2], t[3]]
    sd = np.r_[[0.010] * 8, [0.010] * 4, [0.02] * 2]
    zk, pen, sw, info = wert(x); f = zk + pen
    print('start ziel %.4f pen %.3f %s %s' % (zk, pen, json.dumps(info), json.dumps(sw)), flush=True)
    for it in range(int(sys.argv[2]) if len(sys.argv) > 2 else 250):
        y = x + rng.normal(0, 1, len(x)) * sd
        y[12:14] = np.clip(y[12:14], 0, 0.15)
        y[8:12] = np.clip(y[8:12], 0.03, 0.16)
        try:
            z2, p2, sw2, i2 = wert(y)
        except Exception:
            continue
        f2 = z2 + p2
        if f2 < f:
            x, f, zk, pen, sw, info = y, f2, z2, p2, sw2, i2; sd *= 1.3
            print('it %d ziel %.4f pen %.3f %s %s' % (it, zk, pen, json.dumps(info), json.dumps(sw)), flush=True)
        else:
            sd *= 0.93
        sd = np.maximum(sd, 0.002)
