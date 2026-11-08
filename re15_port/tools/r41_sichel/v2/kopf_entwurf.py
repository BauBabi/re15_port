"""Spur SF Schwanz V2, Kopf (Nutzer-Entscheid F4 2026-11-06, PORT-WAHL, Quelle KI-Fassung): Masszahlen fuer FORMEN-Variante.

1. Texeldichte der Haut (Texel je Einheit, Wurzel aus Texelflaeche / Oberflaeche) je Mesh des Modells -> Groesse der Kopfflaeche
   im TIM, damit der Kopf dieselbe Dichte hat wie der Rumpf (Vorgabe "kein hoeher aufgeloestes Einzelteil").
2. Abbildung Kopfraum (Mesh 9) -> Kopfbild (u, v) als Parallelprojektion aus der Vorderansicht h118 (Pose anpassen_v2.py),
   Blickrichtung im Kopfraum.
3. Schulterauge: KI birkin19 (1280er (705-715, 360-372)) auf die Schulter (Mesh 8) zurueckgerechnet, vier Ecken im Brustraum.
4. KI-Masse des Kopfs (Anteile der Kopfbox: Auge, Maul) aus birkin19 (Kopfbox Leser A: x 755-820, y 318-440).
python kopf_entwurf.py <emd> <posen_vorn.json>
"""
import sys, os, json
import numpy as np
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2
import birkin_buehne as BB
import emd_ansichtsblatt as EA
import treffer as TR


def texeldichte(mo, b):
    m = mo.meshes[b]
    fl_t, fl_w = 0.0, 0.0
    for verts, faces, uvs in ((m.tv, m.tf, m.tuv), (m.qv, m.qf, m.quv)):
        for f, uv in zip(faces, uvs):
            ie = f[1::2]
            P = [np.array(verts[i][:3], float) for i in ie]
            T = [np.array([uv[3 * k], uv[3 * k + 1]], float) for k in range(len(ie))]
            tris = [(0, 1, 2)] if len(ie) == 3 else [(0, 1, 3), (0, 3, 2)]
            for a, bb, c in tris:
                fl_w += 0.5 * np.linalg.norm(np.cross(P[bb] - P[a], P[c] - P[a]))
                d1, d2 = T[bb] - T[a], T[c] - T[a]
                fl_t += 0.5 * abs(d1[0] * d2[1] - d1[1] * d2[0])
    return np.sqrt(fl_t / fl_w) if fl_w else 0.0


def bildlage(mo, pose, mesh):
    """Mesh-lokale Vertices und ihre Videolage (640x480) in der Pose."""
    r, kam, M, li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    R, T = mo.pose(kf)
    m = mo.meshes[mesh]
    L = np.array([v[:3] for v in m.tv], float)
    Rb, Tb = R[mesh].astype(float) / 4096.0, T[mesh].astype(float)
    P = b2.projiziere(pose, L @ Rb.T + Tb)
    return L, P, Rb, Tb


