"""E5: Umkehr auf das Wesen. Video -> Spielfarbe e (inverses Modell je Szene, Streulicht abgezogen) -> geteilt durch das Port-Licht
(Disc, Kennungs-Render K5e_f1 an der Szenenlage: Schattierung je Pixel, Mesh/Flaeche je Pixel) -> Albedo je Pixel in Textur-Einheiten
(0..255, wie TIM << 3). Material je Pixel aus ruhe/messung/material.json (3828 Flaechen, gleiche Flaechenfolge in K5c/K5e_f1/K5f).
Dieselbe Kette (vorwaerts, dann zurueck) laeuft fuer K5c, K5e_f1 und K5f: so ist der Vergleich gleich unscharf wie das Video.
Ausschluss: Uebersteuerung (V >= 215, +3 px), Feuer/Ueberstrahlung (V >= 200, +15 px, nur Bild 2), Streulichtanteil > 50 %,
Chroma-Saum (Figur um 6 x 3 px erodiert), Schattierung < 20. Ausgang daten/e5_wesen.json, Scratch e5_*.npz."""
import json
import numpy as np
import cv2
import ek
import em
import v4f

P_ = ek.P
FV = ek.FV
MAT = json.load(open(ek.K5 + '/ruhe/messung/material.json'))['material']
LISTEN = {n: set(v['liste']) for n, v in MAT.items()}
STAENDE = (('K5c', ek.FORM + '/stufe3/basis_k5c'), ('K5e_f1', ek.K5 + '/ruhe/K5e_f1'), ('K5f', ek.K5 + '/k5f/K5f'))
REF = ek.K5 + '/ruhe/K5e_f1'
HALS6 = {'22|1|1', '22|1|8', '22|1|9', '22|1|194', '22|1|201', '22|1|209'}   # MESSBLATT_K5F 0.2
NAMEN = ['Haut', 'Gelenk', 'Schwanzspitze', 'Buckel', 'Krallen', 'Kopf', 'Lippe/Unterkiefer', 'Hals', 'F1', 'Sonder']


def material_von(key):
    s = lambda n: key in LISTEN[n]
    if s('Kralle'):
        return 'Krallen'
    if s('Gelenk'):
        return 'Gelenk'
    if s('Fleisch_Schwanzspitze'):
        return 'Schwanzspitze'
    if s('Fleisch_Buckel'):
        return 'Buckel'
    if s('F1'):
        return 'F1'
    if s('Zahn') or s('Auge') or s('Maulinneres'):
        return 'Sonder'
    mesh = int(key.split('|')[0])
    if key in HALS6 or mesh == 21:
        return 'Hals'
    if s('Lippensaum') or mesh == 23:
        return 'Lippe/Unterkiefer'
    if mesh == 22:
        return 'Kopf'
    return 'Haut'


CACHE = {}


