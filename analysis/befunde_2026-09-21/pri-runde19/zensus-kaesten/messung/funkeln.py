# -*- coding: utf-8 -*-
"""Zensus A, Schritt 12: FUNKELN die Sprenkel auf der Figur?

Die Sprenkel (Komponenten < 10 Punkte) sind Pult-, Papier- und Schattenpunkte, die die
Tiefschwarz-Regel der Stuhl-Zelle zuschlaegt. Jeder, der die Figur verdeckt, ist ein
Hintergrundpunkt auf ihr. Gemessen ohne Quote, je Standplatz, mit ABDECKUNG.

⛔ Die Kachel-MAX-Statistik (anwenden.py:496) schuetzt hier NICHT: ein einzelner Sprenkel
in einer eigenen Kachel ist das Maximum seiner Kachel und traegt damit seine eigene, nahe
Tiefe.
"""
import io, json, os, struct, sys
import numpy as np
from scipy import ndimage as nd
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
DUMP = 'build/floor_dump.txt'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))


def sprenkel(reg, grenze=10):
    lab, n = nd.label(reg)
    if n == 0:
        return np.zeros_like(reg)
    gr = np.bincount(lab.ravel())
    klein = np.nonzero(gr < grenze)[0]
    klein = klein[klein > 0]
    return np.isin(lab, klein)


for room, cut in (('ROOM10F0', 4), ('ROOM10F0', 5)):
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    ges = np.zeros((240, 320), bool)
    for o in objs:
        ges |= o[1]
    sp = sprenkel(ges) & deck
    floor = abnahme.floor_aus_dump(DUMP, rid)
    plaetze = 0; beruehrt = 0; verd = 0; schlimm = []
    for band, pts in floor.items():
        yfoot = -band * 0x708
        for (wx, wz) in pts:
            pf = abnahme.proj(R, t, H, wx, yfoot, wz)
            pk = abnahme.proj(R, t, H, wx, yfoot - abnahme.KOPF, wz)
            if pf is None or pk is None:
                continue
            plaetze += 1
            fsx, fsy, fvz = pf
            ksx, ksy, kvz = pk
            hw = abnahme.HALB * H / fvz
            x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
            y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
            if x1 <= x0 or y1 <= y0:
                continue
            box = sp[y0:y1, x0:x1]
            if not box.any():
                continue
            beruehrt += 1
            rows = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, rows)
            vzs = np.where(ok, vzs, fvz)[:, None]
            n = int((box & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)).sum())
            verd += n
            if n:
                schlimm.append((n, band, int(wx), int(wz)))
    schlimm.sort(reverse=True)
    print('=' * 96)
    print('%s C%d  Sprenkel (Komponenten < 10 Punkte) in der Maske: %d Punkte von %d (%.1f %%)'
          % (room, cut, int(sp.sum()), int(deck.sum()), 100.0 * sp.sum() / max(1, deck.sum())))
    print('  ABDECKUNG: %d Standplaetze projiziert, %d haben Sprenkel im Koerperkasten (%.1f %%)'
          % (plaetze, beruehrt, 100.0 * beruehrt / max(1, plaetze)))
    print('  davon VERDECKEN: %d Punkte an %d Standplaetzen' % (verd, len(schlimm)))
    for x in schlimm[:8]:
        print('     Band %d (%d,%d): %d Sprenkelpunkte auf der Figur' % (x[1], x[2], x[3], x[0]))
    np.save('build/r19/sprenkel_%s_C%d.npy' % (room, cut), sp)
