#!/usr/bin/env python3
"""gesten_arme.py - Spur D (Runde 34 Nacht): welcher ARM tut in einem Gesten-Clip was?

Misst je Clip des Raum-RBJ (Record 0 = Spieler) fuer beide Armketten des PL00-Skeletts
  rechts = Knochen 9 (Schulter, rel z=-369) -> 10 (Unterarm) -> 11 (Hand = Waffenhand, Skill
           re15-weapon-render: "the weapon REPLACES the hand mesh at bone 11")
  links  = Knochen 12 (rel z=+369) -> 13 -> 14
(PL00.EMR rel-Offsets, emd_ansichtsblatt.emr_parse), jeweils im Koordinatensystem des Rumpfs
(Knochen 0), damit Koerperdrehung/Wurzelbewegung nicht mitzaehlen:
  - Handposition (x=vorn, y=unten, z=links) je Bild und ihre groesste Auslenkung gegen Bild 0,
  - die Verdrehung der Hand um die Unterarmachse (Winkel der Hand-X-Achse, projiziert auf die
    Ebene senkrecht zum Unterarm, gegen Bild 0) — "Arm um 180 Grad drehen",
  - lokale 12-Bit-Euler der Kette (4096 = 360 Grad) in Bild 0 und im Extrem.
Nur Mathematik aus der Engine (emd_ansichtsblatt: RotMatrix @0x80068130, Posenkette).

Aufruf: gesten_arme.py <ROOM.RDT> <rec> <clip> [<clip> ...]
"""
import math, os, sys
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import gesten_streifen as G                      # noqa: E402
E = G.E

ARME = {"rechts": (9, 10, 11), "links": (12, 13, 14)}


def rumpf_lokal(R, T, b):
    """Pose von Knochen b im Rumpf-(Knochen-0-)System."""
    R0 = R[0].astype(float) / 4096.0
    Rb = R[b].astype(float) / 4096.0
    return R0.T @ Rb, R0.T @ (T[b].astype(float) - T[0].astype(float))


def verdrehung(Rh, Ru, Rh0, Ru0):
    """Winkel der Hand-x-Achse um die Unterarm-y-Achse, gegen Bild 0 (Grad, -180..180)."""
    def ang(Rhand, Rarm):
        achse = Rarm[:, 1] / (np.linalg.norm(Rarm[:, 1]) or 1)
        hx = Rhand[:, 0] - achse * np.dot(Rhand[:, 0], achse)
        ref = Rarm[:, 0] - achse * np.dot(Rarm[:, 0], achse)
        hx /= (np.linalg.norm(hx) or 1); ref /= (np.linalg.norm(ref) or 1)
        return math.degrees(math.atan2(np.dot(np.cross(ref, hx), achse), np.dot(ref, hx)))
    a = ang(Rh, Ru) - ang(Rh0, Ru0)
    return (a + 180) % 360 - 180


def unterarm_rollen(Ru, Ro, Ru0, Ro0):
    """Rollen des Unterarms um seine eigene Laengsachse gegen den Oberarm (Grad)."""
    return verdrehung(Ru, Ro, Ru0, Ro0)


def main():
    rdt, rec = sys.argv[1], int(sys.argv[2])
    mo = G.Leon(rdt, rec)
    for clip in [int(c) for c in sys.argv[3:]]:
        first, n = mo.clips[clip]
        print("=== clip %d (%d Bilder)" % (clip, n))
        posen = []
        for b in range(n):
            kf = mo.kf_von(clip, b)
            R, T = mo.pose(kf)
            posen.append((kf, R, T))
        for arm, (s, u, h) in ARME.items():
            kf0, R0_, T0_ = posen[0]
            Rh0, ph0 = rumpf_lokal(R0_, T0_, h)
            Ru0, _ = rumpf_lokal(R0_, T0_, u)
            Ro0, _ = rumpf_lokal(R0_, T0_, s)
            weg, dreh, roll, bahn = [], [], [], []
            for (kf, R, T) in posen:
                Rh, ph = rumpf_lokal(R, T, h)
                Ru, _ = rumpf_lokal(R, T, u)
                Ro, _ = rumpf_lokal(R, T, s)
                weg.append(float(np.linalg.norm(ph - ph0)))
                dreh.append(verdrehung(Rh, Ru, Rh0, Ru0))
                roll.append(unterarm_rollen(Ru, Ro, Ru0, Ro0))
                bahn.append(ph)
            i = int(np.argmax(weg))
            d = bahn[i] - ph0
            print("  %-6s Hand max %5.0f bei b%-2d  delta(vorn,unten,links)=(%5.0f,%5.0f,%5.0f)  "
                  "Handdrehung um Unterarm: max|%4.0f| Grad (b%d)  Unterarmrollen max|%4.0f| Grad"
                  % (arm, weg[i], i, d[0], d[1], d[2],
                     max(abs(x) for x in dreh), int(np.argmax([abs(x) for x in dreh])),
                     max(abs(x) for x in roll)))
            eul0 = [E.kf_angles(mo.skel, kf0, b) for b in (s, u, h)]
            euli = [E.kf_angles(mo.skel, posen[i][0], b) for b in (s, u, h)]
            print("         Euler b0 %s  ->  b%d %s" % (eul0, i, euli))


if __name__ == "__main__":
    main()
