#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 39 Spur O "parkplatz11c0": SCD-Programme + Text der Tuersperre ROOM11C0 -> engine/src/gen/parkplatz_r39_programme.inc.

  python re15_port/tools/r39_parkplatz/programme_bauen.py [--pruefen]   (druckt das Listing; --pruefen vergleicht nur)

Nutzer (AUFTRAG.md Nachtrag 9): "bei room11C0, waehrend der Kampf gegen die Gorillas noch laeuft, soll bei Leon der Text
kommen "I cannot leave Ada alone here with those monsters!". Wenn Leon versucht durch die Tuer aus dem Raum raus zu gehen."
Dossier analysis/befunde_runde39/O_parkplatz11c0.md, Konstanten include/re15_parkplatz_r39.h (der Riegel
test_r39_parkplatz11c0 prueft beide gegeneinander und gegen das ausgelieferte RDT).

ROOM11C0.RDT (shared_assets/PSX/STAGE1, main_scd @0x1710, sub_scd @0x1758):
  main00 @0x01712 Door_aot_set Slot 0 (die EINZIGE Tuer, -> ROOM11B0, Ziel (-25552,0,-25331) = Nutzlast +14..+19)
  sub01  @0x01820 Ck(4,64)==0 -> @0x01824 Evt_exec sub02 (Ada-Szene; sub02 @0x0184E Set(4,64)=1 ... @0x01ACE Set(2,7)=0
         = Steuerung zurueck = Kampf laeuft)
  sub01  @0x0182E Ck(3,67)==0 && @0x01836 Ck(7,96)==1 && @0x0183E Ck(7,97)==1 -> @0x01842 Evt_exec sub03 (Kampf-Ende;
         sub03 @0x01B02 Set(3,67)=1 ... @0x01C08 Aot_on(0) = die Tuer feuert)
PROGRAMME (ersetzen das Ziel des jeweiligen Evt_exec; die Szenen selbst laufen UNVERAENDERT als Gosub aus dem RDT —
Form Gosub-Huelle: Runde 38 F SZENE1021 "Gosub sub02 + Aot_set", Gosub @0x8003fc04):
  SZENE  Aot_reset Slot 0 -> MESSAGE msg 40, dann Gosub sub02.
         Form der Zeile: ROOM11B0 sub01 @0x01216 `46 01 01 31 0f 00 ff ff 00 00` (RE1.5 Tuer -> Text) und RE2 ROOM7040
         sub06 @0x0185A `46 00 04 31 01 00 00 00 ff ff` (RE2: Tuer -> Text in der Kampf-Beginn-Szene, vor der Rueckgabe
         der Steuerung). Die Zeile steht vor dem Gosub: sub02 sperrt den Pad in DERSELBEN Runde (@0x01856 `22 02 07 01`,
         das dritte Opcode, kein Yield dazwischen) und gibt ihn erst @0x01ACE zurueck — zwischen Sperre und Rueckgabe
         kann Leon nichts ausloesen, die Wirkung ist dieselbe wie direkt vor @0x01ACE.
  ENDE   Aot_reset Slot 0 -> DOOR mit der Nutzlast aus main00 @0x01712+14 (Ziel x/y/z), dann Gosub sub03.
         Form: ROOM1141 sub05 @0x00FDC `46 01 02 31 ..` (RE1.5 Text -> Tuer, Nutzlast = Door_aot_set-Ziel) und RE2
         ROOM7040 sub07 @0x01ACC `46 00 01 31 f4 98 00 00 c9 a5` (RE2: Tuer zurueck in der Kampf-Ende-Szene). Sie MUSS
         vor sub03 @0x01C08 `47 00` stehen: Aot_on @0x8004082c `jalr` ruft den Handler, den Slot 0 GERADE hat.
