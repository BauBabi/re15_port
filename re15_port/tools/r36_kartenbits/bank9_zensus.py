#!/usr/bin/env python3
"""bank9_zensus.py - Runde 36, Nachtrag zu Spur N (Zweig r36/kartenbits): wer benutzt welches Bit der
Flag-Bank 9 - im ORIGINAL (alle ausgelieferten RDTs + EXE/Overlays) und im PORT (Quelltext).

Dossier: analysis/befunde_runde36/N_kartenitems.md Abschnitt 9. Ausgabe eingecheckt unter
analysis/befunde_runde36/N_belege/bank9_zensus.txt.

WAS BANK 9 IM ORIGINAL IST (selbst disassembliert, info/Re1.5/PSX.EXE):
  Flag-Tabelle 0x80074664, Eintrag [9] @0x80074688 = 0x800b1078 -> Bank 9 = 0x800b1078..0x800b1097 (256 Bit,
  Wort = Bit>>5, Maske 0x80000000 >> (Bit&31): FUN_8004efe4 @0x8004efe4-f004, Ck @0x8003fd10-40).
  Die Tabelle 0x80074664 wird in der EXE an GENAU fuenf Stellen indiziert (ghidra1_V2.txt XREF PTR_DAT_80074664):
    0x8003fd2c  Ck        (Opcode 0x21, Handler 0x8003fcf4): Bank pc[1], Bit pc[2]
    0x8003fda0  Flag-Ck2  (Opcode 0x58, Handler 0x8003fd54): Bank pc[1], Bit erst zur Laufzeit (work_vars)
    0x8003fe04  Set       (Opcode 0x22, Handler 0x8003fdd0): Bank pc[1], Bit pc[2]
    0x8003fed0  Flag-Set2 (Opcode 0x59, Handler 0x8003fe90): Bank pc[1], Bit erst zur Laufzeit (work_vars)
    0x80043140  AOT sce 4 FLAG_CHG (LAB_80043120): Bank u16 Nutzlast+0, Bit u16 +2, an/aus u16 +4
  Dazu der Eintrag [9] direkt (XREF DAT_800b1078: 0x8001e0d4, 0x80021fd4, 0x800406d8, 0x80074688):
    0x800406d8  Item_aot_set (Opcode 0x50, Handler 0x80040644): Taken-Bit = u16 pc[18] (@0x80040680) bzw.
                pc[26] in der Langform (@0x80040670), Test FUN_8004efe4 @0x800406dc
    0x8001e0d4 / 0x80021fd4  Aufnahme: setzen das Bit AUS DEM Item_aot_set-Satz (FUN_8004ef90 @0x8001e0d0 /
                @0x80021fd8, a1 = Satzfeld) - keine eigenen Bitnummern.
  -> Original-Bits der Bank 9 = Item_aot_set-Taken-Bits + Ck/Set(9,n) + FLAG_CHG-Saetze mit Bank 9
     (+ 0x58/0x59 mit Bank 9, deren Bit statisch nicht bestimmbar ist - sie werden gezaehlt).

Laengentabelle: EINE, aus scd_walk_lib (= scd_dump_room.py = engine/src/scd_vm.c s_opcode_sizes).

Aufruf (Wurzel des Baums):  python re15_port/tools/r36_kartenbits/bank9_zensus.py [original|exe|port]
"""
import collections, glob, os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.abspath(os.path.join(HERE, ".."))
PORT = os.path.abspath(os.path.join(TOOLS, ".."))
REPO = os.path.abspath(os.path.join(PORT, ".."))
sys.path.insert(0, TOOLS)
import scd_walk_lib as W  # noqa: E402

PSX = os.path.join(PORT, "shared_assets", "PSX")
EXE = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")
BINS = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN")

BANK9_LO, BANK9_HI = 0x800B1078, 0x800B1097      # Flag-Tabelle 0x80074664[9] @0x80074688
TABELLE, TAB9 = 0x80074664, 0x80074688


