"""Patch Runde 8c Teil 4: schwanz_r8.py klasse_uv + Konstanten + Doku; schwanz_r8.json Felder C1 / RB; riegel_schwanz_r8.py R8d (3) Ring,
R8e Klassen aus UV-Feldern (Bauch und Ring liegen beide auf Zeile 493 = Original 480)."""
import os, json
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
S = None


def lade(n):
    global S
    S = open(os.path.join(W, n), encoding='utf-8').read()


def ers(alt, neu, n=1):
    global S
    assert S.count(alt) == n, (alt[:70], S.count(alt))
    S = S.replace(alt, neu)


# ---------------------------------------------------------------- schwanz_r8.py
lade('schwanz_r8.py')
ers("""ZEILE_GRAU, ZEILE_FLEISCH, ZEILE_BAND = 487, 495, 493      # 487 = Schwanz-Oberseite seit V6; 495 = Original 482; 493 = Original 480 (bytegleich)
SEITE_FLEISCH, SEITE_BAND = 2, 0""",
    """ZEILE_GRAU, ZEILE_FLEISCH, ZEILE_BAND = 487, 493, 493      # 487 = Schwanz-Oberseite seit V6; 493 = Original 480 (bytegleich); Runde 8c: Bauch und
SEITE_FLEISCH, SEITE_BAND = 0, 0                            # Ring beide auf 493 Seite 0 (Felder C1 / RB, schwanz_r8.json); bis Runde 8b Bauch 495 = Original 482
ZEILE_WURZEL = 495                                          # die 14 Wurzelflaechen von Mesh 16 (Stufe w/u) behalten Zeile 495 und ihre UV""")
ers("""def klasse_von(f, farbe, g):""",
    """def klasse_uv(uv, felder=None):
    \"\"\"Runde 8c: Flaechenklasse einer gebauten Flaeche aus Zeile, Seite und UV -> 'grau' / 'bauch' / 'band' / None (fuer die Riegel: Bauch und
    Ring liegen beide auf Zeile 493, getrennt durch ihre Felder in schwanz_r8.json).\"\"\"
    felder = felder or felder_laden()
    z, seite = (uv[2] >> 6) & 0x1FF, uv[5] & 0xF
    us, vs = uv[0::3], uv[1::3]
    if z == ZEILE_GRAU:
        return 'grau'
    fr = felder.get('ring')
    if fr and (z, seite) == (fr['zeile'], fr['seite']) and all(fr['u0'] <= u <= fr['u1'] for u in us) and all(fr['v0'] <= v <= fr['v1'] for v in vs):
        return 'band'
    ff = felder['fleisch']
    if (z, seite) == (int(ff.get('zeile', ZEILE_FLEISCH)), int(ff.get('seite', SEITE_FLEISCH))):
        ulo, uhi = min(ff['u_v0'], ff['u_v1']), max(ff['u_v0'], ff['u_v1']) + ff['breite'] - 1
        if all(ulo <= u <= uhi for u in us) and all(ff['v0'] <= v <= ff['v1'] for v in vs):
            return 'bauch'
    return None


def klasse_von(f, farbe, g):""")
ers("""  bauch = Bauchsektor: Original-Zeile 482 (Asset 495), schraeges Feld (Parallelogramm, suche(): v 114-123, Zeilenanfang u 1 -> 19, Breite 21, im Bau 20;""",
    """  Runde 8c (Koordinator 14.11., Nutzer waehlt Probe b: "wobei das Original Video der Schwanz noch etwas heller ist und nicht ganz so rot"):
  bauch = Original-Zeile 480 (Asset 493), Seite 0, Feld C1 u 54-65 v 48-57 (schwanz_r8.json; ausgewaehlt unter 18 Original-Feldern nach der
          Messung im Spielbild F255-F320, Lab-Abstand zur Video-Mitte des Pruefers gruenblut 5,1 statt 8,7; SF_belege/v8/runde8c); grosse
          gefleckte Felder ergaben je Bild sehr verschiedene Toene (nur ein Teil des Felds sichtbar), gespiegelte Abbildung ein Augenmuster;
  band  = Probe b: Ring umlaufend auf allen 11 Sektoren der Fugen (Mantel S-C / D-E und Deckel), Original-Zeile 480 (Asset 493), Seite 0,
          Feld RB u 93-100 v 10-17 mit Verlauf hell (u 100, am Gelenk) -> dunkel (u 93, am vollen Querschnitt), ohne Texel unter L* 18;
  Bis Runde 8b:
  bauch = Bauchsektor: Original-Zeile 482 (Asset 495), schraeges Feld (Parallelogramm, suche(): v 114-123, Zeilenanfang u 1 -> 19, Breite 21, im Bau 20;""")
