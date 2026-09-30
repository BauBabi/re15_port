# -*- coding: utf-8 -*-
"""Runde 34 Nacht, Spur E: Zensus "Item-Zone gegen Nachrichten-Zone" ueber ALLE RE1.5-RDTs.

Frage (ROOM1020: Marvins Notiz liegt auf dem Schreibtisch, dessen Untersuchen-Satz "It's
Lieutenant Branagh's desk." ein Nachrichten-AOT in Slot 10 ist): wie ordnet das ORIGINAL einen
aufhebbaren Gegenstand und eine Untersuchen-Nachricht, deren Rechtecke sich ueberschneiden?
Der Aktions-Scan FUN_80042bac feuert je Druck genau EINEN Satz, den mit dem kleinsten Slot
(Port: aot_common.c re15_aot_scan, action_fired). Liegt der Gegenstand im kleineren Slot, gewinnt
er; sonst die Nachricht.

Walker: dieselben Laengen wie re15_port/tools/scd_dump_room.py (aot_sce_census.py / scd_vm.c
s_opcode_sizes; 0x2C 20/28, 0x3B 32/40, 0x50 22/30 nach pc[3]&0x80). Gezaehlt werden alle
Aot_set mit sce 1 (Nachricht) und alle Item_aot_set in main00 UND allen subs, je Raum; Paare mit
sich schneidenden Rechtecken werden mit Slot-Reihenfolge ausgegeben. Unbekannter Opcode ->
Abbruch des Blocks (gezaehlt, nicht still).

Aufruf: python re15_port/tools/r34n_e/zensus_item_nachricht.py [CD-Wurzel]
"""
import glob
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
CD = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "re15_port", "shared_assets", "PSX")

SIZES = {
    0x00: 1, 0x01: 2, 0x02: 1, 0x03: 4, 0x04: 4, 0x05: 2, 0x06: 4, 0x07: 4,
    0x08: 2, 0x09: 4, 0x0A: 3, 0x0B: 1, 0x0C: 1, 0x0D: 6, 0x0E: 2, 0x0F: 4,
    0x10: 2, 0x11: 4, 0x12: 2, 0x13: 4, 0x14: 6, 0x15: 4, 0x16: 2, 0x17: 6,
    0x18: 2, 0x19: 2, 0x1A: 2, 0x1B: 6, 0x1C: 1, 0x1D: 1, 0x1E: 1,
    0x20: 1, 0x21: 4, 0x22: 4, 0x23: 6, 0x24: 4, 0x25: 3, 0x26: 6, 0x27: 4,
    0x28: 1, 0x29: 2, 0x2A: 1, 0x2B: 4, 0x2C: 20, 0x2D: 34, 0x2E: 3, 0x2F: 4,
    0x30: 1, 0x31: 1, 0x32: 8, 0x33: 8, 0x34: 4, 0x35: 3, 0x36: 12, 0x37: 4,
    0x38: 12, 0x39: 4, 0x3A: 16, 0x3B: 32, 0x3C: 2, 0x3D: 3, 0x3E: 6, 0x3F: 4,
    0x40: 8, 0x41: 10, 0x42: 1, 0x43: 4, 0x44: 20, 0x45: 3, 0x46: 10, 0x47: 2,
    0x48: 16, 0x49: 8, 0x4A: 2, 0x4B: 3, 0x4C: 18, 0x4D: 10, 0x4E: 5, 0x4F: 22,
    0x50: 22, 0x51: 4, 0x52: 4, 0x53: 3, 0x54: 6, 0x55: 6, 0x56: 6, 0x57: 4,
    0x58: 4, 0x59: 4, 0x5A: 6, 0x5B: 4, 0x5C: 4, 0x5D: 4, 0x5E: 4,
}


def u16(b, o):
    return b[o] | (b[o + 1] << 8)


def s16(b, o):
    return struct.unpack_from("<h", b, o)[0]


def u32(b, o):
    return struct.unpack_from("<I", b, o)[0]


def op_size(d, pc):
    op = d[pc]
    if op >= 0x5F or op == 0x1F:
        return None
    if op == 0x2C:
        return 28 if (d[pc + 3] & 0x80) else 20
    if op == 0x3B:
        return 40 if (d[pc + 3] & 0x80) else 32
    if op == 0x50:
        return 30 if (d[pc + 3] & 0x80) else 22
    return SIZES.get(op)


