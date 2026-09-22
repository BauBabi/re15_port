#!/usr/bin/env python3
"""discard_codegate.py - misst je Wegwerf-Stelle, OB ein Code-/Raetsel-Panel davorhaengt,
WANN es bedient wird (vor oder nach der Meldung) und WELCHER Speicherplatz festhaelt,
dass es erledigt ist.

Frage (Nutzer 2026-09-22): "das Problem das ich Bei dir sah, war das du die discard
Abfrage auch bei Toren mit Raetsel panels machst, also wo man einen Code eingeben muss.
Da ist das natuerlich erst dann korrekt, wenn man den zugriffscode den man braucht
einmalig richtig eingegeben hat."

Gemessen wird, nicht vermutet:

  (1) PANEL-KENNUNG JE RAUM.  Ein Raum hat ein bedienbares Panel, wenn sein SCD
      `Sce_key_ck` (0x51/0x52) ausfuehrt - der einzige Opcode, mit dem ein Skript den
      Pad selbst liest (Dispatch PTR_LAB_800744a8 @0x80074690/0x80074694).
      CODE-PANEL zusaetzlich, wenn der Raum die 4-Ziffern-Textreihe
      "Enter the first number." .. "Wrong code, try again." fuehrt UND sie per
      Message_on ausgibt.

  (2) REIHENFOLGE JE STELLE - und zwar in AUSFUEHRUNGSREIHENFOLGE, nicht nach
      Datei-Offset (die Ziffern-Subs liegen im Datei-Bild VOR sub20, laufen aber
      danach). Gemessen wird: steht im selben geradlinigen Block NACH der
      Wegwerf-Nachricht ein `Evt_exec`/`Gosub`, von dem aus (transitiv, Tiefe 4) eine
      Ziffern-Nachricht ausgegeben wird? Dann kommt die Wegwerf-Meldung VOR dem Code.
      (Dynamisch bestaetigt mit Bildnummern von
      tests/unit/probe_r22_codepanel_reihenfolge.c.)

  (3) DER MARKER.  Fuer jede Stelle: das letzte `Set(bank,bit,1)` VOR und das erste
      NACH der Wegwerf-Nachricht im selben Block, mit Bank != 1/2/5/12
      (1+2 = Engine-Globale/Pause-Wort, 5 = Panel-Arbeitsspeicher, 12 = Nachrichten-FSM).
      Zusaetzlich die Riegel-Kette des Code-Panels
      `Ck(3,x,0) + Ck(5,13..16,1) -> Evt_exec sub19 -> Set(3,x,1)`.

  (4) UMKEHRBARKEIT.  Vollzensus ueber ALLE ausgelieferten RDTs: jedes Set/Ck auf den
      gefundenen (bank,bit). Kein `Set(...,0)` im Bestand = einmalig.

Flag-Bank-Zuordnung (PTR_DAT_80074664; dumpbar mit
.claude/skills/re15-psx-disasm/scripts/re15_disasm.py table 0x80074664 16):
  [0]=800aca38 [1]=800aca3c [2]=800aca40(Pause-Wort) [3]=800b0ff8 [4]=800b1018
  [5]=800b1028 [6]=800b1030 [7]=800b1038 [8]=800b1058 [9]=800b1078 [10]=800b1098
  [11]=800aca44 [12]=800b8520 (Nachrichten-FSM; Bit 31 = die Ja/Nein-Antwort)
Bank 5 ist der Arbeitsspeicher der Panels, Bank 3/4 der bestaendige Spielstand.

Aufruf: python discard_codegate.py [ASSET_ROOT]
"""
import sys, os, glob, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from scd_walk_lib import (u16, u32, op_size, fwd_target, rdt_section_end,
                          section_regions, walk_ops, regionen, messages, NAMES)


ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")

# Die 16 Benutzungsstellen aus tools/gen_discard_sites.py (dort aus den Daten abgeleitet).
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
ZIFFERNREIHE = ["Enter the first number.", "Enter the second number.",
                "Enter the third number.", "Enter the fourth number.",
                "Wrong code, try again."]
