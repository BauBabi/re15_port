"""Verdeckungskarte: wo im Raum verschluckt die Maske die Figur?

⛔ WOZU (Nutzer-Befunde vom 2026-09-03): der schaedliche Fehler ist nicht die fehlende
Maske, sondern die Maske, die den Spieler uebermalt, waehrend er DAVOR steht. Diese
Karte macht genau das sichtbar: jeder begehbare Bodenpunkt wird eingefaerbt danach,
welcher Anteil der Figur dort hinter naeheren Maskenpixeln verschwindet.

Zwei Dinge kommen aus dem ECHTEN Code, nicht aus einer Nachbildung:
  * die BEGEHBAREN Punkte aus der Engine (RE15_FLOOR_DUMP -> build/floor_<raum>.txt);
    eine Python-Nachbildung der SCA-Auswertung hatte frueher 69 % der KUENSTLER-Cuts
    faelschlich als "verschluckt" gemeldet.
  * die Maskendeckung PIXELGENAU aus dem Atlas (Palettenindex 0 = durchsichtig);
    mit Rechtecken statt Atlaspixeln ueberschaetzt man die Deckung deutlich.

Aufruf:
    python re15_port/tools/maske/verdeckungskarte.py ROOM1140            (alle Cuts)
    python re15_port/tools/maske/verdeckungskarte.py ROOM1140 2
"""
import argparse, os, struct, sys
import numpy as np
from PIL import Image, ImageDraw
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from geom import load_bg, load_rdt, cut_view
from blatt import aufhellen
import maskenbild as MB

MASKS = "re15_port/shared_assets/PSX/MASKS"
CD = "re15_port/shared_assets/PSX"
KOERPER_HOCH = 1500      # wie die Engine-Sonde (main.c, RE15_POCC_SCAN)
KOERPER_HALB = 300


def deckung(room, cut):
    p = os.path.join(MASKS, "%s.MSK" % room)
    if not os.path.exists(p):
        return None, None
    ms = MB.masken(open(p, "rb").read(), cut)
    if not ms:
        return None, None
    t = MB.lies_tim(os.path.join(MASKS, "%s_PRI%02d.TIM" % (room, cut)))
    idx = t[0] if t else None
    tief = np.zeros((240, 320), np.int32)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w); y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0: continue
        sub = idx[sy+(y0-Y):sy+(y1-Y), sx+(x0-X):sx+(x1-X)] if idx is not None else None
        op = (sub != 0) if sub is not None else np.ones((y1-y0, x1-x0), bool)
        z = tief[y0:y1, x0:x1]
        tief[y0:y1, x0:x1] = np.where(op & ((z == 0) | (dep < z)), dep, z)
    return tief, ms


def boden(room):
    p = "build/floor_%s.txt" % room[4:].lower()
    if not os.path.exists(p):
        raise SystemExit("Bodenpunkte fehlen: %s — Spiel mit RE15_FLOOR_DUMP=1 laufen "
                         "lassen und aus debug.log herausziehen." % p)
    a = np.loadtxt(p, dtype=np.int32)
    return a


def karte(room, cut, punkte, out):
    rid = int(room[4:], 16)
    rdt, _ = load_rdt(CD, room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    v = cut_view(rdt, cam, cut)
    if not v: return None
    R, t, H = v
    tief, ms = deckung(room, cut)
    if tief is None: return None
    bg = load_bg("build/bg_ppm", rid, cut)
    base = (aufhellen(bg) * 255).astype(np.uint8)
    im = Image.fromarray(base).resize((960, 720), Image.LANCZOS)
    d = ImageDraw.Draw(im, "RGBA")

    X = punkte[:, 0].astype(np.float64); Z = punkte[:, 1].astype(np.float64)
    vz = (X*R[6] + Z*R[8]) / 4096.0 + t[2]
    ok = vz > 64
    X, Z, vz = X[ok], Z[ok], vz[ok]
    vx = (X*R[0] + Z*R[2]) / 4096.0 + t[0]
    vyf = (X*R[3] + Z*R[5]) / 4096.0 + t[1]
    vyk = (X*R[3] + (-KOERPER_HOCH)*R[4] + Z*R[5]) / 4096.0 + t[1]
    sx = 160 + vx*H/vz; syf = 120 + vyf*H/vz; syk = 120 + vyk*H/vz
    halb = KOERPER_HALB*H/vz
    n = 0; schluck = 0; teil = 0
    for i in range(len(X)):
        x0 = int(max(0, sx[i]-halb[i])); x1 = int(min(320, sx[i]+halb[i]))
        y0 = int(max(0, min(syk[i], syf[i]))); y1 = int(min(240, max(syk[i], syf[i])))
        if x1 <= x0 or y1 <= y0: continue
        box = tief[y0:y1, x0:x1]
        verd = float(((box > 0) & (box*64.0 < vz[i])).mean())
        n += 1
        if verd >= 0.60:
            schluck += 1; col = (255, 0, 0, 210)
        elif verd >= 0.15:
            teil += 1; col = (255, 190, 0, 170)
        elif verd > 0:
            col = (60, 160, 255, 120)
        else:
            continue
        d.rectangle([sx[i]*3-1, syf[i]*3-1, sx[i]*3+1, syf[i]*3+1], fill=col)
    os.makedirs(out, exist_ok=True)
    p = os.path.join(out, "%s_%02d_verdeckung.png" % (room, cut))
    im.save(p)
    return p, n, schluck, teil


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room"); ap.add_argument("cut", type=int, nargs="?")
    ap.add_argument("--out", default="build/verdeckung")
    a = ap.parse_args()
    room = a.room.upper()
    pkt = boden(room)
    rdt, _ = load_rdt(CD, room)
    cuts = [a.cut] if a.cut is not None else range(rdt[1])
    for c in cuts:
        r = karte(room, c, pkt, a.out)
        if not r: continue
        p, n, s, t_ = r
        print("Cut %2d: %6d Stehplaetze im Bild | >=60 %% verdeckt: %5d (%.1f %%) | "
              "15-60 %%: %5d (%.1f %%)  -> %s"
              % (c, n, s, 100.0*s/max(1, n), t_, 100.0*t_/max(1, n), p))


if __name__ == "__main__":
    main()
