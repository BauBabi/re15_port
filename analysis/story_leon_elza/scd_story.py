#!/usr/bin/env python3
"""scd_story.py - SCD-Zensus fuer die Story-Erhebung (Leon xxx0 / Elza xxx1).

Walker = re15_port/tools/scd_walk_lib.py (EINE Laengentabelle, aus scd_dump_room.py).
Zusaetzlich: Bedingungsstapel (Ifel_ck/Else_ck/Endif, Switch/Case) je Instruktion, damit
jede Aussage "X passiert, wenn Flag Y" mit Datei-Offset belegt ist.

Feldlagen (alle im Port belegt):
  0x44 Sce_em_set 20 B: [1] Slot [2] Typ [3] Verhalten, Pos s16 @+8/+10/+12, Yaw @+16
        (scd_vm.c:3168ff, RE1.5-Handler 0x800420A0, `addiu v1,v1,20` @0x8004262c)
  0x2B Message_on: [1] Nachrichten-Id
  0x21 Ck / 0x22 Set: [1] Bank [2] Bit [3] Wert
  0x3B Door_aot_set: Nutzlast pc+14 (Viereck pc+22): +8 Stage(0-basiert) +9 Raum +10 Cut
        (scd_vm.c op_door_aot_set; Warp FUN_8001d600). Ziel-Id = ((stage+1)<<12)|(raum<<4)|Variante
  0x2C Aot_set: [2] sce (1 = Text, Nutzlast u16 @+14 = Nachricht; 0/3 = Ereignis, Nutzlast
        +16 Typ / +17 Sub)   0x46 Aot_reset: [2] sce, Nutzlast @+4 (Text: u16 Nachricht;
        Ereignis: +6 Typ / +7 Sub)
  0x50 Item_aot_set: u16 @+14 Item, u16 @+16 Anzahl, u16 @+18 Aufnahme-Flag
  0x04 Evt_exec: [3] Sub    0x18 Gosub: [1] Sub
  0x13 Switch [1] = work_vars-Index (0x0A = angezeigter/Eintritts-Cut, scd_room_setup.c:411)

Aufruf:  python scd_story.py            -> census.json + scd_annot/ROOMxxxx.txt (Story-Raeume)
         python scd_story.py 1011 1211  -> nur diese Raeume annotieren (stdout)
"""
import os, sys, json, glob
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L  # noqa: E402

PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
ITEMS = {}
try:
    ITEMS = {int(k, 16): v for k, v in json.load(open(os.path.join(HERE, "item_namen.json"))).items()}
except Exception:
    pass

COND_OPS = (0x21, 0x23, 0x3E, 0x4F, 0x51, 0x5E)


def room_path(rid):
    st = int(rid[0], 16)
    return os.path.join(PSX, "STAGE%d" % st, "ROOM%s.RDT" % rid)


def fmt_cond(d, pc):
    op = d[pc]
    if op == 0x21:
        return "Ck(%d,0x%02X)==%d" % (d[pc + 1], d[pc + 2], d[pc + 3])
    if op == 0x23:
        return "Cmp(var%02X op%d %d)" % (d[pc + 2], d[pc + 3], L.s16(d, pc + 4))
    if op == 0x3E:
        return "Member_cmp(m%02X op%d %d)" % (d[pc + 2], d[pc + 3], L.s16(d, pc + 4))
    if op == 0x5E:
        return "Keep_Item_ck(0x%02X %s)" % (d[pc + 1], ITEMS.get(d[pc + 1], "?"))
    if op == 0x51:
        return "Sce_key_ck(%02x %02x %02x)" % (d[pc + 1], d[pc + 2], d[pc + 3])
    if op == 0x4F:
        return "Op4F(...)"
    return "?"


