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
import bisect
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


def entzerren_komp(bl, lg, runden=600, grenze=6):
    """Entzerrt ALLE Blaetter eines Gebaeudes GEMEINSAM.

    ⛔ EIN GETEILTER ORT DARF SICH BEWEGEN - ABER UEBERALL GLEICH. Ein erster Wurf
    nagelte die geteilten Orte fest (sie halten ja das Stapeln der Stockwerke) und kam
    auf Seite 3 nicht unter 20 % Ueberlappung: dort ueberlappten festgenagelte Orte
    einander, und nichts durfte ausweichen. Wird dagegen JEDE Bewegung eines Ortes auf
    allen seinen Blaettern zugleich ausgefuehrt, bleibt das Stapeln erhalten und der
    Entzerrer behaelt seine Freiheit.

    Ursache der Ueberlappung ist das Neurunden: die Polizeiwache faellt von 585 auf
    ~1400 Welteinheiten je Pixel, wenn alle fuenf Blaetter denselben Massstab bekommen,
    und jede Zelle rundet dabei neu.
    """
    pix = {}
    for pg in lg:
        for o, st in lg[pg].items():
            pix[(pg, o)] = bl[pg].pixel(o, st)
    ueb = {}
    for pg in lg:
        ns = sorted(lg[pg])
        for i in range(len(ns)):
            for j in range(i + 1, len(ns)):
                n = len(pix[(pg, ns[i])] & pix[(pg, ns[j])])
                if n:
                    ueb[(pg, ns[i], ns[j])] = n
    weg = {}
    for _ in range(runden):
        if not ueb:
            break
        best = max(ueb.items(), key=lambda kv: kv[1])[0]
        pg, a, b = best
        wer = ander = None
        for kand, geg in ((b, a), (a, b)):
            if weg.get(kand, 0) < grenze:
                wer, ander = kand, geg
                break
        if wer is None:
            del ueb[best]
            continue
        cw = (sum(p[0] for p in pix[(pg, wer)]) / float(len(pix[(pg, wer)])),
              sum(p[1] for p in pix[(pg, wer)]) / float(len(pix[(pg, wer)])))
        ca = (sum(p[0] for p in pix[(pg, ander)]) / float(len(pix[(pg, ander)])),
              sum(p[1] for p in pix[(pg, ander)]) / float(len(pix[(pg, ander)])))
        dx, dy = cw[0] - ca[0], cw[1] - ca[1]
        if abs(dx) >= abs(dy):
            sx, sy = (1.0 if dx >= 0 else -1.0), 0.0
        else:
            sx, sy = 0.0, (1.0 if dy >= 0 else -1.0)
        # ⛔ UEBERLAPPUNG IST DIE SCHRANKE, DER DURCHGANG DAS ZIEL. Ein erster Wurf
        # schob nur nach Ueberlappung: Seite 3 fiel von 20,6 % auf 12,8 % - und die
        # beruehrenden Durchgaenge von 6 von 7 auf 3 von 7. Dieselbe Falle wie beim
        # Posen-Nachlauf. Ein Zug wird deshalb nur angenommen, wenn er KEINEN Durchgang
        # kostet; sonst wird die andere Richtung und der andere Partner probiert.
        vorher = dict((pg2, bl[pg2].kontakte(
                          dict((o, pix[(pg2, o)]) for o in lg[pg2])))
                      for pg2 in lg if wer in lg[pg2])
        alt_lage = dict((pg2, lg[pg2][wer]) for pg2 in lg if wer in lg[pg2])
        alt_pix = dict((pg2, pix[(pg2, wer)]) for pg2 in lg if wer in lg[pg2])

        def anwenden(vx, vy):
            for pg2 in list(alt_lage):
                ox, oy, k, sp = alt_lage[pg2]
                lg[pg2][wer] = (ox + vx, oy + vy, k, sp)
                pix[(pg2, wer)] = bl[pg2].pixel(wer, lg[pg2][wer])

        def zuruecknehmen():
            for pg2 in list(alt_lage):
                lg[pg2][wer] = alt_lage[pg2]
                pix[(pg2, wer)] = alt_pix[pg2]

        gut = False
        for (vx, vy) in ((sx, sy), (sy, sx), (-sy, -sx)):
            if vx == 0.0 and vy == 0.0:
                continue
            anwenden(vx, vy)
            ok = True
            for pg2 in vorher:
                if bl[pg2].kontakte(dict((o, pix[(pg2, o)])
                                         for o in lg[pg2])) < vorher[pg2]:
                    ok = False
                    break
            if ok:
                gut = True
                break
            zuruecknehmen()
        if not gut:
            del ueb[best]
            continue
        weg[wer] = weg.get(wer, 0) + 1
        for pg2 in list(alt_lage):
            for o in lg[pg2]:
                if o == wer:
                    continue
                schl = (pg2, min(o, wer), max(o, wer))
                n = len(pix[(pg2, wer)] & pix[(pg2, o)])
                if n:
                    ueb[schl] = n
                elif schl in ueb:
                    del ueb[schl]
    return lg