def rdts():
    fs = sorted(glob.glob(os.path.join(PSX, "STAGE*", "ROOM*.RDT")))
    return fs, [f for f in fs if os.path.getsize(f) >= 0x48]


def walk_all(d):
    """[(tag, idx, pc, op, sz)] + Desync-Zaehler: eine Region, deren Walk vor ihrem Ende ohne Evt_end stoppt."""
    out, desync = [], 0
    ms, ss = W.u32(d, 0x40), W.u32(d, 0x44)
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d):
            continue
        se = W.rdt_section_end(d, sec)
        for (o, e, idx) in W.section_regions(d, sec, se):
            ops = W.walk_ops(d, sec + o, sec + e)
            if ops:
                pc, op, sz = ops[-1]
                if op != 0x01 and pc + sz < sec + e:
                    # Abbruch mitten in der Region: unbekannter Opcode oder Ueberlauf
                    nxt = pc + sz
                    if W.op_size(d, nxt) is None or nxt + (W.op_size(d, nxt) or 0) > sec + e:
                        desync += 1
            for (pc, op, sz) in ops:
                out.append((tag, idx, pc, op, sz))
    return out, desync


def teil_original():
    alle, mit_kopf = rdts()
    print("== ORIGINAL: Bank-9-Bits in allen ausgelieferten RDTs (shared_assets/PSX/STAGE*/ROOM*.RDT) ==")
    print("RDT-Dateien: %d, davon mit Kopf (>= 0x48 B): %d" % (len(alle), len(mit_kopf)))
    quellen = collections.defaultdict(list)       # bit -> [Text]
    n_ops = n_items = n_desync = 0
    n_ck = n_set = 0
    op58, op59 = [], []
    flagchg_baenke = collections.Counter()
    item_hi = []
    for f in mit_kopf:
        d = open(f, "rb").read()
        room = os.path.basename(f)[4:8]
        ops, ds = walk_all(d)
        n_desync += ds
        for (tag, idx, pc, op, sz) in ops:
            n_ops += 1
            ort = "ROOM%s %s%02d @0x%05X" % (room, tag, idx, pc)
            if op == 0x50:
                lang = (d[pc + 3] & 0x80) != 0
                b = pc + (8 if lang else 0)
                item, menge, bit, prop = W.u16(d, b + 14), W.u16(d, b + 16), W.u16(d, b + 18), d[b + 20]
                n_items += 1
                if bit >= 256:
                    item_hi.append("%s Item 0x%02X Bit %d" % (ort, item, bit))
                quellen[bit].append("%s Item_aot_set%s Slot %d Item 0x%02X x%d Prop %d"
                                    % (ort, "(lang)" if lang else "", d[pc + 1], item & 0xFF, menge, prop))
            elif op in (0x21, 0x22) and d[pc + 1] == 9:
                if op == 0x21:
                    n_ck += 1
                    quellen[d[pc + 2]].append("%s Ck(9,%d,%d)" % (ort, d[pc + 2], d[pc + 3]))
                else:
                    n_set += 1
                    quellen[d[pc + 2]].append("%s Set(9,%d,%d)" % (ort, d[pc + 2], d[pc + 3]))
            elif op == 0x58 and d[pc + 1] == 9:
                op58.append("%s Flag-Ck2(9, work[%d])" % (ort, d[pc + 2]))
            elif op == 0x59 and d[pc + 1] == 9:
                op59.append("%s Flag-Set2(9, work[%d])" % (ort, d[pc + 2]))
            elif op == 0x2C and d[pc + 2] == 4:
                lang = (d[pc + 3] & 0x80) != 0
                b = pc + (22 if lang else 14)
                bank, bit = W.u16(d, b), W.u16(d, b + 2)
                flagchg_baenke[bank] += 1
                if bank == 9:
                    quellen[bit].append("%s Aot_set sce4 FLAG_CHG(9,%d,%d)" % (ort, bit, W.u16(d, b + 4)))
            elif op == 0x46 and d[pc + 2] == 4:
                bank, bit = W.u16(d, pc + 4), W.u16(d, pc + 6)
                flagchg_baenke[bank] += 1
                if bank == 9:
                    quellen[bit].append("%s Aot_reset sce4 FLAG_CHG(9,%d,%d)" % (ort, bit, W.u16(d, pc + 8)))
    print("Abdeckung: %d Opcodes, %d Desync-Stopps, %d Item_aot_set, %d Ck(9,..), %d Set(9,..)"
          % (n_ops, n_desync, n_items, n_ck, n_set))
    print("Flag-Ck2 0x58 mit Bank 9: %d %s" % (len(op58), op58[:6]))
    print("Flag-Set2 0x59 mit Bank 9: %d %s" % (len(op59), op59[:6]))
    print("FLAG_CHG-Saetze (Aot_set/Aot_reset sce 4) je Bank: %s" % dict(sorted(flagchg_baenke.items())))
    print("Item_aot_set mit Bit >= 256 (liefe in Bank 10 ueber): %d %s" % (len(item_hi), item_hi[:4]))
    belegt = sorted(b for b in quellen if b < 256)
    print()
    print("BELEGT im Original: %d von 256 Bits" % len(belegt))
    print("  " + " ".join(str(b) for b in belegt))
    frei = [b for b in range(256) if b not in quellen]
    bloecke, start = [], None
    for b in range(257):
        if b < 256 and b in frei:
            if start is None:
                start = b
        elif start is not None:
            bloecke.append((start, b - 1))
            start = None
    print("FREI im Original (Bloecke): " + ", ".join(("%d" % a) if a == e else ("%d..%d" % (a, e))
                                                     for a, e in bloecke))
    print()
    print("JE BIT (alle Quellen):")
    for b in belegt:
        print("  Bit %3d: %d Stelle(n)" % (b, len(quellen[b])))
        for q in quellen[b]:
            print("           " + q)
    return set(belegt)


