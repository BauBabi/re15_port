#!/usr/bin/env python3
"""codepanel_zensus.py - klassifiziert die 16 Wegwerf-Benutzungsstellen nach MECHANIK.

Frage des Nutzers: "bei Toren mit Raetsel-Panels, also wo man einen Code eingeben muss,
darf die Abfrage erst kommen, wenn man den Zugriffscode einmal richtig eingegeben hat."

Also muss je Stelle entschieden werden, WAS dort im ausgelieferten SCD passiert. Die
Entscheidung faellt an den Opcodes, nicht am Namen:

  TASTENEINGABE  - Sce_key_ck (0x51) oder Sce_espr_control (0x52, Flanken-Zwilling,
                   scd_vm.c:4483-4494) im Zweig: das Skript LIEST einen Knopf ab.
                   Das ist die Signatur eines Eingabe-Raetsels (Cursor/Zahlenschloss).
  WERTVERGLEICH  - Member_cmp (0x3E) oder Cmp (0x23) auf einen Fortschrittswert:
                   das Skript prueft einen ZUSTAND, der vorher eingegeben wurde.
  JA/NEIN-FRAGE  - Message_on mit Steuerbyte 0x03 im Text + Ck(12,31,x):
                   Bestaetigungsdialog, KEINE Codeeingabe.
  GENOMMEN-BIT   - Ck(9,<bit>,1): das taken-Bit eines Item_aot_set. Besitzersatz.

Ausgabe je Stelle:
  * Datei-Byte-Offset des Message_on-Records und der Subroutine
  * der vollstaendige Opcode-Strom der Subroutine (Datei-Offsets)
  * die Bedingungskette, die den Message_on erreicht (nur vorwaerts-lesbare
    Ifel_ck/Ck/Member_cmp/Sce_key_ck, die ihn UEBERSPRINGEN koennen)
  * der Raum-Zensus: alle 0x51/0x52/0x3E/0x23 im ganzen Raum, mit Offsets

Aufruf:  python codepanel_zensus.py [ASSET_ROOT] [--room ROOM10D0] [--full]
"""
import sys, os, glob, argparse, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from discard_zensus import (op_size, fwd_target, rdt_section_end,
                            section_regions, u16, u32, s16)
from discard_nutzstellen import messages, walk_ops, decode

# Die 16 Stellen aus engine/src/gen/discard_sites.inc (Commit 47766cb4).
SITES = [
    (0x1090,  9, 0x31, "Fire Extinguisher"),
    (0x10D0,  9, 0x38, "Blue Keycard"),
    (0x10D1,  9, 0x38, "Blue Keycard"),
    (0x1100,  4, 0x44, "Minidisc Player w/ Disc"),
    (0x1101,  4, 0x44, "Minidisc Player w/ Disc"),
    (0x11E0,  9, 0x39, "Yellow Keycard"),
    (0x11E0, 12, 0x30, "Pliers"),
    (0x11E1,  9, 0x39, "Yellow Keycard"),
    (0x11E1, 12, 0x30, "Pliers"),
    (0x1230,  9, 0x37, "Red Keycard"),
    (0x1231,  9, 0x37, "Red Keycard"),
    (0x3010,  1, 0x36, "Green Keycard"),
    (0x3011,  1, 0x36, "Green Keycard"),
    (0x3050,  5, 0x46, "Red Master Keycard"),
    (0x3051,  5, 0x46, "Red Master Keycard"),
    (0x4000,  2, 0x47, "Blue Master Keycard"),
]

NAMES = {
    0x00:"Nop", 0x01:"Evt_end", 0x02:"Evt_next", 0x03:"Evt_chain", 0x04:"Evt_exec",
    0x05:"Evt_kill", 0x06:"Ifel_ck", 0x07:"Else_ck", 0x08:"Endif", 0x09:"Sleep",
    0x0A:"Sleeping", 0x0D:"For", 0x0E:"Next", 0x0F:"While", 0x10:"Ewhile",
    0x11:"Do", 0x12:"Edwhile", 0x13:"Switch", 0x14:"Case", 0x15:"Default",
    0x16:"Eswitch", 0x17:"Goto", 0x18:"Gosub", 0x19:"Return", 0x1A:"Break",
    0x21:"Ck", 0x22:"Set", 0x23:"Cmp", 0x24:"Save", 0x25:"Copy", 0x26:"Calc",
    0x27:"Calc2", 0x29:"Cut_chg", 0x2A:"Cut_old", 0x2B:"Message_on", 0x2C:"Aot_set",
    0x2D:"Obj_model_set", 0x2E:"Work_set", 0x2F:"Speed_set", 0x32:"Pos_set",
    0x33:"Dir_set", 0x34:"Member_set", 0x35:"Member_set2", 0x36:"Se_on",
    0x37:"Sca_id_set", 0x38:"Flr_set", 0x39:"Sca_floor_set", 0x3A:"Sce_espr_on",
    0x3B:"Door_aot_set", 0x3C:"Cut_auto", 0x3D:"Member_copy", 0x3E:"Member_cmp",
    0x3F:"Plc_motion", 0x40:"Plc_dest", 0x41:"Plc_neck", 0x42:"Plc_ret",
    0x43:"Plc_flg", 0x44:"Sce_em_set", 0x45:"Col_chg_set", 0x46:"Aot_reset",
    0x47:"Aot_on", 0x48:"Super_set", 0x49:"Super_reset", 0x4A:"Plc_gun",
    0x4B:"Cut_replace", 0x4C:"Sce_espr_kill", 0x4E:"op4E", 0x4F:"op4F",
    0x50:"Item_aot_set", 0x51:"Sce_key_ck", 0x52:"Sce_espr_control",
    0x53:"Sce_fade_set", 0x54:"Sce_bgm_control", 0x55:"Member_calc",
    0x56:"Member_calc2", 0x57:"Sce_bgmtbl_set", 0x58:"Plc_rot", 0x59:"Xa_on",
    0x5A:"Weapon_chg", 0x5B:"Plc_cnt", 0x5C:"Sce_shake_on", 0x5D:"Mizu_div_set",
    0x5E:"Keep_Item_ck",
}

