#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 5: jede RDT-Nachricht der Mod mit einer Seite von DREI Zeilen -> zwei Zeilen bzw. zwei Seiten.
Ausgabe engine/src/gen/zweizeilig_r42_texte.inc (eingesetzt von engine/src/namen_r42.c nach re15_msg_load_room_block; RDT unveraendert).

    python re15_port/tools/r42_namen/zweizeilig_s5.py           (schreibt die .inc)
    python re15_port/tools/r42_namen/zweizeilig_s5.py --check   (prueft nur)

ENTSCHEID (Orchestrator Stufe 5, Regel "Original oder nicht"): die Disc-Regel "hoechstens zwei Zeilen je Seite" gilt fuer ALLE
Untertitel. Belege (lesbar_s4.py): die echte 40%-Disc setzt jede neue Zeile auf y = 180 + 16 (@0x8002899c/@0x800289a0/@0x800289ac),
0 von 700 Seiten haben drei Zeilen; der Letterbox-Balken beginnt bei y = 216 (Tabelle @0x80072ecc, Halbwort @0x80072f06), Zeile 3
(y 212) liegt auf ihm. Zensus aller Raeume wie im Spiel geladen (Riegel unit_r42_namen_alle_zeilen): 35 RDT-Seiten mit drei Zeilen
(diese Datei) + 5 Port-Texte (in ihren eigenen Werkzeugen, Kopf von lesbar_s5.py).

WORTLAUT bleibt Glyphe fuer Glyphe (das Werkzeug prueft es), der Namenskopf `05 <Farbe> <Name>: 05 00` steht auf JEDER Seite,
Systemtexte und der Erzaehler (ROOM1240/1241, ohne Namen wie RE2) bleiben ohne Namen. Der Schwanz `04 01 01 N` bzw. `01 00` bleibt
der des RDT. Regeln (Zeilen <= 271 px, keine Zeile mit nur einem Wort ausser "Name: Wort", Trennstelle, Zeitpunkt): lesbar_s5.py.

TABELLE (Raum, Id, RDT-Datei-Offset, Woerter auf Seite 1 bzw. None = nur neu umbrechen, Zeit t in s bzw. None, Art, Beleg):
  Art 'pause'  Stimme, Seite 2 am Pausenende t der Aufnahme synchro/STAGEn/roomXXXX/mainNN.wav (namen/s5/pausen5.py), die Woerter vor
               der Pause aus Whisper medium.en auf dem Stueck [0, Pausenmitte] (namen/s5/asr_pausen.txt, im Beleg "VOR: ...");
               N = round(30 t) (lesbar_s4.seitenwartezeit, S = 0: die Aufnahme gehoert zu dieser Nachricht, audio_pc.c main%02d).
  Art 'wort'   Stimme ohne passende Pause: t = Whisper-Wortanfang des ersten Worts von Seite 2 (namen/s5/asr.txt).
  Art 'ohne'   keine Aufnahme: Seite 1 steht so lange wie die Original-Untertitel desselben Raums (nachlauf_r42.nachlauf(raum, 2)).
  Art 'system' Systemtext `04 02 .. 01 00`: Seite 2 nach der Aktionstaste `02 00` (Form der 16 Speicherstellen-Texte, z. B.
               re15_savepoint.c "`04 02` Satz1 `02 00` Satz2"; FUN_80028134 Zustand 1 = warten auf die Taste, msg_common.c).
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
sys.path.insert(0, HIER)
import texte_stage3a as T  # noqa: E402
import nachlauf_r42  # noqa: E402
import lesbar_s4 as LS  # noqa: E402
import lesbar_s5 as L5  # noqa: E402
import fortsetzung_bauen as F  # noqa: E402  (rdt_msg, zeilen_worte, wort_str, breite, zeile_glyphen)

ZIEL = os.path.join(T.PORT, 'engine', 'src', 'gen', 'zweizeilig_r42_texte.inc')