# Banken, die KEIN bestaendiger Spielstand sind (Begruendung im Kopf).
FLUECHTIG = (0, 1, 2, 5, 12)


def rdt_path(root, rid):
    for p in glob.glob(os.path.join(root, "STAGE*", "ROOM%04X.RDT" % rid)):
        return p
    return None


def regionen(d):
    """{(tag,idx): [(pc,op,size), ...]} fuer main- und sub-SCD, opcode-exakt."""
    out = {}
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d):
            continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            out[(tag, idx)] = walk_ops(d, sec + o, sec + e)
    return out


def fmt(d, pc, op, sz):
    raw = " ".join("%02x" % b for b in d[pc:pc + sz])
    ex = ""
    if op in (0x21, 0x22):
        ex = "  bank=%d bit=%d val=%d" % (d[pc + 1], d[pc + 2], d[pc + 3])
    elif op == 0x2B:
        ex = "  msg=%d" % d[pc + 1]
    elif op == 0x04:
        ex = "  sub=%d" % d[pc + 3]
    elif op == 0x18:
        ex = "  sub=%d" % d[pc + 1]
    elif op == 0x3E:
        ex = "  member=0x%02X ==%d" % (d[pc + 1], u16(d, pc + 3))
    elif op in (0x51, 0x52):
        ex = "  maske=0x%02X" % d[pc + 2]
    return "0x%05X %-14s %s%s" % (pc, NAMES.get(op, "op%02X" % op), raw, ex)


def sub_graph(d, regs):
    """sub-Index -> (ausgegebene msg-Ids, aufgerufene sub-Indizes)."""
    msgs, calls = collections.defaultdict(set), collections.defaultdict(set)
    for (tag, idx), ops in regs.items():
        if tag != "sub":
            continue
        for (pc, op, sz) in ops:
            if op == 0x2B:
                msgs[idx].add(d[pc + 1])
            elif op == 0x04:
                calls[idx].add(d[pc + 3])
            elif op == 0x18:
                calls[idx].add(d[pc + 1])
    return msgs, calls


