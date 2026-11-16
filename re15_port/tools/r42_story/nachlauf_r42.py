#!/usr/bin/env python3
"""Runde 42 Story, Pruefhinweis H4: Nachlauf der neuen Story-Untertitel aus den Original-Zeilen DESSELBEN Raums.

    python re15_port/tools/r42_story/nachlauf_r42.py      (druckt die Belege je Raum)

Ein Untertitel endet mit `04 01 01 N` (msg_common.c re15_msg_compute_duration: `01 N` = N Bilder Nachlauf nach dem Text, Zustand 6
des Typewriters FUN_80028134). Die Port-Zeilen trugen bisher fest N = 0x63 (texte_stage3a.py:132 ohne Beleg). Jetzt kommt N aus den
Original-Untertiteln (Nachrichtenblock @RDT+0x3C, Kopf `04 00`) des Raums:

  1. Belegraum = der Raum selbst, wenn er Original-Untertitel hat (BELEGRAUM unten fuer die uebrigen).
  2. Hat der Belegraum Untertitel mit DERSELBEN Zeilenzahl wie die neue Zeile, gilt deren Mehrheit (strikt), sonst die Mehrheit aller
     Untertitel des Belegraums. Ein Gleichstand ist ein Fehler (kein Raten).
     Gemessen (Zensus aller 240 RDTs, kk_NOTIZ.md): ROOM3060 8 x `01 3c`, ROOM30E0 14 x `01 63`, ROOM3070 5 x `01 63`;
     ROOM3020 gemischt: einzeilig msg 0 @0x0E45 / msg 2 @0x0EB9 `04 01 01 59`, zweizeilig msg 1 @0x0E92 `04 01 01 63`
     (Offsets = Beginn des Schwanzes `04 01 01 N`, wie ROOM3060 msg 1 @0x1131 im Pruefhinweis H4).
  3. Raeume ohne eigene Original-Untertitel (nur Systemtexte `04 02 .. 01 00`): ROOM11A0/11A1 und ROOM2080/2081. Belegraum ist der
     naechste Raum derselben Begleiter-Kette mit Original-Untertiteln derselben Sprecher: 2080 -> 11A0 -> Leiter -> ROOM3000
     (Leon/Marvin, 7 x `01 63`), 2081 -> 11A1 -> Leiter -> ROOM3001 (Elza/John/Sherry, einzeilig 3 x `01 63` gegen 1 x `01 32`,
     zweizeilig 4 x `01 63`). Konzept 2 / K-07: die Szene vor der Leiter endet damit, dass die Begleiter als Erste hinaufgehen,
     "deshalb sind sie in 3000/3001 schon oben".
  4. ABWAEGUNG gegen die naeher liegenden Raeume derselben Stage (Nachbesserung Kleinkram R1, Pruefer-Hinweis; Zahlen druckt
     `python nachlauf_r42.py`, Abschnitt ABWAEGUNG, aus den RDTs):
     - Elza-Seite: ROOM2001 (vor 2081, Elza/John/Sherry) 15 x `01 63`, ROOM11B1 (Elza/Sherry) 11 x `01 63`, ROOM3001 wie oben - jeder
       Kandidat ergibt 0x63. Die Wahl des Belegraums aendert dort nichts.
     - Leon-Seite: ROOM2000 (Leon/Marvin/Ada, Eingang der Kanalisation) zweizeilig 6 x `01 77` / 2 x `01 63`, einzeilig 4 x `01 63`;
       ROOM11B0 (Leon/Marvin am Kanaldeckel) sechs verschiedene Werte 0x3b..0x87 fuer 13 Zeilen. Beide Raeume setzen N je Zeile
       verschieden (2000: `01 77` an den sechs laengsten zweizeiligen Zeilen msg 2/4/7/8/9/11, `01 63` an den kuerzeren msg 5/6;
       11B0: einzeilig 0x3b/0x55/0x5f, zweizeilig 0x5f/0x69, dreizeilig 0x69/0x87). Eine Regel dafuer steht in keinem Code - N ist
       Datenbyte je Zeile (msg_common.c re15_msg_compute_duration, FUN_80028134 Zustand 6) -, eine Uebertragung auf neue Zeilen
       waere geraten. (Je Zeile verschieden sind auch ROOM10D0, 1150 und 11C0 in STAGE1.) Die Begleiter-Kette ab der Trennung ist
       dagegen gleichfoermig (ROOM3000 7 x 0x63, ROOM30E0 14 x 0x63, ROOM3070 5 x 0x63).
     - Spiegel: 2080/2081 und 11A0/11A1 sind Szene fuer Szene gespiegelt (2080/2081 msg 43..45 bis auf die Namen gleich, 11A0/11A1 msg 5 wortgleich). Die Elza-Seite
       ist eindeutig 0x63; mit ROOM2000/11B0 als Leon-Beleg bekaemen wortgleiche Zeilen je Figur verschiedene Nachlaeufe.
     - Geschichte: 2080 -> 11A0 -> Leiter -> 3000 ist der Weg DIESER Szenen (K-07: die Unterhaltung in 11A0 geht in 3000 weiter);
       11B0 (vor dem Abstieg) und 2000 (die Trennung am Kanaleingang, "better to separate") liegen davor, beide vor dem Alligator.
     Ergebnis: Belegraum bleibt ROOM3000/ROOM3001 (Werte unveraendert 0x63).
"""
import collections
import os
import struct

