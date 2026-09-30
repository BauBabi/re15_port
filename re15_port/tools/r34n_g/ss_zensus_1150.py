#!/usr/bin/env python3
"""Spur G2 (Runde 34 Nacht) - Savestate-Zensus ROOM1150/1151: Zustand der Blink-Maskengruppen.

Nur LESEN. Laeuft ueber alle DuckStation-Staende (*.sav, *.bak) der angegebenen Ordner (rekursiv)
und meldet je Stand: Sauberkeit der EXE (Wort @0x80026e4c == 0x03e00008), Stage-Fingerabdruck
CRC32(0x80100000..+0x8000), Raum DAT_800b0fe2, Cut DAT_800b0fe4, VBlank-Zaehler 0x800787dc,
VSync-Modus DAT_800b5456 (FUN_8002137c: VSync(DAT_800b5456)).

Fuer Staende in STAGE 1 / Raum 0x15 (ROOM1150/1151) zusaetzlich die Masken-Record-Tabelle:
  DAT_800ac778 = RDT im RAM, RDT[0] = Maskenzahl des aktuellen Cuts (FUN_800392d4),
  RDT[7] = Maximum; DAT_800b2584 = Record-Tabelle, 4 Byte je Maske:
  Byte0 Bit0 = zeichnen (FUN_80039590), Byte1 = Gruppe+1, Halbwort +2 = Tiefe.
Opcode 0x45 (@0x800428d4 -> FUN_800396a8) setzt Byte0 der Gruppe op1+1.

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_zensus_1150.py <ordner> [<ordner> ...]
"""
import glob
import os
import struct
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
import re15_ss  # noqa: E402

STAGE1_FP = 0x96290818
STUB_CLEAN = 0x03e00008


def main():
    dirs = sys.argv[1:]
    if not dirs:
        print(__doc__)
        return 2
    files = []
    for d in dirs:
        for pat in ("*.sav", "*.bak"):
            files += glob.glob(os.path.join(d, "**", pat), recursive=True)
    files = sorted(set(files))
    treffer = 0
    raeume = {}
    for f in files:
        try:
            r = re15_ss.Ram(f)
        except Exception as e:  # noqa: BLE001
            print("UNLESBAR %s: %s" % (f, e))
            continue
        stub = r.u32(0x80026e4c)
        fp = zlib.crc32(r.bytes(0x80100000, 0x8000)) & 0xffffffff
        room = r.u16(0x800b0fe2)
        cut = r.u16(0x800b0fe4)
        key = ("sauber" if stub == STUB_CLEAN else "GEPATCHT", fp, room)
        raeume[key] = raeume.get(key, 0) + 1
        if fp != STAGE1_FP or room != 0x15:
            continue
        treffer += 1
        rdt = r.u32(0x800ac778)
        nmask = r.u8(rdt)
        nmax = r.u8(rdt + 7)
        tab = r.u32(0x800b2584)
        recs = [r.bytes(tab + 4 * i, 4) for i in range(nmask)]
        blink = [(i, b[0], b[1], struct.unpack_from("<H", b, 2)[0]) for i, b in enumerate(recs)
                 if 6 <= b[1] <= 11]
        andere_aus = sum(1 for b in recs if not (b[0] & 1) and not (6 <= b[1] <= 11))
        print("%s\n   EXE %s (Stub %08x) Vcount=%d Cut=%d VSync-Modus=%d RDT@%08x RDT[0]=%d RDT[7]=%d"
              " Records@%08x" % (f, "sauber" if stub == STUB_CLEAN else "GEPATCHT", stub,
                                 r.u32(0x800787dc), cut, r.u8(0x800b5456), rdt, nmask, nmax, tab))
        if blink:
            print("   Blink-Gruppen: " + "  ".join("rec%d g%d byte0=%d tiefe=%d" % (i, g - 0, b0, dep)
                                                   for (i, b0, g, dep) in blink))
        else:
            print("   Blink-Gruppen: keine im aktuellen Cut")
        print("   uebrige Records mit Byte0 Bit0 = 0: %d" % andere_aus)
    print("\nStaende gesamt %d, davon ROOM1150/1151 (STAGE 1, Raum 0x15): %d" % (len(files), treffer))
    print("Verteilung (EXE, Fingerabdruck, Raum): " + ", ".join(
        "%s/%08x/%02x:%d" % (k[0], k[1], k[2], v) for k, v in sorted(raeume.items())))
    return 0


if __name__ == "__main__":
    sys.exit(main())
