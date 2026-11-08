# SF V3 Runde 2: Belegzahlen der KI-Freisteller fuer Kopf V3 Runde 2 (Maul-Aufbau vorn, Gesichtsfeld gegen Nachbarhaut,
# Profil seitlich). Eigene Messung, Fenster aus Messblatt V3 (K1, Abschnitt 6) bzw. hier genannt. Ausgabe JSON (sfv3/n2/ki_kopf.json).
#  Vorn (birkin19 h118 H 569, birkin20 h125 H 599): Maulfenster x = Messblatt 6 (165-212 / 185-239), y = Augenmitte + 0,028 H ..
#   + 0,09 H. Zeilenprofil = Median L* je Zeile ueber die mittleren 60 % der Maulbreite. Band = Zeilen mit Median L* < 0,6 x Kuppe
#   (Kuppe = Messblatt K9 Ring um das Auge, hier Median L* im Ring 6-9 px). Spalt = Zeilen des Bands mit Median L* <= Band-P25.
#   Stummel = 8er-Komponenten L* > 40 im Band (1-12 px). Glanz = max(RGB) > 170 im Band (Sicht-Pruefer r1 B1).
#   Kinn = Zeilen Bandende+2 .. +0,06 H, mittlere 60 %.
#  Gesichtsfeld/Nachbarhaut: Feld = Augenmitte x +-0,035 H, y -0,035..+0,02 H; Nachbarhaut = gleiche Hoehe, x +-(0,09..0,15) H
#   (beide Seiten, nur Alpha > 128), Mittel L*.
#  Seite (birkin12 h85 H 504): Schnauzenspitze = rechtester Alpha-Punkt in y 100-135; Auge = dunkelster Punkt (Gauss 1) in
#   x Spitze-20..Spitze-6, y 115-130, Halbwert-Maske; Maulprofil = a* > 15 in x 330..Spitze, y 125-142; Stummel L* > 45 dort;
#   Schnauzenhaut oben y 100-118 / unten y 118-128 in x 345-368 (L* > 15).
import os, sys, json
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
from P_farbe import lab, r1
V = 'C:/workspace/git/reAi_v2/Version_80%/'
OUT = 'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/sfv3/n2/'


def lade(n):
    im = cv2.imread(V + 'birkin%02d.png' % n, cv2.IMREAD_UNCHANGED)
    rgb = im[..., 2::-1].astype(float)
    return rgb, im[..., 3], lab(rgb)


