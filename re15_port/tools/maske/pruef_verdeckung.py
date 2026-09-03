"""Prueft, ob eine Maske die Figur an erreichbaren Stellen VERSCHLUCKT.

⛔ WARUM ES DAS GIBT (Nutzer-Befund 2026-09-03, error2.png): in ROOM1140 Cut 3 war von
Leon nur noch der Kopf zu sehen. Ursache war meine AUSWAHL: ich hatte Teppichflaechen
links und rechts des Podiums fuer Moebel gehalten. Boden zu markieren ist der
schlimmstmoegliche Fehler — der Boden zieht sich HINTER den Spieler, seine Maske
verdeckt ihn also ueberall, wo er steht.

Die bisherigen Pruefungen konnten das nicht sehen:
  - die Deckungsprobe (Atlas auf den Hintergrund blitten) prueft nur, ob die Maske
    dorthin gehoert, wo sie gezeichnet wird — nicht, ob sie dort etwas verdecken DARF;
  - RE15_POCC_SCAN zaehlt nur Punkte INNERHALB der Maskenrechtecke und ist damit
    einseitig (erkennt zu weite Tiefen, nicht zu nahe).

Diese Pruefung stellt die richtige Frage: wie viel der Figur waere an einer erreichbaren
Bodenposition verdeckt? Sie faehrt dazu ein Bodengitter aus der Kollision (SCA), lasst
die Figur dort stehen, projiziert ihren Koerper und misst, welcher Anteil davon hinter
Maskenpixeln verschwindet, deren Tiefe naeher ist als die Figur.

Aufruf:
    python re15_port/tools/maske/pruef_verdeckung.py ROOM1140
    python re15_port/tools/maske/pruef_verdeckung.py --alle
"""
import argparse
import glob
import os
import struct
import sys

import numpy as np

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom
from geom import load_rdt

CD = "re15_port/shared_assets/PSX"
MASKS = "re15_port/shared_assets/PSX/MASKS"

# Koerpermass der Figur in Weltkoordinaten. Leon ist rund 1500 Einheiten hoch
# (PSX-Y zeigt nach unten, deshalb negativ) und rund 600 breit — beides aus den
# Spielerpositionen der Sonde und der Modellgroesse abgeleitet, nicht kritisch:
# die Aussage "Figur fast vollstaendig verdeckt" haengt nicht am letzten Pixel.
KOERPER_HOCH = 1500
KOERPER_BREIT = 600


def sca_eintraege(d):
    off = struct.unpack_from("<I", d, 0x20)[0]
    if off < 0x40 or off + 24 > len(d):
        return []
    cnt = struct.unpack_from("<5I", d, off + 4)
    n = sum(c & 0xFFFF for c in cnt)
    if n <= 0 or n > 4000:
        return []
    out = []
    for i in range(n):
        p = off + 24 + i * 12
        if p + 12 > len(d):
            break
        w, dens, x, z = struct.unpack_from("<HHhh", d, p)
        out.append((x, z, w, dens))
    return out


