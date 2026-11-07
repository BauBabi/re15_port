p = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/generator/kf_g4.py'
t = open(p, encoding='utf-8').read()
orig = t


def rep(old, new, n=1):
    global t
    assert t.count(old) == n, ('Anker', t.count(old), old[:80])
    t = t.replace(old, new)


# ---- Docstring
rep("""Aufruf: python kf_g4.py [--aus <Ordner>]""", """Runde 4 (weniger Rot; Nutzer 2026-11-07 ~17:10, woertlich, bindend: "bist du noch an der Verbesserung von k5c ran, und entfernst die
übertriebenen roten feuerrillen wieder, und stellst es wieder nach vorlage um?"; Ziele MESSBLATT_FARBE_V3_NACHTRAG_ROT.md 5, Riegel RV1-RV4):
- Zeilen 0-2 bleiben G4 bytegleich (H2). Jede Koerperflaeche liest eine abgeleitete Zeile ihrer Gruppe (Gruppe x Seite, R4_GRUPPEN).
- Haut (G4-Eintraege ohne Rot-Gewicht): L* bleibt, Farbton der Gruppe, Buntheit = C x s(L) x G4-Hautbuntheit / C75 (Flecken aus der G4-Malerei).
- Adern (G4-Rotklasse, Rampe ROT_C0..ROT_C1): nie heller als q x Hautmedian, nie bunter als kappa x mittlere Haut-Buntheit (15-Bit-Farben).
  Die Feuerrillen sind damit entfernt; die Aderzeichnung bleibt als dunkle Struktur.
- Augen lesen G4-Zeile 0 (F10, dunkel), Maulinneres F2 bleibt auf den G4-Zeilen, Sonderzeilen 6-8 (Leuchtstellen) wie Runde 3.
- Baufehler A (Zahlen-Pruefer V3 r3): die Rand-Texel des F1-Flecks werden mit umgefaerbt (kein rohes G4-Blutrot mehr).

Aufruf: python kf_g4.py [--aus <Ordner>]""")

