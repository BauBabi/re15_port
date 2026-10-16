#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_lift6030_cut.py — Runde 37 Spur C Nachbesserung 1 (AUFTRAG.md Nachtrag 5b): schneidet den Ton der
RE2-HUBPLATTFORM ("It's a switch for the lift", RE2 ROOM6030 msg 9) und baut daraus die Mini-VAB-Bank
LIFT6030.VBS fuer die zwei kleinen Aufzuege der STAGE2 (ROOM2030/2031 "TREATMENT POOL", Lift A obj 1 / Lift B
obj 2). Satzformat wie ELZASE.VBS / ELEVSE.VBS (tools/r36_l/re2_elza_se_cut.py).

⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL (Beta -> Retail, Sound ist RE2). RE1.5 ist hier stumm: ROOM2030 sub02 /
   sub04 (Fahrt Lift A / B) enthalten kein Se_on (0x36), ROOM2030/2031 hat ueberhaupt keins (Dossier N1-0).

RE2-VORBILD (alles selbst aus info/re2leon/PL0/RDT/ROOM6030.RDT gelesen; ROOMF030 = dieselbe Plattform):
  sub10 @0x02F8C `22 02 07 01` Set(2,7)=1, @0x02F90 `36 02 11 00 ..` Se_on(2,0x11) = KLICK (Schalter),
        @0x02F9C Sleep 10, Cut_chg 1, @0x02FBA `36 02 0b 00 ..` Se_on(2,0x0B) = FAHRT,
        @0x02FDC `0d 00 60 00 b4 00` For 180 (je 1 Evt_next): Plattform obj 0 + Spieler y += 30 = 5400 in 180 Bildern.
  sub12 @0x030B6 / @0x030BA / @0x030E4 / @0x03106: dieselbe Folge in Gegenrichtung.
  Raumbank (Bank 2 = snd0, RDT-Kopf +0x08/+0x0C/+0x10): EDT @0x0E128, VH @0x0E1E8, VB @0x0F008.
  Satz 0x11 EDT @0x0E16C `00 01 a3 00` -> Prog 1 Ton 10 (@0x0ED48) -> VAG 15 @0x22F08  2496 B (0,40 s Klick)
  Satz 0x0B EDT @0x0E154 `00 01 13 02` -> Prog 1 Ton 1  (@0x0EC28) -> VAG 22 @0x2F818 34320 B (7,88 s):
        Anlauf-Schlag (Spitze T6), gleichmaessige Fahrt, Stopp-Schlag ab T199 = 19 Bilder nach dem Fahrt-Ende T180.

DIE EINE PORT-WAHL (FORM aus den Daten, Dossier N1-2): Lift A faehrt in RE1.5 nur 90 Bilder (ROOM2030 sub02
  @0x01E54/@0x01E98 `0d 00 06 00 2d 00` For 45 mit 2x Evt_next), Lift B 180 (sub04 @0x01F2E/@0x01F72 For 90 x 2 =
  RE2-Laenge). Fuer Lift A gibt es in RE2 keine Welle; ungekuerzt liefe der Motor 3,6 s ueber die Ankunft hinaus.
  Satz FAHRT_KURZ = dieselbe RE2-Welle, aus deren GLEICHMAESSIGEM Mittelteil genau (180-90) Bilder herausgenommen:
  KUERZUNG = round(90 * Abtastwerte_je_Bild / 28) ADPCM-Bloecke (Abtastrate aus note2pitch2 der Ton-Bytes);
  Schnittstelle A = kleinste Abweichung des Decoder-Zustands (s1,s2) zwischen Block A-1 und Block A+KUERZUNG-1,
  gesucht nur zwischen Ende des Anlauf-Schlags und Beginn des Stopp-Schlags (beides aus der Huellkurve). Anlauf und
  Stopp-Schlag bleiben bytegleich, der Stopp-Schlag kommt so wie in RE2 19 Bilder nach dem Fahrt-Ende.

EINZIGE weitere Daten-Aenderungen (wie re2_elza_se_cut.py): Tone-Byte +0x16 (VAG-Index) und EDT-Byte 2 oberes
  Nibble (Ton im Programm) / Byte 1 (Programm 0) -> Lage in der Mini-Bank. vol/pan/center/shift/ADSR/Prioritaet/
  Stimme bytegleich.

