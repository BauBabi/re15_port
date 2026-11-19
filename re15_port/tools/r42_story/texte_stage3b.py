#!/usr/bin/env python3
"""Runde 42 Story, Stufe STAGE3b (30E0, 3070): Port-Nachrichten -> engine/src/gen/story3b_r42_texte.inc

    python re15_port/tools/r42_story/texte_stage3b.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_stage3b.py --check   (prueft nur)

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md,
Abschnitt 3.2 L8-L10, Zeitleiste L-11/L-12; Entscheid F-08). Ids und Sprecher = VERTRAG analysis/befunde_runde42/VERTRAG.md
Abschnitt 2.3 (Zensus zensus_r42.py, Werte in include/re15_story_r42.h):
  ROOM30E0 msg 10/11/13 Neutext (NEUAUF, L8), 16 neu (L8), 17 neu (L10, optional); ROOM3070 msg 6 neu (L10, F-08).
  30E0 msg 3 (L7) steht schon in texte_stage3a.py (Stufe STAGE3a NB1 B5).
Nur Leons Fassung (ROOM30E0/3070); Elzas ROOM30E1/3071 bleiben unveraendert.

Kodierung = texte_stage3a.py (dieselben Funktionen, importiert). Farben aus den ausgelieferten Zeilen derselben Raeume
(rdt_msgdump.py): Leon 01 (ROOM30E0 msg 1 @0x110B `05 01`, ROOM3070 msg 0 @0x398C `05 01`), Ada 02 (ROOM30E0 msg 0 @0x10E8
`05 02`), Marvin 07 (ROOM30E0 msg 11 @0x1381 `05 07`).
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import texte_stage3a as T  # noqa: E402

ZIEL = os.path.join(T.PORT, 'engine', 'src', 'gen', 'story3b_r42_texte.inc')

# (Raum, Id, Farbe, Sprecher, Text)  -  VERTRAG 2.3, Block B
UNTERTITEL = (
    # L8 (RDT-Texte @0x133A / @0x1381 / @0x13FD werden ersetzt; sub03 Message_on 10/11/13 @0x00ED6/@0x00EFE/@0x00F5A)
    (0x30E0, 10, 0x01, 'Leon',   "That's a bite... They got you?"),
    (0x30E0, 11, 0x07, 'Marvin', "The lobby's gone...! | They smashed through the front glass and got inside!"),
    (0x30E0, 13, 0x07, 'Marvin', "There's more of them than I thought... | If they've come this far, the city's finished."),
    # L8 neu: nach dem Glasbruch (RDT 0..15 belegt)
    (0x30E0, 16, 0x01, 'Leon',   "That came from the lobby... Marvin!"),
    # L10 optional: Uebergang Ada -> Marvin
    (0x30E0, 17, 0x02, 'Ada',    "Someone's coming."),
    # L10 / F-08: 3070 nach Birkins Flucht (RDT 0..5 belegt)
    (0x3070, 6, 0x01, 'Leon',    "That came from the storage room... Ada?"),
)


def main():
    fehler = []
    out = ['/* Generiert von re15_port/tools/r42_story/texte_stage3b.py - nicht von Hand aendern. PORT-STORY (Konzept 3.2 L8-L10,',
           ' * F-08, VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3). */']
    tab = []
    for raum, mid, farbe, name, text in UNTERTITEL:
        b, zeilen, bw = T.untertitel(farbe, name, text, raum, mid)   # '|' = Seitenwechsel (Runde 42 namen Stufe 5, lesbar_s5.py)
        if len(zeilen) > 2:
            fehler.append('ROOM%04X msg %d: %d Zeilen auf einer Seite > 2 (tools/r42_namen/lesbar_s5.py)' % (raum, mid, len(zeilen)))
        for i, w in enumerate(bw):
            if w > T.MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, T.MAX_PX))
        n = 'k_s3b_%04x_msg%d' % (raum, mid)
        out += T.carr(n, b, 'ROOM%04X %d %s: %s  (Zeilen %s px; %s)' % (raum, mid, name, ' / '.join(zeilen), '/'.join(map(str, bw)),
                                    T.nachlauf_r42.nachlauf(raum, T.letzte_seite_zeilen(zeilen))[1] + T.seite2_beleg(raum, mid, text)))
        tab.append((raum, mid, n))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } s3b_text_t;')
    out.append('static const s3b_text_t k_s3b_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define S3B_TEXTE_N ((int)(sizeof k_s3b_texte / sizeof k_s3b_texte[0]))')
    if fehler:
        sys.exit(chr(10).join(fehler))
    text = chr(10).join(out) + chr(10)
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline=chr(10)).write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