# ---- Konstanten Runde 4
rep("""# ================================================================ Laden
def g4_roh():""", '''# ================================================================ Runde 4: weniger Rot (Versuch 3 Runde 4)
R4_NUTZER = ('bist du noch an der Verbesserung von k5c ran, und entfernst die übertriebenen roten feuerrillen wieder, und stellst es wieder '
             'nach vorlage um?')     # Nutzer 2026-11-07 ~17:10, woertlich, bindend
# Gruppen: je Gruppe und Seite eine abgeleitete CLUT-Zeile. Begruendung je Teilung (Bauer-Lauf farbe_lesung/V3R4_bau/, Dossier V3R4.2):
#  - Rumpf Ruecken / Rumpf Flanke: mit EINER Rumpfzeile lag das Bild-1-Rot ganz auf dem Ruecken (Rumpf-Pixel 46 % / 35 % rot) und 0 % auf der
#    Flanke (such4.diag); die Flanke liegt im Bild-1-Licht tiefer und braucht mehr Buntheit.
#  - Brust (Mesh 10 Ruecken + Flanke) / Hals Bauch (Hals + Mesh 10 Brust/Bauch) / Kopf: Bild-2-Farbton-Fenster (Nachtrag 5) Ruecken -12,9..2,3,
#    Brust/Bauch -0,1..15,9, Hals 1,7..20,3, Kopf 13,1..24,7. Mit Kopf + Hals + Bauch in einer Zeile blieb nur ein Fenster von rund 1 Grad
#    (scan: Gruppenfarbton -24 / -18 / -12 -> Kopf 12,7 / 18,4 / 22,0, Brust/Bauch 14,2 / 18,8 / 22,3, Hals 19,1 / 22,1 / 24,5 Grad): Der Kopf
#    erscheint in der Kette weniger orange als Hals und Bauch, in der Vorlage ist es umgekehrt. Mit Ruecken + Bauch in einer Zeile kippte
#    Bild 2 zwischen "Ruecken zu wenig Rot" und "Bauch-Rotflaeche zu gross" (3 Saaten: C 39 -> Ruecken GRUEN, Bauch-Flaeche 19,6 > 17,81).
#  - Vorderbein r B1 (Flaechen mit Runde-3-Bild-1-Zeile) / Vorderbeine (G4-Zeile): Bild 1 Vorderglied Rot <= 7,1 %, Bild 2 Vorderbeine 56,4-74,5 %
#    bei 34,5..38,8 Grad (Nachtrag 5).
#  Zeilenzahl 17 (0-2 G4, 6-8 Sonder, 11 Gruppenzeilen) = 4352 Texel hohe Port-Textur (render_pc.c: je Zeile eine Kopie); Windows-Port
#  Direct3D 11 (Grenze 16384). Offen fuer ein Spiel-Asset auf GLES2-Geraeten mit 4096 (Dossier V3R4.5).
R4_GRUPPEN = ('Schwanz', 'Hinterbeine', 'Rumpf Ruecken', 'Rumpf Flanke', 'Vorderbein r B1', 'Brust', 'Hals Bauch', 'Kopf', 'Vorderbeine')
R4_ZEILEN_FREI = (3, 4, 5) + tuple(range(9, 30))   # 3-5 (Runde-3-Bild-1-Zeilen entfallen), 6-8 Sonderzeilen bleiben, danach 9 ff.
R4_N_GRENZE = 0.5          # Region je Flaeche wie MESSBLATT_FARBE_V3_NACHTRAG_ROT.md 1.2 (basis.N_GRENZE: Ruecken ny < -0,5, Bauch ny > 0,5)
# Helle Nicht-Ader-Eintraege behalten ihre G4-Farbe (Horn, Krallenspitzen; Sicht-Pruefer V3 r3: "khakifarbene Spitze ... wie die G4-Krallen").
# Grenze = Mitte der Luecke in der L*-Verteilung der genutzten Nicht-Ader-Eintraege: kein Eintrag zwischen L* 45,3 und 59,5 (gemessen,
# Dossier V3R4.2); darueber nur Farbton 71-118 Grad (khaki-gelb).
R4_L_HELL = 52.4
# Parameter je Gruppe in v3_daten/r4_par.json (eine Quelle fuer Bau und Riegel; Herkunft je Wert dort unter "quelle"):
#  h = Farbton der Gruppe (Grad, Texel), C = Haut-Buntheit am Hautmedian, beta = Buntheit dunkler Haut (s(L) = 1 + beta (Lm - L) / Lm,
#  begrenzt auf 0..R4_SMAX), q = Aderhelligkeit <= q x Hautmedian, kappa = Ader-Buntheit <= kappa x mittlere Haut-Buntheit.
R4_PAR = None
R4_PAR_DATEI = os.path.join(HIER, 'v3_daten', 'r4_par.json')
# s(L) begrenzt auf 0..3: gemessen ohne Einfluss auf das Ergebnis (scan smax 3 / 5, beta 5: Hals dL -1,7 / -1,7, Kopf dL 0,2 / 0,3).
R4_SMAX = 3.0


def r4_par():
    global R4_PAR
    if R4_PAR is None:
        R4_PAR = json.load(open(R4_PAR_DATEI, encoding='utf-8'))['par']
    return R4_PAR


def r4_flaechen_normalen(mo):
    """Je Mesh: lokale Normale je Flaeche (tf, dann qf), Mittel der Eckennormalen (wie V3R4_skripte/basis.flaechen_normalen)."""
    out = []
    for m in mo.meshes:
        nn = []
        for norms, faces in ((m.tn, m.tf), (m.qn, m.qf)):
            if not faces:
                continue
            N = np.array([x[:3] for x in norms], float) / 4096.0
            for g in faces:
                v = N[list(g[0::2])].mean(0)
                nn.append(v / max(np.linalg.norm(v), 1e-9))
        out.append(np.array(nn) if nn else np.zeros((0, 3)))
    return out


def r4_region(b, nx, ny):
    """K5c-Region einer Flaeche (wie V3R4_skripte/basis.region_k5c; Nachtrag 1.2)."""
    if b in (0, 7, 10):
        if ny < -R4_N_GRENZE:
            return 'Ruecken'
        if ny > R4_N_GRENZE or (b == 10 and nx > R4_N_GRENZE):
            return 'Brust/Bauch'
        return 'Flanke'
    if 1 <= b <= 6:
        return 'Schwanz'
    if b in (8, 9, 24, 25, 26, 27):
        return 'Hinterbeine'
    if 11 <= b <= 20 or 28 <= b <= 31:
        return 'Vorderbeine'
    if b == 21:
        return 'Hals'
    if b in (22, 23):
        return 'Kopf'
    return None


def r4_gruppe(key, familie, k5, fn):
    """Gruppe einer Koerperflaeche. familie = 'b1' / 'g4' (Zeilenfamilie der Runde 3: b1 = Flaeche mit Bild-1-Aderrot)."""
    mi, teil, fi = key
    nr = fi if teil == 't' else len(k5.meshes[mi].tf) + fi
    r = r4_region(mi, fn[mi][nr][0], fn[mi][nr][1])
    if r in ('Schwanz', 'Hinterbeine', 'Kopf'):
        return r
    if r == 'Vorderbeine':
        return 'Vorderbein r B1' if familie == 'b1' else 'Vorderbeine'
    if r == 'Hals':
        return 'Hals Bauch'
    if mi in (0, 7):
        return 'Rumpf Ruecken' if r == 'Ruecken' else 'Rumpf Flanke'
    if mi == 10:
        return 'Hals Bauch' if r == 'Brust/Bauch' else 'Brust'
    raise SystemExit('Runde 4: Flaeche %s ohne Gruppe (Region %s)' % (key, r))


def r4_eintraege(zeile):
    """Lab, C*, Farbton und Rot-Gewicht w je Eintrag (w wie zeile_b1: Rampe ROT_C0..ROT_C1 im Farbtonfenster ROT_H0..ROT_H1 = Ader)."""
    L = lab(wort_rgb(zeile))
    C = np.hypot(L[:, 1], L[:, 2])
    h = np.degrees(np.arctan2(L[:, 2], L[:, 1]))
    w = np.clip((C - ROT_C0) / (ROT_C1 - ROT_C0), 0, 1) * ((h > ROT_H0) & (h < ROT_H1))
    return L, C, h, w


def r4_statistik(zeile, cnt):
    """Je Gruppe und Seite (texelgewichtet, cnt = Texel je Index): Hautmedian L* (Haut = w 0) und C75 der Haut-Buntheit (Haut unter R4_L_HELL)."""
    L, C, h, w = r4_eintraege(zeile)
    cnt = np.asarray(cnt, float)
    haut = (w == 0) & (cnt > 0) & (np.array(zeile) != 0)
    haut_d = haut & (L[:, 0] < R4_L_HELL)

    def wq(x, sel, q):
        o = np.argsort(x[sel])
        cs = np.cumsum(cnt[sel][o])
        return float(x[sel][o][np.searchsorted(cs, q * cs[-1])])
    return dict(cnt=cnt, Lm=wq(L[:, 0], haut, 0.5), C75=wq(C, haut_d, 0.75))


def r4_zeile(g4zeile, par, st):
    """Abgeleitete Zeile einer Gruppe aus der G4-Zeile ihrer Seite (keine Neuquantisierung: je Eintrag eine neue Farbe, Index bleibt).
    Haut (w = 0): L* bleibt, Farbton par h, Buntheit C x s(L) x C*(G4) / C75 (C75 ist nur die Normierung von C).
    Ader (w > 0): L* = min(L*(G4), q x Hautmedian), Buntheit <= kappa x mittlere Haut-Buntheit der Gruppe (texelgewichtet, an den 15-Bit-Farben
    gemessen). Nutzer 2026-11-07: Adern "nie heller und nie roter als die umgebende Haut" (Auftrag V3 R4).
    Helle Nicht-Ader-Eintraege (L* >= R4_L_HELL) und 0x0000 bleiben."""
    L, C, h, w = r4_eintraege(g4zeile)
    Lm = st['Lm']
    q, kap = par['q'], par['kappa']
    cnt = st['cnt']
    z0 = np.array(g4zeile) != 0
    behalten = (w == 0) & (L[:, 0] >= R4_L_HELL)

    def s(Lx):
        return np.clip(1 + par['beta'] * (Lm - Lx) / Lm, 0, R4_SMAX)
    hr = math.radians(par['h'])
    Ls = L[:, 0]
    Cs = par['C'] * s(Ls) * (C / st['C75'])
    rgb_s = rgb_aus_lab(np.stack([Ls, Cs * math.cos(hr), Cs * math.sin(hr)], 1))
    Lab_s = lab(wort_rgb([wort(x) for x in rgb_s]))
    haut = (w == 0) & (cnt > 0) & ~behalten & z0
    C_haut = float((np.hypot(Lab_s[haut, 1], Lab_s[haut, 2]) * cnt[haut]).sum() / max(cnt[haut].sum(), 1))
    Lv = np.minimum(L[:, 0], q * Lm)
    Cv = np.minimum(par['C'] * s(Lv), kap * C_haut)
    Ln = (1 - w) * Ls + w * Lv
    Cn = (1 - w) * Cs + w * Cv
    ad = w > 0
    Ln[ad] = np.minimum(Ln[ad], q * Lm)                 # Deckel nach der Mischung: auch Eintraege im Rampenbereich sind Ader
    Cn[ad] = np.minimum(Cn[ad], kap * C_haut)
    rgb = rgb_aus_lab(np.stack([Ln, Cn * math.cos(hr), Cn * math.sin(hr)], 1))
    neu = list(g4zeile)
    for i in range(256):
        if g4zeile[i] == 0 or behalten[i]:
            continue
        wn = wort(rgb[i]) | (g4zeile[i] & 0x8000)
        neu[i] = wn if wn != 0 else 0x8000          # 0x0000 waere transparent (re15_tim_texel_argb); Schwarz wie G4 als 0x8000
    return neu, dict(Lm=round(Lm, 2), C75=round(st['C75'], 2), C_haut=round(C_haut, 2), adern=int(ad[z0 & (cnt > 0)].sum()),
                     haut=int(haut.sum()), behalten=int((behalten & (cnt > 0)).sum()))


# ================================================================ Laden
def g4_roh():''')

