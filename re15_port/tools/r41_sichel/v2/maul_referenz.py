"""Spur SF Schwanz V2, Kopf (Nutzer-Vorgabe 2026-11-06 "in der grafischen Qualitaet des Spieles"): Dreiecke und Texel eines
Originalmauls mit Zaehnen zum Vergleich mit dem Kopf V2.

Referenz: RE2-G4 (CDEMD0.EMS kind 0x34, rec 3 EMD / rec 2 TIM, Knochen 26 = Kiefer; birkin_bahnsteig.py lade_rohdaten) - die
Birkin-Form mit Maul. Gezaehlt: Dreiecke (Viereck = 2) des Kiefer-Meshes, Texelflaeche seiner UVs, Texeldichte (Texel je
Einheit), Anteil heller Texel (Zaehne: L > 150) in der Kieferflaeche. Dazu dieselben Zahlen fuer Kopf V2 (Mesh 9 nach der
Umtexturierung, Schulterauge Mesh 8) aus einer Varianten-EMD.
python maul_referenz.py <variante.emd>
"""
import sys, os
import numpy as np
import cv2
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import b2                                      # noqa: F401  (Pfad zu tools)
import birkin_bahnsteig as BS
import emd_ansichtsblatt as EA
import kopf_entwurf as KE


def flaechen(m, filt=None):
    """(Dreiecke, Texelflaeche, Oberflaeche, Liste der UV-Dreiecke) fuer die Faces, die filt(uv) erfuellen."""
    n, ft, fw, uvs = 0, 0.0, 0.0, []
    for verts, faces, us in ((m.tv, m.tf, m.tuv), (m.qv, m.qf, m.quv)):
        for f, uv in zip(faces, us):
            if filt and not filt(uv):
                continue
            ie = f[1::2]
            P = [np.array(verts[i][:3], float) for i in ie]
            T = [np.array([uv[3 * k] + (uv[5] & 0xF) * 128, uv[3 * k + 1]], float) for k in range(len(ie))]
            for a, b, c in ([(0, 1, 2)] if len(ie) == 3 else [(0, 1, 3), (0, 3, 2)]):
                n += 1
                fw += 0.5 * np.linalg.norm(np.cross(P[b] - P[a], P[c] - P[a]))
                d1, d2 = T[b] - T[a], T[c] - T[a]
                ft += 0.5 * abs(d1[0] * d2[1] - d1[1] * d2[0])
                uvs.append((T[a], T[b], T[c], (uv[2] >> 6)))
    return n, ft, fw, uvs


def hell_anteil(mo, uvs, schwelle=150):
    m = np.zeros(mo.tex.shape, np.uint8)
    hell, alle = 0, 0
    for a, b, c, zeile in uvs:
        m[:] = 0
        cv2.fillConvexPoly(m, np.round(np.array([a, b, c])).astype(np.int32), 1)
        idx = mo.tex[m > 0]
        z = max(0, min(mo.pal.shape[0] - 1, zeile - mo.clut_y))
        L = mo.pal[z][idx].astype(float) @ np.array([0.299, 0.587, 0.114])
        hell += int((L > schwelle).sum()); alle += len(L)
    return hell, alle


def main():
    emd, tim = BS.lade_rohdaten()
    g4 = BS.modell(emd, tim)
    n, ft, fw, uvs = flaechen(g4.meshes[26])
    h, a = hell_anteil(g4, uvs)
    print('RE2-G4 Kiefer (Mesh 26): %d Dreiecke, Texelflaeche %.0f, Dichte %.3f Texel/E., helle Texel (L>150) %d von %d (%.0f %%)' % (
        n, ft, np.sqrt(ft / fw), h, a, 100.0 * h / max(a, 1)))
    n10, ft10, fw10, _u = flaechen(g4.meshes[10])
    print('RE2-G4 Rumpf/Kopf (Mesh 10): %d Dreiecke, Dichte %.3f Texel/E.' % (n10, np.sqrt(ft10 / fw10)))
    mo = b2.modell(sys.argv[1])
    kopf = lambda uv: (uv[2] >> 6) == 486
    n9, ft9, fw9, uvs9 = flaechen(mo.meshes[9], kopf)
    n9a, _f, _w, _u = flaechen(mo.meshes[9])
    h9, a9 = hell_anteil(mo, uvs9)
    print('Kopf V2 (Mesh 9): %d von %d Dreiecken mit dem Kopfbild, Texelflaeche %.0f, Dichte %.3f Texel/E., helle Texel %d von %d (%.0f %%)' % (
        n9, n9a, ft9, np.sqrt(ft9 / fw9), h9, a9, 100.0 * h9 / max(a9, 1)))
    n8, ft8, fw8, _u = flaechen(mo.meshes[8], kopf)
    print('Schulterauge (Mesh 8): %d Dreiecke, Texelflaeche %.0f, Dichte %.3f Texel/E.' % (n8, ft8, np.sqrt(ft8 / max(fw8, 1))))
    print('Haut zum Vergleich (Mesh 8 ohne Auge): Dichte %.3f, Kopf EM30 (Referenz 2, Mesh 9): %.3f Texel/E.' % (
        KE.texeldichte(mo, 8), 0.091))


if __name__ == '__main__':
    main()
