# -*- coding: utf-8 -*-
"""Zensus A, Schritt 6: KOSTET die Redundanz etwas?

Die Engine liest hart hoechstens 105 Rechtecke je Cut (re15_pri.h:50,
RE15_PRI_MAX_MASKS_PER_CUT; pri_common.c:125 verwirft alles danach STILL). Die Kachelung
in anwenden.bau_objektweise startet auf der GROEBSTEN Kante und verfeinert nur, solange
Rechteckzahl und Atlasflaeche halten (anwenden.py:436-465). Jedes Objekt, das nichts
beitraegt, verbraucht also Budget und macht die Kachelung der uebrigen GROEBER - und eine
groebere Kachel traegt EINE Tiefe ueber bis zu 64x64 Punkte.

Gemessen wird derselbe Bau zweimal: mit allen Objekten (Auslieferungsstand) und ohne die
Objekte, die im Schritt 5 als beitragslos belegt sind. Geschrieben wird nur in build/r19,
NICHT in shared_assets.
"""
import io, json, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum, anwenden

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
TMP = 'build/r19/tmp'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
DUMP = 'build/floor_dump.txt'

# Im Schritt 5 belegt beitragslos: 0 eigene Punkte UND 0 Punkte mit der naechsten Tiefe
TOT = {
    ('ROOM10F0', 4): ['Buerostuhl x-1600 z12200', 'Buerostuhl x-1800 z8200', 'Buerostuhl x-1900 z4300'],
    ('ROOM10F0', 5): ['Buerostuhl x1600 z5700', 'Buerostuhl x1600 z1900', 'Buerostuhl x1500 z-2000'],
    ('ROOM1100', 1): ['Suedblock', 'Suedwestwand'],
    ('ROOM1100', 2): ['Nordwand'],
}


def sektion(room, cut, objs):
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    rid = int(room[4:], 16)
    bg = geom.load_bg(PPM, rid, cut)
    res = anwenden.bau_objektweise(rdt, cam, cut, objs, bg, TMP, room)
    if not res:
        return None
    sec, n = res
    blob = geom.pack_container({cut: sec}, rdt[1])
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(TMP, '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    return ms, deck, tief, sec


def rail(room, cut, objs, deck, tief):
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    rid = int(room[4:], 16)
    R, t, H = geom.cut_view(rdt, cam, cut)
    floor = abnahme.floor_aus_dump(DUMP, rid)
    dep = []
    for o in objs:
        reg = o[1]
        d = geom.depth_map_objekt(rdt, cam, cut, reg, o[2], o[3], o[4], [], o[5], o[6],
                                  ('szene' if o[7] == 'szene' else None), o[9], o[10], None)
        if d is None:
            continue
        # depth_map_objekt gibt Tiefenstufen; die Schiene will Kamera-z -> aus der Stufe zurueck
        vz = np.where(reg & (d > 0), (1023.0 * 64.0) / np.maximum(d, 1), 0.0)
        dep.append((reg, np.where(reg, d.astype(float), 0.0), o[3] or 0))
    stand, stand_y0 = abnahme.standlinie(dep)
    return abnahme.standplatz_schiene(R, t, H, deck, tief, stand, floor, stand_y0=stand_y0), floor


for (room, cut), toten in TOT.items():
    e = raum.eintrag(AUS[room][str(cut)])
    alle = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    lebend = [o for o in alle if o[0] not in toten]
    print('=' * 96)
    print('%s C%d   %d Objekte -> ohne die %d beitragslosen: %d Objekte'
          % (room, cut, len(alle), len(toten), len(lebend)))
    aus = {}
    for tag, objs in (('MIT allen (Auslieferungsstand)', alle), ('OHNE die beitragslosen', lebend)):
        print('  --- %s ---' % tag)
        r = sektion(room, cut, objs)
        if r is None:
            print('     kein Ergebnis'); continue
        ms, deck, tief, sec = r
        soll = np.zeros((240, 320), bool)
        for o in objs:
            soll |= o[1]
        tiefen = sorted(set(m[6] for m in ms))
        print('     %d Rechtecke | Deckung %d (Soll %d, fehlt %d, zuviel %d) | %d Tiefenstufen %d..%d'
              % (len(ms), int(deck.sum()), int(soll.sum()), int((soll & ~deck).sum()),
                 int((deck & ~soll).sum()), len(tiefen), min(tiefen), max(tiefen)))
        aus[tag] = (ms, deck, tief, soll)
    if len(aus) == 2:
        a = list(aus.values())
        print('  VERGLEICH: Deckung identisch: %s | Tiefenkarte identisch: %s'
              % (np.array_equal(a[0][1], a[1][1]), np.array_equal(a[0][2], a[1][2])))
