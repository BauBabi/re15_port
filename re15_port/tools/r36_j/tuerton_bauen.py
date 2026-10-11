#!/usr/bin/env python3
"""tuerton_bauen.py - Spur J (Runde 36, Nachbesserung 1): der Tonteil des RE2-Tuerarchivs DOOR1A als
engine/src/gen/szl11b0_tuerton.inc — der Tuer-zu-Ton, wenn Marvin die Werkstatt-Tuer in ROOM11B0 SCHLIESST.

NUTZER-VORGABE (AUFTRAG.md Nachtrag 2026-10-10 abends): erst schliessen (Animation + Tuer-zu-Ton aus der
RE2-Tuerbank), dann abschliessen. RE2 spielt den Tuer-zu-Ton als Satz 1 ("Door_exit") des Tuerarchivs:
RE2 PSX.EXE @0x800141f0 `jal 0x8005ba28` mit @0x800141f4 `lui a0,0x1` (Satz 1) — derselbe Satz, den der
Port in ROOM10F0 fuer "Tuer schliessen" spielt (cut_10f0.c, DOOR13).

Warum DOOR1A (PORT-WAHL, die Werkstatt-Tuer ist keine Door_aot_set-Tuer und hat kein eigenes Archiv):
  - es ist das Archiv der ANDEREN Tuer desselben Raums (gen/tuer_zuordnung.inc S094 ROOM11B0 -> ROOM11C0
    "DOOR1A V0"; Hintergrund ROOM11B.BSS Bild 1 und Bild 2: beide eine graue Metalltuer mit Druecker),
  - DOOR1A ist "Druecker + Kartenleser" (analysis/befunde_runde31/tueren_02_re2.md Z.256) — der Nutzer nennt
    die Werkstatt-Tuer eine elektronische Tuer (AUFTRAG.md Z.129; sie oeffnet bei Strom, sub00 @0x0118C),
  - Tonfamilie F4 (= DOOR15/DOOR23, bytegleich), Satz 1 = VAG 3, 0,89 s (tueren_02_re2.md Z.341).
Tonteil = Datei[0 .. 0x5338) laut RE2-Archivtabelle @0x8009A658 `38 53 fc 92 0b 00 00 00 f5 f4 00 00`
(gen/re2_tuer_tabelle.inc DOOR1A), Bytes UNVERAENDERT aus shared_assets/RE2/DOOR/DOOR1A.DO2 (60156 B);
Tonkopf `00 00 14 16 | 00 00 24 17 | ff..` (Satz 0 / Satz 1), VH "pBAV" @0x10 — Lader wie TORSE
(re15_audio_re2_tuer_laden, audio_pc.c).
"""
import hashlib, os, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
SRC = os.path.join(PORT, "shared_assets", "RE2", "DOOR", "DOOR1A.DO2")
OUT = os.path.join(PORT, "engine", "src", "gen", "szl11b0_tuerton.inc")
TON = 0x5338   # RE2 @0x8009A658 Halbwort 0

d = open(SRC, "rb").read()
if len(d) != 60156:
    sys.exit("DOOR1A.DO2: %d B statt 60156" % len(d))
ton = d[:TON]
if ton[:16] != bytes([0, 0, 0x14, 0x16, 0, 0, 0x24, 0x17]) + b"\xff" * 8 or ton[0x10:0x14] != b"pBAV":
    sys.exit("DOOR1A.DO2: Tonkopf/VH weicht ab")
sha = hashlib.sha1(ton).hexdigest()
lines = ["/* ERZEUGT von re15_port/tools/r36_j/tuerton_bauen.py - NICHT von Hand aendern.",
         " * Tonteil von shared_assets/RE2/DOOR/DOOR1A.DO2 [0 .. 0x%04X) (RE2-Archivtabelle @0x8009A658)," % TON,
         " * bytegleich; sha1 %s. Satz 0 = Skript-Se_on (oeffnen), Satz 1 = Door_exit (zu). */" % sha,
         "#define RE15_SZL_TUERTON_LEN %d" % len(ton),
         "static const uint8_t k_szl_tuerton[RE15_SZL_TUERTON_LEN] = {"]
for i in range(0, len(ton), 16):
    lines.append("    " + ", ".join("0x%02x" % b for b in ton[i:i + 16]) + ",")
lines.append("};")
with open(OUT, "w", encoding="utf-8", newline="\n") as f:
    f.write("\n".join(lines) + "\n")
print("-> %s: %d Bytes, sha1 %s" % (OUT, len(ton), sha))
