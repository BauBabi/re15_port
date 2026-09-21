#!/usr/bin/env python3
"""re15_doc_check.py - Gegenstueck zu re2_doc_worldmodels.py fuer RE1.5.

Frage: gibt es in RE1.5 ueberhaupt Dokument-Items in der Welt, und haben sie ein
Weltmodell (prop != 255)?

Item_aot_set in RE1.5 = Opcode 0x50 (RE2: 0x4E). Feldlage nach
re15_port/engine/src/scd_vm.c:3800-3841 (dort gegen das Original belegt):
    Kurzform (pc[3]&0x80 == 0, 22 B): +14 item  +16 amount  +18 taken-bit  +20 prop
    Langform (pc[3]&0x80 != 0, 30 B): +22 item  +24 amount  +26 taken-bit  +28 prop
`prop` ist das RE1.5-Gegenstueck zu RE2s md1: der Slot in der Modelltabelle des
Raums; 255 = kein Weltmodell.

Dokument-Ids in RE1.5 (Namensblock DEBUG.BIN ab Datei-0x4A28, re15_items.py):
    0x48 Chris' Diary, 0x49 Operation Report, 0x4A..0x51 File 3..File 10,
    0x62..0x64 Umbrella File 7..9.

Aufruf: re15_doc_check.py
"""
import sys, os, glob, struct, collections

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "nutzer_batch_2026-08-27", "tools"))
sys.path.insert(0, HERE)
import re15_scd_walk as W
import re15_items as I

DOC_IDS = set(range(0x48, 0x52)) | set(range(0x62, 0x65))
NM = {i: t for i, _, t in I.names()}


def brute(d):
    """Gegenprobe ohne Walker: jede Byte-Position als 0x50-Kurzform lesen und nur
    annehmen, was ALLE Schranken erfuellt. Noetig, weil der Walker 28 Bloecke
    abbricht und RE1.5-Textbloecke voller 0x50 ('m') stecken."""
    hits = []
    for p in range(0, len(d) - 22):
        if d[p] != 0x50 or d[p + 2] != 0x09:      # sce 9 = Item (alle 162 belegten)
            continue
        if d[p + 3] & 0x80:
            continue
        it = struct.unpack_from("<H", d, p + 14)[0]
        n = struct.unpack_from("<H", d, p + 16)[0]
        prop = d[p + 20]
        if it not in DOC_IDS or n == 0 or n > 16:
            continue
        if not (prop < 0x20 or prop == 255):
            continue
        hits.append((p, it, prop))
    return hits


def main():
    rooms = sorted(glob.glob(os.path.join(REPO, "info", "Re1.5", "PSX", "STAGE*", "ROOM*.RDT")))
    total = 0
    with_prop = 0
    docs = []
    brute_docs = []
    hist = collections.Counter()
    bad = 0
    for path in rooms:
        d = open(path, "rb").read()
        if len(d) < 0x60:
            continue
        room = os.path.splitext(os.path.basename(path))[0]
        try:
            tbl = struct.unpack_from("<24I", d, 0x20)
            blks = W.blocks(d)
        except Exception:
            continue
        for name, base, subs in blks:
            for i, o in enumerate(subs):
                start = base + o
                end = base + subs[i + 1] if i + 1 < len(subs) else None
                if end is None:
                    cand = [x for x in tbl if start < x <= len(d)]
                    end = min(cand) if cand else len(d)
                pc = start
                while pc < end:
                    op = d[pc]
                    try:
                        n = W.oplen(d, pc)
                    except IndexError:
                        break
                    if n == 0 or pc + n > end:
                        bad += 1
                        break
                    if op == 0x50:
                        lf = bool(d[pc + 3] & 0x80)
                        it = d[pc + (22 if lf else 14)]
                        prop = d[pc + (28 if lf else 20)]
                        total += 1
                        hist[it] += 1
                        if prop != 255:
                            with_prop += 1
                        if it in DOC_IDS:
                            docs.append((room, name, i, pc, it, prop))
                    pc += n
        for p, it, prop in brute(d):
            brute_docs.append((room, p, it, prop))

    print("RE1.5: %d RDTs" % len(rooms))
    print("Item_aot_set (0x50) gesamt:              %3d" % total)
    print("davon MIT Weltmodell (prop != 255):      %3d" % with_prop)
    print("davon ohne (prop == 255):                %3d" % (total - with_prop))
    print("verschiedene Item-Ids:                   %3d (0x%02X..0x%02X)"
          % (len(hist), min(hist), max(hist)))
    print("DOKUMENT-Platzierungen (Id 0x48..0x51, 0x62..0x64): %d" % len(docs))
    for r in docs:
        print("   ", r)
    print("Brute-Force-Gegenprobe ueber JEDE Byte-Position aller RDTs: %d Dokument-Treffer"
          % len(brute_docs))
    for r in brute_docs[:20]:
        print("   ", r)
    print("Walker-Abbrueche: %d" % bad)
    print()
    print("Dokument-NAMEN in RE1.5 (DEBUG.BIN), obwohl nichts platziert ist:")
    for i in sorted(DOC_IDS):
        print("   0x%02X  %s" % (i, NM.get(i, "?")))


if __name__ == "__main__":
    main()
