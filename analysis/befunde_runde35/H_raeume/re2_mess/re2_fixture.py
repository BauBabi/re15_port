"""re2_fixture.py - erzeugt re15_port/tests/unit/r35_raeume_re2orig.inc aus den RE2-ORIGINAL-Mitschnitten
(daten/<lauf>/frames.bin, DuckStation-GDB, re2_gdb_grab.py). Je Lauf die 19 Halte-Bilder 16..34 (Ueberblendung
+0x14E abgelaufen): Leon-Pin/-Blick/-Clipwort, Arm-Ursprung/-Blick/-Clipwort/-Variante (+0x10E & 1), die
Part-Welt-Matrizen (+0x48 m[3][3] s16, +0x5C t s32) von Leons Part 0 (Rumpf) und 8 (Kopf) und von Unterarm/Hand
des Halters (Bone Hand-1/Hand, Hand = 3 Arm A / 10 Arm B, Tabelle @0x80101414).
Nachbesserung 4: dazu je Bild Leons Blick-Akku Part 8 +0x98/+0x9A (auf -2048..2047 gefaltet, die RAM traegt ihn
ungefaltet) und das Blickziel PL+0x1B8 als Satz-Nummer (-1 = SELBST; Entity-Adresse -> Satz ueber die Halter-
Adressen aller Laeufe, derselbe Spielstand), je Lauf das erste Halte-Bild 0..39 mit Ziel != SELBST (-1 = keins) = Bild
der ersten Zielwahl FUN_8003DB38 im Griff; dazu die zehn Arm-Saetze aus ROOM2050.RDT @0x1970 + n*0x16 (x/y/z/Blick s16
an +10/+12/+14/+16, info/re2leon/PL0/RDT/ROOM2050.RDT).
Aufruf: python re2_fixture.py   (Runde 35 Spur H, Nachbesserung 3/4)"""
import os, struct
import re2_frames as F

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(HERE, "..", "..", "..", "..", "re15_port", "tests", "unit", "r35_raeume_re2orig.inc")
LAEUFE = [  # (Ordner, Satz, Hoehe, Griff 0 = Gesicht / 1 = Ruecken)
    ("g1_ost_gesicht", 9, -2160, 0), ("g2_ost_ruecken", 9, -2160, 1),
    ("g3_west_gesicht", 2, -2180, 0), ("g4_west_ruecken", 2, -2180, 1),
    ("g5_r5_gesicht", 5, -2480, 0), ("g6_r5_ruecken", 5, -2480, 1),
    ("g7_r0_gesicht", 0, -2580, 0), ("g8_r0_ruecken", 0, -2580, 1),
    ("g9_r8_gesicht", 8, -2540, 0),
    ("g11_r7_gesicht", 7, -2700, 0), ("g12_r3_gesicht", 3, -1930, 0), ("g13_r6_gesicht", 6, -2000, 0),
]
B0, NB = 16, 19
PL = 0x800CFBF8


def falte(v):
    return ((v + 2048) & 0xfff) - 2048


ADR = {}   # Entity-Adresse -> Satz (aus den Haltern aller Laeufe)


def ziel(f):
    t = struct.unpack_from("<I", f["pl"], 0x1b8)[0]
    return -1 if t == PL else ADR.get(t, 99)


def mat(blob, k):
    m, t = F.part(blob, k)
    return "{{%s},{%s}}" % (",".join(str(x) for x in m), ",".join(str(x) for x in t))


lines = ["/* r35_raeume_re2orig.inc - ERZEUGT von analysis/befunde_runde35/H_raeume/re2_mess/re2_fixture.py aus den",
         " * RE2-ORIGINAL-Mitschnitten ROOM2050 (DuckStation-GDB, Spielstand SLUS-00748_5.sav, EXE = info/re2leon/PSX.EXE,",
         " * Code-Abgleich 61440/61440 Worte). Je Lauf Halte-Bilder %d..%d (nach der +0x14E-Ueberblendung)." % (B0, B0 + NB - 1),
         " * Matrizen = Part +0x48 (m[3][3] Q12) / +0x5C (t) - Pin-Quelle @0x80100C18-38, geschrieben vom Zeichnen.",
         " * NB4: Blick-Akku Part8 +0x98/+0x9A (gefaltet), Ziel PL+0x1B8 als Satz (-1 SELBST), Lauf.wechsel, Arm-Saetze.",
         " * NICHT VON HAND AENDERN. */",
         "#define R2O_LAEUFE %d" % len(LAEUFE), "typedef char r2o_bilder_pruefung[(R2O_BILDER == %d) ? 1 : -1];" % NB, "static const r2o_lauf_t s_r2o[R2O_LAEUFE] = {"]
for name, satz, hoehe, griff in LAEUFE:
    ADR[F.frames(os.path.join(HERE, "daten", name, "frames.bin"))[0]["holder"]] = satz
rdt = open(os.path.join(HERE, "..", "..", "..", "..", "info", "re2leon", "PL0", "RDT", "ROOM2050.RDT"), "rb").read()
lines.insert(-1, "static const int16_t s_r2o_saetze[10][4] = {   /* ROOM2050.RDT @0x1970 + n*0x16: x, y, z, Blick */")
for n in range(10):
    lines.insert(-1, "  { %d,%d,%d,%d }," % struct.unpack_from("<4h", rdt, 0x1970 + n * 0x16 + 10))
lines.insert(-1, "};")
for name, satz, hoehe, griff in LAEUFE:
    fr = F.frames(os.path.join(HERE, "daten", name, "frames.bin"))
    he = fr[B0]["hent"]
    var = F.ent_f10e(he) & 1
    hb = 10 if var else 3
    wechsel = next((i for i, f in enumerate(fr) if ziel(f) != -1), -1)
    lines.append("  { \"%s\", %d, %d, %d, %d, %d, {" % (name, satz, hoehe, griff, var, wechsel))
    for f in fr[B0:B0 + NB]:
        pl, h = f["pl"], f["hent"]
        px, py, pz = F.ent_pos(pl); ax, ay, az = F.ent_pos(h)
        ny, npi = struct.unpack_from("<2h", f["lparts"], 8 * F.PART_SZ + 0x98)
        lines.append("    { %d,%d,%d,%uu, %d,%d,%d,%d,%uu, %s,%s,%s,%s, %d,%d,%d }," % (
            px, pz, F.ent_yaw(pl), F.ent_cw(pl), ax, ay, az, F.ent_yaw(h), F.ent_cw(h),
            mat(f["lparts"], 0), mat(f["lparts"], 8), mat(f["hparts"], hb - 1), mat(f["hparts"], hb),
            falte(ny), falte(npi), ziel(f)))
    lines.append("  } },")
lines.append("};")
open(OUT, "w", newline="\n").write("\n".join(lines) + "\n")
print("geschrieben:", os.path.normpath(OUT), os.path.getsize(OUT), "Bytes")