Ausgabe (Standard = Ermittlungsstand): build/r37_fahrstuhl/lift6030/LIFT6030.VBS + re2_lift6030_bank.inc
Mit --install: re15_port/shared_assets/RE2/LIFT6030.VBS + re15_port/engine/src/gen/re2_lift6030_bank.inc
"""
import hashlib
import math
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

# --- selbst gelesene Anker in ROOM6030.RDT -------------------------------------------------------------------
SKRIPT = [   # (Datei-Offset, Rohbytes, was)
    (0x02F8C, "22020701", "sub10 Set(2,7)=1"),
    (0x02F90, "360211000000000000000000", "sub10 Se_on(2,0x11) KLICK"),
    (0x02FBA, "36020b000000000000000000", "sub10 Se_on(2,0x0B) FAHRT"),
    (0x02FDC, "0d006000b400", "sub10 For 180"),
    (0x030B6, "22020701", "sub12 Set(2,7)=1"),
    (0x030BA, "360211000000000000000000", "sub12 Se_on(2,0x11) KLICK"),
    (0x030E4, "36020b000000000000000000", "sub12 Se_on(2,0x0B) FAHRT"),
    (0x03106, "0d006000b400", "sub12 For 180"),
]
# (name, re2-satz, EDT-Rohsatz, EDT-Datei-Offset, Tone-Datei-Offset, VAG-Datei-Offset, VAG-Laenge, sha1, ROOMF030-VAG)
SAETZE = [
    ("KLICK", 0x11, "0001a300", 0x0E16C, 0x0ED48, 0x22F08, 2496,
     "b70bf75ffb44", 0x1CF68),
    ("FAHRT", 0x0B, "00011302", 0x0E154, 0x0EC28, 0x2F818, 34320,
     "efbad613e558", 0x1FDD8),
]
RE2_FAHRT_BILDER = 180      # ROOM6030 sub10 @0x02FDC For 0xB4, 1 Evt_next je Durchlauf
RE15_LIFT_A_BILDER = 90     # ROOM2030 sub02 @0x01E54 For 0x2D, 2 Evt_next je Durchlauf (@0x01E5B/@0x01E5C)

VAB_HDR = 32
VAB_PROG_TABLE = 128 * 16
VAB_TONE_SEG = 16 * 32
VAB_SIZE_TABLE = 256 * 2
VH_SIZE = VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + VAB_SIZE_TABLE   # nprog = 1
SE_MAP_BYTES = 0x10
TRAILER = 8

# note2pitch2-LUT DAT_80077520 (engine/src/vab_common.c s_pitch_lut, byte-extrahiert)
LUT = [
    [0x1000, 0x100E, 0x101D, 0x102C, 0x103B, 0x104A, 0x1059, 0x1068, 0x1078, 0x1087, 0x1096, 0x10A5, 0x10B5, 0x10C4, 0x10D4, 0x10E3],
    [0x10F3, 0x1103, 0x1113, 0x1122, 0x1132, 0x1142, 0x1152, 0x1162, 0x1172, 0x1182, 0x1193, 0x11A3, 0x11B3, 0x11C4, 0x11D4, 0x11E5],
    [0x11F5, 0x1206, 0x1216, 0x1227, 0x1238, 0x1249, 0x125A, 0x126B, 0x127C, 0x128D, 0x129E, 0x12AF, 0x12C1, 0x12D2, 0x12E3, 0x12F5],
    [0x1306, 0x1318, 0x132A, 0x133C, 0x134D, 0x135F, 0x1371, 0x1383, 0x1395, 0x13A7, 0x13BA, 0x13CC, 0x13DE, 0x13F1, 0x1403, 0x1416],
    [0x1428, 0x143B, 0x144E, 0x1460, 0x1473, 0x1486, 0x1499, 0x14AC, 0x14BF, 0x14D3, 0x14E6, 0x14F9, 0x150D, 0x1520, 0x1534, 0x1547],
    [0x155B, 0x156F, 0x1583, 0x1597, 0x15AB, 0x15BF, 0x15D3, 0x15E7, 0x15FB, 0x1610, 0x1624, 0x1638, 0x164D, 0x1662, 0x1676, 0x168B],
    [0x16A0, 0x16B5, 0x16CA, 0x16DF, 0x16F4, 0x170A, 0x171F, 0x1734, 0x174A, 0x175F, 0x1775, 0x178B, 0x17A1, 0x17B6, 0x17CC, 0x17E2],
    [0x17F9, 0x180F, 0x1825, 0x183B, 0x1852, 0x1868, 0x187F, 0x1896, 0x18AC, 0x18C3, 0x18DA, 0x18F1, 0x1908, 0x191F, 0x1937, 0x194E],
    [0x1965, 0x197D, 0x1995, 0x19AC, 0x19C4, 0x19DC, 0x19F4, 0x1A0C, 0x1A24, 0x1A3C, 0x1A55, 0x1A6D, 0x1A85, 0x1A9E, 0x1AB7, 0x1ACF],
    [0x1AE8, 0x1B01, 0x1B1A, 0x1B33, 0x1B4C, 0x1B66, 0x1B7F, 0x1B98, 0x1BB2, 0x1BCC, 0x1BE5, 0x1BFF, 0x1C19, 0x1C33, 0x1C4D, 0x1C67],
    [0x1C82, 0x1C9C, 0x1CB7, 0x1CD1, 0x1CEC, 0x1D07, 0x1D22, 0x1D3D, 0x1D58, 0x1D73, 0x1D8E, 0x1DA9, 0x1DC5, 0x1DE0, 0x1DFC, 0x1E18],
    [0x1E34, 0x1E50, 0x1E6C, 0x1E88, 0x1EA4, 0x1EC1, 0x1EDD, 0x1EFA, 0x1F16, 0x1F33, 0x1F50, 0x1F6D, 0x1F8A, 0x1FA7, 0x1FC5, 0x1FE2],
]
# PSX-ADPCM-Filter (psx-spx SPU ADPCM)
FILT = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]


def die(msg):
    sys.stderr.write("re2_lift6030_cut: FEHLER: %s\n" % msg)
    sys.exit(1)


def note2pitch2(note, fine, center, shift):
    """Wie vab_common.c re15_vab_note2pitch2 (Aufruf audio_pc.c: (min, shift, center, shift))."""
    s = ((fine & 0xFFFF) + (shift & 0xFF)) >> 3
    frac, carry = s, 0
    if s > 15:
        frac, carry = s - 16, 1
    sem = carry + note + 0x3C - center
    octv = int(sem / 12) - 5
    idx = max(0, min(191, (sem % 12) * 16 + frac))
    p = LUT[idx // 16][idx % 16]
    if octv < 0:
        p >>= (-octv) & 31
    elif octv >= 1:
        p = 0x3FFF
    return p & 0xFFFF


def bloecke(w):
    """Je 16-B-Block: (Abtastwerte, s1, s2) bei durchgehendem Decoder-Zustand."""
    out, s1, s2 = [], 0, 0
    for b in range(0, len(w), 16):
        blk = w[b:b + 16]
        sh, fi = blk[0] & 15, (blk[0] >> 4) & 15
        f0, f1 = FILT[fi] if fi < 5 else (0, 0)
        smp = []
        for i in range(28):
            byte = blk[2 + i // 2]
            nib = (byte >> 4) if (i & 1) else (byte & 15)
            if nib & 8:
                nib -= 16
            s = (nib << 12) >> sh
            s += (s1 * f0 + s2 * f1 + 32) >> 6
            s = max(-32768, min(32767, s))
            s2, s1 = s1, s
            smp.append(s)
        out.append((smp, s1, s2))
        if (blk[1] & 1) and not (blk[1] & 2):
            break
    return out


def huelle(samples, je_bild):
    """RMS je Spielbild (1/30 s) der dekodierten Welle."""
    n = int(len(samples) / je_bild)
    r = []
    for k in range(n):
        seg = samples[int(k * je_bild):int((k + 1) * je_bild)]
        r.append(math.sqrt(sum(x * x for x in seg) / len(seg)) if seg else 0.0)
    return r


def schlaege(rms):
    """Anlauf-Ende und Stopp-Beginn (in Bildern) aus der Huellkurve: M = Median der Fahrt T30..T150;
    Anlauf-Ende = erstes Bild nach der Anlauf-Spitze (Maximum T0..T29) mit RMS <= M;
    Stopp-Beginn = erstes Bild nach T150 mit RMS > 2*M."""
    mitte = sorted(rms[30:150])[len(rms[30:150]) // 2]
    spitze = max(range(30), key=lambda k: rms[k])
    anlauf_ende = next(k for k in range(spitze, len(rms)) if rms[k] <= mitte)
    stopp = next(k for k in range(150, len(rms)) if rms[k] > 2 * mitte)
    return mitte, spitze, anlauf_ende, stopp


def main():
    install = "--install" in sys.argv
    if install:
        out_vbs = os.path.join(REPO, "re15_port", "shared_assets", "RE2", "LIFT6030.VBS")
        out_inc = os.path.join(REPO, "re15_port", "engine", "src", "gen", "re2_lift6030_bank.inc")
    else:
        od = os.path.join(REPO, "build", "r37_fahrstuhl", "lift6030")
        out_vbs = os.path.join(od, "LIFT6030.VBS")
        out_inc = os.path.join(od, "re2_lift6030_bank.inc")
    os.makedirs(os.path.dirname(out_vbs), exist_ok=True)
    os.makedirs(os.path.dirname(out_inc), exist_ok=True)

    p = os.path.join(RDT, "ROOM6030.RDT")
    pf = os.path.join(RDT, "ROOMF030.RDT")
    if not os.path.isfile(p):
        die("Quelle fehlt: %s" % p)
    a = open(p, "rb").read()
    f030 = open(pf, "rb").read() if os.path.isfile(pf) else None
    edt, vh, vb = struct.unpack_from("<III", a, 8)
    if (edt, vh, vb) != (0x0E128, 0x0E1E8, 0x0F008) or a[vh:vh + 4] != b"pBAV":
        die("ROOM6030 Kopf weicht ab: EDT 0x%X VH 0x%X VB 0x%X" % (edt, vh, vb))
    for off, roh, was in SKRIPT:
        got = a[off:off + len(roh) // 2].hex()
        if got != roh:
            die("ROOM6030 @0x%05X (%s): %s != %s" % (off, was, got, roh))
    nprog = struct.unpack_from("<H", a, vh + 0x12)[0]
    sizetab = vh + 0x820 + nprog * 0x200

    wellen, toene, saetze, zeilen = [], [], [], []
    for (name, sid, edt_hex, edt_off, tone_off, vag_off, vag_len, sha, f_off) in SAETZE:
        if edt + sid * 4 != edt_off:
            die("Satz 0x%02X: EDT-Offset 0x%X != 0x%X" % (sid, edt + sid * 4, edt_off))
        raw = a[edt_off:edt_off + 4]
        if raw.hex() != edt_hex:
            die("Satz 0x%02X @0x%X: %s != %s" % (sid, edt_off, raw.hex(), edt_hex))
        prog, tone = raw[1] & 0x7F, raw[2] >> 4
        if vh + 0x820 + prog * 0x200 + tone * 0x20 != tone_off:
            die("Satz 0x%02X: Tone-Offset weicht ab" % sid)
        t = bytearray(a[tone_off:tone_off + 32])
        vagi = struct.unpack_from("<H", t, 0x16)[0]
        pos = vb
        for k in range(0, vagi):
            pos += struct.unpack_from("<H", a, sizetab + k * 2)[0] << 3
        vlen = struct.unpack_from("<H", a, sizetab + vagi * 2)[0] << 3
        if (pos, vlen) != (vag_off, vag_len):
            die("Satz 0x%02X: VAG %d @0x%X/%d B statt @0x%X/%d B" % (sid, vagi, pos, vlen, vag_off, vag_len))
        wav = a[vag_off:vag_off + vag_len]
        h = hashlib.sha1(wav).hexdigest()
        if not h.startswith(sha):
            die("Satz 0x%02X: sha1 %s" % (sid, h))
        if f030 is not None and f030[f_off:f_off + vag_len] != wav:
            die("Gegenprobe ROOMF030 @0x%X: Welle 0x%02X nicht bitgleich" % (f_off, sid))
        wellen.append(wav); toene.append(t); saetze.append(raw)
        zeilen.append((name, sid, raw, tone_off, vagi, vag_off, vag_len, h))

    # --- FAHRT_KURZ: Fahrt-Welle um (180-90) Bilder im gleichmaessigen Mittelteil kuerzen ----------------------
    t_fahrt = toene[1]
    pitch = note2pitch2(t_fahrt[6], t_fahrt[5], t_fahrt[4], t_fahrt[5])
    rate = 44100.0 * pitch / 4096.0
    je_bild = rate / 30.0
    bl = bloecke(wellen[1])
    smp = [s for b in bl for s in b[0]]
    rms = huelle(smp, je_bild)
    mitte, spitze, anlauf_ende, stopp = schlaege(rms)
    kuerzung = int(round((RE2_FAHRT_BILDER - RE15_LIFT_A_BILDER) * je_bild / 28.0))
    lo = int(math.ceil(anlauf_ende * je_bild / 28.0))
    hi = int(stopp * je_bild / 28.0) - kuerzung          # A + kuerzung <= Block des Stopp-Beginns
    best = None
    for A in range(max(lo, 1), hi + 1):
        B = A + kuerzung
        mm = abs(bl[A - 1][1] - bl[B - 1][1]) + abs(bl[A - 1][2] - bl[B - 1][2])
        if best is None or mm < best[0]:
            best = (mm, A, B)
    if best is None:
        die("kein Schnitt im Fenster")
    mm, A, B = best
    kurz = wellen[1][:A * 16] + wellen[1][B * 16:]
    # Gegenmessung an der gekuerzten Welle: Stopp-Schlag-Lage
    rms_k = huelle([s for b in bloecke(kurz) for s in b[0]], je_bild)
    schnitt_bild = int(A * 28 / je_bild)                 # ab der Schnittstelle (Median M der langen Welle)
    stopp_k = next(k for k in range(schnitt_bild, len(rms_k)) if rms_k[k] > 2 * mitte)
    print("  Fahrt-Welle: pitch 0x%X = %.1f Hz, %.2f Abtastwerte/Bild; Huellkurve Median %.0f, Anlauf-Spitze T%d, "
          "Anlauf-Ende T%d, Stopp-Schlag T%d (= %d Bilder nach Fahrt-Ende T%d)"
          % (pitch, rate, je_bild, mitte, spitze, anlauf_ende, stopp, stopp - RE2_FAHRT_BILDER, RE2_FAHRT_BILDER))
    print("  FAHRT_KURZ: %d Bloecke = %.2f Bilder heraus, Schnitt Block %d|%d (Fenster %d..%d), Zustands-Abweichung %d;"
          " Stopp-Schlag T%d (= %d Bilder nach Fahrt-Ende T%d)"
          % (kuerzung, kuerzung * 28.0 / je_bild, A, B, lo, hi, mm, stopp_k, stopp_k - RE15_LIFT_A_BILDER,
             RE15_LIFT_A_BILDER))
    if stopp_k - RE15_LIFT_A_BILDER != stopp - RE2_FAHRT_BILDER:
        die("gekuerzte Welle: Stopp-Schlag %d statt %d Bilder nach dem Fahrt-Ende"
            % (stopp_k - RE15_LIFT_A_BILDER, stopp - RE2_FAHRT_BILDER))
    wellen.append(kurz); toene.append(bytearray(t_fahrt)); saetze.append(saetze[1])
    zeilen.append(("FAHRT_KURZ", 0x0B, saetze[1], SAETZE[1][4], -1, SAETZE[1][5], len(kurz),
                   hashlib.sha1(kurz).hexdigest()))

    # --- Mini-Bank -----------------------------------------------------------------------------------------
    se_map = bytearray(SE_MAP_BYTES)
    vh_out = bytearray(VH_SIZE)
    vbd = bytearray()
    for i, (wav, t, raw) in enumerate(zip(wellen, toene, saetze)):
        t = bytearray(t)
        struct.pack_into("<H", t, 0x14, 0)                            # Ton gehoert zu Programm 0
        struct.pack_into("<H", t, 0x16, i + 1)                        # VAG-Index in der Mini-Bank
        vh_out[VAB_HDR + VAB_PROG_TABLE + i * 32:VAB_HDR + VAB_PROG_TABLE + (i + 1) * 32] = t
        rec = bytearray(raw)
        rec[1] = 0
        rec[2] = ((i & 0xF) << 4) | (raw[2] & 0x0F)
        se_map[i * 4:i * 4 + 4] = rec
        struct.pack_into("<H", vh_out, VAB_HDR + VAB_PROG_TABLE + VAB_TONE_SEG + 2 * (i + 1), len(wav) // 8)
        vbd += wav
    vh_out[0:4] = b"pBAV"
    vh_out[4:12] = a[vh + 4:vh + 12]
    struct.pack_into("<I", vh_out, 12, VH_SIZE + len(vbd))
    vh_out[16:18] = a[vh + 16:vh + 18]
    struct.pack_into("<H", vh_out, 18, 1)
    struct.pack_into("<H", vh_out, 20, 16)
    struct.pack_into("<H", vh_out, 22, len(wellen))
    vh_out[24:32] = a[vh + 24:vh + 32]
    vh_out[VAB_HDR:VAB_HDR + 16] = a[vh + VAB_HDR + 16:vh + VAB_HDR + 32]   # Programm 1 von ROOM6030
    vh_out[VAB_HDR] = len(wellen)
    trailer = struct.pack("<II", SE_MAP_BYTES, 0)
    edt_rec = bytes(se_map) + bytes(vh_out) + trailer
    datei = edt_rec + bytes(vbd)
    open(out_vbs, "wb").write(datei)
    fnv = 2166136261
    for c in datei:
        fnv = ((fnv ^ c) * 16777619) & 0xFFFFFFFF

    with open(out_inc, "w", newline="\n") as o:
        o.write("/* AUTO-GENERATED by re15_port/tools/fahrstuhl_r37/re2_lift6030_cut.py -- DO NOT EDIT.\n"
                " * Satz-TOC der Mini-Bank shared_assets/RE2/LIFT6030.VBS (Runde 37 Spur C N1, RE2-Ergaenzung:\n"
                " * Ton der RE2-Hubplattform ROOM6030 fuer die STAGE2-Aufzuege ROOM2030/2031).\n"
                " * Satzformat wie ELZASE.VBS: [SE-Map @0 .. vh_off)[VH pBAV @vh_off][Trailer, u32 vh_off @size-8][VBD].\n"
                " * Herkunft (RE2 info/re2leon/PL0/RDT/ROOM6030.RDT, Datei-Offsets; ROOMF030 bitgleich):\n")
        for (name, sid, raw, tone_off, vagi, vag_off, vag_len, h) in zeilen:
            o.write(" *   %-10s Satz 0x%02X  EDT %s  Tone @0x%05X  VAG @0x%05X %5d B  sha1 %s\n"
                    % (name, sid, raw.hex(" "), tone_off, vag_off, vag_len, h))
        o.write(" * FAHRT_KURZ = FAHRT ohne die Bloecke [%d,%d) (%d Bloecke = %d Fahrt-Bilder: RE2 For 180 @0x02FDC\n"
                " * gegen RE1.5 Lift A For 45 x 2 @0x01E54); Stopp-Schlag %d Bilder nach dem Fahrt-Ende wie RE2.\n"
                " */\n" % (A, B, kuerzung, RE2_FAHRT_BILDER - RE15_LIFT_A_BILDER, stopp - RE2_FAHRT_BILDER))
        o.write("#define RE2_LIFT_EDT_OFF   0u\n")
        o.write("#define RE2_LIFT_EDT_SIZE  %du\n" % len(edt_rec))
        o.write("#define RE2_LIFT_VBD_OFF   %du\n" % len(edt_rec))
        o.write("#define RE2_LIFT_VBD_SIZE  %du\n" % len(vbd))
        o.write("#define RE2_LIFT_DATEI_GROESSE %du\n" % len(datei))
        o.write("#define RE2_LIFT_DATEI_FNV  0x%08Xu\n" % fnv)
        o.write("#define RE2_LIFT_SE_COUNT  %d\n" % len(wellen))
        o.write("#define RE2_LIFT_SE_KLICK       0   /* RE2 ROOM6030 Satz 0x11, %d B */\n" % len(wellen[0]))
        o.write("#define RE2_LIFT_SE_FAHRT       1   /* RE2 ROOM6030 Satz 0x0B, %d B */\n" % len(wellen[1]))
        o.write("#define RE2_LIFT_SE_FAHRT_KURZ  2   /* Satz 0x0B gekuerzt, %d B */\n" % len(wellen[2]))
        o.write("#define RE2_LIFT_FAHRT_PITCH    0x%Xu  /* note2pitch2 der Ton-Bytes @0x0EC28 */\n" % pitch)
        o.write("#define RE2_LIFT_STOPP_NACH_FAHRT %d  /* Bilder: Stopp-Schlag T%d - RE2-Fahrt-Ende T%d */\n"
                % (stopp - RE2_FAHRT_BILDER, stopp, RE2_FAHRT_BILDER))

    for (name, sid, raw, tone_off, vagi, vag_off, vag_len, h) in zeilen:
        print("  %-10s RE2 0x%02X EDT %s Tone @0x%05X VAG %d @0x%05X %d B sha1 %s" % (
            name, sid, raw.hex(" "), tone_off, vagi, vag_off, vag_len, h[:12]))
    print("  LIFT6030.VBS: edt_size=%d vbd_size=%d gesamt=%d B FNV 0x%08X sha1 %s"
          % (len(edt_rec), len(vbd), len(datei), fnv, hashlib.sha1(datei).hexdigest()))
    print("  -> %s" % out_vbs)
    print("  -> %s" % out_inc)
    return 0


if __name__ == "__main__":
    sys.exit(main())
