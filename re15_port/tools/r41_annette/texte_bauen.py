#!/usr/bin/env python3
"""Runde 41 Spur AN: Port-Nachrichten der Annette-Szenen -> engine/src/gen/annette_r41_texte.inc

    python re15_port/tools/r41_annette/texte_bauen.py           (schreibt die .inc)
    python re15_port/tools/r41_annette/texte_bauen.py --check   (prueft nur)

Kodierung wie jede Port-Zeile (Vorbild tools/r39_funk10f1/szene_bauen.py, Glyphen = msg_common.c re15_msg_glyph):
    04 00            Untertitel-Kopf
    05 cc <Name:> 05 00   Sprecherfarbe (01 Leon/Elza, 02 Annette/Ada; Census P_story.md "Farben")
    <Text>           A-Z 0x1D.., a-z 0x3D.., ' ' 0x00, '!' 0x1A, '?' 0x1B, ':' 0x16, ',' 0x18, "'" 0x3A, '-' 0x3B, '.' 0x57
    08               Zeilenumbruch (FUN_80028868; wie tools/r36_p/texte_bauen.py)
    04 01 01 nn      Ende mit nn Bildern Halt. Der Port zeigt eine Zeile genau diesen Halt lang (msg_common.c
                     re15_msg_compute_duration: nur 0x01 N und 0x02 N zaehlen, FUN_80028134 Zustand 6).
                     Standard 0x63 = 99 Bilder (wie die Port-Zeilen der Runden 35-41). Nachbesserung 3 (Abnahme 1 M6:
                     "Annette: William." stand 3,3 s ueber den Schnitt, RE2-Stimme 1,17 s; "Leon: Annette!" stand noch
                     ueber dem Kampfstart): 5090 haelt so lange wie die RE2-Stimme bzw. bis zum naechsten Schritt:
                       43 "William."                 35 Bilder = Xa 0x30, 11 Kanalsektoren * 4032 / 37800 s = 1,173 s
                       40 "W-William! You're alive!" 118 Bilder = Xa 0x31, 37 Sektoren = 3,947 s
                       42 "Leon: Annette!"           60 Bilder = Sleep vor dem Kampfstart im Programm (annette_r41.c k_prog)
                     (Xa-Tabelle der Claire-STAGE6.BIN @0x4D4, Dossier N1.1; die Programm-Sleeps nach Message_on sind
                     dieselben Zahlen, die Zeile endet also mit dem naechsten Schritt.)
Zeilenbreite <= 271 px mit Sprecherkopf in Zeile 1 (include/font_width.h = DEBUG.BIN[0x4416+code], Grenze wie
tools/r36_p/texte_bauen.py MAX_PX).

Wortlaut und Herkunft (Dossier analysis/befunde_runde41/AN_annette.md, Nachbesserung 1):
  ROOM5090 (Szene im hinteren Wagen, RE2 Claire ROOM6141 sub02 nachgebaut)
    40 Annette: W-William! You're alive!   RE2 woertlich (Claire-Disc PL1/VOICE/VOICE16.XAS, Xa 0x31 = Kanal 8 Block 37
                                           Laenge 37, ROOM6141 sub02 @0x0111A `59 00 31 00`)
    41 (kein Text)                         Schrei nur als Ton (Nutzer F9 2026-11-02; RE2 Xa 0x32 @0x01150)
    42 Leon: Annette!                      PORT-WAHL (vom Nutzer freigegeben, F8)
    43 Annette: William.                   RE2 woertlich (Xa 0x30 = Kanal 10 Block 138 Laenge 11, @0x0107A `59 00 30 00`)
  ROOM6030 (Zugstart nach dem Tor; Ids 27-30 nach Absprache mit Spur ST80-T, die 21-26 belegt)
    27-30                                   PORT-WAHL, Vorschlag nach Nutzer-Idee und Story-Nachtrag (Sherry im Zug)
"""
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
ZIEL = os.path.join(PORT, 'engine', 'src', 'gen', 'annette_r41_texte.inc')
FW = os.path.join(PORT, 'include', 'font_width.h')
MAX_PX = 271

