# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: welche Ziffernfolge oeffnet die Kartenleser-Schloesser (Original-Daten)?

Das Schloss ist ein DREH-Rad (tests/integration/test_keypad.c, ROOM1230): sub01 vergleicht den
Radwert Member[15] des Objekts (Work_set kind 3 idx n) mit festen Werten (`Member_cmp 3e 00 0f 00 v 00`)
und startet bei VIERECK (`Sce_key_ck 51 01 40 00`) je Wert eine Ziffern-Sub. Die RICHTIGE Folge ergibt
sich aus den Ziffern-Subs selbst: jede setzt ein Stellen-Bit (Bank 5 Bit 13/14/15/16) und prueft das
Weiterschalt-Bit der vorigen Stelle (Ck 5,x). Dieses Werkzeug liest die Folge aus den Bytes:
Stelle 1 = die Sub, die Bit 13 setzt, usw., und gibt den Radwert je Stelle aus.

Die Umrechnung Radwert -> Ziffer steht NICHT im Skript (sie steckt in der Radstellung des Modells).
Belegt wird sie ueber die Original-Hinweistexte: der Zettel in ROOM1230 msg 10 ("5632") und in
ROOM1110 msg 0 ("4312") - passt die Folge zu genau EINEM Versatz, ist die Zuordnung belegt.

Aufruf: python re15_port/tools/r34n_e/codes.py ROOM10D0 ROOM10D1 ROOM1230 ROOM1231
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import zensus_item_nachricht as Z  # noqa: E402  (Walker: op_size, regionen)

CD = os.path.join(Z.REPO, "re15_port", "shared_assets", "PSX")


def subs_lesen(d):
    s = Z.u32(d, 0x44)
    out = {}
    for a, e, idx in Z.regionen(d, s):
        ops = []
        pc = a
        while pc < e:
            sz = Z.op_size(d, pc)
            if sz is None or pc + sz > e:
                break
            ops.append((pc, d[pc:pc + sz]))
            pc += sz
        out[idx] = ops
    return out


def main():
    for name in sys.argv[1:]:
        stage = name[4]
        d = open(os.path.join(CD, "STAGE%s" % stage, name + ".RDT"), "rb").read()
        subs = subs_lesen(d)
        # sub01: Radwert -> Ziffern-Sub
        wert_sub = {}
        letzter = None
        for pc, b in subs.get(1, []):
            if b[0] == 0x3E and b[2] == 0x0F:
                letzter = struct.unpack_from("<h", b, 4)[0]
            if b[0] == 0x04 and letzter is not None:
                wert_sub[b[3]] = (letzter, pc)
                letzter = None
        # Ziffern-Subs: welche setzt Bank 5 Bit 13..16?
        stelle = {}
        for idx, ops in subs.items():
            for pc, b in ops:
                if b[0] == 0x22 and b[1] == 5 and b[2] in (13, 14, 15, 16) and b[3] == 1:
                    stelle[b[2] - 12] = (idx, pc)
        folge = []
        for k in (1, 2, 3, 4):
            if k not in stelle:
                folge.append(None)
                continue
            idx, pc = stelle[k]
            w = wert_sub.get(idx)
            folge.append((idx, pc, w))
        print("%s: Radwerte je Sub %s" % (name, {k: v[0] for k, v in sorted(wert_sub.items())}))
        for k, f in enumerate(folge, 1):
            if f:
                print("   Stelle %d: sub%02d setzt Bank5/Bit%d @0x%05X, Radwert %s" % (k, f[0], 12 + k, f[1],
                      "%d (Member_cmp @0x%05X)" % f[2] if f[2] else "?"))
        werte = [f[2][0] for f in folge if f and f[2]]
        if len(werte) == 4:
            kand = []
            for off in range(-20, 21):
                z = [(w - off) % 10 for w in werte]
                kand.append((off, "".join(str(x) for x in z)))
            print("   Radwerte %s -> Ziffern je Versatz (w - v) mod 10: %s" % (werte, ", ".join(
                "v=%d:%s" % (o, s) for o, s in kand if o in (0, 1, 2, 3, 4, 5, 6, 7))))


if __name__ == "__main__":
    main()
