#!/usr/bin/env python3
"""kistenregel_zensus.py - Runde 37 Spur Q: ZENSUS aller Original-Abfragen, die den BESITZ eines Gegenstands
ueber ein BIT statt ueber das Inventar pruefen (Leon und Elza, alle RDTs).

Hintergrund (Dossier analysis/befunde_runde37/Q_kistenregel.md): RE1.5 hat keine Item-Box ("Itembox is not
available in this preview"); ein aufgenommener Gegenstand verlaesst das Inventar dort nie (kein Opcode entfernt
Gegenstaende, Dispatch 0x00..0x5E; Keep_Item_ck 0x5E = LAB_80042b04 ist KEIN Praedikat). Deshalb prueft das
Original den Besitz ueber
  (T) das TAKEN-Bit der Aufnahme (Bank 9, Item_aot_set pc[18] @0x80040680, gesetzt @0x8001e0d0/@0x80021fd8),
  (A) ein ABGELEITETES Bit, das ein Raumskript setzt, sobald das Taken-Bit steht (Muster ROOM1000 sub01
      @0x00D00 Ck(9,0x86)==1 -> @0x00D0C Set(3,0x85)=1),
  (S) ein SKRIPT-Bit fuer Gegenstaende, die gar nicht ins Inventar gehen ("You've taken the Fuse." ROOM2030
      sub06 Set(3,0x6C)).
Mit der RE2-Item-Box des Ports (Nutzer-Auftrag 2026-08-30) ist "Bit gesetzt" nicht mehr "im Inventar".

Ausgabe:
  1. je Gegenstand: Platzierungen (Item_aot_set), Taken-Bit
  2. abgeleitete Bits (A): jedes Set(b,i,1), das unter einem Ck(9,tk)==1 steht
  3. Skript-Bits (S): jedes Set(b,i,1) in einem Block mit "taken the <Name>"-Nachricht
  4. JEDE Ck-Stelle auf einem T/A/S-Bit mit Kontext (umschliessende Bedingungen, Nachrichten des bewachten
     Blocks, Else-Zweig) -> daraus die Fall-Tabelle im Dossier (Entscheidung je Fall von Hand, mit Beleg).

Walk: die EINE Laengentabelle aus tools/scd_dump_room.py (ueber tools/scd_walk_lib.py); Bloecke: Ifel_ck
(0x06, Ende pc+4+u16), Else_ck (0x07, Ende pc+s16), While/Do/Switch (pc+4+s16), For (pc+6+s16).

Aufruf:  python kistenregel_zensus.py [ASSET_ROOT] [--json DATEI]
"""
import os, sys, glob, json, argparse, collections

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.abspath(os.path.join(HERE, ".."))
sys.path.insert(0, TOOLS)
import scd_walk_lib as W                      # noqa: E402  (Laengentabelle = scd_dump_room.py)
import struct                                 # noqa: E402

# Item-Namen: DEBUG.BIN (Basis 0x800C0000), Offsettabelle @0x800C495C, Blob @0x800C4A28, 0x07-Ende —
# dieselbe Lesart wie tools/gen_discard_sites.py item_names() (dort NICHT importierbar: die Datei ruft beim
# Import main() auf und schreibt engine/src/gen/discard_sites.inc neu).
_DEBUG = os.path.join(TOOLS, "..", "shared_assets", "PSX", "BIN", "DEBUG.BIN")


def item_names():
    d = open(_DEBUG, "rb").read()
    base, off, blob = 0x800C0000, 0x800C495C, 0x800C4A28
    n = (blob - off) // 2
    offs = struct.unpack_from("<%dH" % n, d, off - base)
    out = {}
    for i, o in enumerate(offs):
        p = blob - base + o
        s = ""
        while d[p] != 0x07:
            g = W.glyph(d[p])
            s += g if g else "?"
            p += 1
        out[i] = s
    return n, out

COND_OPS = {0x21, 0x23, 0x3E, 0x51, 0x52, 0x5E}   # Ck, Cmp, Member_cmp, Sce_key_ck, Sce_espr_control, Keep_Item_ck
BLOCK_OPS = {0x06, 0x07, 0x0D, 0x0F, 0x11, 0x13, 0x1B}


