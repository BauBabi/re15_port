"""zonen.json und Tafeln (je Zone: Nutzer-Umriss ueber Vorlage | Modellflaechen eingefaerbt | beide uebereinander)."""
import sys, os, json, hashlib
import numpy as np, cv2
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/V4F_skripte')
import v4f
from zonen_flaechen import TEILNAME, NFL, k
HIER = os.path.dirname(os.path.abspath(__file__))
ZIEL = sys.argv[1]
os.makedirs(ZIEL + '/tafeln', exist_ok=True)
PAL = {'rot': (237, 28, 36), 'gruen': (34, 177, 76), 'grau': (160, 160, 160), 'blau': (63, 72, 204), 'lila': (163, 73, 164), 'orange': (255, 127, 39),
       'dunkelrot': (180, 0, 30), 'hellblau': (0, 162, 232), 'braun': (185, 122, 87)}

MATERIAL = {
    'oberkoerper_hinten': ('Haut', 'Oberkörper (Rückseite); Farbe vom Nutzer nicht genannt'),
    'wunde_hinten': ('Fleisch', 'fleischige Wunde/Haut'),
    'musterung_rechts_hinten': ('Musterung', 'Oberkörper Musterung rechts; Farbe nicht genannt'),
    'musterung_links_hinten': ('Musterung', 'Oberkörper Musterung links; Farbe nicht genannt'),
    'kopf_hinten': ('Kopf', 'Kopf (von hinten)'),
    'auge_hinten': ('Auge', 'wahrscheinlich etwas Auge (Nutzer unsicher)'),
    'mund_hinten': ('Maul', 'Mund? (Nutzer unsicher)'),
    'schwanz': ('Haut', 'Schwanz der in den Rücken übergeht'),
    'schwanz_muster': ('Musterung', 'Schwanz Muster / Maserung'),
    'zipfel': ('Schwanzabschnitt', 'Zipfel - separater Schwanzabschnitt am Schluss'),
    'zipfel_musterung': ('Musterung', 'Schwanz Musterung (im Zipfel)'),
    'rotes_fleisch_zipfel': ('Fleisch', 'verschieden farbiges rotes Fleisch wie oben am Becken'),
    'bein_hinten_links': ('Haut', 'linkes Bein (hinten), Bein-Abschnitte, unten direkt verbunden mit dem Fuß'),
    'weisse_musterung_bein_hinten_links': ('Musterung', 'weiße Musterung'),
    'braeunliche_musterung_bein_hinten_links': ('Musterung', 'bräunliche Musterung'),
    'bein_hinten_rechts': ('Haut', 'rechtes Bein hinten, Beinabschnitte, unten direkt verbunden mit Fuß'),
    'weisse_musterung_bein_hinten_rechts': ('Musterung', 'weiße Musterung'),
    'braeunliche_musterung_bein_hinten_rechts': ('Musterung', 'bräunliche Musterung'),
    'fuss_hinten_links': ('Fuß', 'linker Fuß mit Teil der Fußkralle vorne'),
    'fusskrallen_hinten_links': ('Kralle', 'Teil der Fußkralle vorne; kaum sichtbar, wahrscheinlich 3 Krallen; rechts sicher genauso'),
    'vorderbein_rechts': ('Haut', 'rechtes Bein und Fuß (vorn), wahrscheinliche Bein-Abschnitte'),
    'weisse_musterung_vorderbein_rechts': ('Musterung', 'weiße Musterung'),
    'fuss_kralle_vorderbein_rechts': ('Kralle', 'Fuß und Kralle'),
    'oberkoerper_vorn': ('Haut', 'Oberkörper (Vorderseite)'),
    'fleisch_vorn': ('Fleisch', 'großer fleischfarbener Teil'),
    'gesicht_vorn': ('Kopf', 'Vorderseite Gesicht mit Maul nach unten rechts blickend'),
    'maul_vorn': ('Maul', 'geöffnetes Maul'),
    'unterkiefer_vorn': ('Unterkiefer', 'Teil des Unterkiefers'),
    'augen_vorn': ('Auge', 'Augen (zwei Hinweislinien)'),
    'arm_hand_krallen_vorn_rechts': ('Haut', 'vorderer rechter Arm mit Krallen'),
    'arm_vorn_rechts': ('Haut', 'vorderer rechter Arm'),
    'hand_vorn_rechts': ('Haut', 'vordere rechte Hand mit Krallen'),
    'krallen_vorn_rechts': ('Kralle', 'Krallen'),
    'vorderbein_links': ('Haut', 'Vorderes linkes Bein mit Krallen'),
    'beinabschnitt_vorn_links': ('Haut', 'Beinabschnitt'),
    'fussabschnitt_vorn_links': ('Haut', 'Fußabschnitt'),
    'krallen_vorn_links': ('Kralle', 'Exakt 4 Krallen'),
}
# Teil-Namen des Nutzers -> Teile-Bogen (nur Namenszuordnung, nicht gemessen)
NAMENSTEILE = {
    'schwanz': [1, 2, 3, 4, 5], 'zipfel': [6], 'bein_hinten_links': [8, 24, 25], 'bein_hinten_rechts': [9, 26, 27],
    'vorderbein_rechts': [16, 17, 18, 19, 20, 29, 31], 'arm_hand_krallen_vorn_rechts': [16, 17, 18, 19, 20, 29, 31],
    'vorderbein_links': [11, 12, 13, 14, 15, 28, 30], 'gesicht_vorn': [22, 23], 'kopf_hinten': [22, 23],
    'fuss_hinten_links': [25], 'krallen_vorn_links': [14, 15, 28, 30], 'krallen_vorn_rechts': [19, 20, 29, 31],
}


