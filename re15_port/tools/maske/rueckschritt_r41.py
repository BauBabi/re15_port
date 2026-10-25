"""Runde 41 Spur M, Nachbesserung 5 (Abnahme 4, Mangel 1 = Entscheid M-E2 "Rueckschritt-Suche wie in Nachbesserung 3"):
3D-Suche nach Figurpunkten, die der NEUE Maskenstand verdeckt und der alte nicht (und umgekehrt).

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 5".

Verfahren (wie die Rueckschritt-Suche der Abnahme 3 ueber die NB3-Cuts, eigene Umsetzung):
  * Plaetze = erreichbare Stehplaetze des Cuts (`test_r37_masken36 plaetze <raum> <cut>`: Saaten + Flutfuellung + Kamerazonen +
    Figuren-Cull am Regionsviereck, dieselben Plaetze wie der Ausschluss), Datei p_<RAUM>_<CUT>.txt mit Zeilen "P x z band".
  * Figur grosszuegig: Zylinder Radius 450 (Kollisionsradius des Spielers DAT_80073e94[6]) x Hoehe 50..1800 ueber dem Bandboden
    (y = -1800 * Band, BAND_Y 0x708), 4 Ringe (0/150/300/450) x 12 Richtungen x 15 Hoehen.
  * Deckung je Bildpunkt wie Lader + Zeichner: Sektion wie re15_pri_parse_section (Gruppen-Anker + u8-Versatz, Zahl = deklarierte
    Zahl & 0xFF, hoechstens 128 = RE15_PRI_MAX_MASKS_PER_CUT), Ausschluss-Tabelle masken36_ausschluss_r37.inc, Atlas-Index 0 =
    durchsichtig; ein Figurpunkt mit Kamera-z vz ist verdeckt gdw. depth < (1023*vz)>>16 (@0x8002565c sra v1,v1,4, ZSF3 341
    @0x80066c70, Masken-OT @0x80039650).
  * NEU VERDECKT = jetzt verdeckt, vorher nicht -> Pruefung: liegt IRGENDEINE solide SCA-Zelle (u0 & 1, alle Baender, Huellrechteck,
    Bandboden bis 2400 darueber) zwischen Kamera und Punkt? Sonst "ohne Zelle davor" (= moeglicher Rueckschritt: Figur VOR dem
    Gegenstand uebermalt).
  * NEU FREI = vorher verdeckt, jetzt nicht (moeglicher Rueckschritt: Figur hinter einem Gegenstand wieder sichtbar).
Aufruf:
  python re15_port/tools/maske/rueckschritt_r41.py --vor-masks <MASKS vorher> --vor-aus <Ausschluss vorher> --plaetze <Ordner>
         [--nach-masks <MASKS>] [--nach-aus <Ausschluss>] [--schritt N] RRRR_C ...
"""
import argparse
import json
import os
import re
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
MAXM = 128                      # re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT (NB5)


def ausschluss(pfad):
    t = open(pfad).read()
    out = {}
    for m in re.finditer(r"\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+), \{ 0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u, 0x([0-9A-F]{8})u, "
                         r"0x([0-9A-F]{8})u \} \}", t):
        r, c = int(m.group(1), 16), int(m.group(2))
        bits = [int(m.group(i), 16) for i in range(4, 8)]
        out[(r, c)] = set(i for i in range(128) if (bits[i >> 5] >> (i & 31)) & 1)
    return out


def sektion(blob, off):
    gc, mc = struct.unpack_from("<HH", blob, off)
    if (gc, mc) == (0xFFFF, 0xFFFF) or gc == 0 or mc == 0:
        return [], 0
    p = off + 4
    groups = []
    for _ in range(gc):
        n, _b, dx, dy = struct.unpack_from("<HHhh", blob, p)
        groups.append((n, dx, dy))
        p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(g[0] for g in groups)):
        sx, sy, dxl, dyl, dep = struct.unpack_from("<BBBBH", blob, p)
        size_b = blob[p + 7]
        p += 8
        if (size_b & 0xF0) == 0:
            w, h = struct.unpack_from("<HH", blob, p)
            p += 4
        else:
            w = h = (size_b >> 4) * 8
        while gi < gc and used >= groups[gi][0]:
            gi += 1
            used = 0
        ax, ay = (groups[gi][1], groups[gi][2]) if gi < gc else (0, 0)
        used += 1
        if len(out) < MAXM:
            X = ((dxl + ax + 0x8000) & 0xFFFF) - 0x8000
            Y = ((dyl + ay + 0x8000) & 0xFFFF) - 0x8000
            out.append((sx, sy, X, Y, w, h, dep))
    return out, min(mc & 0xFF, len(out))


