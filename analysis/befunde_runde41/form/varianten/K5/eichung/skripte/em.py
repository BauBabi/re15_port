"""Farbmodell Spiel -> Video je Szene (nur Messung, kein RE-Wert, gehoert nicht in die Engine).

Kette (Unschaerfe wie form_video.video_filter, gleiche Konstanten):
  L  = (e/255)^2,2                         Spielfarbe e (0..255) -> Licht
  X  = G(L, sigma 2) + KA*G(L,32) + KB*G(L,128)   Optik + Streulicht (form_video SIG_OPTIK, KA, KB; nicht neu gefittet)
  Y  = k * (C @ X)                          C = 3x3 Uebersprechen (Diagonale 1), k = Kanal-Verstaerkung je Szene
  u  = a * Y^p + b                          Kanalkurve: Verstaerkung a, Gamma p, Versatz b (V-Einheiten)
  V  = W * soft(u / W)                      weiches Ende bei Video-Weiss W = 227 (form_video)
  V  = chroma(V)                            Farbdifferenz-Unschaerfe 6 x 3 px (form_video)
Parameter je Szene: a(3) p(3) b(3) C(6) k(3, nur Szenenwechsel).
"""
import numpy as np, cv2
import ek
FV = ek.FV
W = FV.W
_UG = np.linspace(-3.0, 4.0, 14001)
_SG = FV._soft(_UG)


def soft(x):
    return FV._soft(x)


def soft_inv(y):
    return np.interp(y, _SG, _UG)


def licht(e):
    return np.power(np.clip(np.asarray(e, np.float32), 0, 255) / 255.0, 2.2).astype(np.float32)


def optik(L):
    return cv2.GaussianBlur(L, (0, 0), FV.SIG_OPTIK) + FV._glow(L).astype(np.float32)


def teile(th):
    th = np.asarray(th, float)
    a, p, b = th[0:3], th[3:6], th[6:9]
    C = np.eye(3)
    C[0, 1], C[0, 2], C[1, 0], C[1, 2], C[2, 0], C[2, 1] = th[9:15]
    k = np.exp(th[15:18]) if len(th) >= 18 else np.ones(3)
    return a, p, b, C, k


def kurve(X, th, chroma=True):
    a, p, b, C, k = teile(th)
    Y = np.maximum(X @ C.T, 1e-7) * k
    u = a * np.power(Y, p) + b
    V = W * soft(u / W)
    if chroma:
        V = np.clip(FV._chroma(V.astype(np.float32)), 0, W)
    return V


def vorwaerts(e, th, chroma=True):
    return kurve(optik(licht(e)), th, chroma)


def umkehr_X(V, th):
    """Video -> X (Licht nach Optik), ohne Chroma-Umkehr."""
    a, p, b, C, k = teile(th)
    u = W * soft_inv(np.clip(np.asarray(V, float), 0.3, W - 0.3) / W)
    Y = np.power(np.maximum((u - b) / a, 1e-9), 1.0 / p) / k
    return Y @ np.linalg.inv(C).T


def umkehr(V, th, iter_=4):
    """Video -> Spielfarbe e (Streulicht abgezogen, sigma-2-Unschaerfe bleibt). float 0..255."""
    X = umkehr_X(V, th).astype(np.float32)
    L = X.copy()
    for _ in range(iter_):
        L = np.maximum(X - FV._glow(np.maximum(L, 0)).astype(np.float32), 0)
    L = np.maximum(L, 0.25 * np.maximum(X, 0))
    return 255.0 * np.power(L, 1 / 2.2)


TH0 = np.array([300, 300, 300, .45, .45, .45, -10, -10, -10, 0, 0, 0, 0, 0, 0], float)
