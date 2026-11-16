#!/usr/bin/env python3
"""Runde 42 Story, Stufe STAGE6 (Zug 6030 Leon / 6031 Elza): Port-Nachrichten -> engine/src/gen/story6_r42_texte.inc

    python re15_port/tools/r42_story/texte_stage6.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_stage6.py --check   (prueft nur)

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md,
Abschnitt 3.2 L21, 4.2 E6/E9). Ids und Sprecher = VERTRAG analysis/befunde_runde42/VERTRAG.md Abschnitt 2.3 (Zensus
zensus_r42.py, Werte in include/re15_story_r42.h):
  ROOM6030 msg 21, 2, 3, 22, 4, 5 Neutext (NEUAUF, L21; Sprecherwechsel msg 2 Sherry -> Leon, msg 22 John -> Sherry).
  ROOM6031 msg 1, 2 Neutext (NEUAUF, E6), 12, 13 neu (E6), 14..17 neu (E9).
Diese Ids ersetzen die Runde-36/41-Fassungen aus tools/r36_p/texte_bauen.py (dort 6030 3/5/21/22 und 6031 1 entfernt).

Kodierung = texte_stage3a.py (dieselben Funktionen, importiert): Untertitel `04 00 05 <Farbe> <Sprecher>: 05 00 00 <Text> 04 01 01 63`.
Der Schwanz `04 01 01 63` (Nachlauf des Untertitels) ist der der Original-Zeilen DESSELBEN Raums: ROOM6030 msg 0 @0x13F5,
msg 3 @0x1498, msg 4 @0x14C1; ROOM6031 msg 0 @0x1161, msg 1 @0x11AF, msg 2 @0x11CA (Datei-Offsets, das Werkzeug prueft sie).
Farben aus den ausgelieferten Zeilen: Leon 01 (ROOM6030 msg 0 @0x13BE `05 01`), Elza 01 (ROOM6031 msg 0 @0x1148 `05 01`),
Sherry 06 (ROOM6030 msg 2 @0x1416 / ROOM6031 msg 1 @0x1165 `05 06`), John 07 (ROOM6030 msg 3 @0x144E `05 07`; ROOM6031 hat
keine John-Zeile).
Laenge: hoechstens 192 Byte (PC MSG_RAW_LEN, msg_common.c:37); ueber 128 Byte (PSX MSG_RAW_LEN, msg_common.c:33) wird
gemeldet (PSX schneidet ab). Hoechstens 3 Zeilen je Seite (28 Original-Dialoge haben drei Zeilen, z. B. ROOM10D0 msg 17;
texte_bauen.py), Zeilenbreite <= 271 px.
Zwei Seiten ('|' im Text, nur 6030 msg 4: der Nutzersatz braucht vier Zeilen): Form des einzigen zweiseitigen Original-Untertitels
ROOM3060 msg 7 @0x120C, Seitenwechsel @0x1244 `04 01 02 3c 04 00` = Seite zu Ende, Seitenwartezeit N Bilder OHNE Taste
(msg_common.c Typewriter: `02 N`, N > 0 -> message_fsm 2 mit Timer), neue Seite ohne Sprecherkopf. N = der Nachlauf der
Zeilen desselben Raums (ROOM6030 `01 63` @0x13F5, 0x63 = 99 Bilder) statt 0x3C aus 3060.
"""
import os
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import texte_stage3a as T  # noqa: E402

ZIEL = os.path.join(T.PORT, 'engine', 'src', 'gen', 'story6_r42_texte.inc')
MAX_LEN = 192
PSX_LEN = 128
MAX_ZEILEN = 3
SEITE = [0x04, 0x01, 0x02, 0x63, 0x04, 0x00]   # Form ROOM3060 @0x1244 `04 01 02 3c 04 00`, N = 0x63 (ROOM6030 @0x13F5 `01 63`)
SCHWANZ = bytes([0x04, 0x01, 0x01, 0x63])
# (Raum, Id) der Original-Zeilen, deren Schwanz belegt wird (Datei-Offset des Schwanzes oben im Kopf)
SCHWANZ_BELEG = ((0x6030, 0, 0x13F5), (0x6030, 3, 0x1498), (0x6030, 4, 0x14C1),
                 (0x6031, 0, 0x1161), (0x6031, 1, 0x11AF), (0x6031, 2, 0x11CA))

LEON, ELZA, SHERRY, JOHN = (0x01, 'Leon'), (0x01, 'Elza'), (0x06, 'Sherry'), (0x07, 'John')

