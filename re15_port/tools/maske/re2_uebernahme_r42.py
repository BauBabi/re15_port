#!/usr/bin/env python3
"""re2_uebernahme_r42.py - Runde 42 Strang rest, Stufe 6: vier Vordergrund-Masken aus RE2 (STAGE4-6, gleiches Hintergrundbild).

NUTZERWUNSCH 15.11. (Liste 29_naechste_punkte Punkt 9): "STAGE4 Labor: fehlende Masken/PRIs aus RE2 uebernehmen"; 18.11.: "Die PRIs in
Stage 4 habe ich dich gefragt, ob du die grossteils von Resident Evil 2 uebernehmen kannst." Die Bestandsaufnahme vom 18.11. (Bildvergleich
gegen alle 1262 RE2-Hintergruende, Kamerasaetze, Sichtpruefung) fand genau vier Kameras, deren RE2-Bild gleich ist und deren RE2-Maske
die heutige Luecke schliesst: ROOM4090 K2, ROOM50D0 K10, ROOM5100 K7, ROOM6000 K1 (Leon xxx0 und Elza xxx1 haben dieselben Kamerasaetze,
RDT-Kamerasatz +0x00..+0x1B bytegleich, und dasselbe Bild).

QUELLE (RE2 Leon, info/re2leon, selbst gelesen):
  Masken   PL0/RDT/ROOMrrrr.RDT, Kamerasatz RID (Offset-Tabelle [7] @RDT+0x24) + Cut*0x20, +0x1C = sprite.pri-Abschnitt.
           RE2-Form (Groessenfeld u16 @+6: 0 = Rechteck mit Breite/Hoehe, sonst Quadratseite) RE2 FUN_80049b1c @0x80049c10-48.
  Atlas    COMMON/BSS/ROOMrrr.BSS, Chunk Cut*0x10000; Laenge L aus der SLD-Tabelle der Stage-Datei (STAGE6.BIN @0x1650 =
           0x8011b650 - 0x8011a000, Zeiger gespeichert @STAGE6.BIN+0x1184..0x1190 `lui v0,0x8012 / addiu v0,v0,-0x49b0 / ...
           sw v0,-0x518(at)` -> 0x800cfae8; STAGE7.BIN @0x54 wie tools/r41_g5auftritt/re2_schnitt9.py), Block ab chunk[L-8],
           entpackt mit RE2 FUN_80010084 (re2_schnitt9.entpacken). Gegenprobe: bytegleich mit dem fremd entpackten
           COMMON/BSS/ROOMrrr/ROOMrrrrccc.tim.

UMRECHNUNG (re15_port/include/re15_pri.h:142-165, selbst gelesen):
  RE2  Figur-OT = otz >> 3 (@0x80027d60 srl v0,v0,1 / @0x80027d64 andi v0,v0,0xffc), Masken-OT = depth (@0x80049dac sll a0,a0,2),
       Masken VOR den Figuren eingehaengt (@0x80026770 jal 0x80049ca8 vor @0x8002689c) -> gleicher Bucket: Maske oben.
       Maske verdeckt gdw. bucket2(vz) = (1023*vz)>>15 >= d.
  RE1.5 Figur-OT = otz >> 4 (@0x8002565c), Masken-OT = depth (@0x80039658), Masken zuletzt (@0x8001ce54) -> gleicher Bucket: Figur oben.
       Maske verdeckt gdw. bucket1(vz) = (1023*vz)>>16 > d', d.h. bucket2(vz) >= 2*d' + 2.
  => d gerade: d' = d/2 - 1 (exakt). d ungerade: kein exaktes d'. PORT-WAHL: d' = (d+1)/2 - 1 -> verdeckt ab bucket2 >= d+1, also
     eine RE2-Stufe (32768/1023 = 32,03 Kamera-z) SPAETER als RE2 (eher eine Figur zu wenig verdeckt als eine davor uebermalt).
     Allgemein d' = ceil(d/2) - 1.

BILDLAGE (gemessen, Werkzeug-Ausgabe "Lage"): je Kamera der Versatz (dx, dy), um den das RE1.5-Bild gegen das RE2-Bild verschoben ist,
  als Mittel ueber alle Maskenpunkte (RE2-Atlasfarbe gegen RE1.5-Bild, Summe |RGB|). 4090/50D0/6000 (0,0), 5100 (+1,0) (RE2-Kamera
  um 0,186 Grad gedreht, Ziel (-11322,-2484,-1584) statt (-11340,-2484,-1602)). Der Versatz geht in die Gruppenanker (dst).

AUSGABE (je Leon- und Elza-Raum, bytegleich; Port-Lader bg_pc.c / main.c "RE2-Zusatz"):
  MASKS/ROOM%04X_RE2.MSK     R15M-Container (re15_pri.h) mit genau einer Sektion (dieser Cut), RE1.5-Sektionsform, alle Masken als
                             Rechteck (Groessenfeld 0 + Breite/Hoehe), srcX/srcY und Gruppen wie RE2, dst + Versatz, Tiefe d'.
  MASKS/ROOM%04X_RE2_%02d.TIM  Atlas im RE2-Layout (gleiche Abmessung, gleiche durchsichtige Punkte = Index 0 wie RE2), Farben aus dem
                             RE1.5-Bild an der Bildstelle des Punkts (dst + Versatz); Palette median-cut, CLUT wie tools/maske/atlas.clut_aus.

Aufruf (Repo-Wurzel):
  re15_port/build/tests/unit/probe_bg_dump.exe re15_port/shared_assets/PSX <bg> 4   (dito 5, 6)
  python re15_port/tools/maske/re2_uebernahme_r42.py <bg>            schreibt die 16 Dateien
  python re15_port/tools/maske/re2_uebernahme_r42.py <bg> --pruefen  vergleicht nur (Exit 1 bei Abweichung)
"""
import os
import struct
import sys

