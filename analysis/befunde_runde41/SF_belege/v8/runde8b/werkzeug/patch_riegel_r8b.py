"""Patch der Riegel fuer Runde 8b (Pruefer pr8, Koordinator): riegel_schwanz_r8.py (lage, R8b/R8c, R8e, R8f, R8g) und riegel_v8.py (S6 streng)."""
import os, json
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel_v8/re15_port/tools/r41_sichel/v8'
p = os.path.join(W, 'riegel_schwanz_r8.py')
s = open(p, encoding='utf-8').read()


def ers(a, b):
    global s
    assert a in s, a[:90]
    s = s.replace(a, b)


ers('''def lage(b):
    """Ring-Startindizes je Mesh nach dem Bau (schwanz_r8.mesh_bauen)."""
    if b == 16:
        return dict(D=22, E=35)
    if b == 18:
        return dict(C=0, M=11, D=22, S=35, E=46)
    if b == 22:
        return dict(C=0, S=24)
    return dict(C=0, D=11, S=24, E=35)''', '''def lage(b):
    """Ring-Startindizes je Mesh nach dem Bau (schwanz_r8.mesh_bauen; Runde 8b mit Zwischenringen Z1/Z2, gemessen info['pos'])."""
    if b == 16:
        return dict(D=22, E=35)
    if b == 18:
        return dict(C=0, M=11, D=22, S=35, Z1=46, Z2=57, E=68)
    if b == 22:
        return dict(C=0, E=11, S=24)
    return dict(C=0, D=11, S=24, Z1=35, Z2=46, E=57)''')
