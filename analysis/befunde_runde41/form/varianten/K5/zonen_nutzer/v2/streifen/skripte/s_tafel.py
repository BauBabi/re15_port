"""Tafel 23_streifen_herkunft.png fuer den Nutzer (nur Messbilder, nichts geaendert)."""
import json
import cv2
from PIL import Image, ImageDraw, ImageFont
from s_basis import *  # noqa: F401,F403

FONT = 'C:/Windows/Fonts/arial.ttf'; FONTB = 'C:/Windows/Fonts/arialbd.ttf'
F = lambda g, b=False: ImageFont.truetype(FONTB if b else FONT, g)
# Runde-1-Tafeln, die der Nutzer gesehen hat: byte-gleich (md5 8ba19871.../0fb4ea69...) mit Commit 239eac698 -> von dort lesen
import subprocess
TAFEL_COMMIT = '239eac698'
TAFEL00 = 'analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/tafeln/00_uebersicht.png'
TAFEL08 = 'analysis/befunde_runde41/form/varianten/K5/zonen_nutzer/v2/tafeln/08_Vorne_Oberkoerper.png'


def lies_commit(p):
    b = subprocess.run(['git', '-C', WT, 'show', TAFEL_COMMIT + ':' + p], capture_output=True, check=True).stdout
    return cv2.cvtColor(cv2.imdecode(np.frombuffer(b, np.uint8), 1), cv2.COLOR_BGR2RGB)
FAR = {10: (245, 130, 48), 21: (150, 150, 20), 22: (60, 170, 75), 23: (40, 120, 220)}
NAMEN = {10: 'Brust/Buckel (Teil 10)', 21: 'Hals (Teil 21)', 22: 'Kopf (Teil 22)', 23: 'Unterkiefer (Teil 23)'}
PW, PH = 620, 520   # Feldgroesse


def lies(p):
    return cv2.cvtColor(cv2.imdecode(np.fromfile(p, np.uint8), 1), cv2.COLOR_BGR2RGB)


