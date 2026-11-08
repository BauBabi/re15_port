"""SF V4 Runde 5 (Bauer): Schwanztextur der Sichel-Form aus Vorlage 2 - durchgehend entlang der Achse (Entscheid O8).

    python r5_schwanz_textur.py [--aus farben_schwanz_v4.npz] [--runden 3]

Anlass (Pruefung V4 r4 Sicht 1.3 Klasse A, Entscheid O8): der Schwanz bestand aus drei einfarbigen Bloecken je Mesh (L* 7,3 / 28,6 / 13,2,
a* 10 / 2 / 22) mit Quernaehten 16,7 dE76 an den Meshgrenzen, die Helligkeit lag verkehrt herum (an der Biegung L* 7,3, Original 26-28),
die Wurzel war eine weinrote Manschette (a* 22, Original 8-9). Ursache: je Glied eine Farbklasse (Wurzel / Mitte / senkrecht / Ende).

Verfahren (Messblatt V4 Abschnitt 5 "Schirmfarbe = Texel x Licht", hier je Texel wie r2_koerper_farben.py fuer den Koerper):
  1. Generator mit uv art 'achse' (birkin_formen.kette_meshes_v4): v = Bogenlaenge ueber ALLE Glieder fortlaufend, u = Lage um den
     Umfang (Vorder-/Rueckseite teilen den Texel). Render in der Kamera von Vorlage 2 (Pose "85") mit Texelkoordinate und Licht lambda je
     Bildpunkt (r2_koerper_farben.raster).
  2. Ziel je Bildpunkt = die Farbe des ORIGINALS an derselben Bildstelle (Vorlage 2 roh, Port-Schirm P = (V - a) / b). Liegt die Stelle
     im Original nicht auf dem Schwanz (die Port-Kontur weicht bis 0,03 H ab, Riegel TA), gilt der naechste Bildpunkt der Quellmaske:
     Leser-Maske A (t2_masken/schwanz_maske.npz, Messblatt Abschnitt 4) 2 px erodiert; am Ruecken (x >= X_RUECKEN, y < 262) zusaetzlich
     die Figur (Vorlage 2 weicht vom geeichten BSS-Hintergrund ab), dort geht der Schwanz "in den Ruecken ueber" (Nutzer-Beschriftung).
  3. Texel t = Sum lambda D / Sum lambda^2 je Kanal (kleinste Quadrate ueber die Bildpunkte des Texels); ungesehene Texel = naechster
     gesehener Texel (Abstandstransformation in der Texturflaeche).
  4. Nachfuehrung (Messblatt Abschnitt 5 Schritt 4 "Liegt dE00 ueber dem Riegel, T nachfuehren"): nach dem Bau wird die Vergleichsregel
     (sigma 1,5, a/b) am Render gemessen und die Abweichung zum Original je Bildpunkt auf das Ziel aufgeschlagen (--runden Mal).
  5. Quantisierung auf 255 Indizes (Index 0 bleibt frei) per k-Mittel; Zeile 487 = Klassenmittel. 8 bpp, kein Weichzeichner (M3).
Keine RE-Werte: die Sichel-Form gibt es auf der Disc nicht; die Farben sind das 80%-Video (Vorlage 2 = n1435).
"""
import os, sys, argparse, json
import numpy as np
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_formen as BF
import r2_koerper_farben as RK
import r4_mass as R4