def regionen(d, s):
    if s == 0 or s + 2 > len(d):
        return []
    first = u16(d, s)
    if first < 2 or first % 2 or s + first > len(d):
        return []
    tbl = [u16(d, s + 2 * i) for i in range(first // 2)]
    ends = []
    for o in range(0x40, 0x60, 4):
        v = u32(d, o)
        if s + max(tbl) < v <= len(d):
            ends.append(v)
    se = min(ends) if ends else len(d)
    so = sorted(set(o for o in tbl if o and s + o < len(d)))
    out = []
    for i, o in enumerate(so):
        e = so[i + 1] if i + 1 < len(so) else se - s
        out.append((s + o, s + min(e, se - s), tbl.index(o)))
    return out


def walk(d, a, e):
    pc = a
    recs = []
    while pc < e:
        sz = op_size(d, pc)
        if sz is None or pc + sz > e:
            return recs, False
        op = d[pc]
        if op == 0x2C and d[pc + 2] == 1:
            recs.append(("MSG", d[pc + 1], s16(d, pc + 6), s16(d, pc + 8), s16(d, pc + 10), s16(d, pc + 12),
                         u16(d, pc + 14), pc))
        if op == 0x50:
            recs.append(("ITEM", d[pc + 1], s16(d, pc + 6), s16(d, pc + 8), s16(d, pc + 10), s16(d, pc + 12),
                         u16(d, pc + 14), pc))
        pc += sz
    return recs, True


def main():
    rooms = sorted(glob.glob(os.path.join(CD, "STAGE*", "ROOM*.RDT")))
    n_blocks = n_abbruch = 0
    paare = []
    for p in rooms:
        d = open(p, "rb").read()
        if len(d) < 0x60:
            continue                                  # Platzhalter-Datei ohne RDT-Kopf
        recs = []
        for sec in (u32(d, 0x40), u32(d, 0x44)):
            for a, e, idx in regionen(d, sec):
                r, ok = walk(d, a, e)
                n_blocks += 1
                n_abbruch += (not ok)
                recs += r
        items = [r for r in recs if r[0] == "ITEM"]
        msgs = [r for r in recs if r[0] == "MSG"]
        for it in items:
            for m in msgs:
                ax0, az0, ax1, az1 = it[2], it[3], it[2] + it[4], it[3] + it[5]
                bx0, bz0, bx1, bz1 = m[2], m[3], m[2] + m[4], m[3] + m[5]
                if it[4] == 0 or it[5] == 0:
                    continue                          # Null-Rechteck (nur per Aot_on)
                if ax0 < bx1 and bx0 < ax1 and az0 < bz1 and bz0 < az1:
                    paare.append((os.path.basename(p)[:8], it, m))
    print("RDTs %d, Bloecke %d, davon abgebrochen %d" % (len(rooms), n_blocks, n_abbruch))
    vor = sum(1 for _, it, m in paare if it[1] < m[1])
    nach = sum(1 for _, it, m in paare if it[1] > m[1])
    gleich = sum(1 for _, it, m in paare if it[1] == m[1])
    print("Paare Item-Rechteck x Nachrichten-Rechteck mit Ueberschneidung: %d" % len(paare))
    print("   Item im KLEINEREN Slot (Item gewinnt): %d | Nachricht im kleineren Slot: %d | gleicher Slot: %d"
          % (vor, nach, gleich))
    for r, it, m in paare:
        print("  %s  ITEM slot %2d Id 0x%02X @0x%05X rect(%d,%d,%d,%d)  |  MSG slot %2d msg %d @0x%05X rect(%d,%d,%d,%d)  -> %s"
              % (r, it[1], it[6] & 0xFF, it[7], it[2], it[3], it[4], it[5], m[1], m[6], m[7], m[2], m[3], m[4], m[5],
                 "Item gewinnt" if it[1] < m[1] else ("NACHRICHT gewinnt" if it[1] > m[1] else "gleicher Slot")))


if __name__ == "__main__":
    main()