# Opcodes, die ein Praedikat sind (liefern wahr/falsch an das umgebende Ifel_ck).
PRED = {0x21:"Ck", 0x23:"Cmp", 0x3E:"Member_cmp", 0x51:"Sce_key_ck",
        0x52:"Sce_espr_control", 0x5E:"Keep_Item_ck"}
TASTEN = {0x51, 0x52}
VERGLEICH = {0x23, 0x3E}


def ann(d, pc, op):
    """Kurz-Kommentar je Opcode, aus den Rohbytes."""
    if op in (0x21, 0x22):
        return f"bank={d[pc+1]} bit={d[pc+2]} val={d[pc+3]}"
    if op == 0x2B:
        return f"msg={d[pc+1]} typ=0x{d[pc+2]:02X} ?={d[pc+3]}"
    if op == 0x04:
        return f"sub={d[pc+3]} (cond={d[pc+1]:#x} typ={d[pc+2]:#x})"
    if op == 0x18:
        return f"sub={d[pc+1]}"
    if op == 0x06:
        return f"ueberspringe {u16(d,pc+2)} B -> 0x{pc+4+u16(d,pc+2):X}"
    if op == 0x07:
        return f"-> 0x{pc+s16(d,pc+2):X}"
    if op == 0x51 or op == 0x52:
        return f"param={d[pc+1]} maske=0x{u16(d,pc+2):04X}"
    if op == 0x3E:
        return f"member={d[pc+2]} op={d[pc+3]} wert={s16(d,pc+4)}"
    if op == 0x23:
        return f"bank={d[pc+1]} var={d[pc+2]} op={d[pc+3]} wert={s16(d,pc+4)}"
    if op == 0x2E:
        return f"kind={d[pc+1]} idx={d[pc+2]}"
    if op == 0x50:
        lf = (d[pc+3] & 0x80) != 0
        t = d[pc+22] if lf else d[pc+14]
        n = d[pc+24] if lf else d[pc+16]
        b = d[pc+26] if lf else d[pc+18]
        return f"typ=0x{t:02X} n={n} tk_bit={b}{' LANG' if lf else ''}"
    if op == 0x3B:
        return f"slot={d[pc+1]} -> stage={d[pc+22] if len(d)>pc+22 else '?'}"
    if op == 0x34:
        return f"member={d[pc+1]} wert={u16(d,pc+2)}"
    if op == 0x24:
        return f"bank={d[pc+1]} var={d[pc+2]} wert={d[pc+3]}"
    if op == 0x26:
        return f"bank={d[pc+1]} var={d[pc+2]} op={d[pc+3]} wert={s16(d,pc+4)}"
    return ""


def dump_sub(d, s, e, mark=None, out=None):
    out = out if out is not None else []
    for (pc, op, sz) in walk_ops(d, s, e):
        raw = " ".join(f"{b:02x}" for b in d[pc:pc+sz])
        if len(raw) > 47:
            raw = raw[:44] + "..."
        m = "*" if (mark is not None and pc == mark) else " "
        out.append(f"  {m} 0x{pc:05X}  {NAMES.get(op,'op%02X'%op):16s} {raw:<48s} {ann(d,pc,op)}")
    return out