AUS = os.path.join(HIER, 'farben_schwanz_v4.npz')
# Am Ruecken zaehlt die ganze Figur als Quelle (Messblatt Abschnitt 4: "Die Schwanz-Oberseite geht dort ohne Kante in den Buckel
# ueber"): ab dem Nutzerpunkt Beginn x 357 (Messblatt 1a) minus 0,05 H2 - dort endet die Leser-Maske oben (x 336, Sicht-Pruefer V4 r3
# 1.3: "ab x 336 ist die Leser-Maske oben bei y 196 abgeschnitten")
X_RUECKEN = 357 - int(round(0.05 * G.H_ORIG[2]))
# Figur = Vorlage 2 weicht vom geeichten BSS-Hintergrund staerker ab als das p99 des Unterschieds an der Wand links (Messblatt F0-Fenster
# x 40-119, y 150-299, dort ist keine Figur) - gemessen in quelle_maske(), Wert im Protokoll ('figur_schwelle')
# Bildpunkte mit lambda < LAM_MIN zaehlen nicht (streifend an der Kontur): derselbe Wert wie fuer den Koerper (r2_koerper_farben.LAM_MIN 0,35;
# Median-lambda der Schwanzbildpunkte 0,96, Protokoll 'lambda_median')
LAM_MIN = RK.LAM_MIN
QUER_MAX = float(os.environ.get('RE15_R5_QUER', '1.0'))   # s. zuordnung; Laeufe r5 Schwanz q0.5 / q0.7 / q1.0 am Endstand: rot 5 / 5 / 4 (T8 F17 7,5 / 6,4 / gruen)
ZUORDNUNG = os.environ.get('RE15_R5_GLEICHE_STELLE') != '1'    # Schritt 2 nach Anteil der Achse (s. zuordnung), sonst dieselbe Bildstelle


def quelle_maske(o2):
    """Bildpunkte des Originals, deren Farbe als Schwanzfarbe gilt (s. Kopf, Schritt 2)."""
    m = R4.masken_original()[0]
    bg = G.lab(G.videoraum(G.hintergrund()))
    lo = G.lab(o2)
    de = np.linalg.norm(lo - bg, axis=2)
    schwelle = float(np.percentile(de[150:300, 40:120], 99))
    fig = de > schwelle
    xx = np.arange(640)[None, :]; yy = np.arange(480)[:, None]
    q = nd.binary_erosion(m, iterations=2) | (nd.binary_erosion(fig, iterations=2) & (xx >= X_RUECKEN) & (yy < 262))
    return q, schwelle


def naechster(o2, q):
    """Je Bildpunkt die Originalfarbe (Videoraum) an der Stelle bzw. am naechsten Quellpunkt."""
    _d, (iy, ix) = nd.distance_transform_edt(~q, return_indices=True)
    return o2[iy, ix]


def texel_loesen(mo, pose, D, uvd):
    _img, ids, U, Vt, Z, LAM = RK.raster(mo, pose)
    pb = 128 * (uvd['page'] & 0xF) + uvd['u'][0]
    sel = (Z == (uvd['clut'] >> 6)) & np.isin(ids, G.SCHWANZ) & (LAM.mean(2) >= LAM_MIN)
    num = np.zeros((256, 64, 3)); den = np.zeros((256, 64, 3))
    uu = np.clip(U[sel] - pb, 0, 63); vv = np.clip(Vt[sel], 0, 255)
    lam = LAM[sel].astype(float)
    np.add.at(num, (vv, uu), lam * D[sel]); np.add.at(den, (vv, uu), lam * lam)
    ges = den[..., 0] > 1e-6
    T = num / np.maximum(den, 1e-9)
    _d, (iy, ix) = nd.distance_transform_edt(~ges, return_indices=True)
    T = T[iy, ix]
    lam_med = float(np.median(LAM[np.isin(ids, G.SCHWANZ)].mean(1)))
    return np.clip(T, 0, 248), ges, lam_med


def quantisieren(T, k=255, saat=41):
    X = T.reshape(-1, 3)
    rng = np.random.default_rng(saat)
    C = X[rng.choice(len(X), k, replace=False)].copy()
    for _ in range(30):
        d = ((X[:, None, :] - C[None]) ** 2).sum(2); lb = d.argmin(1)
        for j in range(k):
            mj = lb == j
            if mj.any():
                C[j] = X[mj].mean(0)
    idx = (1 + lb).reshape(T.shape[:2]).astype(np.uint8)
    clut = np.zeros((256, 3)); clut[1:] = C
    return idx, clut