def einpassen(rgb, w=PW, h=PH, inter=Image.NEAREST):
    im = Image.fromarray(rgb)
    s = min(w / im.width, h / im.height)
    im = im.resize((max(1, int(im.width * s)), max(1, int(im.height * s))), inter)
    out = Image.new('RGB', (w, h), (255, 255, 255)); out.paste(im, ((w - im.width) // 2, (h - im.height) // 2))
    return out, s, ((w - im.width) // 2, (h - im.height) // 2)


def teilbild(idx, tm, extra=None):
    m = np.where(idx >= 0, tm[np.maximum(idx, 0)], -1)
    out = np.full(m.shape + (3,), 255, np.uint8); out[m >= 0] = 200
    for t, f in FAR.items():
        out[m == t] = f
    return out, m


def kontur(rgb, msk, farbe, d=1):
    c, _ = cv2.findContours(msk.astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    cv2.drawContours(rgb, c, -1, farbe, d)
    return rgb


def feld(titel, bild, unter=None, w=PW):
    gr = 22
    unten = []
    if unter:
        d = ImageDraw.Draw(Image.new('RGB', (1, 1)))
        for absatz in unter.split('\n'):
            zeile = ''
            for wort in absatz.split(' '):
                t = (zeile + ' ' + wort).strip()
                if d.textlength(t, font=F(19)) > w - 10:
                    unten.append(zeile); zeile = wort
                else:
                    zeile = t
            unten.append(zeile)
    H = 34 + bild.height + 8 + 24 * len(unten)
    out = Image.new('RGB', (w, H), (255, 255, 255))
    dr = ImageDraw.Draw(out)
    dr.text((0, 4), titel, font=F(gr, True), fill=(0, 0, 0))
    out.paste(bild, (0, 34))
    dr.rectangle([0, 34, bild.width - 1, 34 + bild.height - 1], outline=(170, 170, 170))
    for i, z in enumerate(unten):
        dr.text((2, 34 + bild.height + 6 + 24 * i), z, font=F(19), fill=(30, 30, 30))
    return out


def legende(w=PW):
    out = Image.new('RGB', (w, 64), (255, 255, 255)); dr = ImageDraw.Draw(out)
    x = 0
    for i, (t, f) in enumerate(FAR.items()):
        xx = (i % 2) * (w // 2); yy = (i // 2) * 30
        dr.rectangle([xx, yy + 4, xx + 22, yy + 24], fill=f); dr.text((xx + 30, yy + 3), NAMEN[t], font=F(19), fill=(0, 0, 0))
    return out


def unter_einander(*ims, abstand=6):
    w = max(i.width for i in ims); h = sum(i.height for i in ims) + abstand * (len(ims) - 1)
    out = Image.new('RGB', (w, h), (255, 255, 255)); y = 0
    for i in ims:
        out.paste(i, (0, y)); y += i.height + abstand
    return out


def zeile_bauen(felder, abstand=26):
    w = sum(f.width for f in felder) + abstand * (len(felder) - 1); h = max(f.height for f in felder)
    out = Image.new('RGB', (w, h), (255, 255, 255)); x = 0
    for f in felder:
        out.paste(f, (x, 0)); x += f.width + abstand
    return out


def schnittbild(w=PW, h=PH):
    """Senkrechter Schnitt durch die Figur in Blickrichtung der Bild-2-Kamera, durch die Mitte von Kopf-Flaeche 4 (groesster Keil)."""
    k = kand(ASSET); lage, gier, clip, bild = k.stand(2); Mg, kam = g_matrix(2)
    s, c = EA.sin_q12(gier), EA.cos_q12(gier)
    Y = np.array([[c, 0, s], [0, 4096, 0], [-s, 0, c]], float) / 4096.0
    Vk = np.asarray(kam.V, float) / 4096.0
    wt = welt_dreiecke(k.mo, k.kf(clip, bild))
    def zu_sicht(V):
        return ((Vk @ ((Y @ V.T).T + np.array(lage, float)).T).T + np.asarray(kam.t, float))
    sicht = [(tag, zu_sicht(V)) for tag, V in wt if tag[0] in FAR]
    ziel = [V for tag, V in sicht if tag[0] == 22 and tag[1] == 4 and tag[2] == 1]
    x0 = float(np.mean([V[:, 0].mean() for V in ziel]))
    segs = []
    for tag, V in sicht:
        d = V[:, 0] - x0
        pts = []
        for a, b in ((0, 1), (1, 2), (2, 0)):
            if (d[a] < 0) != (d[b] < 0):
                t = d[a] / (d[a] - d[b]); pts.append(V[a] + t * (V[b] - V[a]))
        if len(pts) == 2:
            segs.append((tag[0], pts[0][[2, 1]], pts[1][[2, 1]]))
    zc0 = np.mean([V.mean(0) for V in ziel], axis=0)
    zmin, zmax = zc0[2] - 1100, zc0[2] + 1300
    ymin, ymax = zc0[1] - 900, zc0[1] + 700
    segs = [(t, a, b) for t, a, b in segs if zmin <= min(a[0], b[0]) and max(a[0], b[0]) <= zmax and ymin <= min(a[1], b[1]) and max(a[1], b[1]) <= ymax]
    rand = 40
    sc = min((w - 2 * rand) / (zmax - zmin), (h - 2 * rand - 30) / (ymax - ymin))
    im = Image.new('RGB', (w, h), (255, 255, 255)); dr = ImageDraw.Draw(im)
    P = lambda p: (rand + (p[0] - zmin) * sc, rand + 30 + (p[1] - ymin) * sc)
    # Sehstrahl durch die Mitte von Kopf-Flaeche 4: Kamera (0,0) -> Punkt
    zc = np.mean([V.mean(0) for V in ziel], axis=0)
    for teil in (21, 10, 23, 22):
        for t, a, b in segs:
            if t == teil:
                dr.line([P(a), P(b)], fill=FAR[t], width=4 if t == 22 else 3)
    za = zmin + 20; ya = zc[1] * za / zc[2]
    dr.line([P((za, ya)), P((zc[2], zc[1]))], fill=(200, 0, 0), width=2)
    dr.ellipse([P((zc[2], zc[1]))[0] - 6, P((zc[2], zc[1]))[1] - 6, P((zc[2], zc[1]))[0] + 6, P((zc[2], zc[1]))[1] + 6], outline=(200, 0, 0), width=3)
    dr.text((rand, 4), 'Kamera links, Blick nach rechts  \u2192', font=F(19), fill=(0, 0, 0))
    dr.text((P((za, ya))[0], P((za, ya))[1] + 6), 'Sehstrahl', font=F(18), fill=(200, 0, 0))
    # Massstab 200 Einheiten
    x1, y1 = rand, h - 18
    dr.line([(x1, y1), (x1 + 200 * sc, y1)], fill=(0, 0, 0), width=3)
    dr.text((x1 + 200 * sc + 8, y1 - 11), '200 Modell-Einheiten (Kopf ist ca. 1.800 lang)', font=F(17), fill=(0, 0, 0))
    return im


def main():
    sb = json.load(open(AUS + '/schnitt_bild2.json', encoding='utf-8'))
    hal = json.load(open(AUS + '/hals.json', encoding='utf-8'))
    hs = json.load(open(AUS + '/hals_spiel.json', encoding='utf-8'))
    sv = {n: json.load(open(AUS + '/spielvergleich_%s.json' % n, encoding='utf-8')) for n in ('b2_5090', 'vorn_5080', 'griff_5090')}
    # ---------- Zeile A: Buckel ----------
    t00 = lies_commit(TAFEL00)[1560:1800, 1120:1560]
    t08 = lies_commit(TAFEL08)[930:1480, 1110:1900]
    a1 = unter_einander(einpassen(t00, PW, 250, Image.LANCZOS)[0], einpassen(t08, PW, 250, Image.LANCZOS)[0])
    A1 = feld('1  Ihre Tafel (Runde 1), Ausschnitt', a1,
              'Oben Tafel 00: grüne Keile im orangen Buckel. Unten Tafel 08: graue Keile im grünen Teil. Grün (oben) und Grau (unten) sind '
              'in beiden Tafeln der Kopf.')
    d = dict(np.load(AUS + '/r_K5e_f1_2.npz'))
    X0, X1, Y0, Y1 = 110, 430, 70, 300
    mal, ma = teilbild(d['maler_i'], d['tm']); puf, mb = teilbild(d['puffer_i'], d['tm'])
    keil = np.isin(ma, (22, 23)) & np.isin(mb, (10, 21))
    mal2 = kontur(mal.copy(), keil, (200, 0, 0), 1)
    sl = (slice(Y0, Y1), slice(X0, X1))
    a2, s2, o2 = einpassen(mal2[sl], PW, PH - 70)
    dr = ImageDraw.Draw(a2)
    for e in sb['keil_flaechen'][:5]:
        x0, y0, x1, y1 = e['bbox_grundlage']
        sel = keil & (d['maler_i'] == d['maler_i'])  # nur fuer Mitte
        cx = ((x0 + x1) / 2 - X0) * s2 + o2[0]; cy = ((y0 + y1) / 2 - Y0) * s2 + o2[1]
        nr = e['flaeche'].split(',')[1].strip().split()[1]
        dr.text((cx - 10, cy - 10), 'K' + nr, font=F(18, True), fill=(0, 0, 0))
    A2 = feld('2  Modell wie im Spiel gezeichnet', unter_einander(a2, legende()),
              'Rot umrandet = Kopfflächen, die gezeichnet werden, obwohl Buckel oder Hals davor liegen (%d Bildpunkte in Bild 2). '
              'K4, K15, K205, K12, K13 = Kopf-Flächen Nr. 4, 15, 205, 12, 13.' % sb['keil_px'])
    # Spielbild nah
    g = lies(AUS + '/spiel/b2_5090/hwnd_08.png'); s5 = dict(np.load(AUS + '/spiel_b2_5090.npz'))
    m5 = np.where(s5['maler_i'] >= 0, s5['tm'][np.maximum(s5['maler_i'], 0)], -1)
    p5 = np.where(s5['puffer_i'] >= 0, s5['tm'][np.maximum(s5['puffer_i'], 0)], -1)
    k5 = np.isin(m5, (22, 23)) & np.isin(p5, (10, 21))
    gx0, gx1, gy0, gy1 = 225, 405, 160, 280
    hell = np.clip(g.astype(float) * 2.5, 0, 255).astype(np.uint8)
    links = hell[gy0:gy1, gx0:gx1].copy()
    rechts = kontur(hell.copy(), k5, (0, 255, 0), 1)[gy0:gy1, gx0:gx1]
    a3 = unter_einander(einpassen(links, PW, 225)[0], einpassen(rechts, PW, 225)[0])
    e5 = sv['b2_5090']['alle_ordnungsfehler']; e8 = sv['vorn_5080']['alle_ordnungsfehler']
    A3 = feld('3  Spiel 5090, heutiges Modell, nah', a3,
              'Echtes Spielfenster (aufgehellt ×2,5 nur zum Ansehen). Unten grün umrandet: die Keil-Stellen aus Feld 2. '
              'An allen falsch überdeckten Stellen gleicht das Spielbild der gezeichneten Reihenfolge (Abweichung %.1f) und nicht der '
              'räumlichen (%.1f); 5080 von vorn genauso (%.1f gegen %.1f).' % (e5['abweichung_spiel_zu_maler'], e5['abweichung_spiel_zu_tiefenpuffer'],
                                                                                 e8['abweichung_spiel_zu_maler'], e8['abweichung_spiel_zu_tiefenpuffer']))
    a4a, _, _ = einpassen(puf[sl], PW, 230)
    a4 = unter_einander(a4a, schnittbild(PW, 300))
    A4 = feld('4  Was räumlich vorn liegt + Schnitt', a4,
              'Oben: dieselbe Ansicht, aber je Bildpunkt nur das vorderste Teil – dort gibt es keine Kopf-Keile im Buckel. Unten: senkrechter '
              'Schnitt in Blickrichtung durch Kopf-Fläche 4 (roter Kreis). Die Umrisse von Kopf (grün), Hals (oliv) und Buckel (orange) '
              'liegen ineinander: der Kopf steckt im Buckel und im Hals.')
    # ---------- Zeile B: Hals-Unterseite ----------
    h = dict(np.load(AUS + '/hals_griff.npz'))
    gg = lies(AUS + '/spiel/griff_5090/hwnd_08.png')
    sechs = np.zeros(h['maler_i'].shape, bool)
    for kk, t in enumerate(h['tags']):
        if int(t[0]) == 22 and int(t[2]) == 1 and int(t[1]) in (1, 8, 9, 194, 201, 209):
            sechs |= h['maler_i'] == kk
    hx0, hx1, hy0, hy1 = 330, 500, 40, 190
    hm, _ = teilbild(h['maler_i'], h['tm']); hm[sechs] = (220, 0, 0)
    hp, mpp = teilbild(h['puffer_i'], h['tm'])
    sechs_p = np.zeros(sechs.shape, bool)
    for kk, t in enumerate(h['tags']):
        if int(t[0]) == 22 and int(t[2]) == 1 and int(t[1]) in (1, 8, 9, 194, 201, 209):
            sechs_p |= h['puffer_i'] == kk
    hp[sechs_p] = (220, 0, 0)
    B1 = feld('5  Kopf-Unterseite: 6 Flächen', einpassen(hm[hy0:hy1, hx0:hx1], PW, PH - 70)[0],
              'Spiel 5090, Griff-Haltung (Clip 27 Bild 20). Rot = Kopf-Flächen 1, 8, 9, 194, 201, 209. Sie liegen außen an der '
              'Kopf-Unterseite zum Hals hin.')
    B2 = feld('6  Räumlich vorn: dieselben 6 Flächen', einpassen(hp[hy0:hy1, hx0:hx1], PW, PH - 70)[0],
              'Auch wenn je Bildpunkt nur das Vorderste zählt, bleiben sie sichtbar (%d gegen %d Bildpunkte). Hier steckt nichts durch: '
              'es sind echte Außenflächen.' % (hal['ansichten']['5090 Schnitt 11, Griff Clip 27 Bild 20']['rot_tiefenpuffer'],
                                              hal['ansichten']['5090 Schnitt 11, Griff Clip 27 Bild 20']['rot_maler']))
    gz = np.clip(gg.astype(float) * 2.0, 0, 255).astype(np.uint8)
    gz1 = gz[hy0:hy1, hx0:hx1].copy(); gz2 = kontur(gz.copy(), sechs, (0, 255, 0), 1)[hy0:hy1, hx0:hx1]
    B3 = feld('7  Spiel 5090, heutiges Modell, nah', unter_einander(einpassen(gz1, PW, 225)[0], einpassen(gz2, PW, 225)[0]),
              'Echtes Spielfenster (aufgehellt ×2). Unten grün umrandet die 6 Flächen. Dort sind %.0f %% der Bildpunkte warm-rot, '
              'in der Haut daneben %.0f %%.' % (100 * hs['kern_warmrot_anteil'], 100 * hs['ring_warmrot_anteil']))
    # Textur der 6 Flaechen
    k = kand(ASSET)
    kacheln = []
    for fi in (1, 8, 9, 194, 201, 209):
        uv = k.mo.meshes[22].quv[fi]
        po = (uv[5] & 0xF) * 128; z = ((uv[2] >> 6) & 0x1FF) - k.tex.clut_y
        pts = np.array([(uv[0] + po, uv[1]), (uv[3] + po, uv[4]), (uv[9] + po, uv[10]), (uv[6] + po, uv[7])], np.int32)
        xa, ya = pts.min(0); xb, yb = pts.max(0) + 1
        tex = k.tex.lut[z][k.tex.px[ya:yb, xa:xb]][..., :3]
        kacheln.append(einpassen(tex, 190, 110)[0])
    tf = Image.new('RGB', (PW, 2 * 140), (255, 255, 255)); dt = ImageDraw.Draw(tf)
    for i, kk in enumerate(kacheln):
        x = (i % 3) * 205; y = (i // 3) * 140
        tf.paste(kk, (x, y + 24)); dt.text((x, y), 'Kopf-Fläche %d' % (1, 8, 9, 194, 201, 209)[i], font=F(18), fill=(0, 0, 0))
    B4 = feld('8  Die Textur dieser 6 Flächen', tf,
              'Alle 6 holen ihre Farbe aus Texturzeile 0 und sind als "Maulinneres" eingetragen; 93-100 % ihrer Bildpunkte sind '
              'warm-rot. Die Streifen sind also Farbe, kein Durchstoßen.')
    # ---------- zusammen ----------
    kopf = Image.new('RGB', (4 * PW + 3 * 26, 150), (255, 255, 255)); dk = ImageDraw.Draw(kopf)
    dk.text((0, 0), '23  Woher kommen die Streifen?  (nur gemessen, nichts geändert)', font=F(36, True), fill=(0, 0, 0))
    dk.text((0, 52), 'Ihr Wort zur Tafel: "bei den Huckel oben auf der Vorderseite hast du teilweise wieder grün streifen drin". '
            'Ihr Wort zum Spiel (K5e): "auf der Unterseite am Hals hat er komische rote streifen".', font=F(21), fill=(30, 30, 30))
    dk.text((0, 84), 'Oben: die Keile am Buckel = der Kopf steckt im Buckel und wird wegen der Zeichen-Reihenfolge darüber gemalt – in der Tafel '
            'UND im Spiel.', font=F(21, True), fill=(150, 0, 0))
    dk.text((0, 114), 'Unten: die roten Streifen am Hals = 6 Kopf-Flächen außen mit roter Maul-Farbe – eine Farbzuordnung, kein Durchstoßen.',
            font=F(21, True), fill=(150, 0, 0))
    za = zeile_bauen([A1, A2, A3, A4]); zb = zeile_bauen([B1, B2, B3, B4])
    trenn = Image.new('RGB', (za.width, 40), (255, 255, 255)); ImageDraw.Draw(trenn).line([(0, 20), (za.width, 20)], fill=(160, 160, 160), width=2)
    ges = unter_einander(kopf, za, trenn, zb, abstand=10)
    rand = Image.new('RGB', (ges.width + 60, ges.height + 60), (255, 255, 255)); rand.paste(ges, (30, 30))
    os.makedirs(ZIEL, exist_ok=True)
    rand.save(ZIEL + '/23_streifen_herkunft.png')
    print(rand.size)


if __name__ == '__main__':
    main()
