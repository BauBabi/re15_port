#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 6a (P6): Varianten der linken Hand (Mesh 12) und des linken Unterarms (Mesh 11) gegen das Breitenprofil
der Vorlage (analysis/befunde_runde41/SF_MESSBLATT_HAND.md) - Wahl nach ZAHL, nicht nach Blick.

Jede Variante ist ein Parametersatz fuer hand_r6.anwenden (Original-Klauenhand EM30 Typ 0x30 in neuer Groesse und Drehung um die
Handachse, Original-Unterarm mit Ringverlauf). Gemessen wird mit hand_mass.modell - dieselbe Messregel wie an den Nutzerbildern.

WAHLREGEL (in dieser Reihenfolge):
  1. Ausschluss: zwei Krallen kreuzen sich in P1, P2 oder PV (die Vorlage belegt kein Kreuzen, Messblatt Abschnitt 3).
  2. kleinste ZAHL der Masse ausserhalb der gemessenen Streuung der Vorlage (min - 1 px .. max + 1 px, 1 px = Rasterkante):
     8 Stationen + 10 Stellen Unterarm-Profil + 10 Stellen Hand-Profil + Handlaenge + Handbreite/Handlaenge (hier ohne
     Zuschlag, es ist ein Verhaeltnis) + 4 Krallenlaengen, alle in P1 (nur dort zeigt die Vorlage den linken Arm; in P2 und PV
     liegt er hinter dem Rumpf, dort gibt es KEIN Vorlagemass).
  3. kleinste Summe der Betraege der Abweichungen vom Vorlage-Median ueber dieselben 28 Profilmasse (Videopixel).
Die Krallenlaengen der Vorlage stammen aus dem V7-Messblatt (SF_sichel.md, "V7 Hand": rote Fingerkralle 15.0 .. 23.1 px,
Median 19.0; graue Daumenkralle 6.7 .. 22.6 px, Median 12.7) - im Messblatt Hand sind einzelne Krallen nicht aufloesbar.

PORT-WAHL: die Form steht auf keiner Disc, es gibt keine @0x-Adresse; jede Zahl der Varianten kommt aus dem Messblatt
(Verhaeltnis Vorlage zu Modell) oder ist ein Raster der Suche (Drehung 90 / 105 / 118 Grad: 118 = gemessener Azimut der Kamera
im Handraum in P1, 90 = Handflaeche quer, 105 = Mitte).

RUNDE 6c (P7): gemessen wird nicht mehr in den Ersatz-Posen P1 / P2 / PV, sondern in den echten Szenenposen (hand_szene.py):
Profil, Groesse und Krallenlaengen als MITTEL ueber die fuenf Paar-Posen h125, h126, h128, h130, h132 (Zahl "ausser" und Summe je Pose,
dann gemittelt), Kreuzen in allen acht Paar- und Kontroll-Posen. Die Wahl der Runde 6a ("D 0.93 0.72 / 118") stammte aus P1 und ist
abgeloest (hand_v8.json "quelle.wahl"); die Varianten-Liste bleibt als Protokoll der Runde 6a stehen.

