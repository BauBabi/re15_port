"""Zonen in Vorlagenkoordinaten, Abschnitte, Flaechen je Zone (Kennungs-Render Spiel-Asset), Ganzteil-Ergaenzung."""
import sys, os, json
import numpy as np, cv2
from scipy import ndimage as ndi
sys.path.insert(0, 'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/farbe_lesung/V4F_skripte')
import v4f
from zonen_spec import SPEC, lade, VS, AB, D, lies, key

HIER = os.path.dirname(os.path.abspath(__file__))
TEILNAME = {0: 'Huefte', 1: 'Schwanz 1', 2: 'Schwanz 2', 3: 'Schwanz 3', 4: 'Schwanz 4', 5: 'Schwanz 5', 6: 'Schwanz 6 (Spitze)', 7: 'Ruecken/Wirbel',
            8: 'Oberschenkel l', 9: 'Oberschenkel r', 10: 'Brust/Buckel', 11: 'Oberarm l', 12: 'Unterarm l', 13: 'Hand l', 14: 'Kralle l1', 15: 'Kralle l2',
            16: 'Oberarm r', 17: 'Unterarm r', 18: 'Hand r', 19: 'Kralle r1', 20: 'Kralle r2', 21: 'Hals', 22: 'Kopf', 23: 'Unterkiefer',
            24: 'Unterschenkel l', 25: 'Fuss l', 26: 'Unterschenkel r', 27: 'Fuss r', 28: 'Kralle l3', 29: 'Kralle r3', 30: 'Kralle l4', 31: 'Kralle r4'}
# Zonen, die der Nutzer als ganzes Teil benennt (Ganzteil-Ergaenzung um die Rueckseite)
GANZTEIL = {'schwanz', 'zipfel', 'kopf_hinten', 'bein_hinten_links', 'bein_hinten_rechts', 'vorderbein_rechts', 'gesicht_vorn',
            'arm_hand_krallen_vorn_rechts', 'vorderbein_links', 'oberkoerper_hinten', 'oberkoerper_vorn', 'fuss_hinten_links'}
ABSCHNITTE = {'bein_hinten_links': 'lila', 'bein_hinten_rechts': 'lila', 'vorderbein_rechts': 'lila'}

asset = HIER + '/asset'
k = v4f.kand(asset)
NFL = {b: len(m.tuv) + len(m.quv) for b, m in enumerate(k.mo.meshes)}


def nach_vorlage(mask_l, lf, n):
    v = VS[lf]; a = AB[str(n)]['aehnlich']; s = a['massstab']; tx, ty = a['versatz']
    # beschriftet -> Grundlage: g = l - (dx, dy); Grundlage -> Vorlage: V = s*g + t
    M = np.array([[s, 0, tx - s * v['dx']], [0, s, ty - s * v['dy']]], np.float32)
    f = cv2.warpAffine(mask_l.astype(np.float32), M, (640, 480), flags=cv2.INTER_AREA)
    return f >= 0.5, M


def abschnitte(mask_l, lf, farbe):
    Mk, fig, _ = lade(lf)
    w = cv2.dilate(Mk[farbe].astype(np.uint8), np.ones((5, 5), np.uint8)) > 0
    lab, nl = ndi.label(mask_l & ~w)
    teile = []
    for i in range(1, nl + 1):
        q = lab == i
        if q.sum() >= 150:
            teile.append(q)
    teile.sort(key=lambda q: np.nonzero(q)[0].mean())
    # Trennlinie an die Nachbarn zurueckgeben
    out = []
    for q in teile:
        out.append(cv2.dilate(q.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool) & mask_l)
    return out