def mat_index(mesh, flaeche):
    out = np.full(mesh.shape, -1, np.int16)
    sel = mesh >= 0
    for me, fl in set(zip(mesh[sel].tolist(), flaeche[sel].tolist())):
        key = '%d|%d|%d' % (me, fl // 100000, fl % 100000)
        if key not in CACHE:
            CACHE[key] = NAMEN.index(material_von(key))
        out[(mesh == me) & (flaeche == fl)] = CACHE[key]
    return out


def render(ordner, n, dx=0, dy=0):
    k = v4f.kand(ordner)
    lage, gier, clip, bild = k.stand(n)
    kam = P_.kamera(n)
    kf = k.kf(clip, bild)
    L = P_.licht_von_disc(n)
    tr = v4f.dreiecke_id(k.mo, kf, lage, gier, kam, L)
    sh = np.zeros((480, 640, 3), np.uint8)
    K = v4f.rastern_id(tr, ek.weiss_textur(k.tex), sh, kam.abbildung)
    s = np.zeros((480, 640, 3), np.uint8)
    v4f.rastern_id(tr, k.tex, s, kam.abbildung)
    alb = np.zeros((480, 640, 3), np.uint8)
    v4f.rastern_id(v4f.dreiecke_id(k.mo, kf, lage, gier, kam, None), k.tex, alb, kam.abbildung)
    sch = lambda a: np.roll(a, (dy, dx), (0, 1))
    return dict(sh=sch(sh), s=sch(s), alb=sch(alb), m=sch(K['maske']), mat=sch(mat_index(K['mesh'], K['flaeche'])))


def ausrichten(n):
    fig = cv2.imread(ek.FORM + '/pruefung/figur_bild%d.png' % n, 0) > 0
    r = render(REF, n)
    best = None
    for dy in range(-10, 11):
        for dx in range(-10, 11):
            m = np.roll(r['m'], (dy, dx), (0, 1))
            iou = (m & fig).sum() / float((m | fig).sum())
            if best is None or iou > best[0]:
                best = (iou, dx, dy)
    iou0 = (r['m'] & fig).sum() / float((r['m'] | fig).sum())
    return best, iou0, fig


def statistik(alb, mat, gueltig):
    out = {}
    for i, nm in enumerate(NAMEN):
        q = gueltig & (mat == i)
        if q.sum() < 15:
            out[nm] = dict(pixel=int(q.sum()))
            continue
        a = alb[q]
        Lb = ek.lab(np.clip(a, 0, 255))
        med = np.median(a, 0)
        out[nm] = dict(pixel=int(q.sum()), median=[round(float(x), 1) for x in med],
                       p10=[round(float(x), 1) for x in np.percentile(a, 10, 0)], p90=[round(float(x), 1) for x in np.percentile(a, 90, 0)],
                       lab_median=[round(float(x), 2) for x in ek.lab(np.clip(med, 0, 255))],
                       L_p10_p90=[round(float(np.percentile(Lb[:, 0], 10)), 1), round(float(np.percentile(Lb[:, 0], 90)), 1)])
    return out


class Modell:
    """Vorwaerts (Spielbild e -> Video) und Umkehr (Video -> e) einer Szene."""

    def __init__(self, name, n, th=None, ex=None):
        self.name, self.n, self.th, self.ex = name, n, th, ex

    def vor(self, e):
        if self.th is not None:
            return em.vorwaerts(e, self.th)
        return FV.video_filter(np.clip(e + .5, 0, 255).astype(np.uint8), bild=self.n, entzerren=False, rauschen=False,
                               ex=self.ex).astype(np.float32)

    def zurueck(self, V):
        if self.th is not None:
            return em.umkehr(V, self.th)
        alt = FV.EX[self.n].copy()
        if self.ex is not None:
            FV.EX[self.n] = np.asarray(self.ex, float)
        try:
            return FV.video_umkehr(np.clip(V + .5, 0, 255).astype(np.uint8), self.n).astype(np.float32)
        finally:
            FV.EX[self.n] = alt


def modelle(n):
    if n == 2:
        th = np.array(json.load(open(ek.DATEN + '/e4b_modell_bild2.json'))['alle']['frei'])
        return [Modell('form_video Bestand', 2), Modell('em_frei (BG + Frau)', 2, th=th)]
    j = json.load(open(ek.DATEN + '/e4_modell_bild1.json'))
    return [Modell('form_video EX an Leon', 1, ex=j['ex']), Modell('form_video Bestand (EX farbe.md)', 1)]


def kette(mod, fuell, r):
    """Spielbild mit Figur r['s'] -> Video (Modell) -> zurueck -> / Schattierung."""
    comp = fuell.copy()
    comp[r['m']] = r['s'][r['m']]
    Vs = mod.vor(comp)
    eS = mod.zurueck(Vs)
    return eS * 255.0 / np.maximum(r['sh'].astype(np.float32), 1), Vs


def main():
    aus = {}
    for n in (1, 2):
        V = ek.video(n)
        (iou, dx, dy), iou0, fig = ausrichten(n)
        ref = render(REF, n, dx, dy)
        m = ref['m'] & fig
        kern = cv2.erode(m.astype(np.uint8), cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (13, 7))) > 0
        clip = cv2.dilate((V.max(2) >= 215).astype(np.uint8), np.ones((7, 7), np.uint8)) > 0
        feuer = (cv2.dilate((V.max(2) >= 200).astype(np.uint8), np.ones((31, 31), np.uint8)) > 0) if n == 2 else np.zeros_like(clip)
        dunkel_sh = ref['sh'].max(2) < 20
        eintrag = dict(ausrichtung=dict(iou=round(float(iou), 4), iou_ohne=round(float(iou0), 4), versatz=[dx, dy]), modelle={})
        for mi, mod in enumerate(modelle(n)):
            eR = mod.zurueck(V)
            Lr = ((np.maximum(eR, 0) / 255.0) ** 2.2).astype(np.float32)
            glow = FV._glow(Lr)
            glowanteil = (glow / np.maximum(cv2.GaussianBlur(Lr, (0, 0), FV.SIG_OPTIK) + glow, 1e-6)).max(2)
            streu = glowanteil > 0.5
            gueltig = kern & ~clip & ~feuer & ~streu & ~dunkel_sh
            ausschluss = dict(figur_render_und_video=int(m.sum()), nach_chroma_saum=int(kern.sum()), uebersteuert=int((kern & clip).sum()),
                              feuer_ueberstrahlung=int((kern & feuer & ~clip).sum()),
                              streulicht_ueber_50=int((kern & streu & ~clip & ~feuer).sum()),
                              schattierung_unter_20=int((kern & dunkel_sh).sum()), gueltig=int(gueltig.sum()))
            alb = eR * 255.0 / np.maximum(ref['sh'].astype(np.float32), 1)
            if n == 2:
                fuell = np.load(ek.SCR + '/e3_b2.npz')['E'].astype(np.float32)
                fuell[m] = eR[m]
            else:
                fuell = eR.astype(np.float32)
            staende = {}
            for nm, o in STAENDE:
                r = render(o, n, dx, dy)
                albS, Vs = kette(mod, fuell, r)
                rg = gueltig & r['m']
                staende[nm] = dict(kette=statistik(albS, ref['mat'], rg),
                                   textur_sichtbar=statistik(r['alb'].astype(np.float32), ref['mat'], rg),
                                   video_sim_dE76_median=round(float(np.median(ek.de76(Vs[rg], V[rg]))), 2))
                if mi == 0:
                    np.savez_compressed(ek.SCR + '/e5_b%d_%s.npz' % (n, nm), Vs=Vs.astype(np.float32), albS=albS.astype(np.float32),
                                        alb=r['alb'], s=r['s'])
            ohne_streu = kern & ~clip & ~feuer & ~dunkel_sh
            Lt = Lr + glow
            ober = 255.0 * np.power(np.maximum(Lt, 0), 1 / 2.2) * 255.0 / np.maximum(ref['sh'].astype(np.float32), 1)
            eintrag['modelle'][mod.name] = dict(ausschluss=ausschluss, video=statistik(alb, ref['mat'], gueltig), staende=staende,
                                                ohne_streu_ausschluss=statistik(alb, ref['mat'], ohne_streu),
                                                obergrenze_ohne_streu_abzug=statistik(ober, ref['mat'], ohne_streu))
            if mi == 0:
                eintrag['schattierung_median_je_material'] = {nm: [int(x) for x in np.median(ref['sh'][gueltig & (ref['mat'] == i)], 0)]
                                                             for i, nm in enumerate(NAMEN) if (gueltig & (ref['mat'] == i)).sum() >= 15}
                np.savez_compressed(ek.SCR + '/e5_b%d.npz' % n, alb=alb.astype(np.float32), eR=eR.astype(np.float32), gueltig=gueltig,
                                    mat=ref['mat'], sh=ref['sh'], m=m, clip=clip, feuer=feuer, streu=streu, kern=kern)
            print(n, mod.name, json.dumps(eintrag['ausrichtung']), json.dumps(ausschluss), flush=True)
            for nm in NAMEN:
                v = eintrag['modelle'][mod.name]['video'].get(nm, {})
                print('  ', nm, v.get('pixel'), v.get('median'), v.get('lab_median'), flush=True)
        aus['bild%d' % n] = eintrag
    json.dump(aus, open(ek.DATEN + '/e5_wesen.json', 'w'), indent=1, ensure_ascii=False)


if __name__ == '__main__':
    main()
