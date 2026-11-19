#!/usr/bin/env python3
"""Runde 42 Strang "namen", Stufe 5: hoechstens zwei Zeilen je Seite fuer ALLE Untertitel - gemeinsame Regeln und Bausteine.

ENTSCHEID (Orchestrator, Regel "Original oder nicht", keine Nutzerfrage): die Disc-Regel aus Stufe 4 gilt fuer jeden Text, den das
Spiel im Nachrichtenfeld zeigt (RDT-Texte der Mod und alle Port-Texte). Belege (lesbar_s4.py): Zeilenwechsel der echten 40%-Disc
y = 180 + 16 fest (@0x8002899c/@0x800289a0/@0x800289ac), 0 von 700 Seiten mit drei Zeilen; Letterbox-Balken ab y 216 (@0x80072f06).
Nicht betroffen: die CHECK-Beschreibungen im Inventar (eigenes Feld, Stift y = 0xA0, menu_common.c msg_open(.., 0x13, 0xa0); die
Bank DEBUG.BIN @0x800c50de der Mod hat dort selbst 25 Seiten mit drei Zeilen) und der Akten-Leser (eigene Seiten).

ZEILEN (wie lesbar_s4.zeilen): passt die Seite in eine Zeile (<= 271 px, Zeile 1 mit dem Namenskopf), dann eine Zeile; sonst die
Zweiteilung, deren breitere Zeile am schmalsten ist, jede Zeile mit mindestens zwei Woertern. PORT-WAHL (Darstellung): eine
einzeilige Seite mit Namenskopf darf ein einziges Wort tragen ("Ada: Underground..." - zwei Worte auf dem Schirm).

SEITEN (passt der Text nicht in zwei Zeilen): zwei Seiten EINER Nachricht, Wortlaut unveraendert.
  Untertitel  `04 00 [05 F Name: 05 00 00] <Seite 1> 04 01 02 N 04 00 [05 F Name: 05 00 00] <Seite 2> <Schwanz>` (Form Stufe 1/4,
              Vorbild ROOM3060 msg 7 @0x1244 `04 01 02 3c 04 00`); der Namenskopf steht auf JEDER Seite, der Erzaehler und
              Systemtexte bleiben ohne Namen. Seite 2 erscheint N + 1 Bilder nach Seite 1 (unit_r42_namen_seitenwechsel).
  Systemtext  `04 02 <Seite 1> 02 00 <Seite 2> 01 00` - Seite 2 nach der Aktionstaste (Form der Speicherstellen-Texte "`04 02` Satz1
              `02 00` Satz2", re15_savepoint.c; FUN_80028134 Zustand 1, msg_common.c `02 00` -> warten auf die Taste).
TRENNSTELLE und N (gemessen, Werkzeuge im Strang-Ordner namen/s5):
  1. mit Stimme: an einer Sprechpause der Aufnahme (pausen5.py: >= 120 ms unter max(-40 dB, Grundrauschen + 8 dB); die -40-dB-Regel
     aus Stufe 4 bleibt fuer leise Aufnahmen gleich), die Wortgrenze aus Whisper medium.en auf dem Stueck vor der Pausenmitte
     (asr_pausen.py); unter den passenden Pausen die, an der die kuerzere Seite am laengsten steht; N = round(30 * Pausenende).
  2. mit Stimme, keine passende Pause: Wortgrenze hinter einem Satzzeichen, sonst an einer Satzteilgrenze (vor "and"/"in"/"for"/
     einer Apposition; PORT-WAHL Lesbarkeit - seit Runde 2, vorher "die, an der die kuerzere Seite am laengsten steht", Pruefbericht
     s5_r1 G2), t = Whisper-Wortanfang des ersten Worts von Seite 2; N = round(30 * t).
  Passt der Text an keiner Pause in zwei Seiten (ROOM1240/1241 msg 4), dann drei Seiten an zwei gemessenen Pausen; Seite k+1 steht
     N_k + 1 Bilder nach Seite k, also N_2 = round(30 * t_2) - N_1 - 1 (Seite 3 beim Ende der zweiten Pause).
  3. ohne Stimme: Seite 1 steht so lange wie die Original-Untertitel desselben Raums (nachlauf_r42.nachlauf(raum, 2)); Trennung hinter
     einem Satzzeichen, sonst die ausgeglichenste.
"""
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, '..', 'r42_story'))
import texte_stage3a as T  # noqa: E402
import lesbar_s4 as LS  # noqa: E402