def masken_des_cuts(blob, cut):
    """-> Liste (X, Y, w, h, depth) in Bildkoordinaten, oder None."""
    if blob is None or blob[:4] != b"R15M":
        return None
    _, nc = struct.unpack_from("<II", blob, 4)
    if cut >= nc:
        return None
    offs = struct.unpack_from("<%dI" % nc, blob, 12)
    if not offs[cut]:
        return None
    o = offs[cut]
    gc, mc = struct.unpack_from("<HH", blob, o)
    p = o + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, base, dx, dy = struct.unpack_from("<HHhh", blob, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        sx, sy, dx, dy = blob[p], blob[p+1], blob[p+2], blob[p+3]
        dep, size = struct.unpack_from("<HH", blob, p + 4); p += 8
        if (size & 0xf000) == 0:
            w, h = struct.unpack_from("<HH", blob, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        out.append((((dx + ax + 0x8000) & 0xffff) - 0x8000,
                    ((dy + ay + 0x8000) & 0xffff) - 0x8000, w, h, dep))
    return out


def pruefe(room, schwelle=0.80, anteil_warn=0.15):
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(CD, room)
    if not rdt:
        return None
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    p = os.path.join(MASKS, "%s.MSK" % room)
    blob = open(p, "rb").read() if os.path.exists(p) else None
    if blob is None:
        return None
    sca = sca_eintraege(rdt)
    if not sca:
        return None
    X0 = min(e[0] for e in sca); X1 = max(e[0] + e[2] for e in sca)
    Z0 = min(e[1] for e in sca); Z1 = max(e[1] + e[3] for e in sca)

    zeilen = []
    for cut in range(rdt[1]):
        ms = masken_des_cuts(blob, cut)
        if not ms:
            continue
        v = geom.cut_view(rdt, cam, cut)
        if not v:
            continue
        R, t, H = v
        if H <= 0:
            continue
        # Tiefenkarte der Maske (kleinste = naechste Tiefe je Pixel)
        tief = np.zeros((240, 320), np.int32)
        for (X, Y, w, h, dep) in ms:
            x0, x1 = max(0, X), min(320, X + w)
            y0, y1 = max(0, Y), min(240, Y + h)
            if x1 <= x0 or y1 <= y0:
                continue
            sub = tief[y0:y1, x0:x1]
            tief[y0:y1, x0:x1] = np.where((sub == 0) | (dep < sub), dep, sub)

        n = 0; verschluckt = 0
        for wx in range(X0, X1 + 1, 400):
            for wz in range(Z0, Z1 + 1, 400):
                vz = (wx*R[6] + 0*R[7] + wz*R[8]) / 4096.0 + t[2]
                if vz <= 64:
                    continue
                vx = (wx*R[0] + 0*R[1] + wz*R[2]) / 4096.0 + t[0]
                vyf = (wx*R[3] + 0*R[4] + wz*R[5]) / 4096.0 + t[1]
                vyk = (wx*R[3] + (-KOERPER_HOCH)*R[4] + wz*R[5]) / 4096.0 + t[1]
                sxc = 160 + vx*H/vz
                syf = 120 + vyf*H/vz          # Fuesse
                syk = 120 + vyk*H/vz          # Kopf
                halb = (KOERPER_BREIT/2.0)*H/vz
                x0 = int(max(0, sxc - halb)); x1 = int(min(320, sxc + halb))
                y0 = int(max(0, min(syk, syf))); y1 = int(min(240, max(syk, syf)))
                if x1 <= x0 or y1 <= y0:
                    continue
                n += 1
                box = tief[y0:y1, x0:x1]
                # verdeckt, wo eine Maske NAEHER liegt als die Figur
                verd = ((box > 0) & (box * 64.0 < vz)).mean()
                if verd >= schwelle:
                    verschluckt += 1
        if n:
            zeilen.append((cut, n, verschluckt, verschluckt / n))
    return zeilen


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room", nargs="?")
    ap.add_argument("--alle", action="store_true")
    ap.add_argument("--schwelle", type=float, default=0.80)
    a = ap.parse_args()
    rooms = ([os.path.basename(p)[:-4] for p in sorted(glob.glob(os.path.join(MASKS, "ROOM*.MSK")))]
             if a.alle else [a.room.upper()])
    schlimm = 0
    for room in rooms:
        z = pruefe(room, a.schwelle)
        if not z:
            continue
        print("== %s" % room)
        for (cut, n, v, q) in z:
            flagge = "  <<< VERSCHLUCKT DIE FIGUR" if q > 0.15 else ""
            print("   Cut %2d: %4d erreichbare Stellen, davon %4d mit >=%.0f %% verdeckter Figur"
                  " = %5.1f %%%s" % (cut, n, v, 100*a.schwelle, 100*q, flagge))
            if q > 0.15:
                schlimm += 1
    print("\nCuts, die die Figur an mehr als 15 %% der Stellen verschlucken: %d" % schlimm)
    return 1 if schlimm else 0


if __name__ == "__main__":
    sys.exit(main())
