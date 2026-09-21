# -*- coding: utf-8 -*-
"""Zensus A, Schritt 4: WAS TRAEGT JEDER KASTEN WIRKLICH BEI?

Baut die Objektregionen mit demselben Code, der die ausgelieferten Sektionen erzeugt hat
(raum.objekt_regionen, also mit nur_kunst / tiefe=szene), und rechnet je Kasten:
  roh        = Punkte der Kasten-Projektion (Silhouette des QUADERS)
  wirksam    = Punkte, die der Kasten nach nur_kunst tatsaechlich beisteuert
  kunst      = davon Punkte, die im Lasso DESSELBEN Moebels liegen
  nur_dunkel = davon Punkte, die nur ueber die Tiefschwarz-Regel dazukommen
  allein     = Punkte, die KEIN anderes Objekt des Cuts deckt (Kasten allein verantwortlich)
Reine Messung, kein Schreiben.
"""
import json, os, struct, sys
import numpy as np
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum, maske_aus_png

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
ZIELE = [('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2), ('ROOM10D0', 7)]


def lassos(room, cut):
    """Rohe Nutzer-Lassos des Cuts (ohne Tiefenmodell) -> {name: region}"""
    out = {}
    for o in AUS[room][str(cut)].get('objekte') or []:
        if 'png' in o and all(k in o for k in ('x', 'y')):
            r = maske_aus_png.setze(o['png'], o['x'], o['y'], o.get('massstab', 1))
            if r is not None:
                out[o['png']] = r
    return out


for room, cut in ZIELE:
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    las = lassos(room, cut)
    kunst_alle = np.zeros((240, 320), bool)
    for r in las.values():
        kunst_alle |= r
    print('=' * 100)
    print('%s Cut %d  -  %d Objekte aus objekt_regionen, %d Nutzer-Lassos (%d Kunstpunkte)'
          % (room, cut, len(objs), len(las), int(kunst_alle.sum())))
    regs = [(o[0], o[1]) for o in objs]
    for i, o in enumerate(objs):
        name, r = o[0], o[1]
        q = o[9]
        andere = np.zeros((240, 320), bool)
        for j, (n2, r2) in enumerate(regs):
            if j != i:
                andere |= r2
        allein = int((r & ~andere).sum())
        if q is not None:
            qq = [int(x) for x in q]
            vz, tr = geom.quader_tiefe(R, t, H, qq[0], qq[0] + qq[2], qq[1], qq[1] + qq[3], qq[4])
            roh = int(tr.sum())
        else:
            roh = -1
        kun = int((r & kunst_alle).sum())
        print('  %-42s wirksam %6d | roh(Quader) %6d | davon Kunst %5d, nur Tiefschwarz %5d | ALLEIN %5d'
              % (name[:42], int(r.sum()), roh, kun, int(r.sum()) - kun, allein))