def vorn(n, H, auge, mx):
    rgb, al, A = lade(n); L = A[..., 0]
    ax, ay = auge
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ax, yy - ay)
    kuppe = float(np.median(L[(rr >= 6) & (rr <= 9)]))
    x0, x1 = mx; b = x1 - x0
    xc0, xc1 = int(x0 + 0.2 * b), int(x1 - 0.2 * b) + 1
    y0, y1 = int(ay + 0.028 * H), int(ay + 0.09 * H) + 1
    prof = np.array([np.median(L[y, xc0:xc1]) for y in range(y0, y1)])
    band = np.nonzero(prof < 0.6 * kuppe)[0]
    # laengste zusammenhaengende Folge
    folgen = np.split(band, np.nonzero(np.diff(band) > 1)[0] + 1)
    f = max(folgen, key=len)
    b0, b1 = y0 + f[0], y0 + f[-1]
    pb = prof[f]
    spalt = [y0 + f[i] for i in range(len(f)) if pb[i] <= np.percentile(pb, 25)]
    sub = L[b0:b1 + 1, x0:x1 + 1]; Asub = A[b0:b1 + 1, x0:x1 + 1]; rsub = rgb[b0:b1 + 1, x0:x1 + 1]
    st = sub > 40
    lb, k = nd.label(st, structure=np.ones((3, 3)))
    gr = nd.sum(st, lb, range(1, k + 1))
    stummel = [int(g) for g in gr if 1 <= g <= 12]
    gl = rsub.max(-1) > 170
    lippe = (~st) & (sub >= np.percentile(sub, 25))
    kinn = A[b1 + 2:int(b1 + 0.06 * H) + 1, xc0:xc1]
    feld = A[int(ay - 0.035 * H):int(ay + 0.02 * H) + 1, int(ax - 0.035 * H):int(ax + 0.035 * H) + 1, 0]
    nb = []
    for s in (-1, 1):
        xa, xb = sorted((int(ax + s * 0.09 * H), int(ax + s * 0.15 * H)))
        sl = (slice(int(ay - 0.035 * H), int(ay + 0.02 * H) + 1), slice(xa, xb + 1))
        nb.append(L[sl][al[sl] > 128])
    nb = np.concatenate(nb)
    return dict(H=H, kuppe_L=round(kuppe, 1),
                band_y=[int(b0), int(b1)], band_h_H=round((b1 - b0 + 1) / H, 4), band_oben_unter_auge_H=round((b0 - ay) / H, 4),
                spalt_zeilen=len(spalt), spalt_h_H=round(len(spalt) / H, 4), spalt_anteil=round(len(spalt) / (b1 - b0 + 1), 2),
                lab_spalt=r1(A[min(spalt):max(spalt) + 1, xc0:xc1].reshape(-1, 3).mean(0)),
                lab_band=r1(Asub.reshape(-1, 3).mean(0)), lab_lippe=r1(Asub[lippe].mean(0)),
                stummel_n=len(stummel), stummel_px=stummel, stummel_b_H=round(np.sqrt(np.mean(stummel)) / H, 4) if stummel else 0,
                lab_stummel=r1(Asub[st].mean(0)) if st.any() else None,
                glanz_px=int(gl.sum()), glanz_anteil_band=round(float(gl.mean()), 3), lab_glanz=r1(Asub[gl].mean(0)) if gl.any() else None,
                rgb_glanz=r1(rsub[gl].mean(0)) if gl.any() else None,
                lab_kinn=r1(kinn.reshape(-1, 3).mean(0)), L_kinn_kuppe=round(float(kinn[..., 0].mean()) / kuppe, 3),
                feld_L=round(float(feld.mean()), 1), nachbar_L=round(float(nb.mean()), 1), feld_nachbar=round(float(feld.mean() / nb.mean()), 3),
                L_band_kuppe=round(float(sub.mean()) / kuppe, 3), L_kinn=round(float(kinn[..., 0].mean()), 1))


def seite(n=12, H=504):
    rgb, al, A = lade(n); L = A[..., 0]
    ys = np.arange(100, 136)
    rechts = [int(np.nonzero(al[y] > 128)[0].max()) for y in ys]
    sx = max(rechts); sy = int(ys[int(np.argmax(rechts))])
    Lg = nd.gaussian_filter(L, 1)
    w = Lg[115:131, sx - 20:sx - 5]
    iy, ix = np.unravel_index(np.argmin(w), w.shape); ey, ex = 115 + iy, sx - 20 + ix
    # Haut um das Auge: Ring 4-7 px
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ex, yy - ey)
    Lh = float(np.median(L[(rr >= 4) & (rr <= 7) & (al > 128)]))
    lb, _ = nd.label((L < 0.5 * (L[ey, ex] + Lh)) & (rr < 6))
    m = lb == lb[ey, ex]
    my, mxs = np.nonzero(m)
    rot = (A[..., 1] > 15) & (al > 128)
    rot[:, :330] = False; rot[:125] = False; rot[143:] = False; rot[:, sx + 1:] = False
    ry, rx = np.nonzero(rot)
    stz = (L > 45) & (al > 128); stz[:, :330] = False; stz[:125] = False; stz[143:] = False
    lbs, ks = nd.label(stz, structure=np.ones((3, 3)))
    oben = A[100:119, 345:369][L[100:119, 345:369] > 15]; unten = A[119:129, 345:369][L[119:129, 345:369] > 15]
    OKy = int(np.nonzero((al > 128).any(1))[0].min())
    return dict(H=H, OK_y=OKy, spitze=[sx, sy], auge=[int(ex), int(ey)], auge_hinter_spitze_H=round((sx - ex) / H, 4),
                auge_unter_OK_H=round((ey - OKy) / H, 4), auge_b_H=round((mxs.ptp() + 1) / H, 4), auge_h_H=round((my.ptp() + 1) / H, 4),
                lab_auge=r1(A[m].mean(0)), L_auge_haut=round(float(L[m].mean()) / Lh, 3), L_haut_auge=round(Lh, 1),
                maul_x=[int(rx.min()), int(rx.max())], maul_y=[int(ry.min()), int(ry.max())],
                maul_hinter_spitze_H=[round((sx - rx.max()) / H, 4), round((sx - rx.min()) / H, 4)],
                maul_unter_auge_H=[round((ry.min() - ey) / H, 4), round((ry.max() - ey) / H, 4)],
                lab_maul=r1(A[rot].mean(0)), stummel_n=int(ks), lab_stummel=r1(A[stz].mean(0)) if stz.any() else None,
                lab_schnauze_oben=r1(oben.mean(0)), lab_schnauze_unten=r1(unten.mean(0)))