# ---------------------------------------------------------------- R8b / R8c neu
a0 = s.index('def r8b_r8c(blob, bezug, D):')
a1 = s.index('# ------------------------------------------------------------------ Fugen (Geometrie, Baender), Fleisch, Herkunft')
s = s[:a0] + '''def mittellinie(m):
    """Laengster Pfad auf dem Skelett der groessten Zusammenhangsflaeche der Maske m (Verfahren des Pruefers pr8 a_mass.mittellinie)."""
    import heapq
    from scipy import ndimage as ndi
    from skimage.morphology import skeletonize
    m = ndi.binary_opening(m, iterations=1) if m.sum() > 200 else m
    lab, n = ndi.label(m)
    if n == 0:
        return None
    m = lab == (np.argmax(np.bincount(lab.ravel())[1:]) + 1)
    ys, xs = np.nonzero(skeletonize(m))
    idx = {(y, x): i for i, (y, x) in enumerate(zip(ys, xs))}
    nb = [[] for _ in ys]
    for i, (y, x) in enumerate(zip(ys, xs)):
        for dy in (-1, 0, 1):
            for dx in (-1, 0, 1):
                if (dy or dx) and (y + dy, x + dx) in idx:
                    nb[i].append((idx[(y + dy, x + dx)], (dy * dy + dx * dx) ** 0.5))

    def weit(s0):
        dd_, pr = {s0: 0.0}, {s0: None}
        q = [(0.0, s0)]
        while q:
            dd, u = heapq.heappop(q)
            if dd > dd_[u]:
                continue
            for v, w in nb[u]:
                if dd + w < dd_.get(v, 1e18):
                    dd_[v] = dd + w; pr[v] = u; heapq.heappush(q, (dd + w, v))
        e = max(dd_, key=dd_.get)
        return e, pr
    a, _ = weit(0)
    b, pr = weit(a)
    pfad, u = [], b
    while u is not None:
        pfad.append((ys[u], xs[u])); u = pr[u]
    return dict(pfad=np.array(pfad), dt=ndi.distance_transform_edt(m), maske=m)


def profil_abstand(m, wurzel_xy, abst=ABSTAENDE):
    """Breite (2 x Abstandstransformation) in festen Abstaenden (px entlang der Mittellinie) ab dem koerpernahen Ende; dazu die Laenge."""
    r = mittellinie(m)
    pf = r['pfad']
    if np.hypot(pf[-1][1] - wurzel_xy[0], pf[-1][0] - wurzel_xy[1]) < np.hypot(pf[0][1] - wurzel_xy[0], pf[0][0] - wurzel_xy[1]):
        pf = pf[::-1]
    seg = np.r_[0, np.cumsum(np.hypot(*np.diff(pf, axis=0).T))]
    out = {}
    for d_ in abst:
        out[d_] = None if d_ > seg[-1] else round(2 * float(r['dt'][pf[int(np.argmin(abs(seg - d_)))][0], pf[int(np.argmin(abs(seg - d_)))][1]]), 1)
    return dict(L=round(float(seg[-1]), 1), breite=out)


def pv_sichtbar(blob):
    """PV (Vorlage-Pose), ganze Figur mit f_rast (Maler-Reihenfolge, Culling): sichtbare Schwanz-Bildpunkte (Knochen 16-22)."""
    import f_rast as FR
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV, licht=False)
    return (A['bone'][:480] >= 16) & (A['bone'][:480] <= 22), mo


def r8b_r8c(blob, bezug, D):
    """Runde 8b (Pruefer pr8 (C) "R8b in festen Abstaenden ab der Wurzel", "R8c nur mit der sichtbaren Laenge"): PV, sichtbare Schwanz-Bildpunkte der
    ganzen Figur, Mittellinie und Breiten wie das Pruefer-Werkzeug a_mass.profil_abs; Video = Nutzer-Umriss h90 mit demselben Verfahren
    (riegel_schwanz_r8.json, gemessen n7_pv_abstand.py)."""
    vid = {int(k): v for k, v in D['ansichten']['90']['abstand']['breite'].items()}
    vL, vy = D['ansichten']['90']['abstand']['L'], D['ansichten']['90']['abstand']['tief_y']
    m, mo = pv_sichtbar(blob)
    mb, _mob = pv_sichtbar(bezug)
    n, b = profil_abstand(m, PV_WURZEL), profil_abstand(mb, PV_WURZEL)
    ys = np.nonzero(m)[0]; ysb = np.nonzero(mb)[0]
    abw = {d_: (None if (n['breite'][d_] is None or vid.get(d_) is None) else round(n['breite'][d_] - vid[d_], 1)) for d_ in R8B_STELLEN}
    werte = [abs(x) for x in abw.values() if x is not None]
    okb = (all(x is not None for x in abw.values()) and max(werte) <= R8B_MAX and float(np.mean(werte)) <= R8B_MITTEL
           and n['breite'][90] <= 0.9 * b['breite'][90])
    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    i = int(np.argmax(P22[:, 1]))
    achse = bool(P22[i][0] == 0 and P22[i][2] == 0)
    okc = n['L'] >= R8C_LAENGE and int(ys.max()) >= R8C_TIEF and achse and int(P22[i][1]) == int(Q.SPITZE_Y)
    return (dict(breite=n['breite'], bezug=b['breite'], video=vid, abweichung=abw, mittel=round(float(np.mean(werte)), 1) if werte else None,
                 ok=bool(okb)),
            dict(laenge=n['L'], bezug=b['L'], video_laenge=vL, tief_y=int(ys.max()), bezug_tief_y=int(ysb.max()), video_tief_y=vy,
                 nadel_y=int(P22[i][1]), soll_y=int(Q.SPITZE_Y), auf_achse=achse, ok=bool(okc)), dict(mo=mo))


''' + s[a1:]
ers("""ZIEL_SPITZE = ((286.0, 289.0), (349.0, 356.0))""", """ZIEL_SPITZE = ((286.0, 289.0), (349.0, 356.0))
ABSTAENDE = (10, 30, 50, 70, 90, 110, 130, 150, 170, 190, 210)
PV_WURZEL = (376, 220)                # Wurzel im Bild (PV, Gelenk 0 projiziert 375,7 / 219,5) - waehlt das koerpernahe Ende der Mittellinie
R8B_STELLEN = (30, 50, 70, 90, 110, 130, 150, 170)
R8B_MAX, R8B_MITTEL = 6.0, 5.0        # Abweichung Port - Video je Stelle hoechstens 6 px, im Mittel 5 px (Video weich, +2-3 px Unschaerfe, ANALYSE 7;
                                      # das Verfahren misst Port und Video gleich); dazu bei 90 px <= 0,9 x vorher (Mitte duenner)
R8C_LAENGE = 180.0                    # sichtbare Laenge in PV (Video 216,9, vorher 164,7; der Rest liegt laut Analyse an der Wurzel und der Bahn)
R8C_TIEF = 346                        # tiefster sichtbarer Schwanz-Bildpunkt in PV (Video 351, vorher 325; Pruefer "bis y ~350")""")
# R8d / R8e / R8f Anpassungen
ers("""    nutz = {Q.ZEILE_FLEISCH: A4.nutzung(None, 482), Q.ZEILE_BAND: A4.nutzung(None, 481)}""",
    """    nutz = {Q.ZEILE_FLEISCH: A4.nutzung(None, 482), Q.ZEILE_BAND: A4.nutzung(None, 480 + (Q.ZEILE_BAND - 493))}   # 493/494/495 = Original 480/481/482""")
