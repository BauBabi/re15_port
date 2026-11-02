#!/usr/bin/env python3
"""Runde 41 Spur AN: Nachrichten 40-42 der Annette-Szene ROOM5090 -> engine/src/gen/annette_r41_texte.inc

    python re15_port/tools/r41_annette/texte_bauen.py

Kodierung wie jede Port-Zeile (Vorbild tools/r39_funk10f1/szene_bauen.py, Glyphen = msg_common.c re15_msg_glyph):
    04 00            Untertitel-Kopf
    05 cc <Name:> 05 00   Sprecherfarbe (01 Leon/Elza, 02 Annette/Ada; Census P_story.md "Farben")
    <Text>           A-Z 0x1D.., a-z 0x3D.., ' ' 0x00, '!' 0x1A, '?' 0x1B, ':' 0x16, ',' 0x18, "'" 0x3A, '-' 0x3B, '.' 0x57
    04 01 01 63      Ende mit 99-Bilder-Halt (wie die Port-Zeilen der Runden 35-41)

Wortlaut (Dossier analysis/befunde_runde41/AN_annette.md Abschnitt 5):
    40 Annette: W-William! You're alive!   RE2 woertlich (Claire-Disc VOICE16 Kanal 9, Kanalsektor 74-111)
    41 Annette: Aaaah!                     PORT-WAHL fuer den Schrei (RE2 VOICE16 Kanal 12, Frage F6)
    42 Leon: Annette!                      PORT-WAHL nach Claires Ruf "What happened? Annette!" (RE2 VOICE16 Kanal 12)
"""
import os

HIER = os.path.dirname(os.path.abspath(__file__))
ZIEL = os.path.normpath(os.path.join(HIER, '..', '..', 'engine', 'src', 'gen', 'annette_r41_texte.inc'))

ZEILEN = (
    (40, 0x02, 'Annette', "W-William! You're alive!"),
    (41, 0x02, 'Annette', 'Aaaah!'),
    (42, 0x01, 'Leon', 'Annette!'),
)

TAB = {' ': 0x00, '!': 0x1A, '?': 0x1B, ':': 0x16, ',': 0x18, "'": 0x3A, '-': 0x3B, '.': 0x57}


def glyphe(ch):
    if 'A' <= ch <= 'Z':
        return 0x1D + ord(ch) - ord('A')
    if 'a' <= ch <= 'z':
        return 0x3D + ord(ch) - ord('a')
    if '0' <= ch <= '9':
        return 0x0C + ord(ch) - ord('0')
    return TAB[ch]


def kodiere(farbe, name, text):
    b = [0x04, 0x00, 0x05, farbe] + [glyphe(c) for c in name + ':'] + [0x05, 0x00, 0x00]
    b += [glyphe(c) for c in text]
    return b + [0x04, 0x01, 0x01, 0x63]


def main():
    out = ['/* Generiert von re15_port/tools/r41_annette/texte_bauen.py - nicht von Hand aendern. */']
    for mid, farbe, name, text in ZEILEN:
        b = kodiere(farbe, name, text)
        out.append('/* %d %s: %s */' % (mid, name, text))
        out.append('static const uint8_t k_an_msg%d[%d] = {' % (mid, len(b)))
        for i in range(0, len(b), 16):
            out.append('    ' + ' '.join('0x%02x,' % x for x in b[i:i + 16]))
        out.append('};')
    open(ZIEL, 'w', newline='\n').write('\n'.join(out) + '\n')
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
