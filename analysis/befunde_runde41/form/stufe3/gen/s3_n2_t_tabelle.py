"""Nachbesserung 2, Textur-Spur: Bildmass-JSON (s3_n2_textur.py bild --json) je Satz und Teil ueber alle Ansichten zusammenfassen.

    python s3_n2_t_tabelle.py BILD.json [--md]
"""
import json, sys

TEILE = ('rumpf', 'schwanz', 'beine', 'arme', 'hals', 'gelenk', 'kopf', 'kiefer', 'klauen', '_gelenk_grenze', '_haut_kante', '_gesamt')


def zusammen(d):
    out = {}
    for n, v in d.items():
        agg = {}
        for _an, m in v.items():
            for t, x in m.items():
                a = agg.setdefault(t, dict(kn=0, k=0.0, px=0, i=0.0, b=0.0, bn=0, tp=0.0))
                kp = x.get('kante_px', 0)
                px = x.get('px', 0) or 0
                a['kn'] += kp
                a['k'] += x.get('kante', 0) * kp
                a['px'] += px
                a['i'] += x.get('innen', 0) * px
                if x.get('block') is not None:
                    a['b'] += x['block'] * px
                    a['bn'] += px
                if x.get('texel_px') is not None:
                    a['tp'] += x['texel_px'] * px
        out[n] = {t: dict(kante=round(a['k'] / max(a['kn'], 1), 1) if a['kn'] else None, innen=round(a['i'] / max(a['px'], 1), 1),
                          block=round(a['b'] / max(a['bn'], 1), 1) if a['bn'] else None, texel_px=round(a['tp'] / max(a['px'], 1), 2),
                          px=a['px']) for t, a in agg.items()}
    return out


def main():
    d = zusammen(json.load(open(sys.argv[1], encoding='utf-8')))
    namen = list(d)
    if '--md' in sys.argv:
        print('| Teil | ' + ' | '.join('%s Kante / Innen / Block / Texel px / Anteil' % n for n in namen) + ' |')
        print('|---|' + '---|' * len(namen))
    for t in TEILE:
        zeile = []
        for n in namen:
            x = d[n].get(t)
            ges = d[n]['_gesamt']['px'] or 1
            if not x:
                zeile.append('-')
                continue
            k = '-' if t == '_gesamt' else x['kante']
            zeile.append('%s / %s / %s / %s / %d %%' % (k, x['innen'], x['block'], x['texel_px'], round(100.0 * x['px'] / ges)))
        if '--md' in sys.argv:
            print('| %s | %s |' % (t, ' | '.join(zeile)))
        else:
            print('%-8s %s' % (t, ' || '.join(zeile)))


if __name__ == '__main__':
    main()
