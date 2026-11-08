# Leser A V3: Kopfbox der Vorderansichten. Seiten = dunkelste Spalte (Mittel L* ueber Augenzeile +-3)
# in [Augen-x - 45, Augen-x - 12] bzw. [Augen-x + 12, Augen-x + 50] = Rillen zu den Schultern;
# Oberkante = erste Alpha-Zeile in der Augenspalte. Kuppe = helle Flaeche L*>55 im Kopf oberhalb des Mauls.
import sys, json
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools/r41_sichel/v3')
from A_common import *
A = json.load(open(SCR + 'A_augen.json'))
out = {}
for nn in (17, 18, 19, 20, 21, 22, 23, 24):
    rgb, al = load(nn); L3 = lab(rgb); L = L3[..., 0]
    e = A['k%02d' % nn]; ex, ey = [int(round(v)) for v in e['mitte']]
    band = L[ey - 3:ey + 4].mean(0)
    sm = np.convolve(band, np.ones(3) / 3, 'same')
    lo = ex - 45 + int(np.argmin(sm[ex - 45:ex - 12])); hi = ex + 12 + int(np.argmin(sm[ex + 12:ex + 51]))
    top = int(np.nonzero(al[:, ex])[0].min())
    H = int(hbox(al)[4])
    x0b, x1b, y0b, y1b, _ = hbox(al)
    m = e.get('maul')
    my0 = m['bbox'][1] if m else None
    kup = np.zeros_like(al); 
    if my0:
        kup[top:my0, lo:hi + 1] = True
    kmask = kup & erode(al, 2)
    hell = kmask & (L > 50)
    rot = kmask & (L3[..., 1] > 15)
    lo, hi = int(lo), int(hi)
    baender = []
    for b in range(6):
        ya, yb = top + int(b * 0.05 * H), top + int((b + 1) * 0.05 * H)
        bm = np.zeros_like(al); bm[ya:yb, lo:hi + 1] = True; bm &= erode(al, 2)
        baender.append([round(b * 0.05, 2), round(float((bm & (L3[..., 1] > 15)).sum() / max(bm.sum(), 1)), 2),
                        round(float((bm & (L > 50)).sum() / max(bm.sum(), 1)), 2), round(float(L[bm].mean()), 1), round(float(L3[..., 1][bm].mean()), 1)])
    out[nn] = dict(baender_H_rot_hell_L_a=baender, links=lo, rechts=hi, breite=hi - lo, oben=top, H=H, ytop_bbox=int(y0b),
                   kuppe_hoehe=(my0 - top) if my0 else None,
                   kuppe_hell_anteil=round(float(hell.sum() / max(kmask.sum(), 1)), 3),
                   kuppe_rot_anteil=round(float(rot.sum() / max(kmask.sum(), 1)), 3),
                   kuppe_lab=stats(rgb, L3, kmask), hell_lab=stats(rgb, L3, hell))
    o = out[nn]
    print('birkin%02d H %d Kopf x %d-%d (B %d px = %.3f H) oben y %d; Auge (%d,%d) rel x %.2f; Kuppe bis Maul %s px; hell %.2f rot %.2f' % (
        nn, H, lo, hi, hi - lo, (hi - lo) / H, top, ex, ey, (ex - lo) / (hi - lo), o['kuppe_hoehe'], o['kuppe_hell_anteil'], o['kuppe_rot_anteil']))
    print('    Baender (ab Oberkante, Kopfspalten) [H, Anteil a*>15, Anteil L*>50, L*, a*]:', baender)
json.dump(out, open(SCR + 'A_kopfbox.json', 'w'), indent=1)