TEXT: msg 40 = NUTZER-VORGABE, Erzaehlform `04 02 <Zeile> 08 <Zeile> 01 00` wie ROOM1090 msg 7 @0x28E6 ("I must hurry up
and get something to / put out this fire to save that woman!") und Runde 38 F 1141 msg 41.
"""
import io, os, struct, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
RDT_PFAD = os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM11C0.RDT")
OUT = os.path.join(PORT, "engine", "src", "gen", "parkplatz_r39_programme.inc")
sys.path.insert(0, os.path.join(PORT, "tools"))
import scd_walk_lib as L  # noqa: E402

D = open(RDT_PFAD, "rb").read()

# ---- Konstanten (muessen mit include/re15_parkplatz_r39.h uebereinstimmen; der Riegel vergleicht) ----
MSG_SPERRE = 40         # RE15_R39O_MSG_SPERRE   VERTRAG 1.2 (11C0 Id 40)
TUER_SLOT = 0           # RE15_R39O_TUER_SLOT    main00 @0x01712 pc[1]
SAT = 0x31              # RE15_R39O_SAT          main00 @0x01715 pc[3] (Door_aot_set) = 11B0 sub01 @0x01219 pc[3]
MASKE = 0xFFFF          # RE15_R39O_MASKE        11B0 sub01 @0x0121A..1D Nutzlast +2 (alle ausgelieferten sce-1-Saetze)
SUB_SZENE = 2           # RE15_R39O_SUB_SZENE    sub01 @0x01824 `04 0a 18 02`
SUB_ENDE = 3            # RE15_R39O_SUB_ENDE     sub01 @0x01842 `04 ff 18 03`

ERWARTET = (   # (Datei-Offset, Bytes, Bedeutung) — das Werkzeug bricht ab, wenn das RDT anders aussieht
    (0x01712, "3b 00 02 31 00 00 24 96 1c 3e a0 0f 8c 0a 30 9c 00 00 0d 9d", "main00 Door_aot_set Slot 0 -> 11B0"),
    (0x01726, "fb 0b 00 1b 01", "main00 Door_aot_set Blick 3067 / Stage 0 / Raum 0x1B / Cut 1"),
    (0x01770, "44 00 42 40", "sub00 Sce_em_set Ada (0x42)"),
    (0x01784, "44 01 27 30 02 00 00 60", "sub00 Gorilla 1 (0x27, Kill-Flag 0x60)"),
    (0x01798, "44 02 27 30 02 00 00 61", "sub00 Gorilla 2 (0x27, Kill-Flag 0x61)"),
    (0x017C8, "07 00", "sub00 Else (Ada-Szene gelaufen)"),
    (0x017D0, "21 03 43 00", "sub00 Ck(3,67)==0 = Kampf laeuft beim Wiedereintritt"),
    (0x01820, "21 04 40 00", "sub01 Ck(4,64)==0"),
    (0x01824, "04 0a 18 02", "sub01 Evt_exec sub02 (Ada-Szene)"),
    (0x0182E, "21 03 43 00", "sub01 Ck(3,67)==0"),
    (0x01836, "21 07 60 01", "sub01 Ck(7,96)==1 (Gorilla 1 tot)"),
    (0x0183E, "21 07 61 01", "sub01 Ck(7,97)==1 (Gorilla 2 tot)"),
    (0x01842, "04 ff 18 03", "sub01 Evt_exec sub03 (Kampf-Ende)"),
    (0x0184E, "22 04 40 01 22 01 1b 01 22 02 07 01", "sub02 Set(4,64)=1, Balken, Pad-Sperre"),
    (0x01ACE, "22 02 07 00", "sub02 Set(2,7)=0 = Steuerung zurueck, Kampf laeuft"),
    (0x01B02, "22 03 43 01 22 01 1b 01 22 02 07 01", "sub03 Set(3,67)=1, Balken, Pad-Sperre"),
    (0x01C08, "47 00 01 00", "sub03 Aot_on(0) + Evt_end"),
)

prog = []


def op(b, c):
    prog.append((bytes(b), c))


def pruefe_rdt():
    for off, hexs, was in ERWARTET:
        soll = bytes.fromhex(hexs)
        ist = D[off:off + len(soll)]
        if ist != soll:
            sys.exit("ABBRUCH: ROOM11C0 @0x%05X (%s) erwartet %s, ist %s" % (off, was, soll.hex(" "), ist.hex(" ")))
    # Sub-Tabelle: sub02/sub03 zeigen auf die erwarteten Stellen (sub_scd @0x1758, u16 je Eintrag)
    tab = 0x1758
    if tab + L.u16(D, tab + 2 * SUB_SZENE) != 0x0184E or tab + L.u16(D, tab + 2 * SUB_ENDE) != 0x01B02:
        sys.exit("ABBRUCH: ROOM11C0 sub_scd-Tabelle: sub02/sub03 nicht @0x0184E/@0x01B02")
    # main00 + alle Subs: genau EIN Door_aot_set (Slot 0), kein Aot_reset/Aot_set auf eine Tuer
    tueren = []
    for name, base, end in ((("main", 0x1710, 0x1758)), (("sub", 0x1758, 0x1C7C))):
        n = L.u16(D, base) // 2
        offs = [base + L.u16(D, base + 2 * i) for i in range(n)]
        for i, s in enumerate(offs):
            e = offs[i + 1] if i + 1 < n else end
            pc = s
            while pc < e:
                sz = L.op_size(D, pc)
                if sz is None:
                    sys.exit("ABBRUCH: unbekannter Opcode 0x%02X @0x%05X" % (D[pc], pc))
                if D[pc] == 0x3B:
                    tueren.append((pc, D[pc + 1]))
                if D[pc] == 0x46 or (D[pc] == 0x2C and D[pc + 2] == 2):
                    sys.exit("ABBRUCH: unerwartetes Aot_reset/Aot_set(Tuer) @0x%05X" % pc)
                pc += sz
    if tueren != [(0x01712, TUER_SLOT)]:
        sys.exit("ABBRUCH: Tueren im RDT %r, erwartet genau [(0x01712, 0)]" % tueren)


def baue_szene():
    del prog[:]
    op([0x46, TUER_SLOT, 0x01, SAT, MSG_SPERRE, 0x00, MASKE & 0xFF, MASKE >> 8, 0x00, 0x00],
       "Aot_reset Slot 0 (Tuer -> 11B0) -> MESSAGE msg %d, Maske 0x%04X (Form ROOM11B0 sub01 @0x01216 "
       "`46 01 01 31 0f 00 ff ff 00 00`; RE2 ROOM7040 sub06 @0x0185A)" % (MSG_SPERRE, MASKE))
    op([0x18, SUB_SZENE], "Gosub sub02 = die Ada-Szene UNVERAENDERT aus dem RDT (@0x0184E..@0x01B00; Pad-Sperre @0x01856 "
                          "in derselben Runde, Rueckgabe @0x01ACE)")
    op([0x01, 0x00], "Evt_end")
    return list(prog)


def baue_ende():
    del prog[:]
    nutzlast = D[0x01712 + 14:0x01712 + 20]
    op([0x46, TUER_SLOT, 0x02, SAT] + list(nutzlast),
       "Aot_reset Slot 0 -> DOOR, Nutzlast = main00 @0x01712+14 Ziel (%d,%d,%d) (Form ROOM1141 sub05 @0x00FDC "
       "`46 01 02 31 ..`; RE2 ROOM7040 sub07 @0x01ACC) — VOR sub03 @0x01C08 Aot_on(0)" % struct.unpack("<hhh", nutzlast))
    op([0x18, SUB_ENDE], "Gosub sub03 = die Kampf-Ende-Szene UNVERAENDERT aus dem RDT (@0x01B02..@0x01C0A, msg 7/8/9, "
                         "Aot_on(0) @0x01C08 -> ROOM11B0)")
    op([0x01, 0x00], "Evt_end")
    return list(prog)


# ---------------------------------------------------------------- Text
ENC = {" ": 0x00, ":": 0x16, ",": 0x18, "\"": 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10): ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i


def enc(t):
    return bytes(ENC[c] for c in t)


def breite(t):
    import re
    src = open(os.path.join(PORT, "include", "font_width.h"), encoding="utf-8", errors="replace").read()
    w = [int(x, 0) for x in re.findall(r"\b(0x[0-9a-fA-F]+|\d+)\b", src.split("{", 1)[1].split("}", 1)[0])]
    return sum(w[b] for b in enc(t))


SATZ = "I cannot leave Ada alone here with those monsters!"     # NUTZER-VORGABE (AUFTRAG.md Nachtrag 9), woertlich
Z1, Z2 = "I cannot leave Ada alone here", "with those monsters!"  # Umbruch wie 1141 msg 41 ("I cannot leave him alone" /
                                                                   # "with those monsters.") vor "with"


def baue_text():
    if Z1 + " " + Z2 != SATZ:
        sys.exit("ABBRUCH: Zeilen ergeben nicht den Nutzer-Satz")
    for z in (Z1, Z2):
        if breite(z) > 271:
            sys.exit("ABBRUCH: Zeile zu breit (%d px > 271): %r" % (breite(z), z))
    # Form-Probe: ROOM1090 msg 7 @0x28E6 beginnt mit `04 02`, bricht mit `08` um, endet `1a 01 00` ("woman!")
    d1090 = open(os.path.join(PORT, "shared_assets", "PSX", "STAGE1", "ROOM1090.RDT"), "rb").read()
    m7 = d1090[0x28E6:d1090.index(bytes([0x01, 0x00]), 0x28E6) + 2]
    if m7[:2] != b"\x04\x02" or m7[-3:] != b"\x1a\x01\x00" or b"\x08" not in m7:
        sys.exit("ABBRUCH: Form-Probe ROOM1090 msg 7 @0x28E6")
    return b"\x04\x02" + enc(Z1) + b"\x08" + enc(Z2) + b"\x01\x00"


# ---------------------------------------------------------------- Ausgabe
def carr(f, name, b):
    f.write("static const uint8_t %s[%d] = {" % (name, len(b)))
    for i, x in enumerate(b):
        f.write(("\n    " if i % 16 == 0 else "") + "0x%02x," % x)
    f.write("\n};\n")


def fnv(b):
    h = 0x811C9DC5
    for x in b:
        h = ((h ^ x) * 0x01000193) & 0xFFFFFFFF
    return h


QUELLEN = (   # (Name, von, bis) — der C-Teil prueft diese Bereiche im GELADENEN RDT (FNV-1a), sonst bleibt der Original-Ablauf
    ("MAIN00_TUER", 0x01712, 0x01732),   # Door_aot_set Slot 0 (Nutzlast der Ruecknahme)
    ("SUB00_03", 0x01768, 0x01C0C),      # sub00 (Wiedereintritt), sub01 (Bedingungen), sub02 (Szene), sub03 (Ende + Aot_on)
)


def main():
    pruefen = "--pruefen" in sys.argv
    pruefe_rdt()
    programme = [("k_r39o_p_szene", baue_szene()), ("k_r39o_p_ende", baue_ende())]
    text = baue_text()
    f = io.StringIO()
    f.write("/* GENERIERT von re15_port/tools/r39_parkplatz/programme_bauen.py - NICHT HAND-EDITIEREN.\n"
            " * Runde 39 Spur O: Tuersperre ROOM11C0 waehrend des Gorilla-Kampfes (Programme + Text + Anker).\n"
            " * Dossier: analysis/befunde_runde39/O_parkplatz11c0.md. */\n\n")
    for name, von, bis in QUELLEN:
        f.write("#define RE15_R39O_ANKER_%s_OFF 0x%05Xu\n" % (name, von))
        f.write("#define RE15_R39O_ANKER_%s_LEN %du\n" % (name, bis - von))
        f.write("#define RE15_R39O_ANKER_%s_FNV 0x%08Xu\n" % (name, fnv(D[von:bis])))
    f.write("\n")
    for name, zeilen in programme:
        b = b"".join(x for x, _ in zeilen)
        f.write("/* %s: %d Byte\n" % (name, len(b)))
        o = 0
        for x, k in zeilen:
            f.write(" *   +%04X %-30s %s\n" % (o, x.hex(" "), k.replace("*/", "* /")))
            o += len(x)
        f.write(" */\n")
        carr(f, name, b)
        f.write("\n")
    f.write("/* ROOM11C0 msg %d: NUTZER-VORGABE \"%s\" (AUFTRAG.md Nachtrag 9), Erzaehlform `04 02 .. 08 .. 01 00` wie\n"
            " * ROOM1090 msg 7 @0x28E6 / Runde 38 F 1141 msg 41  [Zeilenbreiten %d, %d px <= 271] */\n"
            % (MSG_SPERRE, SATZ, breite(Z1), breite(Z2)))
    carr(f, "k_r39o_msg_sperre", text)
    text_out = f.getvalue()
    for name, zeilen in programme:
        print("== %s (%d B)" % (name, sum(len(x) for x, _ in zeilen)))
        o = 0
        for x, k in zeilen:
            print("  +%04X %-30s %s" % (o, x.hex(" "), k))
            o += len(x)
    print("== k_r39o_msg_sperre: %s\n   %s" % (SATZ, text.hex(" ")))
    if pruefen:
        ok = os.path.exists(OUT) and open(OUT, encoding="utf-8").read() == text_out
        print("%s %s" % (OUT, "ok" if ok else "ANDERS"))
        return 0 if ok else 1
    with io.open(OUT, "w", encoding="utf-8", newline="\n") as fo:
        fo.write(text_out)
    print("->", OUT)
    return 0


if __name__ == "__main__":
    sys.exit(main())
