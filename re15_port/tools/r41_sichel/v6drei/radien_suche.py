"""SF V6 drei Punkte: Radien der Gelenke 3/4/5 (Glied 19/20/21) nach der Laengenaenderung (Bauer, 2026-11-08).

    python radien_suche.py --start PROT.json [--r3 ..] [--r4 ..] [--r5 ..]

Start = Radien aus schwanz_form.passen (drei_bau --radien-frei, Protokoll 'schwanz.radien.radien_neu'). passen trifft die Foto-Breite
(T-F1), liess aber am Gelenk 19/20 einen Breitensprung im Bild (T-F5c 21 %, P2: der lange Glied 19 endet dort im Zickzack-Knick).
Gitter ueber r3/r4/r5 (r6/r7 <= r5), je Kandidat Bau (Stufe s) und riegel_v5f T-F1 / T-F5c sowie Steigungsknick (T-F5d).
Lauf 1 r3 200/225/250 x r4 140/165/187 x r5 65/75, Lauf 2 r3 240/255 x r4 155/165/175 x r5 75/80/85 (Ausgabe im Dossier);
Lauf 3 (Laengen der Suche 1121/350/204/608) r3 245/255 x r4 160/165/170 x r5 75/80/85; gewaehlt r3 255 / r4 170 / r5 80:
T-F1 max 14 %, T-F5c gruen (Rasterboden 1 px), Knick 10,5 Grad; r4 160/165 liessen T-F5c rot (11 %).
"""
import os, sys, json, argparse, itertools, re
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import drei_bau as DB            # noqa: E402
import kopf_orig_bau as KB       # noqa: E402
import riegel_v5f as RV5         # noqa: E402
import schwanz_form as SF        # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--start', required=True)
    ap.add_argument('--r3', default='240,255'); ap.add_argument('--r4', default='155,165,175'); ap.add_argument('--r5', default='75,80,85')
    a = ap.parse_args()
    base = json.load(open(a.start))['schwanz']['radien']['radien_neu']
    base = {int(k) if k.isdigit() else k: v for k, v in base.items()}
    gl = lambda s: [float(x) for x in s.split(',')]
    for r3, r4, r5 in itertools.product(gl(a.r3), gl(a.r4), gl(a.r5)):
        rad = dict(base); rad[3], rad[4], rad[5] = r3, r4, r5
        rad[6], rad[7] = min(base[6], r5), min(base[7], r5)
        neu, _p = DB.bauen(stufen='s', radien=rad)
        p = os.path.join(KB.AUS, 'EM30_V6rs.EMD'); open(p, 'wb').write(neu)
        R = RV5.pruefen(p, os.path.join(KB.AUS, 'EM30_V5k.EMD'), mit_kopf=False)
        z = {e['name'].split(' ')[0]: e for e in R['zeilen']}
        f1 = [abs(float(x)) for x in re.findall(r'\(([+-]?\d+) %\)', z['T-F1']['ist'])]
        f5c = [float(x) for x in re.findall(r'\((\d+) %\)', z['T-F5c']['ist'])]
        kn = max(k['knick'] for k in SF.profil_knicke_geo(KB.teile(neu)[2]))
        print((r3, r4, r5, max(f1), max(f5c), kn, z['T-F1']['ok'], z['T-F5c']['ok']), flush=True)


if __name__ == '__main__':
    main()
