#!/usr/bin/env python3
"""Runde 42 Story, Stufe STAGE2: Port-Nachrichten -> engine/src/gen/story2_r42_texte.inc

    python re15_port/tools/r42_story/texte_stage2.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_stage2.py --check   (prueft nur)

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md,
Abschnitt 3.2 L1/L1b, 4.2 E1, 6b; Korrekturen K-01..K-03, K-07, K-08). Ids und Sprecher = VERTRAG
analysis/befunde_runde42/VERTRAG.md Abschnitt 2.3 (Zensus zensus_r42.py, Werte in include/re15_story_r42.h).

Kodierung (wie jede Port-Zeile, Vorbild tools/r41_annette/texte_bauen.py, Glyphen = msg_common.c re15_msg_glyph):
  Untertitel  04 00 05 <Farbe> <Name:> 05 00 00 <Text> [08 <Text>] 04 01 01 63
              Farben aus den ausgelieferten Zeilen derselben Stage (rdt_msgdump.py ROOM2001): Elza 01 (msg 2 @0x1E74
              `05 01`), John 07 (msg 0 @0x1E0A `05 07`), Sherry 06 (msg 8 @0x1FA8 `05 06`); Leon 01 / Marvin 07 = ROOM2000
              (msg 1 @0x1BFD "Marvin:" `05 07`, tools/r41_kanister2080/texte_bauen.py). Nachlauf N (`04 01 01 N`) aus den
              Original-Zeilen desselben Raums, sonst des Belegraums (nachlauf_r42.py, Pruefhinweis H4); im Kommentar je Zeile.
  Systemtext  04 02 <Text> [08 <Text>] 01 00   (Form ROOM2070 msg 0 @0x0D06 "You've opened the lock.", ctrl 04:02 .. 01:END)
  Beschreibung <Text> [08 <Text>] [02 00 <Text> ...] 01 00   (Form der Bank gen/inv_desc_bank.inc, z. B. Item 0x01
              "Manufactured by FN,/Belgium.{02 00}It uses ...": Zeilen mit 08, Seiten mit 02 00)
Zeilenbreite: Untertitel/Systemtext <= 271 px (Textkasten, tools/r36_p/texte_bauen.py MAX_PX), Beschreibung <= 191 px
(breiteste Zeile der ausgelieferten Beschreibungsbank, gemessen mit include/font_width.h).
"It's locked from the other side." ist der ausgelieferte Satz ROOM11A0 msg 2 @0x0142C (bytegleich kopiert, geprueft).
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import nachlauf_r42  # noqa: E402  (H4: Nachlauf aus den Original-Zeilen desselben Raums)

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
ZIEL = os.path.join(PORT, 'engine', 'src', 'gen', 'story2_r42_texte.inc')
FW = os.path.join(PORT, 'include', 'font_width.h')
PSX = os.path.join(PORT, 'shared_assets', 'PSX')
MAX_PX = 271
MAX_DESC_PX = 191

TAB = {' ': 0x00, '!': 0x1A, '?': 0x1B, ':': 0x16, ',': 0x18, "'": 0x3A, '-': 0x3B, '.': 0x57, '"': 0x19}
AUF = 0x61   # oeffnendes Anfuehrungszeichen (ROOM1011 msg 18 @0x1231 `61 .. 19`, tools/r38_dining/programme_bauen.py)

# (Raum, Id, Farbe, Sprecher, Text)  -  VERTRAG 2.3, Block B
UNTERTITEL = (
    (0x2080, 43, 0x01, 'Leon',   "Thanks, Marvin. I thought I was gonna end up as dinner."),
    (0x2080, 44, 0x01, 'Leon',   "What brought you here, anyway?"),
    (0x2080, 45, 0x07, 'Marvin', "Hard to miss. The whole place was shaking."),
    (0x2080, 46, 0x01, 'Leon',   "Yeah... right. Thanks again."),
    (0x2080, 47, 0x07, 'Marvin', "Go on ahead, Leon. I'll wait here and watch your back."),
    (0x2080, 48, 0x07, 'Marvin', "And I'll keep an eye out for that little girl."),
    (0x2081, 43, 0x01, 'Elza',   "Thanks, John. I thought I was gonna end up as dinner."),
    (0x2081, 44, 0x01, 'Elza',   "What brought you here, anyway?"),
    (0x2081, 45, 0x07, 'John',   "Hard to miss. The whole place was shaking."),
    (0x2081, 46, 0x01, 'Elza',   "Well... thanks again."),
    (0x2081, 47, 0x06, 'Sherry', "Elza, are you okay? I heard some really loud noises!"),
    (0x2081, 48, 0x01, 'Elza',   "I'm fine, Sherry. Thanks."),
    (0x2081, 49, 0x07, 'John',   "So you're Sherry. Stick close, kid."),
    (0x2081, 50, 0x07, 'John',   "Elza, go on ahead. I'll wait here and look after Sherry until you've cleared the way."),
    (0x2081, 51, 0x01, 'Elza',   "All right. Sherry, you stay with John."),
)
# (Raum, Id, Text) - Systemtexte (VERTRAG 2.3 Art SYS)
SYSTEM = (
    (0x2080, 52, "Next to the computer... there's a keyhole."),
    (0x2081, 52, "Next to the computer... there's a keyhole."),
    (0x2080, 53, "The gate is locked. It needs a key."),
    (0x2081, 53, "The gate is locked. It needs a key."),
    (0x2080, 54, "Leon used the key."),
    (0x2081, 54, "Elza used the key."),
    (0x2090, 47, "There's something else lying next to it..."),
    (0x2091, 47, "There's something else lying next to it..."),
    (0x11A0, 11, "Leon unlocked the door."),
    (0x11A1, 11, "Elza unlocked the door."),
    (0x2040, 6, "Leon unlocked the door."),
    (0x2041, 6, "Elza unlocked the door."),
)
# 2070/2071 msg 1 = ROOM11A0 msg 2 @0x0142C bytegleich
ANDERE_SEITE = ('STAGE1/ROOM11A0.RDT', 0x0142C, "It's locked from the other side.")
# Beschreibung Item 0x3F bei Leon (F-19, Konzept 6b): Seiten aus Zeilen
UHR_LEON = (["A pocket watch. Something's", "engraved on the back:"], ['"For Sherry. Love, Mom."'])


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
    nums = [int(x, 0) for x in re.findall(r'\b(0x[0-9a-fA-F]+|\d+)\b', src.split('{', 1)[1].split('}', 1)[0])]
    assert len(nums) == 256, len(nums)
    return nums


WID = breiten()


def kod(text):
    out = []
    offen = True
    for c in text:
        if c == '"':
            out.append(AUF if offen else 0x19)
            offen = not offen
        else:
            out.append(glyphe(c))
    return out


def px(text):
    return sum(WID[g] for g in kod(text))


def umbruch(text, erste_breite_vorweg, maxpx):
    """Gieriger Umbruch an Leerzeichen; Zeile 1 traegt den Sprecherkopf (erste_breite_vorweg px)."""
    worte = text.split(' ')
    zeilen, akt = [], ''
    for w in worte:
        probe = (akt + ' ' + w) if akt else w
        vorweg = erste_breite_vorweg if not zeilen else 0
        if akt and vorweg + px(probe) > maxpx:
            zeilen.append(akt)
            akt = w
        else:
            akt = probe
    zeilen.append(akt)
    return zeilen


def untertitel(farbe, name, text, raum):
    """Untertitel; der Nachlauf N im Schwanz `04 01 01 N` kommt aus den Original-Zeilen desselben Raums (nachlauf_r42.py, H4)."""
    kopf = name + ': '
    zeilen = umbruch(text, px(kopf), MAX_PX)
    b = [0x04, 0x00, 0x05, farbe] + kod(name + ':') + [0x05, 0x00, 0x00]
    for i, z in enumerate(zeilen):
        if i:
            b.append(0x08)
        b += kod(z)
    bw = [px(kopf) + px(zeilen[0])] + [px(z) for z in zeilen[1:]]
    n, _ = nachlauf_r42.nachlauf(raum, len(zeilen))
    return b + [0x04, 0x01, 0x01, n], zeilen, bw


def systemtext(text):
    zeilen = umbruch(text, 0, MAX_PX)
    b = [0x04, 0x02]
    for i, z in enumerate(zeilen):
        if i:
            b.append(0x08)
        b += kod(z)
    return b + [0x01, 0x00], zeilen, [px(z) for z in zeilen]


def beschreibung(seiten):
    b, bw = [], []
    for si, seite in enumerate(seiten):
        if si:
            b += [0x02, 0x00]
        for zi, z in enumerate(seite):
            if zi:
                b.append(0x08)
            b += kod(z)
            bw.append(px(z))
    return b + [0x01, 0x00], bw


def carr(name, b, kommentar):
    out = ['/* %s */' % kommentar, 'static const uint8_t %s[%d] = {' % (name, len(b))]
    for i in range(0, len(b), 16):
        out.append('    ' + ' '.join('0x%02x,' % x for x in b[i:i + 16]))
    out.append('};')
    return out


def main():
    fehler = []
    out = ['/* Generiert von re15_port/tools/r42_story/texte_stage2.py - nicht von Hand aendern. PORT-STORY (Konzept 3.2 L1/L1b,',
           ' * 4.2 E1, 6b; VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3). */']
    tab = []
    for raum, mid, farbe, name, text in UNTERTITEL:
        b, zeilen, bw = untertitel(farbe, name, text, raum)
        for i, w in enumerate(bw):
            if w > MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, MAX_PX))
        n = 'k_s2_%04x_msg%d' % (raum, mid)
        out += carr(n, b, 'ROOM%04X %d %s: %s  (Zeilen %s px; %s)' % (raum, mid, name, ' / '.join(zeilen), '/'.join(map(str, bw)),
                                    nachlauf_r42.nachlauf(raum, len(zeilen))[1]))
        tab.append((raum, mid, n))
    for raum, mid, text in SYSTEM:
        b, zeilen, bw = systemtext(text)
        for i, w in enumerate(bw):
            if w > MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, MAX_PX))
        n = 'k_s2_%04x_msg%d' % (raum, mid)
        out += carr(n, b, 'ROOM%04X %d System: %s  (Zeilen %s px)' % (raum, mid, ' / '.join(zeilen), '/'.join(map(str, bw))))
        tab.append((raum, mid, n))
    datei, off, text = ANDERE_SEITE
    d = open(os.path.join(PSX, datei), 'rb').read()
    soll, _, _ = systemtext(text)
    ist = list(d[off:off + len(soll)])
    if ist != soll:
        fehler.append('%s @0x%05X ist nicht "%s": %s' % (datei, off, text, bytes(ist).hex(' ')))
    for raum in (0x2070, 0x2071):
        n = 'k_s2_%04x_msg1' % raum
        out += carr(n, ist, 'ROOM%04X 1 System: %s = %s msg 2 @0x%05X bytegleich' % (raum, text, datei, off))
        tab.append((raum, 1, n))
    b, bw = beschreibung(UHR_LEON)
    for w in bw:
        if w > MAX_DESC_PX:
            fehler.append('Beschreibung 0x3F Zeile %d px > %d' % (w, MAX_DESC_PX))
    out += carr('k_s2_uhr_leon_desc', b, 'Item 0x3F Beschreibung Leon (F-19): %s  (Zeilen %s px)'
                % (' | '.join(' / '.join(s) for s in UHR_LEON), '/'.join(map(str, bw))))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } s2_text_t;')
    out.append('static const s2_text_t k_s2_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define S2_TEXTE_N ((int)(sizeof k_s2_texte / sizeof k_s2_texte[0]))')
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
