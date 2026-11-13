"""SF V8 Runde 7b (K1): Mass fuer die GLAETTE der Schwanz-Unterseite in ECHTEN Framedumps, geeicht am eigenen Fleisch des Modells.

    python glaette_r7b.py marker EMD ZIEL.EMD                     Klassen-Marker-Asset (jede Flaeche auf eine deckende Klassen-Zeile)
    python glaette_r7b.py mass DIR_LAUF DIR_MARKER [--von F] [--bis F] [--json JSON]
                                                                  Mass je Klasse (DIR_* = Laufordner von v4/lauf_v4.sh mit fd/)

Anlass (Sicht-Pruefer Runde 6, SF_PRUEFUNG_V8_r6_sicht.md (1b), woertlich): "Die Unterseite ist ein einheitliches, glattes, sattes Rot, fast
ohne Fleischstruktur. Von vorn in der Griff-Phase (F264, F276, auch F306/F312) wirkt der Schwanz als grosse leuchtend rote Klinge." Das
Profil des Riegels S15 (d) (L*-Median je 15-Grad-Band theta 90-180) misst den Verlauf QUER ueber den Schwanz (grau oben -> rot unten), nicht
die Zeichnung in der Flaeche; es kann die Glaette nicht zeigen. Dieses Mass misst sie: oertliche Streuung von L* (CIELAB, Videoraum
unveraendert = die Bildpunkte des Framedumps) in 5 x 5-Fenstern, die ganz in einer Klasse liegen.

Klassen aus einem Marker-Lauf (gleiche Szene, gleiche Posen; state.log bytegleich): jede Flaeche bekommt eine deckende Klassen-Zeile
  480 magenta  Schwanz-Unterseite (Meshes 16-22 auf der Fleisch-Zeile 495)
  481 gelb     Schwanz-Oberseite (Meshes 16-22 auf Zeile 487)
  482 gruen    Rumpf (Mesh 8)
  483 blau     Haut der Beine (Meshes 1-6 auf Zeile 480; Zeile 481 = Hose -> sonst)
  484 cyan     rechter Arm (Meshes 13-15)
  485 weiss    sonst
Unter den Flaechen des Assets liegen 0 transparente Texel (gemessen sfv8/r7b/zeilen.py), die Silhouette bleibt also gleich.
Eichung (Auftrag Runde 7b K1 (3)): am EIGENEN Fleisch des Modells - Rumpf und Beine in DENSELBEN Framedumps (vom Sicht-Pruefer nicht
beanstandet), nicht an der Farbe der Videobilder. Soll: Median der RELATIVEN oertlichen Streuung (sd / mittleres L* des Fensters) der
Unterseite >= dem kleineren der Mediane von Rumpf und Beinhaut und <= dem groesseren (zweiseitig: nicht flacher als das eigene Fleisch,
nicht unruhiger). Festgelegt nach der Probe F260-F280 am Stand a474188a und VOR der Messung der Varianten.
Kein RE-Wert; Fenster 5 x 5 und Rand-Erosion 2 px sind PORT-WAHL (Fenster = kleinste Flaeche, die in den Szenenbildern in die Unterseite passt).
"""
import os, sys, json, argparse
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402

FARBEN = {0: (31, 0, 31), 1: (31, 31, 0), 2: (0, 31, 0), 3: (0, 0, 31), 4: (0, 31, 31), 5: (31, 31, 31)}
NAMEN = {0: 'unterseite', 1: 'oberseite', 2: 'rumpf', 3: 'beinhaut', 4: 'arm_rechts', 5: 'sonst'}
FENSTER = 5
EROSION = 2
MIN_PX = 150                          # Klasse zaehlt in einem Bild ab 150 Fenster-Mittelpunkten


def klasse(b, z):
    if b in V.SCHWANZ:
        return 0 if z == 495 else 1
    if b == 8:
        return 2
    if 1 <= b <= 6 and z == 480:
        return 3
    if 13 <= b <= 15:
        return 4
    return 5


def marker(emd, ziel):
    blob = open(emd, 'rb').read()
    d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    n = {}
    for b, m in enumerate(ms):
        for U in (m['tuv'], m['quv']):
            for uv in U:
                k = klasse(b, (uv[2] >> 6) & 0x1FF)
                AV.setze_zeile(uv, 480 + k)
                n[NAMEN[k]] = n.get(NAMEN[k], 0) + 1
    for k, (r, g, bl) in FARBEN.items():
        t['clut'][k][:] = r | (g << 5) | (bl << 10)          # ohne STP-Bit (deckend, wie die meisten Original-Eintraege)
    os.makedirs(os.path.dirname(os.path.abspath(ziel)), exist_ok=True)
    open(ziel, 'wb').write(KB.packen(blob, d, ms, KB.tim_packen(t)))
    print('Marker', ziel, n)


def klassen_bild(a, n=None):
    """Marker-Framedump -> Klasse je Bildpunkt (-1 = keine). Nach Farbton: Kanal 'an' = >= 0,55 x hellster Kanal, 'aus' = <= 0,25 x.
    n = Framedump des normalen Laufs: nur Bildpunkte, die sich zwischen beiden Laeufen unterscheiden (> 8 in einem Kanal), gehoeren zur
    Figur (erster Lauf ohne diese Bedingung: grauer Hintergrund als 'sonst', dunkle Bodenpunkte als Klassen, Unterseite L* 3,7)."""
    a = a.astype(float)
    fig = None if n is None else (np.abs(a - n.astype(float)).max(-1) > 8)
    mx = a.max(-1)
    an = a >= 0.55 * mx[..., None]
    aus = a <= 0.25 * mx[..., None]
    k = np.full(a.shape[:2], -1, int)
    ok = mx >= 24
    if fig is not None:
        ok &= fig
    muster = {0: (1, 0, 1), 1: (1, 1, 0), 2: (0, 1, 0), 3: (0, 0, 1), 4: (0, 1, 1)}
    for kk, (r, g, b) in muster.items():
        m = ok.copy()
        for c, s in enumerate((r, g, b)):
            m &= an[..., c] if s else aus[..., c]
        k[m] = kk
    m = ok & an.all(-1)
    k[m] = 5
    return k