a0 = s.index('def r8f(blob, bezug):')
a1 = s.index('def ist_r8(blob):')
s = s[:a0] + '''def r8f(blob, bezug):
    """Bytegleich zum Eingang ausser Meshes 16-22 und (Runde 8b) den 12-Bit-Winkeln der Knochen 17-22 in den angehobenen Keyframes: die EMR des
    Pruefllings, in der genau diese Bits auf die Werte des Eingangs zurueckgesetzt werden, muss bytegleich zur EMR des Eingangs sein."""
    import birkin_formen as BF
    import emd_ansichtsblatt as EA
    d, _o, ms, tim = KB.teile(blob)
    de, _oe, mse, time = KB.teile(bezug)
    sek = [blob[d[i]:d[i + 1]] == bezug[de[i]:de[i + 1]] for i in range(7)]
    meshes = [ms[i] == mse[i] for i in range(16)]
    mo = V.modell(blob)
    sk = mo.skel
    emr, emr_e = bytearray(blob[d[2]:d[3]]), bytes(bezug[de[2]:de[3]])
    geaendert = 0
    if len(emr) == len(emr_e):
        for k in range(sk.kf_count):
            o = EA.u16(emr, 2) + k * sk.kf_size + 12
            anders = False
            for b in range(17, 23):
                for j in range(3):
                    bit = b * 36 + j * 12
                    alt = sum(((emr_e[o + ((bit + i) >> 3)] >> ((bit + i) & 7)) & 1) << i for i in range(12))
                    neu = sum(((emr[o + ((bit + i) >> 3)] >> ((bit + i) & 7)) & 1) << i for i in range(12))
                    if alt != neu:
                        anders = True
                        BF._bits_schreiben(emr, o, bit, alt)
            geaendert += anders
    emr_rest = bytes(emr) == emr_e
    sek_ok = all(sek[i] for i in range(7) if i != 2) and emr_rest
    gl = all(tuple(mo.skel.rel[b]) == (0, 541, 0) for b in range(17, 23))
    return dict(sektionen=sek, emr_nur_schwanzwinkel=emr_rest, keyframes_angehoben=geaendert, tim=tim == time, meshes_0_15=all(meshes),
                mesh9=meshes[9], glieder_541=gl, ok=bool(sek_ok and tim == time and all(meshes) and gl))


def r8g(debug_7e, debug_neu, state_7e, state_neu, blob, nachlauf=None):
    """Runde 8b (Koordinator "Szenenkette gegen 7e ... Verschiebung muss begruendet oder beseitigt sein"): Schwanzkette der Szene (debug.log
    "[hangar-takt] h.. Schwanz B") gegen den Lauf 7e. Begruendet = (1) state.log bytegleich; (2) jede Abweichung > 1 E. liegt in einem Takt, in dem die
    7e-Kette mit den Mesh-Massen DIESES Assets die Bodenregel des Codes verletzt (schwanz_r41.c glaetten: Gelenk j + radius[j] oder Dornpunkt
    Gelenk 6 + Achse x dorn_y unter y 0; Masse wie sw_mesh_mass), oder im Nachlauf danach (<= NACHLAUF Takte, Abweichung faellt); (3) in den Takten,
    in denen die 7e-Kette verletzt, haelt die neue Kette die Bodenregel ein (Gelenke + Radius, Dornpunkt y <= 0,5)."""
    import re
    nachlauf = R8G_NACHLAUF if nachlauf is None else nachlauf

    def lese(p):
        G = {}
        for l in open(p, encoding='latin-1'):
            m_ = re.match(r'\\[hangar-takt\\] h(-?\\d+) Schwanz B y (-?\\d+) (.*)', l)
            if m_:
                G[int(m_.group(1))] = np.array([[int(a), int(b_), int(c)] for a, b_, c in re.findall(r'\\d\\((-?\\d+),(-?\\d+),(-?\\d+)\\)', m_.group(3))], float)
        return G
    A, B = lese(debug_7e), lese(debug_neu)
    mo = V.modell(blob)
    rad, dorn = np.zeros(8), 0.0
    for g in range(7):
        P = np.array([v[:3] for v in mo.meshes[16 + g].tv], float)
        r_ = float(np.hypot(P[:, 0], P[:, 2]).max())
        rad[g], rad[g + 1] = max(rad[g], r_), max(rad[g + 1], r_)
        if g == 6:
            dorn = float(P[:, 1].max())

    def verletzt(P):
        dp = P[6] + (P[7] - P[6]) * dorn / 541.0
        return bool((P[2:, 1] + rad[2:] > 0.5).any() or dp[1] > 0.5)
    hs = sorted(set(A) & set(B))
    flag = [h for h in hs if verletzt(A[h])]
    diff = {h: round(float(np.linalg.norm(A[h] - B[h], axis=1).max())) for h in hs}
    anders = [h for h in hs if diff[h] > 1]
    unbegruendet = []
    for h in anders:
        if h in flag:
            continue
        vor = [f for f in flag if 0 < h - f <= nachlauf]
        if not vor:
            unbegruendet.append((h, diff[h])); continue
        f = max(vor)
        if diff[h] > max(diff[f2] for f2 in flag if f2 <= h) + 1:
            unbegruendet.append((h, diff[h]))
    neu_verletzt = [h for h in flag if verletzt(B[h])]
    st = open(state_7e, 'rb').read() == open(state_neu, 'rb').read()
    ok = st and not unbegruendet and not neu_verletzt
    return dict(takte=len(hs), anders=len(anders), max_e=max(diff.values()) if diff else 0, bodenregel_takte=flag, unbegruendet=unbegruendet,
                neue_kette_verletzt=neu_verletzt, state_gleich=st, radius=rad.round(1).tolist(), dorn_y=dorn, nachlauf=nachlauf,
                diff=diff, ok=bool(ok))


''' + s[a1:]
ers("""R8B_STELLEN = (30, 50, 70, 90, 110, 130, 150, 170)""", """R8B_STELLEN = (30, 50, 70, 90, 110, 130, 150, 170)
R8G_NACHLAUF = 40                     # Takte Nachlauf nach dem letzten Bodenregel-Takt (Feder/Erhalt 0,5, Mitnahme 0,8: gemessen klingt die Abweichung
                                      # in der 7e/8-Szene binnen 34 Takten auf <= 1 E. ab, szene_kette_7e_r8.txt h67 -> h101)""")
