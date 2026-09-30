#!/usr/bin/env python3
"""Spur E (Runde 34 Nacht), Bau-Stufe 9.0: Selbstpruefung der tragenden Plan-Konstanten.

Die Gegenpruefung dieser Spur ist am Konto-Limit gestorben; deshalb prueft der Bau selbst jede
tragende Datei-Byte-Behauptung des Bauplans (E_dokumente.md 2..5) an den ECHTEN Dateien nach
(memory reai-v2-zitierte-adresse-ist-kein-beleg: eine zitierte Adresse ist kein Beleg).
Disassembly-Behauptungen (FUN_80042bac, RE2 @0x80071bbc/@0x80071d04) prueft der Bau mit
re15_disasm.py / re2_disasm.py; hier nur Datei-Bytes, md5 und Texte.

Aufruf:  C:/Python310/python.exe re15_port/tools/r34n_e/selbstpruefung.py
Ausgabe: je Pruefung PASS/FAIL + gelesene Werte; Endzeile "SELBSTPRUEFUNG n/m PASS".
"""
import hashlib
import os
import struct
import sys

W = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
PSX = os.path.join(W, 're15_port', 'shared_assets', 'PSX')
RE2 = os.path.join(W, 'info', 're2leon', 'PL0', 'RDT')
AUFTRAG = os.path.join(W, 'analysis', 'befunde_runde34_nacht', 'AUFTRAG.md')
TEXTE = os.path.join(W, 'analysis', 'befunde_runde34_nacht', 'E_texte')
SATZ = os.path.join(W, 'build', 'r34n_e', 'satz_neu')
MD5LISTE = os.path.join(W, 'analysis', 'befunde_runde34_nacht', 'E_belege', 'satz_md5.txt')

ergebnisse = []


def pruefe(name, ok, info=''):
    ergebnisse.append((name, bool(ok)))
    print(('PASS ' if ok else 'FAIL ') + name + ('  | ' + info if info else ''))


def rdt(raum):
    with open(os.path.join(PSX, 'STAGE%s' % raum[0], 'ROOM%s.RDT' % raum), 'rb') as f:
        return f.read()


def hx(b):
    return ' '.join('%02x' % x for x in b)


def s16(b, o):
    return struct.unpack_from('<h', b, o)[0]


def u16(b, o):
    return struct.unpack_from('<H', b, o)[0]


def bytes_gleich(name, raum, off, erwartet_hex):
    d = rdt(raum)
    e = bytes.fromhex(erwartet_hex.replace(' ', ''))
    ist = d[off:off + len(e)]
    pruefe('%s ROOM%s @0x%05X' % (name, raum, off), ist == e, hx(ist))
    return d


# ---- 1. Aktions-Saetze / Items aus den RDTs --------------------------------------------------
# Dok 1: Leichen-Satz ROOM1051 main00 (Rechteck-Quelle Dok 1) und sein Stilllegen in sub01
d = bytes_gleich('Leichen-Satz Aot_set Slot 11 sce 3', '1051', 0x0C0E,
                 '2c 0b 03 31 00 00 92 3b ae e3 e8 03 e8 03 ff 00 18 03 00 00')
pruefe('  -> Rechteck (x,z,w,d)', (s16(d, 0x0C0E + 6), s16(d, 0x0C0E + 8), s16(d, 0x0C0E + 10),
                                  s16(d, 0x0C0E + 12)) == (15250, -7250, 1000, 1000),
       str((s16(d, 0x0C0E + 6), s16(d, 0x0C0E + 8), s16(d, 0x0C0E + 10), s16(d, 0x0C0E + 12))))
bytes_gleich('sub01 Ck(9,165) -> Aot_reset(11, sce 0, sat 0)', '1051', 0x0CB2,
             '06 00 10 00 21 09 a5 01 46 0b 00 00 00 00 00 00 00 00')
# In ROOM1050 ist Slot 11 NICHT belegt: derselbe Satz darf dort nicht stehen
d50 = rdt('1050')
pruefe('ROOM1050 hat keinen Aot_set Slot 11 sce 3 (Leichen-Satz nur Elza)',
       bytes.fromhex('2c0b0331') not in d50)