if __name__ == "__main__" and len(sys.argv) == 1:
    erg = dict(vorn_h118=vorn(19, 569, (185.9, 40.9), (165, 212)), vorn_h125=vorn(20, 599, (205.3, 41.0), (185, 239)), seite_h85=seite())
    print(json.dumps(erg, indent=1))
    json.dump(erg, open(OUT + 'ki_kopf.json', 'w'), indent=1)


def ringe(n, H, auge, schritt=0.005, bis=0.05):
    """Median L* in Ringen um das Kopfauge (nur oberhalb Augenmitte + 0,01 H, Alpha > 128): Halo der hellen Kuppe (K9)."""
    rgb, al, A = lade(n); L = A[..., 0]
    ax, ay = auge
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    rr = np.hypot(xx - ax, yy - ay) / H
    erg = []
    r = 0.0075
    while r < bis:
        m = (rr >= r) & (rr < r + schritt) & (yy <= ay + 0.01 * H) & (al > 128)
        erg.append((round(r + schritt / 2, 4), r1(np.median(A[m], axis=0))))
        r += schritt
    return erg


if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'ringe':
    for n, H, a in ((19, 569, (185.9, 40.9)), (20, 599, (205.3, 41.0))):
        print(n, ringe(n, H, a))


def schnauze(n=12):
    """KI birkin12 Schnauzenhaut (Fenster wie seite(): x 345-368, oben y 100-118, unten y 119-128, L* > 15): Lab P5/P50/P95 und Std."""
    rgb, al, A = lade(n); L = A[..., 0]
    erg = {}
    for nm, (y0, y1) in (('oben', (100, 119)), ('unten', (119, 129))):
        sub = A[y0:y1, 345:369]; m = (sub[..., 0] > 15) & (al[y0:y1, 345:369] > 128)
        v = sub[m]
        from scipy.ndimage import gaussian_filter
        hp = (L - gaussian_filter(L, 0.0037 * 504))[y0:y1, 345:369][m]          # Texelmassstab wie feinstreuung()
        erg[nm] = dict(L_p5_p50_p95=[round(float(x), 1) for x in np.percentile(v[:, 0], [5, 50, 95])], L_std=round(float(v[:, 0].std()), 1),
                       std_hp=round(float(hp.std()), 2),
                       ab_median=[round(float(np.median(v[:, 1])), 1), round(float(np.median(v[:, 2])), 1)])
    return erg


if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'schnauze':
    print(json.dumps(schnauze()))