PSX_LEN = 128      # PSX MSG_RAW_LEN (msg_common.c:43); der PSX-Bau wird nicht ausgeliefert
LEER, UMBRUCH = 0x00, 0x08


def _breite(glyphen):
    return sum(T.WID[g] for g in glyphen)


def _zeile(ws):
    g = []
    for i, w in enumerate(ws):
        if i:
            g.append(LEER)
        g += w
    return g


def zeilen(woerter, vorweg, mit_name):
    """Glyphen-Woerter einer Seite -> 1 oder 2 Zeilen (Listen von Woertern); AssertionError, wenn es nicht passt."""
    if vorweg + _breite(_zeile(woerter)) <= T.MAX_PX and (len(woerter) >= 2 or mit_name):
        return [list(woerter)]
    best = None
    for i in range(2, len(woerter) - 1):
        wa, wb = vorweg + _breite(_zeile(woerter[:i])), _breite(_zeile(woerter[i:]))
        if wa <= T.MAX_PX and wb <= T.MAX_PX and (best is None or max(wa, wb) < best[0]):
            best = (max(wa, wb), [list(woerter[:i]), list(woerter[i:])])
    assert best, 'passt nicht in %d Zeilen' % LS.MAX_ZEILEN
    return best[1]


def seiten(ws, k, vorweg, mit_name):
    """k = Woerter auf Seite 1 (int), mehrere Seiten: Tupel der Schnittstellen (Runde 2, ROOM1240/1241 msg 4 an zwei Pausen)."""
    if k is None:
        return [zeilen(ws, vorweg, mit_name)]
    ks = [0] + (list(k) if isinstance(k, (tuple, list)) else [k]) + [len(ws)]
    return [zeilen(ws[a:b], vorweg, mit_name) for a, b in zip(ks, ks[1:])]


def _rev(g):
    if 0x1D <= g <= 0x36:
        return chr(ord('A') + g - 0x1D)
    if 0x3D <= g <= 0x56:
        return chr(ord('a') + g - 0x3D)
    if 0x0C <= g <= 0x15:
        return chr(ord('0') + g - 0x0C)
    r = {v: k for k, v in T.TAB.items()}
    r[0x57] = '.'
    r[0x61] = '"'
    return r.get(g, '<%02x>' % g)


def bauen(kopf, seiten_, schwanz, n, leer=LEER):
    """kopf = `04 00 05 F Name: 05 00` (Untertitel) | `04 00` (Erzaehler) | `04 02` (Systemtext).
    -> (Bytes, [[Zeilentext je Zeile] je Seite], [[px je Zeile] je Seite])."""
    system = list(kopf) == [0x04, 0x02]
    mit_name = len(kopf) > 2
    vorweg = (_breite(kopf[4:-2]) + T.WID[leer]) if mit_name else 0
    b, zz, bw = [], [], []
    ns = list(n) if isinstance(n, (tuple, list)) else [n] * max(1, len(seiten_) - 1)   # je Seitenwechsel ein N (Runde 2)
    for si, zs in enumerate(seiten_):
        if si:
            b += [0x02, 0x00] if system else [0x04, 0x01, 0x02, ns[si - 1]]
        if si == 0 or not system:
            b += list(kopf) + ([leer] if mit_name else [])
        for zi, z in enumerate(zs):
            if zi:
                b.append(UMBRUCH)
            b += _zeile(z)
        zz.append([' '.join(''.join(_rev(g) for g in w) for w in z) for z in zs])
        bw.append([(vorweg if zi == 0 else 0) + _breite(_zeile(z)) for zi, z in enumerate(zs)])
    assert all(1 <= x <= 255 for x in ns) or system or len(seiten_) == 1, ns
    return b + list(schwanz), zz, bw


