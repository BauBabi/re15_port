#!/usr/bin/env python3
"""codepanel_muster.py - Byte-Muster-Zensus: WELCHE Raeume haben ueberhaupt ein Code-Panel?

Drei unabhaengige Messungen ueber ALLE 206 RDTs, damit die Panel-Liste nicht aus den
16 Wegwerf-Stellen "erraten" wird, sondern aus dem Auslieferungsstand fallt:

 (1) TEXT-ZENSUS  - welcher Raum enthaelt den Text "Enter the first number."?
     Das ist die Aufforderung zur Ziffern-Eingabe; ohne sie gibt es keine Code-Eingabe.
 (2) MUSTER-ZENSUS - welcher Raum enthaelt die Byte-Folge
     21 05 0d 01  21 05 0e 01  21 05 0f 01  21 05 10 01
     = Ck(5,13,1) Ck(5,14,1) Ck(5,15,1) Ck(5,16,1) = "alle vier Ziffern richtig".
     Das ist die Pruefung, die das Schloss oeffnet. Der Zensus laeuft ueber die ROHEN
     Datei-Bytes (nicht ueber den Opcode-Walk), damit kein Desync eine Stelle verschluckt
     - Ghidras/Walkers Lueckenproblem, vgl. CLAUDE.md-Hinweis zu unvollstaendigen XREFs.
     Jeder Treffer wird danach im Opcode-Walk BESTAETIGT (Offset muss ein Opcode-Anfang
     sein) und der umgebende Ifel_ck/Set-Rahmen ausgegeben.
 (3) EINGABE-ZENSUS - welcher Raum liest ueberhaupt Tasten (Sce_key_ck 0x51 /
     Sce_espr_control 0x52)? Ein Raum ohne diese Opcodes kann keine Eingabe haben.

Aufruf: python codepanel_muster.py [ASSET_ROOT]
"""
import sys, os, glob, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from discard_zensus import (op_size, fwd_target, rdt_section_end,
                            section_regions, u16, u32)
from discard_nutzstellen import messages, walk_ops

MUSTER = bytes([0x21,0x05,0x0d,0x01, 0x21,0x05,0x0e,0x01,
                0x21,0x05,0x0f,0x01, 0x21,0x05,0x10,0x01])
TEXTE  = ["Enter the first number.", "Wrong code, try again.",
          "You've opened the lock.", "insert four digits"]


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "..", "..", "..", "re15_port", "shared_assets", "PSX")
    rdts = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
    gelesen = stummel = 0
    text_treffer = collections.defaultdict(list)
    muster_treffer = []
    eingabe = {}

    for p in rdts:
        d = open(p, "rb").read()
        room = os.path.basename(p).split(".")[0]
        if len(d) < 0x48:
            stummel += 1
            continue
        gelesen += 1

        # (1) Text
        msgs = messages(d)
        for i, (t, c) in msgs.items():
            for needle in TEXTE:
                if needle in t:
                    text_treffer[needle].append((room, i))

        # (2) rohes Byte-Muster
        pos = d.find(MUSTER)
        rohe = []
        while pos >= 0:
            rohe.append(pos)
            pos = d.find(MUSTER, pos + 1)

        # (3) Opcode-Walk: Eingabe-Opcodes + Bestaetigung der Musterstellen
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        opstarts = set()
        n51 = n52 = 0
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d):
                continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                for (pc, op, sz) in walk_ops(d, sec+o, sec+e):
                    opstarts.add(pc)
                    if op == 0x51: n51 += 1
                    if op == 0x52: n52 += 1
        if n51 or n52:
            eingabe[room] = (n51, n52)
        for off in rohe:
            muster_treffer.append((room, off, off in opstarts))

    print(f"=== ABDECKUNG === {gelesen} RDTs mit Header gelesen, {stummel} Stummel (<0x48 B), "
          f"{len(rdts)} Dateien insgesamt")

    print(f"\n=== (1) TEXT-ZENSUS ===")
    for needle in TEXTE:
        tr = text_treffer[needle]
        raeume = sorted(set(r for (r, i) in tr))
        print(f"  {needle!r:32s} {len(tr):>3} Nachrichten in {len(raeume)} Raeumen: "
              f"{', '.join(raeume)}")

    print(f"\n=== (2) MUSTER-ZENSUS  Ck(5,13..16,1) = alle vier Ziffern richtig ===")
    print(f"  rohe Byte-Treffer: {len(muster_treffer)}")
    for (room, off, ok) in muster_treffer:
        print(f"    {room}  Datei-Offset 0x{off:05X}   "
              f"{'im Opcode-Walk BESTAETIGT' if ok else 'NICHT auf Opcode-Grenze (Datenzufall)'}")

    print(f"\n=== (3) EINGABE-ZENSUS  Raeume mit Sce_key_ck(0x51)/Sce_espr_control(0x52) ===")
    print(f"  {len(eingabe)} Raeume")
    for room in sorted(eingabe):
        n51, n52 = eingabe[room]
        print(f"    {room}  0x51 x{n51:<3} 0x52 x{n52}")


if __name__ == "__main__":
    main()
