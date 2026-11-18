#!/usr/bin/env python3
"""Runde 42 Strang finale (Plan D5, K-11): Untertitel der Schlusszeile -> engine/src/gen/finale_r42_texte.inc

    python re15_port/tools/r42_finale/texte_finale.py           (schreibt die .inc)
    python re15_port/tools/r42_finale/texte_finale.py --check   (prueft nur)

NUTZERWUNSCH 17.11.2026 (woertlich): "Beim erschießen des endgegners mit dem raketenwerfer möchte ich das du das game over voiceover
von Leon übernimmst und das voiceover von Claire." Konzept K-11 (KONZEPT.md Zeile 32): Untertitel wie bei den uebrigen Stimmen, die
Option VOICE OFF gilt auch hier. Stimme: synchro/STAGE5/room5090/main02.wav = Kopie von voice/leon/VOICE07.XAS[1]_02.wav (RE2
VOICE07.XAS Kanal 1 Block 107 Laenge 12, Dossier B 1.1); RE2 selbst zeigt im Englischen keinen Text (ROOM7020 msg 7/9 `fa 00 fa 02
fe 49`, Dossier B 5).

Id 2 = erste freie Id hinter den RDT-Meldungen 0..1 von ROOM5090 (VERTRAG 2.1, Zensus tools/r42_story/zensus_r42.py N1-N4).
Form der Zeile = tools/r42_story/texte_stage2.py untertitel() (Kopf 04 00, Farbe 01 Leon, Ende 04 01 01 N). Halt N = 33 Bilder wie
die Annette-Zeilen desselben Raums (tools/r41_annette/texte_bauen.py: Halt = Programm-Sleep nach Message_on): das Programm
(finale_r42.c) wartet nach der Zeile das RE2-Lesefenster ab, 12*16 - 0x1E = 162 Sektoren (STAGE7.BIN @0x150 `01 00 6b 00 0c 00`,
Ende @0x80012ae8-0x80012b10) / 150 Sektoren je s = 1,08 s = 32,4 Spielbilder, Wsleeping prueft je Bild (@0x80053ac0-0x80053ad0) ->
33 Bilder (RE15_FI_ZEILE_BILDER).
Elza (ROOM5091 Id 2): "You lose, big guy!" = voice/Clair/VOICE17.XAS[7]_1.wav (Claire-PAL VOICE17.XAS Kanal 6 Block 78 Laenge 24 =
dieselbe Aufnahme, Dossier B 1.2), Halt 71 = 354 Sektoren / 150 * 30 = 70,8 (Claire-STAGE7.BIN @0x150 `06 00 4e 00 18 00`).
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
import texte_stage2 as t2  # noqa: E402  (Glyphen, Breiten, Kopfform)

ZIEL = os.path.join(t2.PORT, 'engine', 'src', 'gen', 'finale_r42_texte.inc')
ZEILEN = ((0x5090, 2, 0x01, 'Leon', 'Game over.', 33),
          (0x5091, 2, 0x01, 'Elza', 'You lose, big guy!', 71))


def kodiere(farbe, name, text, halt):
    kopf = name + ': '
    zeilen = t2.umbruch(text, t2.px(kopf), t2.MAX_PX)
    b = [0x04, 0x00, 0x05, farbe] + t2.kod(name + ':') + [0x05, 0x00, 0x00]
    for i, z in enumerate(zeilen):
        if i:
            b.append(0x08)
        b += t2.kod(z)
    bw = [t2.px(kopf) + t2.px(zeilen[0])] + [t2.px(z) for z in zeilen[1:]]
    return b + [0x04, 0x01, 0x01, halt], zeilen, bw


def main():
    out = ['/* Generiert von re15_port/tools/r42_finale/texte_finale.py - nicht von Hand aendern. NUTZERWUNSCH K-11 (17.11.2026). */']
    for raum, mid, farbe, name, text, halt in ZEILEN:
        b, zeilen, bw = kodiere(farbe, name, text, halt)
        for w in bw:
            if w > t2.MAX_PX:
                sys.exit('ROOM%04X %d: Zeile %d px > %d' % (raum, mid, w, t2.MAX_PX))
        out += t2.carr('k_fi_%04x_msg%02d' % (raum, mid), b, 'ROOM%04X %d %s: %s  (Zeilen %s px, Halt %d Bilder)'
                       % (raum, mid, name, ' / '.join(zeilen), '/'.join(map(str, bw)), halt))
    text = chr(10).join(out) + chr(10)
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline=chr(10)).write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
