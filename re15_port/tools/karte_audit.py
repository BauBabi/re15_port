# -*- coding: utf-8 -*-
"""KARTEN-AUDIT: prueft JEDEN Raum gegen jede Invariante der gerechneten Karte.

Auftrag des Nutzers 2026-09-03: "ueberpruefe jeden einzelnen Raum auf Fehler und nimm noch
korrekturen zur perfekten Verfeinerung vor."

Gemessen wird ausschliesslich, was der Spieler SIEHT - nicht, was im Verfahren bequem ist
(Memory reai-v2-schwaches-mass). Jede Pruefung nennt die betroffenen Raeume namentlich,
damit kein Aggregat einen Einzelfall zudecken kann (reai-v2-proxy-ohne-zielmenge).

Aufruf:
    python re15_port/tools/karte_audit.py            # Zusammenfassung + alle Befunde
    python re15_port/tools/karte_audit.py --kurz     # nur die Zusammenfassung

Die LIVE-Groessen (Marker-Sprung beim Durchschreiten, Hervorhebung des aktuellen Raums)
misst der Test integration_map_uebergang bzw. unit_map_durchgang - dieses Werkzeug prueft
die erzeugte Tabelle statisch und vollstaendig.
"""
import collections
import io
import os
import re
import sys

WURZEL = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
KOPF = os.path.join(WURZEL, 're15_port', 'engine', 'src', 're15_map_zones.h')

sys.path.insert(0, os.path.join(WURZEL, 're15_port', 'tools'))
_src = io.open(os.path.join(WURZEL, 're15_port', 'tools', 'gen_map_zones.py'),
               encoding='utf-8').read()
_ns = {'__name__': 'audit', '__file__': os.path.join(WURZEL, 're15_port', 'tools',
                                                     'gen_map_zones.py')}
exec(compile(_src[:_src.index('def main(')], 'gen', 'exec'), _ns)
read_rdt = _ns['read_rdt']


# ---------------------------------------------------------------- Tabelle lesen
def _block(name):
    raus, drin = [], False
    for z in io.open(KOPF, encoding='utf-8'):
        if name in z:
            drin = True
            continue
        if drin and z.strip() == '};':
            break
        if drin:
            raus.append(z)
    return raus


ZONEN, SYNTH, ZELLEN, MARKEN = [], [], [], []
for z in _block('s_map_zones[] = {'):
    m = re.search(r'\{\s*0x([0-9A-Fa-f]{4}),(.*)\}', z)
    if not m:
        continue
    rid = int(m.group(1), 16)
    t = [int(x.strip()) for x in m.group(2).split(',')]
    ZONEN.append(dict(room=rid, variante=rid & 1, wx0=t[0], wz0=t[1], wx1=t[2], wz1=t[3],
                      page=t[4], rect=t[5], idx=t[6], zid=t[7], synth=t[14], etage=t[15]))
for z in _block('s_map_synth[] = {'):
    m = re.search(r'\{\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),'
                  r'\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\s*\}', z)
    if m:
        v = [int(x) for x in m.groups()]
        SYNTH.append(dict(x=v[0], y=v[1], w=v[2], h=v[3], erste=v[4], n=v[5],
                          A=v[6], B=v[7], C=v[8], D=v[9], E=v[10], F=v[11]))
for z in _block('s_map_marks[] = {'):
    m = re.search(r'\{\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+),\s*(\d+)\s*\}', z)
    if m:
        v = [int(x) for x in m.groups()]
        MARKEN.append(dict(i=len(MARKEN), page=v[0], rect=v[1], mx=v[2], my=v[3],
                           kind=v[4], zid=v[5], zid2=v[6]))

HAUPT = [z for z in ZONEN if not z['variante']]
ZID2RAUM = {}
for z in HAUPT:
    ZID2RAUM.setdefault(z['zid'], z['room'])


import kunst_zuordnung as _K                                # noqa: E402

_RECHTECKE = {}


def gemaltes_rechteck(page, rect):
    """Ein Rechteck aus der ausgelieferten Seiten-Tabelle @0x80076840."""
    if page not in _RECHTECKE:
        try:
            _RECHTECKE[page] = _K.rechtecke(page)
        except Exception:
            _RECHTECKE[page] = []
    rs = _RECHTECKE[page]
    if 0 <= rect < len(rs):
        _k, x, y, w, h = rs[rect]
        return (x, y, w, h)
    return None