def bauen(npz):
    BF.V4_SCHWANZ = npz
    BF.BAU_INFO.clear()
    return BF.baue(BF.lade_em30(), BF._variante_v4())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--aus', default=AUS)
    ap.add_argument('--runden', type=int, default=3)
    a = ap.parse_args()
    P = G.posen()['85']
    uvd = BF.SCHWANZ_V4['uv']
    o2 = G.vorlage(2)
    q, schw = quelle_maske(o2)
    Vo = naechster(o2, q)                           # Ziel im Videoraum je Bildpunkt (Schritt 2: dieselbe Bildstelle)
    if ZUORDNUNG:                                   # Runde 5 Lauf r5 Schwanz c: Gegenstelle nach Anteil der Achse und Lage quer
        import riegel_v4 as RV
        mo0 = b2.modell(bauen(os.path.join(G.SFV4, 'r5', 'nicht_da.npz')))
        img0, ids0, iv0 = G.zeichne(mo0, P, voll=True)
        ok0, so0, H0 = G.h_port(iv0)
        sm = RV.schwanz_mass_h(img0, ids0, ok0, so0)
        gx, gy, zinfo = zuordnung(ids0, H0, sm)
        _d, (iy, ix) = nd.distance_transform_edt(~q, return_indices=True)
        t_ = gx >= 0
        xi = np.clip(np.round(gx[t_]).astype(int), 0, 639); yi = np.clip(np.round(gy[t_]).astype(int), 0, 479)
        Vo[t_] = o2[iy[yi, xi], ix[yi, xi]]
    tmp = os.path.join(G.SFV4, 'r5', 'schwanz_tmp_%d.npz' % os.getpid())
    os.makedirs(os.path.dirname(tmp), exist_ok=True)
    prot = dict(figur_schwelle=schw, x_ruecken=X_RUECKEN, lam_min=LAM_MIN, zuordnung=zinfo if ZUORDNUNG else None, runden=[])
    D = G.port_ziel(Vo)
    for r in range(a.runden + 1):
        mo = b2.modell(bauen(tmp))
        T, ges, lam_med = texel_loesen(mo, P, D, uvd)
        idx, clut = quantisieren(T)
        np.savez(tmp, schwanz=idx, clut487=clut)
        mo = b2.modell(bauen(tmp))
        img, ids = G.zeichne(mo, P)
        V = G.videoraum(img)
        sw = nd.binary_erosion(np.isin(ids, G.SCHWANZ), iterations=2)
        dE = np.linalg.norm(G.lab(V[sw]) - G.lab(Vo[sw]), axis=1)
        prot['runden'].append(dict(runde=r, gesehen=int(ges.sum()), lambda_median=lam_med, dE76_mittel=float(dE.mean()),
                                   dE76_p90=float(np.percentile(dE, 90))))
        print(prot['runden'][-1], flush=True)
        if r < a.runden:              # Nachfuehrung: Abweichung Videoraum (Render) -> Original je Bildpunkt auf das Ziel aufschlagen
            D = D + np.where(np.isin(ids, G.SCHWANZ)[..., None], (Vo - V) / G.EB_, 0.0)
    np.savez(a.aus, schwanz=idx, clut487=clut)
    os.remove(tmp)
    json.dump(prot, open(os.path.splitext(a.aus)[0] + '_protokoll.json', 'w'), indent=1)
    print('geschrieben', a.aus)



# ---------------------------------------------------------------------------------------------------- Zuordnung entlang der Achse
# V4 Runde 5, Lauf r5 Schwanz b: mit "derselben Bildstelle" (Schritt 2) lag die Port-Achse im waagerechten Teil 15 px ueber der Original-
# Achse (Port-Gelenke nach TA/T2, nicht nach der Kontur) - die Port-Mitte bekam die helle Oberkante des Originals (Station Mitte L* 21,1
# gegen 10,2), die Wurzel den Glanz des Buckels (Quernaht 19,0 dE76 bei x 372). Deshalb gilt als Gegenstueck eines Port-Bildpunkts die Stelle
# des Originals mit demselben ANTEIL der Achse (Bogenlaenge zwischen denselben Punkten Beginn - Biegung - Knick - Ende, linear dazwischen)
# und derselben LAGE QUER zur Achse (Abstand zur Mittellinie / halbe Breite dort). Mittellinien: t2_mass_v4.linie (Verfahren O2); das
# Original endet dort bei y ~294 (die Leser-Maske A ist am Knick y 296-303 unterbrochen), darunter die Zeilenmitten der Maske (x 270-305).
def _normale(P):
    t = np.gradient(P, axis=0)
    t = t / np.maximum(np.linalg.norm(t, axis=1, keepdims=True), 1e-9)
    return np.stack([-t[:, 1], t[:, 0]], 1)


