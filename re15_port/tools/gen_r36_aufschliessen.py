#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""gen_r36_aufschliessen.py - Runde 36 Spur G (Punkt 3): die Stellen, an denen in RE1.5 eine Tuer
ERFOLGREICH aufgeschlossen wird, und welche RE2-Tonfolge dort laeuft.

Nutzer (AUFTRAG.md Runde 36): "Türen die erfolgreich geöffnet wurden mit Karte oder sowas machen in Resident
Evil 2 erst ein "piep" Sound und dann ein unlock Geräusch. Bei uns nicht."

⛔ RE2-ERGAENZUNG, KEIN RE1.5-ORIGINAL: kein Aufschliess-Block in RE1.5 hat ein Se_on (Spalte "eigen_se" der
Ausgabe; Bedingung C unten). RE2 (analysis/befunde_runde36/G_tueren.md G3):
  Kartenleser-Tuer ROOM2110 sub08: @0x01B4C Se_on(2,0x0C) Karte ein, Sleep 15, @0x01B5C Se_on(2,0x0D) PIEP,
      Sleep 15, @0x01B90 Se_on(2,0x0B) Aufschliessen.
  Von dieser Seite entriegelt (key_type 0xFE): EXE @0x800515ec Text "You unlocked it." + @0x800515f8
      Se_on(2,0x26) im selben Bild.
  Mechanischer Schluessel (@0x80051658 Se_on(2,0x25)) - in RE1.5 kein Treffer dieser Art (s. u.).

ARTEN (Wortlaut der Nachricht, normalisiert; die Auswahl ueber den Wortlaut ist PORT-WAHL wie
tools/gen_lock_se_sites.py, belegt ist je Art die RE2-FOLGE):
  KARTE_EIN  "You've used the <Farbe> Keycard." in einem Raum MIT Ziffernfeld ("Enter the first number.")
             -> nur Karte ein (Satz 0, t = 0). Das Aufschliessen kommt erst nach dem Code.
  KARTE      "You've used the ... Keycard." in einem Raum OHNE Ziffernfeld -> volle RE2-Folge
             Karte ein (t 0) / Piep (t 15) / Aufschliessen (t 30).
  KARTE_AUF  "You've opened the lock." in einem Raum mit Ziffernfeld oder Minidisc-Schloss
             ("You've used the Minidisc Player w/ Disc.") -> Piep (t 0) / Aufschliessen (t 15).
  RIEGEL     "You've opened the lock." sonst (Riegel von dieser Seite) -> RE2 Satz 0x26 (t 0).
Bedingungen: B die Nachricht oeffnet ein Message_on im Skript (Weg 2) oder ein Text-AOT (Weg 1);
             C zwischen dem vorigen Message_on des Blocks und diesem steht KEIN Se_on (sonst hat RE1.5
               dort einen eigenen Ton -> nicht aufgenommen).