Aufruf:  python hand_suche.py [--emd <EMD>] [--json <Protokoll>] [--bild <Tafel>] [--pruefe hand_v8.json]
"""
import os, sys, json, argparse
import numpy as np, cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import hand_r6 as H6                  # noqa: E402
import hand_mass as HM                # noqa: E402

FINGER_VORLAGE = (15.0, 19.0, 23.1)   # min, Median, max der roten Fingerkralle (V7-Messblatt, 7 Bilder)
DAUMEN_VORLAGE = (6.7, 12.7, 22.6)    # graue Daumenkralle (V7-Messblatt, 3 Bilder)

# Unterarm: 'grund' = Stand V8 r5d (Original x 1.229 / 0.80 / 1.229, unveraendert seit V4). 'verlauf' = Stuetzstellen
# [y, y neu, Radialfaktor]: Messblatt Abschnitt 2 - das Modell ist bei 30 % der Unterarmlaenge 42.5 px breit, die Vorlage 36.6
# (Faktor 0.86, Ring y 236), bei 50 % 36.5 gegen 33.5 (Faktor 0.92, Ring y 442); Ellbogen- und Handgelenkring bleiben.
U_ALT = dict(grund=[1.229, 0.80, 1.229], verlauf=[[0, 0, 1.0], [700, 700, 1.0]])
U_NEU = dict(grund=[1.229, 0.80, 1.229], verlauf=[[0, 0, 1.0], [90, 90, 1.0], [236, 236, 0.86], [442, 442, 0.92], [627, 627, 1.0], [700, 700, 1.0]])
# Krallen: A x 1.33 und B x 2.0 heben die zwei kurzen Original-Fingerkrallen (P1: 14 px und 8 px) in die Streuung der Vorlage
# (15.0 .. 23.1 px); B wird dabei zur Haelfte in die Richtung der Nachbarkralle C gedreht - mit Anteil 0 kreuzt B die Kralle C
# in P2 (und bei 118 Grad auch in PV), mit 0.5 nirgends (Raster 0 / 0.5 / 1.0). C und Daumenkralle T bleiben Original.
KRALLEN = dict(A=dict(faktor=1.33), B=dict(faktor=2.0, nach='C', anteil=0.5))
# Handansatz (Ring der Ecken 1 .. 4, Original y 30): auf y -30 zurueck in das Unterarmende, damit die Unterarmspitze (15 Einheiten
# hinter dem Gelenk) in jeder Handstellung im Handkoerper steckt (Riegel S17 b).
RING = {str(i): [-30, 1.0] for i in (1, 2, 3, 4)}


def _hand(skala, grad, voll=True):
    h = dict(skala=skala, dreh_grad=grad, schub_y=0)
    if voll:
        h.update(krallen=KRALLEN, wurzelring=RING)
    return h


VARIANTEN = [
    # Groesse: 0.80 = Original x 0.80 wie die Laengsachse des Unterarms (V4); 0.93 quer = 0.80 x Vorlage 44.5 px / Modell 39.8 px an
    # der breiten Stelle bei 118 Grad (Vorlauf); 0.72 laengs = 0.80 x Vorlage-Handlaenge 59.5 px / Modell 66.2 px (Variante C).
    ('A 0.80 / 90', dict(hand=_hand([0.8, 0.8, 0.8], 90, False), unterarm=U_ALT)),      # nur Groesse + Drehung, Rest wie bisher
    ('B 0.80 / 105', dict(hand=_hand([0.8, 0.8, 0.8], 105), unterarm=U_NEU)),
    ('C 0.93 / 118', dict(hand=_hand([0.93, 0.8, 0.93], 118), unterarm=U_NEU)),
    ('D 0.93 0.72 / 118', dict(hand=_hand([0.93, 0.72, 0.93], 118), unterarm=U_NEU)),
]


def bau(blob, cfg):
    d, _do, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    H6.anwenden(ms, t, cfg, {})
    return KB.packen(blob, d, ms, KB.tim_packen(t))


def _reihen(v):
    return ([v['stationen'][s] for s in HM.STATIONEN], list(v['profil_unterarm']), list(v['profil_hand']))


def vorlage_streuung(vor=None):
    """-> (Median, min, max) je Reihe (Stationen, Unterarm-Profil, Hand-Profil) ueber die fuenf Nutzerbilder."""
    vor = vor or {k: HM.vorlage(k) for k in HM.VORLAGE_BILDER}
    R = [np.array([_reihen(v)[i] for v in vor.values()], float) for i in range(3)]
    hl = np.array([v['hand_laenge'] for v in vor.values()], float)
    bl = np.array([v['stationen']['hand_breit'] / v['hand_laenge'] for v in vor.values()], float)
    return [(np.median(r, axis=0), r.min(0), r.max(0)) for r in R] + [(float(np.median(hl)), float(hl.min()), float(hl.max())),
                                                                       (float(np.median(bl)), float(bl.min()), float(bl.max()))], vor


def messen(mo, streu):
    """Masse einer Variante in den echten Szenenposen: Profil gegen die Vorlage je Paar-Pose (Mittel), Krallen und Kreuzen in allen acht Posen."""
    out = dict(posen={})
    for pn in HM.POSEN:
        v = HM.modell(mo, HM.POSEN[pn])
        out['posen'][pn] = dict(HM.roh(v), kreuzen=v['kreuzen'])
        out['_' + pn] = v
    n_p = float(len(HM.PAARE))
    summe, ausser, k_aus, g_aus, teile = 0.0, 0.0, 0.0, 0.0, {}
    for pn in HM.PAARE:
        v = out['_' + pn]
        for nm, r, (med, lo, hi) in zip(('stationen', 'unterarm_profil', 'hand_profil'), _reihen(v), streu[:3]):
            a = np.array(r, float)
            s, n = float(np.abs(a - med).sum()), int(((a < lo - 1) | (a > hi + 1)).sum())
            d = teile.setdefault(nm, dict(werte={}, summe=0.0, ausser=0.0))
            d['werte'][pn] = [round(float(x), 1) for x in a]
            d['summe'] += s / n_p; d['ausser'] += n / n_p
            summe, ausser = summe + s / n_p, ausser + n / n_p
        for n, k in v['krallen'].items():
            sp = DAUMEN_VORLAGE if n == 'T' else FINGER_VORLAGE
            k_aus += (not (sp[0] - 1 <= k['laenge'] <= sp[2] + 1)) / n_p
        hl, bl = v['hand_laenge'], v['stationen']['hand_breit'] / v['hand_laenge']
        g_aus += (int(not (streu[3][1] - 1 <= hl <= streu[3][2] + 1)) + int(not (streu[4][1] <= bl <= streu[4][2]))) / n_p
    for d in teile.values():
        d['summe'], d['ausser'] = round(d['summe'], 1), round(d['ausser'], 1)
    P = [out['_' + pn] for pn in HM.PAARE]
    kl = {n: round(float(np.mean([v['krallen'][n]['laenge'] for v in P])), 1) for n in sorted(P[0]['krallen'])}
    kreuz = {pn: out['posen'][pn]['kreuzen'] for pn in HM.POSEN}
    hl = float(np.mean([v['hand_laenge'] for v in P])); hb = float(np.mean([v['stationen']['hand_breit'] for v in P]))
    out.update(groesse_ausser=round(g_aus, 1), vorlage_handlaenge=[round(x, 1) for x in streu[3]], vorlage_breite_durch_laenge=[round(x, 3) for x in streu[4]])
    out.update(teile=teile, summe=round(summe, 1), ausser=round(ausser + k_aus + g_aus, 1), krallen_ausser=round(k_aus, 1), krallen_laenge_p1=kl, kreuzen=kreuz,
               kreuzt=any(kreuz.values()), hand_laenge=round(hl, 1), hand_breit=round(hb, 1), breite_durch_laenge=round(hb / hl, 3),
               spitze_ueber_boden_H=round(float(np.mean([v['spitze_ueber_boden'] / v['figur_hoehe'] for v in P])), 3))
    return out


def wahl(erg):
    return min(erg, key=lambda n: (erg[n]['kreuzt'], erg[n]['ausser'], erg[n]['summe']))


def tafel(erg, vor, gew, pfad, kante=110, f=3):
    """Zeile 1: die fuenf Nutzerbilder (Messkachel). Darunter je Variante h125 / h130 / h100 (echte Szenenposen), jeweils Szene und frei gezeichnet."""
    g = kante * f
    oben = [cv2.resize(HM.kachel(v['_bild'], v, 0.5, kante, float(f), 'Vorlage %s' % v['bild']), (g, g)) for v in vor.values()]
    oben.append(np.full((g, g, 3), 40, np.uint8))
    zeilen = [np.hstack(oben)]
    for nm, e in erg.items():
        ks = []
        for pn in ('h125', 'h130', 'h100'):
            v = e['_' + pn]
            for img in (v['_voll'], v['_bild']):
                c = (np.array(v['W']) + np.array(v['T'])) / 2
                x0 = max(min(int(c[0] - kante / 2), img.shape[1] - kante), 0)
                y0 = max(min(int(c[1] - kante / 2), img.shape[0] - kante), 0)
                k = cv2.resize(np.ascontiguousarray(img[y0:y0 + kante, x0:x0 + kante, :3]), None, fx=f, fy=f, interpolation=cv2.INTER_NEAREST)
                k = cv2.cvtColor(k, cv2.COLOR_RGB2BGR)
                cv2.putText(k, '%s %s%s' % (nm, pn, '  GEWAEHLT' if nm == gew else ''), (4, 14), cv2.FONT_HERSHEY_SIMPLEX, 0.42, (0, 255, 255), 1, cv2.LINE_AA)
                ks.append(k)
        cv2.putText(ks[1], 'ausser %.1f  Summe %.1f  kreuzt %s' % (e['ausser'], e['summe'], 'JA' if e['kreuzt'] else 'nein'), (4, g - 22),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.42, (255, 255, 255), 1, cv2.LINE_AA)
        cv2.putText(ks[1], 'Hand %.1f lang, %.1f breit, B/L %.2f' % (e['hand_laenge'], e['hand_breit'], e['breite_durch_laenge']), (4, g - 8),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.42, (255, 255, 255), 1, cv2.LINE_AA)
        zeilen.append(np.hstack(ks))
    cv2.imencode('.png', np.vstack(zeilen))[1].tofile(pfad)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', default=V.ASSET)
    ap.add_argument('--json')
    ap.add_argument('--bild')
    ap.add_argument('--pruefe', help='hand_v8.json: muss der gewaehlten Variante gleichen')
    a = ap.parse_args()
    blob = open(a.emd, 'rb').read()
    streu, vor = vorlage_streuung()
    print('Vorlage (Median | min .. max), Videopixel:')
    for nm, (med, lo, hi) in zip(('Stationen', 'Unterarm-Profil', 'Hand-Profil'), streu[:3]):
        print('  %-16s %s' % (nm, ' '.join('%.1f|%.1f..%.1f' % t for t in zip(med, lo, hi))))
    print('  Handlaenge       %.1f|%.1f..%.1f    Handbreite/Handlaenge %.3f|%.3f..%.3f' % (streu[3] + streu[4]))
    erg = {}
    for nm, cfg in VARIANTEN:
        e = messen(V.modell(bau(blob, cfg)), streu)
        e['cfg'] = cfg
        erg[nm] = e
        print('%-18s ausser %.1f (Krallen %.1f, Groesse %.1f)  Summe %5.1f  kreuzt %s  | Hand %.1f lang %.1f breit B/L %.3f | Spitze %s H ueber Boden' % (
            nm, e['ausser'], e['krallen_ausser'], e['groesse_ausser'], e['summe'], e['kreuzen'], e['hand_laenge'], e['hand_breit'], e['breite_durch_laenge'], e['spitze_ueber_boden_H']))
        for t, d in e['teile'].items():
            print('     %-16s Summe %5.1f ausser %.1f  %s' % (t, d['summe'], d['ausser'], d['werte']))
        print('     Krallenlaenge Mittel der Paar-Posen %s' % e['krallen_laenge_p1'])
    gew = wahl(erg)
    print('GEWAEHLT: %s' % gew)
    if a.pruefe:
        ist = json.load(open(a.pruefe, encoding='utf-8'))
        gleich = all(ist[k] == erg[gew]['cfg'][k] for k in ('hand', 'unterarm'))
        print('hand_v8.json gleicht der gewaehlten Variante: %s' % ('ja' if gleich else 'NEIN'))
        if not gleich:
            sys.exit(1)
    if a.bild:
        tafel(erg, vor, gew, a.bild)
    if a.json:
        aus = {n: {k: v for k, v in e.items() if not k.startswith('_')} for n, e in erg.items()}
        json.dump(dict(gewaehlt=gew, varianten=aus, vorlage={n: dict(median=m.round(1).tolist(), min=lo.tolist(), max=hi.tolist())
                                                           for n, (m, lo, hi) in zip(('stationen', 'unterarm_profil', 'hand_profil'), streu[:3])}),
                  open(a.json, 'w', encoding='utf-8', newline='\n'), indent=1, sort_keys=True)


if __name__ == '__main__':
    main()
