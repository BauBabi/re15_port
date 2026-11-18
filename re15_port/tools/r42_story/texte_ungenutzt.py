#!/usr/bin/env python3
"""Runde 42 Strang "rest", Stufe 3 "Ungenutzte Inhalte STAGE2-6": Port-Nachrichten -> engine/src/gen/ungenutzt_r42_texte.inc

    python re15_port/tools/r42_story/texte_ungenutzt.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_ungenutzt.py --check   (prueft nur)

Grundlage: scratchpad ungenutzt/VORSCHLAEGE.md U-01/U-02/U-11 mit PRUEFUNG.md (Vorrang). Regel des Auftrags: neue Saetze NUR im
Capcom- (40-%-Disc Version_40%) oder 80-%-Wortlaut; die englische Fassung ist eine PORT-WAHL (Uebersetzung), Japanisch gelesen mit
analysis/story_begegnungen/werkzeuge/jp.py.

  ROOM3091 msg 33  John   D309 #21 (Disc ROOM3091 Nachrichtenblock @0x1B4E):
                          「おい、そろそろ行こうぜ。 / こんな所にいたら、奴[ら] またゾロゾロ / [集]まってくる。」
                          PORT-WAHL Uebersetzung: "Hey, let's get going. If we stay here, they'll come swarming back."
  ROOM3091 msg 34  Elza   D309 #24 (Disc ROOM3091 @0x1BD6): 「ちょっと、大丈夫でしょうね？」
                          PORT-WAHL Uebersetzung: "Hey... you'll be all right, won't you?"
  ROOM3091 msg 35  System 80 % T25 (analysis/story80/KATALOG_80.md:215, Video A 0:40.84 = Foto A#051): 「電げんがある」, englische
                          Spalte des Katalogs "There's a power supply." (zweite Zeile im Foto unlesbar -> nicht erfunden).
  ROOM4020/4021 msg 2     KEIN neuer Satz: Bytes = Mod ROOM5030 msg 1 @0x1132 "You've taken the laboratory map." (der einzige
                          Kartenfund des Spiels, 5030 sub02 @0x10CC); Blatt 8 heisst im Kartenbild "Laboratory B1"
                          (menu_common.c ORDER: 8-11 Laboratory B1-B4). Das Werkzeug liest die Bytes aus der RDT.

Kodierung = texte_stage3a.py (importiert): Untertitel `04 00 05 <Farbe> <Name:> 05 00 00 <Text> 04 01 01 N`, N aus den Original-
Zeilen desselben Raums (nachlauf_r42.py: ROOM3091 hat eigene Untertitel). Farben aus den ausgelieferten 3091-Zeilen: Elza 01
(msg 21 @0x234B `05 01`), John 07 (msg 23 @0x238A `05 07`). Systemtext in der Form ROOM3091 msg 3 @0x1F36 `04 02 08 .. 01 00`
(Leerzeile vorn wie msg 2/3/4/8 desselben Raums).
Ids: ROOM3091 traegt msg 0..32 (Tabelle @0x1E74, 33 Eintraege), synchro/STAGE3/room3091 main00..main30 -> 33..35 frei;
ROOM4020/4021 msg 0..1 (@0xB80, 2 Eintraege) -> 2 frei.
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import texte_stage3a as T  # noqa: E402
import nachlauf_r42  # noqa: E402

ZIEL = os.path.join(T.PORT, 'engine', 'src', 'gen', 'ungenutzt_r42_texte.inc')
MAX_ZEILEN = 3
ELZA, JOHN = (0x01, 'Elza'), (0x07, 'John')

UNTERTITEL = (
    (0x3091, 33, JOHN, "Hey, let's get going. If we stay here, they'll come swarming back.",
     'D309 #21 Disc ROOM3091 @0x1B4E'),
    (0x3091, 34, ELZA, "Hey... you'll be all right, won't you?", 'D309 #24 Disc ROOM3091 @0x1BD6'),
)
SYSTEM = (
    (0x3091, 35, "There's a power supply.", '80 % T25 Foto A#051 (KATALOG_80.md:215)'),
)
# (Raum, Id, Quellraum, Quell-Id, erwarteter Text) - Bytes unveraendert aus der RDT
KOPIE = (
    (0x4020, 2, 0x5030, 1, "You've taken the laboratory map."),
    (0x4021, 2, 0x5030, 1, "You've taken the laboratory map."),
)
# Pruefung der Formbelege (Datei-Offset, erwartete Bytes)
BELEGE = (
    (0x3091, 0x234B, bytes([0x04, 0x00, 0x05, 0x01])),          # msg 21 Elza Farbe 01
    (0x3091, 0x238A, bytes([0x04, 0x00, 0x05, 0x07])),          # msg 23 John Farbe 07
    (0x3091, 0x1F36, bytes([0x04, 0x02, 0x08])),                # msg 3 Systemtext mit Leerzeile vorn
    (0x5030, 0x1132, bytes([0x04, 0x02, 0x35, 0x4B, 0x51])),    # msg 1 "You..."
)


def rdt(raum):
    return open(os.path.join(T.PORT, 'shared_assets', 'PSX', 'STAGE%X' % (raum >> 12), 'ROOM%04X.RDT' % raum), 'rb').read()


def rdt_msg(raum, mid):
    d = rdt(raum)
    ms = struct.unpack_from('<I', d, 0x3C)[0]
    n = struct.unpack_from('<H', d, ms)[0] // 2
    o = [struct.unpack_from('<H', d, ms + 2 * i)[0] for i in range(n)]
    a = ms + o[mid]
    e = a
    while d[e] != 0x01:
        e += 1 if d[e] not in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B) else 2
    return d[a:e + 2], a, n


def main():
    fehler = []
    for raum, off, soll in BELEGE:
        if rdt(raum)[off:off + len(soll)] != soll:
            fehler.append('Beleg ROOM%04X @0x%04X ist %s, erwartet %s' % (raum, off, rdt(raum)[off:off + len(soll)].hex(' '),
                                                                        soll.hex(' ')))
    out = ['/* Generiert von re15_port/tools/r42_story/texte_ungenutzt.py - nicht von Hand aendern. Runde 42 rest Stufe 3 (U-01, U-02,',
           ' * U-11): Capcom-/80-%-Wortlaut, englische Fassung PORT-WAHL; 4020/4021 msg 2 = Bytes ROOM5030 msg 1. */']
    tab = []
    for raum, mid, (farbe, name), text, beleg in UNTERTITEL:
        b, zeilen, bw = T.untertitel(farbe, name, text, raum)
        if len(zeilen) > MAX_ZEILEN:
            fehler.append('ROOM%04X msg %d: %d Zeilen' % (raum, mid, len(zeilen)))
        for i, w in enumerate(bw):
            if w > T.MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, T.MAX_PX))
        n = 'k_u42_%04x_msg%d' % (raum, mid)
        out += T.carr(n, b, 'ROOM%04X %d %s (%s): %s  (Zeilen %s px; %s)' % (raum, mid, name, beleg, ' / '.join(zeilen),
                                                                            '/'.join(map(str, bw)),
                                                                            nachlauf_r42.nachlauf(raum, len(zeilen))[1]))
        tab.append((raum, mid, n))
    for raum, mid, text, beleg in SYSTEM:
        zeilen = T.umbruch(text, 0, T.MAX_PX)
        b = [0x04, 0x02, 0x08]                     # Form ROOM3091 msg 3 @0x1F36: Leerzeile vorn
        for i, z in enumerate(zeilen):
            if i:
                b.append(0x08)
            b += T.kod(z)
        b += [0x01, 0x00]
        bw = [T.px(z) for z in zeilen]
        if len(zeilen) > 2:
            fehler.append('ROOM%04X msg %d: %d Zeilen + Leerzeile' % (raum, mid, len(zeilen)))
        n = 'k_u42_%04x_msg%d' % (raum, mid)
        out += T.carr(n, b, 'ROOM%04X %d System (%s): [leer] / %s  (Zeilen %s px; Form ROOM3091 msg 3 @0x1F36)'
                      % (raum, mid, beleg, ' / '.join(zeilen), '/'.join(map(str, bw))))
        tab.append((raum, mid, n))
    for raum, mid, qraum, qid, soll in KOPIE:
        b, a, nq = rdt_msg(qraum, qid)
        if bytes(b[2:-2]) != bytes(T.kod(soll)):
            fehler.append('ROOM%04X msg %d @0x%04X ist nicht "%s"' % (qraum, qid, a, soll))
        _, _, nziel = rdt_msg(raum, 0)
        if mid != nziel:
            fehler.append('ROOM%04X: Ziel-Id %d, RDT hat %d Nachrichten' % (raum, mid, nziel))
        n = 'k_u42_%04x_msg%d' % (raum, mid)
        out += T.carr(n, list(b), 'ROOM%04X %d = ROOM%04X msg %d @0x%04X Byte fuer Byte: %s' % (raum, mid, qraum, qid, a, soll))
        tab.append((raum, mid, n))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } u42_text_t;')
    out.append('static const u42_text_t k_u42_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define U42_TEXTE_N ((int)(sizeof k_u42_texte / sizeof k_u42_texte[0]))')
    if fehler:
        sys.exit('\n'.join(fehler))
    text = '\n'.join(out) + '\n'
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    open(ZIEL, 'w', newline='\n', encoding='utf-8').write(text)
    print('geschrieben', ZIEL)


if __name__ == '__main__':
    main()
