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

DREI QUELLEN FUER DIESEN PUNKT, nicht eine (siehe _paar_punkte, BEFUND.md §29):
Trigger<->Trigger (genau, Median 7 px), einseitige Tueren ueber den Spawn des Nachbarn
(9 px) und der Spawn im selben Datensatz (9 px). Die ungenauen sind Rueckfallebene, keine
Beimischung - gemischt wird das Ergebnis schlechter.

⛔ DER RIEGEL IST DIE AUSLASSPROBE. Fuer jeden Raum, der eine ausgelieferte Zeile HAT,
wird so getan, als fehlte sie; die Zeile wird hergeleitet und mit der echten verglichen.
Ohne diese Probe waere die Herleitung eine Behauptung. Und sie muss ihre ABDECKUNG
mitmelden (Memory reai-v2-schiene-abdeckung): sie prueft 26 der 38 ausgelieferten
Zeilen - die uebrigen 12 haengen an Nachbarn, die selbst keine ausgelieferte Zeile haben,
und sind damit nicht pruefbar, ohne die Kette durch bereits hergeleitete Zeilen zu
fuehren. Gemessen: Median 7 px, 16 von 26 innerhalb 8 px, die besten exakt.

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

NL = chr(10)          # der Header wird mit LF geschrieben (.gitattributes)


# ⛔ ORIENTIERUNG (2026-09-09, Nutzer-Marken 3-6): jeder Raum hat ein EIGENES lokales
# System; nichts zwingt Welt-+z eines Raums auf Karten-oben. FLIPS[raum] = (fx, fz)
# traegt die je Raum ERKANNTE Spiegelung der hergeleiteten Zeile (Ordnungs-Signatur,
# s. herleiten); die ausgelieferten Zeilen definieren ihre Orientierung selbst (lhu,
# positiv) und bekommen nie einen Eintrag.
FLIPS = {}
# Raeume mit AUSGELIEFERTER Zeile - nur sie sind erstklassige Zeugen fuer die
# Ordnungs-Signatur (eine selbst hergeleitete Nachbar-Zeile kann selbst schief
# stehen; ROOM1180 bekam so einen falschen Flip ueber den 11D0-Punkt).
AUSGELIEFERT = set()


def karte_x(w, ox, sx, flip=0):
    v = ((((w + 32000) * 10 * sx) >> 20) + 5) // 10
    return (-v if flip else v) + ox