# ---- EXE + Overlays: direkte Adressen der Bank 9 / der Tabelle -----------------------------------------
def _insn_scan(buf, base, name):
    """lui rX,hi gefolgt (<= 8 Instruktionen, gleiche Registerzeile) von addiu/ori/l*/s* rY,lo(rX): Zieladresse."""
    treffer = []
    n = len(buf) // 4
    hi = {}
    for i in range(n):
        w = struct.unpack_from("<I", buf, i * 4)[0]
        op, rs, rt, imm = w >> 26, (w >> 21) & 31, (w >> 16) & 31, w & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        for r in list(hi):
            if i - hi[r][1] > 8:
                del hi[r]
        if op == 0x0F:                                   # lui
            hi[rt] = ((imm << 16) & 0xFFFFFFFF, i)
            continue
        if op in (0x09, 0x0D, 0x20, 0x21, 0x23, 0x24, 0x25, 0x28, 0x29, 0x2B) and rs in hi:
            adr = (hi[rs][0] + (imm if op == 0x0D else simm)) & 0xFFFFFFFF
            if BANK9_LO <= adr <= BANK9_HI or adr == TAB9 or adr == TABELLE:
                treffer.append("%s 0x%08X  -> 0x%08X%s" % (name, base + i * 4, adr,
                               "  (Bank-9-Speicher)" if BANK9_LO <= adr <= BANK9_HI else
                               "  (Tabelle[9])" if adr == TAB9 else "  (Tabelle 0x80074664)"))
    return treffer