def walk_blocks(d, ops):
    """Je Op: (pc, op, size, ctx). ctx = Liste der offenen Bloecke (aussen->innen), je {kind, start, end,
    conds:[(pc,bytes)] bis zu diesem Op}. Ein Ck, das ausserhalb jedes Blocks steht, zaehlt zur Ebene 'top'."""
    res = []
    stack = [dict(kind="top", start=ops[0][0] if ops else 0, end=1 << 30, conds=[])]
    for (pc, op, sz) in ops:
        while len(stack) > 1 and pc >= stack[-1]["end"]:
            stack.pop()
        ctx = [dict(kind=f["kind"], start=f["start"], end=f["end"], conds=list(f["conds"])) for f in stack]
        res.append((pc, op, sz, ctx))
        if op in COND_OPS:
            stack[-1]["conds"].append((pc, bytes(d[pc:pc + sz])))
        if op == 0x06:
            stack.append(dict(kind="if", start=pc, end=pc + 4 + W.u16(d, pc + 2), conds=[]))
        elif op == 0x07:
            # Else_ck steht am ENDE des If-Koerpers; sein Koerper beginnt bei pc+4, endet bei pc+s16.
            # Die Bedingungen des If gelten dort NEGIERT -> als eigene Ebene 'else' mit Verweis.
            if len(stack) > 1 and stack[-1]["kind"] == "if":
                negated = stack[-1]["conds"]
                stack.pop()
            else:
                negated = []
            stack.append(dict(kind="else", start=pc, end=pc + W.s16(d, pc + 2),
                              conds=[(p, b"NOT " + c) for (p, c) in negated]))
        elif op in (0x0F, 0x11, 0x13):
            stack.append(dict(kind="loop%02x" % op, start=pc, end=pc + 4 + W.s16(d, pc + 2), conds=[]))
        elif op in (0x0D, 0x1B):
            stack.append(dict(kind="for", start=pc, end=pc + 6 + W.s16(d, pc + 2), conds=[]))
    return res


def ck_of(b):
    """(bank, bit, val) eines Ck-Satzes `21 bank bit val`, sonst None."""
    if len(b) >= 4 and b[0] == 0x21:
        return (b[1], b[2], b[3])
    return None


def cond_txt(c):
    pc, b = c
    neg = b.startswith(b"NOT ")
    if neg:
        b = b[4:]
    if b[0] == 0x21:
        s = "Ck(%d,0x%02X)==%d" % (b[1], b[2], b[3])
    elif b[0] == 0x23:
        s = "Cmp(%s)" % b[1:].hex(" ")
    else:
        s = "%s(%s)" % (W.NAMES.get(b[0], "op%02X" % b[0]), b[1:].hex(" "))
    return ("NOT " if neg else "") + s + "@0x%05X" % pc


