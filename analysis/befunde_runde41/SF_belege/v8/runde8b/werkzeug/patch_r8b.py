"""Patch schwanz_r8.py fuer die Nachbesserung Runde 8b (Pruefer pr8): Stachel, Zwischenringe, Pendel-UV, rosa Band, Keyframe-Anhebung."""
import os
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'schwanz_r8.py')
s = open(p, encoding='utf-8').read()


def ers(a, b):
    global s
    assert a in s, a[:90]
    s = s.replace(a, b)


# ---------------------------------------------------------------- Konstanten Stachel
a0 = s.index('DORN_ALT = 48.0')
a1 = s.index('ENDE_Y = 560.0')
a1 = s.index('\n', a1) + 1
s = s[:a0] + '''DORN_ALT = 48.0                       # Ende von Mesh 22 bis Runde 7e: Dorn 0,8 x r7 = 48 E. ueber dem Gelenk 7 (Stand 07b6703f, Ecke 22 y 589)
# Runde 8b (Pruefer pr8, PRUEFUNG.md (A)/(E)1: "Knolle in PV als Verdickung sichtbar ... ~18 px breit bei 150-190 px ab Wurzel", "sichtbarer Schwanz
# in PV bis y ~350", "die Nadel ist ein 1-2 px duenner Draht ... soll eine sichtbare Skorpionspitze sein"). Gemessen (n1_richtung.py): in PV ist
# bildabwaerts fast ueberall weltabwaerts (in 855-897 von 912 Keyframes), eine abgewinkelte Nadel waere also nicht bodensicherer als eine gerade;
# die gerade Nadel entlang der Gliedachse bildet der Code genau ab (sw_mesh_mass dorn_y), eine abgewinkelte nur als Radius um das Gelenk 7
# (mehr Anhebung). Deshalb: Knolle Radius 168 bei y 330 (PV ~17 px), Verjuengung, kegelige Nadel (Radius 62 -> 22) bis zur Spitze SPITZE_Y.
# Bodenfreiheit in den Keyframes stellt heben() her (S6 streng).
# (y, Radius) hinter Ring C, rund; Knolle D 336 E.
STACHEL = ((200.0, 150.0), (330.0, 168.0), (450.0, 160.0), (560.0, 130.0), (650.0, 95.0), (750.0, 62.0), (900.0, 40.0), (1050.0, 22.0))
STACHEL_C = 128.0                     # Radius Ring C von Glied 22 (Knolle steigt aus der Kerbe 0,85 x 115,8 = 98 an)
SPITZE_Y = 1200.0                     # Nadelspitze (PV: Glied-22-Achse 3,5 px je 100 E. ab Gelenk 7 y_bild 325,8 -> 348,8; Pruefer "bis y ~350")
ENDE_Y = 750.0                        # Ring 1 von Mesh 22 = Nadelansatz (Radius 62) -> riegel_v8.s4 "Ende <= 10 px"
ZWISCHEN = {1: (0.3733, 0.6267), 2: (0.31, 0.69), 3: (0.3733, 0.6267), 4: (0.3733, 0.6267), 5: (0.3733, 0.6267)}
                                      # Runde 8b: zwei Zwischenringe Z1/Z2 je Glied 17-21 (Glied 18 um M bei 0,5) - das Fleischfeld liegt dreimal
                                      # gependelt entlang des Glieds statt einmal gestreckt (Pruefer "Rot flaechig wie gemalt", 17 x 7 gestreckt)
''' + s[a1:]

