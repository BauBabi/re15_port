#!/usr/bin/env python3
"""K5e: die zwei GLOBALEN Zahlen bestimmen, die nicht direkt aus dem Messblatt uebernommen werden koennen (nach dem Bau mit der echten
Rundung und der echten G4-Zeile 2; die Messblatt-Vorschau rechnete mit Abschneiden r >> 3 und mit K5c-Zeile 2 = Schwanz-Gruppenzeile
als "G4-Seite 2").

    python kf_k5e_global.py <par.json>      -> schreibt haut.dL und fleisch.dL samt Quelle in par.json (Lauf-Protokoll auf stdout)

1. haut.dL (EIN Helligkeitsparameter der ganzen Haut, Messblatt RUHE 3.5 / W6): kleinste Hebung d, bei der Bild 2 mit Videofilter
   (3 Saaten, Pixelmenge V4F 3.1 = Nutzer-Teilmasken & K5c-Modell & ohne Weisskappe) die Vorlage 15,37 erreicht. Das ist die einzige
   Zahl, die aus dem Video kommt (ein Figur-Mittelwert, kein Texel-, Fleck- oder Regionswert).
2. fleisch.dL (EIN Satz fuer Schwanzspitze und Buckel): kleinster Versatz auf die Hautabbildung, mit dem beide Fleischstellen im Albedo
   mindestens ihren Abstand aus K4 erreichen ("vorher da", K4_heute gemessen mit ruhe_wunsch.stand_albedo auf denselben Flaechen-
   schluesseln): Spitze gegen Schwanz (Meshes 1-5) +6,89, Buckel gegen die Mesh-10-Haut +4,77. Kein Videowert.
"""
import json
import os
import shutil
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(HIER), 'ruhe', 'skripte'))
sys.path.insert(0, HIER)
import rl  # noqa: E402
import ruhe_wunsch as W  # noqa: E402
import kf_k5e as KE  # noqa: E402
import riegel_k5d as RK  # noqa: E402

TMP = os.path.join(rl.SCR, 'global_bau')


def bau(par):
    if os.path.isdir(TMP):
        shutil.rmtree(TMP)
    KE.bauen(par, TMP)
    rl.v4f._KC.pop(TMP, None)      # v4f.kand cacht je Ordner; ohne das misst jede Stufe den ersten Bau (Lauf 1: 10 x 15,912)
    return TMP


def k4_abstaende():
    mt = W.mat()
    A, _F = W.stand_albedo(os.path.join(KE.K5, 'K4_heute'))
    med = lambda keys: np.median(np.vstack([A[k] for k in keys if k in A])[:, 0])
    sp = med(mt['Fleisch_Schwanzspitze']) - med([k for k in mt['Haut'] if k[0] in (1, 2, 3, 4, 5)])
    bu = med(mt['Fleisch_Buckel']) - med([k for k in mt['Haut'] if k[0] == 10])
    return float(sp), float(bu)


def abstaende(ordner):
    mt = W.mat()
    A, _F = W.stand_albedo(ordner)
    med = lambda keys: np.median(np.vstack([A[k] for k in keys])[:, 0])
    return (float(med(mt['Fleisch_Schwanzspitze']) - med([k for k in mt['Haut'] if k[0] in (1, 2, 3, 4, 5)])),
            float(med(mt['Fleisch_Buckel']) - med([k for k in mt['Haut'] if k[0] == 10])))


def main():
    pp = sys.argv[1]
    par = json.load(open(pp, encoding='utf-8'))
    vor = RK.Vorlage()
    _s, Kc = rl.v4f.szene_id(KE.BASIS, 2)
    fig = RK.nutzer_union(2) & Kc['maske'] & ~vor.d[2]['clip']
    ziel = float(vor.d[2]['LV'][fig][:, 0].mean())
    print('Vorlage Bild 2 Video', round(ziel, 3), 'px', int(fig.sum()), flush=True)

    def hell(d):
        p = json.loads(json.dumps(par))
        p['haut']['dL'] = d
        o = bau(p)
        s, _K = rl.v4f.szene_id(o, 2)
        return float(np.mean([RK.labm(rl.v4f.video(s, 2, sa))[fig][:, 0].mean() for sa in rl.v4f.SAATEN]))
    k4 = k4_abstaende()
    print('K4 Abstaende Spitze/Buckel', [round(x, 2) for x in k4], flush=True)
    for runde in range(2):
        lo, hi = 18.0, 26.0
        for _ in range(10):
            d = round(0.5 * (lo + hi), 3)
            v = hell(d)
            print('  haut.dL %.3f -> Bild 2 Video %.3f' % (d, v), flush=True)
            if v >= ziel:
                hi = d
            else:
                lo = d
        par['haut']['dL'] = round(hi, 3)
        lo, hi = 0.0, 20.0
        for _ in range(10):
            g = round(0.5 * (lo + hi), 3)
            p = json.loads(json.dumps(par))
            p['fleisch']['dL'] = g
            a = abstaende(bau(p))
            print('  fleisch.dL %.3f -> Spitze %+.2f Buckel %+.2f' % (g, a[0], a[1]), flush=True)
            if a[0] >= k4[0] and a[1] >= k4[1]:
                hi = g
            else:
                lo = g
        par['fleisch']['dL'] = round(hi, 3)
        print('Runde', runde, 'haut.dL', par['haut']['dL'], 'fleisch.dL', par['fleisch']['dL'], flush=True)
    v = hell(par['haut']['dL'])
    a = abstaende(bau(par))
    par['haut']['dL_quelle'] = ('kf_k5e_global.py: kleinste Hebung (Bisektion 10 Schritte, 2 Runden) mit Bild 2 Video >= Vorlage %.2f (Messblatt RUHE 3.5/W6, '
                                'EIN globaler Helligkeitswert); Ergebnis %.2f. Messblatt-Vorschau +21,016 galt fuer Abschneiden r>>3 und K5c-Zeile 2 als G4-Seite 2. '
                                'Nutzer 11-08 "insgesamt ein wenig heller ... Helligkeit der Vergleichsbilder"' % (ziel, v))
    par['fleisch']['dL_quelle'] = ('kf_k5e_global.py: kleinster Versatz (Bisektion), mit dem Spitze und Buckel im Albedo mindestens ihren K4-Abstand erreichen '
                                   '(K4_heute: Spitze gegen Schwanz %+.2f, Buckel gegen Mesh-10-Haut %+.2f; "vorher da"); Ergebnis Spitze %+.2f, Buckel %+.2f. '
                                   'Messblatt-Wert 6,9 = nur K4-Spitze; die G4-Buckeltexel liegen 3,7 L* unter der Mesh-10-Haut'
                                   % (k4[0], k4[1], a[0], a[1]))
    json.dump(par, open(pp, 'w', encoding='utf-8'), indent=1, ensure_ascii=False)
    shutil.rmtree(TMP, ignore_errors=True)
    print('fertig', json.dumps({'haut.dL': par['haut']['dL'], 'fleisch.dL': par['fleisch']['dL'], 'bild2_video': round(v, 3), 'abstaende': a}))


if __name__ == '__main__':
    main()