def teil_exe():
    print("== EXE + OVERLAYS: direkte Adressen von Bank 9 (0x800B1078..97), Tabelle[9] (0x80074688), Tabelle (0x80074664) ==")
    exe = open(EXE, "rb").read()
    t_addr = struct.unpack_from("<I", exe, 0x18)[0]
    treffer = _insn_scan(exe[0x800:], t_addr, "PSX.EXE")
    for f in sorted(glob.glob(os.path.join(BINS, "*.BIN"))):
        nm = os.path.basename(f)
        base = 0x800C0000 if nm == "DEBUG.BIN" else 0x80100000
        treffer += _insn_scan(open(f, "rb").read(), base, nm)
    for t in treffer:
        print("  " + t)
    # Ghidra-XREFs (PSX.EXE) als zweite, unabhaengige Quelle
    gd = os.path.join(REPO, "ghidra1_V2.txt")
    if os.path.exists(gd):
        print("  ghidra1_V2.txt XREF-Zeilen:")
        zeilen = open(gd, encoding="utf-8", errors="replace").read().split(chr(10))
        for i, z in enumerate(zeilen):
            if re.match(r"\s+(DAT_800b1078|PTR_DAT_80074664|PTR_DAT_80074688)\s+XREF", z):
                print("    " + z.strip())
                k = i + 1
                while k < len(zeilen) and re.match(r"\s+[0-9a-f]{8}\([*RW]\)", zeilen[k]):
                    print("    " + zeilen[k].strip())
                    k += 1
    # Overlay-Dekompilate: erstes Argument jedes Aufrufs der Bit-Helfer FUN_8004ef90 (setzen) / FUN_8004efb8
    # (loeschen) / FUN_8004efe4 (testen) = die Bank-Basis
    ov = sorted(glob.glob(os.path.join(REPO, "RE_15_Quellcode_Overlays", "STAGE*_overlay.c")))
    if ov:
        zaehler = collections.Counter()
        for f in ov:
            for m in re.finditer(r"func_0x8004ef(90|b8|e4)\(([^,)]*)", open(f, encoding="utf-8", errors="replace").read()):
                zaehler[(m.group(1), m.group(2).strip() or "(Register des Aufrufers)")] += 1
        print("  Overlay-Dekompilate (RE_15_Quellcode_Overlays/STAGE1..6_overlay.c), Bank-Basis je Aufruf der Bit-Helfer:")
        for (fn, a0), n in sorted(zaehler.items(), key=lambda x: -x[1]):
            print("    %3d x FUN_8004ef%s(%s, ...)" % (n, fn, a0))
        print("    (0x800b1028 = Bank 5, 0x800b1038 = Bank 7, 0x800b1058 = Bank 8; puVar+0x78 = 0x800b0fe0+0x78 = Bank 8;"
              " unaff_s0+0x48a4 = 0x800ac784+0x48a4 = Bank 5 wie PSX.EXE @0x80051784 (Ghidra a0=>DAT_800b1028);"
              " Bank 9 = 0x800b1078 kommt NICHT vor)")
    print("  (%d Treffer; Lesart: PSX.EXE 0x8003fd2c/0x8003fda0/0x8003fe04/0x8003fed0/0x80043140 = die fuenf"
          " Tabellen-Leser, 0x800406d8 = Item_aot_set, 0x8001e0d4/0x80021fd4 = Aufnahme mit Bit aus dem Satz)"
          % len(treffer))


