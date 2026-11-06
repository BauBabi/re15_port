#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_xa_schnitt.py - Runde 41 G5-Auftritt (Abnahme 1 M5): die zwei RE2-XA-Strecken der Szene bytegleich aus VOICE07.XAS schneiden.

RE2 room7040 MAIN sub06: @0x01920 `59 00 0b 00` Xa_on(0, 0x0B) + Wsleep, @0x01946 `59 00 0c 00` Xa_on(0, 0x0C) + Wsleep.
  Handler 0x59 @0x80058028 (Tabelle @0x800a74c8[89]): a0 = pc[1] (`lbu a0,1(v0)` @0x80058040), a1 = pc[2..3] (`lhu a1,2(v0)`
  @0x80058044) -> FUN_800129b4 @0x80058048, danach 0x800cfbd8 |= 0x20 @0x80058060.
  FUN_800129b4: Tabelle = *0x800ce544 (STAGE7.BIN, Ladeadresse 0x8011a000 -> Datei @0x144), Eintrag 6 Byte {u16 Kanal, u16 Block,
  u16 Laenge}; Datei = Stage-Tabelle @0x80010818 (Stage-Index 6 -> u16 0x34A) -> Dateisatz @0x800988a4 + 0x34A*8
  {u32 Groesse 4447744, u16/u8 LBA 105782} = PL0/VOICE/VOICE07.XAS (ISO-Verzeichnis der Disc: VOICE07.XAS;1 Extent 105782).
  Start = LBA + Block*16 + Kanal, Ende = LBA - 0x1E + (Block+Laenge)*16 + Kanal; CdlSetfilter(Datei 1, Kanal); Abbruch bei
  Lage >= Ende (`sltu v0,v0,v1` @0x80012e3c -> Zustand 3 @0x80012e4c -> CdlPause @0x80012f58-6c, Bit 0x20 aus @0x80012f90-94).
  Laufzeitbeleg (Savestates der Abnahme-1-Aufnahme): s06 Start 105786 / Ende 107100 / Filter 01 04, s12 105783 / 107465 / 01 01.

Ausgabe: je Strecke die rohen 2336-Byte-Sektoren DES Kanals im Lesefenster [Start, Ende), Schritt 16 (8 Kanaele in 16er-Zeilen,
Subheader Datei 1, Kanal, Submode 0x64, Codierung 0x00 = mono / 37800 Hz / 4 Bit):
  R704_XA0B.XAS = Kanal 4, Sektoren 4, 20, .., 1316  (83 Sektoren, 193888 Byte)
  R704_XA0C.XAS = Kanal 1, Sektoren 1, 17, .., 1681  (106 Sektoren, 247616 Byte)
Das Lesefenster selbst (1314 / 1682 Sektoren) traegt der Port als Konstante (g5auftritt_r41.c), nicht die Datei.
Aufruf: re2_xa_schnitt.py [<Ausgabeverzeichnis>]   (Standard re15_port/shared_assets/RE2)"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
XAS = os.path.join(REPO, "info", "re2leon", "PL0", "VOICE", "VOICE07.XAS")
STAGE7 = os.path.join(REPO, "info", "re2leon", "COMMON", "BIN", "STAGE7.BIN")
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "re15_port", "shared_assets", "RE2")
SEK = 2336
XA_TAB = 0x144                      # STAGE7.BIN: *0x800ce544 = 0x8011a144 bei Ladeadresse 0x8011a000
XAS_MD5 = "2f592fe0ea42e0d34cabddf9fbb4cec4"   # = Disc VOICE07.XAS;1 (LBA 105782, 1904 Sektoren), gemessen 2026-11-02


def main():
    x = open(XAS, "rb").read()
    assert hashlib.md5(x).hexdigest() == XAS_MD5, "VOICE07.XAS ist nicht die Disc-Datei"
    assert len(x) == 1904 * SEK
    st7 = open(STAGE7, "rb").read()
    os.makedirs(OUT, exist_ok=True)
    for nr, soll in ((0x0B, (4, 0, 84)), (0x0C, (1, 0, 107))):
        kanal, block, laenge = struct.unpack_from("<HHH", st7, XA_TAB + nr * 6)
        assert (kanal, block, laenge) == soll, "STAGE7.BIN @0x%X: %r" % (XA_TAB + nr * 6, (kanal, block, laenge))
        start = block * 16 + kanal                      # @0x80012af4..
        ende = -0x1E + (block + laenge) * 16 + kanal    # @0x80012b00..b10
        teile = []
        for i in range(start, ende, 16):
            s = x[i * SEK:(i + 1) * SEK]
            assert s[0] == 1 and s[1] == kanal and s[2] == 0x64 and s[3] == 0x00 and s[4:8] == s[0:4], (nr, i, s[:8].hex())
            teile.append(s)
        name = "R704_XA%02X.XAS" % nr
        open(os.path.join(OUT, name), "wb").write(b"".join(teile))
        print("%s: Kanal %d, Sektoren %d..%d (%d), Lesefenster %d Sektoren = %.3f s, PCM %.3f s, md5 %s" % (
            name, kanal, start, start + 16 * (len(teile) - 1), len(teile), ende - start, (ende - start) / 150.0,
            len(teile) * 4032 / 37800.0, hashlib.md5(b"".join(teile)).hexdigest()))


if __name__ == "__main__":
    main()
