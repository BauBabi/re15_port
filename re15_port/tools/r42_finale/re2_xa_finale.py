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
Aufruf: re2_xa_finale.py [<Ausgabeverzeichnis>]   (Standard re15_port/shared_assets/RE2)

Stufe 4 (Plan S9, Elza 5091): R702_XA03_CLAIRE.XAS aus der Claire-PAL-Disc des Nutzers (SLES-10972; im Repo gibt es keine brauchbare
Claire-XA: info/re2clair/PL1/VOICE/VOICE11.XAS ist nur Nullbytes, VOICE17.XAS fehlt, Dossier B 1.3). Gelesen wird NUR ueber ISO9660:
PL1/VOICE/VOICE17.XAS;1 = LBA 96572, 4685824 Byte = 2288 Sektoren (je Sektor die rohen 2336 Byte ab Byte 16 des 2352-Byte-Rohsektors),
COMMON/BIN/STAGE7.BIN (420 Byte) Tabelle @0x144, Eintrag 3 @0x156 `05 00 00 00 4f 00` = Kanal 5, Block 0, Laenge 79; Lesefenster
79*16 - 0x1E = 1234 Sektoren (Claire-Overlay mit denselben Formeln, Dossier B 6). Gleiche Schnittregel wie Leon.
Aufruf: re2_xa_finale.py --claire "<.../Resident Evil 2 Claire.bin>" [<Ausgabeverzeichnis>]"""
import hashlib, os, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
XAS = os.path.join(REPO, "info", "re2leon", "PL0", "VOICE", "VOICE07.XAS")
STAGE7 = os.path.join(REPO, "info", "re2leon", "COMMON", "BIN", "STAGE7.BIN")
_ARGS = [a for a in sys.argv[1:] if a != "--claire"]
if "--claire" in sys.argv[1:]:
    _ARGS = _ARGS[1:]
OUT = _ARGS[0] if _ARGS else os.path.join(REPO, "re15_port", "shared_assets", "RE2")
SEK = 2336
XA_TAB = 0x144
XAS_MD5 = "2f592fe0ea42e0d34cabddf9fbb4cec4"   # = Disc VOICE07.XAS;1 (re2_xa_schnitt.py)


def schneiden(x, st7, nr, soll, name, kanal_erwartet=None):
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


CLAIRE_XAS = ("PL1/VOICE/VOICE17.XAS", 96572, 4685824)   # ISO-Verzeichnis der Claire-PAL-Disc (Dossier B 1.2)
CLAIRE_ST7 = "COMMON/BIN/STAGE7.BIN"


def claire(disc_pfad):
    import importlib.util
    roh = open(disc_pfad, "rb")

    def sektor(lba):
        roh.seek(lba * 2352)
        return roh.read(2352)

    def iso_finde(pfad):
        pvd = sektor(16)[24:24 + 2048]
        assert pvd[1:6] == b"CD001", "kein ISO9660"
        lba, groesse = struct.unpack_from("<I", pvd, 156 + 2)[0], struct.unpack_from("<I", pvd, 156 + 10)[0]
        for teil in pfad.split("/"):
            daten = b"".join(sektor(lba + i)[24:24 + 2048] for i in range((groesse + 2047) // 2048))
            p, treffer = 0, None
            while p < len(daten):
                ln = daten[p]
                if ln == 0:
                    p = ((p // 2048) + 1) * 2048
                    continue
                n = daten[p + 33:p + 33 + daten[p + 32]].decode("ascii", "replace").split(";")[0]
                if n.upper() == teil.upper():
                    treffer = (struct.unpack_from("<I", daten, p + 2)[0], struct.unpack_from("<I", daten, p + 10)[0])
                    break
                p += ln
            assert treffer, "%s nicht auf der Disc" % pfad
            lba, groesse = treffer
        return lba, groesse

    lba, groesse = iso_finde(CLAIRE_XAS[0])
    assert (lba, groesse) == CLAIRE_XAS[1:], "VOICE17.XAS: LBA %d Groesse %d" % (lba, groesse)
    n = (groesse + 2047) // 2048
    x = b"".join(sektor(lba + i)[16:16 + SEK] for i in range(n))
    l7, g7 = iso_finde(CLAIRE_ST7)
    st7 = sektor(l7)[24:24 + 2048][:g7]
    assert g7 == 420, "Claire-STAGE7.BIN %d Byte" % g7
    print("Claire-Disc: VOICE17.XAS LBA %d, %d Sektoren, md5 %s; STAGE7.BIN LBA %d md5 %s" % (
        lba, n, hashlib.md5(x).hexdigest(), l7, hashlib.md5(st7).hexdigest()))
    os.makedirs(OUT, exist_ok=True)
    schneiden(x, st7, 0x03, (5, 0, 79), "R702_XA03_CLAIRE.XAS")


def main():
    if len(sys.argv) > 2 and sys.argv[1] == "--claire":
        return claire(sys.argv[2])
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