import numpy as np
from PIL import Image

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
RE2 = os.path.join(REPO, "info", "re2leon")
MASKS = os.path.join(PORT, "shared_assets", "PSX", "MASKS")
sys.path.insert(0, HIER)
sys.path.insert(0, os.path.join(PORT, "tools", "r41_g5auftritt"))
import atlas as atlasmod                      # noqa: E402  clut_aus
from re2_schnitt9 import entpacken            # noqa: E402  RE2 FUN_80010084

# (RE1.5-Raum, Cut, RE2-Raum, RE2-Cut, Stage-Datei, SLD-Tabelle, Zeile, Versatz x, Versatz y)
KAMERAS = [
    (0x4090, 2, "60D0", 2, "STAGE6.BIN", 0x1650, 0x0D, 0, 0),
    (0x50D0, 10, "6030", 10, "STAGE6.BIN", 0x1650, 0x03, 0, 0),
    (0x5100, 7, "6020", 3, "STAGE6.BIN", 0x1650, 0x02, 1, 0),
    (0x6000, 1, "7000", 1, "STAGE7.BIN", 0x54, 0x00, 0, 0),
]
CHUNK = 0x10000


def tiefe_re15(d):
    """RE2-Maskentiefe -> RE1.5 (Kopf: verdeckt ab bucket2 >= 2*d'+2; gerade d exakt, ungerade eine RE2-Stufe spaeter)."""
    return max(0, (d + 1) // 2 - 1)


def re2_kamera(rdt, cut):
    rid = struct.unpack_from("<I", rdt, 0x24)[0]
    satz = rid + cut * 32
    return satz, struct.unpack_from("<I", rdt, satz + 0x1C)[0]


def re2_sektion(rdt, po):
    """-> (gruppen [(n, base, gx, gy)], masken [(sx, sy, dx_lo, dy_lo, depth, w, h, gruppe)], laenge) in RE2-Form."""
    gc, mc = struct.unpack_from("<HH", rdt, po)
    p = po + 4
    gruppen = []
    for _ in range(gc):
        gruppen.append(struct.unpack_from("<HHhh", rdt, p))
        p += 8
    masken = []
    for gi, (n, _b, _gx, _gy) in enumerate(gruppen):
        for _ in range(n):
            sx, sy, dx, dy = rdt[p:p + 4]
            dep, feld = struct.unpack_from("<HH", rdt, p + 4)
            p += 8
            if feld == 0:                                  # @0x80049c18 beq -> Rechteck, Breite/Hoehe folgen
                w, h = struct.unpack_from("<HH", rdt, p)
                p += 4
            else:                                          # @0x80049c3c: Quadrat, Seite = Feld
                w = h = feld
            masken.append((sx, sy, dx, dy, dep, w, h, gi))
    return gruppen, masken, mc, p - po


def tim8(b):
    assert struct.unpack_from("<II", b, 0) == (0x10, 9)
    cl = struct.unpack_from("<I", b, 8)[0]
    p = 8 + cl
    iw, ih = struct.unpack_from("<HH", b, p + 8)
    pix = np.frombuffer(b, np.uint8, iw * 2 * ih, p + 12).reshape(ih, iw * 2)
    return cl, p, pix


def clut_rgb(b):
    c = np.frombuffer(b, "<u2", 256, 20).astype(np.int32)
    return np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1)


