#!/usr/bin/env python3
"""dialog_budget.py - Spur D (Runde 34 Nacht, Gegenpruefung Auflage 4): wie lange laesst das ORIGINAL
eine Dialogzeile stehen?

Je Dialog-Message_on (Maske 0 = Untertitel, `2b nn 00 00`) werden die Bilder bis zum NAECHSTEN
Message_on im selben Faden gezaehlt, sofern dazwischen nur Opcodes mit fester Zeit stehen (Sleep =
pc[2..3] Bilder, Evt_next = 1 Bild; Plc_motion/Plc_flg/Work_set/Set/Plc_neck/Se_on/Member_set/...
kosten keine Zeit). Faeden mit Warteschleifen (Plc_dest + Do/Edwhile) fallen heraus. Dazu die Zeichen
der Zeile ohne Sprecher -> Bilder je Zeichen. Walker/Laengen: scd_walk_lib (= scd_vm.c s_opcode_sizes).

Aufruf: dialog_budget.py [PSX-Verzeichnis]   (Standard: re15_port/shared_assets/PSX)
"""
import glob, os, sys
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, ".."))
import scd_walk_lib as L                        # noqa: E402

FEST = {0x2E, 0x3F, 0x43, 0x22, 0x41, 0x04, 0x05, 0x34, 0x3E, 0x2F, 0x20, 0x00}


def main():
    psx = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HIER, "..", "..", "shared_assets", "PSX")
    rows = []
    for p in sorted(glob.glob(os.path.join(psx, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x60:
            continue
        room = os.path.basename(p)[4:8]
        regs, msgs = L.regionen(d), L.messages(d)
        for (tag, idx), ops in regs.items():
            for i, (pc, op, sz) in enumerate(ops):
                if not (op == 0x2B and d[pc + 2] == 0 and d[pc + 3] == 0):
                    continue
                mid, fr, ok, nxt = d[pc + 1], 0, False, -1
                for (pc2, op2, sz2) in ops[i + 1:]:
                    if op2 == 0x09:
                        fr += d[pc2 + 2] | (d[pc2 + 3] << 8)
                    elif op2 == 0x02:
                        fr += 1
                    elif op2 == 0x2B:
                        ok, nxt = True, d[pc2 + 1]
                        break
                    elif op2 not in FEST:
                        break
                if ok:
                    txt = msgs.get(mid, ("", []))[0]
                    body = txt.split(":", 1)[1].strip() if ":" in txt[:8] else txt
                    rows.append((fr, len(body), room, tag, idx, pc, mid, nxt, txt))
    rows.sort()
    print("Dialogzeilen mit fester Zeit bis zum naechsten Message_on: %d" % len(rows))
    for r in rows[:12]:
        print("%4d Bilder  %3d Zeichen  %5.2f B/Z  ROOM%s %s%02d @0x%05X msg %d -> %d  \"%s\""
              % (r[0], r[1], r[0] / max(1, r[1]), r[2], r[3], r[4], r[5], r[6], r[7], r[8][:70]))
    print("--- knappste Bilder je Zeichen")
    for r in sorted(rows, key=lambda r: r[0] / max(1, r[1]))[:8]:
        print("%4d Bilder  %3d Zeichen  %5.2f B/Z  ROOM%s %s%02d @0x%05X msg %d  \"%s\""
              % (r[0], r[1], r[0] / max(1, r[1]), r[2], r[3], r[4], r[5], r[6], r[8][:70]))


if __name__ == "__main__":
    main()