S = 'synchro/STAGE%s/room%s/main%02d.wav'
FAELLE = (
    (0x10D0, 17, 0x021BD, 6, 2.89, 'pause', 'Pause 2.40-2.89, VOR: "Has everyone been eaten by zombies?"'),
    (0x10D0, 20, 0x02298, 8, 2.66, 'pause', 'Pause 2.13-2.66, VOR: "You are the only one who I found."'),
    (0x10D0, 22, 0x02351, 9, 3.00, 'pause', 'Pause 2.78-3.00, VOR: "The Chief told me about using the communication system."'),
    (0x10D0, 23, 0x023B4, 5, 2.68, 'pause', 'Pause 2.10-2.68, VOR: "But the system is damaged."'),
    (0x10D0, 26, 0x024CE, 7, 3.28, 'pause', 'Pause 2.98-3.28, VOR: "Okay, I\'ll meet with the chief first."'),
    (0x1141, 5, 0x011E6, 5, 2.33, 'pause', 'Pause 1.83-2.33, VOR: "Don\'t go to the rooftop!"'),
    (0x1141, 9, 0x012C0, 6, 2.38, 'pause', 'Pause 1.99-2.38, VOR: "Looks like some kind of gorilla!"'),
    (0x1150, 9, 0x015A6, 4, 3.19, 'pause', 'Pause 2.45-3.19, VOR: "I know. I know."'),
    (0x1150, 11, 0x01657, 3, None, 'ohne', 'keine Aufnahme (main11.wav fehlt); Trennung hinter "card..."'),
    (0x11B0, 5, 0x019F4, 14, 5.13, 'pause', 'Pause 4.82-5.13, VOR: "Well, you saw it, right? We can\'t get out using the patrol cars '
                                            'anymore." (Pause 2.00-2.48 nach "right?": Seite 2 passt nicht in zwei Zeilen)'),
    (0x11B0, 8, 0x01AD7, 10, 3.54, 'pause', 'Pause 2.94-3.54, VOR: "We don\'t really have any other option than to try."'),
    (0x1211, 5, 0x0246E, 5, 2.67, 'pause', 'Pause 2.55-2.67, VOR: "Listen, we\'re in serious trouble."'),
    (0x1240, 0, 0x00638, 8, 2.70, 'wort', 'keine sichere Pause (Musikbett), Whisper "an"[2.70]'),
    (0x1240, 2, 0x006EB, 6, 2.60, 'wort', 'keine Pause, Whisper "international"[2.60]'),
    (0x1240, 3, 0x00742, 6, 3.34, 'wort', 'keine sichere Pause (Musikbett), Whisper "Unit"[3.34]'),
    (0x1240, 4, 0x0079E, 9, 3.24, 'wort', 'keine sichere Pause (Musikbett), Whisper "of"[3.24]'),
    (0x1241, 0, 0x0063C, 8, 2.70, 'wort', 'wie ROOM1240 msg 0 (room1241/main00.wav bytegleich room1240/main00.wav)'),
    (0x1241, 2, 0x006EF, 6, 2.60, 'wort', 'wie ROOM1240 msg 2 (Aufnahme bytegleich)'),
    (0x1241, 3, 0x00746, 6, 3.34, 'wort', 'wie ROOM1240 msg 3 (Aufnahme bytegleich)'),
    (0x1241, 4, 0x007A2, 9, 3.24, 'wort', 'wie ROOM1240 msg 4 (Aufnahme bytegleich)'),
    (0x3000, 3, 0x018DE, 2, 1.73, 'pause', 'Pause 1.45-1.73, VOR: "Oh, man."'),
    (0x3070, 2, 0x039FF, 6, 2.36, 'wort', 'Pause 0.99-1.53 nach "Underground." passt nicht (Seite 2 drei Zeilen); Whisper "off"[2.36]'),
    (0x3091, 16, 0x021BB, 4, 2.69, 'pause', 'Pause 2.43-2.69, VOR: "Sherry, huh? I\'m John."'),
    (0x3091, 18, 0x02256, 7, 3.07, 'pause', 'Pause 2.95-3.07, VOR: "Well, of course. She saved my life!"'),
    (0x30E0, 5, 0x01206, 6, 1.89, 'pause', 'Pause 1.58-1.89, VOR: "And what happened in that tunnel?"'),
    (0x30E0, 7, 0x01275, 3, 1.81, 'pause', 'Pause 1.43-1.81, VOR: "That\'s really bad."'),
    (0x30E1, 12, 0x011E8, 4, 2.04, 'pause', 'Pause 1.63-2.04, VOR: "Yes, I think so."'),
    (0x30E1, 14, 0x01282, 8, 3.60, 'pause', 'Pause 3.25-3.60, VOR: "Anyway, there are too many walking corpses outside."'),
    (0x4010, 8, 0x01167, 10, 3.50, 'pause', 'Pause 3.06-3.50, VOR: "It\'s a virus that makes people turn into those things."'),
    (0x4010, 10, 0x01208, 5, 2.00, 'pause', 'Pause 1.70-2.00, VOR: "The vaccine itself isn\'t here."'),
    (0x4010, 20, 0x013D6, 9, 2.56, 'wort', 'keine Pause, Whisper "center\'s"[2.56]'),
    (0x4010, 21, 0x01442, 11, 2.48, 'pause', 'Pause 2.28-2.48, VOR: "As long as you can start up the program for it."'),
    (0x5011, 3, 0x00F19, 9, 4.10, 'pause', 'Pause 3.82-4.10, VOR: "John, I read some files in this research center."'),
    (0x5060, 5, 0x033F7, 6, None, 'system', 'Systemtext, Trennung am Satzende "building."'),
    (0x5061, 5, 0x033F7, 6, None, 'system', 'wie ROOM5060 msg 5 (gleicher Text, gleicher Offset)'),
)


