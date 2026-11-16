#!/usr/bin/env python3
"""Runde 42 Story, Stufe Kleinkram (T-09): Beschreibung der Blue Master Keycard -> engine/src/gen/story_klein_r42_texte.inc

    python re15_port/tools/r42_story/texte_klein.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_klein.py --check   (prueft nur)

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md, Abschnitt 5
T-09 [B39]; VERTRAG analysis/befunde_runde42/VERTRAG.md Abschnitt 3): "An ID card for the research center. It belongs to someone
named "Michael."" - die Notiz in 4010 nennt Michael (ROOM4010 msg 39 @0x183D "To Michael: I left an ID card on the right desk.").

Item 0x47 = Blue Master Keycard: Namensbank gen/inv_name_bank.inc (Eintrag 0x47 "Blue Master Keycard"), einziger Fundort ROOM4010
main00 Item_aot_set Slot 9 @0x00592 `50 09 09 31 .. 47 00 01 00 06 00` (Zensus aller RDTs: kein weiterer 0x47-Satz). Die
ausgelieferte Beschreibung (Bank @0x800c50de, Eintrag 0x47 @0x800c5e53, DEBUG.BIN) lautet "It seems to belong to a / researcher
named "Goro / Macida"." - drei Zeilen auf einer Seite, Zeilen mit 08, Ende 01 00. Die neue Beschreibung nimmt dieselbe Form:
gieriger Umbruch auf <= 191 px (breiteste Zeile der Bank, texte_stage2.py MAX_DESC_PX), hoechstens 3 Zeilen je Seite (Zensus der
Bank: keine Seite hat mehr als 3 Zeilen). Die Beschreibung haengt am Gegenstand, nicht an der Figur: sie gilt fuer Leon und Elza.
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import texte_stage2 as t2  # noqa: E402  (Glyphen, Breiten, Beschreibungsform)

ZIEL = os.path.join(t2.PORT, 'engine', 'src', 'gen', 'story_klein_r42_texte.inc')
ITEM_BLUE = 0x47
TEXT_BLUE = 'An ID card for the research center. It belongs to someone named "Michael."'
MAX_ZEILEN = 3


def main():
    zeilen = t2.umbruch(TEXT_BLUE, 0, t2.MAX_DESC_PX)
    fehler = []
    if len(zeilen) > MAX_ZEILEN:
        fehler.append('Blue Master Keycard: %d Zeilen > %d' % (len(zeilen), MAX_ZEILEN))
    b, bw = t2.beschreibung([zeilen])
    for i, w in enumerate(bw):
        if w > t2.MAX_DESC_PX:
            fehler.append('Blue Master Keycard Zeile %d = %d px > %d' % (i + 1, w, t2.MAX_DESC_PX))
    if fehler:
        sys.exit(chr(10).join(fehler))
    out = ['/* Generiert von re15_port/tools/r42_story/texte_klein.py - nicht von Hand aendern. PORT-STORY (Konzept 5 T-09,',
           ' * VERTRAG analysis/befunde_runde42/VERTRAG.md 3). */',
           '#define RE15_SKL_ITEM_BLUE 0x%02X   /* Blue Master Keycard: ROOM4010 Item_aot_set Slot 9 @0x00592 (Item 0x47) */' % ITEM_BLUE]
    out += t2.carr('k_skl_blue_desc', b, 'Item 0x47 Beschreibung (T-09): %s  (Zeilen %s px; Original 0x47 @0x800c5e53 3 Zeilen, 1 Seite)'
                   % (' / '.join(zeilen), '/'.join(map(str, bw))))
    text = chr(10).join(out) + chr(10)
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline=chr(10)).write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
