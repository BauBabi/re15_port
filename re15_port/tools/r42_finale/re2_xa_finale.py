#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_xa_finale.py - Runde 42 Strang finale (Plan S1): den RE2-Todesschrei des Super-Tyrant (Xa 0x03) bytegleich schneiden.

Tyrant-Phase 7 ruft die XA direkt aus dem Overlay (CDEMD0_EM2B_ai1.BIN gegen 0x80100000): `move a0,zero` @0x80103bec,
`jal 0x800129b4` @0x80103bf0, `addiu a1,zero,3` @0x80103bf4; danach 0x800cfbd8 |= 0x20 @0x80103bf8-0x80103c08. Phase 8 wartet,
bis das Bit wieder aus ist (`andi v0,v0,0x20 / bnez` @0x80103d70-74), dann FUN_80018fb0 @0x80103d7c (Abschuss-Flag).
FUN_800129b4 (RE2-Leon-EXE): Tabelle = *0x800ce544 = STAGE7.BIN @0x144 (Ladeadresse 0x8011a000), Eintrag 3 @0x156
`05 00 00 00 42 00` = Kanal 5, Block 0, Laenge 66; Start = Block*16 + Kanal (@0x80012ac4-0x80012adc), Ende = -0x1E +
(Block+Laenge)*16 + Kanal (@0x80012ae8-0x80012b10). Datei VOICE07.XAS (Stage-Tabelle @0x80010818, Index 6), Herleitung wie
dem bis v0.8.30 benutzten Schnittwerkzeug der G5-Szene (entfernt).

Ausgabe: die rohen 2336-Byte-Sektoren DES Kanals im Lesefenster [Start, Ende), Schritt 16 (Subheader Datei 1, Kanal 5,
Submode 0x64, Codierung 0x00 = mono 37800 Hz 4 Bit):
  R702_XA03_LEON.XAS = Kanal 5, Sektoren 5, 21, .., (Lesefenster 1026 Sektoren)
Das Lesefenster selbst (1026) traegt der Port als Konstante (finale_r42.c, RE15_FI_XA03_LESE), nicht die Datei.
Aufruf: re2_xa_finale.py [<Ausgabeverzeichnis>]   (Standard re15_port/shared_assets/RE2)"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
XAS = os.path.join(REPO, "info", "re2leon", "PL0", "VOICE", "VOICE07.XAS")
STAGE7 = os.path.join(REPO, "info", "re2leon", "COMMON", "BIN", "STAGE7.BIN")
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "re15_port", "shared_assets", "RE2")
SEK = 2336
XA_TAB = 0x144
XAS_MD5 = "2f592fe0ea42e0d34cabddf9fbb4cec4"   # = Disc VOICE07.XAS;1 (re2_xa_schnitt.py)


def main():
    x = open(XAS, "rb").read()
    assert hashlib.md5(x).hexdigest() == XAS_MD5, "VOICE07.XAS ist nicht die Disc-Datei"
    assert len(x) == 1904 * SEK
    st7 = open(STAGE7, "rb").read()
    os.makedirs(OUT, exist_ok=True)
    for nr, soll, name in ((0x03, (5, 0, 66), "R702_XA03_LEON.XAS"),):
        kanal, block, laenge = struct.unpack_from("<HHH", st7, XA_TAB + nr * 6)
        assert (kanal, block, laenge) == soll, "STAGE7.BIN @0x%X: %r" % (XA_TAB + nr * 6, (kanal, block, laenge))
        start = block * 16 + kanal
        ende = -0x1E + (block + laenge) * 16 + kanal
        teile = []
        for i in range(start, ende, 16):
            s = x[i * SEK:(i + 1) * SEK]
            assert s[0] == 1 and s[1] == kanal and s[2] == 0x64 and s[3] == 0x00 and s[4:8] == s[0:4], (nr, i, s[:8].hex())
            teile.append(s)
        daten = b"".join(teile)
        open(os.path.join(OUT, name), "wb").write(daten)
        print("%s: Kanal %d, Sektoren %d..%d (%d), Lesefenster %d Sektoren = %.3f s, PCM %.3f s, %d Byte, md5 %s" % (
            name, kanal, start, start + 16 * (len(teile) - 1), len(teile), ende - start, (ende - start) / 150.0,
            len(teile) * 4032 / 37800.0, len(daten), hashlib.md5(daten).hexdigest()))


if __name__ == "__main__":
    main()
