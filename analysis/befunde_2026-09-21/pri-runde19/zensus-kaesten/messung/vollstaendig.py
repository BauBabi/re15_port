# -*- coding: utf-8 -*-
"""Zensus A, Schritt 5: ist die AUSGELIEFERTE Deckung die ganze gewollte Flaeche?
(Loecher = "fehlende Stuecke") und tragen die inneren Kaesten ueberhaupt eine Tiefe bei?
Reine Messung."""
import io, json, os, struct, sys
import numpy as np
sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme, raum

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))
ZIELE = [('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2), ('ROOM10D0', 7)]

for room, cut in ZIELE:
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    e = raum.eintrag(AUS[room][str(cut)])
    objs = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    soll = np.zeros((240, 320), bool)
    for o in objs:
        soll |= o[1]
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    print('=' * 92)
    print('%s C%d: Soll (Vereinigung aller Objekte) %6d | ausgeliefert %6d | FEHLT %5d | ZUVIEL %5d | %d Rechtecke'
          % (room, cut, int(soll.sum()), int(deck.sum()), int((soll & ~deck).sum()),
             int((deck & ~soll).sum()), len(ms)))
    # Tiefen-Beitrag der inneren Kaesten: wo liefert welcher Kasten die NAECHSTE Tiefe?
    qs = []
    for o in objs:
        if o[9] is None:
            continue
        q = [int(x) for x in o[9]]
        vz, tr = geom.quader_tiefe(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4])
        qs.append((o[0], o[1], np.where(o[1], vz, np.inf)))
    if len(qs) > 1:
        stack = np.stack([v for (_, _, v) in qs])
        gewinner = np.argmin(stack, 0)
        endlich = np.isfinite(stack).any(0)
        for i, (n, r, v) in enumerate(qs):
            g = int(((gewinner == i) & endlich & r).sum())
            print('   %-42s liefert die naechste Tiefe an %6d Punkten (von %6d eigenen)'
                  % (n[:42], g, int(r.sum())))
