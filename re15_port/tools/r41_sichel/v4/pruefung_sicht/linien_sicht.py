# Sicht-Pruefer V4: eigene Linienmessung (Memory reai-v2-beschriftungslinien-messen)
import numpy as np, cv2, json, itertools
from PIL import Image
V80 = r'C:/workspace/git/reAi_v2/Version_80%/'
out = {}
for b, texte in ((1, ['Auge','Haare','Nase']), (2, ['Auge seitl.','Haare seitl.','Spitze Ende','Beginn','Knick'])):
    a = np.asarray(Image.open(V80+'Beschreibung%d.png'%b).convert('RGB')).astype(int)
    v = np.asarray(Image.open(V80+'Beschreibung%d_Vorlage.png'%b).convert('RGB')).astype(int)
    H, W = a.shape[:2]
    # Linienfarbe = haeufigste Nichtweiss/Nichtgrau-Farbe im Randfeld
    rand = a[:, 640:].reshape(-1,3)
    bunt = rand[(np.abs(rand[:,0]-rand[:,1])>40)]
    farbe = np.array([np.bincount(bunt[:,i]).argmax() for i in range(3)])
    m = (np.abs(a-farbe).sum(2) <= 60)
    # Gegenprobe: Linienpixel im Bildteil = Differenz Beschreibung - Vorlage
    d = (np.abs(a[:, :640]-v).max(2) > 40)
    out['bild%d'%b] = dict(linienfarbe=farbe.tolist(), maske_bildteil=int(m[:, :640].sum()), diff_bildteil=int(d.sum()),
                           schnitt=int((m[:, :640] & d).sum()))
    # Textbloecke im Randfeld: Glyphen = Maske rechts von x=640, Zeilen clustern
    mr = m.copy(); mr[:, :640] = False
    # Linien = 8er-Komponenten der Maske mit Breite > 40
    n, lab, st, cen = cv2.connectedComponentsWithStats(m.astype(np.uint8), connectivity=8)
    linien = [i for i in range(1, n) if st[i, cv2.CC_STAT_WIDTH] > 40]
    glyph = np.zeros_like(m)
    for i in range(1, n):
        if i not in linien: glyph |= (lab == i)
    # Textzeilen: Zeilenprofil der Glyphen
    prof = glyph[:, 640:].sum(1) > 0
    zeilen = []; y = 0
    while y < H:
        if prof[y]:
            y0 = y
            while y < H and prof[y]: y += 1
            zeilen.append((y0, y-1))
        else: y += 1
    # Zeilen mit Abstand <= 12 zu Bloecken
    bloecke = []
    for z in zeilen:
        if bloecke and z[0] - bloecke[-1][1] <= 11: bloecke[-1] = (bloecke[-1][0], z[1])
        else: bloecke.append(z)
    # Skelett-Enden der Linien (Zhang-Suen via cv2.ximgproc fehlt evtl -> einfache Enden: Pixel mit 1 Nachbarn nach Thinning)
    lm = np.isin(lab, linien).astype(np.uint8)
    try:
        sk = cv2.ximgproc.thinning(lm*255) > 0
    except Exception:
        from skimage.morphology import skeletonize
        sk = skeletonize(lm > 0)
    k = cv2.filter2D(sk.astype(np.uint8), -1, np.ones((3,3)), borderType=cv2.BORDER_CONSTANT)
    enden = [(int(x), int(y)) for y, x in zip(*np.nonzero(sk & (k == 2)))]
    enden_bild = [e for e in enden if e[0] < 640]
    enden_rand = [e for e in enden if e[0] >= 640]
    # Linienstart je Textblock = Randende mit kleinstem |y - Blockmitte|
    md = cv2.dilate(m.astype(np.uint8), np.ones((3,3)))
    def beleg(p, q):
        L = max(1, int(np.hypot(q[0]-p[0], q[1]-p[1]) * 2)); t = np.linspace(0,1,L)
        xs = np.round(p[0] + t*(q[0]-p[0])).astype(int); ys = np.round(p[1] + t*(q[1]-p[1])).astype(int)
        return float(md[ys, xs].mean())
    starts = []
    for (y0, y1) in bloecke:
        ym = (y0+y1)/2
        s = min(enden_rand, key=lambda e: abs(e[1]-ym)) if enden_rand else None
        starts.append(s)
    mat = [[beleg(s, e) for e in enden_bild] for s in starts]
    # beste Permutation (kreuzungsfrei nicht erzwungen, nur Summe)
    best = None
    for perm in itertools.permutations(range(len(enden_bild)), len(starts)):
        sm = sum(mat[i][perm[i]] for i in range(len(starts)))
        if best is None or sm > best[0]: best = (sm, perm)
    res = []
    for i, (y0, y1) in enumerate(bloecke):
        j = best[1][i]
        alt = sorted([(mat[i][jj], enden_bild[jj]) for jj in range(len(enden_bild)) if jj != j], reverse=True)[:2]
        res.append(dict(text=texte[i] if i < len(texte) else '?', zeilen=(y0, y1), start=starts[i], ende=enden_bild[j],
                        belegung=round(mat[i][j],3), alternativen=[(round(a_,3), e) for a_, e in alt]))
    out['bild%d'%b].update(bloecke=bloecke, enden_bild=enden_bild, enden_rand=enden_rand, zuordnung=res)
json.dump(out, open('linien_sicht.json','w'), indent=1)
for b in out:
    print(b, 'farbe', out[b]['linienfarbe'], 'maske/diff/schnitt', out[b]['maske_bildteil'], out[b]['diff_bildteil'], out[b]['schnitt'])
    for r in out[b]['zuordnung']: print('  ', r)
