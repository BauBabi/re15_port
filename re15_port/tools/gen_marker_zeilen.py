# -*- coding: utf-8 -*-
"""Die fehlenden MARKER-ZEILEN aus der Tuerkette herleiten.

⛔ WAS FEHLT UND WARUM: die Tabelle @0x800768b0 setzt die lokalen Koordinaten EINES Raums
auf die gemalte Karte ({x_off, y_off, x_scale, z_scale}, Formel FUN_800473f8
@0x8004741c-0x80047528). Von 106 Slots tragen nur 39 eine echte Zeile; die uebrigen haben
Massstab 1, und damit bildet die Formel jede Weltposition auf denselben Punkt ab - in
diesen Raeumen weiss das Original nicht, wo der Spieler auf seiner eigenen Karte steht.
Das Kartensystem des Prototyps ist an dieser Stelle unfertig.

⛔ ES GIBT KEINEN GEMEINSAMEN WELTRAUM. Gemessen 2026-09-03 (BEFUND.md §26): die Zeilen
der geeichten Raeume EINES Blattes beschreiben verschiedene Abbildungen - auf Blatt 7
streut a = 0,00187..0,00238 und b = 118..270. RE1.5-Raumkoordinaten sind RAUMLOKAL, jede
RDT hat ihren eigenen Ursprung. Man kann einen fehlenden Raum also nicht einfach in eine
blattweite Abbildung hineinprojizieren - jede Zeile ist die Platzierung genau dieses Raums.

DIE TUER IST DER GEMEINSAME PUNKT. Sie ist derselbe Ort in ZWEI lokalen Systemen: Raum A
kennt sie bei (ax,az), Raum B bei (bx,bz). Ist As Zeile bekannt, ist der Kartenpunkt der
Tuer ausrechenbar - und Bs Zeile muss (bx,bz) genau dorthin abbilden.

    EINE Tuer  -> der Versatz ist bestimmt, der Massstab muss geliehen werden.
    ZWEI Tueren zu bekannten Nachbarn -> zwei Punkte, zwei Unbekannte je Achse:
        sx = 2^20 * (P1x - P2x) / (b1x - b2x)
        ox = P1x - (b1x + 32000) * sx / 2^20
      also Versatz UND Massstab bestimmt.

⛔ DER RIEGEL IST DIE AUSLASSPROBE. Fuer jeden Raum, der eine ausgelieferte Zeile HAT,
wird so getan, als fehlte sie; die Zeile wird hergeleitet und mit der echten verglichen.
Ohne diese Probe waere die Herleitung eine Behauptung. Gemessen (gleiches Blatt, keine
Selbst-Tuer, beide Raeume einzonig): Median 6 px, 21 von 32 innerhalb 8 px, die besten
exakt.

Aufruf:
    python re15_port/tools/gen_marker_zeilen.py            # Auslassprobe + Uebersicht
    python re15_port/tools/gen_marker_zeilen.py --schreiben  # Header erzeugen
"""
import collections
import io
import os
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import karte_audit as A                                    # noqa: E402
import kunst_zuordnung as K                                # noqa: E402

# Massstaebe der ausgelieferten Zeilen: 1200..3200. Ein hergeleiteter Wert ausserhalb
# dieses Bandes ist ein Rechenartefakt (zu nah beieinanderliegende Tueren), kein Ergebnis.
SKALA_MIN, SKALA_MAX = 1200, 3300


def karte_x(w, ox, sx):
    return ((((w + 32000) * 10 * sx) >> 20) + 5) // 10 + ox


