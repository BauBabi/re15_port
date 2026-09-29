#!/usr/bin/env python3
"""tuer_rest_helligkeit.py - Helligkeit eines Port-Archivs in der Sequenz (echte exe) gegen Texel,
Modell und das GEMALTE RE1.5-Blatt an denselben Texelstellen (Runde 33, Pilot).

    python re15_port/tools/tueren/tuer_rest_helligkeit.py <serienbild.ppm> <Kennung> <Variante> <Bild>

Das Serienbild kommt aus RE15_TUER_SERIE (Rueckleser vor dem Present, 960x720, beschleunigter
Renderer). Geometrie und Bewegung = Basis-Archiv (SCD/MD1 bytegleich, probe_r33_tueren "archive"):
Objektstand von Bild <Bild> aus dem Katalog-Simulator (tuerkatalog.VM) wie tor_helligkeit.py `bild`,
Textur aus shared_assets/RE15DOOR/<Kennung>.DO2, gemaltes Feld = dasselbe RE1.5-Feld, das der Generator
benutzt (tuer_archiv_bauen.re15_blattfeld). Nur Innenpunkte der Blattdreiecke (Objekt 0).

Ausgabe: F/T (gezeigt/Texel), Modell PSX (T * c/128) gegen F, F/P (gezeigt/gemalt) - Ziel F/P = 1
("gezeigt = gemalt", Runde 32), also KEINE doppelte Abdunklung.
"""
import json
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
sys.path.insert(0, os.path.join(PORT, "tools", "tor"))
sys.path.insert(0, HIER)
import do2_format as fmt          # noqa: E402
import tor_helligkeit as th       # noqa: E402
import tuer_archiv_bauen as tab   # noqa: E402

FELDER = {"P07G": (tab.G1_AUSWAHL, False), "P1DG": (tab.G4_AUSWAHL, True)}


def main(argv):
    pfad, kennung, var, bildnr = argv[1], argv[2], int(argv[3]), int(argv[4])
    arch = {e["kennung"]: e for e in json.load(open(tab.AUS_JSON, encoding="utf-8"))["archive"]}[kennung]
    basis = "%02X" % arch["basis_nr"]
    md1, _, _ = th.archiv(basis)
    port = fmt.Do2.lesen(open(os.path.join(tab.AUS_DIR, kennung + ".DO2"), "rb").read())
    tex, ok = th.tim_rgb(fmt.Tim.lesen(port.tim)[0])
    gemalt = None
    if kennung in FELDER:
        feld, _ = tab.re15_blattfeld(FELDER[kennung][0], nur_zeilen=FELDER[kennung][1])
        gemalt = np.zeros((256, 128, 3), np.float32)
        gemalt[:tab.BLATT_V] = feld
    jetzt, vor = th.vm_saetze(basis, var, bildnr)
    tris = []
    for k, sz in sorted(jetzt.items()):
        if sz["parent"] >= 0 or sz["obj"] != 0 or sz["mesh"] >= len(md1.meshes):
            continue
        tris += th.dreiecke(md1, tex, sz, None, vor.get(k))
    R = th.rastern(tris, tex, ok, gemalt)
    F = th.ppm(pfad)
    m = R["innen"] & (th.lum(R["T"]) >= 8)
    if gemalt is not None:
        m &= th.lum(R["P"]) > 0
    psx = np.minimum(255, R["T"] * (R["c"] / 128.0)[..., None])
    res = dict(bild=os.path.basename(pfad), kennung=kennung, basis="DOOR" + basis, variante=var, door_move_bild=bildnr,
               bildpunkte=int(m.sum()), c_mittel=round(float(R["c"][m].mean()), 2),
               F_mittel=round(float(th.lum(F)[m].mean()), 2), T_mittel=round(float(th.lum(R["T"])[m].mean()), 2),
               F_durch_T=round(float(th.lum(F)[m].sum() / th.lum(R["T"])[m].sum()), 4),
               abweichung_F_modell_psx=round(float(np.abs(th.lum(F) - th.lum(psx))[m].mean()), 2))
    if gemalt is not None:
        res.update(P_mittel=round(float(th.lum(R["P"])[m].mean()), 2),
                   F_durch_P=round(float(th.lum(F)[m].sum() / th.lum(R["P"])[m].sum()), 4),
                   F_rgb=[round(float(x), 1) for x in F[m].mean(0)],
                   P_rgb=[round(float(x), 1) for x in R["P"][m].mean(0)])
    print(json.dumps(res, indent=1))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
