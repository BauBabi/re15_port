# -*- coding: utf-8 -*-
"""ZUORDNUNG: welcher Raum ist welches GEMALTE Rechteck der Original-Karte?

⛔ WARUM DAS UEBERHAUPT GEHT (gemessen 2026-09-03, analysis/karte_grundriss/BEFUND.md §22):
Die gemalten Rechtecke SIND die Kollisionsboxen der Raeume, gezeichnet im Massstab der
Zeile @0x800768b0. Von den 38 Raeumen mit ausgelieferter Zeile treffen 30 ihr Rechteck zu
mindestens 50 %, vier davon zu 100 % (ROOM3040, ROOM5010, ROOM5100, ROOM50C0).

⛔ WARUM DAS NOETIG IST: die Zeile fehlt fuer 67 von 106 Raeumen (Massstab 1 = alles auf
denselben Punkt, das Kartensystem des Prototyps ist unfertig). Ohne Zuordnung gibt es fuer
diese Raeume weder Spieler-Marker noch rote Hervorhebung auf der Original-Kunst.

⛔ WARUM GROESSE ALLEIN NICHT REICHT: ein erster Wurf ordnete nur nach Groesse zu (mit dem
Blatt-Median als Massstab) und kam auf 55 von 103 Zonen - auf Blatt 0 sogar 0 von 6,
obwohl dort zwei Raeume eine geeichte Zeile haben. Der Massstab ist je RAUM verschieden,
und mehrere Raeume eines Blattes haben aehnliche Groesse. Die Groesse ist ein schwaches
Mass; sie darf nur die Feinauswahl treffen.

DAS TRAGENDE MERKMAL IST DIE NACHBARSCHAFT. Zwei Raeume, die eine Tuer teilen, sind auf
der gemalten Karte benachbart - ihre Rechtecke beruehren sich. Damit ist die Zuordnung
eine GRAPHENABBILDUNG: der Tuergraph der Zonen auf den Beruehrungsgraphen der Rechtecke.
Die geeichten Raeume sind dabei feste Anker, weil ihre Lage direkt ausrechenbar ist.

Aufruf:
    python re15_port/tools/kunst_zuordnung.py            # Uebersicht je Blatt
    python re15_port/tools/kunst_zuordnung.py 4          # ein Blatt im Detail
"""
import collections
import io
import os
import re
import struct
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import karte_audit as A                                    # noqa: E402

# ---- die ausgelieferten Tabellen aus dem verbatim uebernommenen EXE-Bereich ----------
_BLOB_BASE = 0x800762A0


def _blob():
    src = io.open(os.path.join(HIER, '..', 'engine', 'src', 're15_inv_ui_tables.c'),
                  encoding='utf-8', errors='replace').read()
    i = src.index('re15_inv_map_blob[')
    j = src.index('};', i)
    return bytes(int(x, 16) for x in re.findall(r'0x([0-9a-fA-F]{2})', src[i:j]))


BL = _blob()


def _rd(a, n):
    return BL[a - _BLOB_BASE:a - _BLOB_BASE + n]


def _u16(a):
    return struct.unpack_from('<H', _rd(a, 2))[0]


def _s16(a):
    return struct.unpack_from('<h', _rd(a, 2))[0]


def _u32(a):
    return struct.unpack_from('<I', _rd(a, 4))[0]


# Globaler Raum-Slot = Stage-Basis + Raumindex (re15_inv_screen.c:273-320, aus den
# sechs Init-Funktionen FUN_8004b568 ff. gelesen).
SLOT_BASIS = {1: 0, 2: 38, 3: 50, 4: 65, 5: 77, 6: 98}


def zeile(rid):
    """Die ausgelieferte Massstabszeile eines Raums, oder None wenn Stub."""
    slot = SLOT_BASIS[(rid >> 12) & 0xF] + ((rid >> 4) & 0xFF)
    if slot > 105:
        return None
    a = 0x800768B0 + slot * 8
    z = (_s16(a), _u16(a + 2), _u16(a + 4), _u16(a + 6))
    return z if z[2] > 1 and z[3] > 1 else None