# ---------------------------------------------------------------- Farbe
ers("""ZEILE_GRAU, ZEILE_FLEISCH, ZEILE_BAND = 487, 495, 494      # 487 = Schwanz-Oberseite seit V6; 495 = Original 482; 494 = Original 481
SEITE_FLEISCH, SEITE_BAND = 2, 1""", """ZEILE_GRAU, ZEILE_FLEISCH, ZEILE_BAND = 487, 495, 493      # 487 = Schwanz-Oberseite seit V6; 495 = Original 482; 493 = Original 480 (bytegleich)
SEITE_FLEISCH, SEITE_BAND = 2, 0
# Runde 8b (Pruefer pr8 (B)): Band hellrot/rosa statt fast weiss - Original-Zeile 480, Seite 0, Feld aus suche() (L* 40-56, C* 12-28, Farbton 5-35
# >= 60 %, Klasse des Pruefer-Werkzeugs b_bandfeld.py); nur die beiden mittleren Bauchfelder (j 10 / 0) der Fuge, die aeusseren bleiben Fleisch
# ("nicht als Ring ueber die ganze Breite wie Klebeband"). Fleisch: schraeges Feld (Parallelogramm) der Original-Zeile 482 (suche()), um den Bauch
# zweimal und entlang des Glieds je Feld einmal gependelt (Spiegelung an jeder Kante, keine Naht).
BAND_J = (10, 0)
PENDEL = {9: 0.0, 10: 1.0, 0: 0.0, 1: 1.0, 2: 0.0}   # Bauch-Ecke j -> Lage im Feld quer (0 / 1), Spiegelung je Flaeche""")
a0 = s.index("FARBEN = {'a':")
a1 = s.index("STANDARD = 'b'")
s = s[:a0] + """FARBEN = {'a': dict(fleisch='gedeckt', band=False, name='gedeckt, ohne Baender'),
          'b': dict(fleisch='gedeckt', band=True, name='gedeckt, rosa Baender')}
""" + s[a1:]
# ---------------------------------------------------------------- Ringe Z1/Z2
ers("""    if g == 2:
        th = winkel(grenze(g + 0.5))
        out.append(('M', ring(0.5 * L, rr(0.5 * L), quer(rr(0.5 * L)), None, th, eh, eq), th, np.full(S_, g + 0.5), False))
    if g < 6:""", """    if g in ZWISCHEN:
        z1, z2 = ZWISCHEN[g]
        for nm, t in (('Z1', z1), ('M', 0.5), ('Z2', z2)):
            if nm == 'M' and g != 2:
                continue
            th = winkel(grenze(g + t))
            out.append((nm, ring(t * L, rr(t * L), quer(rr(t * L)), None, th, eh, eq), th, np.full(S_, g + t), False))
    if g < 6:""")
ers("""        achse['spitze'] = len(V_); V_.append(np.array([0.0, y_spitze, 0.0]))""",
    """        achse['spitze'] = len(V_); V_.append(np.array([0.0, y_spitze, 0.0]))     # Nadelspitze SPITZE_Y""")
