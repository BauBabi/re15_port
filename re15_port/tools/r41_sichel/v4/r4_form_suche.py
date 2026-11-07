"""SF V4 Runde 4 (Bauer): Schwanzgelenke 2-4, Durchmesser 1-4 und Tiefe 2-3 nach den Befunden der Pruefung V4 r3 nachstellen.

Befunde: T2-Lage nach dem O2-Werkzeug +0,100 H (Soll +0,036 +-0,03, Original +0,056), T5 nicht monoton (Durchmesser Gelenk 3 0,1599 >
Gelenk 2 0,1243, gegen Messblatt V4 Abschnitt 4 Punkt 3), waagerechter Teil ein gerader Balken mit spitzer Aussenecke (Sicht 1.3:
Oberkante x 276-292 6-16 px ueber dem Original, Oberkontur 0,6 px RMS zur Geraden gegen 3,9).

Nebenbedingungen (Strafe, Marge 0,003 H): T1-T6/TA wie r3_form_suche.viol; T2-Drehung 68,4 +-5 (t2_mass_v4); T2-Lage dx -0,245 /
dy +0,036 je +-0,03 (Messblatt, mit dem O2-Werkzeug gemessen, r4_mass.t2_lage_port); T5 mit dem unabhaengigen Quer-Mass
(r4_mass.t5_quer_port) in denselben Toleranzen und Abfall <= 0,01; Oberkante x 276-292 je Spalte <= 0,03 H vom Original.
Durchmesser monoton (Gelenk 1 >= 2 >= 3 >= 4 >= Knick 0,027): Messblatt Abschnitt 4 Punkt 3 "Breite je Glied nach der Breitenzeile
(0,14 -> 0,125 -> 0,065 -> 0,03 H zum Knick)".
Ziel (klein): mittlere Abweichung der Ober- und Unterkontur des waagerechten Teils und der Bogenecke vom Original (Leser-Maske A,
Spalten x 276-354 oben, x 306-354 unten; Vorlage 2, gleiche Kamera wie TA), in H.
(1+1)-Evolutionsstrategie wie r3_form_suche (Erfolg x1,3, Misserfolg x0,93).

    python r4_form_suche.py <saat> <schritte>     -> je Verbesserung eine Zeile "it N ziel Z pen P {json}" auf stdout
"""
import sys, os, json, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import r2_expt as X, t2_mass_v4 as T, v4_gemein as G, b2, birkin_formen as BF
import r4_mass as R4
import riegel_v4 as RV

SEITEN = int(os.environ.get('RE15_R4_SEITEN', '12'))
ORIG = R4.masken_original()[0]
O2 = T.original()


def kontur(m, xs, oben=True):
    out = {}
    for x in xs:
        yy = np.nonzero(m[:R4.Y_MAX + 8, x])[0] if oben else np.nonzero(m[:R4.Y_MAX, x])[0]
        out[x] = (yy.min() if oben else yy.max()) if len(yy) else None
    return out


XO = list(range(276, 355, 2)); XU = list(range(306, 355, 2))
K_OBEN = kontur(ORIG, XO); K_UNTEN = kontur(ORIG, XU, False)


def ziel_kontur(sw, H):
    ko = kontur(sw, XO); ku = kontur(sw, XU, False); d = []
    for x in XO:
        if K_OBEN[x] is not None and x < 336:            # ab x 336 ist die Leser-Maske oben abgeschnitten (Sicht-Pruefer)
            d.append(abs((ko[x] if ko[x] is not None else K_OBEN[x] + 30) - K_OBEN[x]))
    for x in XU:
        if K_UNTEN[x] is not None:
            d.append(abs((ku[x] if ku[x] is not None else K_UNTEN[x] - 30) - K_UNTEN[x]))
    return float(np.mean(d)) / H


# Nebenbedingungen T1-T6/TA: Kopie von r3_form_suche.viol (das Modul beendet sich beim Import)
def viol(s):
    v=[]
    H=s['H']
    t2=s['T2']; v+= [abs(t2[0]+0.245)-0.03, abs(t2[1]-0.036)-0.03]
    t3=s['T3']; v+= [abs(t3[0]+0.265)-0.03, abs(t3[1]-0.257)-0.03]
    t4=s['T4']; v+= [abs(t4[0]+0.245)-0.03, abs(t4[1]-0.397)-0.03, abs(s['ende_ueber_sohle']-0.37)-0.03]
    t1=s['T1']; v+= [abs(t1[0]-0.15)-0.03, abs(t1[1]-0.767)-0.03]
    for nm,p,q in (('b',(357,243),s['beginn']),('k',(290,308),s['knick']),('e',(295,343),s['ende'])):
        v+= [abs(q[0]-p[0])/H-0.03, abs(q[1]-p[1])/H-0.03]
    f=s['T5']
    for k,(z,t) in {0.1:(0.03,0.015),0.3:(0.065,0.02),0.5:(0.125,0.025),0.7:(0.14,0.025)}.items(): v.append(abs(f[k]-z)-t)
    reihe=[f[k] for k in sorted(f) if 0.1<=k<=0.7+1e-9]; v.append(max([reihe[i]-reihe[i+1] for i in range(len(reihe)-1)]+[0])-0.01)
    v+= [0.025-s['end_breite'], s['end_breite']-0.05, 0.12-s['end_laenge'], s['end_laenge']-0.17, 75-s['end_richtung'] if False else 0, s['taille']-0.035, s['taille']-s['schmal_vor_knick']]
    v+= [(75-s['end_richtung'])/100, (s['end_richtung']-95)/100]
    return np.array(v)