ers("""  Proben (Koordinator: nur noch zwei, wenn sie sich sichtbar unterscheiden): a ohne Baender, b mit rosa Baendern (STANDARD).""",
    """  Proben (Koordinator: nur noch zwei, wenn sie sich sichtbar unterscheiden): a ohne Baender, b mit rosa Baendern (STANDARD; Runde 8c: Ringe).""")
open(os.path.join(W, 'schwanz_r8.py'), 'w', encoding='utf-8').write(S)

# ---------------------------------------------------------------- schwanz_r8.json
p = os.path.join(W, 'schwanz_r8.json')
j = json.load(open(p))
j['quelle'] = j['quelle'] + ('; Runde 8c: fleisch = Feld C1 (Original-Zeile 480 = Asset 493, Seite 0, u 54-65 v 48-57) und ring = Feld RB (Zeile 480, '
                             'Seite 0, u 93-100 v 10-17, hell bei u 100), ausgewaehlt nach Messung im Spielbild (Protokoll SF_belege/v8/runde8c/); band = '
                             'Feld der Runde 8b, nur noch fuer den Band-Modus ohne Ring')
j['felder']['fleisch'] = dict(v0=48, v1=57, u_v0=54, u_v1=54, breite=12, zeile=493, seite=0)
j['felder']['ring'] = dict(u0=93, u1=100, v0=10, v1=17, richt='u-', zeile=493, seite=0)
json.dump(j, open(p, 'w'), indent=1)

# ---------------------------------------------------------------- riegel_schwanz_r8.py
lade('riegel_schwanz_r8.py')
ers("""                  in den Gliedmitten daneben; (3) Farbprobe b / c: an allen 6 Gelenken Flaechen auf Zeile 494 (helles Band) und in PV sichtbare
                  Band-Bildpunkte an >= 5 Gelenken; Farbprobe a: ohne Band (Angabe).""",
    """                  in den Gliedmitten daneben; (3) Farbprobe b (Runde 8c, Ring): die Band-Flaechen (Klasse aus dem Ring-Feld, schwanz_r8.klasse_uv)
                  sind GENAU die Fugen-Flaechen aller 11 Sektoren (Ringe S/C bzw. D/E mit Deckel-Mitte, aus den Ring-Lagen lage()) an allen 6
                  Gelenken, und in PV sind Band-Bildpunkte an >= 5 Gelenken sichtbar; Farbprobe a: keine Band-Flaeche.""")
ers("""R8e Fleisch nur   (1) jede Flaeche auf Zeile 495 / 494 (ausser den 14 Wurzelflaechen von Mesh 16 im Rumpf, Stufe w/u) liegt mit allen Ring-Ecken""",
    """R8e Fleisch nur   (1) Runde 8c: jede Bauch-Flaeche (Klasse aus dem Fleisch-Feld, schwanz_r8.klasse_uv; Bauch und Ring liegen auf derselben
                  Zeile 493; eine Flaeche ohne Klasse ist falsch; ausser den 14 Wurzelflaechen von Mesh 16 im Rumpf, Stufe w/u) liegt mit allen Ring-Ecken""")
ers("""                  (3) Herkunft: Texel unter 495 / 494 = Texel, die Original-Flaechen der Original-Zeile 482 / 481 benutzen; Texel unter 487 = Texel""",
    """                  (3) Herkunft: Texel unter 493 / 494 / 495 = Texel, die Original-Flaechen der Original-Zeile 480 / 481 / 482 benutzen; unter 487 = Texel""")