# Dok 3: Tisch-Nachricht (Satz-Waechter + Umzug) ROOM1020/1021
d = bytes_gleich('Tisch-Nachricht Aot_set Slot 10 sce 1 msg 3', '1020', 0x01F0E,
                 '2c 0a 01 31 00 00 a4 d4 4c b9 98 08 e4 0c 03 00 ff ff 00 00')
pruefe('  -> Rechteck', (s16(d, 0x1F0E + 6), s16(d, 0x1F0E + 8), s16(d, 0x1F0E + 10),
                         s16(d, 0x1F0E + 12)) == (-11100, -18100, 2200, 3300))
bytes_gleich('Tisch-Nachricht Aot_set Slot 11 sce 1 msg 12', '1021', 0x01F6C,
             '2c 0b 01 31 00 00 a4 d4 4c b9 98 08 e4 0c 0c 00 ff ff 00 00')

# Dok 4: Original-Items auf dem Verhoertisch ROOM1010 (Hoehe -1600, Nachbar-Spray)
d = bytes_gleich('Spray Item_aot_set Slot 2 Id 0x22 Bit 140 obj 0', '1010', 0x00996,
                 '50 02 09 31 00 00 a2 fe 50 14 e8 03 e8 03 22 00 01 00 8c 00 00 00')
pos = d.find(bytes.fromhex('c800c0f97c15'), 0x0930, 0x0960)
pruefe('Obj_model_set obj 0 (200,-1600,5500) im Satz ab @0x00930', pos >= 0,
       'Treffer @0x%05X, Satz %s' % (pos, hx(d[0x930:0x930 + 34])))
pos = d.find(bytes.fromhex('0807c0f97616'), 0x0940, 0x0990)
pruefe('Obj_model_set obj 1 (1800,-1600,5750) im Satz ab @0x00952', pos >= 0,
       'Treffer @0x%05X, Satz %s' % (pos, hx(d[0x952:0x952 + 34])))
ab = d[0x9AC:0x9AC + 22]
pruefe('Munition Item_aot_set @0x009AC Rechteck (1300,5300,1000,1000) Id 0x15',
       ab[0] == 0x50 and (s16(ab, 6), s16(ab, 8), s16(ab, 10), s16(ab, 12)) == (1300, 5300, 1000, 1000)
       and ab[14] == 0x15, hx(ab))
d11 = rdt('1011')
ab = d11[0x0954:0x0954 + 22]
pruefe('ROOM1011 Item_aot_set @0x00954 Id 0x39 an derselben Stelle (Slot 2)',
       ab[0] == 0x50 and ab[1] == 2 and ab[14] == 0x39 and
       (s16(ab, 6), s16(ab, 8)) == (-350, 5200), hx(ab))

# Vorrang-Beispiel des Originals ROOM10E0: Item Slot 1 + Nachricht Slot 8, gleiches Rechteck
de0 = rdt('10E0')
a = de0[0x0C4E:0x0C4E + 22]
b = de0[0x0CCC:0x0CCC + 20]
pruefe('ROOM10E0 @0x00C4E Item_aot_set Slot 1 / @0x00CCC Aot_set sce 1 Slot 8, gleiches Rechteck',
       a[0] == 0x50 and a[1] == 1 and b[0] == 0x2C and b[1] == 8 and b[2] == 1 and
       a[6:14] == b[6:14] and (s16(a, 6), s16(a, 8), s16(a, 10), s16(a, 12)) == (-650, -7350, 2600, 1000),
       'Item %s | Nachricht %s' % (hx(a), hx(b)))

# ---- 2. pri-Sektionen (Verdeckung / Klemmen) --------------------------------------------------
d = rdt('1010')
for off in (0x458, 0x45C, 0x460, 0x464, 0x470, 0x478):
    pruefe('ROOM1010 pri-Zeiger @0x%03X = NULL' % off, d[off:off + 4] == b'\xff\xff\xff\xff', hx(d[off:off + 4]))
