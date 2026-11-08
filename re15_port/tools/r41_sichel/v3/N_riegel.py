# SF Messblatt V3 Abschnitt 10: Riegel N1-N20 (Nutzer-Befund 2026-11-06 20:35-20:50) am Port-Render der SPIELKAMERA
# 50A0/12 vorn h118 und h125 (dort sieht der Nutzer den Kopf; Leiste 05 Zeile 1/2). Beide Bilder muessen bestehen,
# ausser wo "Summe" steht. Messfunktionen: N_mess.py (gleich fuer KI und Port). Aufruf:
#   python N_render.py EMD NAME        (einmal je EMD; legt sfv3/N/NAME_h118/h125.npz an)
#   python N_riegel.py NAME [NAME ...] (KI wird immer mitgemessen; Ausgabe Tabelle + sfv3/N/N_riegel.json)
import os, sys, json
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import N_mess as M

R = [
    # id, Text, Funktion(m) -> Wert, Test(Wert) -> bool, Soll-Text
    ('N1', 'Augenkern hell (Fenster +-0,012 H: Px L*>55)', lambda m: m['auge']['fenster_L55'], lambda v: v >= 12, '>= 12'),
    ('N2', 'Augenkern Lmax (L*)', lambda m: m['auge']['kern_Lmax'], lambda v: v >= 65, '>= 65'),
    ('N3', 'Augenkern Groesse (Px > Kuppe in der Augenhuelle)', lambda m: m['auge']['kern_px'], lambda v: 4 <= v <= 16, '4-16'),
    ('N4', 'Auge dunkel eingebettet (L* Auge/Kuppe)', lambda m: m['auge']['L_auge_haut'], lambda v: 0.35 <= v <= 0.55, '0,35-0,55'),
    ('N5', 'helle Kuppe um das Auge (L* Ring 0,012-0,02 H)', lambda m: m['auge']['Lh'], lambda v: 38 <= v <= 50, '38-50'),
    ('N6', 'Augenlage im Kopf (Anteil von (B)-links)', lambda m: m['kopf']['lage_kopf'], lambda v: 0.24 <= v <= 0.36, '0,24-0,36'),
    ('N7', 'Auge zu Kopfmitte (H, - = (B)-links = (W)-rechts)', lambda m: m['kopf']['auge_zu_kopfmitte_H'], lambda v: -0.032 <= v <= -0.016, '-0,032..-0,016'),
    ('N8', 'Maulmitte im Kopf (Anteil von (B)-links)', lambda m: m['maul_lage_kopf'], lambda v: 0.28 <= v <= 0.45, '0,28-0,45'),
    ('N9', 'Haarzone dunkel (L* Zone / Kuppe)', lambda m: m['haare']['L_zone_kuppe'], lambda v: 0.30 <= v <= 0.50, '0,30-0,50'),
    ('N10', 'Straehnen (Anzahl >= 0,015 H lang)', lambda m: m['haare']['straehnen'], lambda v: 3 <= v <= 6, '3-6'),
    ('N11', 'Straehnen-Kontrast (L* dazwischen - L* Straehne)', lambda m: (m['haare']['lab_zwischen'][0] - m['haare']['lab_straehne'][0]) if m['haare']['lab_straehne'] else 0, lambda v: v >= 6, '>= 6'),
    ('N12', 'Haarzone Breite (H)', lambda m: m['haare']['zone_H'][0], lambda v: v >= 0.05, '>= 0,05'),
    ('N13', 'Auge frei (Straehnen-Px auf der Augenmaske)', lambda m: m['haare']['ueber_auge'], lambda v: v == 0, '0'),
    ('N14', 'kein Oval: Umriss/Ellipse-IoU', lambda m: m['maul']['umriss_iou'] or 0, lambda v: v <= 0.60, '<= 0,60'),
    ('N15', 'Spalt (Median schwarze Px je Spalte, H)', lambda m: m['maul']['spalt_median_H'], lambda v: v <= 0.018, '<= 0,018'),
    ('N16', 'schwarze Flaeche (1e-4 H^2)', lambda m: m['maul']['schwarz_flaeche_e4H2'], lambda v: v <= 16, '<= 16'),
    ('N17', 'zerklueftet (Anteil groesste schwarze Komponente)', lambda m: m['maul']['anteil_groesste'], lambda v: v <= 0.80, '<= 0,80'),
    ('N18', 'Oberlippenlinie unregelmaessig (Rest-Std, H)', lambda m: (m['maul']['ecken'] or {}).get('oberlinie_rest_std_H', 0), lambda v: v >= 0.005, '>= 0,005'),
    ('N19', 'Zahnreihe oben unregelmaessig (Rest-Std, H)', lambda m: m['maul'].get('oben_rest_std_H', 0), lambda v: v >= 0.005, '>= 0,005'),
    ('N20', 'Mundwinkel (B)-rechts tiefer (H)', lambda m: (m['maul']['ecken'] or {}).get('dy_rechts_minus_links_H', 0), lambda v: v >= 0.004, '>= +0,004'),
    ('N21', 'Lippen dunkelrot (Anteil in den Maulspalten)', lambda m: m['maul']['lippe_anteil'], lambda v: v >= 0.38, '>= 0,38'),
    ('N22', 'Lippen L*/Kuppe', lambda m: m['maul']['L_lippe_kuppe'] or 0, lambda v: 0.32 <= v <= 0.48, '0,32-0,48'),
    ('N23', 'Kinn rot (Anteil a* >= 12)', lambda m: m['maul']['kinn_anteil_a12'], lambda v: v >= 0.70, '>= 0,70'),
    ('N24', 'Kinn L*', lambda m: m['maul']['lab_kinn'][0], lambda v: 14 <= v <= 22, '14-22'),
]
SUMME = ('N25', 'Lippenglanz max(RGB)>170 ohne Zaehne, Summe h118+h125', lambda ms: sum(m['maul']['glanz_px'] for m in ms), lambda v: v >= 4, '>= 4')


