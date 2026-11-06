"""Abnahme 2 PF: Panel 2030 im Fensterbild (gdigrab, 30 Bilder/s, verlustfrei). Eigene Auswertung.
panel_mass.py <lauf> <inc vor|nach>"""
import sys, json, math, re
import numpy as np
import bilder, panel_inc, schrift

lauf, welche = sys.argv[1], sys.argv[2]
G = panel_inc.grund('panel_%s.inc' % welche).astype(np.int16)
GL = panel_inc.glyphen('panel_%s.inc' % welche)
TRANSP = {0: {0}, 1: {0, 8}, 2: {0}}
PX, PY = 124.5, 87.5                         # Drehpunkt (Nachher: Texelmitte; vorher 124/87 - wir nehmen den Kreis grosszuegig)
yy, xx = np.mgrid[0:240, 0:320]
KREIS = (xx + .5 - PX) ** 2 + (yy + .5 - PY) ** 2 <= 13.5 ** 2
SCHEIBEN = np.zeros((240, 320), bool); SCHEIBEN[143:172, 108:256] = True      # vier Drehscheiben (Mitten y 160/160,5, R 13,9), ohne Band ab y 172
WORTE = [('A 100', 0, 4), ('B 50', 4, 7), ('C 30', 7, 10), ('80 rot', 10, 12), ('BRIDGE', 12, 18), ('SET 80', 18, 23), ('CAUTION', 23, 30), ('HIGH PRESSURE', 30, 42)]

ev = bilder.ereignisse(lauf, ('[panel2030]', '[msgtext]', 'GELOEST'))
t_offen = next(t for t, s in ev if 'offen:' in s)
t_geloest = next(t for t, s in ev if 'GELOEST: A' in s)
t40 = next(t for t, s in ev if 'id=40' in s)
fuell = [(t, s) for t, s in ev if 'umgefuellt' in s or 'Quelle' in s or 'Anzeige 80' in s]

def tinte_maske(c, k):
    g = schrift.glyphe(c)
    return np.isin(g, list(TRANSP[k]), invert=True)

erg = dict(lauf=lauf, inc=welche)
panel = []          # (t, logisch)
frage = []
ein = []
for i, t, b in bilder.bilder(lauf):
    if t < t40 - 0.5 or t > t_geloest + 1.0:
        continue
    l = bilder.logisch(b)
    gleich = float((np.abs(l.astype(np.int16) - G).max(-1) == 0).mean())
    if gleich > 0.6:
        panel.append((t, l.copy()))
        if len(panel) % 15 == 1:
            ein.append(bilder.einfarbig(b))
    elif t < t_offen:
        frage.append((t, l.copy()))
erg['panel_bilder'] = len(panel)
erg['einfarbig_3x3_min'] = min(ein) if ein else None
print('Panel-Bilder', len(panel), 'von', '%.2f..%.2f s' % (panel[0][0], panel[-1][0]), '3x3 einfarbig min %.4f' % min(ein))

# --- Beschriftung: Tintentexel gleich der erwarteten Farbe (Grund-Bild = gestempelte Schrift), Erkennung je Zelle
def schrift_pruefen(l):
    ok = tot = 0
    text = ''
    for c, x, y, k in GL:
        m = tinte_maske(c, k)
        d = np.abs(l[y:y + 8, x:x + 8].astype(np.int16) - G[y:y + 8, x:x + 8]).max(-1)
        tot += int(m.sum()); ok += int((d[m] == 0).sum())
        fremd = np.zeros((8, 8), bool)
        for c2, x2, y2, k2 in GL:
            if (c2, x2, y2) == (c, x, y) or abs(x2 - x) >= 8 or abs(y2 - y) >= 8:
                continue
            m2 = np.zeros((24, 24), bool); m2[8 + y2 - y:16 + y2 - y, 8 + x2 - x:16 + x2 - x] = tinte_maske(c2, k2)
            fremd |= m2[8:16, 8:16]
        z, f, abst = schrift.erkenne(l[y:y + 8, x:x + 8], fremd, TRANSP[k])
        text += z
    return ok, tot, text

# Zustaende: vor der Loesung ohne Text (letztes Bild vor der ersten Cursor-Bewegung), nach der Loesung (Anzeige 80 + 0,8 s)
t_cursor = next(t for t, s in ev if 'Steuerkreuz' in s)
t80 = next(t for t, s in ev if 'Anzeige 80' in s)
vor = min(panel, key=lambda p: abs(p[0] - (t_cursor - 0.5)))
nach = min(panel, key=lambda p: abs(p[0] - (t80 + 0.8)))
for name, (t, l) in (('vor_loesung', vor), ('nach_loesung', nach)):
    ok, tot, text = schrift_pruefen(l)
    erg[name] = dict(t=t, tinte_gleich=ok, tinte=tot, erkannt=text)
    np.save('p_%s_%s.npy' % (lauf, name), l)
    print('%s t=%.2f: Tinte %d/%d Texel = Spielschrift; erkannt "%s"' % (name, t, ok, tot, text))
    for w, a, bb in WORTE:
        print('   %-14s erkannt %-14s' % (w, text[a:bb]))

# --- ausserhalb der Laufzeit-Teile gleich dem Grund?
def statisch_abw(l, t):
    d = np.abs(l.astype(np.int16) - G).max(-1) > 0
    dyn = np.zeros_like(d)
    dyn |= KREIS
    for tank_x0, tank_x1 in ((50, 84), (88, 96), (136, 162), (180, 206)):
        pass
    return d