d = rdt('1000')
pruefe('ROOM1000 pri-Zeiger Cut 0 @0x4C8 = NULL', d[0x4C8:0x4CC] == b'\xff\xff\xff\xff', hx(d[0x4C8:0x4CC]))


def pri_masken(d, off):
    gc, mc = u16(d, off), u16(d, off + 2)
    grp = []
    p = off + 4
    for _ in range(gc):
        grp.append((u16(d, p), s16(d, p + 4), s16(d, p + 6)))
        p += 8
    total = sum(g[0] for g in grp)
    out, gi, used = [], 0, 0
    for _ in range(total):
        sx, sy, dxl, dyl = d[p], d[p + 1], d[p + 2], d[p + 3]
        depth = u16(d, p + 4)
        sb = d[p + 7]
        p += 8
        if (sb & 0xF0) == 0:
            w, h = u16(d, p), u16(d, p + 2)
            p += 4
        else:
            w = h = (sb >> 4) * 8
        while gi < gc and used >= grp[gi][0]:
            gi += 1
            used = 0
        ax, ay = (grp[gi][1], grp[gi][2]) if gi < gc else (0, 0)
        used += 1
        out.append((dxl + ax, dyl + ay, w, h, depth))
    return gc, mc, out


d = rdt('1020')
gc, mc, ms = pri_masken(d, 0xD28)
m35 = ms[35] if len(ms) > 35 else None
pruefe('ROOM1020 pri Cut 3 @0xD28: Kopf', (gc, mc) == (9, 0x2C), 'Gruppen %d, Masken %d' % (gc, mc))
pruefe('  -> Maske 35 = Tischplatte x127..166 y91..114 Tiefe 258',
       m35 is not None and m35[4] == 258 and m35[0] == 127 and m35[1] == 91 and
       m35[0] + m35[2] - 1 == 166 and m35[1] + m35[3] - 1 == 114, str(m35))

# ---- 3. nOmodel (RDT-Byte 2) -> erste freie obj_id -------------------------------------------
for raum, n in (('1000', 2), ('1001', 2), ('1010', 3), ('1011', 3), ('1020', 7), ('1021', 6),
                ('1050', 2), ('1051', 2)):
    d = rdt(raum)
    pruefe('ROOM%s nOmodel (Byte 2) = %d' % (raum, n), d[2] == n, str(d[2]))

# ---- 4. RE1.5-Kodierung (DEBUG.BIN) -------------------------------------------------------------
with open(os.path.join(PSX, 'BIN', 'DEBUG.BIN'), 'rb') as f:
    dbg = f.read()
pruefe('DEBUG.BIN 0x07370 erste FILE-Id = 48 52 5c', dbg[0x7370:0x7373] == bytes([0x48, 0x52, 0x5C]),
       hx(dbg[0x7370:0x7373]))
pruefe("DEBUG.BIN 0x04e04 \"Chris' Diary\"", dbg[0x4E04:0x4E04 + 13] ==
       bytes.fromhex('1f444e454f3a0020453d4e5507'), hx(dbg[0x4E04:0x4E04 + 13]))

# ---- 5. RE2-Weltmodelle (Datei-Offset + md5) ---------------------------------------------------
MODELLE = (
    ('mesh00 MD1', 'ROOM1150.RDT', 0x029F34, 372, '0541704ee41d6445b718ee0b1b8abb3b'),
    ('mesh00 TIM', 'ROOM1150.RDT', 0x04C3F0, 34848, '6e6f32cb59ddb4b83b27c921a2dbfd90'),
    ('mesh03 MD1', 'ROOM10E0.RDT', 0x002320, 372, 'cf9f316dd6ba0ecf599bea18a83c36d1'),
    ('mesh03 TIM', 'ROOM10E0.RDT', 0x015224, 17440, '63bd93f2be1cc97066aa09afd3c67e4d'),
    ('mesh01 MD1', 'ROOM2020.RDT', 0x003254, 404, 'ae2d0a30a18ed9cb9f089c21839ca61e'),
    ('mesh01 TIM', 'ROOM2020.RDT', 0x01A160, 34848, '6b864a6f02ffd6c0720be1efa654c3cf'),
    ('mesh04 MD1', 'ROOM60A0.RDT', 0x002F08, 404, 'cab7b32d230a19728221ffb6b1d75104'),
    ('mesh04 TIM', 'ROOM60A0.RDT', 0x03B924, 34848, '73fa3595c568578eace3379747390ff6'),
)
for name, datei, off, n, md5 in MODELLE:
    with open(os.path.join(RE2, datei), 'rb') as f:
        f.seek(off)
        b = f.read(n)
    ist = hashlib.md5(b).hexdigest()
    pruefe('%s %s @0x%06X %d B md5' % (name, datei, off, n), ist == md5, ist)

