"""Mass 3: jede Glyphe der Panel-Beschriftung im (logischen) Fensterbild gegen die 8x8-Schrift aus TEX.TIM.
Treffer an Lage p: jeder Glyphen-Index != 0 hat im Bild genau EINE Farbe (alle Texel mit demselben Index gleich, keine Zwischentoene),
und die Farbe der Tinte (haeufigster Index) kommt unter den transparenten Texeln (Index 0) der Zelle nicht vor.
Ausgabe je Schild: gefundene Glyphen / Zeichen, Lagen (Vorschub), Farben je Index."""
import sys, numpy as np, schrift, logisch
SCHILDER = [('BRIDGE', (205, 45, 275, 70)), ('SET 80', (205, 62, 275, 85)), ('80', (85, 65, 112, 85)), ('A 100', (45, 128, 100, 145)),
            ('B 50', (128, 128, 175, 145)), ('C 30', (175, 128, 215, 145)), ('CAUTION', (118, 178, 235, 195)), ('HIGH PRESSURE', (118, 190, 235, 208))]
def treffer(img, c, x, y):
    g = schrift.glyphe(c)
    if y + 8 > img.shape[0] or x + 8 > img.shape[1]: return None
    zelle = img[y:y + 8, x:x + 8]
    farben = {}
    for i in np.unique(g):
        if i == 0: continue
        px = zelle[g == i]
        if len(np.unique(px.reshape(-1, 3), axis=0)) != 1:
            if i == 8: farben[8] = 'ohne Schatten'; continue      # dunkle Schrift: Schatten-Index 8 transparent
            return None
        farben[int(i)] = tuple(int(v) for v in px[0])
    tinte = max((int((g == i).sum()), i) for i in farben if i != 8)[1]
    ys, xs = np.where((g != 0) & (g != 8))                      # Tinten-Rechteck (Ziffern: Vorschub 6 < 8, Nachbar liegt in Spalte 6/7)
    box = np.zeros_like(g, bool); box[ys.min():ys.max() + 1, xs.min():xs.max() + 1] = True
    leer = zelle[(g == 0) & box].reshape(-1, 3)
    if any((leer == farben[tinte]).all(axis=1)): return None
    return farben
def pruefe(pfad, ausgabe=True):
    img, s, ein = logisch.lade(pfad)
    if ausgabe: print('%s: Skalierung %d, Bloecke einfarbig %.4f' % (pfad.split('/')[-1], s, ein))
    gesamt = [0, 0]
    for text, (x0, y0, x1, y1) in SCHILDER:
        gef = []
        for c in text:
            if c == ' ': continue
            lagen = [(x, y, treffer(img, c, x, y)) for y in range(y0, y1 - 7) for x in range(x0, x1 - 7)]
            lagen = [l for l in lagen if l[2]]
            gef.append((c, lagen))
        n_ok = sum(1 for c, l in gef if l)
        gesamt[0] += n_ok; gesamt[1] += len(gef)
        if ausgabe:
            lage = ' '.join('%s@%s' % (c, ('%d,%d' % (l[0][0], l[0][1])) if l else '-') for c, l in gef)
            fb = gef[0][1][0][2] if gef and gef[0][1] else {}
            print('  %-14s %d/%d Glyphen gleich der TEX.TIM-Schrift | %s | Farben je Index %s' % (text, n_ok, len(gef), lage, fb))
    return gesamt
if __name__ == '__main__':
    for p in sys.argv[1:]:
        g = pruefe(p)
        print('  GESAMT %d/%d' % tuple(g))