def tim8(p):
    d = open(p, "rb").read()
    flag = struct.unpack_from("<I", d, 4)[0]
    q = 8
    if flag & 8:
        q += struct.unpack_from("<I", d, q)[0]
    _bl, _x, _y, w, h = struct.unpack_from("<IHHHH", d, q)
    return np.frombuffer(d, np.uint8, w * 2 * h, q + 12).reshape(h, w * 2)


def deckung(masks_dir, aus, rid, cut):
    """-> Schluessel je Bildpunkt (vz ab dem verdeckt wird, inf = keine Maske)."""
    blob = open(os.path.join(masks_dir, "ROOM%04X.MSK" % rid), "rb").read()
    _v, nc = struct.unpack_from("<II", blob, 4)
    key = np.full((240, 320), np.inf)
    if cut >= nc:
        return key
    off = struct.unpack_from("<I", blob, 12 + 4 * cut)[0]
    if not off:
        return key
    recs, dc = sektion(blob, off)
    img = tim8(os.path.join(masks_dir, "ROOM%04X_PRI%02d.TIM" % (rid, cut)))
    ex = aus.get((rid, cut), set())
    for i, (sx, sy, X, Y, w, h, dep) in enumerate(recs[:dc]):
        if i in ex:
            continue
        k = (dep + 1) * 65536.0 / 1023.0
        for yy in range(h):
            yb = Y + yy
            if not (0 <= yb < 240) or sy + yy >= img.shape[0]:
                continue
            row = img[sy + yy, sx:min(sx + w, img.shape[1])]
            xs = np.arange(len(row)) + X
            ok = (xs >= 0) & (xs < 320) & (row != 0)
            sel = xs[ok]
            key[yb, sel] = np.minimum(key[yb, sel], k)
    return key


def isqrt(n):
    return int(np.floor(np.sqrt(float(n)))) if n > 0 else 0


def tdiv(a, b):
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


def ansicht(rdt, cut):
    """Kamera wie re15_camera_build_view (FUN_80053ca4): RDT +0x24 Satz 32 B; -> (R, t, H, Kameraort)."""
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    rec = cam + cut * 32
    fov = struct.unpack_from("<H", rdt, rec + 2)[0]
    px, py, pz = struct.unpack_from("<iii", rdt, rec + 4)
    tx, ty, tz = struct.unpack_from("<iii", rdt, rec + 0x10)
    dx, dy, dz = tx - px, ty - py, tz - pz
    dist = isqrt(dx * dx + dy * dy + dz * dz)
    horiz = isqrt(dx * dx + dz * dz)
    sp = tdiv(-dy * 4096, dist)
    cp = tdiv(horiz * 4096, dist)
    sy = tdiv(dx * 4096, horiz)
    cy = tdiv(dz * 4096, horiz)
    R = [cy, 0, -sy, (sp * sy) >> 12, cp, (sp * cy) >> 12, (cp * sy) >> 12, -sp, (cp * cy) >> 12]
    t = [(R[0] * -px + R[1] * -py + R[2] * -pz) >> 12, (R[3] * -px + R[4] * -py + R[5] * -pz) >> 12,
         (R[6] * -px + R[7] * -py + R[8] * -pz) >> 12]
    return np.array(R, float).reshape(3, 3) / 4096.0, np.array(t, float), fov >> 7, np.array([px, py, pz], float)


def kaesten(rdt, hoehe=2400.0):
    s = struct.unpack_from("<I", rdt, 0x20)[0]
    n = sum(struct.unpack_from("<5I", rdt, s + 4))
    out, seen = [], set()
    for i in range(n):
        o = s + 24 + 12 * i
        w, d, x, z = struct.unpack_from("<HHhh", rdt, o)
        typ, u0, _u1, f = rdt[o + 8:o + 12]
        if not (u0 & 1) or (typ & 15) not in range(1, 10) or w <= 0 or d <= 0:
            continue
        k = (x, z, w, d, f >> 4)
        if k in seen:
            continue
        seen.add(k)
        y0 = -1800.0 * (f >> 4)
        out.append((np.array([x, y0 - hoehe, z], float), np.array([x + w, y0, z + d], float)))
    return out


