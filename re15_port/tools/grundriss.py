# -*- coding: utf-8 -*-
"""GRUNDRISS-LOESER: eine zusammenhaengende Karte je Blatt aus Kollision + Tuergraph.

PORT-ERGAENZUNG, keine Rekonstruktion. Das Original zeichnet seine Karte von Hand:
nur 33 der 72 Karten-Raeume tragen ueberhaupt eine Massstabszeile @0x800768b0, 33 der
100 Raeume haben gar kein Rechteck, und die gemalte Kunst ist NICHT massstabsgetreu zur
Kollision (ROOM1100 misst echt 41x47 px, sein gemaltes Rechteck 24x24). Der Nutzer hat
am 2026-09-01 entschieden, dass die Karte davon abweichen darf, wenn sie die
ORIGINAL-SYMBOLE verwendet und dafuer vollstaendig wird.

Sein Modell: "ein neues Kartenstueck schliesst genau da an, wo man den Raum davor durch
die Tuer verlassen hat." Genau das rechnet dieser Loeser:

  AUFBAU     Breitensuche durch den Tuergraph. Anker ist der REZIPROKE Tuer-Datensatz
             des Nachbarn, NICHT der Spawn nx/nz - der liegt schon im Zielraum und
             schiebt die Raeume um diesen Abstand ineinander (gemessen 22-27 %).
  LAGE       Drehung (4) und Spiegelung (2) je Raum: jeder RDT-Raum hat sein eigenes
             lokales Koordinatensystem. Gewaehlt wird die Pose mit der kleinsten
             Ueberlappung; fuer ROOM1120 faellt daraus 180 Grad - dasselbe, was vorher
             unabhaengig aus dem gemalten Tuersymbol und der Nachbarposition gemessen
             wurde.
  AUSRUECKEN Das Trigger-Rechteck deckt den Anlaufbereich ab, nicht nur die Laibung;
             beide Anker liegen also im Rauminneren. Jeder Raum wird laengs der
             Tuer-Normalen zurueckgeschoben, bis nichts mehr doppelt liegt.
  EINPASSEN  Ein Stockwerk misst im ausgelieferten Massstab bis 203x299 px, das
             Kartenfeld ist 132x140. Der fertige Grundriss wird als Ganzes gleichmaessig
             verkleinert - die relative Lage und damit alle Nachbarschaften bleiben.

Massstab = Median der ausgelieferten Zeilen @0x800768b0 derselben Stage (gemessen):
STAGE1 459/464, STAGE2 448/454, STAGE3 481/448, STAGE4 683/648, STAGE5 601/612,
STAGE6 485/455 Welteinheiten je Pixel.

Ergebnis (2026-09-01): Seite 4 0,0 % Ueberlappung und 3 von 3 Durchgaengen beruehrend,
Seite 3 1,6 % und 6 von 6, Seite 2 2,2 % und 12 von 12.

SCHNITTSTELLE
    B = Blatt(zimmer, rdts, ex, ey)   zimmer: [raum_id], rdts: {raum: (sca, tueren, ...)}
    lage, f, kosten = B.loesen()      lage: {raum: (ox, oy, drehung 0..3, spiegel 0/1)}
    B.abbildung(lage[raum])           affine Abbildung Welt -> Kartenpixel
    B.pixel(raum, lage[raum])         belegte Kartenpixel des Raums
"""
import collections
import math

FELD = (100, 55, 132, 140)      # Kartenfeld: dort liegen alle Rechtecke aller Seiten


def dreh(wx, wz, k, sp):
    """Drehung um k*90 Grad, optional vorher gespiegelt."""
    if sp:
        wx = -wx
    if k == 0:
        return (wx, wz)
    if k == 1:
        return (-wz, wx)
    if k == 2:
        return (-wx, -wz)
    return (wz, -wx)