def karte_y(w, oy, sz):
    t = -(((((w + 32000) * 10 * sz) >> 20)) + 5)
    return (t // 10 if t >= 0 else -((-t) // 10)) + oy


def _blatt_und_zonen():
    blatt, zonen_n = {}, collections.Counter()
    for z in A.ZONEN:
        if z['room'] & 1 or z['etage']:
            continue
        blatt[z['room']] = z['page']
        zonen_n[z['room']] += 1
    return blatt, zonen_n


BLATT, ZONEN_N = _blatt_und_zonen()


def tueren(a):
    """Installierte Tueren eines Raums (inerte Aot_set mit Nullflaeche fallen weg)."""
    return [d for d in A.RDT.get(a, ((), ()))[1]
            if not (d['rw'] == 0 and d['rd'] == 0)]


def gegentuer(a, b):
    """Der Datensatz in B, der zurueck nach A fuehrt."""
    for d in tueren(b):
        if d['dest'] == a:
            return d
    return None


def _paar_punkte(b, bekannt):
    """Fuer Raum b: Liste (lokaler Tuerpunkt in b, Kartenpunkt) ueber bekannte Nachbarn."""
    aus = []
    for d in tueren(b):
        a = d['dest']
        if a == b or a not in bekannt:
            continue
        if BLATT.get(a) is None or BLATT.get(a) != BLATT.get(b):
            continue                       # ueber Blattgrenzen gibt es keinen Punkt
        if ZONEN_N[a] != 1 or ZONEN_N[b] != 1:
            continue                       # bei Mehrzonen-Raeumen ist unklar, welcher
        da = gegentuer(b, a)
        if not da:
            continue
        za = bekannt[a]
        P = (karte_x(da['lx'], za[0], za[2]), karte_y(da['lz'], za[1], za[3]))
        aus.append(((d['lx'], d['lz']), P, a))
    return aus


def herleiten(b, bekannt):
    """Zeile fuer b herleiten. Rueckgabe (zeile, guete) oder (None, grund)."""
    pk = _paar_punkte(b, bekannt)
    if not pk:
        return None, 'kein bekannter Nachbar auf dem Blatt'

    # --- DIE ACHSEN GETRENNT LOESEN ----------------------------------------------
    # ⛔ Ein erster Wurf verlangte von EINEM Tuerpaar Abstand in BEIDEN Achsen. Zwei
    # Tueren an derselben Wand unterscheiden sich aber nur in EINER - damit fiel der
    # Zwei-Tueren-Weg fast immer aus (1 von 96 Raeumen), und fast ueberall musste der
    # Massstab geliehen werden, was der groesste Fehleranteil ist. x und z sind
    # unabhaengige Gleichungen; sie duerfen aus VERSCHIEDENEN Paaren kommen.
    def loese(achse):
        best = None
        for i in range(len(pk)):
            for j in range(i + 1, len(pk)):
                (b1, P1, a1), (b2, P2, a2) = pk[i], pk[j]
                d = b1[achse] - b2[achse]
                if abs(d) < 3000:
                    continue               # zu nah: die Division wird beliebig
                dp = P1[achse] - P2[achse]
                s = int(round(1048576.0 * (dp if achse == 0 else -dp) / d))
                if not (SKALA_MIN <= s <= SKALA_MAX):
                    continue
                if best is None or abs(d) > best[0]:
                    best = (abs(d), s, a1, a2)
        return best

    lx, lz = loese(0), loese(1)
    za = bekannt[pk[0][2]]
    sx = lx[1] if lx else za[2]
    sz = lz[1] if lz else za[3]
    b1, P1, a1 = pk[0]
    ox = P1[0] - karte_x(b1[0], 0, sx)
    oy = P1[1] - karte_y(b1[1], 0, sz)
    wie = ('Massstab x %s, z %s'
           % ('aus %04X/%04X' % (lx[2], lx[3]) if lx else 'geliehen von %04X' % a1,
              'aus %04X/%04X' % (lz[2], lz[3]) if lz else 'geliehen von %04X' % a1))
    return (ox, oy, sx, sz), wie


def feinschliff(rm, z):
    """⛔ GEMESSEN UND VERWORFEN (2026-09-03) - nur noch ueber RE15_FEINSCHLIFF=1.

    Die Idee: die hergeleitete Zeile an den gemalten Rechtecken nachziehen, weil die
    Kunst die staerkere Fessel ist. Das Ergebnis ist SCHLECHTER: die Auslassprobe faellt
    von Median 7 px auf 15 px, und die Treffer innerhalb 8 px von 13 auf 7.

    Der Grund ist mein eigener alter Fehler (Memory reai-v2-proxy-mass): maximiert wird
    die Deckung mit IRGENDEINEM Rechteck des Blattes - also zieht der Raum auf den
    dichtesten Haufen, nicht auf SEIN Rechteck. Richtig waere, nur die Deckung mit den
    ihm zugeordneten Rechtecken zu zaehlen; die Zuordnung braucht aber die Zeile, die
    hier erst entsteht. Wer das aufloest, muss die beiden Schritte gemeinsam loesen,
    nicht nacheinander.

    Urspruengliche Begruendung: die Herleitung setzt den Raum ueber EINEN Tuerpunkt. Jede
    Ungenauigkeit dort schlaegt voll durch - die Auslassprobe lag im Median bei 7 px.
    Die gemalte Kunst ist die staerkere Fessel: der Kasten des Raums soll auf den
    Rechtecken liegen, die er ueberdeckt (BEFUND.md §22: die gemalten Rechtecke SIND die
    Kollisionsboxen). Gesucht ist die Verschiebung, die die Deckung maximiert.

    Der MASSSTAB wird dabei NICHT angetastet - ihn aus derselben groben Lage neu zu
    schaetzen hiesse, den Fehler mit sich selbst zu korrigieren.
    """
    g = A.read_rdt(rm)
    zell = [c for c in g[0] if c[2] > 0 and c[3] > 0] if g else []
    if not zell:
        return z, 0
    wx0 = min(c[0] for c in zell); wx1 = max(c[0] + c[2] for c in zell)
    wz0 = min(c[1] for c in zell); wz1 = max(c[1] + c[3] for c in zell)
    rs = K.rechtecke(BLATT[rm])
    if not rs:
        return z, 0

    def deckung(ox, oy):
        x0 = karte_x(wx0, ox, z[2]); x1 = karte_x(wx1, ox, z[2])
        y0 = karte_y(wz1, oy, z[3]); y1 = karte_y(wz0, oy, z[3])
        x0, x1 = min(x0, x1), max(x0, x1); y0, y1 = min(y0, y1), max(y0, y1)
        s = 0
        for (_k, rx, ry, rw, rh) in rs:
            ox2 = min(x1, rx + rw) - max(x0, rx)
            oy2 = min(y1, ry + rh) - max(y0, ry)
            if ox2 > 0 and oy2 > 0:
                s += ox2 * oy2
        return s

    b0 = deckung(z[0], z[1])
    best = (b0, z[0], z[1])
    for dx in range(-8, 9):
        for dy in range(-8, 9):
            s = deckung(z[0] + dx, z[1] + dy)
            # Bei Gleichstand die KLEINERE Verschiebung - der Zug soll die Herleitung
            # nachziehen, nicht sie ersetzen.
            if s > best[0] or (s == best[0] and abs(dx) + abs(dy) <
                               abs(best[1] - z[0]) + abs(best[2] - z[1])):
                best = (s, z[0] + dx, z[1] + dy)
    return (best[1], best[2], z[2], z[3]), best[0] - b0


def alle_zeilen():
    """Ausgelieferte Zeilen plus hergeleitete, in Ausbreitungsrunden."""
    bekannt = {}
    quelle = {}
    for rm in BLATT:
        z = K.zeile(rm)
        if z:
            bekannt[rm] = z
            quelle[rm] = 'ausgeliefert'
    for _runde in range(6):
        neu = {}
        for rm in sorted(BLATT):
            if rm in bekannt:
                continue
            z, g = herleiten(rm, bekannt)
            if z:
                neu[rm] = (z, g)
        if not neu:
            break
        for rm, (z, g) in neu.items():
            if os.environ.get('RE15_FEINSCHLIFF') == '1':
                z, _gew = feinschliff(rm, z)
            bekannt[rm] = z
            quelle[rm] = g
    return bekannt, quelle


def auslassprobe():
    """Jede ausgelieferte Zeile einmal verstecken und herleiten."""
    echt = dict((rm, K.zeile(rm)) for rm in BLATT if K.zeile(rm))
    fehler = []
    for rm in sorted(echt):
        ohne = dict(echt)
        del ohne[rm]
        z, g = herleiten(rm, ohne)
        if not z:
            continue
        if os.environ.get('RE15_FEINSCHLIFF') == '1':
            z, _gew = feinschliff(rm, z)
        dx = z[0] - echt[rm][0]
        dy = z[1] - echt[rm][1]
        fehler.append((abs(dx) + abs(dy), rm, dx, dy, g))
    fehler.sort()
    return fehler


def main(argv):
    fehler = auslassprobe()
    print("=== AUSLASSPROBE: %d ausgelieferte Zeilen hergeleitet ===" % len(fehler))
    for f, rm, dx, dy, g in fehler:
        print("   ROOM%04X  Fehler (%+3d,%+3d)  %s" % (rm, dx, dy, g))
    if fehler:
        med = fehler[len(fehler) // 2][0]
        v4 = sum(1 for f, _, _, _, _ in fehler if f <= 4)
        v8 = sum(1 for f, _, _, _, _ in fehler if f <= 8)
        print("   Median %d px | <= 4 px: %d | <= 8 px: %d | von %d"
              % (med, v4, v8, len(fehler)))

    bekannt, quelle = alle_zeilen()
    art = collections.Counter(q.split(' (')[0] for q in quelle.values())
    print("\n=== ERGEBNIS: %d von %d Raeumen haben eine Zeile ===" % (len(bekannt), len(BLATT)))
    for k, n in art.most_common():
        print("   %-28s %d" % (k, n))
    fehlt = sorted(set(BLATT) - set(bekannt))
    print("   ohne Zeile: %d  %s" % (len(fehlt), ", ".join("ROOM%04X" % r for r in fehlt)))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