def main():
    ml = dict(np.load(HIER + '/zonen_beschriftet.npz'))
    KEN = {n: dict(np.load(HIER + '/kennung%d.npz' % n)) for n in (1, 2)}
    SZ = {n: cv2.imread(HIER + '/szene%d.png' % n) for n in (1, 2)}
    VORL = {n: v4f.original(n) for n in (1, 2)}
    zonen = []
    for z in SPEC:
        zonen.append((z, ml[z['name']]))
        if z['name'] in ABSCHNITTE:
            for i, q in enumerate(abschnitte(ml[z['name']], z['datei'], ABSCHNITTE[z['name']])):
                zz = dict(z); zz = {**z, 'name': '%s_abschnitt%d' % (z['name'], i + 1), 'text': 'Abschnitt %d von oben (lila Trennlinien)' % (i + 1),
                                    'farbe': 'lila', 'bau': 'abschnitt', 'eltern': z['name']}
                zonen.append((zz, q))
    vmasks = {}; ergebnis = []
    for z, m in zonen:
        n = z['bild']; K = KEN[n]
        mv, M = nach_vorlage(m, z['datei'], n)
        vmasks[z['name']] = mv
        cnt, _ = cv2.findContours(mv.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        polys = [c[:, 0, :].tolist() for c in cnt if len(c) >= 3]
        mod = K['maske'] & mv
        mesh = K['mesh'][mod]; fl = K['flaeche'][mod]
        allm = K['mesh'][K['maske']]; allf = K['flaeche'][K['maske']]
        tot = {}
        for a, b in zip(allm.tolist(), allf.tolist()):
            tot[(a, b)] = tot.get((a, b), 0) + 1
        cnt_z = {}
        for a, b in zip(mesh.tolist(), fl.tolist()):
            cnt_z[(a, b)] = cnt_z.get((a, b), 0) + 1
        fl_liste = sorted(([int(a), int(b // 100000), int(b % 100000), int(c), round(c / tot[(a, b)], 3)] for (a, b), c in cnt_z.items()),
                          key=lambda r: (-r[3]))
        je_mesh = {}
        for a in np.unique(mesh):
            vis_all = int((allm == a).sum()); inz = int((mesh == a).sum())
            je_mesh[int(a)] = dict(teil=TEILNAME[int(a)], px_in_zone=inz, px_sichtbar_gesamt=vis_all, anteil_mesh=round(inz / vis_all, 3),
                                   flaechen_in_zone=int(len({b for (aa, b) in cnt_z if aa == a})), flaechen_mesh=NFL[int(a)])
        ganz = []
        if z['name'] in GANZTEIL:
            ganz = sorted(int(a) for a, d in je_mesh.items() if d['anteil_mesh'] >= 0.5 and d['px_in_zone'] >= 0.03 * max(int(mod.sum()), 1))
        lab = cv2.cvtColor((VORL[n][mv].reshape(-1, 1, 3)).astype(np.uint8), cv2.COLOR_RGB2LAB).reshape(-1, 3).astype(float) if mv.any() else None
        lab_med = None if lab is None else [round(float(np.median(lab[:, 0])) * 100 / 255, 1), round(float(np.median(lab[:, 1])) - 128, 1),
                                           round(float(np.median(lab[:, 2])) - 128, 1)]
        e = dict(zone=z['name'], bild=n, quelle=z['datei'], nutzertext=z['text'], umrissfarbe=z['farbe'], bau=z['bau'],
                 eltern=z.get('eltern'), px_vorlage=int(mv.sum()), px_modell_in_zone=int(mod.sum()),
                 deckung_modell=round(float(mod.sum() / max(mv.sum(), 1)), 3),
                 bbox_vorlage=None if not mv.any() else [int(np.nonzero(mv)[1].min()), int(np.nonzero(mv)[0].min()), int(np.nonzero(mv)[1].max()), int(np.nonzero(mv)[0].max())],
                 polygone_vorlage=polys, vorlage_lab_median_roh=lab_med,
                 meshes=je_mesh, flaechen=fl_liste, ganzteil_meshes=ganz,
                 ganzteil_flaechen_gesamt=int(sum(NFL[a] for a in ganz)),
                 ganzteil_flaechen_unsichtbar=int(sum(NFL[a] for a in ganz) - sum(je_mesh[a]['flaechen_in_zone'] for a in ganz)))
        ergebnis.append(e)
        print('%-46s B%d Vpx %5d Modell %5d (%.2f) Meshes %s ganz %s' % (z['name'], n, mv.sum(), mod.sum(), e['deckung_modell'],
              {a: (d['px_in_zone'], d['anteil_mesh']) for a, d in sorted(je_mesh.items(), key=lambda t: -t[1]['px_in_zone'])[:6]}, ganz))
    np.savez_compressed(HIER + '/zonen_vorlage.npz', **vmasks)
    json.dump(ergebnis, open(HIER + '/zonen_roh.json', 'w', encoding='utf-8'), ensure_ascii=False)


if __name__ == '__main__':
    main()
