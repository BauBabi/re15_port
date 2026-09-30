#!/usr/bin/env python3
"""plc_motion_zensus.py - Spur D (Runde 34 Nacht): alle SPIELER-Plc_motion-Aufrufe aller RDTs.

Je Aufruf: Raum, Thread (main/sub), Datei-Offset, Clip (pc[2]), Flags (pc[3]), ob direkt danach
Plc_flg(0,0x80) folgt (= Rueckwaertslauf, Muster "hin und denselben Weg zurueck"), die Sleep-Summe
bis zum naechsten Plc_motion/Message_on, die zuletzt gezeigte Nachricht (Wortlaut) — und der
INHALTS-Hash des Clips im Raum-RBJ (Record mit Marker-Bit 0 = Spieler, FUN_8001b3f8), damit
dieselbe Geste unter verschiedener Nummer (Bibliothek ab Clip 15 bzw. ab Clip 0) erkannt wird.

Spieler-Kontext: Work_set(1,0) (`2e 01 00`) setzt die Work-Entity auf den Spieler; Work_set(2/3,n)
auf Gegner/Objekt n. Ein Thread ohne Work_set beginnt beim Spieler (Port: t->work_slot = -1 ->
entity 0 -> Spieler, scd_vm.c op_plc_motion).

Walker/Laengentabelle: scd_walk_lib (eine einzige Tabelle, aus scd_vm.c s_opcode_sizes).

Aufruf: plc_motion_zensus.py <out.tsv> <RDT...>
"""
import os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as W                      # noqa: E402
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import rbj_zensus as R                         # noqa: E402


def spieler_record(recs):
    for r in recs:
        if "marker" in r and (r["marker"] & 1):
            return r
    return None


def main():
    out = sys.argv[1]
    rows = []
    for p in sys.argv[2:]:
        d = open(p, "rb").read()
        if len(d) < 0x60:
            continue
        raum = os.path.splitext(os.path.basename(p))[0]
        msgs = W.messages(d)
        recs = R.records(d)
        prec = spieler_record(recs)
        for (tag, idx), ops in sorted(W.regionen(d).items()):
            work = ("spieler", 0)
            last_msg = None
            for i, (pc, op, sz) in enumerate(ops):
                if op == 0x2E:
                    kind, n = d[pc + 1], d[pc + 2]
                    work = ("spieler", 0) if kind == 1 else (f"k{kind}", n)
                elif op == 0x2B:
                    last_msg = d[pc + 1]
                elif op == 0x3F and work[0] == "spieler":
                    clip, flg = d[pc + 2], d[pc + 3]
                    rev = 0
                    sleep = 0
                    for (pc2, op2, sz2) in ops[i + 1:]:
                        if op2 == 0x43 and d[pc2 + 1] == 0 and (d[pc2 + 2] | (d[pc2 + 3] << 8)) & 0x80:
                            rev = 1
                        if op2 == 0x09:
                            sleep += W.u16(d, pc2 + 2)
                        if op2 in (0x3F, 0x2B, 0x40, 0x01, 0x2E):
                            break
                    h, n = "-", "-"
                    if prec and clip < len(prec["clips"]):
                        h = prec["clips"][clip]["hash"]; n = prec["clips"][clip]["bilder"]
                    txt = msgs.get(last_msg, ("", []))[0] if last_msg is not None else ""
                    rows.append((raum, f"{tag}{idx:02d}", f"0x{pc:05X}", pc_bytes(d, pc), clip,
                                 f"0x{flg:02X}", rev, sleep, n, h,
                                 "" if last_msg is None else str(last_msg), txt))
    with open(out, "w", encoding="utf-8") as f:
        f.write("raum\tthread\toff\tbytes\tclip\tflags\trueck\tsleep\tbilder\thash\tmsg\ttext\n")
        for r in rows:
            f.write("\t".join(str(x) for x in r) + "\n")
    print(f"{len(rows)} Spieler-Plc_motion -> {out}")


def pc_bytes(d, pc):
    return " ".join(f"{b:02x}" for b in d[pc:pc + 4])


if __name__ == "__main__":
    main()
