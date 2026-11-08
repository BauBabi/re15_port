"""Riegel V3 R28-R32 (Messblatt V3 Abschnitt 8): Koerper unveraendert gegen Referenz 2, Dreiecke, Texeldichte, Farbtiefe/Palette.
Aufruf: python machart_v3.py <EMD> [Referenz-EMD]  (Standard-Referenz: Spiel-Asset EM30_HANGAR.EMD = Referenz 2, sha256 cfd1ac42...)
Texeldichte je Mesh = sqrt(Texelflaeche / Modellflaeche) (Summe ueber alle Dreiecke, Viereck = 2 Dreiecke; Texelflaeche im
UV-Raum, Modellflaeche im Mesh-Raum ohne Pose)."""
import os, sys, json, hashlib
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(os.path.dirname(HIER)))
import emd_ansichtsblatt as EA
import birkin_formen as BF
from P_farbe import lab
BAUM = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(HIER))))
REF2 = os.path.join(BAUM, 're15_port', 'shared_assets', 'PSX', 'EMD', 'EM30_HANGAR.EMD')
SCHW = range(16, 23)


def dreiecke(m):
    return len(m.tf) + 2 * len(m.qf)


def _tris(m):
    """(Modell-Ecken 3x3, UV-Ecken 3x2, clut, page) je Dreieck; Viereck = (0,1,2) + (1,3,2) wie EA.geometrie."""
    for f, uv in zip(m.tf, m.tuv):
        P = np.array([m.tv[f[2 * k + 1]][:3] for k in range(3)], float)
        U = np.array([(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7])], float)
        yield P, U, uv[2], uv[5]
    for f, uv in zip(m.qf, m.quv):
        P4 = np.array([m.qv[f[2 * k + 1]][:3] for k in range(4)], float)
        U4 = np.array([(uv[0], uv[1]), (uv[3], uv[4]), (uv[6], uv[7]), (uv[9], uv[10])], float)
        for a, b, c in ((0, 1, 2), (1, 3, 2)):
            yield P4[[a, b, c]], U4[[a, b, c]], uv[2], uv[5]


def dichte(m):
    tf = mf = 0.0
    for P, U, _c, _p in _tris(m):
        mf += 0.5 * np.linalg.norm(np.cross(P[1] - P[0], P[2] - P[0]))
        tf += 0.5 * abs((U[1, 0] - U[0, 0]) * (U[2, 1] - U[0, 1]) - (U[2, 0] - U[0, 0]) * (U[1, 1] - U[0, 1]))
    return float(np.sqrt(tf / mf)) if mf else 0.0


def mesh_bytes(m):
    return json.dumps([m.tv, m.tn, m.tf, m.tuv, m.qv, m.qn, m.qf, m.quv]).encode()