def effects(d, wops, i0, end, msgs, limit=14):
    """Kurzfassung der Ops ab Index i0 bis Datei-Offset end."""
    out = []
    for (pc, op, sz, ctx) in wops[i0:]:
        if pc >= end:
            break
        b = d[pc:pc + sz]
        if op == 0x2B:
            t = msgs.get(b[1], ("?", []))[0]
            out.append("@0x%05X Message_on %d %r" % (pc, b[1], t[:90]))
        elif op == 0x22:
            out.append("@0x%05X Set(%d,0x%02X)=%d" % (pc, b[1], b[2], b[3]))
        elif op == 0x21:
            out.append("@0x%05X Ck(%d,0x%02X)==%d" % (pc, b[1], b[2], b[3]))
        elif op == 0x04:
            out.append("@0x%05X Evt_exec sub%02d" % (pc, b[3]))
        elif op in (0x2C, 0x46):
            out.append("@0x%05X %s slot %d sce %d (%s)" % (pc, W.NAMES.get(op), b[1], b[2], b[3:].hex(" ")[:44]))
        elif op == 0x47:
            out.append("@0x%05X Aot_on %d" % (pc, b[1]))
        elif op == 0x3B:
            out.append("@0x%05X Door_aot_set slot %d" % (pc, b[1]))
        elif op == 0x50:
            out.append("@0x%05X Item_aot_set slot %d" % (pc, b[1]))
        elif op in (0x06, 0x07, 0x08, 0x01):
            out.append("@0x%05X %s" % (pc, W.NAMES.get(op)))
        if len(out) >= limit:
            out.append("...")
            break
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root", nargs="?", default=os.path.join(TOOLS, "..", "shared_assets", "PSX"))
    ap.add_argument("--json")
    a = ap.parse_args()
    n_names, names = item_names()
    rdts = sorted(glob.glob(os.path.join(a.root, "STAGE*", "ROOM*.RDT")))

    rooms = {}
    n_hdr = n_stub = 0
    for p in rdts:
        d = open(p, "rb").read()
        room = os.path.basename(p).split(".")[0]
        if len(d) < 0x48:
            n_stub += 1
            continue
        n_hdr += 1
        regs = W.regionen(d)
        rooms[room] = dict(d=d, msgs=W.messages(d),
                           regs={k: walk_blocks(d, v) for k, v in regs.items() if v})

    # 1. Platzierungen
    items = collections.defaultdict(list)   # type -> [(room, sub, pc, slot, tk_bit, amount)]
    tk2item = collections.defaultdict(set)
    for room, R in rooms.items():
        d = R["d"]
        for (tag, idx), wops in R["regs"].items():
            for (pc, op, sz, ctx) in wops:
                if op != 0x50:
                    continue
                lf = (d[pc + 3] & 0x80) != 0
                typ = d[pc + 22] if lf else d[pc + 14]
                amt = d[pc + 24] if lf else d[pc + 16]
                tk = d[pc + 26] if lf else d[pc + 18]
                items[typ].append((room, "%s%02d" % (tag, idx), pc, d[pc + 1], tk, amt))
                tk2item[tk].add(typ)

    # 2. abgeleitete Bits (A): Set(b,i,1) unter Ck(9,tk)==1
    derived = collections.defaultdict(list)  # (b,i) -> [(room, sub, pc, tk, typset)]
    # 3. Skript-Bits (S): Set(b,i,1) im selben Block wie "taken the <X>"
    scripted = collections.defaultdict(list)
    for room, R in rooms.items():
        d, msgs = R["d"], R["msgs"]
        for (tag, idx), wops in R["regs"].items():
            sub = "%s%02d" % (tag, idx)
            for k, (pc, op, sz, ctx) in enumerate(wops):
                if op != 0x22 or d[pc + 3] != 1:
                    continue
                allc = [c for f in ctx for c in f["conds"]]
                for (cpc, cb) in allc:
                    ck = ck_of(cb)
                    if ck and ck[0] == 9 and ck[2] == 1 and ck[1] in tk2item:
                        derived[(d[pc + 1], d[pc + 2])].append((room, sub, pc, ck[1], sorted(tk2item[ck[1]]), cpc))
                # Nachrichten im selben innersten Block
                inner = ctx[-1]
                for (pc2, op2, sz2, ctx2) in wops:
                    if op2 == 0x2B and inner["start"] <= pc2 < inner["end"] and ctx2[-1]["start"] == inner["start"]:
                        t = msgs.get(d[pc2 + 1], ("", []))[0]
                        if "taken the" in t or "take the" in t:
                            scripted[(d[pc + 1], d[pc + 2])].append((room, sub, pc, d[pc2 + 1], t))

    # 4. Ck-Stellen auf T/A/S-Bits
    tbits = {(9, tk) for tk in tk2item}
    abits = set(derived)
    sbits = set(scripted)
    sites = []
    for room, R in sorted(rooms.items()):
        d, msgs = R["d"], R["msgs"]
        for (tag, idx), wops in sorted(R["regs"].items()):
            sub = "%s%02d" % (tag, idx)
            for k, (pc, op, sz, ctx) in enumerate(wops):
                if op != 0x21:
                    continue
                key = (d[pc + 1], d[pc + 2])
                kind = "T" if key in tbits else "A" if key in abits else "S" if key in sbits else None
                if not kind:
                    continue
                # bewachter Bereich: vom Ck bis zum Ende des innersten Blocks; dazu ein Else direkt dahinter
                inner = ctx[-1]
                end = inner["end"] if inner["kind"] != "top" else wops[-1][0] + wops[-1][2]
                eff = effects(d, wops, k + 1, end, msgs)
                els = []
                for (pc2, op2, sz2, ctx2) in wops:
                    if op2 == 0x07 and pc2 + 4 == end:
                        j = [i for i, w in enumerate(wops) if w[0] == pc2][0]
                        els = effects(d, wops, j + 1, pc2 + W.s16(d, pc2 + 2), msgs)
                sites.append(dict(room=room, sub=sub, pc=pc, bank=key[0], bit=key[1], val=d[pc + 3], kind=kind,
                                  ctx=[cond_txt(c) for f in ctx for c in f["conds"]], eff=eff, els=els))

    # ---------------- Ausgabe ----------------
    print("RDTs mit Kopf: %d, Stummel: %d; Item-Namen: %d (DEBUG.BIN)" % (n_hdr, n_stub, n_names))
    print("\n=== 1. Gegenstaende mit Platzierung (Item_aot_set 0x50) ===")
    for typ in sorted(items):
        recs = items[typ]
        print("0x%02X %-26s %s" % (typ, names.get(typ, "?"),
              "; ".join("%s/%s@0x%05X slot%d tk(9,%d) x%d" % r for r in recs)))
    print("\n=== 2. Abgeleitete Bits (A): Set(b,i)=1 unter Ck(9,tk)==1 ===")
    for key in sorted(derived):
        for (room, sub, pc, tk, typs, cpc) in derived[key]:
            print("(%d,0x%02X) <- %s/%s Set@0x%05X unter Ck(9,0x%02X)@0x%05X  Item %s" % (
                key[0], key[1], room, sub, pc, tk, cpc, ",".join("0x%02X %s" % (t, names.get(t, "?")) for t in typs)))
    print("\n=== 3. Skript-Bits (S): Set(b,i)=1 im Block einer 'take(n) the'-Nachricht ===")
    for key in sorted(scripted):
        for (room, sub, pc, mid, t) in scripted[key]:
            print("(%d,0x%02X) <- %s/%s Set@0x%05X  msg %d %r" % (key[0], key[1], room, sub, pc, mid, t[:80]))
    print("\n=== 4. Ck-Stellen auf T/A/S-Bits: %d ===" % len(sites))
    for s in sites:
        print("\n%s/%s @0x%05X  Ck(%d,0x%02X)==%d  [%s]" % (s["room"], s["sub"], s["pc"], s["bank"], s["bit"],
                                                          s["val"], s["kind"]))
        if s["ctx"]:
            print("   unter: " + " & ".join(s["ctx"]))
        for e in s["eff"]:
            print("   " + e)
        if s["els"]:
            print("   ELSE:")
            for e in s["els"]:
                print("     " + e)
    # 5. Gegenprobe von der TEXT-Seite: jede Message_on-Stelle, deren Text eine Benutzung nennt ("use"/"used"/
    #    "insert"), mit ihren umschliessenden Bedingungen. Steht dort KEIN T/A/S-Bit, prueft die Stelle den Besitz
    #    gar nicht (oder ueber etwas anderes) -> im Dossier je Stelle erklaeren.
    print()
    print("=== 5. Benutzungs-Nachrichten (Text-Seite) mit ihren Bedingungen ===")
    keys = ("will you use", "use the", "used the", "insert", "operate")
    for room, R in sorted(rooms.items()):
        d, msgs = R["d"], R["msgs"]
        for (tag, idx), wops in sorted(R["regs"].items()):
            sub = "%s%02d" % (tag, idx)
            for (pc, op, sz, ctx) in wops:
                if op != 0x2B:
                    continue
                t = msgs.get(d[pc + 1], ("", []))[0]
                if not any(k in t.lower() for k in keys):
                    continue
                conds = [cond_txt(c) for f in ctx for c in f["conds"]]
                tas = [c for c in conds if any(("Ck(%d,0x%02X)" % kk) in c for kk in (tbits | abits | sbits))]
                print("%s/%s @0x%05X msg %d %r" % (room, sub, pc, d[pc + 1], t[:70]))
                print("   unter: %s" % (" & ".join(conds) or "-"))
                print("   T/A/S: %s" % (", ".join(tas) or "KEIN"))
    if a.json:
        with open(a.json, "w") as f:
            json.dump(dict(items={"0x%02X" % k: v for k, v in items.items()},
                           derived={"%d,%d" % k: v for k, v in derived.items()},
                           scripted={"%d,%d" % k: v for k, v in scripted.items()},
                           sites=sites), f, indent=1)


if __name__ == "__main__":
    main()