# alles / zeilen
ers("""def alles(blob, farbe='b'):
    D = json.load(open(DATEN))""", """def alles(blob, farbe='b', szene=None):
    D = json.load(open(DATEN))""")
ers("""    r['R8f'] = r8f(blob, bezug)
    return r""", """    r['R8f'] = r8f(blob, bezug)
    if szene:                             # (debug_7e, debug_neu, state_7e, state_neu)
        r['R8g'] = r8g(*szene, blob)
    return r""")
a0 = s.index("    b = r['R8b']\n    z.append('R8b Seite PV")
a1 = s.index("    d = r['R8d']")
s = s[:a0] + """    b = r['R8b']
    z.append('R8b Seite PV in festen Abstaenden ab dem koerpernahen Ende (sichtbare Schwanz-Bildpunkte, Verfahren Pruefer pr8): Abstand -> Breite '
             'neu %s | vorher %s | Video %s; Abweichung neu - Video an 30-170 %s, Mittel %s (Soll je <= %.0f, Mittel <= %.0f; bei 90 <= 0,9 x vorher) -> %s'
             % (b['breite'], b['bezug'], b['video'], b['abweichung'], b['mittel'], R8B_MAX, R8B_MITTEL, g(b['ok'])))
    c = r['R8c']
    z.append('R8c sichtbare Laenge PV: %.1f px (vorher %.1f, Video %.1f; Soll >= %.0f), tiefster sichtbarer Schwanz-Bildpunkt y %d (vorher %d, Video %d; '
             'Soll >= %d), Nadelspitze y %d (Soll %d) auf der Achse %s -> %s'
             % (c['laenge'], c['bezug'], c['video_laenge'], R8C_LAENGE, c['tief_y'], c['bezug_tief_y'], c['video_tief_y'], R8C_TIEF, c['nadel_y'],
                c['soll_y'], c['auf_achse'], g(c['ok'])))
""" + s[a1:]
ers("""    f = r['R8f']
    z.append('R8f Bytegleich zum Eingang 07b6703f: Sektionen 0-6 %s, TIM %s, Meshes 0-15 %s (Mesh 9 %s), Gliedlaengen 541 %s -> %s'
             % (f['sektionen'], f['tim'], f['meshes_0_15'], f['mesh9'], f['glieder_541'], g(f['ok'])))
    rot = [k for k in ('R8a', 'R8b', 'R8c', 'R8d', 'R8e', 'R8f') if not r[k]['ok']]
    z.append('SUMME R8: %d von 6 gruen%s' % (6 - len(rot), ('; ROT: ' + ', '.join(rot)) if rot else ''))""",
"""    f = r['R8f']
    z.append('R8f Bytegleich zum Eingang 07b6703f ausser Meshes 16-22 und den Winkeln der Knochen 17-22 in %d angehobenen Keyframes: Sektionen 0-6 %s '
             '(EMR nach Ruecksetzen dieser Winkel gleich: %s), TIM %s, Meshes 0-15 %s (Mesh 9 %s), Gliedlaengen 541 %s -> %s'
             % (f['keyframes_angehoben'], f['sektionen'], f['emr_nur_schwanzwinkel'], f['tim'], f['meshes_0_15'], f['mesh9'], f['glieder_541'],
                g(f['ok'])))
    keys = ['R8a', 'R8b', 'R8c', 'R8d', 'R8e', 'R8f']
    if 'R8g' in r:
        q = r['R8g']
        keys.append('R8g')
        z.append('R8g Szenenkette gegen 7e: %d Takte, %d anders (groesste Verschiebung %d E.), state.log gleich %s; Bodenregel-Takte der 7e-Kette mit den '
                 'Massen dieses Assets (radius %s, dorn_y %.0f) %s; unbegruendet (nicht Bodenregel, nicht Nachlauf <= %d) %s; neue Kette verletzt %s -> %s'
                 % (q['takte'], q['anders'], q['max_e'], q['state_gleich'], q['radius'], q['dorn_y'], q['bodenregel_takte'], q['nachlauf'],
                    q['unbegruendet'], q['neue_kette_verletzt'], g(q['ok'])))
    rot = [k for k in keys if not r[k]['ok']]
    z.append('SUMME R8: %d von %d gruen%s' % (len(keys) - len(rot), len(keys), ('; ROT: ' + ', '.join(rot)) if rot else ''))""")
