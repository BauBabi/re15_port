#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 5 Runde 2: vier Port-Texte mit drei Zeilen auf einer Seite, die der Zensus der Runde 1 nicht fand
(Pruefbericht s5_r1 R2 - der Riegel quellen_zeilen ueberging Tabellen mit einem Kommentar hinter `{`):

    python re15_port/tools/r42_namen/porttexte_s5r2.py           (schreibt die Tabellen in die beiden .c)
    python re15_port/tools/r42_namen/porttexte_s5r2.py --check   (prueft nur)

  engine/src/cut_10f0.c        10F0 msg 7  "Woman: Did you really think there was / only one staff card for the / Communication Room?"
                               10F0 msg 16 "Leon: Anyway... looks like we can't / contact anyone with this thing / anymore."
  engine/src/irons_tod_1150.c  1150 msg 23 "Leon: Sir, the communication system / can't be fixed! We're going to use the / patrol ..."
                               1150 msg 27 "Irons: Please... one last favor... Look / after yourself and the others who / survived."
Regeln wie tools/r42_namen/lesbar_s5.py (hoechstens zwei Zeilen je Seite <= 271 px, keine Ein-Wort-Zeile, Name auf jeder Seite,
Wortlaut Glyphe fuer Glyphe gleich - das Werkzeug prueft es gegen die bisherigen Bytes). Kopf und Schwanz `04 01 01 63` bleiben.
Aufnahmen: 10F0 = synchro/STAGE1/room10F0/mainNN.wav (audio_pc.c-Regel), 1150 msg 23/27 = main16/main20 (engine/src/stimmen_r39.c).
Pausen: namen/s5/pausen5.py (>= 120 ms unter max(-40 dB, Grundrauschen + 8 dB)), Woerter: Whisper medium.en (namen/s5r2/asr_r2.txt).
"""
import os
import re
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
sys.path.insert(0, HIER)
import texte_stage3a as T  # noqa: E402
import lesbar_s4 as LS  # noqa: E402
import lesbar_s5 as L5  # noqa: E402

SRC = os.path.join(T.PORT, 'engine', 'src')
SCHWANZ = [0x04, 0x01, 0x01, 0x63]   # bisheriger Schwanz aller vier Texte (99 Bilder, ROOM1090 msg 0 @0x275C)

# (Datei, C-Name, Sprecher, Farbe, Text mit '|' = Seitenwechsel, t in s bzw. None, Beleg)
TEXTE = (
    ('cut_10f0.c', 'k_msg07', 'Woman', 0x02,
     'Did you really think there was only one staff card | for the Communication Room?', 2.28,
     'synchro/STAGE1/room10F0/main07.wav: keine Pause (pausen5: Schwelle -26 dB, keine); Satzteilgrenze vor "for", Whisper '
     '"card"[2.00-2.28] "for"[2.28] (RMS 20 ms 2.30-2.38 bei -13..-26 dB)'),
    ('cut_10f0.c', 'k_msg16', 'Leon', 0x01,
     "Anyway... looks like we can't contact anyone with this thing anymore.", None,
     'passt in zwei Zeilen - nur neu umbrochen, eine Seite'),
    ('irons_tod_1150.c', 'k_msg23', 'Leon', 0x01,
     "Sir, the communication system can't be fixed! | We're going to use the patrol car to get out of here.", 3.52,
     'synchro/STAGE1/room1150/main16.wav (stimmen_r39.c msg 23 -> main16): Pause 3.09-3.52 nach "fixed." (Whisper "fixed."'
     '[2.62-3.50] "We\'re"[3.50]); die Pause 0.61-1.05 nach "Sir," gaebe Seite 1 nur ein Wort'),
    ('irons_tod_1150.c', 'k_msg27', 'Irons', 0x02,
     'Please... one last favor... | Look after yourself and the others who survived.', 2.76,
     'synchro/STAGE1/room1150/main20.wav (stimmen_r39.c msg 27 -> main20): Pausen 0.55-1.13 / 2.23-2.76 / 3.76-4.48; gewaehlt '
     '2.23-2.76 nach "favor..." (Satzende; Whisper "favour,"[1.76-2.78] "look"[2.78]) - 0.55-1.13 laesst Seite 2 drei Zeilen, '
     '3.76-4.48 teilt "Look after yourself // and the others"'),
)


def bauen(name, farbe, text, t):
    kopf = [0x04, 0x00, 0x05, farbe] + T.kod(name + ':') + [0x05, 0x00]
    n = LS.seitenwartezeit(t, 0) if t is not None else 1
    b, zz, bw = L5.text_seiten(text, T.kod, kopf, SCHWANZ, n)
    return b, zz, bw, (n if t is not None else None)


def glyphen(b):
    """Text-Glyphen ohne Kopf/Steuercodes/Leerzeichen/Umbruch (Wortlaut-Vergleich)."""
    out, i = [], 0
    while i < len(b):
        c = b[i]
        if c in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            if c == 0x05 and i + 1 < len(b) and b[i + 1] != 0x00:   # Namenskopf bis `05 00` ueberspringen
                j = b.index(0x05, i + 2)
                i = j + 2
                continue
            i += 2
            continue
        if c == 0x01:
            break
        if c not in (0x00, 0x08):
            out.append(c)
        i += 1
    return out


def main():
    fehler, neu = [], {}
    for datei, cname, name, farbe, text, t, beleg in TEXTE:
        pfad = os.path.join(SRC, datei)
        src = neu.get(pfad) or open(pfad, encoding='utf-8').read()
        m = re.search(r'static const uint8_t %s\[\] = \{[^\n]*\n(.*?)\n\};' % cname, src, re.S)
        if not m:
            fehler.append('%s %s nicht gefunden' % (datei, cname))
            continue
        alt = [int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})', m.group(1))]
        b, zz, bw, n = bauen(name, farbe, text, t)
        if glyphen(alt) != glyphen(b):
            fehler.append('%s %s: Wortlaut weicht ab' % (datei, cname))
        if alt[:4 + len(name) + 3] != b[:4 + len(name) + 3] or alt[-4:] != b[-4:]:
            fehler.append('%s %s: Kopf/Schwanz weichen ab' % (datei, cname))
        for si, (zs, ww) in enumerate(zip(zz, bw)):
            if len(zs) > LS.MAX_ZEILEN or max(ww) > T.MAX_PX:
                fehler.append('%s %s Seite %d: %d Zeilen %d px' % (datei, cname, si + 1, len(zs), max(ww)))
        seiten = ' // '.join(' / '.join(z) for z in zz)
        kopfz = '"%s: %s"' % (name, seiten)
        wie = ('Seite 2 nach `02 %d` = round(30 x %.2f s): %s' % (n, t, beleg)) if n is not None else beleg
        zeilen = ['static const uint8_t %s[] = {   /* %s */' % (cname, kopfz),
                  '    /* Runde 42 namen Stufe 5 R2 (tools/r42_namen/porttexte_s5r2.py): hoechstens zwei Zeilen je Seite, Name auf jeder',
                  '     * Seite, Wortlaut unveraendert; Zeilen %s px; %s */' % (' // '.join('/'.join(map(str, w)) for w in bw), wie)]
        for i in range(0, len(b), 16):
            zeilen.append('    ' + ' '.join('0x%02x,' % x for x in b[i:i + 16]))
        zeilen.append('};')
        src = src[:m.start()] + '\n'.join(zeilen) + src[m.end():]
        neu[pfad] = src
        print('%-17s %s %3d B  %s: %s' % (datei, cname, len(b), name, seiten))
    if fehler:
        sys.exit('\n'.join(fehler))
    gleich = all(open(p, encoding='utf-8').read() == s for p, s in neu.items())
    if '--check' in sys.argv:
        print('gleich' if gleich else 'VERALTET')
        sys.exit(0 if gleich else 1)
    for p, s in neu.items():
        crlf = b'\r\n' in open(p, 'rb').read()   # Zeilenenden der Datei behalten (autocrlf-Arbeitskopie)
        with open(p, 'w', encoding='utf-8', newline='\r\n' if crlf else '\n') as f:
            f.write(s)
    print('geschrieben' if not gleich else 'unveraendert')


if __name__ == '__main__':
    main()