def describe(d, pc, op, rid, msgs):
    """Kurztext + strukturierter Eintrag fuer story-relevante Opcodes (sonst None)."""
    var = int(rid[-1], 16)
    if op == 0x44:
        t = d[pc + 2]
        e = dict(k="em", slot=d[pc + 1], typ=t, beh=d[pc + 3],
                 pos=(L.s16(d, pc + 8), L.s16(d, pc + 10), L.s16(d, pc + 12)), yaw=L.u16(d, pc + 16),
                 b4=d[pc + 4], b5=d[pc + 5], b6=d[pc + 6], b7=d[pc + 7])
        park = " GEPARKT" if e["pos"][0] <= -30000 else ""
        return e, "Sce_em_set slot=%d TYP=0x%02X beh=0x%02X pos=%s yaw=%d b4..7=%02x %02x %02x %02x%s" % (
            e["slot"], t, e["beh"], e["pos"], e["yaw"], e["b4"], e["b5"], e["b6"], e["b7"], park)
    if op == 0x2B:
        m = d[pc + 1]
        txt = msgs.get(m, ("<?>", []))[0]
        return dict(k="msg", id=m, text=txt), "Message_on %d  \"%s\"" % (m, txt)
    if op == 0x22:
        return dict(k="set", bank=d[pc + 1], bit=d[pc + 2], val=d[pc + 3]), "SET (%d,0x%02X)=%d" % (
            d[pc + 1], d[pc + 2], d[pc + 3])
    if op == 0x21:
        return dict(k="ck", bank=d[pc + 1], bit=d[pc + 2], val=d[pc + 3]), None
    if op == 0x3B:
        quad = (d[pc + 3] & 0x80) != 0
        nl = pc + (22 if quad else 14)
        st, rm, cut = d[nl + 8], d[nl + 9], d[nl + 10]
        dest = "%X%X%X" % (st + 1, rm >> 4 & 0xF, rm & 0xF) if rm < 0x100 else "?"
        dest = "%d%02X%d" % (st + 1, rm, var)
        e = dict(k="door", slot=d[pc + 1], sce=d[pc + 2], dest=dest, cut=cut)
        return e, "Door_aot_set slot=%d sce=%d -> ROOM%s cut=%d%s" % (
            d[pc + 1], d[pc + 2], dest, cut, "  (sce0 = INERT)" if d[pc + 2] == 0 else "")
    if op == 0x2C:
        sce = d[pc + 2]
        if sce == 1:
            m = L.u16(d, pc + 14)
            txt = msgs.get(m, ("<?>", []))[0]
            return dict(k="aot_text", slot=d[pc + 1], msg=m, text=txt), "Aot_set slot=%d TEXT msg %d \"%s\"" % (
                d[pc + 1], m, txt[:90])
        return dict(k="aot", slot=d[pc + 1], sce=sce, sub=d[pc + 17]), "Aot_set slot=%d sce=%d ev->sub%02d (%02x %02x %02x %02x)" % (
            d[pc + 1], sce, d[pc + 17], d[pc + 14], d[pc + 15], d[pc + 16], d[pc + 17])
    if op == 0x46:
        sce = d[pc + 2]
        if sce == 1:
            m = L.u16(d, pc + 4)
            txt = msgs.get(m, ("<?>", []))[0]
            return dict(k="aot_reset_text", slot=d[pc + 1], msg=m), "Aot_reset slot=%d TEXT msg %d \"%s\"" % (
                d[pc + 1], m, txt[:90])
        if sce == 2:
            return dict(k="aot_reset", slot=d[pc + 1], sce=sce), "Aot_reset slot=%d sce=2 (TUER scharf)" % d[pc + 1]
        return dict(k="aot_reset", slot=d[pc + 1], sce=sce, sub=d[pc + 7]), "Aot_reset slot=%d sce=%d ev->sub%02d" % (
            d[pc + 1], sce, d[pc + 7])
    if op == 0x50:
        it = L.u16(d, pc + 14)
        return dict(k="item", slot=d[pc + 1], item=it, n=L.u16(d, pc + 16), flag=L.u16(d, pc + 18)), \
            "Item_aot_set slot=%d ITEM 0x%02X %s x%d flag=%d" % (d[pc + 1], it, ITEMS.get(it, "?"),
                                                                  L.u16(d, pc + 16), L.u16(d, pc + 18))
    if op == 0x04:
        return dict(k="evt", sub=d[pc + 3]), "Evt_exec -> sub%02d" % d[pc + 3]
    if op == 0x18:
        return dict(k="gosub", sub=d[pc + 1]), "Gosub -> sub%02d" % d[pc + 1]
    if op == 0x2E:
        kind = d[pc + 1]
        # scd_vm.c scd_work_bind (LAB_80040d2c): 1 Spieler, 2 NPC/Gegner-Slot idx, 3 Prop obj_id
        who = {1: "SPIELER", 2: "NPC-Slot %d" % d[pc + 2], 3: "Prop obj %d" % d[pc + 2]}.get(kind, "kind%d" % kind)
        return None, "Work_set -> %s" % who
    if op == 0x40:
        return None, "Plc_dest mode=%d (%d,%d)" % (d[pc + 2], L.s16(d, pc + 4), L.s16(d, pc + 6))
    if op == 0x32:
        return None, "Pos_set (%d,%d,%d)" % (L.s16(d, pc + 2), L.s16(d, pc + 4), L.s16(d, pc + 6))
    if op == 0x3F:
        return None, "Plc_motion %d,%d,%d" % (d[pc + 1], d[pc + 2], d[pc + 3])
    if op == 0x34:
        return None, "Member_set m=0x%02X val=%d" % (d[pc + 1], L.s16(d, pc + 2))
    if op == 0x29:
        return None, "Cut_chg %d" % d[pc + 1]
    if op == 0x2D:
        return dict(k="obj", obj=d[pc + 1]), "Obj_model_set obj=%d pos=(%d,%d,%d)" % (
            d[pc + 1], L.s16(d, pc + 10), L.s16(d, pc + 12), L.s16(d, pc + 14))
    if op == 0x53:
        return None, "Sce_fade_set %02x %02x" % (d[pc + 1], d[pc + 2])
    if op == 0x59:
        return None, "Xa_on %02x %02x %02x" % (d[pc + 1], d[pc + 2], d[pc + 3])
    if op == 0x5A:
        return None, "Weapon_chg %s" % " ".join("%02x" % b for b in d[pc + 1:pc + 6])
    return None, None


