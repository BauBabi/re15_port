# -*- coding: utf-8 -*-
"""GRUNDRISS-LOESER: eine zusammenhaengende Karte je Blatt aus Kollision + Tuergraph.

⛔ VOLLSTAENDIG NEU GEFASST AM 2026-09-02 nach der Vorgabe des Nutzers.

Die vorherige Fassung setzte die AUSGEFUELLTEN SILHOUETTEN der Kollisionszellen und
richtete sie ueber einen Ausgleich aus (Federn ueber alle Kanten + Trennung). Das konnte
nicht sauber werden, und das ist gemessen, nicht vermutet: zwei ineinandergreifende
L-Formen lassen sich nicht gleichzeitig an der Tuer verheften UND ueberlappungsfrei
stellen, und wo ein Rundweg im Tuergraph geometrisch nicht schliesst, blieb ein Rest von
bis zu 35 px stehen (integration_map_uebergang, live gegen die Engine gemessen).

Der Nutzer hat den einfachen Weg vorgegeben:

    "Raum A hat die aeussere Kollisionsbox von Breite X bis Breite Y. Zack, hast du schon
     den Grundriss des Raumes fertig. An Position X,Y befindet sich eine Tuer - zack, hast
     du alle Tueren eingezeichnet. Dann gehst du durch die Tuer: der naechste Raum
     schliesst EXAKT an die Wand der Karte vom Raum davor an, ueberlappt die Wand also.
     ... Ausnahme Treppe -> die fuehrt zu einer neuen Etage."

Und der entscheidende Zusatz:

    "Halte dich nicht an solchem Quatsch wie 'aber die Tuer spawnt mich 500 px weiter im
     Raum'. Die Karte ist nur ein SCHEMA, das davon ausgeht, dass ein Raum an den anderen
     haengt."

Damit faellt alles weg, was die alte Fassung kompliziert gemacht hat: keine Wanddicke,
kein Spawn-Versatz, keine Ruhelaenge, kein Ausgleich, keine Entzerrung.

  ORT       = ein RECHTECK, die aeussere Bbox seiner Kollisionszellen.
  DURCHGANG = legt das naechste Rechteck mit seiner Wand exakt auf die Wand des vorigen,
              Tuerpunkt auf Tuerpunkt. Rechtecke kacheln immer - die Luecken und das
              Verrutschen koennen dabei gar nicht erst entstehen.
  TREPPE    = fuehrt auf ein anderes Blatt; dort wird neu angesetzt.

Massstab = Median der ausgelieferten Zeilen @0x800768b0 derselben Stage (gemessen):
STAGE1 459/464, STAGE2 448/454, STAGE3 481/448, STAGE4 683/648, STAGE5 601/612,
STAGE6 485/455 Welteinheiten je Pixel.

SCHNITTSTELLE (unveraendert, damit der Generator gleich bleibt)
    B = Blatt(zimmer, rdts, ex, ey)   zimmer: [ort], rdts: {ort: (zellen, tueren, treppen)}
    lage, f, kosten = B.loesen()      lage: {ort: (ox, oy, drehung 0..3, spiegel 0/1)}
    B.abbildung(lage[ort])            affine Abbildung Welt -> Kartenpixel
    B.pixel(ort, lage[ort])           belegte Kartenpixel des Ortes
    B.zellen(ort)                     die EINE Zelle: die aeussere Kollisionsbox
"""
import collections

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
        self.feste_posen = {}
        self.feste_lagen = {}
        self.kanten = []
        self.kantenmass = []
        self.notkanten = []
        self.notkantenmass = []
        self._nbp = None
        self._wurzel = None
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
                        self.kantenmass.append(((d['rw'], d['rd']), (e['rw'], e['rd'])))
                    break
        # NOTKANTEN: einseitige Tueren (kein reziproker Gegen-Datensatz). Ohne sie
        # zerfaellt ein Blatt in mehrere Komponenten - Seite 8 in vier, davon wurde
        # frueher nur eine gesetzt (3 von 11 Raeumen). Anker ist dort der Spawn nx/nz.
        fest = set((min(a, b), max(a, b)) for (a, _p, b, _q) in self.kanten)
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
                self.notkantenmass.append((d['rw'], d['rd']))

    # ------------------------------------------------------------------ Geometrie
    def zellen(self, rid):
        """⛔ DIE AEUSSERE KOLLISIONSBOX - EINE Zelle, kein Zellhaufen.

        Der Nutzer: "Raum A hat die aeussere Kollisionsbox von Breite X bis Breite Y.
        Zack, hast du schon den Grundriss des Raumes fertig."

        Die alte Fassung fuellte die einzelnen Zellen (Loecher schliessen,
        orthogonal-konvexe Huelle) und erzeugte damit ineinandergreifende L-Formen, die
        sich nicht gleichzeitig an der Tuer verheften und ueberlappungsfrei stellen
        lassen. Ein Rechteck kachelt immer."""
        if rid in self._zellen:
            return self._zellen[rid]
        alle = [c for c in self.rdts[rid][0] if c[2] > 0 and c[3] > 0]
        if not alle:
            self._zellen[rid] = []
            return self._zellen[rid]
        x0 = min(c[0] for c in alle)
        z0 = min(c[1] for c in alle)
        x1 = max(c[0] + c[2] for c in alle)
        z1 = max(c[1] + c[3] for c in alle)
        self._zellen[rid] = [(x0, z0, x1 - x0, z1 - z0)]
        return self._zellen[rid]

    def kasten(self, b, st):
        """Das Rechteck des Ortes in Kartenpixeln: (x, y, w, h)."""
        z = self.zellen(b)
        if not z:
            return None
        (cx, cz, cw, cd) = z[0]
        ox, oy, k, sp = st
        pts = [dreh(cx, cz, k, sp), dreh(cx + cw, cz, k, sp),
               dreh(cx, cz + cd, k, sp), dreh(cx + cw, cz + cd, k, sp)]
        X = [p[0] for p in pts]
        Z = [p[1] for p in pts]
        x0 = ox + min(X) / self.ex
        x1 = ox + max(X) / self.ex
        y0 = oy - max(Z) / self.ey
        y1 = oy - min(Z) / self.ey
        return (x0, y0, x1 - x0, y1 - y0)

    def pixel(self, b, st):
        k = self.kasten(b, st)
        if not k:
            return set()
        x0 = int(round(k[0]))
        y0 = int(round(k[1]))
        x1 = int(round(k[0] + k[2]))
        y1 = int(round(k[1] + k[3]))
        return set((x, y) for y in range(y0, y1 + 1) for x in range(x0, x1 + 1))

    def punkt(self, b, st, wx, wz):
        ox, oy, k, sp = st
        dx, dz = dreh(wx, wz, k, sp)
        return (ox + dx / self.ex, oy - dz / self.ey)

    def abbildung(self, st):
        """Affine Abbildung Welt -> Kartenpixel als (A, B, C, D, E, F) mit
              mx = (A*wx + B*wz) / 65536 + C
              my = (D*wx + E*wz) / 65536 + F"""
        ox, oy, k, sp = st
        ax, az = dreh(1, 0, k, sp)
        bx, bz = dreh(0, 1, k, sp)
        A = int(round(ax * 65536.0 / self.ex))
        B = int(round(bx * 65536.0 / self.ex))
        D = int(round(-az * 65536.0 / self.ey))
        E = int(round(-bz * 65536.0 / self.ey))
        return (A, B, int(round(ox)), D, E, int(round(oy)))

    # ------------------------------------------------------------------ Anlegen
    @staticmethod
    def wand(kasten, p):
        """Welche Wand des Rechtecks liegt dem Punkt am naechsten?
        0 = Nord, 1 = Ost, 2 = Sued, 3 = West."""
        x, y, w, h = kasten
        d = (p[1] - y, (x + w) - p[0], (y + h) - p[1], p[0] - x)
        best = 0
        for i in range(1, 4):
            if d[i] < d[best]:
                best = i
        return best

    def wandpunkt(self, b, st, p):
        """Der Tuerpunkt AUF die naechste Wand des Rechtecks projiziert - dort wird das
        Symbol gezeichnet, und dort muessen die beiden Enden eines Durchgangs
        zusammenfallen.

        ⛔ Der Tuer-Datensatz selbst liegt im INNEREN des Raums (das Trigger-Rechteck
        deckt den Anlaufbereich ab). Ihn ungeprojiziert zu vergleichen misst die
        Anlauftiefe mit und behauptet einen Fehler, den es auf der Zeichnung nicht gibt.
        Der Nutzer dazu: "Die Karte ist nur ein Schema, das davon ausgeht, dass ein Raum
        an den anderen haengt." """
        ka = self.kasten(b, st)
        if not ka:
            return p
        x, y, w, h = ka
        wd = self.wand(ka, p)
        if wd == 0:
            return (p[0], y)
        if wd == 1:
            return (x + w, p[1])
        if wd == 2:
            return (p[0], y + h)
        return (x, p[1])

    def anlegen(self, a, st_a, pa, b, pb):
        """Alle Lagen, in denen b mit seiner Tuerwand EXAKT auf a's Tuerwand liegt.

        ⛔ DAS IST DER GANZE KERN. Die Tuer sitzt auf einer Wand von a; b muss seine
        Gegen-Tuer auf der GEGENUEBERLIEGENDEN Wand haben, und dann wird b so geschoben,
        dass (1) die beiden Wandlinien zusammenfallen und (2) die beiden Tuerpunkte
        LAENGS der Wand uebereinander liegen. Mehr ist nicht noetig - und mehr darf auch
        nicht sein, sonst entstehen wieder Luecken."""
        ka = self.kasten(a, st_a)
        if not ka:
            return []
        p_a = self.punkt(a, st_a, *pa)
        wa = self.wand(ka, p_a)
        aus = []
        for k in range(4):
            for sp in (0, 1):
                kb = self.kasten(b, (0.0, 0.0, k, sp))
                if not kb:
                    continue
                p_b = self.punkt(b, (0.0, 0.0, k, sp), *pb)
                if self.wand(kb, p_b) != (wa + 2) % 4:
                    continue
                if wa == 1:            # Tuer in a's OSTwand -> b schliesst oestlich an
                    ox = (ka[0] + ka[2]) - kb[0]
                    oy = p_a[1] - p_b[1]
                elif wa == 3:          # WESTwand
                    ox = ka[0] - (kb[0] + kb[2])
                    oy = p_a[1] - p_b[1]
                elif wa == 0:          # NORDwand
                    oy = ka[1] - (kb[1] + kb[3])
                    ox = p_a[0] - p_b[0]
                else:                  # SUEDwand
                    oy = (ka[1] + ka[3]) - kb[1]
                    ox = p_a[0] - p_b[0]
                aus.append((ox, oy, k, sp))
        return aus

    def _anbauen(self, b, lage, pix):
        """Ein Ort ohne jede Tuerverbindung auf diesem Blatt: dicht an das schon
        Gesetzte, aber ueberlappungsfrei (z.B. die Fahrstuhlkabine, die gar keine eigene
        Tuer fuehrt)."""
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
        for k in range(4):
            for sp in (0, 1):
                kb = self.kasten(b, (0.0, 0.0, k, sp))
                if not kb:
                    continue
                for (ox, oy) in ((max(xs) + 1 - kb[0], my - kb[1] - kb[3] / 2.0),
                                 (min(xs) - kb[2] - kb[0], my - kb[1] - kb[3] / 2.0),
                                 (mx - kb[0] - kb[2] / 2.0, max(ys) + 1 - kb[1]),
                                 (mx - kb[0] - kb[2] / 2.0, min(ys) - kb[3] - kb[1])):
                    st = (ox, oy, k, sp)
                    pb = self.pixel(b, st)
                    if not pb:
                        continue
                    ueb = len(pb & belegt)
                    cx = sum(p[0] for p in pb) / float(len(pb))
                    cy = sum(p[1] for p in pb) / float(len(pb))
                    f = ueb * 1000.0 + abs(cx - mx) + abs(cy - my)
                    if best is None or f < best[0]:
                        best = (f, st, pb)
        if best is None:
            best = (0.0, (0.0, 0.0, 0, 0), self.pixel(b, (0.0, 0.0, 0, 0)))
        lage[b] = best[1]
        pix[b] = best[2]

    def aufbauen(self):
        """Breitensuche durch den Tuergraph: jeder Ort wird an die Wand seines schon
        gesetzten Nachbarn gelegt. Reziproke Tueren zuerst, dann einseitige, dann der
        Anbau fuer Orte ganz ohne Verbindung."""
        if not self.zimmer:
            return {}
        grad = collections.Counter()
        for (a, _pa, b, _pb) in self.kanten:
            grad[a] += 1
            grad[b] += 1
        nb = collections.defaultdict(list)
        for (a, pa, b, pb) in self.kanten:
            nb[a].append((b, pa, pb))
            nb[b].append((a, pb, pa))
        nnb = collections.defaultdict(list)
        for (a, pa, b, pb) in self.notkanten:
            nnb[a].append((b, pa, pb))
            nnb[b].append((a, pb, pa))

        lage = {}
        for b in self.zimmer:
            if b in self.feste_lagen:
                lage[b] = self.feste_lagen[b]
        if lage:
            pass
        else:
            wurzel = getattr(self, '_wurzel', None)
            if wurzel is None or wurzel not in self.zimmer:
                wurzel = max(self.zimmer, key=lambda b: (grad[b], -b))
            lage = {wurzel: (0.0, 0.0) + self.feste_posen.get(wurzel, (0, 0))}
        pix = dict((b, self.pixel(b, lage[b])) for b in lage)
        q = sorted(lage)

        def anheften(a, b, pa, pb):
            kand = self.anlegen(a, lage[a], pa, b, pb)
            fest = self.feste_posen.get(b)
            if fest is not None:
                gefiltert = [c for c in kand if (c[2], c[3]) == fest]
                if gefiltert:
                    kand = gefiltert
            if not kand:
                # Keine Pose legt die Gegen-Tuer auf die gegenueberliegende Wand (kommt
                # vor, wenn eine Tuer in einer Ecke sitzt und die naechste Wand die
                # andere Achse ist). Dann wenigstens Tuerpunkt auf Tuerpunkt - eine
                # Ueberlappung ist allemal besser als eine Luecke.
                p = self.punkt(a, lage[a], *pa)
                for k in range(4):
                    for sp in (0, 1):
                        d = dreh(pb[0], pb[1], k, sp)
                        kand.append((p[0] - d[0] / self.ex,
                                     p[1] + d[1] / self.ey, k, sp))
            best = None
            for st in kand:
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
            weiter = None
            for a in sorted(lage):
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
            n = max(offen, key=lambda b: (grad[b], -b))
            self._anbauen(n, lage, pix)
            q.append(n)
        return lage

    # ------------------------------------------------------------------ Bewertung
    def rest(self, lage):
        """Abstand der beiden Tuerpunkte je Durchgang - das Mass, das der Spieler beim
        Durchschreiten sieht. Bei sauber angelegten Rechtecken ist er 0."""
        aus = []
        for (a, pa, b, pb) in self.kanten:
            if a not in lage or b not in lage:
                continue
            qa = self.wandpunkt(a, lage[a], self.punkt(a, lage[a], *pa))
            qb = self.wandpunkt(b, lage[b], self.punkt(b, lage[b], *pb))
            aus.append(max(abs(qa[0] - qb[0]), abs(qa[1] - qb[1])))
        return aus

    def kosten(self, lage, pix=None):
        """(Ueberlappung in %, Zahl der Durchgaenge mit deckungsgleicher Tuer, B, H)"""
        pix = pix or dict((b, self.pixel(b, lage[b])) for b in lage)
        alle = set()
        doppelt = 0
        for b in lage:
            doppelt += len(alle & pix[b])
            alle |= pix[b]
        # ⛔ NICHT MEHR "FLAECHEN BERUEHREN SICH MIT 2 px TOLERANZ". Dieses Mass war zu
        # schwach und hat echte Defekte verdeckt (Memory reai-v2-schwaches-mass).
        # Gezaehlt wird jetzt, wofuer die Karte gebaut ist: liegen die beiden Tuerpunkte
        # eines Durchgangs auf demselben Pixel?
        deckung = 0
        for (a, pa, b, pb) in self.kanten:
            if a not in lage or b not in lage:
                continue
            qa = self.wandpunkt(a, lage[a], self.punkt(a, lage[a], *pa))
            qb = self.wandpunkt(b, lage[b], self.punkt(b, lage[b], *pb))
            if max(abs(qa[0] - qb[0]), abs(qa[1] - qb[1])) <= 1.0:
                deckung += 1
        xs = [p[0] for p in alle]
        ys = [p[1] for p in alle]
        br = (max(xs) - min(xs) + 1) if xs else 0
        ho = (max(ys) - min(ys) + 1) if ys else 0
        # ⛔ DER AUSREISSER GEHOERT INS ZIEL. Ein erster Wurf zaehlte nur, WIE VIELE
        # Tueren deckungsgleich sind - die Zahl stieg von 86 auf 91, und gleichzeitig
        # sprang der schlimmste Uebergang von 49 auf 109 px. Genau den sieht der Nutzer.
        r = self.rest(lage)
        schlimmst = max(r) if r else 0.0
        return (doppelt / max(1, len(alle)) * 100.0, deckung, br, ho, schlimmst)

    def einpassen(self, lage, feld=FELD):
        pix = dict((b, self.pixel(b, lage[b])) for b in lage)
        alle = set()
        for v in pix.values():
            alle |= v
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

    def nachbessern(self, lage, runden=6):
        """Offene Tueren nachbessern.

        ⛔ DIE BREITENSUCHE BENUTZT JEDE TUER NUR EINMAL. Ist der Zielraum ueber einen
        anderen Weg schon gesetzt, bleibt DIESE Tuer unbedient - und genau dort springt
        der Marker. Hier wird fuer jede offene Tuer probiert, den einen oder anderen Raum
        stattdessen an IHR anzulegen; uebernommen wird nur, wenn dadurch insgesamt MEHR
        Tueren deckungsgleich werden (bei Gleichstand weniger Ueberlappung). Das ist ein
        Bergsteiger auf der Baumstruktur, kein Optimum - aber er repariert genau die
        Ringe, die die Reihenfolge offen gelassen hat."""
        for _ in range(runden):
            k0 = self.kosten(lage)
            bester = None
            for (a, pa, b, pb) in self.kanten:
                if a not in lage or b not in lage:
                    continue
                qa = self.wandpunkt(a, lage[a], self.punkt(a, lage[a], *pa))
                qb = self.wandpunkt(b, lage[b], self.punkt(b, lage[b], *pb))
                if max(abs(qa[0] - qb[0]), abs(qa[1] - qb[1])) <= 1.0:
                    continue                      # diese Tuer sitzt schon
                for (wer, anker, p_anker, p_wer) in ((b, a, pa, pb), (a, b, pb, pa)):
                    if wer in self.feste_lagen:
                        continue
                    for st in self.anlegen(anker, lage[anker], p_anker, wer, p_wer):
                        alt_st = lage[wer]
                        lage[wer] = st
                        k = self.kosten(lage)
                        lage[wer] = alt_st
                        if (-k[1], k[4], k[0]) < (-k0[1], k0[4], k0[0]) and                            (bester is None or (-k[1], k[4], k[0]) <
                            (-bester[0][1], bester[0][4], bester[0][0])):
                            bester = (k, wer, st)
            if bester is None:
                break
            lage[bester[1]] = bester[2]
        return lage

    def beste_lage(self):
        """⛔ DIE WURZEL ENTSCHEIDET, WELCHE RINGE AUFGEHEN.

        Die Breitensuche benutzt jede Tuer nur, solange der Zielraum noch nicht liegt.
        Ist er ueber einen anderen Weg schon gesetzt, bleibt DIESE Tuer unbedient - und
        genau dort springt der Marker. Welche Tueren das trifft, haengt allein von der
        Reihenfolge ab, und die ist frei waehlbar. Gemessen an Seite 7: mit der
        Standard-Wurzel blieben 3 von 18 Durchgaengen offen, der schlimmste 107 px.
        Deshalb wird jede Wurzel durchprobiert und die Karte mit den meisten
        deckungsgleichen Tueren genommen (bei Gleichstand die mit weniger
        Ueberlappung)."""
        if not self.zimmer:
            return {}
        if self.feste_lagen:
            return self.nachbessern(self.aufbauen())
        best = None
        for w in self.zimmer:
            self._wurzel = w
            lage = self.nachbessern(self.aufbauen())
            if not lage:
                continue
            k = self.kosten(lage)
            if best is None or (-k[1], k[4], k[0]) < (-best[1][1], best[1][4], best[1][0]):
                best = (lage, k)
        self._wurzel = None
        return best[0] if best else {}

    def loesen_roh(self):
        return self.beste_lage()

    def loesen(self, feld=FELD):
        lage = self.beste_lage()
        if not lage:
            return {}, 1.0, (0.0, 0, 0, 0, 0.0)
        lage, f = self.einpassen(lage, feld)
        return lage, f, self.kosten(lage)