def saeule(n, H, auge, mund_oben):
    """Senkrechtes Profil der Gesichtssaeule: Median L* je Zeile in x Augenmitte +-0,01 H, von 0,04 H ueber dem Auge bis zur
    Maul-Oberkante (Band aus vorn()); hell = L* >= Mittel aus Kuppe (Ring 6-9 px) und Maulband-L*; Rueckgabe: letzte helle Zeile
    unter dem Auge in H und das Profil."""
    rgb, al, A = lade(n); L = A[..., 0]
    ax, ay = auge
    x0, x1 = int(round(ax - 0.01 * H)), int(round(ax + 0.01 * H)) + 1
    ys = range(int(ay - 0.04 * H), mund_oben + 1)
    prof = [(round((y - ay) / H, 4), round(float(np.median(L[y, x0:x1])), 1)) for y in ys]
    return prof


if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'saeule':
    for n, H, a, mo in ((19, 569, (185.9, 40.9), 56), (20, 599, (205.3, 41.0), 57)):
        print(n, saeule(n, H, a, mo))


def feinstreuung(n, H, auge, texel_H=0.0037):
    """Streuung der KI-Haut auf Texelmassstab: Std von L* - Gauss(L*, sigma = 1 Texel) je Zone der Gesichtssaeule (x Auge +-0,02 H):
    Stirn dy < -0,0195 H, Kuppe -0,0195..+0,0155 H (ohne Auge |dy| <= 0,01 H und |dx| <= 0,012 H), unter dem Auge +0,0155..+0,026 H.
    Texel im Kopfbild 0,0035-0,004 H (Texelabstand, KOPF_V3 maul glanz_r)."""
    from scipy.ndimage import gaussian_filter
    rgb, al, A = lade(n); L = A[..., 0]
    hp = L - gaussian_filter(L, texel_H * H)
    ax, ay = auge
    yy, xx = np.mgrid[:L.shape[0], :L.shape[1]]
    dy, dx = (yy - ay) / H, (xx - ax) / H
    s = (np.abs(dx) <= 0.02) & (al > 128)
    zonen = dict(stirn=s & (dy >= -0.04) & (dy < -0.0195), kuppe=s & (dy >= -0.0195) & (dy < 0.0155) & ~((np.abs(dy) <= 0.01) & (np.abs(dx) <= 0.012)),
                 unter=s & (dy >= 0.0155) & (dy < 0.026))
    return {k: dict(std_hp=round(float(hp[m].std()), 2), L_median=round(float(np.median(L[m])), 1),
                    L_p5_p95=[round(float(x), 1) for x in np.percentile(L[m], [5, 95])]) for k, m in zonen.items()}


if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'fein':
    for n, H, a in ((19, 569, (185.9, 40.9)), (20, 599, (205.3, 41.0))):
        print(n, json.dumps(feinstreuung(n, H, a)))


def zeilen(n, H, auge, mx):
    """Zeilenprofil durch das Maul: Median L* (mittlere 60 % der Maulbreite, Messblatt 6) und P90 L* (ganze Breite) je Zeile von
    Augenmitte + 0,015 H bis + 0,08 H."""
    rgb, al, A = lade(n); L = A[..., 0]
    ax, ay = auge
    x0, x1 = mx; b = x1 - x0
    xc0, xc1 = int(x0 + 0.2 * b), int(x1 - 0.2 * b) + 1
    return [(round((y - ay) / H, 4), round(float(np.median(L[y, xc0:xc1])), 1), round(float(np.percentile(L[y, x0:x1 + 1], 90)), 1))
            for y in range(int(ay + 0.015 * H), int(ay + 0.08 * H))]


if __name__ == '__main__' and len(sys.argv) > 1 and sys.argv[1] == 'zeilen':
    for n, H, a, m in ((19, 569, (185.9, 40.9), (165, 212)), (20, 599, (205.3, 41.0), (185, 239))):
        print(n, zeilen(n, H, a, m))
