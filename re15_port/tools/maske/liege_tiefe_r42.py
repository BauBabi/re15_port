#!/usr/bin/env python3
"""liege_tiefe_r42.py - Runde 42 Story, Stufe STAGE6, Punkt T-05 (Konzept Abschnitt 5, Entscheid F-10 "Tiefe aendern"):
die Tiefe der Port-Masken ueber der Liegeflaeche der Liege in ROOM6030/ROOM6031 Kamera 9.

BEFUND (Konzept [B36], hier nachgemessen): Marvin liegt in 6030/6031 auf der Liege (Sce_em_set Typ 0x40 auf (-21241,-920,-26118):
ROOM6031 sub06 @0x00E42 `44 00 40 40 01 00 00 ff 07 ad 68 fc fa 99`, ROOM6030 sub06 @0x01030 dieselben Bytes). Die R15M-Sektion
von Cut 9 (MASKS/ROOM6030.MSK == ROOM6031.MSK, 91 Rechtecke) legt die Rechtecke ueber der Liegeflaeche FLACH mit der Tiefe der
Vorderkante ab (Tiefe 68 = vz 4352; Flaeche am Rand Bildzeile 215: vz 4318). Marvin liegt bei vz 5606..6396 (Konzept-Kern) und
wird dadurch viereckig abgeschnitten (Fehlerklasse "flache Maske verdeckt", Memory reai-v2-flache-maske-verdeckt-nichts).

GEOMETRIE (Belege):
  Liege   = SCA-Zelle x -23080..-18840, z -27221..-25221 (ROOM6030 SCA-Eintrag @0x00768 `90 10 d0 07 d8 a5 ab 95 01 ff 00 03`
            = breite 4240, tiefe 2000, x -23080, z -27221; tests/unit/gen_r41_masken36_zellen.inc; in ROOM6031 dieselben Bytes,
            Pruefung unten).
  Flaeche = y -920 (Marvins Lage-y oben, `68 fc`).
  Kamera 9 = RID-Satz ROOM6030/6031 @0x00180 (Kameratabelle @0x24 -> 0x60, + 9 * 32), in beiden RDTs bytegleich.
  Tiefe   = vz / 64 (Masken-OT-Index = Tiefe x1, Figuren-OT-Index = otz >> 4 = vz / 64: include/re15_pri.h, FUN_80039590
            @0x80039650-64 und FUN_800254a0 @0x80025648-64).

REGEL (Nutzer F-10: Form/Freistellung bleibt, nur die Tiefe; die Vorderkante bleibt):
  Jedes Bildpixel des Rechtecks wird auf die Ebene y -920 zurueckgerechnet. Ein Rechteck wird geaendert, wenn
    (a) mindestens ein Pixel auf der Liegeflaeche liegt,
    (b) kein Pixel ueber die Vorderkante (Fussende x > -18840, Bildzeile ~226..249) oder ueber eine Laengsseite (z ausserhalb
        -27221..-25221) hinausreicht - diese Rechtecke (Vorderkante, Seitenholme) bleiben, wie sie sind; ueber das Kopfende
        (x < -23080) darf es hinausreichen (dort liegt Marvins Kopf, dahinter nur Wand und die stehenden NPC, s. u.),
    (c) seine Tiefe VOR der Flaeche liegt (Tiefe * 64 < groesstes vz der Flaeche unter dem Rechteck).
  Neue Tiefe = ceil(groesstes Flaechen-vz unter dem Rechteck / 64): alles, was AUF der Flaeche liegt, ist naeher als die Flaeche
  selbst (die Kamera blickt von oben) und bleibt sichtbar; was unter/hinter der Liege ist, bleibt verdeckt. Lage, Groesse,
  Atlas-Quelle und Reihenfolge der Rechtecke bleiben bytegleich.
  Gegenprobe hinter dem Kopfende: Sherry (Sce_em_set ROOM6031 sub06 @0x00E6A (-24026,-26208)) liegt in Cut 9 bei vz 8568..8845,
  John (@0x00E56 (-26055,-25784)) bei vz 10418..10846 - beide weiter als jede neue Tiefe (hoechstens Flaechenende vz 7986 ->
  125 = vz 8000), sie bleiben hinter der Liege verdeckt.

Aufruf (Repo-Wurzel oder beliebig):
  python re15_port/tools/maske/liege_tiefe_r42.py            -> Bericht (aendert nichts)
  python re15_port/tools/maske/liege_tiefe_r42.py --schreiben -> MASKS/ROOM6030.MSK und ROOM6031.MSK patchen (nur Tiefen-Halbwoerter)
  python re15_port/tools/maske/liege_tiefe_r42.py --check     -> 0, wenn beide Dateien die Soll-Tiefen tragen
  python re15_port/tools/maske/liege_tiefe_r42.py --test-inc  -> tests/unit/gen_r42_liege_tiefe.inc (Soll-Tiefen + Marvins Kern);
                                                                 VOR --schreiben laufen lassen (tiefe_alt = Ausgangsstand)
"""
import math
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
PORT = os.path.abspath(os.path.join(HIER, "..", ".."))
PSX = os.path.join(PORT, "shared_assets", "PSX")
MASKS = os.path.join(PSX, "MASKS")
import geom  # noqa: E402