# (Raum, Id, Farbe, Sprecher, [Zeilen], Halt in Bildern)
ZEILEN = (
    (0x5090, 40, 0x02, 'Annette', ["W-William! You're alive!"], 118),
    (0x5090, 42, 0x01, 'Leon', ['Annette!'], 60),
    (0x5090, 43, 0x02, 'Annette', ['William.'], 35),
    (0x6030, 27, 0x02, 'Annette', ["There! The train's moving."], 0x63),
    (0x6030, 28, 0x02, 'Annette', ['What was that?', '...Sherry is back there!'], 0x63),
    (0x6030, 29, 0x01, 'Leon', ["I'll check it out."], 0x63),
    # Runde 42 namen Stufe 5: '|' = Seite 2 mit Namenskopf (hoechstens zwei Zeilen je Seite, tools/r42_namen/lesbar_s5.py PORT_SEITEN)
    (0x6030, 30, 0x02, 'Annette', ["No. She's my daughter.", '|', 'Stay here and', 'keep an eye on the controls.'], 0x63),
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


def breiten():
    src = open(FW, encoding='utf-8', errors='replace').read()
    body = src.split('{', 1)[1].split('}', 1)[0]
    nums = [int(x, 0) for x in re.findall(r'\b(0x[0-9a-fA-F]+|\d+)\b', body)]
    assert len(nums) == 256, len(nums)
    return nums


WID = breiten()


def px(text):
    return sum(WID[glyphe(c)] for c in text)


def kodiere(farbe, name, zeilen, halt=0x63, wechsel=None):
    """wechsel = N des Seitenwechsels `04 01 02 N 04 00` vor einem '|' in zeilen (Seite 2 mit demselben Namenskopf, Form Strang
    namen Stufe 1/4, ROOM3060 msg 7 @0x1244 `04 01 02 3c 04 00`)."""
    assert 0 < halt <= 0xFF
    kopf = [0x04, 0x00, 0x05, farbe] + [glyphe(c) for c in name + ':'] + [0x05, 0x00, 0x00]
    b = list(kopf)
    neu = True
    for z in zeilen:
        if z == '|':
            assert wechsel and 0 < wechsel <= 0xFF
            b += [0x04, 0x01, 0x02, wechsel] + kopf
            neu = True
            continue
        if not neu:
            b.append(0x08)
        b += [glyphe(c) for c in z]
        neu = False
    return b + [0x04, 0x01, 0x01, halt]


def seiten_von(zeilen):
    s, akt = [], []
    for z in zeilen:
        if z == '|':
            s.append(akt)
            akt = []
        else:
            akt.append(z)
    return s + [akt]


def main():
    fehler = []
    out = ['/* Generiert von re15_port/tools/r41_annette/texte_bauen.py - nicht von Hand aendern. */']
    for raum, mid, farbe, name, zeilen, halt in ZEILEN:
        bw, wechsel, zus = [], None, ''
        for seite in seiten_von(zeilen):
            bw += [px(name + ': ') + px(seite[0])] + [px(z) for z in seite[1:]]
            if len(seite) > 2:   # Runde 42 namen Stufe 5: hoechstens zwei Zeilen je Seite (tools/r42_namen/lesbar_s5.py)
                fehler.append('ROOM%04X msg %d: %d Zeilen auf einer Seite > 2' % (raum, mid, len(seite)))
            if any(len(z.split(' ')) < 2 for z in seite[1:]):
                fehler.append('ROOM%04X msg %d: Zeile mit nur einem Wort' % (raum, mid))
        if '|' in zeilen:
            sys.path.insert(0, os.path.join(HIER, '..', 'r42_namen'))
            import lesbar_s5 as L5
            wechsel, beleg = L5.PORT_SEITEN[(raum, mid)]
            zus = '; Seite 2 nach `02 %d`: %s' % (wechsel, beleg)
        for i, w in enumerate(bw):
            if w > MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, MAX_PX))
        b = kodiere(farbe, name, zeilen, halt, wechsel)
        out.append('/* ROOM%04X %d %s: %s  (Zeilen %s px, Halt %d Bilder%s) */' % (raum, mid, name,
                                                                                 ' / '.join(zeilen).replace(' / | / ', ' // '),
                                                                                 '/'.join(str(w) for w in bw), halt, zus))
        out.append('static const uint8_t k_an_%04x_msg%d[%d] = {' % (raum, mid, len(b)))
        for i in range(0, len(b), 16):
            out.append('    ' + ' '.join('0x%02x,' % x for x in b[i:i + 16]))
        out.append('};')
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