def walk_region(d, start, end, rid, msgs):
    """[(pc, op, size, conds(list[str]), entry, text)]"""
    ops = L.walk_ops(d, start, end)
    stack = []   # frames: dict(kind, end, conds, neg, case)
    out = []
    prev_if = None
    for (pc, op, sz) in ops:
        # Rahmen schliessen, deren Ende erreicht ist
        while stack and stack[-1]["end"] is not None and pc >= stack[-1]["end"] and stack[-1]["kind"] != "switch":
            stack.pop()
        while stack and stack[-1]["kind"] == "switch" and pc >= stack[-1]["end"]:
            stack.pop()
        if prev_if is not None and op in COND_OPS:
            prev_if["conds"].append(fmt_cond(d, pc))
        else:
            prev_if = None
        conds = []
        for f in stack:
            if f["kind"] == "if" and f["conds"]:
                c = " && ".join(f["conds"])
                conds.append(("!(%s)" % c) if f["neg"] else c)
            if f["kind"] == "switch" and f.get("case") is not None:
                conds.append("var%02X==%d" % (f["var"], f["case"]))
        entry, text = describe(d, pc, op, rid, msgs)
        out.append((pc, op, sz, list(conds), entry, text, len(stack)))
        if op == 0x06:
            fr = dict(kind="if", end=pc + 4 + L.u16(d, pc + 2), conds=[], neg=False)
            stack.append(fr)
            prev_if = fr
        elif op == 0x07:
            # Else: der If-Rahmen endet hier; neuer negierter Rahmen bis zum Sprungziel
            tgt = L.fwd_target(d, pc, op)   # = pc + s16(pc+2), dieselbe Regel wie scd_dump_room
            for i in range(len(stack) - 1, -1, -1):
                if stack[i]["kind"] == "if":
                    stack[i]["neg"] = True
                    stack[i]["end"] = tgt
                    break
        elif op == 0x08:
            for i in range(len(stack) - 1, -1, -1):
                if stack[i]["kind"] == "if":
                    del stack[i]
                    break
        elif op == 0x13:
            stack.append(dict(kind="switch", var=d[pc + 1], end=pc + 4 + L.u16(d, pc + 2), conds=[], case=None))
        elif op == 0x14:
            for f in reversed(stack):
                if f["kind"] == "switch":
                    f["case"] = L.s16(d, pc + 4)
                    break
        elif op == 0x16:
            for i in range(len(stack) - 1, -1, -1):
                if stack[i]["kind"] == "switch":
                    del stack[i]
                    break
    return out