class Blatt(object):
    def __init__(self, zimmer, rdts, ex, ey):
        self.zimmer = [z for z in zimmer if z in rdts]
        self.rdts = rdts
        self.ex = float(ex)
        self.ey = float(ey)
        self._zellen = {}
        self.kanten = []
        for b in self.zimmer:
            for d in rdts[b][1]:
                if (d['rw'] == 0 and d['rd'] == 0) or d['dest'] not in self.zimmer:
                    continue
                for e in rdts[d['dest']][1]:
                    if (e['rw'] == 0 and e['rd'] == 0) or e['dest'] != b:
                        continue
                    # Reziprokes Paar: das Ziel des einen liegt am Ort des anderen.
                    if abs(d['nx'] - e['lx']) + abs(d['nz'] - e['lz']) > 4000:
                        continue
                    if b < d['dest']:
                        self.kanten.append((b, (d['lx'], d['lz']),
                                            d['dest'], (e['lx'], e['lz'])))
                    break

    def zellen(self, rid):
        """Begehbare Zellen. Duenne Waende (hoechstens 1000 Einheiten quer) fliegen
        raus - sie beschreiben die Laibung, nicht die Flaeche."""
        if rid in self._zellen:
            return self._zellen[rid]
        alle = set(self.rdts[rid][0])
        dick = [c for c in alle if min(c[2], c[3]) > 1000]
        self._zellen[rid] = dick or list(alle)
        return self._zellen[rid]

    def pixel(self, b, st):
        ox, oy, k, sp = st
        s = set()
        for (cx, cz, cw, cd) in self.zellen(b):
            pts = [dreh(cx, cz, k, sp), dreh(cx + cw, cz, k, sp),
                   dreh(cx, cz + cd, k, sp), dreh(cx + cw, cz + cd, k, sp)]
            X = [p[0] for p in pts]
            Z = [p[1] for p in pts]
            x0 = int(ox + min(X) / self.ex)
            x1 = int(ox + max(X) / self.ex)
            y0 = int(oy - max(Z) / self.ey)
            y1 = int(oy - min(Z) / self.ey)
            for y in range(y0, y1 + 1):
                for x in range(x0, x1 + 1):
                    s.add((x, y))
        return s

    def punkt(self, b, st, wx, wz):
        ox, oy, k, sp = st
        dx, dz = dreh(wx, wz, k, sp)
        return (ox + dx / self.ex, oy - dz / self.ey)

    def abbildung(self, st):
        """Affine Abbildung Welt -> Kartenpixel als (A, B, C, D, E, F) mit
              mx = (A*wx + B*wz) / 65536 + C
              my = (D*wx + E*wz) / 65536 + F
        Drehung und Spiegelung stecken in den Vorzeichen, der Massstab im Betrag."""
        ox, oy, k, sp = st
        ax, az = dreh(1, 0, k, sp)
        bx, bz = dreh(0, 1, k, sp)
        A = int(round(ax * 65536.0 / self.ex))
        B = int(round(bx * 65536.0 / self.ex))
        D = int(round(-az * 65536.0 / self.ey))
        E = int(round(-bz * 65536.0 / self.ey))
        return (A, B, int(round(ox)), D, E, int(round(oy)))

    def kosten(self, lage, pix=None):
        """(Ueberlappung in %, Zahl der beruehrenden Durchgaenge, Breite, Hoehe)"""
        pix = pix or {b: self.pixel(b, lage[b]) for b in lage}
        alle = set()
        doppelt = 0
        for b in lage:
            doppelt += len(alle & pix[b])
            alle |= pix[b]
        beruehrt = 0
        for (a, pa, b, pb) in self.kanten:
            if a not in lage or b not in lage:
                continue
            nah = False
            for (x, y) in pix[a]:
                for dx in (-2, -1, 0, 1, 2):
                    for dy in (-2, -1, 0, 1, 2):
                        if (x + dx, y + dy) in pix[b]:
                            nah = True
                            break
                    if nah:
                        break
                if nah:
                    break
            if nah:
                beruehrt += 1
        xs = [p[0] for p in alle]
        ys = [p[1] for p in alle]
        br = (max(xs) - min(xs) + 1) if xs else 0
        ho = (max(ys) - min(ys) + 1) if ys else 0
        return (doppelt / max(1, len(alle)) * 100.0, beruehrt, br, ho)

    def _ausruecken(self, b, st, pix, k, sp, anker):
        """Schiebt den Raum vom Anker weg, bis er sich mit dem Bestand nicht mehr
        ueberlagert. Richtung: vom Tueranker zur Raummitte - das ist die Normale der
        Wand, in der die Tuer sitzt. Laengs der Wand aendert sich nichts."""
        if not pix:
            return st
        ox, oy, kk, ss = st
        zs = self.zellen(b)
        mx = sum(c[0] + c[2] / 2.0 for c in zs) / len(zs)
        mz = sum(c[1] + c[3] / 2.0 for c in zs) / len(zs)
        dmx, dmz = dreh(mx - anker[0], mz - anker[1], k, sp)
        vx = dmx / self.ex
        vy = -dmz / self.ey
        n = math.hypot(vx, vy)
        if n < 1e-6:
            return st
        vx /= n
        vy /= n
        beste = st
        bester = None
        for schritt in range(0, 41):
            kand = (ox + vx * schritt * 0.5, oy + vy * schritt * 0.5, kk, ss)
            ueb = sum(len(self.pixel(b, kand) & v) for v in pix.values())
            if ueb == 0:
                return kand
            if bester is None or ueb < bester:
                bester = ueb
                beste = kand
        return beste

    def aufbauen(self):
        if not self.zimmer:
            return {}
        grad = collections.Counter()
        for (a, _, b, _) in self.kanten:
            grad[a] += 1
            grad[b] += 1
        wurzel = max(self.zimmer, key=lambda b: (grad[b], -b))
        nb = collections.defaultdict(list)
        for (a, pa, b, pb) in self.kanten:
            nb[a].append((b, pa, pb))
            nb[b].append((a, pb, pa))
        lage = {wurzel: (0.0, 0.0, 0, 0)}
        pix = {wurzel: self.pixel(wurzel, lage[wurzel])}
        q = [wurzel]
        while q:
            a = q.pop(0)
            for (b, pa, pb) in nb[a]:
                if b in lage:
                    continue
                p = self.punkt(a, lage[a], *pa)
                best = None
                for k in range(4):
                    for sp in (0, 1):
                        d = dreh(pb[0], pb[1], k, sp)
                        st = (p[0] - d[0] / self.ex, p[1] + d[1] / self.ey, k, sp)
                        st = self._ausruecken(b, st, pix, k, sp, pb)
                        pb2 = self.pixel(b, st)
                        ueb = sum(len(pb2 & v) for v in pix.values())
                        if best is None or ueb < best[0]:
                            best = (ueb, st, pb2)
                lage[b] = best[1]
                pix[b] = best[2]
                q.append(b)
        return lage

    def zusammenziehen(self, lage, runden=200):
        """Zieht die Tueren zusammen, ohne je eine Ueberlappung zuzulassen. Harte
        Schranke, kein Gewicht: Ueberlappung ist verboten, der Rest wird nur so klein
        wie moeglich."""
        pix = {b: self.pixel(b, lage[b]) for b in lage}
        for _ in range(runden):
            bewegt = False
            for (a, pa, b, pb) in self.kanten:
                if a not in lage or b not in lage:
                    continue
                p1 = self.punkt(a, lage[a], *pa)
                p2 = self.punkt(b, lage[b], *pb)
                dx = p1[0] - p2[0]
                dy = p1[1] - p2[1]
                d = math.hypot(dx, dy)
                if d < 0.6:
                    continue
                for wer, vz in ((b, 1.0), (a, -1.0)):
                    ox, oy, k, sp = lage[wer]
                    s = min(1.0, d) * 0.5 * vz
                    kand = (ox + dx / d * s, oy + dy / d * s, k, sp)
                    pk = self.pixel(wer, kand)
                    if any(len(pk & pix[o]) > 0 for o in lage if o != wer):
                        continue
                    lage[wer] = kand
                    pix[wer] = pk
                    bewegt = True
                    break
            if not bewegt:
                break
        return lage

    def einpassen(self, lage, feld=FELD):
        pix = {b: self.pixel(b, lage[b]) for b in lage}
        alle = set().union(*pix.values()) if pix else set()
        if not alle:
            return lage, 1.0
        xs = [p[0] for p in alle]
        ys = [p[1] for p in alle]
        br = max(xs) - min(xs) + 1
        ho = max(ys) - min(ys) + 1
        f = min(1.0, feld[2] / float(br), feld[3] / float(ho))
        self.ex /= f
        self.ey /= f
        neu = {}
        for b, (ox, oy, k, sp) in lage.items():
            neu[b] = (feld[0] + (ox - min(xs)) * f, feld[1] + (oy - min(ys)) * f, k, sp)
        return neu, f

    def loesen(self, feld=FELD):
        lage = self.aufbauen()
        if not lage:
            return {}, 1.0, (0.0, 0, 0, 0)
        lage = self.zusammenziehen(lage)
        lage, f = self.einpassen(lage, feld)
        return lage, f, self.kosten(lage)