def zelle_davor(p0, P, boxes):
    hit = np.zeros(len(P), bool)
    d = P - p0
    for lo, hi in boxes:
        t0 = np.zeros(len(P))
        t1 = np.full(len(P), 0.999)
        ok = ~hit
        for a in range(3):
            da = d[:, a]
            par = np.abs(da) < 1e-9
            ok &= ~(par & ((p0[a] < lo[a]) | (p0[a] > hi[a])))
            with np.errstate(divide="ignore", invalid="ignore"):
                ta = (lo[a] - p0[a]) / da
                tb = (hi[a] - p0[a]) / da
            t0 = np.maximum(t0, np.where(par, -np.inf, np.minimum(ta, tb)))
            t1 = np.minimum(t1, np.where(par, np.inf, np.maximum(ta, tb)))
        hit |= ok & (t0 <= t1)
    return hit


def figur(x, z, b):
    y0 = -1800.0 * b
    pts = []
    for r in (0, 150, 300, 450):
        for a in (range(1) if r == 0 else range(0, 360, 30)):
            dx, dz = r * np.cos(np.radians(a)), r * np.sin(np.radians(a))
            for h in range(50, 1801, 125):
                pts.append((x + dx, y0 - h, z + dz))
    return np.array(pts, float)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cuts", nargs="+", help="RRRR_C (Raum hex, Cut)")
    ap.add_argument("--vor-masks", required=True)
    ap.add_argument("--vor-aus", required=True)
    ap.add_argument("--nach-masks", default=os.path.join(PSX, "MASKS"))
    ap.add_argument("--nach-aus", default=os.path.join(REPO, "re15_port", "engine", "src", "gen", "masken36_ausschluss_r37.inc"))
    ap.add_argument("--plaetze", required=True)
    ap.add_argument("--schritt", type=int, default=1, help="nur jeder n-te Platz je Achse (100er-Gitter)")
    ap.add_argument("--out", default=None)
    a = ap.parse_args()
    IV, IN = ausschluss(a.vor_aus), ausschluss(a.nach_aus)
    res = {}
    for rc in a.cuts:
        rid, cut = int(rc.split("_")[0], 16), int(rc.split("_")[1])
        N = deckung(a.nach_masks, IN, rid, cut)
        V = deckung(a.vor_masks, IV, rid, cut)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "ROOM%04X.RDT" % rid), "rb").read()
        R, t, H, cam = ansicht(rdt, cut)
        bx = kaesten(rdt)
        P = [tuple(int(v) for v in l.split()[1:4]) for l in open(os.path.join(a.plaetze, "p_%04X_%d.txt" % (rid, cut))) if l.startswith("P ")]
        P = [p for p in P if (p[0] // 100) % a.schritt == 0 and (p[1] // 100) % a.schritt == 0]
        n_im, neu_v, neu_f = 0, [], []
        for (x, z, b) in P:
            pts = figur(x, z, b)
            c = R.dot(pts.T).T + t
            if c[:, 2].min() <= 200:
                continue
            sx = 160 + H * c[:, 0] / c[:, 2]
            sy = 120 + H * c[:, 1] / c[:, 2]
            inb = (sx >= 0) & (sx < 320) & (sy >= 0) & (sy < 240)
            if inb.sum() < 20:
                continue
            n_im += 1
            idx = np.nonzero(inb)[0]
            X, Y, vz = sx[idx].astype(int), sy[idx].astype(int), c[idx, 2]
            kn, kv = N[Y, X], V[Y, X]
            nv = (kn <= vz) & ~(kv <= vz)
            nf = (kv <= vz) & ~(kn <= vz)
            if nv.any():
                frei = ~zelle_davor(cam, pts[idx[nv]], bx)
                neu_v.append((int(frei.sum()), int(nv.sum()), x, z, b))
            if nf.any():
                hint = zelle_davor(cam, pts[idx[nf]], bx)
                neu_f.append((int(hint.sum()), int(nf.sum()), x, z, b))
        neu_v.sort(reverse=True)
        neu_f.sort(reverse=True)
        e = dict(cut=rc, plaetze=len(P), im_bild=n_im, plaetze_neu_verdeckt=len(neu_v),
                 punkte_neu_verdeckt=sum(q[1] for q in neu_v), punkte_neu_verdeckt_ohne_zelle=sum(q[0] for q in neu_v),
                 plaetze_ohne_zelle=sum(1 for q in neu_v if q[0] > 0), punkte_neu_frei=sum(q[1] for q in neu_f),
                 punkte_neu_frei_mit_zelle=sum(q[0] for q in neu_f), bsp_ohne_zelle=[q for q in neu_v if q[0] > 0][:6],
                 bsp_frei=neu_f[:6])
        res[rc] = e
        print(json.dumps(e), flush=True)
    if a.out:
        json.dump(res, open(a.out, "w"), indent=1)


if __name__ == "__main__":
    main()