def kasten(z):
    """Das Rechteck einer Zone - gemaltes ORIGINAL oder Schema-Zeichnung.

    ⛔ Bis 2026-09-04 lieferte diese Funktion NUR fuer Schema-Zeichnungen etwas. Das
    ging gut, solange jede Zone eine trug (gemessen: 234 von 234); sobald Zonen ihr
    gemaltes Rechteck benutzen, meldete Pruefung [B1] sie als "ohne Zeichnung" - 65
    falsche Fehler, und [F]/[G]/[H] uebersprangen sie stillschweigend.
    """
    if z['synth']:
        s = SYNTH[z['synth'] - 1]
        return (s['x'], s['y'], s['w'], s['h'])
    if z.get('rect', 255) != 255:
        return gemaltes_rechteck(z['page'], z['rect'])
    return None


def auf_karte(z, wx, wz):
    if not z['synth']:
        return None
    s = SYNTH[z['synth'] - 1]
    mx = (s['A'] * wx + s['B'] * wz) // 65536 + s['C']
    my = (s['D'] * wx + s['E'] * wz) // 65536 + s['F']
    return (min(max(mx, s['x']), s['x'] + s['w'] - 1),
            min(max(my, s['y']), s['y'] + s['h'] - 1))


def luecke(A, B):
    gx = max(0, max(A[0], B[0]) - min(A[0] + A[2], B[0] + B[2]))
    gy = max(0, max(A[1], B[1]) - min(A[1] + A[3], B[1] + B[3]))
    return max(gx, gy)


RAEUME = sorted(set(z['room'] for z in HAUPT))
RDT = {}
for _r in RAEUME:
    _g = read_rdt(_r) or read_rdt(_r + 1)
    if _g:
        RDT[_r] = _g

ZONEN_PRO_RAUM = collections.defaultdict(list)
for z in HAUPT:
    ZONEN_PRO_RAUM[z['room']].append(z)