def main():
    emd, posen = sys.argv[1], sys.argv[2]
    mo = b2.modell(emd)
    pose = json.load(open(posen))['118']
    print('1. Texeldichte (Texel je Einheit):', ', '.join('%d: %.3f' % (b, texeldichte(mo, b)) for b in range(16)))
    L, P, Rb, Tb = bildlage(mo, pose, 9)
    x0, x1, y0, y1 = P[:, 0].min(), P[:, 0].max(), P[:, 1].min(), P[:, 1].max()
    print('2. Kopf im Bild h118 (640er): x %.1f-%.1f y %.1f-%.1f' % (x0, x1, y0, y1))
    # Blickrichtung im Kopfraum
    r, kam, M, li = b2.raum()
    g = int(pose['gier'])
    s, c = EA.sin_q12(g) / 4096.0, EA.cos_q12(g) / 4096.0
    Y = np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
    mitte_w = (L.mean(0) @ Rb.T + Tb) @ Y.T + np.array([pose['x'], 0, pose['z']], float)
    d_w = mitte_w - np.array(kam.pos, float)
    d_k = Rb.T @ (Y.T @ (d_w / np.linalg.norm(d_w)))
    print('   Blick im Kopfraum', np.round(d_k, 3).tolist())
    np.save(os.path.join(os.path.dirname(posen), 'kopf_bild.npy'), np.c_[L, P])
    # 3. Schulterauge
    t = TR.treffer(mo, pose, 8, 710 / 2.0, 366 / 2.0)
    z, p, n = t
    # Ecken: Bildversatz +-5,5 / +-6,5 Videopunkte (1280er 11 x 13) in der Treffertiefe -> Brustraum, auf die Tangentialebene
    L8, P8, R8, T8 = bildlage(mo, pose, 8)
    # lineare Naeherung Bild <- lokal um p (aus den Mesh-8-Vertices nahe p)
    nah = np.argsort(np.linalg.norm(L8 - p, axis=1))[:12]
    A = np.c_[L8[nah], np.ones(len(nah))]
    J, *_ = np.linalg.lstsq(A, P8[nah], rcond=None)       # 4x2: lokal -> Bild
    Jl = J[:3].T                                         # 2x3
    # Tangentialbasis: Bildrichtungen auf die Ebene senkrecht zu n
    Bt = np.eye(3) - np.outer(n, n)
    G = Jl @ Bt                                          # Bildversatz je lokale Tangentialrichtung
    inv = np.linalg.pinv(G)                              # 3x2: Bildversatz -> lokaler Tangentialversatz
    ecken = [p + inv @ np.array([sx * 2.75, sy * 3.25]) for sy in (-1, 1) for sx in (-1, 1)]
    print('3. Schulterauge Mesh 8: Mitte', np.round(p).tolist(), 'Normale', np.round(n, 2).tolist())
    print('   Ecken (ol, or, ul, ur):', [np.round(e).astype(int).tolist() for e in ecken])
    print('   Groesse lokal: %.0f x %.0f' % (np.linalg.norm(ecken[1] - ecken[0]), np.linalg.norm(ecken[2] - ecken[0])))
    # 4. KI-Anteile
    kb = (755, 820, 318, 440)
    def rel(x, y):
        return ((x - kb[0]) / (kb[1] - kb[0]), (y - kb[2]) / (kb[3] - kb[2]))
    print('4. KI-Kopfbox 1280er x %d-%d y %d-%d (Leser A: 65 x 122, Mitte 787, Oberkante 318)' % kb)
    print('   Auge oben (782,355) ->', np.round(rel(782, 355), 3))
    print('   Maul x 762-812 y 368-415 ->', np.round(rel(762, 368), 3), np.round(rel(812, 415), 3))
    print('   Auge Durchmesser ~10 px -> Anteil der Kopfbreite %.3f' % (10 / 65.0))


if __name__ == '__main__':
    main()


def abbildung(posen_dir, u, v):
    """Affine Naeherung lokal (x, y, z, 1) -> (u, v): Kopfbox im Bild h118 auf die TIM-Flaeche u x v (kleinste Quadrate)."""
    D = np.load(os.path.join(posen_dir, 'kopf_bild.npy'))
    L, P = D[:, :3], D[:, 3:]
    x0, x1, y0, y1 = P[:, 0].min(), P[:, 0].max(), P[:, 1].min(), P[:, 1].max()
    tu = u[0] + (P[:, 0] - x0) / (x1 - x0) * (u[1] - u[0])
    tv = v[0] + (P[:, 1] - y0) / (y1 - y0) * (v[1] - v[0])
    A = np.c_[L, np.ones(len(L))]
    cu, *_ = np.linalg.lstsq(A, tu, rcond=None)
    cv, *_ = np.linalg.lstsq(A, tv, rcond=None)
    fehler = np.abs(np.c_[A @ cu - tu, A @ cv - tv]).max()
    return np.round(np.vstack([cu, cv]), 5), fehler


if len(sys.argv) > 3:
    u = tuple(int(x) for x in sys.argv[3].split(','))
    v = tuple(int(x) for x in sys.argv[4].split(','))
    A, f = abbildung(os.path.dirname(sys.argv[2]), u, v)
    print('5. Abbildung Kopfraum -> (u, v) fuer u %s v %s: %s  (groesster Fehler %.2f Texel)' % (u, v, A.tolist(), f))
