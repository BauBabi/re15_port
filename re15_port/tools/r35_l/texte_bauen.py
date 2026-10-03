#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""texte_bauen.py - Spur L (Runde 35): die neuen Nachrichten der 1150-Montage als .msg-Rohbytes
(Form wie tools/r34n_d/texte_bauen.py: Glyphentabelle = Umkehrung von msg_common.c re15_msg_glyph,
Dialogzeile `04 00 05 cc <Name> 16 05 00 00 <Text> 04 01 01 63` = ROOM1090 msg 0 @0x275C / msg 1
@0x279C, Tuer-Text `04 02 <Text> 01 00` = ROOM1130 msg 1 @0x0B46; Umbruch 0x08 (FUN_80028868);
Zeilenbreite aus include/font_width.h = DEBUG.BIN[0x4416+code]; Umbruch so, dass jede Zeile <= 271 px
bleibt (99 % der 2286 ausgelieferten Zeilen, tuer1120_1130.c).
Sprecherfarben: Leon 01 (ROOM1150 msg 4 @0x1453), Irons 02 (msg 5 @0x149E), Ada 02 (ROOM11C0 msg 1
@0x1CD0), Marvin 07 (ROOM11B0 msg 1 @0x190E).

Aufruf: python re15_port/tools/r35_l/texte_bauen.py [--c]
"""
import os, re, sys

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
FW = os.path.join(REPO, "re15_port", "include", "font_width.h")

ENC = {" ": 0x00, ":": 0x16, ",": 0x18, '"': 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10): ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i

def widths():
    src = open(FW, encoding="utf-8", errors="replace").read()
    nums = [int(x, 0) for x in re.findall(r"\b(0x[0-9a-fA-F]+|\d+)\b", src.split("{", 1)[1].split("}", 1)[0])]
    return nums

WID = widths()
MAX_PX = 271

def enc(text):
    return bytes(ENC[c] for c in text)

def breite(text):
    return sum(WID[ENC[c]] for c in text)

def umbrechen(text, erste_breite):
    """Woerter so auf Zeilen verteilen, dass Zeile 1 (hinter dem Sprecher) und alle weiteren <= MAX_PX."""
    worte = text.split(" ")
    zeilen, akt, akt_b = [], "", erste_breite
    for w in worte:
        probe = (akt + " " + w) if akt else w
        if akt and akt_b + breite(" " + w) > MAX_PX:
            zeilen.append(akt); akt, akt_b = w, breite(w)
        else:
            akt, akt_b = probe, (akt_b + breite(" " + w) if akt else akt_b + breite(w))
    zeilen.append(akt)
    return zeilen

def dialog(sprecher, farbe, text):
    kopf = bytes([0x04, 0x00, 0x05, farbe]) + enc(sprecher) + bytes([0x16, 0x05, 0x00, 0x00])
    vor = breite(sprecher + ": ")
    zeilen = umbrechen(text, vor)
    body = b""
    for i, z in enumerate(zeilen):
        if i: body += b"\x08"
        body += enc(z)
    return kopf + body + bytes([0x04, 0x01, 0x01, 0x63]), zeilen, vor

def textplatz(text):
    zeilen = umbrechen(text, 0)
    body = b""
    for i, z in enumerate(zeilen):
        if i: body += b"\x08"
        body += enc(z)
    return bytes([0x04, 0x02]) + body + bytes([0x01, 0x00]), zeilen, 0

# (Raum, Id, C-Name, Sprecher, Farbe, Text)  — Texte woertlich AUFTRAG.md Z. 68-91 (NUTZER-VORGABE)
MELDUNGEN = [
    ("1060", 1,  "k_t1060_msg1",  None,     0,    "I have to get the Chief first..."),
    ("1150", 22, "k_msg22", "Leon",   0x01, "Sir!"),
    ("1150", 23, "k_msg23", "Leon",   0x01, "Sir, the communication system can't be fixed! We're going to use the patrol car to get out of here."),
    ("1150", 24, "k_msg24", "Leon",   0x01, "I came to get you, come with me!"),
    ("1150", 25, "k_msg25", "Irons",  0x02, "Leon... I... I'm proud to have an officer as dependable as you!"),
    ("1150", 26, "k_msg26", "Irons",  0x02, "But... I... I'm not going to make it... I'm sorry..."),
    ("1150", 27, "k_msg27", "Irons",  0x02, "Please... one last favor... Look after yourself and the others who survived."),
    ("1150", 28, "k_msg28", "Irons",  0x02, "Be a hero... Leon..."),
    ("1150", 29, "k_msg29", "Leon",   0x01, "SIR, Sir?!"),
    ("11C0", 10, "k_msg10", "Ada",    0x02, "What was this noise? Did you hear that?"),
    ("11C0", 11, "k_msg11", "Marvin", 0x07, "Yes!...."),
    ("11C0", 12, "k_msg12", "Marvin", 0x07, "Oh, no, Leon!"),
    ("11C0", 13, "k_msg13", "Marvin", 0x07, "I have to help him, sorry!"),
    ("11C0", 14, "k_msg14", "Ada",    0x02, "Marvin!..."),
]

def main():
    c_mode = "--c" in sys.argv
    for raum, mid, cname, spr, farbe, text in MELDUNGEN:
        if spr is None:
            b, zeilen, vor = textplatz(text)
        else:
            b, zeilen, vor = dialog(spr, farbe, text)
        if c_mode:
            print("static const uint8_t %s[] = {   /* ROOM%s msg %d: %s%s */" % (cname, raum, mid, (spr + ": ") if spr else "", text))
            for i in range(0, len(b), 16):
                print("    " + ", ".join("0x%02x" % x for x in b[i:i + 16]) + ",")
            print("};")
        else:
            print("ROOM%s msg %2d %-10s %3d B  Zeilen: %s" % (raum, mid, cname, len(b),
                  " | ".join("%s (%d px)" % (z, (vor if i == 0 else 0) + breite(z)) for i, z in enumerate(zeilen))))

if __name__ == "__main__":
    main()