def viol4(s, ids, H):
    v = list(viol(s))
    tl = R4.t2_lage_port(ids, H, s['beginn'])
    v += [abs(tl['drehung'] - O2['ziel']) / 100.0 - O2['tol'] / 100.0, abs(tl['dx'] + 0.245) - 0.03, abs(tl['dy'] - 0.036) - 0.03]
    t5 = R4.t5_quer_port(ids, H, s['beginn'], s['knick'])
    for k, (z, t) in {0.1: (0.03, 0.015), 0.3: (0.065, 0.02), 0.5: (0.125, 0.025), 0.7: (0.14, 0.025)}.items():
        v.append(abs(t5['f'][k] - z) - t)
    v.append(t5['abfall'] - 0.01)
    sw = (ids >= 16) & (ids <= 22)
    ka = R4.oberkontur(sw)['kante']; ko = R4.oberkontur(ORIG)['kante']
    for x in R4.X_KANTE:
        v.append((abs(ka[x] - ko[x]) / H if ka[x] is not None else 0.1) - 0.03)
    return np.array(v), tl, t5


base = copy.deepcopy(BF.SCHWANZ_V4)
base['seiten'] = SEITEN


ENDE = os.environ.get('RE15_R4_ENDE') == '1'      # Nachstellung nur der Gelenke 5-7 (Knick/Endstueck) ab dem Stand im Generator


def bau_ende(x):
    g = np.array(base['gelenke'], float); g[5:8] = x.reshape(3, 2)
    return dict(gelenke=g.round(4).tolist(), seiten=SEITEN)


def bau(x):
    if ENDE:
        return bau_ende(x)
    g = np.array(base['gelenke'], float); d = np.array(base['durchmesser'], float); t = list(base['tiefe'])
    g[2] = x[0:2]; g[3] = x[2:4]; g[4] = x[4:6]
    dd = np.minimum.accumulate(np.r_[x[6:10]])                 # monoton fallend Gelenk 1 -> 4
    d[1:5] = np.maximum(dd, d[5])
    t[2], t[3] = x[10], x[11]
    if len(x) > 12:                                            # Saat >= 10: auch Gelenk 1 (Beginn) frei
        g[1] = x[12:14]
    return dict(gelenke=g.round(4).tolist(), durchmesser=d.round(4).tolist(), tiefe=[round(v, 4) for v in t], seiten=SEITEN)


def wert(x):
    sw = bau(x); BF.SCHWANZ_V4.update(copy.deepcopy(base))
    blob = X.bauen(sw); mo = b2.modell(blob)
    img, ids, iv = G.zeichne(mo, G.posen()['85'], voll=True)
    ok, so, H = G.h_port(iv)
    s = RV.schwanz_mass_h(img, ids, ok, so)
    v, tl, t5 = viol4(s, ids, H)
    pen = 1e4 * np.sum(np.maximum(0, v + 0.003) ** 2)
    zk = ziel_kontur((ids >= 16) & (ids <= 22), H)
    return zk, pen, sw, dict(dreh=round(tl['drehung'], 1), lage=(round(tl['dx'], 3), round(tl['dy'], 3)),
                             t5q=[round(t5['f'][k], 3) for k in (0.1, 0.3, 0.5, 0.7)], abfall=round(t5['abfall'], 3))


if __name__ == '__main__':
    rng = np.random.default_rng(int(sys.argv[1]) if len(sys.argv) > 1 else 1)
    g = np.array(base['gelenke'], float); d = base['durchmesser']; t = base['tiefe']
    start = os.environ.get('RE15_R4_START')
    if start:
        st = json.loads(start)
        g = np.array(st['gelenke'], float); d = st['durchmesser']; t = st['tiefe']
    x = np.r_[g[2], g[3], g[4], d[1], d[2], min(d[3], d[2]), d[4], t[2], t[3]]
    sd = np.r_[[0.012] * 6, [0.010] * 4, [0.02] * 2]
    if ENDE:
        x = g[5:8].ravel().copy(); sd = np.full(6, 0.006)
    if int(sys.argv[1]) >= 10 and not ENDE:
        x = np.r_[x, g[1]]; sd = np.r_[sd, [0.008] * 2]
    zk, pen, sw, info = wert(x); f = zk + pen
    print('start ziel %.4f pen %.3f %s %s' % (zk, pen, json.dumps(info), json.dumps(sw)), flush=True)
    for it in range(int(sys.argv[2]) if len(sys.argv) > 2 else 250):
        y = x + rng.normal(0, 1, len(x)) * sd
        if not ENDE:
            y[10:12] = np.clip(y[10:12], 0, 0.15)
            y[6:10] = np.clip(y[6:10], 0.03, 0.16)
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