def alle_flaechen(b):
    m = k.mo.meshes[b]
    return [[b, 0, i] for i in range(len(m.tuv))] + [[b, 1, i] for i in range(len(m.quv))]


def md5(p):
    return hashlib.md5(open(p, 'rb').read()).hexdigest()


def main():
    roh = json.load(open(HIER + '/zonen_roh.json', encoding='utf-8'))
    ab = json.load(open(HIER + '/abbildung.json')); vs = json.load(open(HIER + '/versatz.json'))
    gl = json.load(open(HIER + '/gleichheit.json')); kr = json.load(open(HIER + '/krallen.json'))
    zv = dict(np.load(HIER + '/zonen_vorlage.npz'))
    KEN = {n: dict(np.load(HIER + '/kennung%d.npz' % n)) for n in (1, 2)}
    SZ = {n: cv2.imread(HIER + '/szene%d.png' % n) for n in (1, 2)}
    VO = {n: cv2.cvtColor(v4f.original(n), cv2.COLOR_RGB2BGR) for n in (1, 2)}
    zonen = []
    for e in roh:
        mat, besch = MATERIAL.get(e['zone'], MATERIAL.get(e.get('eltern') or '', ('Haut', '')))
        if e['bau'] == 'abschnitt':
            besch = e['nutzertext'] + ' in: ' + MATERIAL[e['eltern']][1]
        ganz = e['ganzteil_meshes']
        sicht_ids = {(f[0], f[1], f[2]) for f in e['flaechen']}
        rueck = [f for b in ganz for f in alle_flaechen(b) if tuple(f) not in sicht_ids]
        nt = NAMENSTEILE.get(e['zone'], [])
        z = dict(zone=e['zone'], bild=e['bild'], quelle=e['quelle'], nutzertext=e['nutzertext'], umrissfarbe=e['umrissfarbe'],
                 eltern=e.get('eltern'), material=mat, beschreibung=besch, bau=e['bau'],
                 vorlage=dict(px=e['px_vorlage'], bbox=e['bbox_vorlage'], polygone=e['polygone_vorlage'], lab_median_video_roh=e['vorlage_lab_median_roh']),
                 modell=dict(px_in_zone=e['px_modell_in_zone'], deckung=e['deckung_modell'],
                             meshes={str(a): d for a, d in e['meshes'].items()}),
                 flaechen_sichtbar=dict(spalten=['mesh', 'viereck', 'index', 'px_in_zone', 'anteil_der_sichtbaren_flaeche'], liste=e['flaechen']),
                 flaechen_kern=[f[:3] for f in e['flaechen'] if f[4] >= 0.5],
                 ganzteil=dict(regel='Mesh mit >= 50 % seiner sichtbaren Pixel in der Zone und >= 3 % der Modellpixel der Zone; Zone nennt ein ganzes Teil',
                               meshes=ganz, teile=[TEILNAME[b] for b in ganz], flaechen_gesamt=e['ganzteil_flaechen_gesamt'],
                               rueckseite_flaechen=rueck),
                 namensteile=dict(meshes=nt, teile=[TEILNAME[b] for b in nt],
                                  hinweis='Zuordnung des Nutzer-Teilnamens zum Teile-Bogen, nicht gemessen; nur fuer Teile, die das Bild nicht zeigt'))
        zonen.append(z)
    meta = dict(
        erstellt='2026-11-09', aufgabe='Zonen-Messer Runde 41 Riesenform: Nutzer-Zerlegung auf das Modell uebertragen (nur Messung)',
        modell=dict(emd='re15_port/shared_assets/RE2/BAHNSTEIG_FORM.EMD', emd_md5=md5(HIER + '/asset/BAHNSTEIG_FORM.EMD'),
                    tim='re15_port/shared_assets/RE2/BAHNSTEIG_FORM.TIM', tim_md5=md5(HIER + '/asset/BAHNSTEIG_FORM.TIM'), stand='K5e_f1',
                    meshes=32, flaechen_je_mesh={str(b): n for b, n in NFL.items()}),
        posen={'1': dict(vorlage='Version_80%/vlcsnap-2026-10-31-00h28m45s774.png (= form/bild1.png)', raum='5080 Schnitt 1', clip=0, bild=11,
                         lage=[-18564, 0, -18386], gier=80, kamera='form_pruefung.kamera(1)', licht='mess'),
               '2': dict(vorlage='Version_80%/vlcsnap-2026-10-31-00h29m02s830.png (= form/bild2.png)', raum='5090 Schnitt 11', clip=12, bild=0,
                         lage=[941, 0, -23776], gier=3874, kamera='form_pruefung.kamera(2)', licht='mess')},
        kennungs_render='v4f.szene_id (Nachbau Kandidat.rendern); Gegenprobe gegen Kandidat.rendern: 0 Pixel Unterschied in Bild 1 und 2',
        flaechen_kennung='mesh, viereck (0 Dreieck / 1 Viereck), index innerhalb der Dreiecks- bzw. Vierecksliste des Meshes',
        abbildung={n: dict(grundlage=ab[n]['grundlage'], formel='Vorlage = s * Grundlage + t (keine Spiegelung, keine Drehung)',
                           s=round(ab[n]['aehnlich']['massstab'], 5), t=[round(x, 2) for x in ab[n]['aehnlich']['versatz']],
                           faktor_grundlage_je_vorlagepx=round(ab[n]['aehnlich']['faktor_grundlage_je_vorlagepx'], 4),
                           affin_frei=dict(faktor_x=round(ab[n]['affin']['faktor_x'], 4), faktor_y=round(ab[n]['affin']['faktor_y'], 4),
                                           drehung_grad=round(ab[n]['affin']['drehung_grad'], 3), abweichung_zur_aehnlichkeit_px=[round(x, 2) for x in ab[n]['affin_minus_aehnlich_px']]),
                           ncc=round(ab[n]['aehnlich']['ncc'], 3), ncc_gespiegelt=round(ab[n]['ncc_gespiegelt'], 3),
                           rest_kacheln=ab[n]['rest_kacheln'], rest_median_px=round(ab[n]['rest_median'], 2), rest_p90_px=round(ab[n]['rest_p90'], 2),
                           rest_max_px=round(ab[n]['rest_max'], 2),
                           wasserzeichen_kontrolle=dict(faktor=round(ab[n]['wasserzeichen']['faktor'], 4), t=[round(x, 2) for x in ab[n]['wasserzeichen']['versatz']],
                                                        ncc=round(ab[n]['wasserzeichen']['ncc'], 3)))
                   for n in ('1', '2')},
        beschriftete_bilder={lf: dict(grundlage=vs[lf]['grund'], versatz_in_beschriftet=[vs[lf]['dx'], vs[lf]['dy']],
                                      abweichende_px=gl[lf]['abweichend'], abweichend_ausserhalb_annotation_roh=gl[lf]['davon_ausserhalb_annotation'],
                                      abweichend_ausserhalb_annotation_nach_alpha=gl[lf]['vorfaktoriert_abweichend_ausserhalb_annotation'])
                             for lf in vs},
        seiten='Seitenangaben aus Sicht des Wesens; Bild 1 von hinten (Wesen-links = Bild-links), Bild 2 von vorn (Wesen-rechts = Bild-links); '
               'Modell: Teile "l"/"r" liegen in beiden Posen auf der Wesen-Seite ihres Namens (Kennungs-Render)')
    json.dump(dict(meta=meta, zonen=zonen, krallen=kr), open(ZIEL + '/zonen.json', 'w', encoding='utf-8'), ensure_ascii=False, indent=1)
    # ---------- Tafeln ----------
    for z in zonen:
        n = z['bild']; mv = zv[z['zone']]; K = KEN[n]
        col = PAL[z['umrissfarbe']][::-1]
        a = VO[n].copy()
        cnt, _ = cv2.findContours(mv.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        # Modellflaechen der Zone ganz einfaerben (Anteil >= 0.5 gelb, sonst orange)
        fm = np.zeros(mv.shape, np.uint8)
        fl = K['flaeche']; me = K['mesh']
        for f in z['flaechen_sichtbar']['liste']:
            sel = (me == f[0]) & (fl == f[1] * 100000 + f[2])
            fm[sel] = 2 if f[4] >= 0.5 else 1
        b = SZ[n].copy()
        for v_, c_ in ((1, (0, 140, 255)), (2, (0, 230, 255))):
            s = fm == v_
            b[s] = (b[s] * 0.35 + np.array(c_) * 0.65).astype(np.uint8)
        c = VO[n].copy()
        s = fm > 0
        c[s] = (c[s] * 0.55 + np.array((0, 230, 255)) * 0.45).astype(np.uint8)
        for im in (a, c):
            cv2.drawContours(im, cnt, -1, col, 1)
        ys, xs = np.nonzero(mv | (fm > 0))
        x0, x1 = max(xs.min() - 20, 0), min(xs.max() + 21, 640); y0, y1 = max(ys.min() - 20, 0), min(ys.max() + 21, 480)
        fak = max(1, min(4, int(260 / max(y1 - y0, 1)), int(300 / max(x1 - x0, 1))))
        tiles = []
        for im, t in ((a, 'Nutzer-Umriss / Vorlage'), (b, 'Modellflaechen (gelb >=50 %)'), (c, 'beide')):
            q = cv2.resize(im[y0:y1, x0:x1], None, fx=fak, fy=fak, interpolation=cv2.INTER_NEAREST)
            kopf = np.full((18, q.shape[1], 3), 255, np.uint8); cv2.putText(kopf, t, (3, 13), 0, 0.4, (0, 0, 0), 1)
            tiles.append(np.vstack([kopf, q])); tiles.append(np.full((q.shape[0] + 18, 4, 3), 255, np.uint8))
        t = np.hstack(tiles[:-1])
        kopf = np.full((22, t.shape[1], 3), 255, np.uint8)
        txt = 'Bild %d  %s  "%s"  Modell-Deckung %.2f' % (n, z['zone'], z['nutzertext'].replace('ä', 'ae').replace('ö', 'oe').replace('ü', 'ue').replace('ß', 'ss'), z['modell']['deckung'])
        cv2.putText(kopf, txt[:150], (3, 15), 0, 0.42, (0, 0, 0), 1)
        cv2.imwrite(ZIEL + '/tafeln/%s.png' % z['zone'], np.vstack([kopf, t]))
    # Uebersichten je Bild
    for n in (1, 2):
        a = VO[n].copy(); b = SZ[n].copy()
        rng = np.random.RandomState(3); P = (rng.rand(40, 3) * 200 + 55).astype(np.uint8)
        me = KEN[n]['mesh']; b[me >= 0] = (b[me >= 0] * 0.3 + P[me[me >= 0]] * 0.7).astype(np.uint8)
        for bb in np.unique(me[me >= 0]):
            ys, xs = np.nonzero(me == bb); p = (int(np.median(xs)), int(np.median(ys)))
            cv2.putText(b, str(bb), p, 0, 0.35, (0, 0, 0), 2); cv2.putText(b, str(bb), p, 0, 0.35, (255, 255, 255), 1)
        for z in zonen:
            if z['bild'] != n or z['bau'] == 'abschnitt':
                continue
            cnt, _ = cv2.findContours(zv[z['zone']].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
            for im in (a, b):
                cv2.drawContours(im, cnt, -1, PAL[z['umrissfarbe']][::-1], 1)
        cv2.imwrite(ZIEL + '/tafeln/00_uebersicht_bild%d.png' % n,
                    cv2.resize(np.hstack([a, np.full((480, 4, 3), 255, np.uint8), b]), None, fx=1.5, fy=1.5, interpolation=cv2.INTER_NEAREST))
    print('Zonen', len(zonen))


if __name__ == '__main__':
    main()
