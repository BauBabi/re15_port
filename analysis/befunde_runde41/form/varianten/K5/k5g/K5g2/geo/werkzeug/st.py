# Stichprobe des Orchestrators (Clips 10/12 ganz, sonst jedes 4. Bild; 285 Bilder x 5 Ansichten = 1425) aus den Voll-Laeufen
import json, sys
for pf in sys.argv[1:]:
    d = json.load(open(pf)); erg = d['erg']
    a0 = erg[0]['ansicht']; fol = [(r['clip'], r['bild']) for r in erg if r['ansicht'] == a0]
    rest = [x for x in fol if x[0] not in (10, 12)][::4]
    wahl = set(x for x in fol if x[0] in (10, 12)) | set(rest)
    sub = [r for r in erg if (r['clip'], r['bild']) in wahl]
    for les in ('ab4', 'ab5'):
        v = [sum(r[les]['streif']) for r in sub]
        print(pf, les, 'Ansichten', len(sub), 'Summe', sum(v), 'ueber 20:', sum(1 for x in v if x > 20), 'groesste', max(v))
