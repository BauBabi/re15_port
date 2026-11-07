#!/usr/bin/env python3
"""Abgabe aus einem Ergebnis: gerippe.json, gerippe_bild1.png, gerippe_bild2.png, gerippe_rundum.png.

    python abgabe.py lauf/x.pt --wahl 0

gerippe.json traegt Winkel in 1/4096 Umdrehung (M = Rx Ry Rz), Versaetze ganzzahlig im Eltern-Raum, Aktorlage und Gier wie der Port.
Die Pose wird am Ende ueber form_emd.pose_aus_bild (Ganzzahl-Kette wie skeleton_common.c) nachgerechnet.
"""
import argparse, json, math, os, sys
import numpy as np, cv2, torch

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import fit as FT      # noqa: E402
import rig as RG      # noqa: E402
import ziele as ZI    # noqa: E402
import ansicht as AN  # noqa: E402
import form_emd as FE  # noqa: E402

F, GER, DT = FT.F, FT.GER, FT.DT
ACHSE_FARBE = (255, 255, 255)
DEUTSCH = {'tail': 'Schwanz', 'left_leg': 'Bein links', 'right_leg': 'Bein rechts', 'right_arm': 'Arm rechts', 'part_of_Body': 'Koerperstueck',
           'face_and_upper_body': 'Gesicht, Oberkoerper', 'left_arm_front': 'Arm links', 'right_arm_front': 'Arm rechts', 'frei': 'frei (kein Nutzer-Teil)'}


def proj(kam, p):
    return FT.FK.projiziere(kam, p)