def karte_y(w, oy, sz, flip=0):
    t = -(((((w + 32000) * 10 * sz) >> 20)) + 5)
    v = (t // 10 if t >= 0 else -((-t) // 10))
    return (-v if flip else v) + oy


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


def gegentuer(a, b, eigene=None):
    """Der Datensatz in B, der zurueck nach A fuehrt.

    ⛔ Bei ZWEI Tueren zum selben Nachbarn (ROOM1010<->ROOM1020) nahm der
    Erste-Treffer-Weg fuer BEIDE eigenen Tueren dieselbe Gegentuer - der
    Zeilen-Fit mischte damit Kreuz-Korrespondenzen (Nutzer-Marken 3-6,
    2026-09-09). Die Gegentuer ist der Record, dessen SPAWN im eigenen Raum
    der eigenen Tuer am naechsten liegt - reine Weltdaten, kein Fit.
    """
    kand = [d for d in tueren(b) if d['dest'] == a]
    if not kand:
        return None
    if eigene is None or len(kand) == 1:
        return kand[0]
    return min(kand, key=lambda d: (d['nx'] - eigene['lx']) ** 2
                                 + (d['nz'] - eigene['lz']) ** 2)


def _kartenpunkt(a, wx, wz, bekannt):
    z = bekannt[a]
    f = FLIPS.get(a, (0, 0))
    return (karte_x(wx, z[0], z[2], f[0]), karte_y(wz, z[1], z[3], f[1]))


# Rangfolge der Korrespondenz-Familien. Gemessen 2026-09-03 an der Auslassprobe, jede
# Familie EINZELN (BEFUND.md §29): trigger<->trigger Median 7 px, spawn 9 px, einseitig
# 9 px. Gemischt wird es SCHLECHTER (8 px) - der Spawn liegt nicht in der Tuer, sondern
# ein Stueck im Raum, das ist ein systematischer Versatz. Also: die genaue Familie
# gewinnt, die ungenauen sind nur Rueckfallebene fuer Raeume, die sonst NICHTS haetten.
FAMILIEN = ('trigger', 'einseitig', 'spawn')


def _paar_punkte(b, bekannt):
    """Fuer Raum b: (lokaler Tuerpunkt in b, Kartenpunkt, Nachbar) ueber bekannte Nachbarn.

    ⛔ KEIN ZONEN-FILTER. Ein frueherer Wurf sperrte mehrzonige Raeume auf BEIDEN Seiten
    ("bei Mehrzonen-Raeumen ist unklar, welcher"). Das war fuer den EMPFAENGER falsch: die
    Zeile @0x800768b0 steht pro SLOT = pro RAUM (re15_inv_screen.c:273-320), nicht pro
    Zone, und der Tuerpunkt ist eine Weltkoordinate im lokalen System des Raums - wie
    viele gemalte Rechtecke der Raum bedeckt, kommt in der Rechnung gar nicht vor.
    Gemessen: der mehrzonige Empfaenger ROOM30E0 leitet mit 7 px her, genau wie die
    einzonigen; die Abdeckung der Auslassprobe steigt von 22 auf 24 von 38.
    """
    fam = collections.defaultdict(list)
    def darf(a):
        return (a != b and a in bekannt and BLATT.get(a) is not None
                and BLATT.get(a) == BLATT.get(b))   # ueber Blattgrenzen gibt es
                                                    # keinen gemeinsamen Punkt
    for d in tueren(b):
        a = d['dest']
        if not darf(a):
            continue
        # (1) Trigger in B <-> Trigger in A: beide sind die AOT-Flaeche AN der Tuer.
        da = gegentuer(b, a, d)
        if da:
            fam['trigger'].append(((d['lx'], d['lz']),
                                   _kartenpunkt(a, da['lx'], da['lz'], bekannt), a))
        # (2) Trigger in B <-> SPAWN in A. Steht im SELBEN Datensatz (Door_aot_set
        #     nx/nz = Ankunftspunkt im Zielraum, gen_map_zones.py:335) und braucht
        #     deshalb keine Gegentuer.
        fam['spawn'].append(((d['lx'], d['lz']),
                             _kartenpunkt(a, d['nx'], d['nz'], bekannt), a))
    # (3) EINSEITIGE Tueren: A kennt B, B kennt A nicht. Ein Durchlauf nur ueber
    #     tueren(b) sieht diese Verbindung gar nicht - es waren 5 Raeume (ROOM1080,
    #     ROOM4020, ROOM4040 und in der Folge ROOM4050/ROOM4070), die deshalb ohne
    #     Zeile blieben, obwohl ein Nachbar mit Zeile direkt daneben lag.
    for a in bekannt:
        if not darf(a):
            continue
        for e in tueren(a):
            if e['dest'] == b:
                fam['einseitig'].append(((e['nx'], e['nz']),
                                         _kartenpunkt(a, e['lx'], e['lz'], bekannt), a))
    for f in FAMILIEN:
        if fam.get(f):
            return fam[f], f
    return [], None


def _median(v):
    v = sorted(v)
    return v[len(v) // 2]


def herleiten(b, bekannt):
    """Zeile fuer b herleiten. Rueckgabe (zeile, guete) oder (None, grund)."""
    pk, welche = _paar_punkte(b, bekannt)
    if not pk:
        return None, 'kein bekannter Nachbar auf dem Blatt'

    # --- DIE ACHSEN GETRENNT LOESEN ----------------------------------------------
    # ⛔ Ein erster Wurf verlangte von EINEM Tuerpaar Abstand in BEIDEN Achsen. Zwei
    # Tueren an derselben Wand unterscheiden sich aber nur in EINER - damit fiel der
    # Zwei-Tueren-Weg fast immer aus (1 von 96 Raeumen), und fast ueberall musste der
    # Massstab geliehen werden, was der groesste Fehleranteil ist. x und z sind
    # unabhaengige Gleichungen; sie duerfen aus VERSCHIEDENEN Paaren kommen.
    # ⛔ ORIENTIERUNG ZUERST (Nutzer-Marken 3-6, 2026-09-09): die ORDNUNG der
    # Tuerpunkte entlang jeder Achse ist ein freier Zeuge ohne Offset-Fit. Normal
    # gilt: Welt-x steigt -> Karten-x steigt; Welt-z steigt -> Karten-y FAELLT
    # (FUN_800473f8 negiert die z-Haelfte). Sagen ALLE nutzbaren Paare das
    # Gegenteil, ist die Achse des Raums gegen die Karte gespiegelt. Das
    # Offset-Residuum kann das bei 2 Punkten prinzipiell nicht sehen (mit je neu
    # gefittetem Median ist es vorzeichen-symmetrisch) - ROOM1010 stand deshalb
    # monatelang spiegelverkehrt: beide Tuermarken ueber Kreuz, 24 px, und der
    # Marker sprang beim Durchgang von der oberen zur unteren Tuer.
    def _ordnung(achse):
        stimmen = 0; gesamt = 0
        for i in range(len(pk)):
            for j in range(i + 1, len(pk)):
                (b1, P1, a1), (b2, P2, a2) = pk[i], pk[j]
                # ⛔ NUR ERSTKLASSIGE ZEUGEN: beide Kartenpunkte ueber AUSGELIEFERTE
                # Nachbarzeilen, und der Karten-Abstand deutlich ueber dem
                # Rauschboden (Zeilen-Median 5 px). ROOM1180 bekam sonst einen
                # falschen z-Flip: sein Welt-z bildet kaum auf Karten-y ab
                # (19,5k Welt -> 4 px), und der dritte Zeuge lief ueber die
                # selbst hergeleitete 11D0-Zeile.
                if a1 not in AUSGELIEFERT or a2 not in AUSGELIEFERT:
                    continue
                dw = b1[achse] - b2[achse]
                dP = P1[achse] - P2[achse]
                if abs(dw) < 1500 or abs(dP) < 6:
                    continue
                gesamt += 1
                erwartet = dw * dP         # x: gleichsinnig; z: gegensinnig
                if achse == 1:
                    erwartet = -erwartet
                if erwartet < 0:
                    stimmen += 1
        if gesamt == 0:
            return 0
        if stimmen == gesamt:
            return 1                       # ALLE Paare widersprechen -> Flip
        if stimmen > 0:
            return -1                      # uneins -> kein Urteil, kein Flip
        return 0

    _sig_x, _sig_z = _ordnung(0), _ordnung(1)
    fx = 1 if _sig_x == 1 else 0
    fz = 1 if _sig_z == 1 else 0

    def loese(achse):
        best = None
        flip = fx if achse == 0 else fz
        for i in range(len(pk)):
            for j in range(i + 1, len(pk)):
                (b1, P1, a1), (b2, P2, a2) = pk[i], pk[j]
                d = b1[achse] - b2[achse]
                if abs(d) < 3000:
                    continue               # zu nah: die Division wird beliebig
                dp = P1[achse] - P2[achse]
                s = 1048576.0 * (dp if achse == 0 else -dp) / d
                if flip:
                    s = -s
                s = int(round(s))
                if not (SKALA_MIN <= s <= SKALA_MAX):
                    continue
                if best is None or abs(d) > best[0]:
                    best = (abs(d), s, a1, a2)
        return best

    lx, lz = loese(0), loese(1)

    # ⛔ ZWEITER ZEUGE (ROOM2060): findet die Normal-Variante KEINEN gueltigen
    # Massstab, die gespiegelte aber schon, und streut deren Offset-Fit enger,
    # ist die Achse gespiegelt - auch ohne ausgelieferten Nachbarn. Die Signatur
    # hat Vorrang; hier geht es nur um Achsen OHNE deren Urteil.
    def _streuung(achse, flip, skala):
        f = (karte_x, karte_y)[achse]
        w = sorted(P[achse] - f(bl[achse], 0, skala, flip) for bl, P, _a in pk)
        med = w[len(w) // 2]
        return sum(abs(v - med) for v in w) / len(w)
    _ref_nachbar = pk[0][2]
    for _achse, _sig in ((0, _sig_x), (1, _sig_z)):
        if _sig != 0:
            continue
        _norm = loese(_achse)
        _alt_flip = (fx, fz)
        if _achse == 0: fx = 1
        else:           fz = 1
        _gefl = loese(_achse)
        _ref = bekannt[_ref_nachbar][2 if _achse == 0 else 3]
        if _norm is None and _gefl is not None and len(pk) >= 2 and            _streuung(_achse, 1, _gefl[1]) + 2 < _streuung(_achse, 0, _ref):
            if _achse == 0: lx = _gefl
            else:           lz = _gefl
        else:
            fx, fz = _alt_flip
    lx = lx if lx else loese(0)
    lz = lz if lz else loese(1)
    a1 = pk[0][2]
    sx = lx[1] if lx else bekannt[a1][2]
    sz = lz[1] if lz else bekannt[a1][3]
    # ⛔ DER VERSATZ IST DER MEDIAN ALLER TUERPUNKTE, nicht der des ersten. Eine
    # einzelne Tuer ist in beiden Raeumen nicht exakt derselbe Ort: an den Raeumen mit
    # ausgelieferter Zeile liegen die zwei Kartenpunkte EINER Tuer im Median 5 px
    # auseinander (BEFUND.md §29). Das ist der Rauschboden - mitteln druckt ihn weg,
    # und der Median haelt dabei die Ausreisser draussen (ROOM30E0/ROOM3030: 60 px,
    # weil der Korridor IN der Zone des Nachbarn liegt, §25).
    ox = _median([P[0] - karte_x(bl[0], 0, sx, fx) for bl, P, _a in pk])
    oy = _median([P[1] - karte_y(bl[1], 0, sz, fz) for bl, P, _a in pk])
    if fx or fz:
        FLIPS[b] = (fx, fz)
    else:
        FLIPS.pop(b, None)
    wie = ('%s x%d, Massstab x %s, z %s%s'
           % (welche, len(pk),
              'aus %04X/%04X' % (lx[2], lx[3]) if lx else 'geliehen von %04X' % a1,
              'aus %04X/%04X' % (lz[2], lz[3]) if lz else 'geliehen von %04X' % a1,
              ', GESPIEGELT %s%s (Ordnungs-Signatur)'
              % ('x' if fx else '', 'z' if fz else '') if (fx or fz) else ''))
    return (ox, oy, sx, sz), wie


def feinschliff(rm, z):
    """⛔ GEMESSEN UND ZWEIMAL VERWORFEN (2026-09-03) - nur noch ueber RE15_FEINSCHLIFF=1.

    Die Idee: die hergeleitete Zeile an den gemalten Rechtecken nachziehen, weil die
    Kunst die staerkere Fessel sei. Erster Anlauf (BEFUND.md §28): Auslassprobe faellt von
    7 px auf 15 px. Der Grund war mein alter Fehler (Memory reai-v2-proxy-mass) - maximiert
    wurde die Deckung mit IRGENDEINEM Rechteck des Blattes, also zog der Raum auf den
    dichtesten Haufen statt auf SEIN Rechteck.

    Zweiter Anlauf mit genau der Korrektur, die §28 dafuer verlangt hat (BEFUND.md §29):
    erst mit der groben Zeile das Rechteck ZUORDNEN, dann die Zeile allein auf DIESES
    Rechteck anpassen, Massstab aus dem Groessenverhaeltnis. Ergebnis Median 14 px - fuenf
    Raeume gewinnen spektakulaer (ROOM3090 51->5, ROOM3010 18->3), zwanzig verlieren.
    Ein Riegel ueber die Groessen-Uebereinstimmung rettet es nicht: bei JEDER Schwelle
    von 0 bis 24 px werden mehr Raeume schlechter als besser.

    ⛔ DER GRUND IST MESSBAR UND SCHLIESST DEN WEG: die gemalte Kunst ist auf ein
    8-PIXEL-RASTER gezeichnet - alle 236 Kantenlaengen der 118 Rechtecke ueber alle 13
    Blaetter sind durch 8 teilbar. Ein Massstab aus dem Groessenverhaeltnis erbt diese
    Rasterung: bei einem 24 px breiten Rechteck sind das 33 % Massstabsfehler, bei dem
    8 px breiten Korridor ROOM3030 sogar 100 %. Die Tuerkette ist mit ihrem Rauschboden
    von 5 px die deutlich staerkere Fessel. Der Weg ist damit nicht "noch nicht sauber
    gebaut", sondern durch die Aufloesung der Vorlage begrenzt.

    Der MASSSTAB wird hier NICHT angetastet - ihn aus derselben groben Lage neu zu
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
    AUSGELIEFERT.clear()
    AUSGELIEFERT.update(bekannt)
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


def schreiben():
    """Header mit den HERGELEITETEN Zeilen erzeugen.

    ⛔ NUR DIE FEHLENDEN. Wo das Original eine echte Zeile ausliefert, gilt DIE - sie ist
    die Wahrheit, die hergeleitete waere bestenfalls gleich gut. Geschrieben werden also
    ausschliesslich Slots, deren ausgelieferte Zeile ein Platzhalter ist (Massstab 1).
    """
    bekannt, quelle = alle_zeilen()
    zeilen = []
    for rm in sorted(bekannt):
        if K.zeile(rm):
            continue                       # ausgeliefert -> Original gewinnt
        slot = K.SLOT_BASIS[(rm >> 12) & 0xF] + ((rm >> 4) & 0xFF)
        if slot > 105:
            continue
        ox, oy, sx, sz = bekannt[rm]
        if not (-32768 <= ox < 32768 and 0 <= oy < 65536):
            continue
        if not (SKALA_MIN <= sx <= SKALA_MAX and SKALA_MIN <= sz <= SKALA_MAX):
            continue
        zeilen.append((slot, rm, ox, oy & 0xFFFF, sx, sz, quelle.get(rm, '')))

    o = []
    o.append("/* GENERIERT von tools/gen_marker_zeilen.py - HERGELEITETE MARKER-ZEILEN.")
    o.append(" *")
    o.append(" * Die Tabelle @0x800768b0 setzt die lokalen Koordinaten EINES Raums auf die")
    o.append(" * gemalte Karte ({x_off, y_off, x_scale, z_scale}; Formel FUN_800473f8")
    o.append(" * @0x8004741c-0x80047528). Von 106 Slots tragen nur 39 eine echte Zeile - die")
    o.append(" * uebrigen haben Massstab 1, und die Formel bildet dort JEDE Weltposition auf")
    o.append(" * denselben Punkt ab. Das Kartensystem des Prototyps ist unfertig.")
    o.append(" *")
    o.append(" * Diese Zeilen sind HERGELEITET, nicht ausgeliefert: eine Tuer ist derselbe Ort")
    o.append(" * in zwei raumlokalen Systemen, also ist die Zeile des Nachbarn ueber sie")
    o.append(" * ausrechenbar. Riegel ist die Auslassprobe - jede AUSGELIEFERTE Zeile einmal")
    o.append(" * versteckt und hergeleitet: Median 7 px, 16 von 26 innerhalb 8 px.")
    o.append(" * PORT-ERGAENZUNG, ausdruecklich keine Rekonstruktion. Wo das Original eine")
    o.append(" * echte Zeile hat, steht sie hier NICHT - dort gilt das Original. */")
    o.append("typedef struct {")
    o.append("    unsigned char  slot;")
    o.append("    short          ox;")
    o.append("    unsigned short oy, sx, sz;")
    o.append("} re15_map_zeile_t;")
    o.append("")
    o.append("static const re15_map_zeile_t s_map_zeilen[] = {")
    for slot, rm, ox, oy, sx, sz, wie in zeilen:
        o.append("    { %3d, %6d, %5d, %5d, %5d },   /* ROOM%04X  %s */"
                 % (slot, ox, oy, sx, sz, rm, wie))
    o.append("};")
    o.append("")
    ziel = os.path.join(HIER, '..', 'engine', 'src', 're15_map_zeilen.h')
    io.open(ziel, 'w', encoding='utf-8', newline=NL).write(NL.join(o) + NL)
    print("%d hergeleitete Zeilen -> engine/src/re15_map_zeilen.h" % len(zeilen))
    return 0


def main(argv):
    if '--schreiben' in argv:
        return schreiben()
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