def zerlegen(m):
    """-> (kopf, text-glyphen, schwanz). kopf = `04 00 05 F Name: 05 00` | `04 00` | `04 02`."""
    e = len(m) - 1
    while m[e] != 0x01:
        e -= 1
    if e >= 2 and m[e - 2] == 0x04:
        e -= 2
    schwanz = m[e:]
    if m[:3] == [0x04, 0x00, 0x05]:
        j = m.index(0x05, 4)
        assert m[j + 1] == 0x00 and m[j + 2] == F.LEER, m[:j + 3]
        kopf, i = m[:j + 2], j + 3
    else:
        assert m[:2] in ([0x04, 0x00], [0x04, 0x02]), m[:4]
        kopf, i = m[:2], 2
    text = m[i:e]
    assert not any(x in (0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0A, 0x0B) for x in text), text
    return kopf, text, schwanz


def main():
    fehler = []
    out = ['/* Generiert von re15_port/tools/r42_namen/zweizeilig_s5.py - nicht von Hand aendern. Runde 42 Strang namen, Stufe 5:',
           ' * RDT-Nachrichten mit drei Zeilen auf einer Seite -> hoechstens zwei Zeilen je Seite (Belege im Kopf des Werkzeugs).',
           ' * RDT unveraendert; eingesetzt von engine/src/namen_r42.c nach re15_msg_load_room_block. */']
    tab = []
    for raum, mid, off, k, t, art, beleg in FAELLE:
        a_off, m = F.rdt_msg(raum, mid)
        if a_off != off:
            fehler.append('ROOM%04X msg %d @0x%05X statt @0x%05X' % (raum, mid, a_off, off))
            continue
        kopf, text, schwanz = zerlegen(m)
        alt = F.zeilen_worte(text)
        ws = [w for z in alt for w in z]
        if len(alt) <= 2:
            fehler.append('ROOM%04X msg %d hat nur %d Zeilen' % (raum, mid, len(alt)))
        name = F.wort_str(kopf[4:-3]) if len(kopf) > 2 else ''
        vorweg = (F.breite(kopf[4:-2]) + T.WID[F.LEER]) if name else 0
        if art == 'pause' or art == 'wort':
            n = LS.seitenwartezeit(t, 0)
            wie = '%s t=%.2f s (%s %s): %s' % ('Pausenende' if art == 'pause' else 'Wortanfang', t,
                                              S % (('%X' % raum)[0], '%04X' % raum, mid),
                                              'Whisper medium.en', beleg)
        elif art == 'ohne':
            n, nb = nachlauf_r42.nachlauf(raum, 2)
            wie = '%s; Seite 1 steht wie die Original-Zeilen: %s' % (beleg, nb)
        else:
            n, wie = 0, beleg
        try:
            seiten = L5.seiten(ws, k, vorweg, bool(name))
        except AssertionError as e:
            fehler.append('ROOM%04X msg %d: %s' % (raum, mid, e))
            continue
        b, zeilen, bw = L5.bauen(kopf, seiten, schwanz, n, F.LEER)
        if [w for s in seiten for z in s for w in z] != ws:
            fehler.append('ROOM%04X msg %d: Wortlaut weicht ab' % (raum, mid))
        for si, (zs, ww) in enumerate(zip(zeilen, bw)):
            if len(zs) > LS.MAX_ZEILEN or max(ww) > T.MAX_PX:
                fehler.append('ROOM%04X msg %d Seite %d: %d Zeilen, %d px' % (raum, mid, si + 1, len(zs), max(ww)))
        nn = 'k_zz_%04x_msg%d' % (raum, mid)
        hinweis = '' if len(b) <= L5.PSX_LEN else '; PSX: %d B > %d (MSG_RAW_LEN, PSX-Bau nicht ausgeliefert)' % (len(b), L5.PSX_LEN)
        art_t = {'pause': 'zwei Seiten, Seite 2 nach `02 %d`' % n, 'wort': 'zwei Seiten, Seite 2 nach `02 %d`' % n,
                 'ohne': 'zwei Seiten, Seite 2 nach `02 %d`' % n, 'system': 'zwei Seiten, Seite 2 nach der Taste `02 00`'}[art]
        out += T.carr(nn, b, 'ROOM%04X %d %s%s  (%d B, Zeilen %s px; RDT @0x%05X, Schwanz `%s` aus dem RDT; %s: %s%s)' % (
            raum, mid, (name + ': ') if name else ('System: ' if kopf == [0x04, 0x02] else 'Erzaehler: '),
            ' // '.join(' / '.join(z) for z in zeilen), len(b), ' // '.join('/'.join(map(str, w)) for w in bw), off,
            ' '.join('%02x' % x for x in schwanz), art_t, wie, hinweis))
        tab.append((raum, mid, nn))
        print('ROOM%04X msg %2d %3d B  %s' % (raum, mid, len(b), ' // '.join(' / '.join(z) for z in zeilen)))
    out.append('typedef struct { uint16_t raum; uint8_t id; const uint8_t *b; uint16_t n; } zz_text_t;')
    out.append('static const zz_text_t k_zz_texte[] = {')
    for raum, mid, nn in tab:
        out.append('    { 0x%04X, %d, %s, (uint16_t)sizeof %s },' % (raum, mid, nn, nn))
    out.append('};')
    out.append('#define ZZ_TEXTE_N ((int)(sizeof k_zz_texte / sizeof k_zz_texte[0]))')
    if fehler:
        sys.exit('\n'.join(fehler))
    text = '\n'.join(out) + '\n'
    if '--check' in sys.argv:
        alt = open(ZIEL, encoding='utf-8').read() if os.path.exists(ZIEL) else ''
        print('gleich' if alt == text else 'VERALTET')
        sys.exit(0 if alt == text else 1)
    with open(ZIEL, 'w', encoding='utf-8', newline='\n') as f:
        f.write(text)
    print('geschrieben:', ZIEL)


if __name__ == '__main__':
    main()