class Blatt(object):
    def __init__(self, zimmer, rdts, ex, ey):
        self.zimmer = [z for z in zimmer if z in rdts]
        self.rdts = rdts
        self.ex = float(ex)
        self.ey = float(ey)
        self._zellen = {}
        # FESTE POSEN: Orte, deren Drehung/Spiegelung ein bereits geloestes Blatt
        # DERSELBEN Komponente vorgibt. Ein Ort, der auf zwei Blaettern liegt
        # (Treppenhaus, Fahrstuhl), muss dort gleich herum liegen - sonst stapeln die
        # Stockwerke nicht.
        self.feste_posen = {}
        # FESTE LAGEN: vollstaendige Zustaende (ox, oy, Drehung, Spiegelung) aus einem
        # schon geloesten Blatt derselben Komponente. Damit waechst dieses Blatt um
        # DIESELBEN Anker wie das vorige - sonst legt jeder Loeserlauf sein Stockwerk um
        # eine andere Wurzel, und die Vereinigung aller Blaetter wird unnoetig gross
        # (gemessen 372x342 px statt der 225x323 des groessten Einzelblattes, was den
        # gemeinsamen Massstab von 705 auf 1404 Welteinheiten je Pixel verdoppelte).
        self.feste_lagen = {}
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
        # ⛔ NOTKANTEN. Ein Blatt zerfaellt in MEHRERE Komponenten des Tuergraphen -
        # Seite 8 in {4000,4010,4030}, {4080,4090,40A0,40B0}, den turlosen 4020 und die
        # Wurzelkomponente. Die Breitensuche setzte nur die Wurzelkomponente: 3 von 11
        # Raeumen. Die Bruecken dazwischen SIND vorhanden, nur einseitig: 4000 -> 4020
        # und 4080 -> 4030 tragen keinen reziproken Gegen-Datensatz.
        # Anker ist dort der Spawn nx/nz. Der liegt schon ein Stueck im Zielraum (der
        # Grund, warum er fuer die regulaeren Kanten falsch ist, gemessen 22-27 %
        # Ueberlappung) - AUSRUECKEN und ZUSAMMENZIEHEN korrigieren das anschliessend,
        # und ein um wenige Pixel versetzter Raum ist allemal besser als keiner.
        fest = {(min(a, b), max(a, b)) for (a, _, b, _) in self.kanten}
        self.notkanten = []
        for b in self.zimmer:
            for d in rdts[b][1]:
                if (d['rw'] == 0 and d['rd'] == 0) or d['dest'] not in self.zimmer:
                    continue
                if d['dest'] == b:
                    continue
                if (min(b, d['dest']), max(b, d['dest'])) in fest:
                    continue
                self.notkanten.append((b, (d['lx'], d['lz']),
                                       d['dest'], (d['nx'], d['nz'])))

    @staticmethod
    def schliessen(zellen):
        """Eingeschlossene Loecher schliessen und zu Rechtecken zusammenfassen.

        ⛔ Die Kollisionszellen (RDT +0x20) beschreiben den BEGEHBAREN WEG, nicht
        den Raum. In einem moeblierten Raum ist das ein Ring um die Moebel: ROOM1120
        besteht aus elf Streifen, der breiteste 4 Kartenpixel ([100,90,4,44],
        [100,90,24,4], ...). Ungefuellt gezeichnet sieht ein Buero deshalb aus wie ein
        Labyrinth aus Fluren - im Abzug des 3F-Blattes deutlich zu sehen. Ein
        Kartenraum ist die FLAECHE; alles, was von aussen nicht erreichbar ist, gehoert
        dazu.

        Gerechnet auf dem Gitter der Zell-Kanten (Koordinaten-Kompression), also exakt
        und ohne Wahl einer Rasterweite. Danach werden gleiche Zeilenlaeufe vertikal
        verschmolzen, damit die Zellenliste des Zeichners klein bleibt."""
        zellen = [c for c in zellen if c[2] > 0 and c[3] > 0]
        if not zellen:
            return []
        XS = sorted({c[0] for c in zellen} | {c[0] + c[2] for c in zellen})
        ZS = sorted({c[1] for c in zellen} | {c[1] + c[3] for c in zellen})
        nx, nz = len(XS) - 1, len(ZS) - 1
        if nx <= 0 or nz <= 0:
            return list(zellen)
        belegt = [[False] * nz for _ in range(nx)]
        for (cx, cz, cw, cd) in zellen:
            i0 = bisect.bisect_left(XS, cx)
            i1 = bisect.bisect_left(XS, cx + cw)
            j0 = bisect.bisect_left(ZS, cz)
            j1 = bisect.bisect_left(ZS, cz + cd)
            for i in range(i0, i1):
                for j in range(j0, j1):
                    belegt[i][j] = True
        # Aussenraum vom Gitterrand her fluten; was nicht erreicht wird, ist innen.
        aussen = [[False] * nz for _ in range(nx)]
        stapel = []
        for i in range(nx):
            for j in (0, nz - 1):
                if not belegt[i][j] and not aussen[i][j]:
                    aussen[i][j] = True
                    stapel.append((i, j))
        for j in range(nz):
            for i in (0, nx - 1):
                if not belegt[i][j] and not aussen[i][j]:
                    aussen[i][j] = True
                    stapel.append((i, j))
        while stapel:
            i, j = stapel.pop()
            for (di, dj) in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                a, b = i + di, j + dj
                if 0 <= a < nx and 0 <= b < nz and not belegt[a][b] and not aussen[a][b]:
                    aussen[a][b] = True
                    stapel.append((a, b))
        # ⛔ DIE FLUT LECKT DURCH DIE TUEROEFFNUNG. ROOM1150s begehbarer Ring hat
        # dort, wo die Tuer sitzt, eine Luecke - der Aussenraum erreicht die
        # Moebelflaeche und sie bleibt ein Loch (Vorschau 3F: der Raum stand als
        # Rahmen mit zwei Kaesten darin, 18 Streifen). Zusaetzlich wird deshalb die
        # ORTHOGONAL-KONVEXE Huelle gefuellt: ein Feld gehoert dazu, wenn in seiner
        # Zeile LINKS UND RECHTS und in seiner Spalte OBEN UND UNTEN belegte Felder
        # liegen. Das schliesst den Ring, laesst eine L-Form aber L-foermig (in der
        # Kerbe fehlt jeweils eine der vier Richtungen).
        zl = []
        for j in range(nz):
            idx = [i for i in range(nx) if belegt[i][j]]
            zl.append((idx[0], idx[-1]) if idx else None)
        sp = []
        for i in range(nx):
            idx = [j for j in range(nz) if belegt[i][j]]
            sp.append((idx[0], idx[-1]) if idx else None)
        voll = [[belegt[i][j] or not aussen[i][j] or
                 (zl[j] is not None and zl[j][0] <= i <= zl[j][1] and
                  sp[i] is not None and sp[i][0] <= j <= sp[i][1])
                 for j in range(nz)] for i in range(nx)]
        # Zeilenlaeufe, gleiche Muster vertikal verschmelzen
        raus = []
        j = 0
        while j < nz:
            muster = [voll[i][j] for i in range(nx)]
            k = j + 1
            while k < nz and [voll[i][k] for i in range(nx)] == muster:
                k += 1
            i = 0
            while i < nx:
                if muster[i]:
                    t = i
                    while t < nx and muster[t]:
                        t += 1
                    raus.append((XS[i], ZS[j], XS[t] - XS[i], ZS[k] - ZS[j]))
                    i = t
                else:
                    i += 1
            j = k
        return raus

    def _posen(self, b):
        """Die zulaessigen Posen eines Ortes: alle acht - oder genau die eine, die ein
        anderes Blatt derselben Komponente schon festgelegt hat."""
        f = self.feste_lagen.get(b)
        if f is not None:
            return [(f[2], f[3])]
        f = self.feste_posen.get(b)
        if f is not None:
            return [f]
        return [(k, sp) for k in range(4) for sp in (0, 1)]

    def zellen(self, rid):
        """Die FLAECHE des Raums: Kollisionszellen mit geschlossenen Loechern."""
        if rid in self._zellen:
            return self._zellen[rid]
        alle = list(self.rdts[rid][0])
        self._zellen[rid] = self.schliessen(alle) or alle
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

    def kontakte(self, pix):
        """Zahl der Durchgaenge, deren beide Enden sich beruehren - aus einem fertigen
        Pixel-Cache. Gleiche Regel wie kosten(), nur ohne Neuberechnung."""
        n = 0
        for (a, pa, b, pb) in self.kanten:
            if a not in pix or b not in pix:
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
                n += 1
        return n

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
        # Ein Ort mit FESTER Pose ist die beste Wurzel: an ihm haengt die Ausrichtung
        # zum Nachbarblatt.
        wurzel = max(self.zimmer,
                     key=lambda b: (1 if b in self.feste_posen else 0, grad[b], -b))
        nb = collections.defaultdict(list)
        for (a, pa, b, pb) in self.kanten:
            nb[a].append((b, pa, pb))
            nb[b].append((a, pb, pa))
        nnb = collections.defaultdict(list)
        for (a, pa, b, pb) in self.notkanten:
            nnb[a].append((b, pa, pb))
            nnb[b].append((a, pb, pa))
        # Feste Lagen zuerst setzen - sie sind die Anker, an denen dieses Blatt zu den
        # anderen Stockwerken passt.
        lage = {}
        for b in self.zimmer:
            if b in self.feste_lagen:
                lage[b] = self.feste_lagen[b]
        if lage:
            wurzel = sorted(lage)[0]
        else:
            lage = {wurzel: (0.0, 0.0) + self.feste_posen.get(wurzel, (0, 0))}
        pix = dict((b, self.pixel(b, lage[b])) for b in lage)
        q = sorted(lage)

        def anheften(a, b, pa, pb):
            """b an die schon gesetzte Tuer von a haengen, beste der 8 Posen."""
            p = self.punkt(a, lage[a], *pa)
            best = None
            for (k, sp) in self._posen(b):
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

        while True:
            while q:
                a = q.pop(0)
                for (b, pa, pb) in nb[a]:
                    if b not in lage:
                        anheften(a, b, pa, pb)
            # Komponente erschoepft: ueber eine NOTKANTE weiter, sonst ANBAUEN.
            weiter = None
            for a in lage:
                for (b, pa, pb) in nnb[a]:
                    if b not in lage:
                        weiter = (a, b, pa, pb)
                        break
                if weiter:
                    break
            if weiter:
                anheften(*weiter)
                continue
            offen = [b for b in self.zimmer if b not in lage]
            if not offen:
                break
            self._anbauen(max(offen, key=lambda b: (grad[b], -b)), lage, pix)
            q.append(max(offen, key=lambda b: (grad[b], -b)))
        return lage

    def _anbauen(self, b, lage, pix):
        """Einen Raum ohne jede Tuerverbindung auf dem Blatt anlegen: dicht an das
        schon Gesetzte, aber ueberlappungsfrei. Betrifft z.B. ROOM4020 (Seite 8, gar
        keine Tuer im RDT) und ROOM5090 (Seite 9, nur Selbst-Tueren und eine Tuer auf
        ein anderes Blatt). Ohne diesen Zweig blieben sie ganz ohne Zeichnung."""
        belegt = set()
        for v in pix.values():
            belegt |= v
        if not belegt:
            lage[b] = (0.0, 0.0, 0, 0)
            pix[b] = self.pixel(b, lage[b])
            return
        xs = [p[0] for p in belegt]
        ys = [p[1] for p in belegt]
        mx = (min(xs) + max(xs)) / 2.0
        my = (min(ys) + max(ys)) / 2.0
        best = None
        for (k, sp) in self._posen(b):
            if True:
                roh = self.pixel(b, (0.0, 0.0, k, sp))
                if not roh:
                    continue
                rx = [p[0] for p in roh]
                ry = [p[1] for p in roh]
                br = max(rx) - min(rx) + 1
                ho = max(ry) - min(ry) + 1
                for (ox, oy) in ((max(xs) + 1 - min(rx), int(my - ho / 2) - min(ry)),
                                 (min(xs) - br - min(rx), int(my - ho / 2) - min(ry)),
                                 (int(mx - br / 2) - min(rx), max(ys) + 1 - min(ry)),
                                 (int(mx - br / 2) - min(rx), min(ys) - ho - min(ry))):
                    st = (float(ox), float(oy), k, sp)
                    pb2 = self.pixel(b, st)
                    ueb = len(pb2 & belegt)
                    cx = sum(p[0] for p in pb2) / float(len(pb2))
                    cy = sum(p[1] for p in pb2) / float(len(pb2))
                    f = ueb * 1000.0 + math.hypot(cx - mx, cy - my)
                    if best is None or f < best[0]:
                        best = (f, st, pb2)
        if best is None:
            best = (0.0, (0.0, 0.0, 0, 0), self.pixel(b, (0.0, 0.0, 0, 0)))
        lage[b] = best[1]
        pix[b] = best[2]

    def verbessern(self, lage, runden=8):
        """Posen-Nachlauf. Die Breitensuche waehlt die Pose jedes Raums in dem
        Moment, in dem er gesetzt wird - sie kennt die spaeteren Nachbarn also noch
        nicht, und ab etwa sechs Raeumen je Blatt entscheidet damit die Reihenfolge
        statt der Geometrie (die vollstaendige Suche ueber 8^n Posen ist ab da nicht
        mehr rechenbar: Seite 7 haette 8^15 = 3,5e13 Lagen).
        Stattdessen: jeder Raum wird der Reihe nach an seiner Ankertuer neu in alle
        acht Posen gelegt und die mit der kleinsten Gesamt-Ueberlappung behalten,
        solange sich noch etwas aendert. Das ist ein Bergsteiger, kein Optimum -
        aber er sieht im Gegensatz zur Breitensuche das FERTIGE Blatt."""
        nbb = collections.defaultdict(list)
        for menge in (self.kanten, self.notkanten):
            for (a, pa, b, pb) in menge:
                nbb[b].append((a, pa, pb))     # (Nachbar, dessen Tuerpunkt, eigener)
                nbb[a].append((b, pb, pa))
        pix = {b: self.pixel(b, lage[b]) for b in lage}
        for _ in range(runden):
            bewegt = False
            for b in list(lage):
                if b in self.feste_lagen:
                    continue
                anker = None
                for (a, pa, pb) in nbb[b]:
                    if a in lage and a != b:
                        anker = (a, pa, pb)
                        break
                if anker is None:
                    continue
                a, pa, pb = anker
                p = self.punkt(a, lage[a], *pa)
                fremd = [v for o, v in pix.items() if o != b]

                def guete(st, pb2):
                    """⛔ UEBERLAPPUNG ALLEIN IST DAS FALSCHE MASS. Ein erster
                    Nachlauf, der nur sie minimierte, drueckte Seite 1 von 7,3 % auf
                    3,0 % - und die beruehrenden Durchgaenge von 9 von 9 auf 3 von 9.
                    Das Ziel ist aber der Satz des Nutzers: "ein neues Kartenstueck
                    schliesst genau da an, wo man den Raum davor durch die Tuer
                    verlassen hat." Ueberlappung ist die SCHRANKE (Faktor 10000),
                    der Tuerabstand das eigentliche Mass."""
                    f = sum(len(pb2 & v) for v in fremd) * 10000.0
                    for (o, po, pe) in nbb[b]:
                        if o not in lage or o == b:
                            continue
                        q1 = self.punkt(o, lage[o], *po)
                        q2 = self.punkt(b, st, *pe)
                        f += math.hypot(q1[0] - q2[0], q1[1] - q2[1])
                    return f

                best = (guete(lage[b], pix[b]), lage[b], pix[b])
                for (k, sp) in self._posen(b):
                    if True:
                        d = dreh(pb[0], pb[1], k, sp)
                        st = (p[0] - d[0] / self.ex, p[1] + d[1] / self.ey, k, sp)
                        st = self._ausruecken(b, st, {o: v for o, v in pix.items()
                                                      if o != b}, k, sp, pb)
                        pb2 = self.pixel(b, st)
                        f = guete(st, pb2)
                        if f < best[0] - 1e-9:
                            best = (f, st, pb2)
                if best[1] != lage[b]:
                    lage[b] = best[1]
                    pix[b] = best[2]
                    bewegt = True
            if not bewegt:
                break
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
                    if wer in self.feste_lagen:
                        continue
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

    def entzerren(self, lage, fest=(), runden=60):
        """Nach dem GEMEINSAMEN Einpassen mehrerer Blaetter ueberlappen Zeichnungen,
        die vorher sauber nebeneinander lagen: das Raster ist groeber geworden (die
        Polizeiwache faellt von 585 auf 1292 Welteinheiten je Pixel, wenn alle fuenf
        Blaetter denselben Massstab bekommen), und jede Zelle rundet neu. Gemessen stieg
        Seite 3 dadurch auf 30,7 % Ueberlappung.
        Hier werden die Orte pixelweise auseinandergeschoben, bis nichts mehr doppelt
        liegt - AUSSER den festgenagelten: das sind die Orte, die auch auf einem anderen
        Blatt liegen. Wuerden die sich bewegen, stapelten die Stockwerke nicht mehr."""
        fest = set(fest)
        weg = dict((b, 0) for b in lage)      # wie weit ein Ort schon geschoben wurde
        GRENZE = 4                            # Pixel; mehr zerreisst die Durchgaenge
        for _ in range(runden):
            pix = dict((b, self.pixel(b, lage[b])) for b in lage)
            namen = sorted(lage)
            paare = []
            for i in range(len(namen)):
                for j in range(i + 1, len(namen)):
                    a, b = namen[i], namen[j]
                    n = len(pix[a] & pix[b])
                    if n:
                        paare.append((n, a, b))
            if not paare:
                break
            # ⛔ EIN PAAR, DAS SICH NICHT BEWEGEN DARF, BEENDET NICHT DIE ARBEIT.
            # Ein erster Wurf brach beim schlimmsten Paar ab, wenn BEIDE festgenagelt
            # waren - Seite 3 blieb dadurch bei 19,5 % stehen, obwohl die uebrigen Paare
            # loesbar waren. Jetzt wird das naechste loesbare genommen.
            paare.sort(key=lambda t: -t[0])
            wer = ander = None
            for (_n, a, b) in paare:
                for kand, geg in ((b, a), (a, b)):
                    if kand not in fest and weg[kand] < GRENZE:
                        wer, ander = kand, geg
                        break
                if wer is not None:
                    break
            if wer is None:
                break
            weg[wer] += 1
            cx = sum(p[0] for p in pix[wer]) / float(len(pix[wer]))
            cy = sum(p[1] for p in pix[wer]) / float(len(pix[wer]))
            ax = sum(p[0] for p in pix[ander]) / float(len(pix[ander]))
            ay = sum(p[1] for p in pix[ander]) / float(len(pix[ander]))
            dx, dy = cx - ax, cy - ay
            if abs(dx) >= abs(dy):
                schritt = (1.0 if dx >= 0 else -1.0, 0.0)
            else:
                schritt = (0.0, 1.0 if dy >= 0 else -1.0)
            ox, oy, k, sp = lage[wer]
            lage[wer] = (ox + schritt[0], oy + schritt[1], k, sp)
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

    def loesen_roh(self):
        """Die Lage OHNE Einpassen - im Massstab ex/ey, Ursprung frei. Wird gebraucht,
        wenn mehrere Blaetter eines Gebaeudes gemeinsam eingepasst werden sollen: erst
        ausrichten, dann EIN Massstab fuer alle."""
        roh = self.aufbauen()
        if not roh:
            return {}
        besser = None
        for lage in (dict(roh), self.verbessern(dict(roh))):
            lg = self.zusammenziehen(lage)
            k = self.kosten(lg)
            if besser is None or (-k[1], k[0]) < (-besser[1][1], besser[1][0]):
                besser = (lg, k)
        return besser[0]

    def loesen(self, feld=FELD):
        roh = self.aufbauen()
        if not roh:
            return {}, 1.0, (0.0, 0, 0, 0)
        # ⛔ BEIDE WEGE RECHNEN UND DEN BESSEREN NEHMEN. Der Posen-Nachlauf senkt die
        # Ueberlappung deutlich (Seite 6: 11,9 % -> 3,7 %), kostet aber auf manchen
        # Blaettern Durchgaenge (Seite 1: 9 von 9 -> 3 von 9). Welcher Weg gewinnt,
        # haengt vom Blatt ab und ist nicht vorher zu wissen - also wird gemessen.
        # Rangfolge nach dem Modell des Nutzers: MEHR beruehrende Durchgaenge zuerst
        # ("ein neues Kartenstueck schliesst genau da an, wo man den Raum davor durch
        # die Tuer verlassen hat"), bei Gleichstand weniger Ueberlappung.
        besser = None
        for lage in (dict(roh), self.verbessern(dict(roh))):
            ex, ey = self.ex, self.ey
            lg = self.zusammenziehen(lage)
            lg, f = self.einpassen(lg, feld)
            k = self.kosten(lg)
            if besser is None or (-k[1], k[0]) < (-besser[2][1], besser[2][0]):
                besser = (lg, f, k, self.ex, self.ey)
            self.ex, self.ey = ex, ey      # einpassen hat den Massstab veraendert
        self.ex, self.ey = besser[3], besser[4]
        return besser[0], besser[1], besser[2]