ers("""    ap.add_argument('--farbe', default=Q.STANDARD)
    ap.add_argument('--aus'); ap.add_argument('--json')
    a = ap.parse_args()
    r = alles(open(a.emd, 'rb').read(), a.farbe)""", """    ap.add_argument('--farbe', default=Q.STANDARD)
    ap.add_argument('--aus'); ap.add_argument('--json')
    ap.add_argument('--szene', nargs=4, metavar=('DEBUG_7E', 'DEBUG_NEU', 'STATE_7E', 'STATE_NEU'), help='R8g: Szenenlaeufe 7e und neu')
    ap.add_argument('--nachlauf', type=int, default=None)
    a = ap.parse_args()
    r = alles(open(a.emd, 'rb').read(), a.farbe, a.szene)
    if a.nachlauf is not None and 'R8g' in r:
        r['R8g'] = r8g(*a.szene, open(a.emd, 'rb').read(), nachlauf=a.nachlauf)""")
open(p, 'w', encoding='utf-8').write(s)
# json: Video-Profil h90 (n7_pv_abstand.py, Verfahren a_mass.profil_abs, Wurzel (366, 213))
pj = os.path.join(W, 'riegel_schwanz_r8.json')
d = json.load(open(pj))
d['ansichten']['90']['abstand'] = dict(quelle='Nutzer-Umriss Areas.png - Videobild n1439 (Pruefer pr8 a_mass.nutzer_maske), Mittellinie/Breite wie '
                                       'a_mass.profil_abs, Wurzel (366, 213); gemessen n7_pv_abstand.py (runde8/werkzeug)',
                                       L=216.9, tief_y=351,
                                       breite={'10': 12.0, '30': 26.0, '50': 34.0, '70': 33.1, '90': 33.3, '110': 18.1, '130': 19.8, '150': 17.9,
                                               '170': 18.1, '190': 10.0, '210': 6.0})
json.dump(d, open(pj, 'w'), indent=1)
# riegel_v8 S6 streng
p = os.path.join(W, 'riegel_v8.py')
t = open(p, encoding='utf-8').read()
a = """    r8 = bool(dorn[1] > 1000)                                     # Runde 8: Nadel (riegel_schwanz_r8.ist_r8)
    nadel = dorn.copy()
    if r8:
        K = P22[P22[:, 1] <= 600.0]
        dorn = K[np.argmax(K[:, 1])]                              # Knollen-Ende (Ort des alten Dorns y 589)"""
