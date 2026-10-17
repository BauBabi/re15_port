"""port_modelle_sicht.py - Runde 38 Spur A (Fortsetzung 2026-10-17): Sichtbarkeit der PORT-EINGEBACKENEN Modelle unter
dem Rueckseiten-Verwurf, je Raum und JEDEM Cut (nicht nur dem Start-Cut des 120-Raum-Scans).

Warum: der Verwurf (NCLIP MAC0 < 0 auf v0,v1,v2; FUN_800254a0 @0x800255a8, FUN_800256b0 @0x800257c0, FUN_80022f0c
@0x80023010, FUN_80023098 @0x80023190) streicht genau die Dreiecke, deren Bildschirm-Umlauf negativ ist. Ein FLACHES
Modell (Schluessel, Karte, Zettel) hat nur EINE Seite: zeigt seine Lage der Kamera die Rueckseite, verschwindet es ganz.
Die Wicklungs-Pruefung ueber die gespeicherten Normalen (unit_r38_cursor_wicklung Teil 2/3) sieht das nicht, weil die
Normale eines flachen RE2-Item-Modells nichts darueber sagt, welche Seite die Port-Lage der Kamera zeigt.

Messung: exe ALT (ohne Verwurf, Stand 1bcbd0ff) mit RE15_TRILOG (ganzer Schirm), RE15_FORCE_CUT=c, Debug-Sprung; je
TIM-Platz die Dreiecke des LETZTEN Bildes mit NCLIP-Vorzeichen (wie trilog_nclip.py). NEU = ALT ohne die Dreiecke mit
NCLIP < 0. Gemeldet wird je Port-Platz und Cut: gezeichnet ALT, davon < 0; "WEG" wenn ALT > 0 und alle < 0.
Die rohe Trilog-Datei wird nach der Auswertung geloescht (sonst ~10 MB je Lauf).

Aufruf: python port_modelle_sicht.py <ausgabeordner> [--exe alt|neu] [RAUM[:elza] ...]
  ohne Raumliste: alle Raeume mit Port-Modellen (Leon und Elza).
Ausgabe: <ausgabeordner>/sicht.txt"""
import os, re, subprocess, sys
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor

WT = r'C:/workspace/git/reAi_v2/.claude/worktrees/r38_cursor'
PCDIR = WT + '/re15_port/build/platform/pc'
EXE = {'alt': PCDIR + '/re15_pc_r38a_alt.exe', 'neu': PCDIR + '/re15_pc_r38a.exe'}
PSX = WT + '/re15_port/shared_assets/PSX'

# TIM-Platz der Port-Props: main.c RE15_TIM_SLOT_PROP(op) = op < 6 ? 4 + op : 26 + (op - 6)
PORT_PLATZ = {
    6: 'obj 2 Dokument 1 (1050) / Dokument 2 (1000)',
    7: 'obj 3 Dokument 4 (1010)',
    8: 'obj 4 Memory Card 1010 / Schrot 1090 / Schluessel 10F0-10F1 / Sicherung 1150',
    9: 'obj 5 Feuerloescher 1050/1090/1091 / Irons Diary 1150',
    26: 'obj 6 Irons Keycard 1150',
    27: 'obj 7 Dokument 3 (1020) / Granate 1150',
    28: 'Hebetisch-Cursor 1150 (RE15_HC_TIM_SLOT)',
    35: 'obj 15 Lageplan-Karte (1140/10D0/1030/1200/11D0)',
}
# Raeume mit Port-Modellen (Leon-Nummer; :elza = Spielerwahl Elza, der Raumlader nimmt ROOMxxx1)
RAEUME = ['1050', '1000', '1020', '1010', '1090', '10F0', '1150', '1140', '10D0', '1030', '1200', '11D0',
          '1050:elza', '1000:elza', '1020:elza', '1010:elza', '1090:elza', '10F0:elza', '1150:elza', '1140:elza', '10D0:elza',
          '1030:elza', '1200:elza', '11D0:elza']

PAT = re.compile(r'^(\d+) (\S+) slot=(\d+) clut=0x([0-9A-F]+) z=(-?\d+)  \((-?\d+),(-?\d+)\)uv\((\d+),(\d+)\) '
                 r'\((-?\d+),(-?\d+)\)uv\((\d+),(\d+)\) \((-?\d+),(-?\d+)\)uv\((\d+),(\d+)\)')


def rdt_kopf(raum, elza):
    """RDT-Kopf: Byte 1 = nCut, Byte 2 = nOmodel (RE15_KNOWLEDGE.md §1.1)."""
    r = raum[:3] + ('1' if elza else '0')
    p = '%s/STAGE%s/ROOM%s.RDT' % (PSX, raum[0], r)
    d = open(p, 'rb').read(8)
    return d[1], d[2]


def platz_obj(slot):
    """Umkehrung von RE15_TIM_SLOT_PROP: Platz 4..9 -> obj 0..5, Platz 26.. -> obj 6.."""
    return slot - 4 if 4 <= slot <= 9 else (slot - 20 if slot >= 26 else -1)