def projiziere(z, wx, wz):
    """Original-Formel FUN_800473f8 @0x8004741c-0x80047528."""
    xo, yo, xs, zs = z
    t = ((wx + 32000) * 10 * xs) >> 20
    mx = (t + 5) // 10 + xo
    t2 = -(((( wz + 32000) * 10 * zs) >> 20) + 5)
    my = (t2 // 10 if t2 >= 0 else -((-t2) // 10)) + yo
    return mx, my


def rechtecke(pg):
    """Die gemalten Rechtecke einer Kartenseite (Paar-Tabelle @0x80076840 + 8*Seite)."""
    cnt = _u16(0x80076840 + pg * 8)
    lp = _u32(0x80076844 + pg * 8)
    return [(k,) + struct.unpack_from('<HHHH', _rd(lp + k * 12, 12), 0)
            for k in range(cnt)]


def _beruehrt(a, b, rand=2):
    """Zwei Rechtecke sind benachbart, wenn sie sich beruehren oder ueberlappen."""
    _, ax, ay, aw, ah = a
    _, bx, by, bw, bh = b
    return (min(ax + aw, bx + bw) - max(ax, bx) >= -rand and
            min(ay + ah, by + bh) - max(ay, by) >= -rand)


def zuordnung(pg, laut=False):
    """Zonen dieses Blattes auf gemalte Rechtecke abbilden.

    Rueckgabe: (zu, zonen, rs) mit zu = {(raum, zone): rechteck-index}.
    """
    zonen = [z for z in A.ZONEN
             if z['page'] == pg and not (z['room'] & 1) and not z['etage']]
    # Gast-Zeilen zaehlen mit, wenn sie auf DIESEM Blatt liegen: sie werden dort
    # gezeichnet und brauchen dort ein Rechteck.
    zonen += [z for z in A.ZONEN
              if z['page'] == pg and not (z['room'] & 1) and z['etage']]
    rs = rechtecke(pg)
    if not rs or not zonen:
        return {}, zonen, rs

    # ---- Anker: Raeume mit ausgelieferter Zeile werden DIREKT projiziert ----------
    zu = {}
    belegt = set()
    anker = []
    for z in zonen:
        r = zeile(z['room'])
        if not r:
            continue
        a = projiziere(r, z['wx0'], z['wz0'])
        b = projiziere(r, z['wx1'], z['wz1'])
        x0, x1 = min(a[0], b[0]), max(a[0], b[0])
        y0, y1 = min(a[1], b[1]), max(a[1], b[1])
        best = None
        for rc in rs:
            _, rx, ry, rw, rh = rc
            ox = min(x1, rx + rw) - max(x0, rx)
            oy = min(y1, ry + rh) - max(y0, ry)
            if ox <= 0 or oy <= 0:
                continue
            anteil = (ox * oy) / float(max(1, (x1 - x0) * (y1 - y0)))
            if best is None or anteil > best[0]:
                best = (anteil, rc[0])
        if best and best[0] >= 0.4 and best[1] not in belegt:
            zu[(z['room'], z['idx'])] = best[1]
            belegt.add(best[1])
            anker.append((z['room'], z['idx']))
    if laut:
        print("   Anker (geeichte Zeile, direkt projiziert): %d" % len(anker))

    # ---- Massstab fuer die Stub-Raeume: Median der Anker dieses Blattes ----------
    sk = [zeile(z['room'])[2] for z in zonen if zeile(z['room'])]
    if not sk:
        # ⛔ BLATT OHNE ANKER (Blatt 3): der Massstab ist auf diesem Blatt nirgends
        # ausgeliefert. Ein Median der NACHBARBLAETTER waere geraten - hier wird
        # deshalb NICHTS zugeordnet, und das Blatt bleibt ehrlich leer.
        if laut:
            print("   KEIN Anker auf diesem Blatt - keine Zuordnung moeglich")
        return zu, zonen, rs
    sk.sort()
    SX = sk[len(sk) // 2]

    def px(w):
        return (((w * 10 * SX) >> 20) + 5) // 10

    # ---- Tuergraph der Zonen ----------------------------------------------------
    nb = collections.defaultdict(set)
    for z in zonen:
        for d in A.RDT.get(z['room'], ((), ()))[1]:
            if d['rw'] == 0 and d['rd'] == 0:
                continue
            nb[z['room']].add(d['dest'])
            nb[d['dest']].add(z['room'])

    # ---- Ausbreiten: ein Stub-Raum bekommt ein freies Rechteck, das ein bereits
    # zugeordnetes NACHBAR-Rechteck beruehrt. Die Groesse entscheidet nur noch
    # zwischen den so uebrig bleibenden Kandidaten.
    for _runde in range(6):
        vorschlag = []
        for z in zonen:
            if (z['room'], z['idx']) in zu:
                continue
            pw, ph = px(z['wx1'] - z['wx0']), px(z['wz1'] - z['wz0'])
            for rc in rs:
                if rc[0] in belegt:
                    continue
                # Wieviele meiner schon gesetzten Nachbarn beruehrt dieses Rechteck?
                treffer = 0
                for (rm, zi), kk in zu.items():
                    if rm in nb[z['room']] and _beruehrt(rc, rs[kk]):
                        treffer += 1
                if treffer == 0:
                    continue
                dist = abs(rc[3] - pw) + abs(rc[4] - ph)
                vorschlag.append((-treffer, dist, z['room'], z['idx'], rc[0]))
        if not vorschlag:
            break
        vorschlag.sort()
        # ⛔ OHNE ALTERNATIVE IST DIE GROESSE KEINE FRAGE MEHR. Gibt es fuer eine Zone
        # genau EIN freies Rechteck, das einen ihrer gesetzten Nachbarn beruehrt, dann
        # ist die Zuordnung erzwungen - egal wie gut die Groesse passt. Der
        # Groessenabstand ist ohnehin nur ein schwaches Mass (der Massstab ist je RAUM
        # verschieden, hier steht nur der Blatt-Median zur Verfuegung); ihn ueber eine
        # erzwungene Zuordnung entscheiden zu lassen, verwirft richtige Treffer.
        # Gemessen: 25 Zonen blieben allein an der Schranke dist > 24 haengen.
        einzig = collections.Counter((rm, zi) for _t, _d, rm, zi, _k in vorschlag)
        gesetzt = 0
        for negt, dist, rm, zi, k in vorschlag:
            if (rm, zi) in zu or k in belegt:
                continue
            if dist > 24 and einzig[(rm, zi)] > 1:
                continue
            zu[(rm, zi)] = k
            belegt.add(k)
            gesetzt += 1
        if not gesetzt:
            break
    return zu, zonen, rs


def main(argv):
    if len(argv) > 1:
        pg = int(argv[1])
        zu, zonen, rs = zuordnung(pg, laut=True)
        print("Blatt %d: %d Zonen, %d gemalte Rechtecke" % (pg, len(zonen), len(rs)))
        for z in zonen:
            k = zu.get((z['room'], z['idx']))
            rc = rs[k] if k is not None else None
            print("   ROOM%04X z%d %-6s -> %s"
                  % (z['room'], z['idx'], "(Gast)" if z['etage'] else "",
                     ("Nr%d %dx%d @(%d,%d)" % (rc[0], rc[3], rc[4], rc[1], rc[2]))
                     if rc else "KEINES"))
        frei = [rc[0] for rc in rs if rc[0] not in set(zu.values())]
        print("   Rechtecke ohne Zone: %s" % (frei or "keine"))
        return 0

    gz = go = gr = 0
    for pg in range(13):
        zu, zonen, rs = zuordnung(pg)
        if not zonen:
            continue
        frei = len(rs) - len(set(zu.values()))
        gz += len(zu); go += len(zonen) - len(zu); gr += frei
        print("Blatt %-2d  %2d/%2d Zonen zugeordnet, %d von %d Rechtecken frei"
              % (pg, len(zu), len(zonen), frei, len(rs)))
    print("\nGESAMT: %d Zonen verortet, %d ohne Rechteck, %d Rechtecke ohne Zone"
          % (gz, go, gr))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
