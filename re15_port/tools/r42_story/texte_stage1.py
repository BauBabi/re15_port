#!/usr/bin/env python3
"""Runde 42 Story, Stufe STAGE1: Port-Nachrichten der Szene vor der Leiter 11A0/11A1 -> engine/src/gen/story1_r42_texte.inc

    python re15_port/tools/r42_story/texte_stage1.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_stage1.py --check   (prueft nur)

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md,
Abschnitt 3.2 L3, 4.2 E2, 6b "Rueckgabe der Uhr in 11A1"; Korrekturen K-07, K-08; Entscheid F-26). Ids und Sprecher = VERTRAG
analysis/befunde_runde42/VERTRAG.md Abschnitt 2.3 (Zensus zensus_r42.py, Werte in include/re15_story_r42.h:
RE15_R42_MSG_11A_ERSTE 3, RE15_R42_MSG_11A0_LETZTE 6, RE15_R42_MSG_11A1_LETZTE 10; ROOM11A0/11A1 haben die RDT-Nachrichten 0..2).

Kodierung, Farben und Zeilenbreite wie texte_stage2.py (dieselben Funktionen, von dort importiert): Untertitel
`04 00 05 <Farbe> <Name:> 05 00 00 <Text> [08 <Text>] 04 01 01 63`, Farben Leon/Elza 01, Marvin/John 07, Sherry 06
(ausgelieferte Zeilen ROOM2000/2001, Belege im Kopf von texte_stage2.py), Zeilen <= 271 px.
Stimme je Zeile: synchro/STAGE1/room11A0/main03..06.wav bzw. synchro/STAGE1/room11A1/main03..10.wav (fehlt die Datei, gilt die
Textdauer: re15_msg_install_durations in story1_r42.c).
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import texte_stage2 as t2   # noqa: E402  (Kodierung, Umbruch, Breiten)

ZIEL = os.path.join(t2.PORT, 'engine', 'src', 'gen', 'story1_r42_texte.inc')

# (Raum, Id, Farbe, Sprecher, Text)  -  VERTRAG 2.3, Block B (11A0 L3, 11A1 E2 + 6b/F-26)
UNTERTITEL = (
    (0x11A0, 3, 0x07, 'Marvin', "Leon, wait up!"),
    (0x11A0, 4, 0x07, 'Marvin', "No sign of the girl back there. And nothing's following us."),
    (0x11A0, 5, 0x01, 'Leon',   "Good. Let's get out of here."),
    (0x11A0, 6, 0x07, 'Marvin', "I'll go up first. Stay close."),
    (0x11A1, 3, 0x06, 'Sherry', "Elza! Wait for us!"),
    (0x11A1, 4, 0x07, 'John',   "It's clear back there. Nothing's following us."),
    (0x11A1, 5, 0x01, 'Elza',   "Good. Let's get out of here."),
    (0x11A1, 6, 0x01, 'Elza',   "Sherry, wait. I think this is yours."),
    (0x11A1, 7, 0x06, 'Sherry', "My watch! I must have dropped it in the water..."),
    (0x11A1, 8, 0x01, 'Elza',   "The current carried it all the way down to that alligator."),
    (0x11A1, 9, 0x01, 'Elza',   "Hold on to it. Your mom was right. It IS good luck."),
    (0x11A1, 10, 0x07, 'John',  "I'll go up first. Sherry, you're right behind me."),
)


def main():
    fehler = []
    out = ['/* Generiert von re15_port/tools/r42_story/texte_stage1.py - nicht von Hand aendern. PORT-STORY (Konzept 3.2 L3,',
           ' * 4.2 E2, 6b; K-07, K-08, F-26; VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3). */']
    tab = []
    for raum, mid, farbe, name, text in UNTERTITEL:
        b, zeilen, bw = t2.untertitel(farbe, name, text, raum)
        for i, w in enumerate(bw):
            if w > t2.MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, t2.MAX_PX))
        n = 'k_s1_%04x_msg%d' % (raum, mid)
        out += t2.carr(n, b, 'ROOM%04X %d %s: %s  (Zeilen %s px; %s)' % (raum, mid, name, ' / '.join(zeilen), '/'.join(map(str, bw)),
                                    t2.nachlauf_r42.nachlauf(raum, len(zeilen))[1]))
        tab.append((raum, mid, n))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } s1_text_t;')
    out.append('static const s1_text_t k_s1_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define S1_TEXTE_N ((int)(sizeof k_s1_texte / sizeof k_s1_texte[0]))')
    if fehler:
        sys.exit('\n'.join(fehler))
    text = '\n'.join(out) + '\n'
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline='\n').write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