def analyse(rid):
    p = room_path(rid)
    d = open(p, "rb").read()
    if len(d) < 0x60:
        return None
    msgs = L.messages(d)
    res = dict(room=rid, regions={})
    ms, ss = L.u32(d, 0x40), L.u32(d, 0x44)
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d):
            continue
        se = L.rdt_section_end(d, sec)
        for (o, e, idx) in L.section_regions(d, sec, se):
            res["regions"]["%s%02d" % (tag, idx)] = walk_region(d, sec + o, sec + e, rid, msgs)
    return res, d, msgs


KURZ_OPS = {0x01, 0x04, 0x06, 0x07, 0x08, 0x13, 0x14, 0x16, 0x21, 0x22, 0x2B, 0x2C, 0x3B, 0x44, 0x46,
            0x50, 0x32, 0x2D, 0x59, 0x5A, 0x3E, 0x5E, 0x23, 0x51, 0x4F}


def annot_text(rid, kurz=False):
    r = analyse(rid)
    if r is None:
        return "ROOM%s: Platzhalter (4 B)\n" % rid
    res, d, msgs = r
    lines = ["ROOM%s  (%s)" % (rid, os.path.relpath(room_path(rid), REPO).replace("\\", "/"))]
    for reg, ops in res["regions"].items():
        lines.append("\n=== %s" % reg)
        for (pc, op, sz, conds, entry, text, depth) in ops:
            if kurz and op not in KURZ_OPS:
                continue
            raw = " ".join("%02x" % b for b in d[pc:pc + min(sz, 12)]) + (" .." if sz > 12 else "")
            name = L.NAMES.get(op, "?")
            ind = "  " * depth
            s = "  0x%05X %s%-14s %-40s" % (pc, ind, name, raw)
            if text:
                s += "  ; " + text
            if op in COND_OPS:
                s += "  ; " + fmt_cond(d, pc)
            lines.append(s)
    return "\n".join(lines) + "\n"


def all_rooms():
    out = []
    for st in range(1, 8):
        for p in sorted(glob.glob(os.path.join(PSX, "STAGE%d" % st, "ROOM*.RDT"))):
            out.append(os.path.basename(p)[4:8])
    return out


def main():
    if len(sys.argv) > 1:
        kurz = "--kurz" in sys.argv
        for rid in sys.argv[1:]:
            if rid.startswith("--"):
                continue
            print(annot_text(rid.upper(), kurz))
        return
    census = {}
    os.makedirs(os.path.join(HERE, "scd_annot"), exist_ok=True)
    for rid in all_rooms():
        r = analyse(rid)
        if r is None:
            continue
        res, d, msgs = r
        ev = []
        story = False
        for reg, ops in res["regions"].items():
            for (pc, op, sz, conds, entry, text, depth) in ops:
                if entry is None:
                    continue
                if entry["k"] == "ck":
                    entry = dict(entry)
                e = dict(entry)
                e.update(reg=reg, pc=pc, cond=conds)
                ev.append(e)
                if entry["k"] == "em" and entry["typ"] >= 0x40:
                    story = True
        if any(m[1] and "04:00" in m[1] for m in msgs.values()):
            story = True
        census[rid] = ev
        if story:
            open(os.path.join(HERE, "scd_annot", "ROOM%s.txt" % rid), "w", encoding="utf-8").write(annot_text(rid))
    json.dump(census, open(os.path.join(HERE, "census.json"), "w", encoding="utf-8"), ensure_ascii=False)
    print("Raeume im Zensus:", len(census))


if __name__ == "__main__":
    main()