# ---- PORT: Quelltext ------------------------------------------------------------------------------------
def teil_port():
    print("== PORT: Bank-9-Zugriffe im Quelltext (re15_port/include, engine/src, platform/pc) ==")
    quellen = []
    for pat in ("include/*.h", "engine/src/*.c", "engine/src/gen/*.inc", "platform/pc/*.c", "platform/pc/src/*.c"):
        quellen += sorted(glob.glob(os.path.join(PORT, pat)))
    defines = {}
    for f in quellen:
        for line in open(f, encoding="utf-8", errors="replace"):
            m = re.match(r"\s*#define\s+(RE15_[A-Z0-9_]+)\s+(0x[0-9A-Fa-f]+|\d+)u?\b", line)
            if m:
                defines[m.group(1)] = int(m.group(2), 0)
    bank9_makros = sorted(k for k, v in defines.items() if v == 9 and ("BANK" in k or "ZONE" in k))
    print("Makros mit Wert 9 und BANK/ZONE im Namen: " + ", ".join(bank9_makros))
    zugriff = re.compile(r"re15_game_flag_(get|set)\s*\(\s*(9u?|%s)\s*,\s*([^,)]+)" %
                         "|".join(re.escape(m) for m in bank9_makros))
    programm = re.compile(r"(0x2[12]|OP_SET\s*\()\s*,?\s*(0x09|9|%s)\s*,\s*([A-Za-z0-9_x]+)" %
                          "|".join(re.escape(m) for m in bank9_makros))
    print()
    print("Zugriffsstellen (re15_game_flag_get/set mit Bank 9; SCD-Port-Programme 0x21/0x22/OP_SET mit Bank 9):")
    tabellen = {}                                         # Datei -> Zugriffe mit Variablen-Bit
    for f in quellen:
        rel = os.path.relpath(f, PORT).replace("\\", "/")
        if rel.endswith(".inc") and ("_props" in rel or "prop.inc" in rel or "itembild" in rel or "ui_tables" in rel):
            continue                                      # Bilddaten, kein SCD
        for nr, line in enumerate(open(f, encoding="utf-8", errors="replace"), 1):
            for m in zugriff.finditer(line):
                arg = m.group(3).strip()
                wert = defines.get(arg)
                if wert is None and not re.match(r"^(0x[0-9A-Fa-f]+|\d+)$", arg):
                    tabellen.setdefault(rel, []).append(arg)
                    if wert is None:
                        try:
                            wert = int(arg, 0)
                        except ValueError:
                            pass
                print("  %s:%d  flag_%s(%s, %s)%s" % (rel, nr, m.group(1), m.group(2), arg,
                      ("  = Bit %d" % wert) if wert is not None else "  (Bit aus Tabelle/Satz, s.u.)"))
            for m in programm.finditer(line):
                arg = m.group(3)
                try:
                    wert = int(arg, 0)
                except ValueError:
                    wert = defines.get(arg)
                notiz = ""
                if rel.endswith("kartenitems_r36.c") and arg == "0xEE":
                    notiz = "  (Platzhalter: PRG_BIT wird beim Ausloesen mit k_r36k_lage[k].bit ueberschrieben)"
                print("  %s:%d  SCD %s Bank %s Bit %s%s%s" % (rel, nr, m.group(1).replace("(", "").strip(), m.group(2),
                      arg, ("  = Bit %d" % wert) if wert is not None else "", notiz))
    print()
    print("Bank-9-Zugriffe mit Bit aus einer Tabelle/einem Satz - die *_BIT-Makros, die dieselbe Datei verwendet:")
    for rel in sorted(tabellen):
        f = os.path.join(PORT, rel)
        txt = open(f, encoding="utf-8", errors="replace").read()
        makros = sorted(set(re.findall(r"\b(RE15_[A-Z0-9_]*BIT[A-Z0-9_]*)\b", txt)))
        makros = [m for m in makros if m in defines and "PAD_BIT" not in m]
        print("  %s  [%s]" % (rel, ", ".join(sorted(set(tabellen[rel])))))
        if makros:
            print("      " + ", ".join("%s=%d" % (m, defines[m]) for m in makros))
        else:
            print("      (keine *_BIT-Makros: das Bit stammt aus dem Item_aot_set-/Item-Satz = Original- bzw."
                  " Installer-Bit)")
    # Spur-N-Tabelle (erzeugt): Feld 3 jeder Zeile von k_r36k_lage
    inc = os.path.join(PORT, "engine", "src", "gen", "kartenitems_r36.inc")
    print()
    print("Spur-N-Tabelle %s (k_r36k_lage, Feld 3 = Bank-9-Bit):" % os.path.relpath(inc, PORT).replace("\\", "/"))
    for line in open(inc, encoding="utf-8"):
        m = re.match(r"\s*\{\s*(0x[0-9A-Fa-f]+),\s*(\d+),\s*(\d+),", line)
        if m:
            print("  ROOM%s Blatt %s -> Bit %s" % (m.group(1)[2:].upper(), m.group(2), m.group(3)))


if __name__ == "__main__":
    teile = sys.argv[1:] or ["original", "exe", "port"]
    for t in teile:
        {"original": teil_original, "exe": teil_exe, "port": teil_port}[t]()
        print()
