#!/usr/bin/env python3
"""re2_besitz_zensus.py - Runde 37 Spur Q: wie prueft RE2 RETAIL (Leon, SLUS-00748) den BESITZ eines Gegenstands
vor einer Benutzung? Zensus ueber ALLE RE2-RDTs (info/re2leon/PL0/RDT/ROOM*.RDT, nur gelesen):

  0x5E Keep_Item_ck  (2 B: 5E id)   Handler-Tabelle 0x800A74C8 + 0x5E*4 -> 0x800584F0 (jal 0x800696CC = Inventar-Suche)
  0x62 Sce_item_lost (2 B: 62 id)   -> 0x800585E4 (Platz suchen, Id/Anzahl/Flags := 0, nachruecken)
  0x88 Sce_item_ck_lost (3 B)
  0x21 Ck auf Bank 9? (RE2-Taken-Bits liegen im Item_aot_set +18; ein Besitz-Ersatz ueber Bits waere ein Ck darauf)

Je Fundstelle: Raum/Sub/Datei-Offset, umschliessende Bedingungen, die Ops des bewachten Blocks und des ELSE-Zweigs mit
den ENGLISCHEN Raumtexten (msg/subNN.msg im entpackten Raumordner; RE2 USA: 'sub' = Englisch).

Laengentabelle: tools/re2_sicherung/re2_scd_walk.py (143 Handler ab 0x800A74C8 + Overrides, 0 Desyncs im Vollwalk).
Aufruf:  python re2_besitz_zensus.py [--json DATEI]
"""
import os, sys, glob, json, argparse, struct, collections

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "re2_sicherung"))
_cwd = os.getcwd()
os.chdir(REPO)
import re2_scd_walk as W      # noqa: E402
import re2_items              # noqa: E402
import msg_decode             # noqa: E402
os.chdir(_cwd)

RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
COND = {0x21, 0x23, 0x3E, 0x4F, 0x50, 0x5E, 0x86, 0x88}
HITS = {0x5E, 0x62, 0x88}


def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]


def item_name(i):
    try:
        return re2_items.name(i)[1] if i < re2_items.N_NAMES else "??"
    except Exception:
        return "??"


def msg_text(room, mid):
    p = os.path.join(RDT, "room%s" % room, "msg", "sub%02d.msg" % mid)
    if not os.path.exists(p):
        return "?"
    t = msg_decode.text(p)
    return t.replace("<FA>", "").replace("<FC>", " / ").replace("<NL>", "").replace("<FD>", " ")


def walk_ctx(raw, s, e):
    """[(pc, op, rec, ctx)] mit Blockstapel wie kistenregel_zensus.walk_blocks (RE2-Laengen)."""
    out = []
    p = s
    stack = [dict(kind="top", start=s, end=1 << 30, conds=[])]
    while p < e:
        op = raw[p]
        n = W.LENS.get(op)
        if n is None or p + n > e:
            break
        rec = raw[p:p + n]
        while len(stack) > 1 and p >= stack[-1]["end"]:
            stack.pop()
        ctx = [dict(kind=f["kind"], start=f["start"], end=f["end"], conds=list(f["conds"])) for f in stack]
        out.append((p, op, rec, ctx))
        if op in COND:
            stack[-1]["conds"].append((p, rec))
        if op == 0x06:
            stack.append(dict(kind="if", start=p, end=p + 4 + u16(raw, p + 2), conds=[]))
        elif op == 0x07:
            neg = stack.pop()["conds"] if len(stack) > 1 and stack[-1]["kind"] == "if" else []
            stack.append(dict(kind="else", start=p, end=p + u16(raw, p + 2), conds=[(q, b"NOT " + r) for q, r in neg]))
        p += n
    return out


