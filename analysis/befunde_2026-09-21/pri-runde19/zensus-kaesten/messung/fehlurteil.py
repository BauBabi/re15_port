# -*- coding: utf-8 -*-
"""Zensus A, Schritt 7: WIE VIELE PUNKTE URTEILT DIE MASKE FALSCH?

Der Zeichner entscheidet je Punkt: verdecken, wenn Tiefenstufe < (1023*vz)>>16
(geometrie.verdeckt). Die Maske traegt aber nur EINE Stufe je Kachel, waehrend die
Geometrie je Punkt eine eigene Tiefe hat. Gemessen wird deshalb das FEHLURTEIL:
je begehbarem Standplatz die Punkte im Koerperkasten, bei denen die AUSGELIEFERTE
Kachel-Tiefe ein anderes Urteil faellt als die exakte Punkt-Tiefe derselben Objekte.

Das ist das Mass, das der Nutzer SIEHT: ein falsch verdeckter Punkt ist ein Stueck
Hintergrund auf der Figur, ein falsch freier Punkt ein Stueck Figur auf dem Moebel.

Abdeckung wird ausgegeben (wie viele Standplaetze das Mass ueberhaupt sieht).
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

TOT = {
    ('ROOM10F0', 4): ['Buerostuhl x-1600 z12200', 'Buerostuhl x-1800 z8200', 'Buerostuhl x-1900 z4300'],
    ('ROOM10F0', 5): ['Buerostuhl x1600 z5700', 'Buerostuhl x1600 z1900', 'Buerostuhl x1500 z-2000'],
    ('ROOM1100', 1): ['Suedblock', 'Suedwestwand'],
    ('ROOM1100', 2): ['Nordwand'],
    ('ROOM10D0', 7): [],
}


def exakt(room, cut, objs):
    """Punktgenaue Tiefenstufe der Objekt-Geometrie (naechste gewinnt)."""
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    dep = np.zeros((240, 320), np.int32)
    for o in objs:
        if o[9] is None:
            continue
        q = [int(x) for x in o[9]]
        vz, tr = geom.quader_tiefe(R, t, H, q[0], q[0] + q[2], q[1], q[1] + q[3], q[4])
        reg = o[1] & tr
        d = geometrie.rastern(np.where(reg, vz, 0.0)) * reg
        dep = np.where(reg & (d > 0) & ((dep == 0) | (d < dep)), d, dep)
    return dep


def urteil(R, t, H, deck, tief, exdep, floor):
    """-> (plaetze, gesehen, falsch_verdeckt, falsch_frei, worst)"""
    ges = 0
    fv = 0
    ff = 0
    worst = []
    plaetze = 0
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
            box = deck[y0:y1, x0:x1]
            if not box.any():
                continue
            ges += 1
            ys = np.arange(y0, y1)
            vzs, ok = geometrie.profil_spalte(R, t, H, wx, wz, ys)
            vzs = np.where(ok, vzs, fvz)[:, None]
            a = box & geometrie.verdeckt(tief[y0:y1, x0:x1], vzs)
            b = box & geometrie.verdeckt(exdep[y0:y1, x0:x1], vzs)
            n_fv = int((a & ~b).sum()); n_ff = int((b & ~a).sum())
            fv += n_fv; ff += n_ff
            if n_fv + n_ff:
                worst.append((n_fv + n_ff, band, int(wx), int(wz), n_fv, n_ff))
    worst.sort(reverse=True)
    return plaetze, ges, fv, ff, worst[:6]


for (room, cut), toten in TOT.items():
    rid = int(room[4:], 16)
    rdt, st = geom.load_rdt(CD, room)
    cam = struct.unpack_from('<I', rdt, 0x24)[0]
    R, t, H = geom.cut_view(rdt, cam, cut)
    bg = geom.load_bg(PPM, rid, cut)
    floor = abnahme.floor_aus_dump(DUMP, rid)
    e = raum.eintrag(AUS[room][str(cut)])
    alle = raum.objekt_regionen(room, cut, e, PPM, 'build/blatt')
    exdep = exakt(room, cut, alle)
    print('=' * 100)
    # 1) Auslieferungsstand
    blob = open(os.path.join(CD, 'MASKS', room + '.MSK'), 'rb').read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(open(os.path.join(CD, 'MASKS', '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck, tief = abnahme.deckung_und_tiefe(ms, idx)
    nur = deck & (exdep > 0)
    p, g, fv, ff, w = urteil(R, t, H, nur, tief, exdep, floor)
    print('%s C%d  AUSGELIEFERT (%d Rechtecke): %d Standplaetze projiziert, %d sehen die Maske (ABDECKUNG %.1f %%)'
          % (room, cut, len(ms), p, g, 100.0 * g / max(1, p)))
    print('   Fehlurteil ueber alle Standplaetze: %d Punkte falsch VERDECKT, %d Punkte falsch FREI'
          % (fv, ff))
    for x in w:
        print('     schlimmster Standplatz Band %d (%d,%d): %d falsch verdeckt, %d falsch frei' % (x[1], x[2], x[3], x[4], x[5]))
    # 2) Neubau ohne die beitragslosen Objekte
    if not toten:
        continue
    lebend = [o for o in alle if o[0] not in toten]
    res = anwenden.bau_objektweise(rdt, cam, cut, lebend, bg, TMP, room)
    if not res:
        print('   Neubau: kein Ergebnis'); continue
    sec, n = res
    b2 = geom.pack_container({cut: sec}, rdt[1])
    ms2 = maskenbild.masken(b2, cut) or []
    idx2 = maskenbild.lies_tim_bytes(open(os.path.join(TMP, '%s_PRI%02d.TIM' % (room, cut)), 'rb').read())[0]
    deck2, tief2 = abnahme.deckung_und_tiefe(ms2, idx2)
    nur2 = deck2 & (exdep > 0)
    p2, g2, fv2, ff2, w2 = urteil(R, t, H, nur2, tief2, exdep, floor)
    print('   OHNE die %d beitragslosen (%d Rechtecke): %d Standplaetze sehen die Maske'
          % (len(toten), len(ms2), g2))
    print('   Fehlurteil: %d falsch VERDECKT (vorher %d), %d falsch FREI (vorher %d)'
          % (fv2, fv, ff2, ff))
