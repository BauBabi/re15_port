#!/usr/bin/env python3
"""Runde 42 Strang "gegenstaende" (v0.8.31, Konzept K-10): Texte der neuen Gegenstands-Stellen
-> engine/src/gen/gegenstaende_r42_texte.inc

    python re15_port/tools/r42_gegenstaende/texte.py           (schreibt die .inc)
    python re15_port/tools/r42_gegenstaende/texte.py --check   (prueft nur)

PORT-TEXTE nach C:/workspace/git/reAi_v2_entscheidung_2026-11-06/11_story_v0831/GEGENSTAENDE.md (V-02, V-03, V-06; Nacharbeit
gelb G4: Python, Magnum, Name "Detonator", Fundtexte). Formen:
  Frage   = ROOM2010 msg 0 @0x0906 "Will you take the Crank? "  `04 02 <Text> 05 01 <Name> 05 00 1b 03 02 01 00`
  Danach  = ROOM2010 msg 1 @0x0928 "You've taken the Crank."    `04 02 <Text> 05 01 <Name> 05 00 57 01 00`
  System  = texte_stage2.systemtext (`04 02 <Zeilen mit 08> 01 00`, Zeilen <= 271 px)
  CHECK   = texte_stage2.beschreibung (Zeilen <= 191 px, <= 3 Zeilen je Seite, Seiten `02 00`, Ende `01 00`)
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
import texte_stage2 as t2  # noqa: E402

ZIEL = os.path.join(t2.PORT, 'engine', 'src', 'gen', 'gegenstaende_r42_texte.inc')
MAX_ZEILEN = 3

# CHECK-Beschreibungen (GEGENSTAENDE.md V-03 Tabelle "Beschreibung", V-06 Tabelle "Texte")
BESCHR = (
    (0x2F, 'k_g42_desc_mutter',  'A heavy hex nut. It looks like it would fit a valve stem.'),
    (0x1C, 'k_g42_desc_zuender', 'A detonator for hand grenades. Combine it with a capsule.'),
    (0x1D, 'k_g42_desc_huelsen', 'Empty shells for the M79. Fill them with a capsule to make grenade rounds.'),
    (0x1E, 'k_g42_desc_nitro',   'A capsule of explosive compound. Combine it with shells or a detonator.'),
    (0x1F, 'k_g42_desc_saeure',  'A capsule of strong acid. Combine it with shells or a detonator.'),
    (0x20, 'k_g42_desc_brand',   'A capsule of flammable compound. Combine it with shells or a detonator.'),
)
# Nacharbeit (gelb G4, GEGENSTAENDE.md V-02/V-06): Beschreibungen mit festen Seiten/Zeilen (Form der ausgelieferten
# Waffen-CHECKs, z. B. 0x07 Super Redhawk = DEBUG.BIN-Bank @0x800c50de Eintrag 7: "Manufactured by Sturm, / Ruger & Company
# Inc., USA." Seite `02 00` "A high caliber magnum / pistol.").
BESCHR_FEST = (
    # 0x14 Colt Python statt "Barry Burton's favorite!" (Eintrag 0x14, Blob-Offset 0x5a2)
    (0x14, 'k_g42_desc_python', (["Manufactured by", "Colt's Manufacturing", "Company, USA."], ["A .357 magnum revolver."])),
    # 0x17 Magnum Bullets: die ersten zwei Zeilen bytegleich zum Original (Eintrag 0x17, Blob-Offset 0x64d
    # `20 2b 30 11 0c 1d 57 21 57 ..` "DOT50A.E. rounds. Magnum / rounds for the Super / Redhawk."), dritte Zeile mit Python
    (0x17, 'k_g42_desc_magnum', (["DOT50A.E. rounds. Magnum", "rounds for the Super", "Redhawk or the Colt Python."],)),
)
# Name 0x1C: "Detonator" statt "Remote Detonator" (V-06; Disc-Name 信[管] = Zuender). Form der Port-Namen: Glyphen + 0x07
# (gen/keyitems_r41.inc re15_r41o_kurbel_name `1f 4e 3d 4a 47 07` "Crank").
NAMEN = ((0x1C, 'k_g42_name_zuender', 'Detonator'),)
# Fundtexte der Ausweichstellen (Nacharbeit gelb G2/G3/G7). PORT-WAHL (Wortlaut), englisch wie die uebrigen Port-Texte; Form
# Systemtext. Danach oeffnet das normale Aufnahme-Fenster (Form ROOM1051 sub03 @0x00DB6 Message_on + @0x00DBC Aot_on).
FUND = (
    ('k_g42_fund_1071', 'ROOM1071 Spind (Granatwerfer)', "One of the lockers isn't locked. There's a grenade launcher inside."),
    ('k_g42_leer_1071', 'ROOM1071 Spind nach der Aufnahme', "The locker is empty now."),
    ('k_g42_fund_3010', 'ROOM3010/3011 Kisten im Laster (Zuender)', "Something is lying next to the crates. It's a detonator."),
    ('k_g42_fund_5021', 'ROOM5021 Spalt unter den Kisten (Brand-Kapsel)', "Something is lying by the gap under the crates. It's a capsule."),
)
# 2030/2031 Pult ohne Mutter (V-03, an das Panel der Bruecke angepasst: Ventil 13 entfaellt seit Nachtrag 37)
# Runde 42 namen Stufe 5: '|' = Seite 2 nach der Taste (hoechstens zwei Zeilen je Seite, tools/r42_namen/lesbar_s5.py)
OHNE = 'A control panel for the bridge. Its valve handle spins freely. | The nut that held it in place is gone.'
FRAGE = ('Will you use the ', 'Nut', '?')
DANACH = ("You've fastened the valve handle / with the ", 'Nut', '.')   # ' / ' = Zeilenwechsel 08


def hervor(vor, name, nach, frage):
    teile = vor.split(' / ')
    b = [0x04, 0x02]
    for i, t in enumerate(teile):
        if i:
            b.append(0x08)
        b += t2.kod(t)
    b += [0x05, 0x01] + t2.kod(name) + [0x05, 0x00] + t2.kod(nach)
    b += [0x03, 0x02, 0x01, 0x00] if frage else [0x01, 0x00]
    return b, max([t2.px(t) for t in teile[:-1]] + [t2.px(teile[-1] + name + nach)])


def seiten(text):
    z = t2.umbruch(text, 0, t2.MAX_DESC_PX)
    return [z[i:i + MAX_ZEILEN] for i in range(0, len(z), MAX_ZEILEN)]


def main():
    fehler = []
    out = ['/* Generiert von re15_port/tools/r42_gegenstaende/texte.py - nicht von Hand aendern. PORT-TEXTE nach GEGENSTAENDE.md',
           ' * (V-02 Python/Magnum, V-03 Mutter, V-06 Bausatz und Name Detonator, Fundtexte der Ausweichstellen); Formen im Kopf des Werkzeugs. */']
    for item, name, text in BESCHR:
        s = seiten(text)
        b, bw = t2.beschreibung(s)
        for i, w in enumerate(bw):
            if w > t2.MAX_DESC_PX:
                fehler.append('Item 0x%02X Zeile %d = %d px' % (item, i + 1, w))
        out += t2.carr(name, b, 'Item 0x%02X CHECK: %s  (Zeilen %s px)' % (item, ' | '.join(' / '.join(x) for x in s),
                                                                         '/'.join(map(str, bw))))
    for item, name, seiten_ in BESCHR_FEST:
        b, bw = t2.beschreibung([list(x) for x in seiten_])
        for i, w in enumerate(bw):
            if w > t2.MAX_DESC_PX:
                fehler.append('Item 0x%02X Zeile %d = %d px' % (item, i + 1, w))
        if any(len(x) > MAX_ZEILEN for x in seiten_):
            fehler.append('Item 0x%02X mehr als %d Zeilen je Seite' % (item, MAX_ZEILEN))
        out += t2.carr(name, b, 'Item 0x%02X CHECK: %s  (Zeilen %s px)' % (item, ' | '.join(' / '.join(x) for x in seiten_),
                                                                         '/'.join(map(str, bw))))
    for item, name, text in NAMEN:
        out += t2.carr(name, t2.kod(text) + [0x07], 'Item 0x%02X Name: %s  (%d px)' % (item, text, t2.px(text)))
    for name, wo, text in FUND:
        b, zeilen, bw = t2.systemtext(text)
        if len(zeilen) > 2:
            fehler.append('%s: %d Zeilen > 2 (tools/r42_namen/lesbar_s5.py)' % (wo, len(zeilen)))
        for i, w in enumerate(bw):
            if w > t2.MAX_PX:
                fehler.append('%s Zeile %d = %d px' % (wo, i + 1, w))
        out += t2.carr(name, b, '%s: %s  (Zeilen %s px)' % (wo, ' / '.join(zeilen), '/'.join(map(str, bw))))
    sys.path.insert(0, os.path.join(HIER, '..', 'r42_namen'))
    import lesbar_s5 as L5
    b, zz, bws = L5.text_seiten(OHNE, t2.kod, [0x04, 0x02], [0x01, 0x00], None)
    zeilen, bw = [' // '.join(' / '.join(z) for z in zz)], [w for x in bws for w in x]
    for i, w in enumerate(bw):
        if w > t2.MAX_PX:
            fehler.append('Pult ohne Mutter Zeile %d = %d px' % (i + 1, w))
    out += t2.carr('k_g42_msg_ohne', b, 'ROOM2030/2031 Pult ohne Mutter: %s  (Zeilen %s px; Stufe 5 namen: %s)' % (
        ' / '.join(zeilen), '/'.join(map(str, bw)), L5.SYSTEM_BELEG))
    b, w = hervor(*FRAGE, True)
    if w > t2.MAX_PX:
        fehler.append('Frage %d px' % w)
    out += t2.carr('k_g42_msg_frage', b, 'ROOM2030/2031 Frage (Form ROOM2010 msg 0 @0x0906): %s  (%d px)' % (''.join(FRAGE), w))
    b, w = hervor(*DANACH, False)
    if w > t2.MAX_PX:
        fehler.append('Danach %d px' % w)
    out += t2.carr('k_g42_msg_danach', b, 'ROOM2030/2031 danach (Form ROOM2010 msg 1 @0x0928): %s  (%d px)' % (''.join(DANACH), w))
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