def _halbbreite(maske, P, N):
    w = np.zeros(len(P))
    for j in range(len(P)):
        for sg in (1, -1):
            for st in np.arange(0.5, 60, 0.5):
                x, y = P[j] + sg * st * N[j]
                xi, yi = int(round(x)), int(round(y))
                if not (0 <= xi < 640 and 0 <= yi < 480) or not maske[yi, xi]:
                    break
                w[j] += 0.25
    k = np.ones(5) / 5
    return np.maximum(np.convolve(np.pad(w, 2, mode='edge'), k, mode='valid'), 1.0)


def achse_original():
    import r5_mass as M5
    t = R4.masken_original()[0]
    P, _s, mitte = M5.mittellinie_original(t)
    return P, t, [(357, 243), tuple(mitte), (290, 308), (295, 343)]


def zuordnung(ids_v, H, s_mass):
    """Je Schwanz-Bildpunkt des Ports (Pose 85) der Gegenpunkt im Original (x, y) - s. Kopf dieses Abschnitts."""
    import t2_mass_v4 as T2M
    m_p = np.isin(ids_v, G.SCHWANZ)
    r = T2M.port(ids_v, H)
    Pp = np.asarray(r['P'], float)
    Pp = np.array([Pp[max(0, j - 2):j + 3].mean(0) for j in range(len(Pp))])
    Po, m_o, lm_o = achse_original()
    lm_p = [tuple(s_mass['beginn']), tuple(r['mitte']), tuple(s_mass['knick']), tuple(s_mass['ende'])]
    sp = np.r_[0, np.cumsum(np.linalg.norm(np.diff(Pp, axis=0), axis=1))]
    so = np.r_[0, np.cumsum(np.linalg.norm(np.diff(Po, axis=0), axis=1))]
    kp = [sp[int(np.argmin(np.linalg.norm(Pp - np.asarray(q, float), axis=1)))] for q in lm_p]
    ko = [so[int(np.argmin(np.linalg.norm(Po - np.asarray(q, float), axis=1)))] for q in lm_o]
    Np, No = _normale(Pp), _normale(Po)
    wp, wo = _halbbreite(m_p, Pp, Np), _halbbreite(m_o, Po, No)
    yy, xx = np.nonzero(m_p)
    pts = np.stack([xx, yy], 1).astype(float)
    from scipy.spatial import cKDTree
    _d, j = cKDTree(Pp).query(pts)
    sig = sp[j]
    # Anteil der Achse: stueckweise linear zwischen den Punkten, vor dem Beginn / nach dem Ende mit dem Massstab des Nachbarabschnitts
    sig_o = np.interp(sig, kp, ko)
    vor = sig < kp[0]; nach = sig > kp[-1]
    sig_o[vor] = ko[0] - (kp[0] - sig[vor]) * (ko[1] - ko[0]) / max(kp[1] - kp[0], 1e-6)
    sig_o[nach] = ko[-1] + (sig[nach] - kp[-1]) * (ko[-1] - ko[-2]) / max(kp[-1] - kp[-2], 1e-6)
    sig_o = np.clip(sig_o, 0, so[-1])
    e = ((pts - Pp[j]) * Np[j]).sum(1) / wp[j]
    io = np.clip(np.searchsorted(so, sig_o), 0, len(Po) - 1)
    # quer hoechstens QUER_MAX der halben Breite (Stand 1,0 = ganze Breite): am Rand mischt die Videounschaerfe den Hintergrund ein (Lauf r5 Schwanz c:
    # helle Streifen im senkrechten Teil, Station senkrecht L* 17,4 gegen 7,8)
    q = Po[io] + np.clip(e, -QUER_MAX, QUER_MAX)[:, None] * wo[io][:, None] * No[io]
    gx = np.full(ids_v.shape, -1.0); gy = np.full(ids_v.shape, -1.0)
    gx[yy, xx] = q[:, 0]; gy[yy, xx] = q[:, 1]
    return gx, gy, dict(landmarken_port=lm_p, landmarken_original=lm_o, bogen_port=[round(float(v), 1) for v in kp],
                        bogen_original=[round(float(v), 1) for v in ko])


if __name__ == '__main__':
    main()