# ---- bau(): B1-Zeilen entfallen
rep("""    cl = [list(cl_g4[0]), list(cl_g4[1]), list(cl_g4[2])]       # Zeilen 0-2 bytegleich
    b1 = [zeile_b1(cl_g4[p], B1_C_JE_SEITE[p]) for p in range(3)]
    cl += [b1[p][0] for p in range(3)]                          # Zeilen 3-5 Bild-1-Aderrot
    cl += [None, None, None]                                    # Zeilen 6-8 Sonderfelder (unten)
    bericht = dict(g4=g4q, k5c_emd_sha256=K5C_EMD_SHA, zeilen={}, teile={}, sonder={})
    bericht['zeilen']['b1_umgefaerbt'] = {str(3 + p): len(b1[p][1]) for p in range(3)}""",
"""    cl = [list(cl_g4[0]), list(cl_g4[1]), list(cl_g4[2])]       # Zeilen 0-2 bytegleich
    cl += [None] * 6                                            # Runde 4: 3-5 Gruppenzeilen (unten), 6-8 Sonderfelder (unten)
    bericht = dict(g4=g4q, k5c_emd_sha256=K5C_EMD_SHA, zeilen={}, teile={}, sonder={})""")

rep("""        zeile = (Z_B1 if z == 'b1' else Z_G4)[seite]
        fertig[k] = (q, seite, zeile, nm)
        quelle_feld[k] = '%s:%d/%d' % (G.feld(*herk) or '-', herk[0], herk[1])
""", """        zeile = (Z_B1 if z == 'b1' else Z_G4)[seite]
        fertig[k] = (q, seite, zeile, nm)
        quelle_feld[k] = '%s:%d/%d' % (G.feld(*herk) or '-', herk[0], herk[1])
        familie[k] = z if nm != 'Maulinneres F2' else None
""")
rep("""    # ---- Zeilenfamilie je Flaeche
    fertig = {}
    quelle_feld = {}""", """    # ---- Zeilenfamilie je Flaeche (Runde 3); Runde 4 ersetzt die Zeile unten durch die Gruppenzeile
    fertig = {}
    quelle_feld = {}
    familie = {}""")