# ---- Port-Texte (Strings) ------------------------------------------------------------------------------------------------------
# (Raum, Id) -> (N, Beleg). Seite 2 der Port-Texte, die bisher drei Zeilen hatten; die Trennstelle steht als '|' im Text des
# jeweiligen Werkzeugs (Wortlaut sonst unveraendert). N = round(30 * Pausenende) bzw. Nachlauf der Original-Zeilen des Raums;
# None = Systemtext, Seite 2 nach der Taste `02 00`.
PORT_SEITEN = {
    (0x3060, 24): (60, 'keine Aufnahme (synchro/STAGE3/room3060/main24.wav fehlt): Seite 1 steht wie die Original-Untertitel des Raums, '
                       'Nachlauf 0x3c = ROOM3060 2-zeilig 5 von 5 (2, 3, 5, 7, 8), z. B. msg 2 `04 01 01 3c` @0x1168'),
    (0x30E0, 11): (63, 'synchro/STAGE3/room30E0/main11.wav Pause 1.25-2.11 (VOR: "The hall is no good." - die Aufnahme spricht den '
                       'alten Mod-Satz, Satzgrenze an derselben Stelle)'),
    (0x30E0, 13): (111, 'synchro/STAGE3/room30E0/main13.wav Pause 3.18-3.69 (VOR: "The entire place is overrun." - alter Mod-Satz, '
                        'Ende des ersten Satzes)'),
    (0x6031, 1): (100, 'synchro/STAGE6/room6031/main01.wav Pause 2.87-3.33 (VOR: "A policeman! He\'s fighting off monsters!" - alter '
                       'Mod-Satz; von zwei Pausen die, an der die kuerzere Seite am laengsten steht: 3,18/1,58 s gegen 1,24/3,52 s)'),
    (0x6031, 13): (99, 'keine Aufnahme (main13.wav fehlt): Nachlauf 0x63 = ROOM6031 2-zeilig 5 von 5 (1, 5, 6, 9, 10), '
                       'z. B. msg 1 `04 01 01 63` @0x11AF'),
    (0x2081, 50): (99, 'keine Aufnahme (main50.wav fehlt): Nachlauf 0x63 = ROOM3001 (Belegraum, nachlauf_r42.py) 2-zeilig 4 von 4, '
                       'z. B. msg 4 `04 01 01 63` @0x192C'),
    (0x6030, 30): (76, 'synchro/STAGE6/room6030/main30.wav Pause 2.09-2.54 (VOR: "No. She\'s my daughter.")'),
}
# Systemtexte (Seite 2 nach der Taste `02 00`, Seitenform ROOM2080 msg 0 @0x0CF8 / ROOM2060 msg 0 @0x1766 derselben Stage, Speicherstellen
# re15_savepoint.c): ROOM2030 msg 46 "WARNING: ..." (tools/r41_ventilkette/gen_r41_ventilkette.py), ROOM2030/2031 Pult ohne Mutter
# (tools/r42_gegenstaende/texte.py OHNE), ROOM5060/5061 msg 5 (zweizeilig_s5.py).
SYSTEM_BELEG = 'Seite 2 nach der Taste `02 00` (Seitenform ROOM2080 msg 0 @0x0CF8, ROOM2060 msg 0 @0x1766)'


def text_seiten(text, kod, kopf, schwanz, n, leer=LEER):
    """Port-Text mit '|' als Seitenwechsel -> (Bytes, Zeilen je Seite, px je Seite). kopf wie bauen(); jede Seite <= 2 Zeilen."""
    mit_name = len(kopf) > 2
    vorweg = (_breite(kopf[4:-2]) + T.WID[leer]) if mit_name else 0
    seiten_ = [zeilen([kod(w) for w in s.strip().split(' ')], vorweg, mit_name) for s in text.split('|')]
    return bauen(kopf, seiten_, schwanz, n if n is not None else 0, leer)