def ctxt(room, c):
    p, r = c
    neg = r.startswith(b"NOT ")
    if neg:
        r = r[4:]
    op = r[0]
    if op == 0x21:
        s = "Ck(%d,0x%02X)==%d" % (r[1], r[2], r[3])
    elif op == 0x5E:
        s = "Keep_Item_ck(0x%02X %s)" % (r[1], item_name(r[1]))
    else:
        s = "%s(%s)" % (W.NAMES.get(op, "op%02X" % op), r[1:].hex(" "))
    return ("NOT " if neg else "") + s + "@0x%05X" % p


def summarize(room, ops, i0, end, limit=12):
    out = []
    for (p, op, r, ctx) in ops[i0:]:
        if p >= end:
            break
        if op == 0x2B:
            out.append("@0x%05X Message_on %d %r" % (p, r[2], msg_text(room, r[2])[:100]))
        elif op == 0x5E:
            out.append("@0x%05X Keep_Item_ck 0x%02X %s" % (p, r[1], item_name(r[1])))
        elif op == 0x62:
            out.append("@0x%05X Sce_item_lost 0x%02X %s" % (p, r[1], item_name(r[1])))
        elif op == 0x21:
            out.append("@0x%05X Ck(%d,0x%02X)==%d" % (p, r[1], r[2], r[3]))
        elif op == 0x22:
            out.append("@0x%05X Set(%d,0x%02X)=%d" % (p, r[1], r[2], r[3]))
        elif op == 0x04:
            out.append("@0x%05X Evt_exec %s" % (p, r[1:].hex(" ")))
        elif op in (0x06, 0x07, 0x08, 0x01, 0x29):
            out.append("@0x%05X %s %s" % (p, W.NAMES.get(op, "op%02X" % op), r[1:].hex(" ")))
        if len(out) >= limit:
            out.append("...")
            break
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json")
    a = ap.parse_args()
    rdts = sorted(glob.glob(os.path.join(RDT, "ROOM*.RDT")))
    hits = []
    opcount = collections.Counter()
    nblk = 0
    for path in rdts:
        room = os.path.basename(path)[4:8]
        raw = open(path, "rb").read()
        for (name, s, e) in W.rdt_blocks(raw):
            nblk += 1
            ops = walk_ctx(raw, s, e)
            for k, (p, op, r, ctx) in enumerate(ops):
                opcount[op] += 1
                if op not in HITS:
                    continue
                inner = ctx[-1]
                end = inner["end"] if inner["kind"] != "top" else e
                # Bedingung selbst: Keep_Item_ck ist ein Praedikat -> der bewachte Block ist der umschliessende
                body = summarize(room, ops, k + 1, end)
                els = []
                for (p2, op2, r2, c2) in ops:
                    if op2 == 0x07 and p2 + 4 == end:
                        j = [i for i, w in enumerate(ops) if w[0] == p2][0]
                        els = summarize(room, ops, j + 1, p2 + u16(raw, p2 + 2))
                hits.append(dict(room=room, blk=name, off=p, op=op, item=r[1], name=item_name(r[1]),
                                 ctx=[ctxt(room, c) for f in ctx for c in f["conds"]], body=body, els=els))
    print("RE2-RDTs: %d, SCD-Bloecke: %d; Keep_Item_ck %d, Sce_item_lost %d, Sce_item_ck_lost %d" % (
        len(rdts), nblk, opcount[0x5E], opcount[0x62], opcount[0x88]))
    for h in hits:
        print("\nROOM%s %s @0x%05X %s 0x%02X %s" % (h["room"], h["blk"], h["off"],
              {0x5E: "Keep_Item_ck", 0x62: "Sce_item_lost", 0x88: "Sce_item_ck_lost"}[h["op"]], h["item"], h["name"]))
        if h["ctx"]:
            print("   unter: " + " & ".join(h["ctx"]))
        for x in h["body"]:
            print("   " + x)
        if h["els"]:
            print("   ELSE:")
            for x in h["els"]:
                print("     " + x)
    if a.json:
        with open(a.json, "w") as f:
            json.dump(hits, f, indent=1)


if __name__ == "__main__":
    main()