def achsen_zeichnen(a, s, kam, st, off0, ende0, b=0):
    """Knochenachsen und Gelenke auf Tafel a (Skala s)."""
    T = st['T'][b].cpu().numpy()
    R = st['R'][b].cpu().numpy()
    start = off0['start']
    ende = off0['ende']
    for i in range(RG.N):
        p0 = T[i] + R[i] @ start[i]
        p1 = T[i] + R[i] @ ende[i]
        q0, q1 = proj(kam, p0), proj(kam, p1)
        if not (np.isfinite(q0).all() and np.isfinite(q1).all()):
            continue
        k = RG.klasse(RG.NAMEN[i])
        a0 = tuple(int(v * s + s // 2) for v in q0)
        a1 = tuple(int(v * s + s // 2) for v in q1)
        cv2.line(a, a0, a1, (0, 0, 0), 3, cv2.LINE_AA)
        cv2.line(a, a0, a1, ACHSE_FARBE, 1, cv2.LINE_AA)
        cv2.circle(a, a0, 3, (0, 0, 0), -1, cv2.LINE_AA)
        cv2.circle(a, a0, 2, (255, 255, 255), -1, cv2.LINE_AA)


def pruefblatt(nr, kn, lab, kam, st, achsen, m, rest, pfad):
    s = 2
    x0, y0, x1, y1 = {1: (150, 150, 470, 480), 2: (125, 125, 485, 480)}[nr]
    v = cv2.imread(os.path.join(F, 'bild%d.png' % nr)).astype(np.float32) / 255
    gross = cv2.resize(np.clip(v, 0, 1) ** 0.62, None, fx=s, fy=s, interpolation=cv2.INTER_CUBIC)
    a = (np.clip(gross, 0, 1) * 255).astype(np.uint8)
    b = a.copy()
    t = AN.knochen_teil(nr)
    kn_g = cv2.resize(kn.astype(np.int16), None, fx=s, fy=s, interpolation=cv2.INTER_NEAREST).astype(int)
    farb = np.array([AN.FARBE[t[i]] for i in range(RG.N)], np.uint8)
    an = kn_g >= 0
    ov = farb[np.clip(kn_g, 0, None)]
    b[an] = (0.40 * b[an] + 0.60 * ov[an]).astype(np.uint8)
    rand = np.zeros(kn_g.shape, bool)
    rand[:-1] |= kn_g[:-1] != kn_g[1:]
    rand[:, :-1] |= kn_g[:, :-1] != kn_g[:, 1:]
    b[rand] = (235, 235, 235)
    achsen_zeichnen(b, s, kam, st, achsen, None)
    for n in ZI.TEILE[nr]:
        c, _ = cv2.findContours(m[n].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        for k in c:
            k = (k.reshape(-1, 2) * s + s // 2).astype(np.int32)
            cv2.polylines(a, [k], True, AN.FARBE[n], 1, cv2.LINE_AA)
    c, _ = cv2.findContours((kn_g >= 0).astype(np.uint8), cv2.RETR_LIST, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(a, c, -1, (255, 255, 255), 1)
    ax = a[y0 * s:y1 * s, x0 * s:x1 * s]
    bx = b[y0 * s:y1 * s, x0 * s:x1 * s]
    ob = np.hstack([ax, bx])
    fuss = np.full((80, ob.shape[1], 3), 20, np.uint8)
    cv2.putText(fuss, 'Bild %d  links: Vorlage, Nutzer-Umrisse farbig, Modellkontur weiss  rechts: Kegelstuempfe je Knochen, Achsen, Teile farbig' % nr, (8, 16), 0, .42, (255, 255, 255), 1, cv2.LINE_AA)
    x = 8
    for q, n in enumerate(ZI.TEILE[nr]):
        r = rest[n]
        tx = '%s: IoU %.2f, Deckung %.2f, Kante %.1f px' % (DEUTSCH[n], r['iou'], r['decke_alle'], r['kante_med'])
        y = 36 + 18 * (q % 2)
        xx = 8 + 460 * (q // 2) + 12
        cv2.rectangle(fuss, (xx - 12, y - 9), (xx - 3, y), AN.FARBE[n], -1)
        cv2.putText(fuss, tx, (xx, y), 0, .42, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.rectangle(fuss, (8, 66), (17, 75), AN.FARBE['frei'], -1)
    cv2.putText(fuss, 'frei: Knochen ohne Nutzer-Teil, im Bild verdeckt oder ohne Beleg', (20, 75), 0, .42, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.imwrite(pfad, np.vstack([ob, fuss]))


def farbe_klasse(n):
    k = RG.klasse(n)
    if k.startswith('schwanz'):
        return (80, 255, 80)
    if 'arm' in k or k in ('hand', 'klaue'):
        return (255, 120, 40) if '_r' in n else (60, 200, 255)
    if 'schenkel' in k or k == 'fuss':
        return (0, 160, 255) if n.endswith('_l') else (255, 230, 0)
    if k in ('kopf', 'kiefer', 'hals'):
        return (0, 255, 255)
    return (160, 160, 255)


def ansicht_tafel(st, ger, wurzel, gier, ax, ay, titel, W, H, sk, cx, cy, boden=True):
    kn, z = AN.ortho(st, ger, 0, wurzel, gier, ax, ay, sk, W, H, cx, cy)
    bild = np.full((H, W, 3), 24, np.uint8)
    for i, n in enumerate(RG.NAMEN):
        bild[kn == i] = farbe_klasse(n)
    an = kn >= 0
    if an.any():
        za = z[an]
        sh = 1.0 - 0.6 * (z - za.min()) / max(1.0, za.max() - za.min())
        bild[an] = (bild[an] * np.clip(sh[an], 0.3, 1)[:, None]).astype(np.uint8)
    rand = np.zeros(kn.shape, bool)
    rand[:-1] |= kn[:-1] != kn[1:]
    rand[:, :-1] |= kn[:, :-1] != kn[:, 1:]
    bild[rand] = (230, 230, 230)
    if boden:
        cv2.line(bild, (0, cy), (W, cy), (120, 120, 120), 1)
        x = W - 34
        cv2.line(bild, (x, cy), (x, int(cy - 3005 * sk)), (255, 255, 255), 1)
        cv2.putText(bild, 'Leon 3005', (x - 72, int(cy - 3005 * sk) - 4), 0, .38, (255, 255, 255), 1, cv2.LINE_AA)
    else:
        cv2.line(bild, (W - 30, H - 14), (W - 30 - int(1000 * sk), H - 14), (255, 255, 255), 1)
        cv2.putText(bild, '1000', (W - 30 - int(1000 * sk), H - 20), 0, .38, (255, 255, 255), 1, cv2.LINE_AA)
    cv2.putText(bild, titel, (8, 18), 0, .5, (255, 255, 255), 1, cv2.LINE_AA)
    return bild


def rundum(par, ger, bilder, pfad):
    """Pose 1, Pose 2, Rast: je Seite von links, Hinten-Vorn, Draufsicht. Vorn der Figur rechts im Bild."""
    with torch.no_grad():
        lae, ver, a3, b3, off, start, ende, st = FT.zustand(par, ger, bilder)
        # Rast
        L = dict(zip(RG.L_KEYS, lae[0].tolist()))
        p0 = FT.Parameter(1)
        p0.load_state_dict(par.state_dict())
        p0.ang[0].data.zero_()
        p0.gier[0].data.zero_()
        p0.wurzel[0].data[:] = torch.tensor([0.0, -(L['oberschenkel'] + L['unterschenkel']) / 1000.0, 0.0], dtype=DT, device=GER)
        st0 = FT.zustand(p0, ger, bilder[:1])[-1][0]
        tief = float((st0['c'][..., 1] + st0['ry']).max())
        p0.wurzel[0].data[:, 1] -= tief / 1000.0
        st0 = FT.zustand(p0, ger, bilder[:1])[-1][0]
    W, H, sk = 620, 300, 0.05
    reihen = []
    daten = [('Pose 1', st[0], par.wurzel[0][0].detach().cpu().numpy() * 1000, float(par.gier[0][0])),
             ('Pose 2', st[1], par.wurzel[1][0].detach().cpu().numpy() * 1000, float(par.gier[1][0])),
             ('Rast', st0, p0.wurzel[0][0].detach().cpu().numpy() * 1000, 0.0)]
    for titel, s, w, g in daten:
        cx = W // 2
        cy = H - 30
        t1 = ansicht_tafel(s, ger, w, g, (1, 0, 0), (0, 1, 0), titel + ': Seite (von rechts)', W, H, sk, cx, cy)
        t1b = ansicht_tafel(s, ger, w, g, (0, 0, 1), (0, 1, 0), titel + ': von vorn', 420, H, sk, 210, cy)
        t2 = ansicht_tafel(s, ger, w, g, (1, 0, 0), (0, 0, -1), titel + ': Draufsicht (rechts der Figur unten)', W, H, sk, cx, H // 2, boden=False)
        reihen.append(np.hstack([t1, t1b, t2]))
    cv2.imwrite(pfad, np.vstack(reihen))


def main():
    a = argparse.ArgumentParser()
    a.add_argument('pt')
    a.add_argument('--wahl', type=int, default=0)
    a.add_argument('--aus', default=F)
    a.add_argument('--notiz', default='')
    a = a.parse_args()
    ger = RG.Gerippe(GER, DT)
    bilder = [FT.Bild(1, 4, ger), FT.Bild(2, 4, ger)]
    pfad = os.path.join(HIER, a.pt)
    par, ids, d = FT.lade(pfad, wahl=[a.wahl])
    _, m = ZI.lade()
    erg, karten, (lae, ver, a3, b3, st) = AN.tabelle(par, ger, bilder, 0, m)
    with torch.no_grad():
        off, start, ende = ger.achsen(lae, ver, b3)
    off_n, start_n, ende_n = off[0].cpu().numpy(), start[0].cpu().numpy(), ende[0].cpu().numpy()
    L, V, D = FT.als_dict(lae, ver, a3, b3, 0)
    knochen = []
    zu = {1: AN.knochen_teil(1), 2: AN.knochen_teil(2)}
    for i, n in enumerate(RG.NAMEN):
        k = RG.klasse(n)
        kn = dict(index=i, name=n, eltern=RG.ELTERN[i], seite={1: 'links', -1: 'rechts', 0: 'mitte'}[RG.SEITE[i]], klasse=k,
                  versatz=[int(round(x)) for x in off_n[i]],
                  achse_von=[int(round(x)) for x in start_n[i]], achse_bis=[int(round(x)) for x in ende_n[i]],
                  laenge=int(round(float(np.linalg.norm(ende_n[i] - start_n[i])))),
                  dicke=[dict(s=s, a=int(round(D[k][0][q])), b=int(round(D[k][1][q]))) for q, s in enumerate((0.0, 0.5, 1.0))],
                  querachse_a='Y' if RG.RICHTUNG[k][1] == 0 else 'X', querachse_b='Z',
                  teil_bild1=None if zu[1][i] == 'frei' else zu[1][i], teil_bild2=None if zu[2][i] == 'frei' else zu[2][i])
        knochen.append(kn)
    # Bilder 2 Oberarm gehoert zu zwei Teilen
    posen = {}
    for q, bi in enumerate(bilder):
        w = par.wurzel[q][0].detach().cpu().numpy() * 1000
        g = float(par.gier[q][0])
        G = int(round(g * 4096 / (2 * math.pi))) % 4096
        ang = (par.ang[q][0] * ger.frei).detach().cpu().numpy()
        wink = [[int(round(x * 4096 / (2 * math.pi))) for x in z] for z in ang]
        lage = [int(round(w[0])), 0, int(round(w[2]))]
        Yg = np.array([[math.cos(2 * math.pi * G / 4096), 0, math.sin(2 * math.pi * G / 4096)], [0, 1, 0],
                       [-math.sin(2 * math.pi * G / 4096), 0, math.cos(2 * math.pi * G / 4096)]])
        wurzel = [int(round(x)) for x in Yg.T @ (w - np.array(lage, float))]
        posen['bild%d' % bi.nr] = dict(lage=lage, gier=G, gier_grad=round(math.degrees(g) % 360, 1), wurzel=wurzel, winkel=wink)
    form = dict(knochen=[dict(eltern=k['eltern'], versatz=k['versatz']) for k in knochen])
    prob = {}
    for q, bi in enumerate(bilder):
        p = posen['bild%d' % bi.nr]
        Rq, Tq = FE.pose_aus_bild(form, dict(wurzel=p['wurzel'], winkel=p['winkel']))
        G = p['gier'] * 2 * math.pi / 4096
        Yg = np.array([[math.cos(G), 0, math.sin(G)], [0, 1, 0], [-math.sin(G), 0, math.cos(G)]])
        Twelt = np.array([Yg @ np.array(t, float) + np.array(p['lage'], float) for t in Tq])
        Tfit = st[q]['T'][0].detach().cpu().numpy()
        dw = np.linalg.norm(Twelt - Tfit, axis=1)
        pf = proj(bi.kam, Tfit)
        pw = proj(bi.kam, Twelt)
        prob['bild%d' % bi.nr] = dict(max_einheiten=round(float(dw.max()), 1), max_px=round(float(np.nanmax(np.linalg.norm(pf - pw, axis=1))), 2))
    aus = dict(stand='2026-11-01', quelle=os.path.basename(pfad), start=a.wahl, notiz=a.notiz,
               einheiten='Spieleinheiten, Y nach unten, Boden y=0, +X vorn, +Z links der Figur, Winkel 1/4096 Umdrehung, M = Rx Ry Rz wie form_emd',
               knochen=knochen, versatz_mass={k: round(v, 2) for k, v in V.items()}, laengen={k: round(v) for k, v in L.items()},
               posen=posen, ganzzahl_probe=prob, restfehler={'bild%d' % k: v for k, v in erg.items()},
               zuordnung={'bild1': {n: RG.TEIL_KNOCHEN[1][n] for n in ZI.TEILE[1]}, 'bild2': {n: RG.TEIL_KNOCHEN[2][n] for n in ZI.TEILE[2]}},
               winkelgrenzen_grad={k: v for k, v in RG.GRENZE.items()})
    pm = os.path.join(HIER, 'lauf', 'merkmale_ende.json')
    if os.path.exists(pm):
        aus['merkmale'] = json.load(open(pm, encoding='utf-8'))
    json.dump(aus, open(os.path.join(a.aus, 'gerippe.json'), 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    print('Ganzzahl-Probe', prob)
    ach = dict(start=start_n, ende=ende_n)
    for q, bi in enumerate(bilder):
        kn, lab, z = karten[bi.nr]
        pruefblatt(bi.nr, kn, lab, bi.kam, st[q], ach, m, erg[bi.nr], os.path.join(a.aus, 'gerippe_bild%d.png' % bi.nr))
    rundum(par, ger, bilder, os.path.join(a.aus, 'gerippe_rundum.png'))
    for nr in (1, 2):
        for n, r in erg[nr].items():
            print(nr, n, r)


if __name__ == '__main__':
    main()
