"""Geometrie-Messungen im Modellraum (kameraunabhaengig): Dreieck-Dreieck-Schnitt zwischen Teilen und Eindringtiefe von Ecken.
Ausrichtung der Flaechennormalen: empirisch aus den sichtbaren Dreiecken der Bild-2-Kamera bestimmt (s_schnitt.py prueft das Vorzeichen)."""
import numpy as np


def seg_tri(P0, P1, A, B, C, eps=1e-9):
    """Segmente P0-P1 (n x 3) gegen Dreiecke A,B,C (m x 3): Bool-Matrix n x m und Schnittpunkte (Moeller-Trumbore, 0<=t<=1)."""
    D = (P1 - P0)[:, None, :]
    E1 = (B - A)[None]; E2 = (C - A)[None]
    p = np.cross(D, E2)
    det = np.einsum('ijk,ijk->ij', E1.repeat(len(P0), 0), p)
    ok = np.abs(det) > eps
    inv = np.where(ok, 1.0 / np.where(ok, det, 1), 0)
    T = P0[:, None, :] - A[None]
    u = np.einsum('ijk,ijk->ij', T, p) * inv
    q = np.cross(T, E1)
    v = np.einsum('ijk,ijk->ij', D.repeat(len(A), 1), q) * inv
    t = np.einsum('ijk,ijk->ij', E2.repeat(len(P0), 0), q) * inv
    hit = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (t >= 0) & (t <= 1)
    X = P0[:, None, :] + t[..., None] * D
    return hit, X


def tri_schnitte(TA, TB):
    """TA (n x 3 x 3), TB (m x 3 x 3): Paare (i, j), die sich im Raum schneiden (eine Kante des einen durchstoesst das andere),
    und je Paar die Schnittpunkte. Vorfilter ueber Huellquader."""
    if len(TA) == 0 or len(TB) == 0:
        return []
    amin, amax = TA.min(1), TA.max(1); bmin, bmax = TB.min(1), TB.max(1)
    ov = np.all((amin[:, None] <= bmax[None]) & (bmin[None] <= amax[:, None]), axis=2)
    ia, ib = np.nonzero(ov)
    if len(ia) == 0:
        return []
    out = {}
    for (X, Y, flip) in ((TA, TB, False), (TB, TA, True)):
        I, J = (ib, ia) if flip else (ia, ib)
        for e0, e1 in ((0, 1), (1, 2), (2, 0)):
            P0 = X[I, e0]; P1 = X[I, e1]
            A, B, C = Y[J, 0], Y[J, 1], Y[J, 2]
            # paarweise (nicht kreuzweise): je Paar ein Segment gegen ein Dreieck
            D = P1 - P0; E1 = B - A; E2 = C - A
            p = np.cross(D, E2); det = np.einsum('ij,ij->i', E1, p)
            ok = np.abs(det) > 1e-9
            inv = np.where(ok, 1.0 / np.where(ok, det, 1), 0)
            T = P0 - A; u = np.einsum('ij,ij->i', T, p) * inv
            q = np.cross(T, E1); v = np.einsum('ij,ij->i', D, q) * inv; t = np.einsum('ij,ij->i', E2, q) * inv
            hit = ok & (u >= 0) & (v >= 0) & (u + v <= 1) & (t >= 0) & (t <= 1)
            for k in np.nonzero(hit)[0]:
                a, b = (J[k], I[k]) if flip else (I[k], J[k])
                out.setdefault((int(a), int(b)), []).append(P0[k] + t[k] * D[k])
    return sorted(out.items())


def naechster_punkt(Pt, A, B, C):
    """Naechster Punkt je Punkt (n) auf je Dreieck (m): Abstand^2 (n x m), Punkt (n x m x 3), innen = Lot faellt in die Flaeche (n x m)."""
    P = Pt[:, None, :]
    n = np.cross(B - A, C - A)[None]
    nn = np.maximum((n ** 2).sum(-1), 1e-12)
    d = ((P - A[None]) * n).sum(-1) / nn
    X = P - d[..., None] * n
    def kante(S, T_):
        e = (T_ - S)[None]
        tt = np.clip(((P - S[None]) * e).sum(-1) / np.maximum((e ** 2).sum(-1), 1e-12), 0, 1)
        return S[None] + e * tt[..., None]
    c0 = np.cross(B - A, X - A[None]); c1 = np.cross(C - B, X - B[None]); c2 = np.cross(A - C, X - C[None])
    innen = ((c0 * n).sum(-1) >= 0) & ((c1 * n).sum(-1) >= 0) & ((c2 * n).sum(-1) >= 0)
    best = np.where(innen[..., None], X, np.inf)
    bd = np.where(innen, ((P - X) ** 2).sum(-1), np.inf)
    for S, T_ in ((A, B), (B, C), (C, A)):
        Y = kante(S, T_)
        dY = ((P - Y) ** 2).sum(-1)
        w = dY < bd
        best = np.where(w[..., None], Y, best); bd = np.where(w, dY, bd)
    return bd, best, innen