RAEUME = ("ROOM6030", "ROOM6031")
CUT = 9
LIEGE_SCA = bytes.fromhex("90 10 d0 07 d8 a5 ab 95 01 ff 00 03")   # ROOM6030 @0x00768
X0, X1, Z0, Z1 = -23080, -18840, -27221, -25221
FLAECHE_Y = -920
KAMERA_SATZ = 0x00180


def masken(blob, cut):
    """-> Liste (datei_off_tiefe, X, Y, w, h, tiefe) wie pruef_verdeckung.masken_des_cuts, mit dem Datei-Offset des Tiefen-Halbworts."""
    assert blob[:4] == b"R15M"
    _, nc = struct.unpack_from("<II", blob, 4)
    o = struct.unpack_from("<%dI" % nc, blob, 12)[cut]
    gc, _mc = struct.unpack_from("<HH", blob, o)
    p = o + 4
    gn, gdx, gdy = [], [], []
    for _ in range(gc):
        n_, _b, dx, dy = struct.unpack_from("<HHhh", blob, p)
        gn.append(n_); gdx.append(dx); gdy.append(dy); p += 8
    out, gi, used = [], 0, 0
    for _ in range(sum(gn)):
        dx, dy = blob[p + 2], blob[p + 3]
        dep_off = p + 4
        dep, size = struct.unpack_from("<HH", blob, p + 4); p += 8
        if (size & 0xf000) == 0:
            w, h = struct.unpack_from("<HH", blob, p); p += 4
        else:
            w = h = (size >> 12) * 8
        while gi < gc and used >= gn[gi]:
            gi += 1; used = 0
        ax, ay = (gdx[gi], gdy[gi]) if gi < gc else (0, 0)
        used += 1
        out.append((dep_off, ((dx + ax + 0x8000) & 0xffff) - 0x8000, ((dy + ay + 0x8000) & 0xffff) - 0x8000, w, h, dep))
    return out


def flaeche(R, t, H, sx, sy):
    vz = geom.vz_at_floor(R, t, H, sx, sy, FLAECHE_Y)
    if vz is None:
        return None
    wx, wz = geom.welt_am_boden(R, t, H, sx, sy, FLAECHE_Y)
    return wx, wz, vz


_VORWAERTS = {}


def flaeche_vorwaerts(R, t, H):
    """Liegeflaeche y -920 im Raster 20 -> (Bild-x, Bild-y, vz), vorwaerts projiziert wie kern()."""
    key = (tuple(R), tuple(t), H)
    if key not in _VORWAERTS:
        pts = []
        for wx in range(X0, X1 + 1, 20):
            for wz in range(Z0, Z1 + 1, 20):
                vz = (wx * R[6] + FLAECHE_Y * R[7] + wz * R[8]) / 4096.0 + t[2]
                vx = (wx * R[0] + FLAECHE_Y * R[1] + wz * R[2]) / 4096.0 + t[0]
                vy = (wx * R[3] + FLAECHE_Y * R[4] + wz * R[5]) / 4096.0 + t[1]
                pts.append((160 + vx * H / vz, 120 + vy * H / vz, vz))
        _VORWAERTS[key] = pts
    return _VORWAERTS[key]


def soll(room):
    rdt = open(os.path.join(PSX, "STAGE6", room + ".RDT"), "rb").read()
    if LIEGE_SCA not in rdt:
        raise SystemExit("%s: SCA-Satz der Liege fehlt" % room)
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    if cam + CUT * 32 != KAMERA_SATZ:
        raise SystemExit("%s: Kamera 9 liegt @0x%05X, erwartet @0x%05X" % (room, cam + CUT * 32, KAMERA_SATZ))
    R, t, H = geom.cut_view(rdt, cam, CUT)
    blob = open(os.path.join(MASKS, room + ".MSK"), "rb").read()
    erg = []
    for i, (off, x, y, w, h, dep) in enumerate(masken(blob, CUT)):
        pts = [flaeche(R, t, H, sx + 0.5, sy + 0.5) for sy in range(y, y + h) for sx in range(x, x + w)]
        auf = [p for p in pts if p and X0 <= p[0] <= X1 and Z0 <= p[1] <= Z1]
        raus = any(p is None or p[0] > X1 or not (Z0 <= p[1] <= Z1) for p in pts)
        drin = bool(auf) and not raus
        # groesstes vz der Flaeche im Rechteck aus der VORWAERTS-Projektion (wie die Figuren: vx/vz * H, ein Pixel deckt [sx, sx+1));
        # erster Bau nahm die Rueckrechnung der Pixelmitten und lag bis zu 13 vz zu nah (Riegel unit_r42_story6_t05: Kernpunkt
        # (84,172) vz 6081 in Rechteck 5, Rueckrechnung 6039/6068)
        im_rechteck = [v for (px, py, v) in flaeche_vorwaerts(R, t, H) if x <= px < x + w and y <= py < y + h]
        vmax = max([p[2] for p in auf] + im_rechteck) if auf else 0.0
        neu = dep
        if drin and dep * 64 < vmax:
            neu = int(math.ceil(vmax / 64.0))
        erg.append((i, off, x, y, w, h, dep, neu, drin, vmax))
    return blob, rdt, erg