rep("""        fertig[k] = (np.round(q).astype(int), 0, Z_B1[0], 'Augen F10')""",
    """        fertig[k] = (np.round(q).astype(int), 0, Z_G4[0], 'Augen F10')   # Runde 4: G4-Zeile 0 (F10 ohne Rot-Eintraege; Zeile 3 ist Gruppenzeile)""")

rep("""    for x, y in gelesen:
        if hart[y, x] == ART_F1 and kern[y, x]:
            fleisch_texel[int(px[y, x])] += 1
    eintr = [i for i in fleisch_texel if i not in haut_eintraege and cl_g4[2][i] != 0]""",
"""    for x, y in gelesen:
        if hart[y, x] == ART_F1 and kern[y, x]:
            fleisch_texel[int(px[y, x])] += 1
    # Runde 4, Baufehler A (Zahlen-Pruefer V3 r3: 12 von 162 Fleck-Texeln mit rohem G4-Blutrot, z. B. (173,16,16)): umgefaerbt werden alle
    # Fleisch-Eintraege der gelesenen Fleck-Texel (Kern UND Rand); das Gewicht des Ziels bleibt auf dem Kern (Runde 3, r3/schnell.py).
    rand_eintraege = sorted({int(px[y, x]) for x, y in gelesen if hart[y, x] == ART_F1} - set(fleisch_texel))
    eintr = [i for i in list(fleisch_texel) + rand_eintraege if i not in haut_eintraege and cl_g4[2][i] != 0]""")