def lstar(a):
    import v4_gemein as G
    return G.lab(a.astype(float))[..., 0]


def box(x, w):
    from scipy import ndimage
    return ndimage.uniform_filter(x, size=w, mode='constant')


def mass_bild(Lb, kl):
    from scipy import ndimage
    out = {}
    gy, gx = np.gradient(Lb)
    grad = np.hypot(gx, gy)
    for k in NAMEN:
        m = ndimage.binary_erosion(kl == k, iterations=EROSION)
        if m.sum() < MIN_PX:
            continue
        mf = m.astype(float)
        cnt = box(mf, FENSTER) * FENSTER * FENSTER
        s1 = box(Lb * mf, FENSTER) * FENSTER * FENSTER
        s2 = box(Lb * Lb * mf, FENSTER) * FENSTER * FENSTER
        voll = m & (cnt >= FENSTER * FENSTER - 0.5)
        if voll.sum() < MIN_PX:
            continue
        mu = s1[voll] / cnt[voll]
        sd = np.sqrt(np.maximum(s2[voll] / cnt[voll] - mu * mu, 0.0))
        gr = grad[voll]
        out[NAMEN[k]] = dict(n=int(voll.sum()), sd=float(np.median(sd)), rel=float(np.median(sd / np.maximum(mu, 1.0))),
                             L=float(np.median(mu)), kante=float((gr >= 4.0).mean()))
    return out


def mass(dl, dm, von=200, bis=1100, schritt=1):
    from PIL import Image
    rows = {}
    for f in range(von, bis + 1, schritt):
        pl, pm = os.path.join(dl, 'fd', 'f%06d.ppm' % f), os.path.join(dm, 'fd', 'f%06d.ppm' % f)
        if not (os.path.exists(pl) and os.path.exists(pm)):
            continue
        a = np.asarray(Image.open(pl).convert('RGB'))
        kl = klassen_bild(np.asarray(Image.open(pm).convert('RGB')), a)
        r = mass_bild(lstar(a), kl)
        if r:
            rows[f] = r
    zus = {}
    for nm in NAMEN.values():
        v = [(f, r[nm]) for f, r in rows.items() if nm in r]
        if not v:
            continue
        zus[nm] = dict(bilder=len(v), sd=round(float(np.median([x['sd'] for _f, x in v])), 2),
                       sd_p25=round(float(np.percentile([x['sd'] for _f, x in v], 25)), 2),
                       sd_p75=round(float(np.percentile([x['sd'] for _f, x in v], 75)), 2),
                       rel=round(float(np.median([x['rel'] for _f, x in v])), 4),
                       L=round(float(np.median([x['L'] for _f, x in v])), 1),
                       kante=round(float(np.median([x['kante'] for _f, x in v])), 3))
    soll = None
    if 'rumpf' in zus and 'beinhaut' in zus and 'unterseite' in zus:
        # Soll auf der RELATIVEN Streuung (sd / L*): die Flaechen sind verschieden hell (Unterseite L* 16 gegen Rumpf 35 in F260-F280),
        # Helligkeit wirkt multiplikativ (Memory Texturregel Nachtrag 2026-11-09); die absolute Streuung steht daneben (nur Info)
        lo = min(zus['rumpf']['rel'], zus['beinhaut']['rel']); hi = max(zus['rumpf']['rel'], zus['beinhaut']['rel'])
        u = zus['unterseite']['rel']
        soll = dict(rel_lo=lo, rel_hi=hi, unterseite_rel=u, ok=bool(lo <= u <= hi),
                    sd_lo=min(zus['rumpf']['sd'], zus['beinhaut']['sd']), sd_hi=max(zus['rumpf']['sd'], zus['beinhaut']['sd']),
                    unterseite_sd=zus['unterseite']['sd'])
    return dict(von=von, bis=bis, klassen=zus, soll=soll, je_bild={str(f): r for f, r in rows.items()})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('was', choices=('marker', 'mass'))
    ap.add_argument('a'); ap.add_argument('b')
    ap.add_argument('--von', type=int, default=200); ap.add_argument('--bis', type=int, default=1100)
    ap.add_argument('--schritt', type=int, default=1)
    ap.add_argument('--json')
    a = ap.parse_args()
    if a.was == 'marker':
        marker(a.a, a.b)
        return
    r = mass(a.a, a.b, a.von, a.bis, a.schritt)
    for nm, z in r['klassen'].items():
        print('%-11s Bilder %3d  oertl. Streuung L* Median %5.2f (p25 %5.2f p75 %5.2f)  relativ %.4f  L* %5.1f  Kantenanteil %.3f' %
              (nm, z['bilder'], z['sd'], z['sd_p25'], z['sd_p75'], z['rel'], z['L'], z['kante']))
    print('Soll', r['soll'])
    if a.json:
        json.dump(r, open(a.json, 'w'), indent=1)


if __name__ == '__main__':
    main()