def pruefe(pfad, ref=REF2):
    blob = open(pfad, 'rb').read(); rb = open(ref, 'rb').read()
    mo, mr = BF.modell_aus_blob(blob), BF.modell_aus_blob(rb)
    d, dr = EA.emd_dir(blob), EA.emd_dir(rb)
    erg = dict(datei=os.path.basename(pfad), sha256=hashlib.sha256(blob).hexdigest()[:16], ref_sha256=hashlib.sha256(rb).hexdigest()[:16])
    # R28: Meshes 0-15 (ausser Mesh 9 UV/CLUT und neuen Vierecken auf Mesh 8), Skelett 0-15, EDD
    abw = []
    for b in range(16):
        a, r = mo.meshes[b], mr.meshes[b]
        if b == 9:
            gleich = (a.tv, a.tn, a.tf, a.qv, a.qn, a.qf) == (r.tv, r.tn, r.tf, r.qv, r.qn, r.qf)
        elif b == 8:
            nq = len(a.qf) - len(r.qf)
            # EM30 Mesh 8 teilt die Eckenliste zwischen Drei- und Vierecken (tv == qv): das Schulterauge haengt 4 Ecken an beide an
            nv = len(a.qv) - len(r.qv)
            gleich = (a.tf, a.tuv) == (r.tf, r.tuv) and 0 <= nq <= 1 and 0 <= nv <= 4 * nq and \
                a.tv[:len(r.tv)] == r.tv and a.tn[:len(r.tn)] == r.tn and \
                a.qf[:len(r.qf)] == r.qf and a.quv[:len(r.quv)] == r.quv and a.qv[:len(r.qv)] == r.qv and a.qn[:len(r.qn)] == r.qn
            erg['mesh8_neue_vierecke'] = nq
        else:
            gleich = mesh_bytes(a) == mesh_bytes(r)
        if not gleich:
            abw.append(b)
    sk, skr = mo.skel, mr.skel
    skel_gleich = all(tuple(sk.rel[b]) == tuple(skr.rel[b]) and sk.parent[b] == skr.parent[b] for b in range(16))
    kf_gleich = sk.kf_count == skr.kf_count and all(
        all(tuple(EA.kf_angles(sk, k, b)) == tuple(EA.kf_angles(skr, k, b)) for b in range(16)) and
        tuple(EA.kf_pos(sk, k)) == tuple(EA.kf_pos(skr, k)) for k in range(sk.kf_count))
    edd_gleich = blob[d[1]:d[2]] == rb[dr[1]:dr[2]]
    erg['R28'] = dict(meshes_abweichend=abw, skelett_0_15_gleich=bool(skel_gleich), keyframes_0_15_gleich=bool(kf_gleich),
                      edd_gleich=bool(edd_gleich), ok=bool(not abw and skel_gleich and kf_gleich and edd_gleich))
    # R29/R30
    ds = sum(dreiecke(mo.meshes[b]) for b in SCHW if b < len(mo.meshes))
    erg['R29'] = dict(dreiecke_schwanz=ds, je_glied=[dreiecke(mo.meshes[b]) for b in SCHW], ok=ds <= 212)
    neu8 = 2 * erg.get('mesh8_neue_vierecke', 0)
    erg['R30'] = dict(dreiecke_kopf=dreiecke(mo.meshes[9]), dreiecke_schulterauge=neu8,
                      ok=dreiecke(mo.meshes[9]) <= 118 and neu8 <= 2)
    # R31 Texeldichte
    dk = [round(dichte(mo.meshes[b]), 3) for b in SCHW]
    dh = {b: round(dichte(mo.meshes[b]), 3) for b in (7, 8, 10, 13)}
    erg['R31'] = dict(schwanz=dk, kopf=round(dichte(mo.meshes[9]), 3), haut_ref=dh,
                      ok=all(0.045 <= x <= 0.090 for x in dk) and 0.05 <= dichte(mo.meshes[9]) <= 0.091)
    # R32 Farbtiefe / Palette: 8-bpp-TIM, CLUT-Zeilen der Schwanz-UVs = Hautzeilen 480-482, max C* der Schwanztexel <= max C* der Hauttexel
    tim = blob[d[8]:]
    flags = EA.u32(tim, 4)
    zeilen = sorted({(uv[2] >> 6) for b in SCHW for _P, _U, uvc, _pg in _tris(mo.meshes[b]) for uv in [(0, 0, uvc)]})
    def texel_C(meshes_b):
        cs = []
        for b in meshes_b:
            for _P, U, clut, page in _tris(mo.meshes[b]):
                u0, u1 = int(U[:, 0].min()), int(U[:, 0].max()); v0, v1 = int(U[:, 1].min()), int(U[:, 1].max())
                t = mo.tex[v0:v1 + 1, 128 * (page & 0xF) + u0:128 * (page & 0xF) + u1 + 1]
                rgb = mo.pal[mo.clut_row(clut)][t].reshape(-1, 3).astype(float)
                A = lab(rgb); cs.append(np.hypot(A[:, 1], A[:, 2]).max())
        return float(max(cs)) if cs else 0.0
    cs, chh = texel_C(SCHW), texel_C((7, 8, 10, 13))
    erg['R32'] = dict(bpp8=(flags & 7) == 1, schwanz_clut_zeilen=zeilen, C_max_schwanz=round(cs, 1), C_max_haut=round(chh, 1),
                      ok=(flags & 7) == 1 and set(zeilen) <= {480, 481, 482} and cs <= chh)
    return erg


if __name__ == '__main__':
    e = pruefe(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else REF2)
    print(json.dumps(e, indent=1))