def messe(wer, h):
    d, au, ko, ha, ma = M.alles(wer, h)
    sp = ma['spalten_x_H']; H = d['H']
    mx = au['mitte'][0] + (sp[0] + sp[1]) / 2 * H
    m = M.sauber(dict(H=H, auge=au, kopf=ko, haare=ha, maul=ma))
    m['maul_lage_kopf'] = round((mx - ko['links']) / (ko['rechts'] - ko['links']), 3)
    m['maul_zu_kopfmitte_H'] = round((mx - (ko['links'] + ko['rechts']) / 2) / H, 4)
    return m


if __name__ == '__main__':
    namen = ['KI'] + [n for n in sys.argv[1:] if n != 'KI']
    alle = {}
    for n in namen:
        alle[n] = {h: messe(n, h) for h in (118, 125)}
    zeilen = []
    kopf = '%-4s %-55s %-14s' % ('', 'Mass', 'Soll') + ''.join('%-22s' % ('%s h118 / h125' % n) for n in namen)
    zeilen.append(kopf)
    rot = {n: 0 for n in namen}
    for rid, txt, f, t, soll in R:
        s = '%-4s %-55s %-14s' % (rid, txt, soll)
        for n in namen:
            v = [f(alle[n][h]) for h in (118, 125)]
            ok = all(t(x) for x in v)
            rot[n] += 0 if ok else 1
            s += '%-22s' % (('ok ' if ok else 'ROT ') + ' / '.join(('%.4g' % x) for x in v))
        zeilen.append(s)
    rid, txt, f, t, soll = SUMME
    s = '%-4s %-55s %-14s' % (rid, txt, soll)
    for n in namen:
        v = f([alle[n][h] for h in (118, 125)]); ok = t(v); rot[n] += 0 if ok else 1
        s += '%-22s' % (('ok ' if ok else 'ROT ') + str(v))
    zeilen.append(s)
    zeilen.append('ROT je Variante: ' + ', '.join('%s %d von %d' % (n, rot[n], len(R) + 1) for n in namen))
    print('\n'.join(zeilen))
    out = os.path.join(M.N, 'N_riegel.json')
    json.dump(dict(werte=alle, rot=rot, tabelle=zeilen), open(out, 'w'), indent=1)
    open(os.path.join(M.N, 'N_riegel.txt'), 'w', encoding='utf-8').write('\n'.join(zeilen) + '\n')