# --- Zeiger: Zwischentoene, Flackern, Spruenge
def zeiger(l):
    d = (np.abs(l.astype(np.int16) - G).max(-1) > 0) & KREIS
    px = l[d].astype(int)
    farben = {}
    for p in map(tuple, px):
        farben[p] = farben.get(p, 0) + 1
    ys, xs = np.nonzero(d)
    r = np.hypot(xs + .5 - PX, ys + .5 - PY)
    sel = (r > 5) & (np.abs(l[ys, xs].astype(int) - [16, 15, 15]).max(-1) <= 2)
    if sel.sum() < 2:
        return None, farben, d
    ang = math.degrees(math.atan2((xs[sel] + .5 - PX).mean(), -(ys[sel] + .5 - PY).mean()))
    return ang, farben, d

zw = []
prev = None
flacker = 0
seq = []
for t, l in panel:
    a, farben, m = zeiger(l)
    zone = l[KREIS].astype(int)
    seq.append((t, a, zone, l[SCHEIBEN].astype(int)))
    # Zwischentoene: Farben im Zeigerbereich, die weder Zeigerfarbe, Nabe noch Grund mit Schatten 87/256 sind
    gn = G[m].astype(int); lv = l[m].astype(int)
    zeig = (np.abs(lv - [16, 15, 15]).max(-1) <= 1) | (np.abs(lv - [96, 96, 96]).max(-1) <= 1)
    schatten = np.abs(lv - (gn * (256 - 87)) // 256).max(-1) <= 2
    zw.append(int((~zeig & ~schatten).sum()))
erg['zeiger_zwischentoene_max'] = max(zw)
erg['zeiger_zwischentoene_mittel'] = float(np.mean(zw))
# A-B-A-Muster im Zeigerkreis und in den Scheiben (Bild t-1 == t+1 != t)
aba_z = aba_s = 0
for k in range(1, len(seq) - 1):
    if np.array_equal(seq[k - 1][2], seq[k + 1][2]) and not np.array_equal(seq[k][2], seq[k - 1][2]):
        aba_z += 1
    if np.array_equal(seq[k - 1][3], seq[k + 1][3]) and not np.array_equal(seq[k][3], seq[k - 1][3]):
        aba_s += 1
scheiben_varianten = len({s[3].tobytes() for s in seq})
erg['aba_zeiger'] = aba_z
erg['aba_scheiben'] = aba_s
erg['scheiben_varianten'] = scheiben_varianten
# Winkelverlauf
w = [(t, a) for t, a, _, _ in seq if a is not None]
spr = [(w[k][0], w[k][1] - w[k - 1][1]) for k in range(1, len(w))]
erg['zeiger_winkel_start'] = w[0][1]
erg['zeiger_winkel_ende'] = w[-1][1]
erg['zeiger_groesster_schritt'] = max(abs(s) for _, s in spr)
# Richtungswechsel innerhalb einer Fahrt (Winkel muss je Fahrt monoton laufen)
vz = [np.sign(s) for _, s in spr if abs(s) > 0.01]
wechsel = sum(1 for k in range(1, len(vz)) if vz[k] != vz[k - 1])
erg['zeiger_richtungswechsel'] = wechsel
print('Zeiger: Zwischentoene max %d (Mittel %.1f); A-B-A Zeiger %d, Scheiben %d; Scheiben-Varianten %d; Winkel %.1f -> %.1f, groesster Schritt %.2f Grad, Richtungswechsel %d'
      % (max(zw), np.mean(zw), aba_z, aba_s, scheiben_varianten, w[0][1], w[-1][1], erg['zeiger_groesster_schritt'], wechsel))
json.dump(dict(w=w), open('p_%s_winkel.json' % lauf, 'w'))

# --- Frage 40: Zeilenbelegung im Fragebild mit Ja/Nein
def segmente(maske_spalten):
    seg = []; a = None
    for x, v in enumerate(maske_spalten):
        if v and a is None: a = x
        if not v and a is not None: seg.append((a, x - 1)); a = None
    if a is not None: seg.append((a, len(maske_spalten) - 1))
    return seg
ohne = min(frage, key=lambda q: abs(q[0] - (t40 - 0.4)))[1].astype(int)
mit_ja = []
for t, l in frage:
    hell = l.min(-1) > 150
    z2 = hell[192:208].any(0)
    if z2[170:200].sum() >= 6 and z2[236:262].sum() >= 4:
        mit_ja.append((t, l))
if mit_ja:
    t, l = mit_ja[len(mit_ja) // 2]
    hell = (l.min(-1) > 150) & (np.abs(l.astype(int) - ohne).max(-1) > 40)
    z1 = segmente(hell[176:192].any(0)); z2 = segmente(hell[192:208].any(0))
    erg['frage40'] = dict(t=t, bilder=len(mit_ja), zeile1=[z1[0][0], z1[-1][1]] if z1 else None, zeile2=z2)
    np.save('p_%s_frage40.npy' % lauf, l)
    print('Frage 40 mit Ja/Nein: %d Bilder; Zeile 1 x %s; Zeile 2 Segmente %s' % (len(mit_ja), (z1[0][0], z1[-1][1]) if z1 else None, z2))
json.dump(erg, open('p_%s_mass.json' % lauf, 'w'), indent=1, default=str)
