#!/usr/bin/env python3
"""re2_schnitt9.py - Runde 41 G5-Auftritt: RE2 room7040 Schnitt 9 (Grossaufnahme der offenen Wagentuer) fuer den Port schneiden.

Quelle (RE2 Leon, info/re2leon):
  COMMON/BSS/ROOM704.BSS   Schnitt n = Datei[n*0x10000 : (n+1)*0x10000] (15 Schnitte, 983040 Byte; Kopf je Chunk
                           `xx xx 00 38 01 00 03 00` = derselbe MDEC-Kopf wie RE1.5, Port-Decoder bss_common.c)
  PL0/RDT/ROOM7040.RDT     RID @0x64 (Offset-Tabelle [7]), Schnitt 9 @0x184; sprite.pri-Abschnitt von Schnitt 9 ab dem Zeiger
                           im RID-Satz (+0x1C = 0xDD8) bis zum Zeiger von Schnitt 10 (0xF2C)
  COMMON/BIN/STAGE7.BIN    SLD-Laengentabelle @0x54 (Zeiger 0x8011a054 -> 0x800cfae8, Ladeadresse 0x8011a000), Zeile 4 = room704,
                           Spalte 9 @0xE6 = 0xCF40 (hier geprueft; der Port laedt den hier entpackten Atlas)

Ziel (Paket-Gate: neue RE2-Dateien):
  re15_port/shared_assets/RE2/R704_CUT09.BSS     (65536 Byte, unveraendert)
  re15_port/shared_assets/RE2/R704_CUT09.PRI     (340 Byte, unveraendert)
  re15_port/shared_assets/RE2/R704_CUT10.BSS     (65536 Byte, unveraendert; Schnitt 10, keine Masken)
  re15_port/shared_assets/RE2/R704_CUT10.PRI     (4 Byte 0xFFFFFFFF = leerer Abschnitt, unveraendert)
  re15_port/shared_assets/RE2/R704_CUT09_PRI.TIM (Masken-Atlas, entpackt aus dem Chunk mit RE2s eigenem Verfahren, s. entpacken())

RE2 packt den Masken-Atlas ANDERS als RE1.5 (RE1.5 FUN_800c47e8 / Port sld_common.c scheitert am RE2-Strom mit "Rueckverweis vor
den Pufferanfang"). RE2 FUN_8002bdf4 @0x8002bea0: Zeile/Spalte aus der Tabelle 0x800cfae8, `chunk[L-4] != 0`, Block ab chunk[L-8];
FUN_80010000 setzt den Lesezeiger auf Scratchpad 0x1f800006 (Kopf: u32 entpackte Groesse, u16), FUN_80010084 entpackt in 0x400-Byte-
Schritten (Zustandsmaschine @0x80010170), FUN_80076a40 laedt das TIM.

Aufruf: python re15_port/tools/r41_g5auftritt/re2_schnitt9.py   (reproduzierbar; prueft die Ziel-Bytes gegen die Quelle)
"""
import hashlib
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
REPO = os.path.dirname(PORT)
RE2 = os.path.join(REPO, "info", "re2leon")
ZIEL = os.path.join(PORT, "shared_assets", "RE2")

SCHNITT = 9
CHUNK = 0x10000
RID_OFF_INDEX = 7          # RDT-Kopf: 8 Byte Zaehler, dann 23 Offsets; [7] = RID
SLD_TABELLE = 0x54         # STAGE7.BIN
SLD_ZEILE = 4              # room704
SLD_ERWARTET = 0xCF40


def lesen(*teile):
    with open(os.path.join(RE2, *teile), "rb") as f:
        return f.read()


def entpacken(blk):
    """RE2 FUN_80010084 (Zustandsmaschine, PSX.EXE), byte-gleich zu reevengi bsssld_depack_re2 (info/Resident_Evil_und_Playstation_
    Information/revengi/src/depack_bsssld.c):
      @0x800101a4 lbu s4 / @0x800101ac andi 0x10 / bnez -> Literal-Zweig; sonst Rueckverweis:
      @0x8001028c-9c Versatz = ((s4 & 0xE0) | 0xFFFFFF00) << 3 | naechstes Byte; @0x800102a4 Laenge = s4 & 0xF, bei 0xF + naechstes Byte
      (@0x800102fc-304), @0x8001030c + 3; Kopie vorwaerts byteweise (@0x80010560).
      @0x80010200-04 Literal-Zweig: Byte 0xFF = Ende; @0x800103ac-b4 Laenge = ((s4 | 0xFFE0) ^ 0xFFFF) + 1, bei 0x10 + naechstes Byte
      (@0x800103b8-404); dann Rohbytes.
    Strom ab +6 (FUN_80010000: 0x1f800006)."""
    groesse = struct.unpack_from("<I", blk, 0)[0]
    aus = bytearray()
    i = 6
    while i < len(blk) and len(aus) < groesse:
        while blk[i] & 0x10 == 0:
            n = blk[i] & 0x0F
            versatz = (((blk[i] & 0xE0) | -256) << 3) | blk[i + 1]
            if n == 0x0F:
                n += blk[i + 2]
                i += 3
            else:
                i += 2
            n += 3
            q = len(aus) + versatz
            if q < 0:
                raise ValueError("Rueckverweis vor den Anfang bei %d" % i)
            for k in range(n):
                aus.append(aus[q + k])
        if blk[i] == 0xFF:
            break
        n = ((blk[i] | 0xFFE0) ^ 0xFFFF) + 1
        i += 1
        if n == 0x10:
            n += blk[i]
            i += 1
        aus += blk[i:i + n]
        i += n
    return bytes(aus[:groesse]), groesse


