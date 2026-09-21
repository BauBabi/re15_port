# -*- coding: utf-8 -*-
"""Zensus A, Schritt 13: DUENNE STRUKTUREN, nicht kleine Komponenten.

Das Bild funkeln_ROOM10F0_C4.png zeigt: was auf der Figur erscheint, sind nicht nur
Einzelpunkte, sondern duenne DIAGONALE LINIEN - die Kanten- und Schattenlinien des Pults,
die die Tiefschwalz-Regel (Summe < 45) der Stuhl-Zelle zugeschlagen hat. Eine lange
1 Punkt breite Linie ist eine GROSSE Komponente, das Mass aus Schritt 12 sieht sie also
nicht. Kriterium hier: Strukturbreite, gemessen ueber Erosion.

  duenn = Punkte, die eine 3x3-Erosion nicht ueberleben (Struktur <= 2 Punkte breit)

⛔ Gegenprobe gegen das Nullmodell: die von Hand freigestellten Nutzer-Lassos desselben
Cuts. Sind die AEHNLICH duenn, taugt das Mass nicht. (Memory: Wandbreiten sind teils
3 Punkte - duenn ist nicht per se falsch, deshalb der Vergleich.)
"""
import io, json, os, struct, sys
import numpy as np
from scipy import ndimage as nd
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum, maske_aus_png

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
DUMP = 'build/floor_dump.txt'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
K3 = np.ones((3, 3), bool)


def duenn(reg):
    return reg & ~nd.binary_erosion(reg, K3)


for room, cut in (('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2), ('ROOM10D0', 7)):
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    dn = duenn(deck)
    print('=' * 100)
    print('%s C%d  Maske %6d Punkte, davon DUENN (Struktur <= 2 px) %5d (%.1f %%)'
          % (room, cut, int(deck.sum()), int(dn.sum()), 100.0 * dn.sum() / max(1, deck.sum())))
    # Nullmodell: rohe Nutzer-Lassos
    roh = np.zeros((240, 320), bool)
    for o in AUS[room][str(cut)].get('objekte') or []:
        if 'png' in o and all(k in o for k in ('x', 'y')):
            r = maske_aus_png.setze(o['png'], o['x'], o['y'], o.get('massstab', 1))
            if r is not None:
                roh |= r
    if roh.any():
        print('  NULLMODELL rohe Nutzer-Lassos: %6d Punkte, davon duenn %5d (%.1f %%)'
              % (int(roh.sum()), int(duenn(roh).sum()), 100.0 * duenn(roh).sum() / int(roh.sum())))
    if room != 'ROOM10F0':
        continue
    floor = abnahme.floor_aus_dump(DUMP, rid)
    plaetze = 0; ber = 0; verd = 0; verd_all = 0; schlimm = []
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
            if not deck[y0:y1, x0:x1].any():
                continue
            ber += 1
            rows = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, rows)
            vzs = np.where(ok, vzs, fvz)[:, None]
            v = geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)
            a = int((deck[y0:y1, x0:x1] & v).sum())
            n = int((dn[y0:y1, x0:x1] & v).sum())
            verd_all += a; verd += n
            if n:
                schlimm.append((n, a, band, int(wx), int(wz)))
    schlimm.sort(reverse=True)
    print('  ABDECKUNG: %d Standplaetze projiziert, %d sehen die Maske (%.1f %%)'
          % (plaetze, ber, 100.0 * ber / max(1, plaetze)))
    print('  verdeckende Punkte gesamt %d, davon aus DUENNEN Strukturen %d (%.1f %%) an %d Standplaetzen'
          % (verd_all, verd, 100.0 * verd / max(1, verd_all), len(schlimm)))
    for x in schlimm[:6]:
        print('     Band %d (%d,%d): %d von %d verdeckenden Punkten sind duenn' % (x[2], x[3], x[4], x[0], x[1]))
    np.save('build/r19/duenn_%s_C%d.npy' % (room, cut), dn)