Ausgabe: --install -> re15_port/engine/src/gen/r36_aufschliessen.inc, sonst build/r36_tueren/r36_aufschliessen.inc
"""
import glob
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import scd_walk_lib as L  # noqa: E402


def norm(t):
    return re.sub(r"\s+", " ", t.replace(" / ", " ")).strip()


RX_KARTE = re.compile(r"^You've used the (Blue|Yellow|Red|Green) Keycard\.$|^You've used the Blue Master Keycard\.$")
RX_AUF = re.compile(r"^You've opened the lock\.$")
RX_ZIFFER = re.compile(r"^Enter the first number\.$")
RX_MINIDISC = re.compile(r"^You've used the Minidisc Player w/ Disc\.$")

ARTEN = ("KARTE_EIN", "KARTE", "KARTE_AUF", "RIEGEL")


def main():
    install = "--install" in sys.argv
    out = (os.path.join(REPO, "re15_port", "engine", "src", "gen", "r36_aufschliessen.inc") if install
           else os.path.join(REPO, "build", "r36_tueren", "r36_aufschliessen.inc"))
    os.makedirs(os.path.dirname(out), exist_ok=True)
    files = sorted(glob.glob(os.path.join(REPO, "re15_port", "shared_assets", "PSX", "STAGE*", "ROOM*.RDT")))
    sites, verworfen = [], []
    n_rdt = 0
    for f in files:
        room = int(os.path.basename(f)[4:8], 16)
        d = open(f, "rb").read()
        if len(d) < 0x100:
            continue
        n_rdt += 1
        ms = {k: norm(v[0]) for k, v in L.messages(d).items()}
        ziffer = any(RX_ZIFFER.match(t) for t in ms.values())
        minidisc = any(RX_MINIDISC.match(t) for t in ms.values())
        reg = L.regionen(d)
        mstart = L.u32(d, 0x3C)
        for mi, text in sorted(ms.items()):
            if RX_KARTE.match(text):
                art = "KARTE_EIN" if ziffer else "KARTE"
            elif RX_AUF.match(text):
                art = "KARTE_AUF" if (ziffer or minidisc) else "RIEGEL"
            else:
                continue
            skript, aot, eigen = [], [], []
            for (tag, idx), ops in sorted(reg.items()):
                blk = "%s%02d" % (tag, idx)
                letzte_msg = -1
                se_seit = False
                for pc, op, sz in ops:
                    if op == 0x36:
                        se_seit = True
                    if op == 0x2B:
                        if d[pc + 1] == mi:
                            (eigen if se_seit else skript).append((pc, blk))
                        se_seit = False
                    elif op == 0x2C and d[pc + 2] == 1:
                        pay = pc + (22 if (d[pc + 3] & 0x80) else 14)
                        if (d[pay] | (d[pay + 1] << 8)) == mi:
                            aot.append((pc, blk))
                    elif op == 0x46 and d[pc + 2] == 1 and (d[pc + 4] | (d[pc + 5] << 8)) == mi:
                        aot.append((pc, blk))
            wege = (1 if aot else 0) | (2 if skript else 0)
            moff = mstart + L.u16(d, mstart + 2 * mi)
            if not wege:
                verworfen.append((room, mi, art, text, "B/C: kein Weg ohne eigenes Se_on (eigen: %s)" % (
                    ", ".join("%s @0x%05X" % (b, p) for p, b in eigen) or "-")))
                continue
            sites.append(dict(room=room, msg=mi, art=art, text=text, wege=wege, skript=skript, aot=aot, moff=moff))

    with open(out, "w", newline="\n", encoding="utf-8") as o:
        o.write("/* ERZEUGT von re15_port/tools/gen_r36_aufschliessen.py - NICHT von Hand aendern.\n"
                " * Runde 36 Spur G: Stellen, an denen in RE1.5 eine Tuer ERFOLGREICH aufgeschlossen wird (Nachricht\n"
                " * geht auf), und die RE2-Tonfolge dazu (Arten und Belege im Generator-Kopf und in\n"
                " * engine/src/aufschliessen_r36.c). %d RDTs gelesen, %d Stellen, %d verworfen.\n"
                " * wege: Bit 0 = Text-AOT (re15_scd_show_message), Bit 1 = Skript (Message_on). */\n\n"
                % (n_rdt, len(sites), len(verworfen)))
        o.write("typedef struct { uint16_t room; uint8_t msg; uint8_t art; uint8_t wege; } re15_auf_stelle_t;\n\n")
        for i, a in enumerate(ARTEN):
            o.write("#define RE15_AUF_ART_%-9s %d\n" % (a, i))
        o.write("\nstatic const re15_auf_stelle_t re15_auf_stellen[] = {\n")
        for s in sites:
            wo = ["%s @0x%05X" % (b, p) for p, b in s["skript"]] + ["AOT %s @0x%05X" % (b, p) for p, b in s["aot"]]
            o.write("    { 0x%04X, %2d, RE15_AUF_ART_%-10s %d },   /* msg @0x%05X \"%s\" | %s */\n" % (
                s["room"], s["msg"], s["art"] + ",", s["wege"], s["moff"], s["text"], "; ".join(wo)))
        o.write("};\n#define RE15_AUF_STELLEN %d\n" % len(sites))
        o.write("\n/* VERWORFEN:\n")
        for room, mi, art, text, grund in verworfen:
            o.write(" *   ROOM%04X msg %d (%s) \"%s\": %s\n" % (room, mi, art, text, grund))
        o.write(" */\n")
    for s in sites:
        print("ROOM%04X msg %2d %-9s wege %d  %s  %s" % (s["room"], s["msg"], s["art"], s["wege"], s["text"],
                                                      ", ".join("%s @0x%05X" % (b, p) for p, b in s["skript"])))
    for v in verworfen:
        print("VERWORFEN ROOM%04X msg %d %s %s: %s" % v)
    print("%d Stellen -> %s" % (len(sites), out))
    return 0


if __name__ == "__main__":
    sys.exit(main())
