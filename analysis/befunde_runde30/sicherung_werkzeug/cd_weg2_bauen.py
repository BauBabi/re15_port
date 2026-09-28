#!/usr/bin/env python3
"""MESS-WURZEL fuer Weg 2: Kopien von ITEM/ITPS.ITP und DATA/ITEMALL.PIX, in denen NUR der
Block/Tile 0x40 durch die Prototypen ersetzt ist. Liegt unter build/r30_sicherung/cd_weg2/ —
shared_assets/ bleibt unberuehrt. Die exe nimmt die Wurzel ueber RE15_CD_ROOT (steht in der
Wurzelliste vorn, platform/pc/src/asset_root_pc.c:208-211); alle uebrigen Dateien fallen
auf die normale Wurzel durch.

⛔ NUR MESSUNG. Der Bau bekommt KEINEN Asset-Patch, sondern eingebackene Bytes (Dossier §5.4).

    python analysis/befunde_runde30/sicherung_werkzeug/cd_weg2_bauen.py [marke]
"""
import os
import sys

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
Z = os.path.join(REPO, "build", "r30_sicherung")
marke = sys.argv[1] if len(sys.argv) > 1 else "weg2z"
itps = bytearray(open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read())
pix = bytearray(open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read())
blk = open(os.path.join(Z, marke + "_itps_block_40.bin"), "rb").read()
til = open(os.path.join(Z, marke + "_icon_tile_40.bin"), "rb").read()
assert len(blk) == 0x3000 and len(til) == 1200
alt = bytes(itps)
itps[0x40 * 0x3000:0x41 * 0x3000] = blk
# der Block fuehrt das 40x30-Icon ein zweites Mal bei +0x21A0 (66 von 72 Bloecken bytegleich
# mit ITEMALL.PIX) — mitziehen, sonst zeigen zwei Quellen zwei Gegenstaende
itps[0x40 * 0x3000 + 0x21A0:0x40 * 0x3000 + 0x21A0 + 1200] = til
pix[0x40 * 1200:0x41 * 1200] = til
ziel = os.path.join(Z, "cd_weg2")
os.makedirs(os.path.join(ziel, "ITEM"), exist_ok=True)
os.makedirs(os.path.join(ziel, "DATA"), exist_ok=True)
open(os.path.join(ziel, "ITEM", "ITPS.ITP"), "wb").write(itps)
open(os.path.join(ziel, "DATA", "ITEMALL.PIX"), "wb").write(pix)
d = sum(1 for i in range(len(alt)) if alt[i] != itps[i])
ausserhalb = sum(1 for i in range(len(alt)) if alt[i] != itps[i] and not (0xC0000 <= i < 0xC3000))
print("ITPS.ITP: %d Byte geaendert, davon %d ausserhalb von Block 0x40" % (d, ausserhalb))
print("geschrieben:", ziel)