ers("""    band = {}
    for b in range(16, 23):
        band[b] = sum(1 for uv in ms[b]['tuv'] + ms[b]['quv'] if ((uv[2] >> 6) & 0x1FF) == Q.ZEILE_BAND)
    A = pv['A']
    z = A['zeile'][:480]; bo = A['bone'][:480]
    band_pv = {b: int(((z == Q.ZEILE_BAND) & (bo == b)).sum()) for b in range(16, 23)}
    gel_sicht = sum(1 for j in range(1, 7) if band_pv[15 + j] + band_pv[16 + j] > 0)
    if Q.FARBEN[farbe]['band']:
        ok3 = all(band[15 + j] > 0 and band[16 + j] > 0 for j in range(1, 7)) and gel_sicht >= 5
    else:
        ok3 = True
    return dict(geometrie=geo, ok1=bool(ok1), band_flaechen=band, band_pv_px=band_pv, gelenke_mit_band_pv=gel_sicht, band=Q.FARBEN[farbe]['band'],
                ok3=bool(ok3))""",
    """    band, fuge_n, abweich = {}, {}, {}
    kl = klassen(ms)
    for b in range(16, 23):
        fs = fugen_flaechen(b, ms[b])
        bs = {k for k, c in kl[b].items() if c == 'band'}
        band[b] = len(bs); fuge_n[b] = len(fs)
        abweich[b] = sorted(fs ^ bs)[:6]
    A = pv['A']
    bo = A['bone'][:480]; fk = A['flaeche'][:480]
    band_pv = {b: 0 for b in range(16, 23)}
    ys, xs = np.nonzero((bo >= 16) & (bo <= 22))
    for y, x in zip(ys, xs):
        b = int(bo[y, x])
        if kl[b].get(str(fk[y, x])) == 'band':
            band_pv[b] += 1
    gel_sicht = sum(1 for j in range(1, 7) if band_pv[15 + j] + band_pv[16 + j] > 0)
    if Q.FARBEN[farbe]['band']:
        ok3 = all(not abweich[b] and band[b] > 0 for b in range(16, 23)) and gel_sicht >= 5
    else:
        ok3 = all(band[b] == 0 for b in range(16, 23))
    return dict(geometrie=geo, ok1=bool(ok1), band_flaechen=band, fugen_flaechen=fuge_n, band_ungleich_fuge=abweich, band_pv_px=band_pv,
                gelenke_mit_band_pv=gel_sicht, band=Q.FARBEN[farbe]['band'], ok3=bool(ok3))


def klassen(ms):
    \"\"\"Runde 8c: (Knochen) -> {'d<i>' / 'v<i>': Klasse} aus Zeile, Seite und UV (schwanz_r8.klasse_uv; Schluessel wie f_rast 'flaeche').\"\"\"
    out = {}
    for b in range(16, 23):
        out[b] = {}
        for key, U in (('d', ms[b]['tuv']), ('v', ms[b]['quv'])):
            for i, uv in enumerate(U):
                out[b]['%s%d' % (key, i)] = Q.klasse_uv(uv)
    return out


def fugen_flaechen(b, m):
    \"\"\"Runde 8c: Fugen-Flaechen eines Glieds = Flaechen, deren Ecken alle in (Ring S + Ring C + Anfangs-Mitte) bzw. (Ring D + Ring E + End-Mitte)
    liegen (KERBE_LAENGE x L um die Gelenke; Ring-Lagen aus lage(), Achs-Ecken je Mesh wie schwanz_r8.mesh_bauen). Mesh 22 nur am Anfang
    (Ring E dort = y 760, kein Gelenk), Mesh 16 nur am Ende.\"\"\"
    l = lage(b)
    r = lambda nm: set(range(l[nm], l[nm] + Q.S_))
    if b == 16:
        mengen = [r('D') | r('E') | {33}]
    elif b == 22:
        mengen = [r('S') | r('C') | {23}]
    elif b == 18:
        mengen = [r('S') | r('C') | {34}, r('D') | r('E') | {33}]
    else:
        mengen = [r('S') | r('C') | {23}, r('D') | r('E') | {22}]
    out = set()
    for key, F in (('d', m['tf']), ('v', m['qf'])):
        for i, f in enumerate(F):
            e = set(f[1::2])
            if any(e <= mg for mg in mengen):
                out.add('%s%d' % (key, i))
    return out""")
ers("""    falsch, n495 = [], 0""", """    falsch, n495 = [], 0
    kl = klassen(ms)""")
ers("""                ring = [k for k in e if k in th]
                if z in (Q.ZEILE_FLEISCH, Q.ZEILE_BAND):
                    n495 += z == Q.ZEILE_FLEISCH
                    if any(th[k] > gr[k] + TOL_GRAD for k in ring):
                        falsch.append('%d:%s%d' % (b, key, i))
                elif z == Q.ZEILE_GRAU:""",
    """                ring = [k for k in e if k in th]
                c = kl[b]['%s%d' % (key, i)]
                if c is None:
                    falsch.append('%d:%s%d ohne Klasse' % (b, key, i))     # Runde 8c: Zeile/UV in keinem Feld
                elif c == 'band':
                    continue                                   # Ring umlaufend (R8d (3) prueft, dass er genau auf den Fugen liegt)
                elif c == 'bauch':
                    n495 += 1
                    if any(th[k] > gr[k] + TOL_GRAD for k in ring):
                        falsch.append('%d:%s%d' % (b, key, i))
                elif z == Q.ZEILE_GRAU:""")
ers("""    zz = A['zeile'][:480][ys, xs]
    fl = zz == Q.ZEILE_FLEISCH""",
    """    fk = A['flaeche'][:480][ys, xs]
    fl = np.array([kl[int(b_)].get(str(k_)) == 'bauch' for b_, k_ in zip(bo, fk)], bool)""")
ers("""    nutz = {Q.ZEILE_FLEISCH: A4.nutzung(None, 482), Q.ZEILE_BAND: A4.nutzung(None, 480 + (Q.ZEILE_BAND - 493))}   # 493/494/495 = Original 480/481/482""",
    """    nutz = {z_: A4.nutzung(None, z_ - 13) for z_ in (493, 494, 495)}                  # 493/494/495 = Original 480/481/482 (bytegleich)""")
open(os.path.join(W, 'riegel_schwanz_r8.py'), 'w', encoding='utf-8').write(S)
print('ok')
