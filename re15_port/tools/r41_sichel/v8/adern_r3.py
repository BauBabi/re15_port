"""SF V8 Runde 3 (O1 + O2): (W)-rechte Flaechen wie am Original-RE1.5-Birkin und roter Huegel aus Original-Fleisch -> adern_v8.json.

    python adern_r3.py [--kette JSON] [--ruecken JSON] [--aus JSON]          (Suche; Bau: adern_v8.adern mit modus "original")

Nutzer 2026-11-08 (woertlich): "man sieht ebenfalls beim vorlage Bild, das die Rechte Seite auch mehr rottöne hat, also quasi mehr adern an
verschiedenen Stellen, am Arm und Körper, nicht nur grau. Übernehme die die Textur Sachen die du dafür brauchst vom originalen Birkin von
Resident Evil 1.5"; Skizze Darstellung.png "Little red hill from the back".
Orchestrator Runde 3 (nach Pruefung V8 r2: S7c 5,3 % statt 25-80 %, rotes Band am Unterarm, Huegel 0,0 % rot gegen 58,3 % der Vorlage):
  O1 Fuer die (W)-rechten Flaechen von Rumpf (Mesh 8), Oberarm (13) und Unterarm (14) gilt das ORIGINAL-EM30 als Vorbild: dessen Texel UND
     dessen Original-Paletteneintraege der Zeilen 480-482 (nicht nur die Ader-Eintraege). Ausgeschlossen: Feld C Schulter-Augapfel (Nutzer
     V5 "Auch der rote Fleck oben auf der rechten Schulter, den gibt es in der Vorlage ganz offensichtlich nicht") und die Brust (Nutzer V5
     "Die roten Stellen sind korrekt auf der brust und am oberen linken Arm"; V7-Zeile 488 bleibt).
  O2 Die Huegel-Flaechen bekommen rotes Fleisch aus dem Original-EM30 (ganze Flaechen, Original-Texel und -Eintraege, nicht Feld C).
Regel (Memory "Texturen nie aus Videobildern eichen", Nachtrag 2026-11-08): keine gemalten Inseln, keine Eichung am Video; das Video liefert
nur Orte. Texel bleiben bytegleich V7 (= Original-Texel unter allen Flaechen der Meshes 8/13/14, gemessen sfv8/r3 e2.py: 0 abweichende).
Zeilen: Xk = Original-EM30-Zeile 480 + k bytegleich (k = 0, 1, 2), angehaengt als 493/494/495 (V7 13 Zeilen + 3 = 16, Port-Texturhoehe 4096).
Flaechen (Seite (W)-rechts = Knochenraum 8 z < 0, v8_gemein Kopf):
  Mesh 8:  V7-Zeile 480/481, Flaechenschwerpunkt z < 0, nicht Brust (V7-Zeile 488 bleibt) -> X(Zeile - 480)
  Mesh 13: V7-Zeile 480/489 (beide Original-Zeile 480), UV nicht in Feld C (Seite 0, u 78-127, v 0-100) -> X0; Feld-C-Flaechen bleiben V7
  Mesh 14: alle (V7-Zeile 482) -> X2 (das V8-r2-Band aus 88 Ader-Eintraegen auf grauer Haut entfaellt; Original: blassrosa)
  Huegel:  Flaechen von Mesh 8 (nicht Brust), die in PV >= HUEGEL_MIN_PX der obersten 8 px der Figur in x 428-445 bilden (Zahlen-Pruefer r2:
           Messfenster "oberste 8 px x 428-445"); je Flaeche ein ganzzahliger UV-Schub auf Seite 0, der die Flaeche ganz auf Original-Texel
           legt, die mit Original-Zeile 480 rotes Fleisch sind (a* >= FLEISCH_A, Oeffnung 5 x 5 wie Messblatt 6c "Flaechen-Rot"), nicht in
           Feld C, nicht auf Kopftexeln (Mesh 9), ohne durchsichtigen Texel (Eintrag 0x0000; sonst Loecher im Huegel,
           gemessen sfv8/r3: Kontur x 433/434 3 px tiefer als der Umriss) -> Zeile X0. Wahl: groesster Fleisch-Anteil, dann kleinster Schub.
Kein RE-Wert; PORT-WAHL.
"""
import os, sys, json, argparse
import numpy as np
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402
import f_rast as FR                   # noqa: E402
import v4_gemein as G                 # noqa: E402

FELD_C = (0, 78, 127, 0, 100)         # Seite, u0, u1, v0, v1 (Messblatt V8 6c Tabelle "C Schulter-Augapfel u 78-127 v 0-100")
FLEISCH_A = 20.0                      # Paletten-a* des Original-Fleisches (Messblatt V8 6c: Flaechen-Rot = rot, bleibt unter der Oeffnung 5 x 5)
HUEGEL_MIN_PX = 5
BRUST = 488                           # V7-Zeile der Brust (V5 wie Foto b), bleibt


def uv_ecken(uv):
    n = 3 if len(uv) == 9 else 4
    return [(uv[3 * k], uv[3 * k + 1]) for k in range(n)]


