#!/usr/bin/env python3
"""Sprachdateien (synchro/) -> (Raum, Message-Id) -> Text + Herkunftsurteil.
Regel: synchro/STAGE<n>/room<RAUM>/main<ID>.wav (audio_pc.c re15_voice_load_clip). Sonderzuordnungen:
stimmen_r39.c k_tabelle (1050 22-24 <- main09-11, 1150 22-29 <- main15-22, 11B0 20/21 <- main13/14, 1171 3 <- main02),
elza_r36.c:226 (1011 main16.5), dining_r39.c:550-555 (1141 help01-05 = Id 53-57, door_sound = Id 59; auch 1041/1131),
rekorder_r38 (1100/1101 Irons_Rekorder_Sound).
Urteile: textabgleich_stage*.md T1 (WOERTLICH/SINNGEMAESS/ABGEWANDELT/MOD-TEXT/PORT-TEXT); Ids, die der Port selbst
einsetzt, aus port_module_teil1/2.md (Liste PORT_IDS unten, Herkunft PORT).
Aufruf: python stimmen.py <synchro-Wurzel> <port_zensus.json> <aus.md>"""
import sys, os, re, json, glob
HERE = os.path.dirname(os.path.abspath(__file__))
SY, PJ, OUT = sys.argv[1], sys.argv[2], sys.argv[3]
P = json.load(open(PJ, encoding='utf-8'))
SONDER = {('1050', 'main09'): 22, ('1050', 'main10'): 23, ('1050', 'main11'): 24,
          ('11B0', 'main13'): 20, ('11B0', 'main14'): 21, ('1171', 'main02'): 3, ('1011', 'main16.5'): 24}
for i in range(8): SONDER[('1150', 'main%02d' % (15 + i))] = 22 + i
for i in range(5): SONDER[('1141', 'help%02d' % (i + 1))] = 53 + i
SONDER[('1141', 'door_sound')] = 59
# vom Port eingesetzte/ersetzte Ids (port_module_teil1.md / teil2.md, Rueckmeldungen der Teilspuren)
PORT_IDS = {'10F0': range(6, 24), '1150': list(range(20, 30)), '1151': [20], '11C0': list(range(10, 15)) + [40],
            '11B0': [20, 21, 22, 28, 29], '1050': list(range(20, 27)), '1051': [20, 21, 40], '1060': [1], '1130': [6],
            '1011': [24], '1131': [40, 41, 53, 54, 55, 56, 57], '1141': [40, 41, 51, 52, 53, 54, 55, 56, 57, 59],
            '1041': [40, 41, 53, 54, 55, 56, 57], '10F1': [40, 41, 42], '1210': [20], '1091': [20, 29], '1090': [29],
            '1190': [6, 7], '1191': [6, 7], '1021': [40], '1110': [20, 21], '1111': [20, 21], '1230': [22, 23], '1231': [22, 23],
            '2000': [40], '2001': [40], '2030': list(range(40, 49)) + list(range(50, 56)), '2031': list(range(40, 49)) + list(range(50, 56)),
            '2050': [40, 43, 44], '2051': [40, 43, 44], '20B0': [41], '20B1': [41], '2070': [40], '2071': [40],
            '2010': [40, 43], '2011': [40, 43], '2090': [42, 43], '2091': [42, 43], '2080': [42], '2081': [42]}
OVR = {('30C1', 3), ('30C1', 4), ('30C1', 7), ('6030', 1), ('6030', 2), ('6030', 12), ('6031', 0), ('6031', 1),
       ('50B0', 1), ('6000', 0), ('6000', 3), ('50B1', 0)}
URT = {}
for f in glob.glob(os.path.join(HERE, 'textabgleich_stage*.md')):
    for ln in open(f, encoding='utf-8'):
        c = [x.strip() for x in ln.split('|')]
        if len(c) > 8 and re.fullmatch(r'(ROOM)?[0-9A-F]{4}', c[1]) and c[2].isdigit():
            URT[(c[1][-4:], int(c[2]))] = (c[7], c[6], c[5])
rows = []; summ = {}
for p in sorted(glob.glob(os.path.join(SY, 'STAGE*', 'room*', '*.wav'))):
    room = os.path.basename(os.path.dirname(p))[4:].upper(); stem = os.path.splitext(os.path.basename(p))[0]
    if (room, stem) in SONDER: mid = SONDER[(room, stem)]; via = 'Sonderzuordnung'
    else:
        m = re.fullmatch(r'main(\d+)', stem); mid = int(m.group(1)) if m else None; via = 'Regel' if m else 'unbekannt'
    r = P.get(room, {}); tx = r.get('texts', {}).get(str(mid), '') if mid is not None else ''
    if mid is not None and (room, mid) in OVR:
        her = 'PORT-TEXT (story_r36 ersetzt Mod-Zeile)'; vor = URT.get((room, mid), ('', '', ''))[1]
    elif mid is not None and mid in PORT_IDS.get(room, []):
        her = 'PORT (Modul-Text, Nutzer-Lueckenfueller)'; vor = '-'; tx = tx or '(Port-Text, siehe port_module_teil*.md)'
    elif mid is not None and (room, mid) in URT:
        u = URT[(room, mid)]; her = u[0]; vor = u[1]; tx = u[2]
    elif tx: her = '(kein Urteil gefunden)'; vor = ''
    else: her = 'KEIN TEXT unter dieser Id'; vor = ''
    summ[her.split(' ')[0]] = summ.get(her.split(' ')[0], 0) + 1
    rows.append('| %s | %s | %s | %s | %s | %s | %s |' % (room, stem, mid, via, her, vor, tx.replace('|', '/')[:110]))
open(OUT, 'w', encoding='utf-8').write('# Sprachdateien -> Zeile -> Herkunft (erzeugt von stimmen.py)\n\nZaehlung: %s\n\n'
    % json.dumps(summ, ensure_ascii=False) + '| Raum | Datei | Message-Id | Zuordnung | Herkunft (Urteil) | Disc-Vorlage | Englisch |\n|---|---|---|---|---|---|---|\n'
    + '\n'.join(rows) + '\n')
print(len(rows), summ)
