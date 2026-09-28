#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_lock_se_sites.py — leitet aus den ausgelieferten RE1.5-RDTs die Stellen ab, an denen
der Spieler an einer VERSCHLOSSENEN TUER steht, und erzeugt daraus die Tabelle
(Raum, Nachricht) -> Art des RE2-"verschlossen"-Tons.

⛔ RE2-ERGAENZUNG, KEIN RE1.5-Original (Dossier analysis/befunde_runde30/tuer-verschlossen.md):
   RE1.5 verschliesst Tueren ueber die DATENWAHL (derselbe AOT-Platz bekommt sce=1 Text ODER
   sce=2 Tuer, analysis/door_lock_1170.md §3) und spielt dabei nichts. RE2 spielt an der
   verschlossenen Tuer Se_on(Bank 2, Satz 0x16) — im EXE-Tuer-Handler @0x80051610/@0x800516a4
   und in den Raumskripten (z.B. ROOM2110.RDT sub09 @0x01BBC).

AUFNAHMEBEDINGUNGEN (alle muessen gelten, sonst VERWORFEN mit Grund):
  A  Der Text der Nachricht passt auf eine der woertlichen Schloss-Formeln unten (Art K oder M).
     Keine Formel = keine Aufnahme; es wird nichts "aehnliches" mitgenommen.
  B  Die Nachricht ist im Raum erreichbar: ueber einen Text-AOT (Aot_set/Aot_reset mit sce=1)
     oder ueber ein Message_on im Skript.
  C  Wege mit EIGENEM RE1.5-Ton bleiben RE1.5: steht im selben Skriptblock wie das Message_on
     ein Se_on, wird der Skript-Weg NICHT aufgenommen (ROOM4000 sub02 @0x0142E spielt
     Se_on(2,0x0f) vor msg 0).
  Tuer-Beleg (nur ausgewiesen, keine Bedingung): "Zwilling" = derselbe AOT-Platz bekommt im Raum
     auch ein Door_aot_set bzw. ein Aot_reset auf sce=2; sonst "Text".

ARTEN
  K  elektronisch verriegelt / Kartenleser / Ausweis noetig  -> RE2_DOOR_SE_ZU_E
     (RE2 ROOM2110 Satz 0x16, die Kartenleser-Tuer zur Waffenkammer)
  M  mechanisch verschlossen / von der anderen Seite         -> RE2_DOOR_SE_ZU_A
     (RE2 ROOM1140 Satz 0x16; RE2 nimmt in 9 Revier-Raeumen diese Welle, in 6 die kuerzere
      ZU_B — eine Regel dafuer ist aus den Daten NICHT ableitbar, siehe Dossier §7)
  S  ohne Strom ("The door won't open until the power is restored!") -> RE2_DOOR_SE_ZU_P
     Nachbesserung Runde 30 (Gegenpruefer-Mangel 1). KEINE Port-Wahl: der Text steht
     woertlich in RE2 ROOM7020 (msg 2 @Datei 0x01F69), und RE2 spielt ihn mit Ton:
     sub06 @Datei 0x01598 Message_on 2 / @Datei 0x0159E Se_on(2,0x16) -> Raumbank-Satz
     0x16 = Welle ea19d086e3cb (tools/re2_door_se_cut.py Satz 3). RE1.5 ROOM5080/5081 ist
     derselbe Raum (Generator, dieselben vier Texte); dort legt sub02 @Datei 0x007C2 /
     0x007BA den Text per Aot_reset sce 1 auf den Tuer-Platz 0 (Door_aot_set main00 @0x006FA).
     Diese Formel hat KEIN lock/latched im Wortlaut; deshalb fehlte die Stelle vorher
     auch in der Verworfen-Liste (die zaehlt nur Texte mit diesen Woertern).

