# -*- coding: utf-8 -*-
"""Zensus A, Schritt 1: was liegt AUSGELIEFERT in MASKS, und wie verhaelt sich das
zum ROHEN Kasten? Keine Schreibvorgaenge - reine Messung."""
import json, os, struct, sys
import numpy as np
sys.path.insert(0, 're15_port/tools/maske')
import geom, geometrie, maskenbild, abnahme

CD = 're15_port/shared_assets/PSX'
PPM = 'build/bg_ppm'
AUS = json.load(open('analysis/esp_masken_2026-09-03/auswahl.json'))

ZIELE = [('ROOM10F0', 4), ('ROOM10F0', 5), ('ROOM1100', 1), ('ROOM1100', 2), ('ROOM10D0', 7)]


def shipped(room, cut):
    p = os.path.join(CD, 'MASKS', '%s.MSK' % room)
    blob = open(p, 'rb').read() if os.path.exists(p) else None
    ms = maskenbild.masken(blob, cut) or []
    tp = os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut))
    if not os.path.exists(tp):
        return ms, None, None
    idx = maskenbild.lies_tim_bytes(open(tp, 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    return ms, deck, tief


for room, cut in ZIELE:
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    ms, deck, tief = shipped(room, cut)
    e = AUS[room][str(cut)]
    objs = e.get('objekte', [])
    kaesten = [o for o in objs if 'quader' in o]
    pngs = [o for o in objs if 'png' in o]
    print('=' * 78)
    print('%s Cut %d   Original-Kuenstlermasken: %s' % (room, cut, geom.original_has_masks(rdt, cam, cut)))
    print('  PBM: %s   STAND: %s' % (
        os.path.exists(os.path.join(CD, 'MASKS', '%s_PRI%02d.PBM' % (room, cut))),
        os.path.exists(os.path.join(CD, 'MASKS', '%s_PRI%02d.STAND' % (room, cut)))))
    print('  ausgeliefert: %d Rechtecke, Deckung %d Punkte, Tiefen %s' % (
        len(ms), int(deck.sum()) if deck is not None else -1,
        sorted(set(m[6] for m in ms))))
    print('  Objekte: %d quader, %d png' % (len(kaesten), len(pngs)))
    baender = sorted(geom.begehbare_baender(rid))
    print('  begehbare Baender: %s' % baender)
    for o in kaesten:
        q = [int(x) for x in o['quader']]
        y0 = 0 if len(baender) != 1 else -baender[0] * geom.BAND_HOEHE
        if o.get('ebene') is not None:
            y0 = int(o['ebene'])
        vz, tr = geometrie.quader_auf_band(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4], y0)
        bb = None
        if tr.any():
            xs = np.nonzero(tr.any(0))[0]; ys = np.nonzero(tr.any(1))[0]
            bb = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
        print('   "%-34s" roh %5d Punkte  bbox %s  vz %.0f..%.0f  y0=%d' % (
            o['name'], int(tr.sum()), bb,
            vz[tr].min() if tr.any() else -1, vz[tr].max() if tr.any() else -1, y0))