def zone_von(room, wx, wz, page=None):
    """Wie zone_index_at in der Engine: die KLEINSTE Zone, die den Punkt enthaelt;
    sonst die naechstgelegene."""
    best, bestA, nah, nahD = None, 0, None, None
    for z in ZONEN_PRO_RAUM.get(room, ()):
        if z['etage']:
            continue
        if page is not None and z['page'] != page:
            continue
        dx = max(z['wx0'] - wx, 0, wx - z['wx1'])
        dz = max(z['wz0'] - wz, 0, wz - z['wz1'])
        d = dx * dx + dz * dz
        if nah is None or d < nahD:
            nah, nahD = z, d
        if z['wx0'] <= wx <= z['wx1'] and z['wz0'] <= wz <= z['wz1']:
            a = ((z['wx1'] - z['wx0']) // 64) * ((z['wz1'] - z['wz0']) // 64)
            if best is None or a < bestA:
                best, bestA = z, a
    return best or nah


# ---------------------------------------------------------------- Pruefungen
BEFUNDE = collections.OrderedDict()


def melde(schluessel, titel, zeilen, schwere='FEHLER'):
    BEFUNDE[schluessel] = dict(titel=titel, zeilen=zeilen, schwere=schwere)


def p_hervorhebung():
    """A) Wird der aktuelle Raum SICHTBAR rot?

    Die Engine faerbt genau die Zone rot, in welcher der Spieler steht. Liegt deren
    Rechteck vollstaendig unter dem Rechteck einer ANDEREN Zone desselben Raums, die
    frueher in der Tabelle steht (= weiter oben gezeichnet), sieht man das Rot nie.
    Genau das war ROOM1070 (Nutzer 2026-09-03)."""
    schlecht = []
    for r, zl in sorted(ZONEN_PRO_RAUM.items()):
        gez = [z for z in zl if z['synth'] and not z['etage']]
        for i, z in enumerate(gez):
            ka = kasten(z)
            if not ka:
                continue
            for w in gez:
                if w is z:
                    continue
                kb = kasten(w)
                if not kb or w['page'] != z['page']:
                    continue
                deckt = (kb[0] <= ka[0] and ka[0] + ka[2] <= kb[0] + kb[2] and
                         kb[1] <= ka[1] and ka[1] + ka[3] <= kb[1] + kb[3])
                if deckt and gez.index(w) < i:
                    schlecht.append("ROOM%04X z%d (%dx%d px) liegt komplett unter z%d "
                                    "-> Rot unsichtbar"
                                    % (r, z['idx'], ka[2], ka[3], w['idx']))
    melde('A', 'Hervorhebung des aktuellen Raums', schlecht)


def p_zeichnung():
    """B) Hat jeder Raum ueberhaupt eine Zeichnung, und ist sie brauchbar gross?"""
    ohne, winzig = [], []
    for r, zl in sorted(ZONEN_PRO_RAUM.items()):
        gez = [z for z in zl if z['synth'] and not z['etage']]
        if not gez:
            ohne.append("ROOM%04X hat KEINE Zeichnung auf irgendeinem Blatt" % r)
            continue
        for z in gez:
            ka = kasten(z)
            if ka and (ka[2] < 4 or ka[3] < 4):
                winzig.append("ROOM%04X z%d nur %dx%d px (Blatt %d)"
                              % (r, z['idx'], ka[2], ka[3], z['page']))
    melde('B1', 'Raeume ohne Zeichnung', ohne)
    melde('B2', 'Zeichnungen unter 4 px Kantenlaenge', winzig)


def p_verschachtelt():
    """C) Zonen desselben Raums duerfen einander nicht enthalten (Weltbox)."""
    schlecht = []
    for r, zl in sorted(ZONEN_PRO_RAUM.items()):
        gez = [z for z in zl if not z['etage']]
        seen = {}
        for z in gez:
            seen.setdefault(z['idx'], z)
        for a in seen.values():
            for b in seen.values():
                if a is b:
                    continue
                if (a['wx0'] <= b['wx0'] and b['wx1'] <= a['wx1'] and
                        a['wz0'] <= b['wz0'] and b['wz1'] <= a['wz1']):
                    schlecht.append("ROOM%04X: Zone %d enthaelt Zone %d"
                                    % (r, a['idx'], b['idx']))
    melde('C', 'Verschachtelte Zonen', schlecht)


def p_tuermarken():
    """D) Jede Tuer braucht ein Symbol, und ein Durchgang genau EINES."""
    ohne, doppelt = [], []
    fuer_paar = collections.defaultdict(list)
    for m in MARKEN:
        if m['kind'] >= 4:
            continue
        a = ZID2RAUM.get(m['zid']); b = ZID2RAUM.get(m['zid2'])
        if a is not None and b is not None:
            fuer_paar[tuple(sorted((a, b)))].append(m)
    for r in sorted(RDT):
        for d in RDT[r][1]:
            if d['rw'] == 0 and d['rd'] == 0 or d['dest'] not in RDT:
                continue
            za = zone_von(r, d['lx'], d['lz'])
            if not za or not za['synth']:
                continue
            paar = tuple(sorted((r, d['dest'])))
            eigen = [m for m in MARKEN if m['kind'] < 4 and
                     (m['zid'] == za['zid'] or m['zid2'] == za['zid'])]
            if not eigen:
                ohne.append("ROOM%04X -> ROOM%04X hat KEIN Symbol" % (r, d['dest']))
    # ⛔ MEHRERE SYMBOLE SIND NICHT AUTOMATISCH FALSCH. Zwei Raeume koennen mehrere
    # ECHTE Tueren haben - ROOM1000 <-> ROOM1050 drei, ROOM4040 <-> ROOM4050 zwei -, und
    # ein Raum kann Tueren zu sich selbst fuehren (ROOM4050). Ein erster Wurf dieses
    # Pruefers warf alles in einen Topf und meldete sechs Fehlalarme. Verglichen wird
    # jetzt mit der Zahl der DURCHGAENGE: je Durchgang genau ein Symbol.
    for paar, ms in sorted(fuer_paar.items()):
        a, b = paar
        # Zahl der physisch verschiedenen Tueren zwischen a und b
        tueren = set()
        for (r, ziel) in ((a, b), (b, a)):
            for d in RDT.get(r, ([], [], []))[1]:
                if d['rw'] == 0 and d['rd'] == 0 or d['dest'] != ziel:
                    continue
                # Ein Durchgang wird von beiden Seiten beschrieben; als Schluessel dient
                # der Mittelpunkt zwischen Trigger und Gegen-Spawn, gerundet.
                tueren.add((min(d['lx'] // 2000, d['nx'] // 2000),
                            max(d['lx'] // 2000, d['nx'] // 2000),
                            min(d['lz'] // 2000, d['nz'] // 2000),
                            max(d['lz'] // 2000, d['nz'] // 2000)))
        soll = max(1, len(tueren))
        if len(ms) > soll:
            weit = max(max(abs(x['mx'] - y['mx']), abs(x['my'] - y['my']))
                       for x in ms for y in ms)
            doppelt.append("ROOM%04X <-> ROOM%04X: %d Symbole fuer %d Durchgaenge, "
                           "bis %d px auseinander" % (a, b, len(ms), soll, weit))
    melde('D1', 'Tueren ohne Symbol', ohne)
    melde('D2', 'Ein Durchgang mit mehreren, weit entfernten Symbolen', doppelt)


def p_marke_auf_kante():
    """E) Ein gepaartes Symbol muss auf der Beruehrung der zwei Rechtecke sitzen."""
    schlecht = []
    zv = {}
    for z in HAUPT:
        if z['synth']:
            zv.setdefault((z['zid'], z['page']), z)
    for m in MARKEN:
        if m['kind'] >= 4 or m['zid2'] == 255:
            continue
        za = zv.get((m['zid'], m['page'])); zb = zv.get((m['zid2'], m['page']))
        if not za or not zb:
            continue
        A = kasten(za); B = kasten(zb)
        ux0 = max(A[0], B[0]); ux1 = min(A[0] + A[2], B[0] + B[2])
        uy0 = max(A[1], B[1]); uy1 = min(A[1] + A[3], B[1] + B[3])
        if ux1 <= ux0 or uy1 <= uy0:
            schlecht.append("ROOM%04X <-> ROOM%04X: Rechtecke beruehren sich NICHT "
                            "(Symbol bei %d,%d)"
                            % (ZID2RAUM.get(m['zid'], 0), ZID2RAUM.get(m['zid2'], 0),
                               m['mx'], m['my']))
            continue
        dx = max(ux0 - m['mx'], 0, m['mx'] - (ux1 - 1))
        dy = max(uy0 - m['my'], 0, m['my'] - (uy1 - 1))
        if dx + dy > 2:
            schlecht.append("ROOM%04X <-> ROOM%04X: Symbol %d px neben der gemeinsamen "
                            "Kante" % (ZID2RAUM.get(m['zid'], 0),
                                       ZID2RAUM.get(m['zid2'], 0), dx + dy))
    melde('E', 'Tuersymbol nicht auf der gemeinsamen Kante', schlecht)


def p_nachbarn():
    """F) Durch eine Tuer verbundene Raeume muessen auf demselben Blatt anstossen."""
    schlecht = []
    gesehen = set()
    for r in sorted(RDT):
        for d in RDT[r][1]:
            if d['rw'] == 0 and d['rd'] == 0 or d['dest'] not in RDT or d['dest'] == r:
                continue
            k = tuple(sorted((r, d['dest'])))
            if k in gesehen:
                continue
            za = zone_von(r, d['lx'], d['lz'])
            zb = zone_von(d['dest'], d['nx'], d['nz'])
            if not za or not zb or not za['synth'] or not zb['synth']:
                continue
            if za['page'] != zb['page']:
                continue
            gesehen.add(k)
            L = luecke(kasten(za), kasten(zb))
            if L > 0:
                schlecht.append("ROOM%04X <-> ROOM%04X: %d px Luecke (Blatt %d)"
                                % (k[0], k[1], L, za['page']))
    melde('F', 'Verbundene Raeume stossen nicht aneinander', schlecht)


def p_marker_im_raum():
    """G) Der Marker muss fuer JEDEN begehbaren Punkt im eigenen Rechteck landen."""
    schlecht = []
    for r in sorted(RDT):
        zellen = [c for c in RDT[r][0] if c[2] > 0 and c[3] > 0]
        if not zellen:
            continue
        raus = 0
        proben = 0
        for c in zellen:
            for (fx, fz) in ((0.5, 0.5), (0.15, 0.15), (0.85, 0.85), (0.15, 0.85),
                             (0.85, 0.15)):
                wx = int(c[0] + c[2] * fx); wz = int(c[1] + c[3] * fz)
                z = zone_von(r, wx, wz)
                if not z or not z['synth']:
                    continue
                proben += 1
                p = auf_karte(z, wx, wz)
                ka = kasten(z)
                if not (ka[0] <= p[0] < ka[0] + ka[2] and ka[1] <= p[1] < ka[1] + ka[3]):
                    raus += 1
        if proben and raus * 100 // proben > 0:
            schlecht.append("ROOM%04X: %d von %d Proben ausserhalb des eigenen Rechtecks"
                            % (r, raus, proben))
    melde('G', 'Marker faellt aus dem eigenen Rechteck', schlecht)


def p_massstab():
    """H) Das Rechteck muss die Weltform treffen (Seitenverhaeltnis, mit Drehung)."""
    schlecht = []
    for z in HAUPT:
        if not z['synth'] or z['etage']:
            continue
        ka = kasten(z)
        bw = z['wx1'] - z['wx0']; bd = z['wz1'] - z['wz0']
        if bw <= 0 or bd <= 0 or ka[2] <= 0 or ka[3] <= 0:
            continue
        welt = float(bw) / bd
        gerade = float(ka[2]) / ka[3]
        gedreht = float(ka[3]) / ka[2]
        f = min(abs(welt / gerade - 1.0), abs(welt / gedreht - 1.0))
        if f > 0.45:
            schlecht.append("ROOM%04X z%d: Welt %d:%d, Rechteck %d:%d px -> %.0f %% "
                            "Formfehler" % (z['room'], z['idx'], bw, bd, ka[2], ka[3],
                                            f * 100))
    melde('H', 'Rechteckform passt nicht zur Weltform', schlecht, 'HINWEIS')


def p_treppen():
    """I) Jede Treppen-Zone braucht ein Symbol."""
    fehlt = []
    for r in sorted(RDT):
        n_t = len(RDT[r][2])
        if not n_t:
            continue
        zl = [z for z in ZONEN_PRO_RAUM.get(r, ()) if z['synth'] and not z['etage']]
        if not zl:
            continue
        zids = set(z['zid'] for z in zl)
        n_m = sum(1 for m in MARKEN if m['kind'] >= 4 and
                  (m['zid'] in zids or m['zid2'] in zids))
        if n_m == 0:
            fehlt.append("ROOM%04X hat %d Treppen-Zonen, aber KEIN Treppensymbol"
                         % (r, n_t))
    melde('I', 'Treppen ohne Symbol', fehlt)


def p_blattwechsel():
    """J) Eine gewoehnliche Tuer sollte innerhalb einer Stage das Blatt nicht wechseln."""
    schlecht = []
    for r in sorted(RDT):
        for d in RDT[r][1]:
            if d['rw'] == 0 and d['rd'] == 0 or d['dest'] not in RDT:
                continue
            if (r >> 8) != (d['dest'] >> 8):
                continue          # andere Stage: legitim
            za = zone_von(r, d['lx'], d['lz'])
            zb = zone_von(d['dest'], d['nx'], d['nz'])
            if not za or not zb or not za['synth'] or not zb['synth']:
                continue
            if za['page'] != zb['page']:
                # Ist der Zielraum auf MEINEM Blatt als Gast gezeichnet?
                gast = any(z['room'] == d['dest'] and z['page'] == za['page'] and z['synth']
                           for z in ZONEN_PRO_RAUM.get(d['dest'], ()))
                if not gast:
                    schlecht.append("ROOM%04X (Blatt %d) -> ROOM%04X (Blatt %d), kein "
                                    "Gast auf meinem Blatt"
                                    % (r, za['page'], d['dest'], zb['page']))
    melde('J', 'Tuer wechselt innerhalb einer Stage das Blatt', schlecht, 'HINWEIS')


def p_ueberlappung():
    """K) Wie stark ueberlappen sich die Zeichnungen je Blatt?"""
    zeilen = []
    proBlatt = collections.defaultdict(list)
    _gesehen = set()
    for z in HAUPT:
        # Dieselbe Zeichnung nur EINMAL: Varianten-Raeume teilen sie sich, auf dem
        # Schirm liegt dort ein einziges Rechteck.
        if z['synth'] and (z['page'], z['synth']) not in _gesehen:
            _gesehen.add((z['page'], z['synth']))
            proBlatt[z['page']].append(z)
    for pg in sorted(proBlatt):
        pix = collections.Counter()
        for z in proBlatt[pg]:
            ka = kasten(z)
            for x in range(ka[0], ka[0] + ka[2]):
                for y in range(ka[1], ka[1] + ka[3]):
                    pix[(x, y)] += 1
        if not pix:
            continue
        doppelt = sum(1 for v in pix.values() if v > 1)
        q = doppelt * 100.0 / len(pix)
        if q > 20:
            zeilen.append("Blatt %2d: %.0f %% der Flaeche von mehreren Raeumen belegt "
                          "(%d Orte)" % (pg, q, len(proBlatt[pg])))
    melde('K', 'Blaetter mit starker Ueberlappung', zeilen, 'HINWEIS')


def p_doppelslot():
    """L) Zwei Aot_set-Records auf DEMSELBEN Slot - nur einer kann aktiv sein.

    Der Generator liest beide als Tuer und laesst sich von beiden einschraenken; im Spiel
    ueberschreibt der zweite den ersten. Gefunden bei ROOM5050 und ROOM5060 (je Slot 0
    zweimal: einmal -> ROOM5040, einmal -> ROOM5120)."""
    schlecht = []
    for r in sorted(RDT):
        proSlot = collections.defaultdict(list)
        for d in RDT[r][1]:
            if d['rw'] == 0 and d['rd'] == 0:
                continue
            proSlot[d.get('slot')].append(d)
        for sl, ds in sorted(proSlot.items(), key=lambda q: (q[0] is None, q[0])):
            if len(ds) > 1 and len(set(x['dest'] for x in ds)) > 1:
                schlecht.append("ROOM%04X Slot %s zweimal belegt: -> %s"
                                % (r, sl, ", ".join("ROOM%04X" % x['dest'] for x in ds)))
    melde('L', 'Zwei Tueren auf demselben Aot_set-Slot', schlecht, 'HINWEIS')


def p_ankunft():
    """M) Wo erscheint der Spieler nach dem Durchgang - im Zielrechteck, nah an der Wand?

    Das ist die Groesse, die der Nutzer beim Raumwechsel SIEHT. Gemessen wird der Abstand
    des Erscheinungspunkts zur gemeinsamen Kante der beiden Rechtecke."""
    weit = []
    for r in sorted(RDT):
        for d in RDT[r][1]:
            if d['rw'] == 0 and d['rd'] == 0 or d['dest'] not in RDT:
                continue
            za = zone_von(r, d['lx'], d['lz'])
            zb = zone_von(d['dest'], d['nx'], d['nz'])
            if not za or not zb or not za['synth'] or not zb['synth']:
                continue
            if za['page'] != zb['page']:
                continue
            A = kasten(za); B = kasten(zb)
            ux0 = max(A[0], B[0]); ux1 = min(A[0] + A[2], B[0] + B[2])
            uy0 = max(A[1], B[1]); uy1 = min(A[1] + A[3], B[1] + B[3])
            if ux1 <= ux0 or uy1 <= uy0:
                continue
            p = auf_karte(zb, d['nx'], d['nz'])
            dx = max(ux0 - p[0], 0, p[0] - (ux1 - 1))
            dy = max(uy0 - p[1], 0, p[1] - (uy1 - 1))
            if dx + dy > 12:
                weit.append("ROOM%04X -> ROOM%04X: Erscheinungspunkt %d px von der "
                            "gemeinsamen Kante" % (r, d['dest'], dx + dy))
    melde('M', 'Erscheinungspunkt weit von der gemeinsamen Kante', weit, 'HINWEIS')


def p_symbolabstand():
    """N) Zwei Symbole VERSCHIEDENER Durchgaenge duerfen nicht aufeinander liegen."""
    dicht = []
    T = [m for m in MARKEN if m['kind'] < 4]
    for i in range(len(T)):
        for j in range(i + 1, len(T)):
            a, b = T[i], T[j]
            if a['page'] != b['page']:
                continue
            if {a['zid'], a['zid2']} == {b['zid'], b['zid2']}:
                continue
            if abs(a['mx'] - b['mx']) + abs(a['my'] - b['my']) <= 2:
                dicht.append("Blatt %d (%d,%d): ROOM%s<->ROOM%s und ROOM%s<->ROOM%s"
                             % (a['page'], a['mx'], a['my'],
                                '%04X' % ZID2RAUM.get(a['zid'], 0),
                                '%04X' % ZID2RAUM.get(a['zid2'], 0),
                                '%04X' % ZID2RAUM.get(b['zid'], 0),
                                '%04X' % ZID2RAUM.get(b['zid2'], 0)))
    melde('N', 'Symbole verschiedener Durchgaenge liegen aufeinander', dicht)


def p_gast():
    """O) Eine GAST-Zeile muss ihre Hauptzeile als Zwilling haben (gleiche Weltbox)."""
    schlecht = []
    for z in ZONEN:
        if z['variante'] or not z['etage']:
            continue
        haupt = [w for w in HAUPT if w['room'] == z['room'] and w['idx'] == z['idx']
                 and not w['etage']]
        if not haupt:
            schlecht.append("ROOM%04X z%d ist NUR als Gast (Blatt %d) vorhanden"
                            % (z['room'], z['idx'], z['page']))
            continue
        h = haupt[0]
        if (h['wx0'], h['wz0'], h['wx1'], h['wz1']) != (z['wx0'], z['wz0'], z['wx1'], z['wz1']):
            schlecht.append("ROOM%04X z%d: Gast (Blatt %d) und Hauptzeile (Blatt %d) "
                            "haben verschiedene Weltboxen"
                            % (z['room'], z['idx'], z['page'], h['page']))
    melde('O', 'Gast-Zeilen ohne passende Hauptzeile', schlecht)


def p_unsichtbar():
    """P) Raeume mit Kollisionsgeometrie, die auf KEINEM Blatt gezeichnet sind.

    Gemessen 2026-09-03: sieben, alle zu Recht - vier sind byte-gleiche Platzhalter-
    Schablonen (ROOM1240/ROOM6040 ein hohler Vier-Wand-Rahmen, ROOM5070/ROOM5130 dieselbe
    75-Zellen-Form), ROOM1260 liegt im Original selbst ausserhalb der Seitentabelle
    (sltiu 0x26 @0x8004b574), ROOM20A0/ROOM20B0 haben Seite 0xd, die keine echte
    Kartenseite ist. Kommt ein ACHTER dazu, ist etwas kaputt."""
    import os as _os
    STAGE = _os.path.join(WURZEL, 're15_port', 'shared_assets', 'PSX')
    inTab = set(z['room'] for z in ZONEN)
    zeilen = []
    for st in range(1, 7):
        d = _os.path.join(STAGE, 'STAGE%d' % st)
        if not _os.path.isdir(d):
            continue
        for f in sorted(_os.listdir(d)):
            if not f.startswith('ROOM') or not f.endswith('.RDT'):
                continue
            rid = int(f[4:8], 16)
            if (rid & 1) or rid in inTab:
                continue
            g = read_rdt(rid)
            if not g:
                continue
            zellen = [c for c in g[0] if c[2] > 0 and c[3] > 0]
            if not zellen:
                continue
            rein = sorted(set(q for q in RDT
                              for e in RDT[q][1]
                              if e['dest'] == rid and not (e['rw'] == 0 and e['rd'] == 0)))
            zeilen.append("ROOM%04X: %d Zellen, erreichbar von %s"
                          % (rid, len(zellen),
                             ", ".join("ROOM%04X" % q for q in rein) if rein
                             else "niemandem"))
    melde('P', 'Raeume mit Geometrie, aber ohne Zeichnung', zeilen, 'HINWEIS')


def main():
    kurz = '--kurz' in sys.argv
    for f in (p_hervorhebung, p_zeichnung, p_verschachtelt, p_tuermarken,
              p_marke_auf_kante, p_nachbarn, p_marker_im_raum, p_massstab,
              p_treppen, p_blattwechsel, p_ueberlappung,
              p_doppelslot, p_ankunft, p_symbolabstand, p_gast,
              p_unsichtbar):
        f()
    print("=== KARTEN-AUDIT: %d Raeume, %d Zonen, %d Zeichnungen, %d Marken ==="
          % (len(RAEUME), len(HAUPT), len(SYNTH), len(MARKEN)))
    print()
    fehler = 0
    for k, b in BEFUNDE.items():
        n = len(b['zeilen'])
        if b['schwere'] == 'FEHLER':
            fehler += n
        marke = 'OK    ' if n == 0 else ('%-6s' % b['schwere'])
        print("  [%s] %s %-52s %d" % (k, marke, b['titel'], n))
    print()
    print("  Summe echter Fehler: %d" % fehler)
    if kurz:
        return 0 if fehler == 0 else 1
    for k, b in BEFUNDE.items():
        if not b['zeilen']:
            continue
        print()
        print("--- [%s] %s (%s, %d) ---" % (k, b['titel'], b['schwere'], len(b['zeilen'])))
        for z in b['zeilen'][:60]:
            print("   " + z)
        if len(b['zeilen']) > 60:
            print("   ... und %d weitere" % (len(b['zeilen']) - 60))
    return 0 if fehler == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