Ausgabe (Standard, Ermittlungsstand): build/r30_tuer-verschlossen/lock_se_sites.inc
Mit --install (Bau-Agent):            re15_port/engine/src/gen/lock_se_sites.inc
"""
import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import scd_walk_lib as L

# Woertliche Formeln. Der Text wird vorher normalisiert: Seitenumbruch " / " und
# Mehrfach-Leerzeichen zu EINEM Leerzeichen.
FORMELN = [
    ("K", "elektronisch verriegelt",   re.compile(r"It's electronically ?locked\.")),
    ("K", "Ausweis noetig",            re.compile(r"An ID card is ?required to (open|unlock) it\.")),
    ("M", "von der anderen Seite",     re.compile(r"^It's locked from the other side\.$")),
    ("M", "verschlossen",              re.compile(r"^It's locked\.$")),
    ("M", "verschlossen",              re.compile(r"^The door is locked\.$")),
    ("M", "verschlossen (Buero)",      re.compile(r"^It's a small office\. ?It's locked\.$")),
    ("M", "verschlossen (jemand drin)", re.compile(r"^It's locked\. ?It seems that there is someone ?inside")),
    ("M", "verschlossen (Raumname)",   re.compile(r"^[A-Z][A-Za-z ]+\"It's locked\.$")),
    ("M", "fest verschlossen",         re.compile(r"The door is tightly locked\.")),
    ("M", "verschlossen (Feuer)",      re.compile(r"^The door is locked because of ?the fire!$")),
    ("M", "verriegelt, Schluessel fehlt", re.compile(r"^The door is latched shut and won't open\.")),
    ("S", "ohne Strom (RE2 ROOM7020)",   re.compile(r"^The door won't open until the power is restored!$")),
]


def norm(t):
    t = t.replace(" / ", " ")
    t = re.sub(r"\s+", " ", t).strip()
    return t


def klassifiziere(t):
    n = norm(t)
    for art, name, rx in FORMELN:
        if rx.search(n):
            return art, name
    return None, None


def main():
    install = "--install" in sys.argv
    out = (os.path.join(REPO, "re15_port", "engine", "src", "gen", "lock_se_sites.inc") if install
           else os.path.join(REPO, "build", "r30_tuer-verschlossen", "lock_se_sites.inc"))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    files = sorted(glob.glob(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT")))
    n_rdt = n_stub = 0
    sites = []          # aufgenommen
    verworfen = []      # (raum, msg, text, grund)
    n_lockwort = 0
    for f in files:
        room = int(os.path.basename(f)[4:8], 16)
        d = open(f, "rb").read()
        if len(d) < 0x100:
            n_stub += 1
            continue
        n_rdt += 1
        ms = L.messages(d)
        mstart = L.u32(d, 0x3C)
        reg = L.regionen(d)
        # Tuer-Belege je Platz
        door_slots = set()
        for (tag, idx), ops in reg.items():
            for pc, op, sz in ops:
                if op == 0x3B:
                    door_slots.add(d[pc + 1])
                elif op == 0x46 and d[pc + 2] == 2:
                    door_slots.add(d[pc + 1])
        for mi, (text, ctrl) in sorted(ms.items()):
            art, name = klassifiziere(text)
            hat_wort = re.search(r"lock|latched", text, re.I) is not None
            if hat_wort:
                n_lockwort += 1
            if art is None:
                if hat_wort:
                    verworfen.append((room, mi, norm(text), "A: keine Schloss-Formel (kein Tuer-Schloss im Wortlaut)"))
                continue
            aot = []      # (datei_off, slot, wie)
            skript = []   # (datei_off, block, hat_se_on)
            for (tag, idx), ops in sorted(reg.items()):
                blk = "%s%02d" % (tag, idx)
                hat_se = any(op == 0x36 for _, op, _ in ops)
                for pc, op, sz in ops:
                    if op == 0x2C and d[pc + 2] == 1:
                        pay = pc + (22 if (d[pc + 3] & 0x80) else 14)
                        if (d[pay] | (d[pay + 1] << 8)) == mi:
                            aot.append((pc, d[pc + 1], "Aot_set", blk))
                    elif op == 0x46 and d[pc + 2] == 1:
                        if (d[pc + 4] | (d[pc + 5] << 8)) == mi:
                            aot.append((pc, d[pc + 1], "Aot_reset", blk))
                    elif op == 0x2B and d[pc + 1] == mi:
                        skript.append((pc, blk, hat_se))
            if not aot and not skript:
                verworfen.append((room, mi, norm(text), "B: im Raum nicht erreichbar (kein sce-1-AOT, kein Message_on)"))
                continue
            wege = 0
            if aot:
                wege |= 1
            skript_ok = [s for s in skript if not s[2]]
            skript_re15 = [s for s in skript if s[2]]
            if skript_ok:
                wege |= 2
            if wege == 0:
                verworfen.append((room, mi, norm(text), "C: nur Skript-Wege mit eigenem RE1.5-Se_on (%s)"
                                  % ", ".join("%s @0x%05X" % (s[1], s[0]) for s in skript_re15)))
                continue
            zw = sorted(set(a[1] for a in aot if a[1] in door_slots))
            beleg = ("Zwilling Platz %s" % ",".join(str(z) for z in zw)) if zw else "Text"
            # Datei-Offset des Nachrichtenkoerpers
            moff = mstart + L.u16(d, mstart + 2 * mi)
            sites.append(dict(room=room, msg=mi, art=art, name=name, text=norm(text), wege=wege,
                              aot=aot, skript=skript_ok, skript_re15=skript_re15, beleg=beleg, moff=moff))

    nK = sum(1 for s in sites if s["art"] == "K")
    nM = sum(1 for s in sites if s["art"] == "M")
    nS = sum(1 for s in sites if s["art"] == "S")
    with open(out, "w", newline="\n", encoding="utf-8") as o:
        o.write("/* ERZEUGT von tools/gen_lock_se_sites.py - NICHT von Hand aendern.\n"
                " *\n"
                " * Die Stellen, an denen der Spieler an einer VERSCHLOSSENEN TUER steht, aus den\n"
                " * ausgelieferten RE1.5-Daten ABGELEITET (Bedingungen A/B/C im Generator-Kopf).\n"
                " * RE2-ERGAENZUNG: RE1.5 ist hier stumm (LAB_80043084 @0x80043084 / LAB_800430bc\n"
                " * @0x800430bc), RE2 spielt Se_on(Bank 2, Satz 0x16) @0x80051610 / @0x800516a4.\n"
                " *\n"
                " * ABDECKUNG DIESES LAUFS:\n"
                " *   %d RDTs mit Inhalt gelesen (+%d Platzhalter < 0x100 B)\n"
                " *   %d Nachrichten tragen das Wort lock/latched\n"
                " *   %d Stellen aufgenommen: %d x Art K (elektronisch/Karte), %d x Art M (mechanisch),\n"
                " *   %d x Art S (ohne Strom)\n"
                " *   %d verworfen (Liste am Dateiende)\n"
                " *\n"
                " * wege: Bit 0 = Text-AOT (sce=1, re15_scd_show_message), Bit 1 = Skript (Message_on)\n"
                " */\n\n" % (n_rdt, n_stub, n_lockwort, len(sites), nK, nM, nS, len(verworfen)))
        o.write("typedef struct { uint16_t room; uint8_t msg; uint8_t art; uint8_t wege; } re15_lock_se_site_t;\n\n")
        o.write("#define RE15_LOCK_ART_K 0   /* elektronisch / Kartenleser -> RE2_DOOR_SE_ZU_E */\n")
        o.write("#define RE15_LOCK_ART_M 1   /* mechanisch                 -> RE2_DOOR_SE_ZU_A */\n")
        o.write("#define RE15_LOCK_ART_S 2   /* ohne Strom (RE2 ROOM7020)  -> RE2_DOOR_SE_ZU_P */\n\n")
        o.write("static const re15_lock_se_site_t re15_lock_se_sites[] = {\n")
        for s in sites:
            wo = []
            for (pc, slot, wie, blk) in s["aot"]:
                wo.append("%s Platz %d %s @0x%05X" % (wie, slot, blk, pc))
            for (pc, blk, _) in s["skript"]:
                wo.append("Message_on %s @0x%05X" % (blk, pc))
            for (pc, blk, _) in s["skript_re15"]:
                wo.append("[NICHT: Message_on %s @0x%05X hat eigenes RE1.5-Se_on]" % (blk, pc))
            o.write("    { 0x%04X, %2d, RE15_LOCK_ART_%s, %d },   /* msg @0x%05X \"%s\" | %s | %s */\n"
                    % (s["room"], s["msg"], s["art"], s["wege"], s["moff"], s["text"][:90],
                       s["beleg"], "; ".join(wo)))
        o.write("};\n#define RE15_LOCK_SE_SITE_COUNT %d\n\n" % len(sites))
        o.write("/* VERWORFEN (Nachrichten mit lock/latched im Wortlaut, die KEINE Tuer-Schloss-Stelle sind):\n")
        for (room, mi, t, grund) in verworfen:
            o.write(" *   ROOM%04X msg %2d  %-60s  \"%s\"\n" % (room, mi, grund, t[:80]))
        o.write(" */\n")
    print("%d RDTs (+%d Platzhalter), %d Nachrichten mit lock/latched, %d aufgenommen (K %d / M %d / S %d), %d verworfen"
          % (n_rdt, n_stub, n_lockwort, len(sites), nK, nM, nS, len(verworfen)))
    print("-> %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