def auswerten(pfad):
    je = defaultdict(list)
    with open(pfad, encoding='utf-8', errors='replace') as f:
        for l in f:
            m = PAT.match(l)
            if m:
                je[int(m.group(3))].append(m)
    erg = {}
    for slot, ms in je.items():
        # letztes Bild = hinter der VORLETZTEN Wiederholung der LETZTEN Zeile des Platzes (Kamera per FORCE_CUT fest,
        # Port-Props stehen still -> jedes Bild wiederholt dieselben Zeilen). Die erste Zeile taugt nicht als Marke:
        # sie kann aus dem Startraum VOR dem Sprung stammen (dann zaehlte trilog_nclip.py alle Bilder, z.B. 572 = 4 x 143).
        sig = lambda m: m.group(6, 7, 8, 9, 10, 11, 14, 15)
        letzte = sig(ms[-1])
        frueher = [i for i in range(len(ms) - 1) if sig(ms[i]) == letzte]
        start = (frueher[-1] + 1) if frueher else 0
        c = Counter()
        for m in ms[start:]:
            x0, y0, x1, y1, x2, y2 = (int(m.group(i)) for i in (6, 7, 10, 11, 14, 15))
            mac0 = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
            c['<0' if mac0 < 0 else ('=0' if mac0 == 0 else '>0')] += 1
        erg[slot] = c
    return erg


def lauf(aus, art, raum, elza, cut):
    wd = os.path.join(aus, '%s%s' % (raum, 'e' if elza else ''), 'c%02d' % cut)
    os.makedirs(wd, exist_ok=True)
    fertig = os.path.join(wd, 'slots_%s.txt' % art)
    if os.path.exists(fertig):
        return [l.rstrip('\n') for l in open(fertig)]
    env = dict(os.environ)
    env.update({'RE15_NOAUDIO': '1', 'RE15_NO_INTRO': '1', 'RE15_WINDOW_SCALE': '1', 'RE15_FORCE_CUT': str(cut),
                'RE15_TRILOG': 'trilog_%s.txt' % art})
    if elza:
        env.update({'RE15_TITLE_CONFIRM_MS': '1500', 'RE15_PSELECT_AUTO': '1', 'RE15_PSELECT_AUTO_SWITCH': '1',
                    'RE15_DEBUG_JUMP': raum + '@250', 'RE15_FRAMEDUMP': '330:f_%s.ppm' % art, 'RE15_EXIT_AT': '332'})
    else:
        env.update({'RE15_TITLE_SHOT': 'title.bmp', 'RE15_DEBUG_JUMP': raum + '@60',
                    'RE15_FRAMEDUMP': '140:f_%s.ppm' % art, 'RE15_EXIT_AT': '142'})
    try:
        subprocess.run([EXE[art]], cwd=wd, env=env, timeout=120, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except subprocess.TimeoutExpired:
        pass
    tl = os.path.join(wd, 'trilog_%s.txt' % art)
    zeilen = []
    raum_ist = '?'
    try:
        for l in open(os.path.join(wd, 'debug.log'), encoding='utf-8', errors='replace'):
            m = re.search(r'EXIT_AT: Bild \d+ in Raum ([0-9a-fA-F]+)', l)
            if m:
                raum_ist = m.group(1)
    except OSError:
        pass
    if os.path.exists(tl):
        for slot, c in sorted(auswerten(tl).items()):
            zeilen.append('%d %d %d %d' % (slot, c['<0'], c['=0'], c['>0']))
        os.remove(tl)
    zeilen.insert(0, 'raum_ist %s' % raum_ist)
    with open(fertig, 'w') as f:
        f.write('\n'.join(zeilen) + '\n')
    return zeilen


def main():
    aus = os.path.abspath(sys.argv[1])
    rest = sys.argv[2:]
    art = 'alt'
    if rest[:1] == ['--exe']:
        art = rest[1]
        rest = rest[2:]
    liste = rest or RAEUME
    auftraege = []
    for r in liste:
        raum, _, z = r.partition(':')
        elza = z == 'elza'
        for c in range(rdt_kopf(raum, elza)[0]):
            auftraege.append((raum, elza, c))
    with ThreadPoolExecutor(max_workers=int(os.environ.get('SCAN_WORKERS', '4'))) as ex:
        erg = list(ex.map(lambda a: (a, lauf(aus, art, *a)), auftraege))
    out = []
    weg = []
    for (raum, elza, c), zeilen in erg:
        ist = zeilen[0].split()[1] if zeilen else '?'
        for z in zeilen[1:]:
            slot, neg, null, pos = (int(x) for x in z.split())
            # Port-Platz NUR, wenn die obj_id im Raum frei ist (obj >= nOmodel; die Port-Lader in main.c pruefen
            # "nprops <= oid") — sonst ist es ein Original-Prop desselben Platzes.
            if slot not in PORT_PLATZ or platz_obj(slot) < rdt_kopf(raum, elza)[1]:
                continue
            ges = neg + null + pos
            if ges == 0:
                continue
            marke = 'WEG' if neg == ges else ('teilweise' if neg else 'ganz')
            s = 'ROOM%s%s (Lauf endet in %s) Cut %2d Platz %2d: gezeichnet %4d, NCLIP<0 %4d, =0 %d, >0 %4d -> %s  [%s]' % (
                raum, ' Elza' if elza else '', ist, c, slot, ges, neg, null, pos, marke, PORT_PLATZ[slot])
            out.append(s)
            if marke == 'WEG':
                weg.append(s)
    with open(os.path.join(aus, 'sicht_%s.txt' % art), 'w') as f:
        f.write('\n'.join(out) + '\n\nWEG (Port-Modell ganz rueckseitig -> mit Verwurf unsichtbar):\n' + '\n'.join(weg) + '\n')
    print('\n'.join(out))
    print('\nWEG:\n' + '\n'.join(weg))


if __name__ == '__main__':
    main()