rep("""                                   umgefaerbt=len(eintr), zeile=info6)""",
    """                                   umgefaerbt=len(eintr), rand_eintraege_r4=len(rand_eintraege), zeile=info6)""")

rep("""    # ---- Vollstaendigkeit
    fehlt = [k for k in alle_flaechen(k5) if k not in fertig]""", """    # ---- Runde 4: Gruppenzeilen (R4_GRUPPEN x Seite) aus den G4-Zeilen, jede Koerperflaeche auf ihre Gruppenzeile
    par4 = r4_par()
    fn = r4_flaechen_normalen(k5)
    gruppe = {}
    for k, fam in familie.items():
        if fam is None or k not in fertig or fertig[k][2] not in Z_G4 + Z_B1:
            continue
        gruppe[k] = r4_gruppe(k, fam, k5, fn)
    paare = sorted({(g, fertig[k][1]) for k, g in gruppe.items()}, key=lambda p: (R4_GRUPPEN.index(p[0]), p[1]))
    if len(paare) > len(R4_ZEILEN_FREI):
        raise SystemExit('Runde 4: %d Gruppenzeilen, frei %d' % (len(paare), len(R4_ZEILEN_FREI)))
    zplan = {p: R4_ZEILEN_FREI[i] for i, p in enumerate(paare)}
    zaehl = {p: np.zeros(256) for p in paare}
    for k, g in gruppe.items():
        q, seite, _z, nm = fertig[k]
        xy = texel_set(q)
        if len(xy):
            np.add.at(zaehl[(g, seite)], px[xy[:, 1], xy[:, 0]], 1)
    while len(cl) <= max(zplan.values()):
        cl.append(None)
    bericht['zeilen']['r4'] = {}
    for (g, seite), z in zplan.items():
        st4 = r4_statistik(cl_g4[seite], zaehl[(g, seite)])
        cl[z], info = r4_zeile(cl_g4[seite], par4[g], st4)
        bericht['zeilen']['r4'][str(z)] = dict(gruppe=g, seite=seite, basis=seite, par=par4[g], **info)
    for z in range(len(cl)):
        if cl[z] is None:                       # ungenutzte Luecke (nur bei weniger als 3 Gruppenzeilen): G4-Zeile 0
            cl[z] = list(cl_g4[0])
    for k, g in gruppe.items():
        q, seite, _z, nm = fertig[k]
        fertig[k] = (q, seite, zplan[(g, seite)], nm)
    bericht['zeilen']['basis'] = {str(z): seite for (g, seite), z in zplan.items()}
    bericht['zeilen']['basis'].update({str(Z_F1): 2, str(Z_BAND): 0, str(Z_ZAHN): 0})
    bericht['zeilen']['gruppe'] = {str(z): g for (g, seite), z in zplan.items()}
    bericht['r4'] = dict(nutzer=R4_NUTZER, par_datei=os.path.relpath(R4_PAR_DATEI, K5).replace(os.sep, '/'), L_HELL=R4_L_HELL, SMAX=R4_SMAX,
                         flaechen_je_gruppe={g: sum(1 for x in gruppe.values() if x == g) for g in R4_GRUPPEN},
                         gruppe_je_flaeche={'%d|%s|%d' % k: g for k, g in gruppe.items()})

    # ---- Vollstaendigkeit
    fehlt = [k for k in alle_flaechen(k5) if k not in fertig]""")

rep("""    bericht['konstanten'] = dict(G4_KIND=hex(G4_KIND), B1_ROT_LAB=B1_ROT_LAB""", """    bericht['konstanten'] = dict(R4_PAR=r4_par(), R4_L_HELL=R4_L_HELL, R4_SMAX=R4_SMAX, R4_N_GRENZE=R4_N_GRENZE,
                                 G4_KIND=hex(G4_KIND), B1_ROT_LAB=B1_ROT_LAB""")
rep("""    bericht['basis'] = 'K5c (varianten/K5/K5c): Geometrie, Knochen, Clips unveraendert; UV/CLUT/TPage je Flaeche und FORM.TIM neu (Versuch 3, kf_g4.py)'""",
    """    bericht['basis'] = ('K5c (varianten/K5/K5c): Geometrie, Knochen, Clips unveraendert; UV/CLUT/TPage je Flaeche und FORM.TIM neu '
                        '(Versuch 3, kf_g4.py; Runde 4: Gruppenzeilen, weniger Rot)')""")
assert t != orig
open(p, 'w', encoding='utf-8').write(t)
print('ok')