def re2_atlas(name_bss, cut, stbin, tab, zeile):
    bss = open(os.path.join(RE2, "COMMON", "BSS", name_bss), "rb").read()
    chunk = bss[cut * CHUNK:(cut + 1) * CHUNK]
    st = open(os.path.join(RE2, "COMMON", "BIN", stbin), "rb").read()
    L = struct.unpack_from("<H", st, tab + zeile * 0x20 + cut * 2)[0]
    if struct.unpack_from("<I", chunk, L - 4)[0] == 0:
        raise SystemExit("%s Cut %d: kein Atlas (chunk[L-4] == 0)" % (name_bss, cut))
    sld = struct.unpack_from("<I", chunk, L - 8)[0]
    tim, n = entpacken(chunk[sld:])
    return tim, (stbin, tab + zeile * 0x20 + cut * 2, L, sld)


def baue(bgdir, raum, cut, r2, c2, stbin, tab, zeile, vx, vy, log):
    rdt2 = open(os.path.join(RE2, "PL0", "RDT", "ROOM%s.RDT" % r2), "rb").read()
    satz, po = re2_kamera(rdt2, c2)
    gruppen, masken, decl, laenge = re2_sektion(rdt2, po)
    tim2, herkunft = re2_atlas("ROOM%s.BSS" % r2[:3], c2, *([stbin, tab, zeile]))
    fremd = os.path.join(RE2, "COMMON", "BSS", "ROOM%s" % r2[:3], "ROOM%s%03d.tim" % (r2, c2))
    if os.path.exists(fremd) and open(fremd, "rb").read()[:len(tim2)] != tim2:
        raise SystemExit("RE2-Atlas %s Cut %d weicht vom fremd entpackten %s ab" % (r2, c2, fremd))
    cl, pp, pix = tim8(tim2)
    pal2 = clut_rgb(tim2)
    bg = np.asarray(Image.open(os.path.join(bgdir, "ROOM%03X%02d.ppm" % (raum >> 4, cut))).convert("RGB"))

    # jeder undurchsichtige Atlaspunkt gehoert zu genau einer Maske (Pruefung), Farbe = RE1.5-Bild an dst + Versatz
    rgb = np.zeros(pix.shape + (3,), np.uint8)
    besitzer = np.full(pix.shape, -1, np.int32)
    fehler = []
    for k, (sx, sy, dx, dy, dep, w, h, gi) in enumerate(masken):
        _n, _b, gx, gy = gruppen[gi]
        for j in range(h):
            for i in range(w):
                ax, ay = sx + i, sy + j
                if ax >= pix.shape[1] or ay >= pix.shape[0] or pix[ay, ax] == 0:
                    continue
                if besitzer[ay, ax] >= 0 and besitzer[ay, ax] != k:
                    fehler.append((ax, ay))
                besitzer[ay, ax] = k
                X, Y = dx + gx + i + vx, dy + gy + j + vy
                rgb[ay, ax] = bg[Y, X] if (0 <= X < 320 and 0 <= Y < 240) else pal2[pix[ay, ax]]
    if fehler:
        raise SystemExit("ROOM%04X Cut %d: %d Atlaspunkte von zwei Masken benutzt (erster %s)" % (raum, cut, len(fehler), fehler[0]))
    opak = pix != 0
    q = Image.fromarray(rgb[opak].reshape(-1, 1, 3)).quantize(colors=255, method=Image.MEDIANCUT)
    roh = q.getpalette() or []
    n = min(255, len(roh) // 3)
    pal = np.array(roh[:n * 3], np.uint8).reshape(n, 3)
    flat = rgb[opak].astype(np.int32)
    idx = np.zeros(pix.shape, np.uint8)
    best = np.zeros(len(flat), np.int64)
    dmin = np.full(len(flat), 1 << 30, np.int64)
    for k in range(n):
        d = ((flat - pal[k].astype(np.int32)) ** 2).sum(1)
        sel = d < dmin
        dmin[sel] = d[sel]
        best[sel] = k
    idx[opak] = (best + 1).astype(np.uint8)
    clut = atlasmod.clut_aus(pal)
    tim = bytearray(tim2)
    tim[20:20 + 512] = clut.tobytes()
    tim[pp + 12:pp + 12 + idx.size] = idx.tobytes()
    # Kontrolle: Farbabstand Atlas (5-Bit-CLUT) gegen das RE1.5-Bild
    c = clut.astype(np.int32)
    prgb = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], -1)
    abst = np.abs(prgb[idx[opak]] - rgb[opak].astype(np.int32)).sum(1).mean()

    # Sektion in RE1.5-Form: Gruppen wie RE2 (Anker + Versatz), Masken-Bytes wie RE2, Tiefe d', Rechteckform
    sek = bytearray(struct.pack("<HH", len(gruppen), decl))
    for (gn, gb, gx, gy) in gruppen:
        sek += struct.pack("<HHhh", gn, gb, gx + vx, gy + vy)
    tiefen = []
    for (sx, sy, dx, dy, dep, w, h, gi) in masken:
        d15 = tiefe_re15(dep)
        tiefen.append((dep, d15))
        sek += struct.pack("<BBBBHHHH", sx, sy, dx, dy, d15, 0, w, h)
    n_cut = open(os.path.join(PORT, "shared_assets", "PSX", "STAGE%d" % (raum >> 12), "ROOM%04X.RDT" % raum), "rb").read()[1]
    kopf = 12 + 4 * n_cut
    offs = [0] * n_cut
    offs[cut] = kopf
    msk = b"R15M" + struct.pack("<II", 1, n_cut) + struct.pack("<%dI" % n_cut, *offs) + bytes(sek)
    log.append("ROOM%04X K%d <- RE2 ROOM%s K%d: Kamerasatz RDT+0x%X, PRI @0x%X (%d Byte, %d Gruppen, %d Masken, Kopf %d), "
               "Atlas %s @0x%X = L 0x%X, SLD @0x%X, %dx%d; Versatz (%d,%d); Tiefen RE2 %d..%d -> RE1.5 %d..%d (%d ungerade); "
               "Atlas-Farbabstand %.1f/765"
               % (raum, cut, r2, c2, satz, po, laenge, len(gruppen), len(masken), decl, herkunft[0], herkunft[1], herkunft[2],
                  herkunft[3], pix.shape[1], pix.shape[0], vx, vy, min(t[0] for t in tiefen), max(t[0] for t in tiefen),
                  min(t[1] for t in tiefen), max(t[1] for t in tiefen), sum(t[0] & 1 for t in tiefen), abst))
    return msk, bytes(tim)


def main():
    if len(sys.argv) < 2:
        raise SystemExit(__doc__)
    bgdir = sys.argv[1]
    pruefen = "--pruefen" in sys.argv[2:]
    log = []
    abweichung = 0
    for (raum, cut, r2, c2, stbin, tab, zeile, vx, vy) in KAMERAS:
        msk, tim = baue(bgdir, raum, cut, r2, c2, stbin, tab, zeile, vx, vy, log)
        for rid in (raum, raum + 1):          # Leon xxx0, Elza xxx1 (Kamerasatz und Bild gleich)
            for name, daten in (("ROOM%04X_RE2.MSK" % rid, msk), ("ROOM%04X_RE2_%02d.TIM" % (rid, cut), tim)):
                p = os.path.join(MASKS, name)
                if pruefen:
                    if not os.path.exists(p) or open(p, "rb").read() != daten:
                        print("ABWEICHUNG", name)
                        abweichung += 1
                else:
                    with open(p, "wb") as f:
                        f.write(daten)
    for z in log:
        print(z)
    if pruefen:
        print("pruefen: %d Abweichungen" % abweichung)
        sys.exit(1 if abweichung else 0)


if __name__ == "__main__":
    main()
