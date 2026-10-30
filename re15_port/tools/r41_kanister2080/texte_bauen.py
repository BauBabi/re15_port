#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Runde 41 Spur G "kanister2080": Port-Texte ROOM2080/2081 Id 42 -> engine/src/gen/kanister2080_r41_texte.inc.

  python re15_port/tools/r41_kanister2080/texte_bauen.py [--pruefen]

VERTRAG 1.2: G = 2080/2081 Ids 42-47, 42 = "Hey, Leon/Elza, try with this!" (NUTZER-VORGABE, AUFTRAG.md: Marvin "sagt so
etwas wie - Hey, Leon, try with this!"; Elzas Fassung laut Orchestrator-Hinweis "Hey, Elza, try with this!" (John)).
FORM = die ausgelieferten NPC-Zeilen derselben Stage: Marvin = ROOM2000 msg 1 @0x1BFD ("Marvin: What's up Leon?"),
John = ROOM2001 msg 0 @0x1E0A ("John: Elza, over here!"): `04 00 05 07 <Sprecher:> 05 00 <Text> 04 01 01 63` (Farbe 7 =
NPC). Kodierung/Breite = Werkzeug der Runde 38 Spur F (tools/r38_dining/programme_bauen.py enc/breite), Zeilenbreite
<= 271 px (Textkasten). Stimmen: synchro/STAGE2/room2080/main42.wav und synchro/STAGE2/room2081/main42.wav (Regel
synchro/README.md; liegen sie nicht vor, laeuft die Zeile ohne Stimme).
"""
import importlib.util, os, sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HIER))
OUT = os.path.join(PORT, "engine", "src", "gen", "kanister2080_r41_texte.inc")
_spec = importlib.util.spec_from_file_location("r38bau", os.path.join(PORT, "tools", "r38_dining", "programme_bauen.py"))
R = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(R)

D2000 = open(os.path.join(PORT, "shared_assets", "PSX", "STAGE2", "ROOM2000.RDT"), "rb").read()
D2001 = open(os.path.join(PORT, "shared_assets", "PSX", "STAGE2", "ROOM2001.RDT"), "rb").read()


def vorbild(d, off, sprecher, name):
    kopf = bytes([0x04, 0x00, 0x05, 0x07]) + R.enc(sprecher) + bytes([0x05, 0x00])
    if d[off:off + len(kopf)] != kopf:
        sys.exit("ABBRUCH: Vorbild %s @0x%05X Kopf %s" % (name, off, d[off:off + len(kopf)].hex(" ")))
    ende = d.find(bytes([0x04, 0x01, 0x01, 0x63]), off)
    if ende < 0 or ende - off > 120:
        sys.exit("ABBRUCH: Vorbild %s @0x%05X ohne Ende 04 01 01 63" % (name, off))
    return kopf


def zeile(kopf, text_nach_sprecher, voll):
    if R.breite(voll) > 271:
        sys.exit("ABBRUCH: Zeile zu breit (%d px > 271): %r" % (R.breite(voll), voll))
    return kopf + R.enc(text_nach_sprecher) + bytes([0x04, 0x01, 0x01, 0x63])


def main():
    k_marvin = vorbild(D2000, 0x1BFD, "Marvin:", "ROOM2000 msg 1")
    k_john = vorbild(D2001, 0x1E0A, "John:", "ROOM2001 msg 0")
    texte = [
        ("k_k2080_msg42_leon", zeile(k_marvin, " Hey, Leon, try with this!", "Marvin: Hey, Leon, try with this!"),
         "ROOM2080 msg 42 (Marvin): Form ROOM2000 msg 1 @0x1BFD", "Marvin: Hey, Leon, try with this!"),
        ("k_k2080_msg42_elza", zeile(k_john, " Hey, Elza, try with this!", "John: Hey, Elza, try with this!"),
         "ROOM2081 msg 42 (John): Form ROOM2001 msg 0 @0x1E0A", "John: Hey, Elza, try with this!"),
    ]
    out = ["/* Erzeugt von re15_port/tools/r41_kanister2080/texte_bauen.py - NICHT von Hand aendern. */"]
    for name, b, beschr, voll in texte:
        out.append("/* %s - \"%s\" [Zeilenbreite %d px] */" % (beschr, voll, R.breite(voll)))
        out.append("static const uint8_t %s[%d] = {" % (name, len(b)))
        for i in range(0, len(b), 16):
            out.append("    " + ",".join("0x%02x" % x for x in b[i:i + 16]) + ",")
        out.append("};")
    s = "\n".join(out) + "\n"
    if "--pruefen" in sys.argv:
        alt = open(OUT, encoding="utf-8").read() if os.path.exists(OUT) else ""
        if alt.replace("\r\n", "\n") != s:
            sys.exit("ABBRUCH: %s veraltet" % OUT)
        print("ok")
        return
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write(s)
    print(s)


if __name__ == "__main__":
    main()
