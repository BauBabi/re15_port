#!/usr/bin/env python3
"""BAU-ABNAHME (Runde 30, Thema H): Bytevergleich der eingebackenen Dateien gegen die Soll-Dateien.

  1. gen/sicherung_prop.inc     re15_sicherung_md1  ==  soll/sicherung_zord_normal.md1
  2. gen/sicherung_prop.inc     re15_sicherung_tim  ==  TIM des Standes VOR dem Bau (git HEAD~/Angabe)
  3. gen/sicherung_itembild.inc re15_sicherung_itps_block == soll/weg2n_itps_block_40.bin
  4. gen/sicherung_itembild.inc re15_sicherung_icon_tile  == soll/weg2n_icon_tile_40.bin
  5. Einsetz-Gegenprobe auf Kopien der ausgelieferten Dateien: in ITPS.ITP aendern sich nur
     Bytes in 0xC0000..0xC2FFF, in ITEMALL.PIX nur 0x12C00..0x130AF.

    python analysis/befunde_runde30/sicherung_werkzeug/bau_bytevergleich.py [<alte sicherung_prop.inc>]

Rueckgabe 0 = alles gleich.
"""
import hashlib
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
GEN = os.path.join(REPO, "re15_port", "engine", "src", "gen")
SOLL = os.path.join(HIER, "soll")
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")

# die im Auftrag genannten Pruefsummen (Anfang), damit ein vertauschtes Soll auffaellt
SHA_SOLL = {
    "sicherung_zord_normal.md1": "39badaa31dbc2f4c",
    "weg2n_itps_block_40.bin": "33d01cb1",
    "weg2n_icon_tile_40.bin": "d6a3248f",
}


def feld(pfad, name):
    t = open(pfad, "r", encoding="utf-8").read()
    m = re.search(r"%s\[(\d+)\]\s*=\s*\{(.*?)\};" % re.escape(name), t, re.S)
    if not m:
        raise SystemExit("Feld %s fehlt in %s" % (name, pfad))
    b = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", m.group(2)))
    assert len(b) == int(m.group(1)), (len(b), m.group(1))
    return b


def sha(b):
    return hashlib.sha256(b).hexdigest()


fehler = 0


def vergleich(was, ist, soll, soll_name=None):
    global fehler
    gleich = ist == soll
    ab = sum(1 for a, b in zip(ist, soll) if a != b) + abs(len(ist) - len(soll))
    print("%-58s %6d B  sha256 %s...  %s" % (was, len(ist), sha(ist)[:16],
                                             "GLEICH" if gleich else "VERSCHIEDEN (%d Bytes)" % ab))
    if soll_name:
        anf = SHA_SOLL[soll_name]
        if not sha(soll).startswith(anf):
            print("   ⛔ Soll-Datei %s traegt NICHT die Pruefsumme %s... des Auftrags" % (soll_name, anf))
            fehler += 1
    if not gleich:
        fehler += 1


def main():
    global fehler
    prop = os.path.join(GEN, "sicherung_prop.inc")
    md1 = feld(prop, "re15_sicherung_md1")
    soll = open(os.path.join(SOLL, "sicherung_zord_normal.md1"), "rb").read()
    vergleich("1. re15_sicherung_md1 gegen soll/sicherung_zord_normal.md1", md1, soll,
              "sicherung_zord_normal.md1")
    if len(sys.argv) > 1:
        alt = feld(sys.argv[1], "re15_sicherung_tim")
        vergleich("2. re15_sicherung_tim gegen den Stand vor dem Bau", feld(prop, "re15_sicherung_tim"), alt)
        alt_md1 = feld(sys.argv[1], "re15_sicherung_md1")
        ab = sum(1 for a, b in zip(alt_md1, md1) if a != b)
        print("   (MD1 vor dem Bau -> jetzt: %d von %d Bytes veraendert)" % (ab, len(md1)))

    bild = os.path.join(GEN, "sicherung_itembild.inc")
    if not os.path.exists(bild):
        print("3./4./5. gen/sicherung_itembild.inc fehlt noch")
        return 1 if fehler else 0
    block = feld(bild, "re15_sicherung_itps_block")
    tile = feld(bild, "re15_sicherung_icon_tile")
    vergleich("3. re15_sicherung_itps_block gegen soll/weg2n_itps_block_40.bin", block,
              open(os.path.join(SOLL, "weg2n_itps_block_40.bin"), "rb").read(), "weg2n_itps_block_40.bin")
    vergleich("4. re15_sicherung_icon_tile gegen soll/weg2n_icon_tile_40.bin", tile,
              open(os.path.join(SOLL, "weg2n_icon_tile_40.bin"), "rb").read(), "weg2n_icon_tile_40.bin")

    itps = open(os.path.join(PSX, "ITEM", "ITPS.ITP"), "rb").read()
    pix = open(os.path.join(PSX, "DATA", "ITEMALL.PIX"), "rb").read()
    n_itps = bytearray(itps)
    n_itps[0x40 * 0x3000:0x41 * 0x3000] = block
    n_pix = bytearray(pix)
    n_pix[0x40 * 1200:0x41 * 1200] = tile
    for name, a, b, lo, hi in (("ITPS.ITP", itps, n_itps, 0xC0000, 0xC2FFF),
                               ("ITEMALL.PIX", pix, n_pix, 0x12C00, 0x130AF)):
        ab = [i for i in range(len(a)) if a[i] != b[i]]
        drin = all(lo <= i <= hi for i in ab)
        print("5. %-12s %7d B: %5d Bytes veraendert, erstes 0x%05X letztes 0x%05X, Fenster "
              "0x%05X..0x%05X -> %s" % (name, len(a), len(ab), ab[0] if ab else 0,
                                        ab[-1] if ab else 0, lo, hi,
                                        "nur im Fenster" if drin else "⛔ AUSSERHALB"))
        if not drin or not ab:
            fehler += 1
    return 1 if fehler else 0


if __name__ == "__main__":
    sys.exit(main())