def erreichbare_msgs(start, msgs, calls, tiefe=4):
    seen, front = set(), {start}
    out = set()
    for _ in range(tiefe):
        nxt = set()
        for s in front:
            if s in seen:
                continue
            seen.add(s)
            out |= msgs.get(s, set())
            nxt |= calls.get(s, set())
        front = nxt
    return out


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "shared_assets", "PSX")

    # ---------- Vollzensus Bank-Bits: jedes Set/Ck ueber ALLE RDTs ------------------------
    setzt = collections.defaultdict(list)
    prueft = collections.defaultdict(list)
    keyck = collections.Counter()
    n_rdt = 0
    for p in sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x48:
            continue
        n_rdt += 1
        room = os.path.basename(p).split(".")[0]
        for (tag, idx), ops in regionen(d).items():
            for (pc, op, sz) in ops:
                if op == 0x22:
                    setzt[(d[pc + 1], d[pc + 2])].append((room, pc, d[pc + 3]))
                elif op == 0x21:
                    prueft[(d[pc + 1], d[pc + 2])].append((room, pc, d[pc + 3]))
                elif op in (0x51, 0x52):
                    keyck[room] += 1

    print("==== Abdeckung: %d RDTs mit Header, %d verschiedene (bank,bit) beschrieben, "
          "%d geprueft ====" % (n_rdt, len(setzt), len(prueft)))

    tabelle = []      # (room, msg, item, name, art, ordnung, marker, bedingung)
    marker_alle = set()

    for (rid, mid, iid, nm) in SITES:
        p = rdt_path(root, rid)
        if not p:
            print("\n### ROOM%04X msg %d - RDT fehlt" % (rid, mid))
            continue
        d = open(p, "rb").read()
        room = os.path.basename(p).split(".")[0]
        texte = {i: t for i, (t, c) in messages(d).items()}
        regs = regionen(d)
        msgs, calls = sub_graph(d, regs)
        alle_msg_ops = set()
        for ops in regs.values():
            for (pc, op, sz) in ops:
                if op == 0x2B:
                    alle_msg_ops.add(d[pc + 1])

        ziffern = sorted(i for i, t in texte.items()
                         if t.strip() in ZIFFERNREIHE and i in alle_msg_ops)
        n_key = keyck[room]
        raum_art = "KEIN PANEL"
        if n_key and len(ziffern) >= 4:
            raum_art = "CODE-PANEL (4 Ziffern)"
        elif n_key:
            raum_art = "CURSOR-PANEL"

        # Die Wegwerf-Stelle
        stelle = None
        for (tag, idx), ops in regs.items():
            for k, (pc, op, sz) in enumerate(ops):
                if op == 0x2B and d[pc + 1] == mid:
                    stelle = (tag, idx, pc, k, ops)
                    break
            if stelle:
                break
        print("\n### ROOM%04X msg %2d  0x%02X %-24s  Raum: %s  (Sce_key_ck %dx)"
              % (rid, mid, iid, nm, raum_art, n_key))
        print("    Text: %r" % texte.get(mid, "?"))
        if not stelle:
            print("    KEIN Message_on %d im SCD" % mid)
            continue
        tag0, idx0, pc0, k0, ops0 = stelle
        print("    Stelle: %s%02d @0x%05X" % (tag0, idx0, pc0))

        # (2) Reihenfolge in AUSFUEHRUNGSREIHENFOLGE
        code_nach = []
        for (pc, op, sz) in ops0[k0 + 1:]:
            if op in (0x04, 0x18):
                s = d[pc + 3] if op == 0x04 else d[pc + 1]
                erre = erreichbare_msgs(s, msgs, calls)
                if set(ziffern) & erre:
                    code_nach.append((pc, s, sorted(set(ziffern) & erre)))
        # dasselbe VOR der Stelle im selben Block
        code_vor = []
        for (pc, op, sz) in ops0[:k0]:
            if op in (0x04, 0x18):
                s = d[pc + 3] if op == 0x04 else d[pc + 1]
                erre = erreichbare_msgs(s, msgs, calls)
                if set(ziffern) & erre:
                    code_vor.append((pc, s, sorted(set(ziffern) & erre)))
        if ziffern:
            print("    Ziffern-Nachrichten des Raums: %s" % ziffern)
        if code_nach:
            for (pc, s, m) in code_nach:
                print("    !! REIHENFOLGE: Code-Eingabe wird ERST NACH der Meldung gestartet "
                      "- @0x%05X Evt_exec sub%d, von dort Ziffern-Nachrichten %s" % (pc, s, m))
            ordnung = "Meldung VOR Code"
        elif code_vor:
            print("    REIHENFOLGE: Code-Eingabe laeuft VOR der Meldung (@0x%05X sub%d)"
                  % (code_vor[0][0], code_vor[0][1]))
            ordnung = "Code VOR Meldung"
        else:
            ordnung = "kein Code auf diesem Weg"
            print("    REIHENFOLGE: auf dem Weg dieser Stelle liegt KEINE Ziffern-Eingabe")

        # (3) Marker vor/nach der Stelle im selben Block
        vor = [(pc, d[pc + 1], d[pc + 2]) for (pc, op, sz) in ops0[:k0]
               if op == 0x22 and d[pc + 3] == 1 and d[pc + 1] not in FLUECHTIG]
        nach = [(pc, d[pc + 1], d[pc + 2]) for (pc, op, sz) in ops0[k0 + 1:]
                if op == 0x22 and d[pc + 3] == 1 and d[pc + 1] not in FLUECHTIG]
        if vor:
            print("    Bestaendiges Set VOR der Meldung:  " +
                  ", ".join("Set(%d,%d,1) @0x%05X" % (b, i, pc) for (pc, b, i) in vor))
        if nach:
            print("    Bestaendiges Set NACH der Meldung: " +
                  ", ".join("Set(%d,%d,1) @0x%05X" % (b, i, pc) for (pc, b, i) in nach))
        if not vor and not nach:
            print("    KEIN bestaendiges Set im Block dieser Stelle")

        print("    Umgebung:")
        for (pc, op, sz) in ops0[max(0, k0 - 4):k0 + 6]:
            print("     %s %s" % (" ->" if pc == pc0 else "   ", fmt(d, pc, op, sz)))

        # Die Riegel-Kette des Code-Panels (raumweit)
        code_marker = None
        flat = [(tag, idx, pc, op, sz) for (tag, idx), o in regs.items() for (pc, op, sz) in o]
        flat.sort(key=lambda e: e[2])
        for (tag, idx), o in regs.items():
            for i in range(len(o) - 6):
                w = o[i:i + 7]
                if all(e[1] == 0x21 for e in w[:5]) \
                   and [d[e[0] + 1] for e in w[1:5]] == [5, 5, 5, 5] \
                   and [d[e[0] + 2] for e in w[1:5]] == [13, 14, 15, 16] \
                   and w[5][1] == 0x04 and w[6][1] == 0x22:
                    print("    RIEGEL-KETTE %s%02d (vier richtige Ziffern -> aufschliessen):"
                          % (tag, idx))
                    for e in w:
                        print("        %s" % fmt(d, e[0], e[1], e[2]))
                    code_marker = (d[w[6][0] + 1], d[w[6][0] + 2], w[6][0])
                    print("    => BESTAENDIGER CODE-MARKER: Set(%d,%d,1) @0x%05X" % code_marker)
                    break
            if code_marker:
                break

        if code_nach and code_marker:
            bed = "flag(%d,%d)==1" % (code_marker[0], code_marker[1])
            marker_alle.add((code_marker[0], code_marker[1]))
        elif vor:
            bed = "unveraendert (Marker flag(%d,%d) steht schon)" % (vor[-1][1], vor[-1][2])
            marker_alle.add((vor[-1][1], vor[-1][2]))
        elif nach:
            bed = "unveraendert (Marker flag(%d,%d) folgt unmittelbar)" % (nach[0][1], nach[0][2])
            marker_alle.add((nach[0][1], nach[0][2]))
        else:
            bed = "unveraendert"
        tabelle.append((room, mid, iid, nm, raum_art, ordnung,
                        "flag(%d,%d)" % (code_marker[0], code_marker[1]) if code_marker else "-",
                        bed))

    # ---------- (4) Umkehrbarkeit ---------------------------------------------------------
    print("\n\n==== UMKEHRBARKEIT der gefundenen Marker (Vollzensus %d RDTs) ====" % n_rdt)
    for (bank, bit) in sorted(marker_alle):
        s = setzt[(bank, bit)]
        c = prueft[(bank, bit)]
        nullen = [x for x in s if x[2] == 0]
        print("\n-- flag(%d,%d)   %d Set, davon %d mit val=0   |   %d Ck"
              % (bank, bit, len(s), len(nullen), len(c)))
        for (room, off, val) in s:
            print("     Set %s @0x%05X val=%d" % (room, off, val))
        for (room, off, val) in c:
            print("     Ck  %s @0x%05X val=%d" % (room, off, val))
        print("     URTEIL: %s" % ("EINMALIG (kein Set mit val=0 im Bestand)"
                                   if not nullen else "WIRD ZURUECKGESTELLT - taugt NICHT"))

    b5_set = sum(len(v) for k, v in setzt.items() if k[0] == 5)
    b5_null = sum(len([x for x in v if x[2] == 0]) for k, v in setzt.items() if k[0] == 5)
    print("\n-- Gegenprobe Bank 5 (Panel-Arbeitsspeicher DAT_800b1028): %d Set, davon %d "
          "mit val=0  => nicht bestaendig, taugt NICHT als \"einmalig eingegeben\""
          % (b5_set, b5_null))

    print("\n\n==== ERGEBNISTABELLE (16 Stellen) ====")
    print("%-9s %4s %-24s %-22s %-24s %-13s %s"
          % ("Raum", "msg", "Gegenstand", "Raum-Panel", "Reihenfolge", "Code-Marker",
             "Bedingung"))
    for (room, mid, iid, nm, art, ordnung, cm, bed) in tabelle:
        print("%-9s %4d %-24s %-22s %-24s %-13s %s"
              % (room, mid, nm, art, ordnung, cm, bed))


main()