b = """    r8 = bool(dorn[1] > 1000)                                     # Runde 8: Nadel (riegel_schwanz_r8.ist_r8)
    nadel = dorn.copy()                                           # Runde 8b: S6 wieder STRENG - Spitze und JEDE Ecke von Mesh 22 (Knolle, Nadel)"""
assert a in t; t = t.replace(a, b)
a = """        dn = T[22].astype(float) + R22 @ dorn
        if max(sp[1], dn[1]) > y:
            unter.append(k)
        if r8 and float((T[22].astype(float) + R22 @ nadel)[1]) > y:
            unter_nadel.append(k)"""
b = """        dn = T[22].astype(float) + R22 @ dorn
        if r8:
            dn = np.array([0.0, float((T[22].astype(float)[None, :] + P22 @ R22.T)[:, 1].max()), 0.0])
        if max(sp[1], dn[1]) > y:
            unter.append(k)
            if r8:
                unter_nadel.append(k)"""
assert a in t; t = t.replace(a, b)
a = """    return dict(keyframes=s.kf_count, unter=len(unter), beispiele=unter[:10], szene=sz, ok=ok, r8=r8,
                nadel_unter_info=len(unter_nadel) if r8 else None, nadel_y=int(nadel[1]), knolle_y=int(dorn[1]))"""
b = """    return dict(keyframes=s.kf_count, unter=len(unter), beispiele=unter[:10], szene=sz, ok=ok, r8=r8,
                nadel_unter_info=len(unter_nadel) if r8 else None, nadel_y=int(nadel[1]))"""
assert a in t; t = t.replace(a, b)
a = """        z.append('   S6 Runde 8: Keyframes mit Spitze + Knollen-Ende (Mesh 22 y %d) gezaehlt; Nadel (y %d) offline unter der Sohle in %d von %d Keyframes '
                 '(Angabe - im Spiel hebt die Bodenregel des Codes die Kette, Dorn = Nadel); Szene: Nadelspitze tiefstes y %s (Soll <= 0)'
                 % (s['knolle_y'], s['nadel_y'], s['nadel_unter_info'], s['keyframes'], (s['szene'] or {}).get('nadel_tiefstes_y', 'ohne --szene')))"""
b = """        z.append('   S6 Runde 8b (streng): Spitze und jede Ecke von Mesh 22 (Knolle, Nadel bis y %d) in %d von %d Keyframes unter der Sohle (Soll 0); '
                 'Szene: Nadelspitze tiefstes y %s (Soll <= 0)'
                 % (s['nadel_y'], s['nadel_unter_info'], s['keyframes'], (s['szene'] or {}).get('nadel_tiefstes_y', 'ohne --szene')))"""
assert a in t; t = t.replace(a, b)
a = """S6 Runde 8      (a) Keyframes: Spitze (Gelenk 7) und das Knollen-Ende (groesste Ecke von Mesh 22 bis y 600 = Ort des alten Dorns y 589) unter der
                tiefsten Koerperecke in 0 Keyframes; die Nadel (bis y 1139) offline nur Angabe - im Spiel hebt die Bodenregel des Codes die Kette
                mit der Nadel als Dornspitze (schwanz_r41.c sw_mesh_mass / glaetten); (c) Szene (--szene): alle Gelenke UND die Nadelspitze
                (Gelenk 6 + (Gelenk 7 - Gelenk 6) x 1139 / 541) y <= 0, Gelenkwinkel <= 25,5."""
b = """S6 Runde 8b     STRENG (Pruefer pr8 / Koordinator, die Runde-8-Fassung "Nadel nur Angabe" ist zurueckgenommen): (a) Spitze und JEDE Ecke von Mesh 22
                (Knolle, Nadel) in 0 von allen Keyframes unter der tiefsten Koerperecke (schwanz_r8.heben hebt die Keyframes); (c) Szene (--szene):
                alle Gelenke UND die Nadelspitze (Gelenk 6 + (Gelenk 7 - Gelenk 6) x dorn_y / 541) y <= 0, Gelenkwinkel <= 25,5."""
assert a in t; t = t.replace(a, b)
open(p, 'w', encoding='utf-8').write(t)
print('ok')