# (Raum, Id, (Farbe, Sprecher), Text) - Spielreihenfolge
UNTERTITEL = (
    # L21 (6030 Wagen, Leon): Folge 21, 2, 3, 22, 4, 5 im Wagen-Programm (tools/r41_story80/szenen_bauen.py)
    (0x6030, 21, JOHN,   "Name's John. This is Sherry. We came down here with a girl named Elza."),
    (0x6030, 2,  LEON,   "Where is she now?"),
    (0x6030, 3,  JOHN,   "We got split up in the fire. One second she was right behind us..."),
    (0x6030, 22, SHERRY, "Elza's still out there...!"),
    (0x6030, 4,  LEON,   "If she's as tough as you say, she'll find another way out.|"
                         "But we can't wait. When that reactor goes, nobody gets out."),
    (0x6030, 5,  JOHN,   "Then I'm staying with the kid. And I'll keep an eye on your friend here."),
    # E6 (6031 Wagen, Elza): msg 0 bleibt (story_r36), dann 1, 2, 12, 13
    (0x6031, 1,  SHERRY, "A policeman brought Marvin down here. Then he went back into the fire."),
    (0x6031, 2,  ELZA,   "Back? Why?"),
    (0x6031, 12, SHERRY, "He said he had to find someone. He told us not to wait for him."),
    (0x6031, 13, ELZA,   "...Then we won't. If that core goes, no one gets out. Let's get this train moving."),
    # E9 (6031 Fuehrerstand, Elza): Zugstart
    (0x6031, 14, SHERRY, "It's moving! Elza, we did it!"),
    (0x6031, 15, JOHN,   "What the hell was that? It came from the back car!"),
    (0x6031, 16, SHERRY, "It's that monster... it followed us!"),
    (0x6031, 17, ELZA,   "John, stay with Sherry and Marvin. Don't open that door for anyone but me."),
)


def rdt_msg(raum, mid):
    d = open(os.path.join(T.PORT, 'shared_assets', 'PSX', 'STAGE%X' % (raum >> 12), 'ROOM%04X.RDT' % raum), 'rb').read()
    ms = struct.unpack_from('<I', d, 0x3C)[0]
    n = struct.unpack_from('<H', d, ms)[0] // 2
    o = [struct.unpack_from('<H', d, ms + 2 * i)[0] for i in range(n)]
    a = ms + o[mid]
    b = ms + o[mid + 1] if mid + 1 < n else len(d)
    return d, a, b


def untertitel_seiten(farbe, name, text):
    """Untertitel mit 1..n Seiten ('|'); Seite 1 mit Sprecherkopf (texte_stage3a.untertitel), weitere ohne Kopf."""
    seiten = text.split('|')
    b, z1, bw = T.untertitel(farbe, name, seiten[0])
    b = b[:-4]                                   # Schwanz erst am Ende
    zeilen = [z1]
    for s in seiten[1:]:
        z = T.umbruch(s, 0, T.MAX_PX)
        b += SEITE
        for i, zz in enumerate(z):
            if i:
                b.append(0x08)
            b += T.kod(zz)
        bw += [T.px(zz) for zz in z]
        zeilen.append(z)
    return b + list(SCHWANZ), zeilen, bw


def main():
    fehler = []
    for raum, mid, off in SCHWANZ_BELEG:
        d, a, b = rdt_msg(raum, mid)
        if b - 4 != off or d[b - 4:b] != SCHWANZ:
            fehler.append('ROOM%04X msg %d: Schwanz @0x%X ist %s (erwartet `04 01 01 63` @0x%X)' % (raum, mid, b - 4, d[b - 4:b].hex(' '), off))
    out = ['/* Generiert von re15_port/tools/r42_story/texte_stage6.py - nicht von Hand aendern. PORT-STORY (Konzept 3.2 L21,',
           ' * 4.2 E6/E9, VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3). Schwanz 04 01 01 63 = Original-Zeilen desselben Raums',
           ' * (ROOM6030 msg 0 @0x13F5, ROOM6031 msg 0 @0x1161). */']
    tab = []
    for raum, mid, (farbe, name), text in UNTERTITEL:
        b, zeilen, bw = untertitel_seiten(farbe, name, text)
        if len(b) > PSX_LEN:
            print('Hinweis: ROOM%04X msg %d hat %d Byte > %d (PSX MSG_RAW_LEN schneidet ab; PC 192)' % (raum, mid, len(b), PSX_LEN))
        for i, w in enumerate(bw):
            if w > T.MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, T.MAX_PX))
        if len(b) > MAX_LEN:
            fehler.append('ROOM%04X msg %d: %d Byte > %d (PSX MSG_RAW_LEN)' % (raum, mid, len(b), MAX_LEN))
        if max(len(z) for z in zeilen) > MAX_ZEILEN:
            fehler.append('ROOM%04X msg %d: %d Zeilen > %d' % (raum, mid, len(zeilen), MAX_ZEILEN))
        if bytes(b[-4:]) != SCHWANZ:
            fehler.append('ROOM%04X msg %d: Schwanz' % (raum, mid))
        n = 'k_s6_%04x_msg%d' % (raum, mid)
        out += T.carr(n, b, 'ROOM%04X %d %s: %s  (%d B, Zeilen %s px)' % (raum, mid, name, ' // '.join(' / '.join(z) for z in zeilen), len(b),
                                                                      '/'.join(map(str, bw))))
        tab.append((raum, mid, n))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } s6_text_t;')
    out.append('static const s6_text_t k_s6_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define S6_TEXTE_N ((int)(sizeof k_s6_texte / sizeof k_s6_texte[0]))')
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