def main():
    bss = lesen("COMMON", "BSS", "ROOM704.BSS")
    rdt = lesen("PL0", "RDT", "ROOM7040.RDT")
    st7 = lesen("COMMON", "BIN", "STAGE7.BIN")

    if len(bss) != 15 * CHUNK:
        sys.exit("ROOM704.BSS: %d Byte, erwartet 15 Schnitte a 0x10000" % len(bss))
    chunk = bss[SCHNITT * CHUNK:(SCHNITT + 1) * CHUNK]
    if chunk[2:8] != b"\x00\x38\x01\x00\x03\x00":
        sys.exit("Schnitt 9: unerwarteter MDEC-Kopf %s" % chunk[:8].hex())

    rid = struct.unpack_from("<23I", rdt, 8)[RID_OFF_INDEX]
    satz = rdt[rid + SCHNITT * 32: rid + SCHNITT * 32 + 32]
    flag, fov, ax, ay, az, zx, zy, zz, pri9 = struct.unpack("<HHiiiiiiI", satz)
    pri10 = struct.unpack_from("<I", rdt, rid + (SCHNITT + 1) * 32 + 28)[0]
    pri = rdt[pri9:pri10]
    gruppen, masken = struct.unpack_from("<HH", pri, 0)

    sld = struct.unpack_from("<H", st7, SLD_TABELLE + SLD_ZEILE * 0x20 + SCHNITT * 2)[0]
    if sld != SLD_ERWARTET:
        sys.exit("STAGE7.BIN @0x%X: SLD-Laenge 0x%X statt 0x%X" % (SLD_TABELLE + SLD_ZEILE * 0x20 + SCHNITT * 2, sld, SLD_ERWARTET))
    vorhanden, sld_off = struct.unpack_from("<I", chunk, sld - 4)[0], struct.unpack_from("<I", chunk, sld - 8)[0]
    entpackt = struct.unpack_from("<I", chunk, sld_off)[0]

    atlas, groesse = entpacken(chunk[sld_off:])
    if len(atlas) != groesse or atlas[:8] != bytes([0x10, 0, 0, 0, 0x09, 0, 0, 0]):
        sys.exit("Atlas: %d von %d Byte, Kopf %s (kein 8-bpp-TIM mit CLUT)" % (len(atlas), groesse, atlas[:8].hex()))
    ref = os.path.join(RE2, "COMMON", "BSS", "ROOM704", "ROOM7040009.tim")
    if os.path.exists(ref):
        with open(ref, "rb") as f:
            r = f.read()
        print("Gegenprobe ROOM7040009.tim (fremdes Extraktionswerkzeug): erste %d Byte %s" %
              (groesse, "gleich" if r[:groesse] == atlas else "VERSCHIEDEN"))

    # Schnitt 10: Chunk und (leerer) PRI-Abschnitt, kein Atlas (STAGE7.BIN @0xE8 -> chunk[L-4] = 0)
    chunk10 = bss[10 * CHUNK:11 * CHUNK]
    pri10a = struct.unpack_from("<I", rdt, rid + 10 * 32 + 28)[0]
    pri11a = struct.unpack_from("<I", rdt, rid + 11 * 32 + 28)[0]
    pri_s10 = rdt[pri10a:pri11a]
    sld10 = struct.unpack_from("<H", st7, SLD_TABELLE + SLD_ZEILE * 0x20 + 10 * 2)[0]
    if pri_s10 != bytes([0xFF] * 4) or struct.unpack_from("<I", chunk10, sld10 - 4)[0] != 0:
        sys.exit("Schnitt 10: Masken erwartet keine")
    print("RID Schnitt 10 @0x%X: %s" % (rid + 10 * 32, rdt[rid + 10 * 32:rid + 11 * 32].hex(" ")))

    os.makedirs(ZIEL, exist_ok=True)
    for name, daten in (("R704_CUT09.BSS", chunk), ("R704_CUT09.PRI", pri), ("R704_CUT09_PRI.TIM", atlas),
                        ("R704_CUT10.BSS", chunk10), ("R704_CUT10.PRI", pri_s10)):
        with open(os.path.join(ZIEL, name), "wb") as f:
            f.write(daten)

    print("RID @0x%X Schnitt 9 @0x%X: %s" % (rid, rid + SCHNITT * 32, satz.hex(" ")))
    print("  flag %d fov %d (H %d) Auge (%d,%d,%d) Ziel (%d,%d,%d) PRI 0x%X..0x%X" %
          (flag, fov, fov >> 7, ax, ay, az, zx, zy, zz, pri9, pri10))
    print("PRI Schnitt 9: %d Gruppen, %d Masken, %d Byte" % (gruppen, masken, len(pri)))
    print("SLD: STAGE7.BIN @0x%X = 0x%X, Chunk[L-4] = %d, SLD @0x%X, entpackt 0x%X" %
          (SLD_TABELLE + SLD_ZEILE * 0x20 + SCHNITT * 2, sld, vorhanden, sld_off, entpackt))
    for name, daten in (("R704_CUT09.BSS", chunk), ("R704_CUT09.PRI", pri), ("R704_CUT09_PRI.TIM", atlas),
                        ("R704_CUT10.BSS", chunk10), ("R704_CUT10.PRI", pri_s10)):
        print("%-16s %6d Byte  sha1 %s" % (name, len(daten), hashlib.sha1(daten).hexdigest()))


if __name__ == "__main__":
    main()
