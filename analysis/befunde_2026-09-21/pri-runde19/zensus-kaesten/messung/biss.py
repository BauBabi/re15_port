# -*- coding: utf-8 -*-
"""Zensus A, Schritt 8: BISSPUNKTE - ohne Quote, je Kasten einzeln.

⛔ WARUM eine zweite Schiene: die Standplatz-Schiene (abnahme.standplatz_schiene) zaehlt
einen Standplatz erst als VORverd, wenn >= 95 % seines Koerperkastens verdeckt sind
(Heuristik, abnahme.py:34). Und das Fehlurteil-Mass aus Schritt 7 kann fuer Kasten-Objekte
GAR KEINE Ueber-Deckung finden: die Kacheltiefe ist das MAXIMUM der Punkttiefen
(anwenden.py:496), also immer >= der exakten Tiefe - ein Kasten kann dadurch nur zu FREI
urteilen, nie zu verdeckt. Beide Masse sind fuer die Frage "beisst der Kasten in die Figur"
also blind bzw. teilblind.

Hier wird ohne Quote gezaehlt: fuer jeden begehbaren Standplatz, der eindeutig VOR dem
Kasten liegt (Fuss-Kamera-z < vorderste Kastentiefe - 1 OT-Bucket), die Punkte des
Koerperkastens, die die Maske DIESES Kastens verdeckt. Jeder solche Punkt ist ein Stueck
Hintergrund auf der Figur. Soll = 0.

Ausgegeben wird die ABDECKUNG: wie viele Standplaetze eindeutig vor dem Kasten liegen und
seine Maske ueberhaupt sehen.
"""
import io, json, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
DUMP = 'build/floor_dump.txt'
ZIELE = [('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2), ('ROOM10D0', 7)]

for room, cut in ZIELE:
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    floor = abnahme.floor_aus_dump(DUMP, rid)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    print('=' * 104)
    print('%s Cut %d' % (room, cut))
    for o in objs:
        if o[9] is None:
            continue
        q = [int(x) for x in o[9]]
        vz, tr = geom.quader_tiefe(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4])
        reg = o[1] & tr
        if not reg.any():
            continue
        vorne = float(vz[reg].min())          # vorderste Flaeche des Kastens
        vor_n = 0
        biss = 0
        schlimm = []
        for band, pts in floor.items():
            yfoot = -band * 0x708
            for (wx, wz) in pts:
                pf = abnahme.proj(R, t, H, wx, yfoot, wz)
                pk = abnahme.proj(R, t, H, wx, yfoot - abnahme.KOPF, wz)
                if pf is None or pk is None:
                    continue
                fsx, fsy, fvz = pf
                ksx, ksy, kvz = pk
                if fvz >= vorne - geometrie.BUCKET:
                    continue                  # nicht eindeutig VOR dem Kasten
                hw = abnahme.HALB * H / fvz
                x0 = int(max(0, fsx - hw)); x1 = int(min(320, fsx + hw))
                y0 = int(max(0, min(ksy, fsy))); y1 = int(min(240, max(ksy, fsy)))
                if x1 <= x0 or y1 <= y0:
                    continue
                box = reg[y0:y1, x0:x1] & deck[y0:y1, x0:x1]
                if not box.any():
                    continue
                vor_n += 1
                ys = np.arange(y0, y1)
                vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
                vzs = np.where(ok, vzs, fvz)[:, None]
                n = int((box & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)).sum())
                biss += n
                if n:
                    schlimm.append((n, band, int(wx), int(wz)))
        schlimm.sort(reverse=True)
        print('  %-42s vorderste Tiefe vz %7.0f | ABDECKUNG %4d Standplaetze eindeutig davor mit Maskensicht | BISS %5d Punkte%s'
              % (o[0][:42], vorne, vor_n, biss,
                 '' if not schlimm else '  -> schlimmster (%d,%d): %d' % (schlimm[0][2], schlimm[0][3], schlimm[0][0])))