HIER = os.path.dirname(os.path.abspath(__file__))
PSX = os.path.normpath(os.path.join(HIER, '..', '..', 'shared_assets', 'PSX'))

# Raum ohne eigene Original-Untertitel -> Belegraum (Begruendung im Kopf, Punkt 3)
BELEGRAUM = {0x11A0: 0x3000, 0x11A1: 0x3001, 0x2080: 0x3000, 0x2081: 0x3001}
_STEUER2 = (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B)   # Steuerbytes mit einem Operanden (tools/rdt_msgdump.py decode)


def _rdt(raum):
    return open(os.path.join(PSX, 'STAGE%X' % (raum >> 12), 'ROOM%04X.RDT' % raum), 'rb').read()


def original_untertitel(raum):
    """[(msg, Zeilen, N, Datei-Offset des Bytes N)] aller Original-Untertitel (Kopf 04 00) des Raums."""
    d = _rdt(raum)
    ms = struct.unpack_from('<I', d, 0x3C)[0]
    if ms == 0 or ms >= len(d):
        return []
    n = struct.unpack_from('<H', d, ms)[0] // 2
    tab = [struct.unpack_from('<H', d, ms + 2 * i)[0] for i in range(n)]
    out = []
    for mid, o in enumerate(tab):
        a = ms + o
        e = ms + tab[mid + 1] if mid + 1 < n else len(d)
        if d[a:a + 2] != b'\x04\x00':
            continue
        i, zeilen = a, 1
        while i < e:
            c = d[i]
            if c == 0x01:
                out.append((mid, zeilen, d[i + 1], i + 1))
                break
            if c in _STEUER2:
                i += 2
                continue
            if c == 0x08:
                zeilen += 1
            i += 1
    return out


def nachlauf(raum, zeilen):
    """(N, Beleg-Text) fuer eine neue Untertitel-Zeile mit `zeilen` Zeilen in ROOM<raum>."""
    beleg = BELEGRAUM.get(raum, raum)
    orig = original_untertitel(beleg)
    if not orig:
        raise SystemExit('ROOM%04X: kein Belegraum mit Original-Untertiteln' % raum)
    gleich = [x for x in orig if x[1] == zeilen]
    menge = gleich if gleich else orig
    zahl = collections.Counter(x[2] for x in menge).most_common()
    if len(zahl) > 1 and zahl[0][1] == zahl[1][1]:
        raise SystemExit('ROOM%04X (%d Zeilen): Gleichstand im Belegraum ROOM%04X %s' % (raum, zeilen, beleg, zahl))
    n = zahl[0][0]
    stellen = [x for x in menge if x[2] == n]
    text = 'Nachlauf 0x%02x = ROOM%04X%s %s: %d von %d Original-Untertiteln (%s) `01 %02x`, z. B. msg %d `04 01 01 N` @0x%04X' % (
        n, beleg, '' if beleg == raum else ' (Belegraum)', ('%d-zeilig' % zeilen) if gleich else 'alle', len(stellen), len(menge),
        ', '.join('%d' % x[0] for x in menge), n, stellen[0][0], stellen[0][3] - 3)
    return n, text


# Kandidaten der Abwaegung (Punkt 4): Raum ohne eigene Untertitel -> (gewaehlter Belegraum, naeher liegende Raeume derselben Stage)
ABWAEGUNG = {0x2080: (0x3000, (0x2000,)), 0x2081: (0x3001, (0x2001,)), 0x11A0: (0x3000, (0x11B0,)), 0x11A1: (0x3001, (0x11B1,))}


def muster(raum):
    """{Zeilen: Counter(N)} der Original-Untertitel eines Raums."""
    m = collections.defaultdict(collections.Counter)
    for _mid, z, n, _o in original_untertitel(raum):
        m[z][n] += 1
    return m


if __name__ == '__main__':
    print('ABWAEGUNG (Punkt 4):')
    for r, (gew, andere) in ABWAEGUNG.items():
        for k in (gew,) + andere:
            m = muster(k)
            print('  ROOM%04X <- ROOM%04X%s: %s' % (r, k, ' (gewaehlt)' if k == gew else '', '; '.join(
                '%d-zeilig %s' % (z, ', '.join('%d x 0x%02x' % (c, n) for n, c in m[z].most_common())) for z in sorted(m))))
    for r in (0x11A0, 0x11A1, 0x2080, 0x2081, 0x3020, 0x3060, 0x30E0, 0x3070, 0x6030, 0x6031):
        for z in (1, 2, 3):
            try:
                print('ROOM%04X %d Zeilen: %s' % (r, z, nachlauf(r, z)[1]))
            except SystemExit as e:
                print('ROOM%04X %d Zeilen: FEHLER %s' % (r, z, e))