# Marvins Kern wie die Konzept-Messung [B36] (belege/technik/werkzeug/marvin_pri_kern.py): Mitte (-21241,-920,-26118) = sein
# Sce_em_set, Raster +-300 in x/z (Schritt 100), Hoehe 0/-150/-300 ueber der Flaeche -> Bildpunkt + vz in Cut 9.
MARVIN = (-21241, -920, -26118)
TEST_INC = os.path.join(PORT, "tests", "unit", "gen_r42_liege_tiefe.inc")


def kern(R, t, H):
    pts = []
    for dx in range(-300, 301, 100):
        for dz in range(-300, 301, 100):
            for dy in (0, -150, -300):
                wx, wy, wz = MARVIN[0] + dx, MARVIN[1] + dy, MARVIN[2] + dz
                vz = (wx * R[6] + wy * R[7] + wz * R[8]) / 4096.0 + t[2]
                vx = (wx * R[0] + wy * R[1] + wz * R[2]) / 4096.0 + t[0]
                vy = (wx * R[3] + wy * R[4] + wz * R[5]) / 4096.0 + t[1]
                pts.append((int(160 + vx * H / vz), int(120 + vy * H / vz), int(vz)))
    return pts


def test_inc(erg, R, t, H):
    L = ["/* ERZEUGT von re15_port/tools/maske/liege_tiefe_r42.py --test-inc - NICHT von Hand aendern.",
         " * Runde 42 Story STAGE6 T-05: Soll-Tiefen der Cut-9-Rechtecke (MASKS/ROOM6030.MSK == ROOM6031.MSK) und Marvins Kern",
         " * (Bildpunkt, vz) in Cut 9 wie Konzept [B36]. Benutzt von tests/unit/test_r42_story6.c (Teil t05). */",
         "static const struct { int i; int tiefe_alt; int tiefe; } k_t05_soll[] = {"]
    for (i, off, x, y, w, h, dep, neu, drin, vmax) in erg:
        L.append("    { %2d, %3d, %3d },   /* rect (%d,%d %dx%d)%s */" % (i, dep, neu, x, y, w, h, " Liegeflaeche" if neu != dep else ""))
    L.append("};")
    L.append("static const struct { int sx, sy, vz; } k_t05_kern[] = {")
    for (sx, sy, vz) in kern(R, t, H):
        L.append("    { %d, %d, %d }," % (sx, sy, vz))
    L.append("};")
    open(TEST_INC, "w", encoding="utf-8", newline=chr(10)).write(chr(10).join(L) + chr(10))
    print("geschrieben", TEST_INC)


def main():
    a = sys.argv[1:]
    fehler = 0
    for room in RAEUME:
        blob, _rdt, erg = soll(room)
        if "--test-inc" in a and room == "ROOM6031":
            cam = struct.unpack_from("<I", _rdt, 0x24)[0]
            # Soll-Tabelle gegen die UNVERAENDERTE Datei (tiefe_alt) erzeugen: nur vor --schreiben sinnvoll; nach dem Schreiben
            # sind alt == neu. Deshalb liest --test-inc die Ausgangsdatei aus git (HEAD vor T-05), wenn sie angegeben ist.
            test_inc(erg, *geom.cut_view(_rdt, cam, CUT))
        geaendert = [e for e in erg if e[7] != e[6]]
        print("%s Cut %d: %d Rechtecke, %d ueber der Liegeflaeche, davon %d mit Tiefe vor der Flaeche" % (
            room, CUT, len(erg), sum(1 for e in erg if e[8]), len(geaendert)))
        for (i, off, x, y, w, h, dep, neu, drin, vmax) in geaendert:
            print("  #%2d @0x%05X rect (%3d,%3d %2dx%2d) Tiefe %3d (vz %5d) -> %3d (vz %5d), Flaeche bis vz %5.0f" % (
                i, off, x, y, w, h, dep, dep * 64, neu, neu * 64, vmax))
        if "--schreiben" in a:
            b = bytearray(blob)
            for (i, off, x, y, w, h, dep, neu, drin, vmax) in geaendert:
                struct.pack_into("<H", b, off, neu)
            open(os.path.join(MASKS, room + ".MSK"), "wb").write(bytes(b))
            print("  geschrieben")
        if "--check" in a:
            # nach dem Schreiben ist "geaendert" leer: jede Tiefe ueber der Flaeche liegt hinter ihr
            if geaendert:
                print("  CHECK: %d Rechtecke noch vor der Flaeche" % len(geaendert)); fehler += 1
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