# ---- 6. Satz FILE26..29: md5-Liste und Seitenzahl ---------------------------------------------
liste = {}
with open(MD5LISTE, encoding='utf-8') as f:
    for zeile in f:
        for eintrag in zeile.strip().split(';'):
            if eintrag.strip():
                m, n = eintrag.strip().split(' ', 1)
                liste[n] = m
for nr, max_page in ((26, 3), (27, 4), (28, 2), (29, 2)):
    ordner = os.path.join(SATZ, 'FILE%02d' % nr)
    namen = sorted(x for x in os.listdir(ordner) if x.endswith('.TIM'))
    alle_gleich = all(hashlib.md5(open(os.path.join(ordner, x), 'rb').read()).hexdigest()[:8] == liste.get(x)
                      for x in namen)
    seiten = [x for x in namen if x.startswith('FILE%02d_p' % nr)]
    pruefe('FILE%02d: %d Dateien md5 = satz_md5.txt' % (nr, len(namen)), alle_gleich and len(namen) == max_page + 3)
    pruefe('FILE%02d: p%02d vorhanden, p%02d nicht -> max_page %d' % (nr, max_page, max_page + 1, max_page),
           ('FILE%02d_p%02d_page.TIM' % (nr, max_page)) in seiten and
           ('FILE%02d_p%02d_page.TIM' % (nr, max_page + 1)) not in seiten, ' '.join(seiten))
# Papier Dok 2 = Papier des Irons Diary (FILE25)
p25 = hashlib.md5(open(os.path.join(W, 're15_port', 'shared_assets', 'RE2', 'FILES', 'FILE25_title_paper.TIM'),
                       'rb').read()).hexdigest()
pruefe('FILE27_title_paper == FILE25_title_paper (Irons-Papier)', p25[:8] == liste['FILE27_title_paper.TIM'], p25)

# ---- 7. Texte: unabhaengig aus AUFTRAG.md gezogen -----------------------------------------------
with open(AUFTRAG, encoding='utf-8') as f:
    zeilen = f.read().split('\n')


def ziehe(von, bis):
    out = []
    for z in zeilen[von - 1:bis]:
        assert z.startswith('>'), z
        z = z[1:]
        if z.startswith(' '):
            z = z[1:]
        z = z.lstrip('\t')
        if z.startswith('Text: '):
            z = z[len('Text: '):]
        out.append(z)
    return '\n'.join(out) + '\n'


for datei, von, bis, titelzeile in (('dok1_police_officer', 23, 27, 22), ('dok2_elliot', 30, 38, 29),
                                    ('dok3_marvin', 42, 48, 41), ('dok4_armory', 56, 58, 55)):
    soll = ziehe(von, bis)
    ist = open(os.path.join(TEXTE, datei + '.txt'), encoding='utf-8', newline='').read()
    pruefe('Text %s == AUFTRAG.md Z. %d..%d (%d Zeichen)' % (datei, von, bis, len(soll)), ist == soll)
    titel = zeilen[titelzeile - 1].split('Überschrift: ', 1)[1].strip()
    ist_t = open(os.path.join(TEXTE, datei + '_titel.txt'), encoding='utf-8', newline='').read().strip()
    pruefe('  Titel %r == AUFTRAG.md Z. %d' % (titel, titelzeile), ist_t == titel, ist_t)

n_ok = sum(1 for _, ok in ergebnisse if ok)
print('SELBSTPRUEFUNG %d/%d PASS' % (n_ok, len(ergebnisse)))
sys.exit(0 if n_ok == len(ergebnisse) else 1)
