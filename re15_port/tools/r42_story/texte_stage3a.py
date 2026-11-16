#!/usr/bin/env python3
"""Runde 42 Story, Stufe STAGE3a (Marvin): Port-Nachrichten -> engine/src/gen/story3a_r42_texte.inc

    python re15_port/tools/r42_story/texte_stage3a.py           (schreibt die .inc)
    python re15_port/tools/r42_story/texte_stage3a.py --check   (prueft nur)

PORT-STORY nach dem freigegebenen Konzept (C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/KONZEPT.md,
Abschnitt 3.2 L5, L6, L7; Zeitleiste L-05..L-10; Entscheide F-04, F-05, F-06). Ids und Sprecher = VERTRAG
analysis/befunde_runde42/VERTRAG.md Abschnitt 2.3 (Zensus zensus_r42.py, Werte in include/re15_story_r42.h):
ROOM3020 msg 0..2 Neutext (Glaettung L5), 7..9 neu; ROOM3060 msg 11..27 neu (RDT 0..10); ROOM30E0 msg 3 Neutext (L7).
Nur Leons Fassung (ROOM3020/3060); Elzas ROOM3021/3061 bleiben unveraendert.

Kodierung wie texte_stage2.py (Vorbild tools/r41_annette/texte_bauen.py, Glyphen = msg_common.c re15_msg_glyph):
  Untertitel  04 00 05 <Farbe> <Name:> 05 00 00 <Text> [08 <Text>] 04 01 01 N
              N = Nachlauf der Original-Zeilen desselben Raums (nachlauf_r42.py, Pruefhinweis H4; bis 2026-11-16 fest 0x63 ohne Beleg):
              ROOM3060 0x3C (8 Originale `04 01 01 3c` ab @0x1131), ROOM3020 einzeilig 0x59 (@0x0E45/@0x0EB9) / zweizeilig 0x63
              (@0x0E92), ROOM30E0 0x63 (14 Originale, msg 0 @0x1107). Je Zeile im Kommentar der .inc.
              Farben aus den ausgelieferten Zeilen derselben Raeume (rdt_msgdump.py ROOM3020): Leon 01 (msg 0 @0x0E1A `05 01`),
              Marvin 07 (msg 1 @0x0E49 `05 07`).
  Systemtext  04 02 <Text> [08 <Text>] 01 00   (Form ROOM3020 msg 3 @0x0EBD "It's locked from the other side.", ctrl 04:02 .. 01:END)
msg 20 ("You've received the <Name>.") wird zur Laufzeit je Fund gebaut (story3a_r42.c): hier stehen Kopf und Fuss als Glyphen.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import nachlauf_r42  # noqa: E402  (H4: Nachlauf aus den Original-Zeilen desselben Raums)

HIER = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.normpath(os.path.join(HIER, '..', '..'))
ZIEL = os.path.join(PORT, 'engine', 'src', 'gen', 'story3a_r42_texte.inc')
FW = os.path.join(PORT, 'include', 'font_width.h')
PSX = os.path.join(PORT, 'shared_assets', 'PSX')
MAX_PX = 271
MAX_DESC_PX = 191

TAB = {' ': 0x00, '!': 0x1A, '?': 0x1B, ':': 0x16, ',': 0x18, "'": 0x3A, '-': 0x3B, '.': 0x57, '"': 0x19}
AUF = 0x61   # oeffnendes Anfuehrungszeichen (ROOM1011 msg 18 @0x1231 `61 .. 19`, tools/r38_dining/programme_bauen.py)

# (Raum, Id, Farbe, Sprecher, Text)  -  VERTRAG 2.3, Block B
UNTERTITEL = (
    (0x3020, 0, 0x01, 'Leon',   "Damn it! It won't budge."),
    (0x3020, 1, 0x07, 'Marvin', "There's an open window up there. We could get in that way."),
    (0x3020, 2, 0x07, 'Marvin', "Leon, give me a hand!"),
    (0x3020, 7, 0x07, 'Marvin', "Wait here. I'll find a way to open this from the inside."),
    (0x3020, 8, 0x01, 'Leon',   "Watch yourself, Marvin."),
    (0x3020, 9, 0x01, 'Leon',   "Gunshots...? Marvin!"),
    (0x3060, 11, 0x07, 'Marvin', "Damn it...!"),
    (0x3060, 12, 0x07, 'Marvin', "Leon... Over here."),
    (0x3060, 13, 0x01, 'Leon',   "Marvin! What happened?"),
    (0x3060, 14, 0x07, 'Marvin', "Dogs. One of them got my leg."),
    (0x3060, 15, 0x07, 'Marvin', "I couldn't get that door open for you. But I found this."),
    (0x3060, 17, 0x07, 'Marvin', "And this. A floor plan of the plant."),
    (0x3060, 19, 0x07, 'Marvin', "Found this, too. You'll get more use out of it than me."),
    (0x3060, 21, 0x01, 'Leon',   "A keycard... This should open the door in the loading area."),
    (0x3060, 22, 0x07, 'Marvin', "Go on. With this leg, I'd only slow you down."),
    (0x3060, 23, 0x01, 'Leon',   "I'm not leaving you out here."),
    (0x3060, 24, 0x07, 'Marvin', "I'll make it back to the lobby. I can lock myself in and keep an eye on the front gate."),
    (0x3060, 25, 0x07, 'Marvin', "If Ada turns up, I'll send her your way."),
    (0x3060, 26, 0x01, 'Leon',   "Hang in there. I'll come back for you."),
    (0x3060, 27, 0x07, 'Marvin', "You'd better, rookie."),
    # L7: 30E0 msg 3 Neutext (sub02 Message_on 3 @0x00D6C; RDT-Text @0x1163 "Leon: Y-yeah, Marvin is out in the hall. He can't get out
    # because of all the monsters outside..."), Farbe Leon 01 wie die RDT-Zeile (@0x1163 `05 01`). Stufe STAGE3a NB1 B5.
    (0x30E0, 3, 0x01, 'Leon',   "He's back in the lobby. Some dogs got his leg... he can barely walk."),
)
# (Raum, Id, Text) - Systemtexte (VERTRAG 2.3 Art SYS)
SYSTEM = (
    (0x3060, 16, "You've received the Green Keycard."),
    (0x3060, 18, "You've received the factory map."),
)
# msg 20: Kopf/Fuss fuer die Laufzeit ("You've received the " + Name aus der Namensbank + ".")
MSG20_KOPF = "You've received the "
MSG20_FUSS = "."


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
    out = ['/* Generiert von re15_port/tools/r42_story/texte_stage3a.py - nicht von Hand aendern. PORT-STORY (Konzept 3.2 L5-L7,',
           ' * VERTRAG analysis/befunde_runde42/VERTRAG.md 2.3). */']
    tab = []
    for raum, mid, farbe, name, text in UNTERTITEL:
        b, zeilen, bw = untertitel(farbe, name, text, raum)
        for i, w in enumerate(bw):
            if w > MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, MAX_PX))
        n = 'k_s3a_%04x_msg%d' % (raum, mid)
        out += carr(n, b, 'ROOM%04X %d %s: %s  (Zeilen %s px; %s)' % (raum, mid, name, ' / '.join(zeilen), '/'.join(map(str, bw)),
                                    nachlauf_r42.nachlauf(raum, len(zeilen))[1]))
        tab.append((raum, mid, n))
    for raum, mid, text in SYSTEM:
        b, zeilen, bw = systemtext(text)
        for i, w in enumerate(bw):
            if w > MAX_PX:
                fehler.append('ROOM%04X msg %d Zeile %d = %d px > %d' % (raum, mid, i + 1, w, MAX_PX))
        n = 'k_s3a_%04x_msg%d' % (raum, mid)
        out += carr(n, b, 'ROOM%04X %d System: %s  (Zeilen %s px)' % (raum, mid, ' / '.join(zeilen), '/'.join(map(str, bw))))
        tab.append((raum, mid, n))
    out += carr('k_s3a_msg20_kopf', [0x04, 0x02] + kod(MSG20_KOPF), 'msg 20 Kopf: 04 02 "%s" (%d px)' % (MSG20_KOPF, px(MSG20_KOPF)))
    out += carr('k_s3a_msg20_fuss', kod(MSG20_FUSS) + [0x01, 0x00], 'msg 20 Fuss: "%s" 01 00' % MSG20_FUSS)
    out.append('#define S3A_MSG20_KOPF_PX %d' % px(MSG20_KOPF))
    out.append('#define S3A_MAX_PX %d' % MAX_PX)
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } s3a_text_t;')
    out.append('static const s3a_text_t k_s3a_texte[] = {')
    for raum, mid, n in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, n, n))
    out.append('};')
    out.append('#define S3A_TEXTE_N ((int)(sizeof k_s3a_texte / sizeof k_s3a_texte[0]))')
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