def pfad(d, s, e, ziel):
    """Praedikate, die den Opcode bei `ziel` ueberspringen koennen.

    Nur vorwaerts lesbar: Ifel_ck (0x06) tragt die Sprungweite im Record, wir wissen
    also genau, ob `ziel` im Wenn-Block liegt. Jedes Praedikat direkt NACH einem
    offenen Ifel_ck, dessen Block `ziel` enthaelt, gatet `ziel`.
    """
    ops = walk_ops(d, s, e)
    offen = []     # (ifel_pc, block_ende)
    ketten = []
    for (pc, op, sz) in ops:
        offen = [(a, b) for (a, b) in offen if b > pc]
        if op == 0x06:
            ende = pc + 4 + u16(d, pc+2)
            offen.append((pc, ende))
            continue
        if op in PRED and offen:
            # gehoert das Praedikat zu einem Block, der `ziel` enthaelt?
            for (a, b) in offen:
                if a < ziel < b and a < pc < ziel:
                    ketten.append((pc, op, a, b))
                    break
        if pc >= ziel:
            break
    return ketten


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("root", nargs="?", default=os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "..", "..", "..", "re15_port", "shared_assets", "PSX"))
    ap.add_argument("--room")
    ap.add_argument("--full", action="store_true", help="ganzen Raum dumpen")
    a = ap.parse_args()

    rdts = {}
    for p in glob.glob(os.path.join(a.root, "STAGE*", "ROOM*.RDT")):
        rdts[os.path.basename(p).split(".")[0].upper()] = p

    gefunden = 0
    for (room, msg, item, iname) in SITES:
        rn = f"ROOM{room:04X}"
        if a.room and a.room.upper() != rn:
            continue
        p = rdts.get(rn)
        print("=" * 100)
        if not p:
            print(f"{rn}: KEINE RDT gefunden")
            continue
        d = open(p, "rb").read()
        msgs = messages(d)
        text = msgs.get(msg, ("<kein Text>", []))
        print(f"{rn}  msg {msg}  Gegenstand 0x{item:02X} {iname}")
        print(f"  Datei: {os.path.relpath(p, a.root)}   {len(d)} B")
        print(f"  msgScd@0x{u32(d,0x3C):X}  mainScd@0x{u32(d,0x40):X}  subScd@0x{u32(d,0x44):X}")
        print(f"  Text[{msg}] = {text[0]!r}   Steuerbytes={text[1]}")

        ms, ss = u32(d, 0x40), u32(d, 0x44)
        # Raum-Zensus: Tasteneingabe / Wertvergleich im GANZEN Raum
        zensus = collections.defaultdict(list)
        stellen = []
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d):
                continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                for (pc, op, sz) in walk_ops(d, sec+o, sec+e):
                    if op in (0x51, 0x52, 0x3E, 0x23, 0x5E):
                        zensus[op].append((f"{tag}{idx:02d}", pc))
                    if op == 0x2B and d[pc+1] == msg:
                        stellen.append((tag, idx, sec+o, sec+e, pc))
        print(f"  Raum-Zensus: " + "  ".join(
            f"{NAMES.get(k)}(0x{k:02X})x{len(v)}" for k, v in sorted(zensus.items())) or "  (keine)")
        for k in sorted(zensus):
            print(f"     0x{k:02X} {NAMES.get(k):16s}: " +
                  ", ".join(f"{s}@0x{o:X}" for (s, o) in zensus[k]))

        if not stellen:
            print("  !! KEIN Message_on {msg} im SCD")
            continue
        for (tag, idx, s, e, pc) in stellen:
            gefunden += 1
            print(f"\n  --- {tag}{idx:02d}  Subroutine 0x{s:05X}..0x{e:05X}   "
                  f"Message_on {msg} @0x{pc:05X} ---")
            ket = pfad(d, s, e, pc)
            if ket:
                print("  Bedingungskette (Praedikate, die diesen Message_on gaten):")
                for (qpc, qop, ia, ib) in ket:
                    print(f"     0x{qpc:05X} {NAMES.get(qop):16s} {ann(d,qpc,qop):40s}"
                          f" (Ifel_ck@0x{ia:X}, Block bis 0x{ib:X})")
            else:
                print("  Bedingungskette: KEINE (unbedingt erreichbar in dieser Subroutine)")
            for l in dump_sub(d, s, e, mark=pc):
                print(l)

        if a.full:
            print("\n  === GANZER RAUM ===")
            for sec, tag in ((ms, "main"), (ss, "sub")):
                if sec == 0 or sec >= len(d):
                    continue
                se = rdt_section_end(d, sec)
                for (o, e, idx) in section_regions(d, sec, se):
                    print(f"\n  -- {tag}{idx:02d} 0x{sec+o:05X}..0x{sec+e:05X}")
                    for l in dump_sub(d, sec+o, sec+e):
                        print(l)
            print("\n  === NACHRICHTEN ===")
            for i in sorted(msgs):
                print(f"   [{i:2d}] {msgs[i][0]!r}  ctrl={msgs[i][1]}")

    print("=" * 100)
    print(f"Stellen mit ausfuehrendem Message_on gefunden: {gefunden}")


if __name__ == "__main__":
    main()