def in_feld_c(uv):
    if (uv[5] & 0xF) != FELD_C[0]:
        return False
    us = [u for u, _v in uv_ecken(uv)]; vs = [v for _u, v in uv_ecken(uv)]
    return min(us) <= FELD_C[2] and max(us) >= FELD_C[1] and min(vs) <= FELD_C[4] and max(vs) >= FELD_C[3]


def fleisch_karte():
    """Original-EM30: Texel, die mit ihrer Seitenzeile (Seite k -> Zeile 480 + k) rotes Fleisch zeigen (a* >= FLEISCH_A, Oeffnung 5 x 5)."""
    _o, _ms, to = AV.original()
    pix = to['pix']
    a = np.zeros(pix.shape)
    for pg in range(3):
        L = AV.lab15(to['clut'][pg])
        a[:, pg * 128:(pg + 1) * 128] = L[pix[:, pg * 128:(pg + 1) * 128]][..., 1]
    return ndimage.binary_opening(a >= FLEISCH_A, np.ones((5, 5)))


def flaechen_wahl(ms):
    out = {}
    m = ms[8]
    P = np.array([v[:3] for v in m['v']], float)
    for key, uv, f in [('d%d' % i, u, m['tf'][i]) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u, m['qf'][i]) for i, u in enumerate(m['quv'])]:
        z = (uv[2] >> 6) & 0x1FF
        if z in (480, 481) and float(np.mean([P[i][2] for i in f[1::2]])) < 0:
            out.setdefault('8', []).append(key)
    m = ms[13]
    for key, uv in [('d%d' % i, u) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u) for i, u in enumerate(m['quv'])]:
        z = (uv[2] >> 6) & 0x1FF
        if z in (480, 489) and not in_feld_c(uv):
            out.setdefault('13', []).append(key)
    m = ms[14]
    for key, uv in [('d%d' % i, u) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u) for i, u in enumerate(m['quv'])]:
        if (uv[2] >> 6) & 0x1FF == 482:
            out.setdefault('14', []).append(key)
    return out


def huegel_wahl(blob, ms):
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV)
    bone, fl = A['bone'][:480], A['flaeche'][:480]
    zahl = {}
    for x in range(428, 446):
        ys = np.nonzero(bone[:, x] >= 0)[0]
        for y in range(ys.min(), ys.min() + 8):
            if bone[y, x] == 8:
                zahl[fl[y, x]] = zahl.get(fl[y, x], 0) + 1
    fk = fleisch_karte()
    _o, _ms, to = AV.original()
    leer = to['clut'][0][to['pix']] == 0                             # Eintrag 0x0000 = durchsichtig (PSX), Zeile X0 = Original-Zeile 480
    leer[:, 128:] = True                                             # nur Seite 0
    kopf = KB.texel_nutzung([ms[9]])[0]
    sperre = kopf.copy(); sperre[FELD_C[3]:FELD_C[4] + 1, FELD_C[1]:FELD_C[2] + 1] = True
    wahl = []
    for key, n in sorted(zahl.items(), key=lambda kv: (-kv[1], kv[0])):
        uv = AV.flaeche(ms[8], key)
        if n < HUEGEL_MIN_PX or (uv[2] >> 6) & 0x1FF == BRUST or (uv[5] & 0xF) != 0:
            continue
        us = [u for u, _v in uv_ecken(uv)]; vs = [v for _u, v in uv_ecken(uv)]
        best = None
        for du in range(-min(us), 128 - max(us)):
            for dv in range(-min(vs), 256 - max(vs)):
                q = list(uv)
                for k in range(len(uv_ecken(uv))):
                    q[3 * k] += du; q[3 * k + 1] += dv
                mk = KB.raster(q, len(uv_ecken(uv)), 0)
                if (mk & sperre).any() or (mk & leer).any():
                    continue
                anteil = float(fk[mk].mean())
                sc = (round(anteil, 4), -(du * du + dv * dv))
                if best is None or sc > best[0]:
                    best = (sc, du, dv)
        wahl.append(dict(flaeche=key, pv_px=n, schub=[best[1], best[2]], fleisch_anteil=best[0][0]))
    return wahl


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--kette', default=os.path.join(HIER, 'kette_v8.json'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--aus', default=os.path.join(HIER, 'adern_v8.json'))
    a = ap.parse_args()
    import v8_bau as VB
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    blob, _p = VB.bauen(a.basis, 'sre', kette=json.load(open(a.kette)), ruecken=json.load(open(a.ruecken)))
    _d, _o, ms, _t = KB.teile(blob)
    fw = flaechen_wahl(ms)
    hw = huegel_wahl(blob, ms)
    for h in hw:
        print('Huegel', h)
    hk = set(h['flaeche'] for h in hw)
    fw['8'] = [k for k in fw.get('8', []) if k not in hk]
    res = dict(quelle='adern_r3.py Runde 3 (Basis V7 %s, Kette %s, Ruecken %s): volle Original-EM30-Zeilen 480-482 auf die (W)-rechten Flaechen, '
                      'Huegel auf Original-Fleisch' % (V.V7_SHA[:8], os.path.basename(a.kette), os.path.basename(a.ruecken)),
               modus='original', zeilen=[0, 1, 2], flaechen=fw, huegel=hw)
    json.dump(res, open(a.aus, 'w'), indent=1)
    print({k: len(v) for k, v in fw.items()}, 'Huegel', len(hw))


if __name__ == '__main__':
    main()
