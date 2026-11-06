# Leser B v3: Muster des Schwanzes (S2) - Ringbaender quer, Laengsstreifen, Marmorierung.
# Gestrecktes Bild: entlang der Mittellinie (b_schwanz.py, mittellinie<n>.npy, 1 px Schritt) von s 0,1 bis 0,9,
#   quer dazu 21 Proben von -0,7 bis +0,7 der lokalen Halbbreite (Strahl bis Maskenrand); Wert L* (OpenCV Lab).
# Trend entfernt: Gauss sigma 12 px laengs je Spalte. Kennzahlen:
#   A = L* minus langsamer Verlauf laengs (Gauss 12 px der Zeilenmittel)
#   anteil_quer = Var(Zeilenmittel A) / Var(A) (Ringbaender quer -> hoch)
#   anteil_laengs = Var(Spaltenmittel A) / Var(A) (Laengsstreifen -> hoch; enthaelt den Rundungsschatten)
#   ringanteil = Var(Zeilenmittel R2) / Var(R2), R2 = A minus Spaltenmittel (Rundungsschatten entfernt); Ringe -> hoch
#   AKF laengs der Zeilenmittel: erstes lokales Maximum (Lag >= 4 px) und Hoehe -> Periode der Baender
#   Korrelationslaenge laengs/quer der Restflaeche (AKF < 0,5).
# Vergleich: Ruecken-Rechteck (Hautmuster) mit denselben AKF-Laengen in x und y.
# Aufruf: python b_muster.py <bild.png> <tail.npy> <mittellinie.npy> <name> [rx0:ry0:rx1:ry1]   (Bild = KI oder Port)
import sys, json, cv2, numpy as np
from scipy.ndimage import gaussian_filter1d, map_coordinates
im = cv2.imread(sys.argv[1]); tail = np.load(sys.argv[2]); C = np.load(sys.argv[3]); name = sys.argv[4]
L = cv2.cvtColor(im.astype(np.float32) / 255, cv2.COLOR_BGR2Lab)[..., 0]
n = len(C); i0, i1 = int(0.1 * n), int(0.9 * n)
T = np.gradient(C, axis=0); T /= np.linalg.norm(T, axis=1, keepdims=True); N = np.stack([-T[:, 1], T[:, 0]], 1)
rows = []
for i in range(i0, i1):
    p = C[i]; hw = []
    for sg in (1, -1):
        r = 0
        while True:
            x, y = (p + sg * N[i] * (r + 1)).round().astype(int)
            if not (0 <= y < tail.shape[0] and 0 <= x < tail.shape[1]) or tail[y, x] == 0:
                break
            r += 1
        hw.append(r)
    u = np.linspace(-0.7 * hw[1], 0.7 * hw[0], 21)
    xs = p[0] + N[i, 0] * u; ys = p[1] + N[i, 1] * u
    rows.append(map_coordinates(L, [ys, xs], order=1))
G = np.array(rows)
R = G - gaussian_filter1d(G, 12, axis=0, mode='nearest')
tot = R.var()
rowm = R.mean(1)
A = G - gaussian_filter1d(G.mean(1), 12, mode='nearest')[:, None]   # nur langsamer Helligkeitsverlauf laengs entfernt
colm = A.mean(0); quer_A = A.mean(1)
R2 = A - colm[None, :]   # Querschatten (Rundung) entfernt
ringanteil = float(R2.mean(1).var() / R2.var())   # ganze Zeile gemeinsam hell/dunkel = Ringband


def akf(v):
    v = v - v.mean(); a = np.correlate(v, v, 'full')[len(v) - 1:]; return a / a[0]


a = akf(rowm)
peak = None
for k in range(4, len(a) - 1):
    if a[k] > a[k - 1] and a[k] >= a[k + 1]:
        peak = (k, round(float(a[k]), 3)); break
# Korrelationslaenge laengs (Mittel der Spalten-AKF) und quer (Mittel der Zeilen-AKF)
al = np.mean([akf(R[:, j]) for j in range(R.shape[1])], 0); aq = np.mean([akf(R[i, :]) for i in range(R.shape[0])], 0)
cl_l = int(np.argmax(al < 0.5)); cl_q = int(np.argmax(aq < 0.5))
res = dict(bild=name, laenge_px=int(G.shape[0]), anteil_quer=round(float(quer_A.var() / A.var()), 3), anteil_laengs_inkl_rundungsschatten=round(float(colm.var() / A.var()), 3),
           ringanteil=round(ringanteil, 3), querprofil_L=[round(float(v), 1) for v in G.mean(0)[::4]],
           L_sd_rest=round(float(np.sqrt(tot)), 2), akf_laengs_erstes_max=peak, korrlaenge_laengs_px=cl_l, korrlaenge_quer_proben=cl_q,
           quer_probe_px=round(float(np.mean(np.diff(np.linspace(-0.7, 0.7, 21)))) , 3))
if len(sys.argv) > 5:
    x0, y0, x1, y1 = map(int, sys.argv[5].split(":")); P = L[y0:y1, x0:x1]
    P = P - cv2.GaussianBlur(P, (0, 0), 12)
    ax = np.mean([akf(P[i]) for i in range(P.shape[0])], 0); ay = np.mean([akf(P[:, j]) for j in range(P.shape[1])], 0)
    res["vergleich_haut"] = dict(rechteck=[x0, y0, x1, y1], korrlaenge_x_px=int(np.argmax(ax < 0.5)), korrlaenge_y_px=int(np.argmax(ay < 0.5)),
                                 L_sd_rest=round(float(P.std()), 2))
print(json.dumps(res))
cv2.imwrite(sys.argv[3].replace(".npy", "_gestreckt.png"), cv2.resize(np.clip(G * 2.55, 0, 255).astype(np.uint8), (21 * 6, G.shape[0]), interpolation=cv2.INTER_NEAREST))