ers("""    y_spitze = L + DORN_ALT + NADEL""", """    y_spitze = SPITZE_Y""")
# ---------------------------------------------------------------- Klasse / UV
a0 = s.index('def klasse_von(')
a1 = s.index('def uv_eintrag(')
s = s[:a0] + '''def klasse_von(f, farbe, g):
    """Flaechenklasse 'grau' / 'bauch' / 'band' (Kopf der Datei; Runde 8b: Band nur in den mittleren Bauchfeldern BAND_J)."""
    art, e, (ra, rb), j = f
    if art == 'spitze' or j not in BAUCH_J:
        return 'grau'
    if g == 6:
        fuge = art == 'deckel_anf' or (art == 'mantel' and 'S' in (ra, rb))
    else:
        fuge = art.startswith('deckel') or (art == 'mantel' and bool({ra, rb} & {'S', 'E'}))
    return 'band' if farbe['band'] and fuge and j in BAND_J else 'bauch'


def fleisch_uv(F_, s_um, v_seite):
    """Schraeges Fleischfeld: v = v0 / v1 (v_seite 0 / 1), u = Anfang der Zeile (linear zwischen u_v0 und u_v1) + s_um x (Breite - 1)."""
    v = F_['v0'] + v_seite * (F_['v1'] - F_['v0'])
    u0 = F_['u_v0'] + (F_['u_v1'] - F_['u_v0']) * (v - F_['v0']) / (F_['v1'] - F_['v0'])
    return (u0 + s_um * (F_['breite'] - 1), v)


def uv_setzen(g, V_, F, ringe, info, farbe, felder, L):
    """-> Liste (Flaeche, UV-Liste, Klasse). Runde 8b: Fleisch und Band gependelt (je Flaeche ein Feld, gespiegelt an jeder Kante)."""
    th_e, u_e, seite_e = {}, {}, {}
    folge = info['folge']
    for nm, P, th, u, _k in ringe:
        p0 = info['pos'][nm]
        for k in range(S_):
            th_e[p0 + k] = float(np.degrees(th[k]))
            u_e[p0 + k] = float(u[k])
            seite_e[p0 + k] = folge.index(nm) % 2          # Pendel entlang des Glieds: Ringe abwechselnd Feldanfang / Feldende
    ring_k = {}
    for nm, _P, _t, _u, _k in ringe:
        for k in range(S_):
            ring_k[info['pos'][nm] + k] = k
    ys = V_[:, 1]
    y_lo, y_hi = float(min(ys)), float(max(ys))
    out = []
    feld_grau = felder['grau'][str(16 + g)]
    feld_f = felder['fleisch']
    feld_band = felder['band']
    tb_glied = lambda i: grenze(u_e.get(i, g + 0.5))
    for f in F:
        art, e, (ra, rb), j = f
        kl = klasse_von(f, farbe, g)
        if g == 6 and kl != 'grau' and all(V_[i][1] >= ENDE_Y - 1.0 for i in e):
            kl = 'grau'                                    # Nadel grau (Video: dunkle Spitze)
        uv_e = []
        rs = [q for q in e if q in th_e]
        for i in e:
            if i in th_e:
                th = th_e[i]
                tb = tb_glied(i)
            else:                                          # Achs-Ecke: Mitte der Ring-Ecken der Flaeche
                th = float(np.degrees(np.angle(np.mean([np.exp(1j * np.radians(th_e[q])) for q in rs]))))
                tb = tb_glied(rs[0])
            th = th % 360.0
            s_lang = float(np.clip((V_[i][1] - y_lo) / max(y_hi - y_lo, 1.0), 0.0, 1.0))
            if kl == 'grau':
                s_um = float(np.clip((th - tb) / max(360.0 - 2 * tb, 1.0), 0.0, 1.0))
                uv_e.append(_uv_feld(feld_grau, s_um, s_lang))
                continue
            if i in ring_k:
                s_um = PENDEL.get(ring_k[i], 0.5)
                vs = seite_e[i]
            else:
                s_um = float(np.mean([PENDEL.get(ring_k[q], 0.5) for q in rs]))
                vs = seite_e[rs[0]]
            if kl == 'bauch':
                uv_e.append(fleisch_uv(feld_f, s_um, vs))
            else:
                # Band: quer gependelt ueber die zwei mittleren Felder, laengs v0 am Gelenkring (S / E), v1 am vollen Querschnitt (C / D)
                nm = [n_ for n_ in (ra, rb) if info['pos'][n_] <= i < info['pos'][n_] + S_] if i in ring_k else [ra]
                am_gelenk = 1.0 if (nm and nm[0] in ('S', 'E')) else 0.0
                uv_e.append(_uv_feld(feld_band, s_um, 1.0 - am_gelenk))
        out.append((f, [(int(round(u)), int(round(v))) for u, v in uv_e], kl))
    return out


''' + s[a1:]
# ---------------------------------------------------------------- suche: Fleisch schraeg, Band Seite 0
a0 = s.index("    # gedeckt: Zeile 482, Seite 2")
a1 = s.index("    # Texel des Spiel-Assets in diesen Feldern = Original?")
s = s[:a0] + '''    # Fleisch (Runde 8b): schraeges Feld (Parallelogramm) in der Original-Nutzung der Zeile 482, Seite 2 - Zeilen v0..v1 der hellen Fleischpartie
    # unterhalb der satten (v0 >= FLEISCH_V_AB), Zeilenanfang linear von u_v0 nach u_v1, groesste Flaeche.
    lab = lab_zeile(482)
    nutz = A4.nutzung(None, 482)[:, 256:384]
    Lm = lab[to['pix'][:, 256:384]]
    best = None
    for v0 in range(FLEISCH_V_AB, FLEISCH_V_AB + 4):
        for v1 in range(v0 + 6, 126):
            for a in range(0, 30):
                for b in range(a, 40):
                    for w in range(40, 5, -1):
                        if all(0 <= int(round(a + (b - a) * (v - v0) / (v1 - v0))) and
                               nutz[v, int(round(a + (b - a) * (v - v0) / (v1 - v0))):int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w].all()
                               and int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w <= 128 for v in range(v0, v1 + 1)):
                            if best is None or w * (v1 - v0 + 1) > best[0]:
                                best = (w * (v1 - v0 + 1), v0, v1, a, b, w)
                            break
    _a, v0, v1, a, b, w = best
    sel = np.concatenate([Lm[v, int(round(a + (b - a) * (v - v0) / (v1 - v0))):int(round(a + (b - a) * (v - v0) / (v1 - v0))) + w] for v in range(v0, v1 + 1)])
    erg['fleisch'] = dict(v0=v0, v1=v1, u_v0=a, u_v1=b, breite=w)
    log('Fleisch Zeile 482 Seite 2: schraeges Feld v %d-%d, Zeilenanfang u %d -> %d, Breite %d (%d Texel), Median L* %.1f C* %.1f, L*-Streuung %.1f'
        % (v0, v1, a, b, w, _a, float(np.median(sel[:, 0])), float(np.median(np.hypot(sel[:, 1], sel[:, 2]))), float(np.std(sel[:, 0]))))
    # Band (Runde 8b): Original-Zeile 480 (Asset 493), Seite 0, groesstes Rechteck in der Original-Nutzung mit >= BAND_ROSA_ANTEIL hellrot/rosa Texeln
    lab = lab_zeile(480)
    nz = A4.nutzung(None, 480)[:, 0:128]
    Lm = lab[to['pix'][:, 0:128]]
    C = np.hypot(Lm[..., 1], Lm[..., 2]); hh = np.degrees(np.arctan2(Lm[..., 2], Lm[..., 1]))
    rosa = nz & (Lm[..., 0] >= 40) & (Lm[..., 0] <= 56) & (C >= 12) & (C <= 28) & (hh >= 5) & (hh <= 35)
    bb = None
    for v0 in range(0, 250):
        for v1 in range(v0 + 3, min(v0 + 30, 256)):
            for u0 in range(0, 125):
                for u1 in range(u0 + 3, min(u0 + 16, 128)):
                    if not nz[v0:v1 + 1, u0:u1 + 1].all():
                        break
                    if rosa[v0:v1 + 1, u0:u1 + 1].mean() >= BAND_ROSA_ANTEIL:
                        fl = (u1 - u0 + 1) * (v1 - v0 + 1)
                        if bb is None or fl > bb[0]:
                            bb = (fl, u0, u1, v0, v1)
    _f, u0, u1, v0, v1 = bb
    sel = Lm[v0:v1 + 1, u0:u1 + 1].reshape(-1, 3)
    log('Band Zeile 480 Seite 0: groesstes Rechteck mit >= %.0f %% hellrot/rosa (L* 40-56, C* 12-28, h 5-35) u %d-%d v %d-%d (%d x %d), Anteil %.0f %%, '
        'Median L* %.1f C* %.1f h %.0f' % (100 * BAND_ROSA_ANTEIL, u0, u1, v0, v1, u1 - u0 + 1, v1 - v0 + 1, 100 * rosa[v0:v1 + 1, u0:u1 + 1].mean(),
                                          float(np.median(sel[:, 0])), float(np.median(np.hypot(sel[:, 1], sel[:, 2]))),
                                          float(np.median(np.degrees(np.arctan2(sel[:, 2], sel[:, 1]))))))
    erg['band'] = [int(u0), int(u1), int(v0), int(v1)]
''' + s[a1:]
ers("""    gl = {k: bool(np.array_equal(t['pix'][r_[2]:r_[3] + 1, s * 128 + r_[0]:s * 128 + r_[1] + 1],
                                 to['pix'][r_[2]:r_[3] + 1, s * 128 + r_[0]:s * 128 + r_[1] + 1]))
          for k, r_, s in (('gedeckt', erg['gedeckt'], 2), ('band', erg['band'], 1), ('satt', FELD_SATT, 2))}""",
    """    ff = erg['fleisch']
    gl = {'band': bool(np.array_equal(t['pix'][v0:v1 + 1, u0:u1 + 1], to['pix'][v0:v1 + 1, u0:u1 + 1])),
          'fleisch': bool(np.array_equal(t['pix'][ff['v0']:ff['v1'] + 1, 256:384], to['pix'][ff['v0']:ff['v1'] + 1, 256:384])),
          'zeile_493_gleich_original_480': bool(np.array_equal(t['clut'][13] & 0x7FFF, to['clut'][0] & 0x7FFF))}""")
ers("""GED_L, GED_A, GED_C = 20.0, 15.0, 26.0""", """FLEISCH_V_AB = 114                    # Runde 8b: schraeges Fleischfeld ab Zeile 114 (n6b: ab 111 Median C* 24,4 / L* 23,7, ab 114 C* 24,2 / L* 25,7 =
                                      # naeher am Ton der Probe a L* 26,8 C* 20,3; die satte Partie v 93-112 ist Probe c, verworfen)
BAND_ROSA_ANTEIL = 0.6                # Runde 8b: Band-Feld mit >= 60 % hellrot/rosa Texeln (Pruefer b_bandfeld.py-Klasse)
GED_L, GED_A, GED_C = 20.0, 15.0, 26.0""")
open(p, 'w', encoding='utf-8').write(s)
print('ok')
