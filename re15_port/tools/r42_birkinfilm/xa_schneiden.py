#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""xa_schneiden.py - Runde 42 Strang "birkinfilm": Annettes RE2-Stimmen fuer die Rahmenzeilen in ROOM6000 bytegleich aus
VOICE04.XAS schneiden (Form tools/r41_g5auftritt/re2_xa_schnitt.py).

RE2 Leon-Disc, ROOM4090 sub02 (Ada trifft Annette), Xa_on (0x59, Handler @0x80058028 -> FUN_800129b4):
  Xa 0x09  "Annette Birkin. My husband is the man responsible for the creation of the T-virus. William Birkin."
  Xa 0x11  "And it's all Umbrella's fault. None of this would have happened if they hadn't tried to steal his research
            away from him."   (@0x027D4 `59 00 11 00`, direkt vor dem Film @0x02818 `6f 0b`)
  Xa 0x14  "As a result of his virus-induced transmutation, William should have lost any prior memories he had as a human
            by now."
  (Wortlaut: Abschrift scratchpad zws_birkin/voice04_asr.txt, Whisper medium.en; RE2-US zeigt zu diesen Zeilen keinen Text,
  ROOM4090 Nachrichtenblock 2 @0x3FB0 msg 0..32 = leer `fa fc fc fa 02 fe`.)
Tabelle: COMMON/BIN/STAGE4.BIN @0x384 (Ladeadresse 0x8011a000; @0x28 `lui v0,0x8012` + @0x2C `addiu v0,v0,-0x5c7c` =
  0x8011a384, @0x38 `sw v0,-0x1abc(at)` -> 0x800ce544), Eintrag 6 Byte {u16 Kanal, u16 Block, u16 Laenge}:
  Xa 0x09 @0x3BA `06 00 00 00 44 00`, Xa 0x11 @0x3EA `04 00 00 00 47 00`, Xa 0x14 @0x3FC `03 00 00 00 4e 00`.
Datei: Stage-Tabelle @0x80010818[4] = 0x347 -> Dateisatz @0x800988a4 + 0x347*8 = {Groesse 3438592, LBA 99094} = VOICE04.XAS
  (1472 Sektoren zu 2336 B, Groesse gleich).
Lesefenster (FUN_800129b4): Start = LBA + Block*16 + Kanal (@0x80012ac4-@0x80012adc), Ende = LBA - 0x1E + (Block+Laenge)*16
  + Kanal (@0x80012af4-@0x80012b10); RE2 haelt bei Lage >= Ende. Lesesektoren = Laenge*16 - 0x1E.
Ausgabe: shared_assets/RE2/R409_XA09.XAS / R409_XA11.XAS / R409_XA14.XAS = rohe 2336-Byte-Sektoren DES Kanals im Lesefenster.
Aufruf: python re15_port/tools/r42_birkinfilm/xa_schneiden.py [--check]
"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.abspath(os.path.join(HERE, "..", ".."))


def _haupt(rel):
    d = os.path.abspath(os.path.join(PORT, ".."))
    for _ in range(5):
        p = os.path.join(d, rel)
        if os.path.exists(p):
            return p
        d = os.path.dirname(d)
    return os.path.join(PORT, "..", rel)


XAS = _haupt(os.path.join("info", "re2leon", "PL0", "VOICE", "VOICE04.XAS"))
STAGE4 = _haupt(os.path.join("info", "re2leon", "COMMON", "BIN", "STAGE4.BIN"))
XAS_MD5 = "bf0833e8c81449cbe1eedbefc43fbcd0"     # gemessen 2026-11-18, Groesse = Dateisatz 0x347
OUT = os.path.join(PORT, "shared_assets", "RE2")
SEK = 2336
XA_TAB = 0x384
ZEILEN = ((0x09, (6, 0, 68)), (0x11, (4, 0, 71)), (0x14, (3, 0, 78)))


def strecken():
    x = open(XAS, "rb").read()
    assert hashlib.md5(x).hexdigest() == XAS_MD5, "VOICE04.XAS ist nicht die Disc-Datei"
    assert len(x) == 1472 * SEK
    st4 = open(STAGE4, "rb").read()
    aus = []
    for nr, soll in ZEILEN:
        kanal, block, laenge = struct.unpack_from("<HHH", st4, XA_TAB + nr * 6)
        assert (kanal, block, laenge) == soll, "STAGE4.BIN @0x%X: %r" % (XA_TAB + nr * 6, (kanal, block, laenge))
        start = block * 16 + kanal
        ende = -0x1E + (block + laenge) * 16 + kanal
        teile = []
        for i in range(start, ende, 16):
            s = x[i * SEK:(i + 1) * SEK]
            assert s[0] == 1 and s[1] == kanal and s[2] & 0x7F == 0x64 and s[3] == 0x00 and s[4:8] == s[0:4], (nr, i, s[:8].hex())
            teile.append(s)
        aus.append((nr, kanal, start, ende, b"".join(teile)))
    return aus


def main():
    os.makedirs(OUT, exist_ok=True)
    fehler = 0
    for nr, kanal, start, ende, roh in strecken():
        name = "R409_XA%02X.XAS" % nr
        p = os.path.join(OUT, name)
        md5 = hashlib.md5(roh).hexdigest()
        print("%s: Kanal %d, Sektoren %d..%d (%d), Lesefenster %d Sektoren = %.3f s = %.1f Bilder, md5 %s" % (
            name, kanal, start, start + 16 * (len(roh) // SEK - 1), len(roh) // SEK, ende - start, (ende - start) / 150.0,
            (ende - start) / 5.0, md5))
        if "--check" in sys.argv:
            alt = open(p, "rb").read() if os.path.exists(p) else b""
            if alt != roh:
                print("  VERALTET"); fehler += 1
        else:
            open(p, "wb").write(roh)
    sys.exit(1 if fehler else 0)


if __name__ == "__main__":
    main()
