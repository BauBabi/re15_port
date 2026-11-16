"""SF V8 Runde 7b (K1): Fleisch der Schwanz-Unterseite in ORIGINAL-DICHTE (Bau-Stufe u, nur wenn unterseite_v8.json "fleisch_r7b" traegt).

Aufgerufen aus unterseite_r5.anwenden: mitte_teilen() nach teilen(), uv_flaeche() fuer die Fleischflaechen (Klasse unten / kappe_unten,
Mittel |theta| < FLEISCH_KL = 110) der Meshes 16-22. Ohne den Schluessel "fleisch_r7b" bleibt der Bau wie Runde 6 (rueckwaertsvertraeglich).

Anlass (Sicht-Pruefer Runde 6, SF_PRUEFUNG_V8_r6_sicht.md (1b)): "Die Unterseite ist ein einheitliches, glattes, sattes Rot, fast ohne
Fleischstruktur ... wie eine leuchtend rote Klinge". Nutzer P4 (bindend): "Die Unterseite des großen Schwanzes ist fleischig wenn man in die
Vorlagen Bilder schaut. Nur die oberseite ist durchweg grau."
Ursache (gemessen, sfv8/r7b/dichte.py und dichte_fleisch.py am Stand a474188a): die Fleischflaechen der Runde 6 bilden je Glied das Feld
u 4-36 (um 220 Grad Umfang) x v 98-108 (10 Zeilen laengs 541 E.) ab -> Texeldichte (Wurzel aus Texelflaeche durch Flaeche) Median 0,018-0,021
Texel je Einheit in den Meshes 16-19, gesamt 0,023. Die Original-Flaechen, die dieses Fleischfeld benutzen (Original-EM30 Meshes 12/14/15,
Zeile 482, Texel v 92-125, 54 Dreiecke) haben 0,071 (u 0,062 / v 0,090 je Einheit) -> Runde 6 = 0,25- bis 0,30-fach, unter der Grenze
0,5-fach der Texturregel (Entscheid E4). Jeder Texel wird rund 3,5-fach vergroessert: aus den Laengsstreifen des Original-Fleischs werden
breite glatte Baender.

Abbildung hier (keine Farbe, keine Helligkeit am Video geeicht; Texturbild und CLUT bytegleich, Zeile und Seite wie Runde 6):
  Feld: Seite 2, Zeile 495 (= Original-Zeile 482), u U_LO-U_HI x v V_A-V_B = das zusammenhaengende obere Fleischstueck des Original-Unterarms,
        u 4-37 x v 96-106: alle Texel von Original-Flaechen der Zeile 482 benutzt, jede Zeile bei u 3-38 zu >= 86 % Fleisch (a* >= 15), jeder
        Laengsstreifen laeuft durch (sfv8/r7b feld_zoom.png). Darunter (v 110-117) trennt ein unbenutzter Schlitz (u 18-33, schraeg) zwei
        Original-UV-Inseln; ein Feld bis v 116 (Vorprobe U1) legte Texel in diesen Schlitz, v 95-109 (U3) liess die Furche u 14-16 vor jeder
        Abschnittsgrenze enden (Strichelreihe laengs der Unterseite).
  laengs: v entlang der Gliedachse, je Abschnitt V_A <-> V_B; jedes Glied hat ABSCHNITTE Abschnitte, die Grenzen sind Ringe (Mesh 18: der
        vorhandene Mittelring; sonst neue Ringe auf den Laengskanten, mitte_teilen: Ort, Normale und UV linear, die Form bleibt). An jedem
        Gelenk und jeder Abschnittsgrenze stossen dieselben Texelzeilen aneinander (keine Naht). Ohne Abschnitte reichen 11 Zeilen nicht
        fuer 541-710 E. Glied.
  um den Umfang: u = Feldmitte + p(Ecke) x W(Ring) / 2. p ist eine Dreieckswelle ueber die Ringecken j (11 je Ring, alle 32,7 Grad) mit
        Umkehr alle JE_DURCHGANG Ecken ab j = 9 (theta ~ -92); gespiegelt wird nur AN Ringecken (je Dreieck affin), an der Spiegelkante
        stossen gleiche Texel aneinander. W(Ring) = DICHTE x JE_DURCHGANG x Ringradius x 2 pi / 11: gleiche Dichte auf jedem Ring, an einem
        Gelenk gleiche Breite auf beiden Seiten (gleicher Radius). DICHTE = Feldbreite / Breite des Wurzelrings (konfig).
  Gewaehlt (unterseite_v8.json): Feld u 4-37 x v 96-106, JE_DURCHGANG 2 (Dichte um den Umfang 0,066 = 0,93-fach Original 0,071),
        ABSCHNITTE [1, 1, 2, 2, 2, 2, 2] (neue Ringe nur in den Meshes 19-22), Vierecke quer geteilt (diagonale false).
        Warum so (gemessen): jede Abschnittsteilung in den Meshes 16/17 hob Riegel S2 b (Sattel der Oberkontur PV 1,7 -> 2,4 px bei U4K2,
        3,0 px bei U7c, Soll <= 2): neue Ecken auf der Silhouette werden auf ganze Bildpunkte gerundet, die Rueckseiten-Pruefung der Vierecke
        (Ecken 0-1-2, platform/pc/main.c ~2310) faellt an den Teilstuecken anders aus; mit Teilung nur in 19-22 bleiben S1/S2 wie a474188a.
        Die Zerlegung an der gezeichneten Diagonale 0-3 (diagonale true) haelt die Flaeche exakt, liess aber Teilstuecke der zweiten
        Haelfte einzeln wegfallen: 84 Loch-Bildpunkte in 50 Ansichten (quer geteilt: 3; loecher.py). Laengs bleibt die Dichte in 16/17
        unter dem Soll (11 Zeilen auf 516-710 E.); die Laengsstreifen des Feldes sind entlang v gleich, sichtbar ist nur die Dichte quer.
        Proben im Spiel (echte Framedumps F262-F398 mit beleuchteter Unterseite, glaette_r7b.py, relative oertliche Streuung): Stand
        a474188a 0,079; U4 (3 je Durchgang, Mittelring in allen) 0,116; U5 (Feld u 17-37 ohne Furche) 0,087; U7 (2 je Durchgang, 3 Abschnitte
        in allen) 0,141; Eichband am eigenen Fleisch Rumpf 0,197 / Beinhaut 0,171.
  Ring-Deckel und Dorn (Achs-Ecke): u = Mittel ihrer Ring-Ecken, v um DICHTE x Abstand zur Ringebene ins Feld hinein (Runde 6: alle drei
        Ecken auf einer Zeile = Strich-Abbildung, Streckung bis 62); reicht das Feld nicht, wird die Ringkante in u gestaucht (Streckung <= 2).
Teilungs-Ecken der Runde 6 (Grenzen 110 / 120 auf einer Ringsehne) bekommen p linear zwischen ihren zwei Ring-Ecken (Lage auf der Sehne).
Jede Zahl ist PORT-WAHL mit der Messung oben; keine @0x-Konstante (die Sichel-Form existiert auf keiner Disc).
"""
import numpy as np

S_ = 11
U_LO, U_HI = 4.0, 37.0                # Feldbreite 33 Texel (Nutzungsmaske v 107-108 ab u 4)
V_A, V_B = 96.0, 106.0                # 11 Zeilen, in denen jeder Streifen des Original-Fleischs durchlaeuft (L*-Karte sfv8/r7b feld_zoom.png:
                                      # Lichtstreifen u 7 / 24-25 / 34-35 und Furche u 14-16 von v 94 bis 106/107; ab v 107-109 enden sie, ab
                                      # 110 beginnt der hellere Teil). Probe U3 mit v 95-109: die Furche endete vor jedem Mittelring ->
                                      # Strichelreihe ("Naht") laengs der Unterseite (Tafel nah_g2.png). v 96: 86 % Fleisch, v 98-106: >= 97 %
UC = 0.5 * (U_LO + U_HI)
DICHTE = 0.044                        # Texel je Einheit um den Umfang: groesste Dichte, mit der der Wurzelring (Mesh 16 Ring 1, r 437) mit
                                      # 3 Flaechen je Durchgang in das Feld passt (0,044 x 3 x 437 x 2 pi / 11 = 33,0); = 0,62-fach Original 0,071
JE_DURCHGANG = 3
# Dreieckswelle je Ringecke j (theta der Normalen am Stand a474188a: j 0 -26, 1 +6, 2 +39, 3 +72, 4 +105, 5 +137, 6 +170, 7 -157, 8 -125,
# 9 -92, 10 -59); Umkehr bei j 9 (-1), j 1 (+1), j 4 (-1). Ecken j 5-8 liegen nur ueber die Teilungs-Ecken (theta < 120) im Fleisch.
P_J = {9: -1.0, 10: -1.0 / 3, 0: 1.0 / 3, 1: 1.0, 2: 1.0 / 3, 3: -1.0 / 3, 4: -1.0, 5: -1.0 / 3, 6: 1.0 / 3, 7: 1.0 / 3, 8: -1.0 / 3}
# Laengs: jede Ring-Ecke hat eine Abschnittsnummer sigma (ganzzahlig, ueber den ganzen Schwanz fortlaufend: Gelenk g / g+1 teilen ihr sigma);
# v = V_A bei geradem, V_B bei ungeradem sigma, dazwischen linear. ABSCHNITTE = Abschnitte je Glied (Standard 2: Gelenk V_A, Mitte V_B).
# Mesh 16: Ring 0 liegt seit Runde 4 auf Ring 1 (Laenge 0, gemessen sfv8/r7b/laengen.py) und zaehlt als Ring 1 (_ring_k); sein sichtbares
# Band Ring 1 -> 2 (Unterseite 250-555 E.) wird wie das Band Ring 0 -> 1 der Meshes 17, 19-22 (516-659 E.) in ABSCHNITTE Teile geteilt.
# Mesh 18 hat einen Mittelring (Ring 1; Baender 222-282 / 317-402 E.): bei 2 Abschnitten bleibt er der einzige, bei 3 bekommt das laengere
# Band Ring 1 -> 2 einen weiteren.
ABSCHNITTE = 2
DIAGONALE = True                      # Vierecke zuerst an der gezeichneten Diagonale 0-3 zerlegen (konfig "diagonale")
V_SCHWANZ = tuple(range(16, 23))


def _abschnitte(b):
    return ABSCHNITTE[b - 16] if isinstance(ABSCHNITTE, (list, tuple)) else ABSCHNITTE


def _plan(b):
    """-> (sigma je Ring relativ zum Gliedanfang, [(Band ka, kb, Teile)]) fuer die Abschnittszahl des Glieds (Mesh 18 mindestens 2: es hat
    seinen Mittelring)."""
    a = _abschnitte(b)
    if b == 16:
        return [0, 0, a], ([(1, 2, a)] if a > 1 else [])
    if b == 18:
        assert a >= 2, a
        return ([0, 1, 2], []) if a == 2 else ([0, 1, a], [(1, 2, a - 1)])
    return [0, a], ([(0, 1, a)] if a > 1 else [])


def p_welle(j, n):
    """Dreieckswelle ueber die Ringecken: Schritt q ab Ecke j 9 (theta ~ -92) im Umlauf j 9, 10, 0, 1, ... (j 8 = q -1); Umkehr alle n Ecken
    (-1 bei q = 0, +1 bei q = n, ...)."""
    q = (j - 9) % S_
    if q > 8:
        q -= S_
    x = (q / float(n)) % 2.0
    return -1.0 + 2.0 * x if x <= 1.0 else 3.0 - 2.0 * x


WURZEL_R = 437.5                      # Radius Mesh 16 Ring 1 am Stand a474188a (gemessen ring_radius; Kette unveraendert)


def konfig(cfg):
    """unterseite_v8.json "fleisch_r7b": 1 (Standard oben) oder {"feld": [u_lo, u_hi, v_a, v_b], "je_durchgang": n, "abschnitte": k}. Die
    Dichte um den Umfang folgt aus dem Feld: Feldbreite / (n x Flaechenbreite des Wurzelrings, r WURZEL_R)."""
    global U_LO, U_HI, V_A, V_B, UC, DICHTE, JE_DURCHGANG, P_J, ABSCHNITTE, DIAGONALE
    if isinstance(cfg, dict):
        DIAGONALE = bool(cfg.get('diagonale', True))
        U_LO, U_HI, V_A, V_B = [float(x) for x in cfg.get('feld', (U_LO, U_HI, V_A, V_B))]
        JE_DURCHGANG = int(cfg.get('je_durchgang', JE_DURCHGANG))
        ab = cfg.get('abschnitte', ABSCHNITTE)
        ABSCHNITTE = [int(x) for x in ab] if isinstance(ab, (list, tuple)) else int(ab)
        DICHTE = round((U_HI - U_LO) / (JE_DURCHGANG * WURZEL_R * 2.0 * np.pi / S_), 4)
        P_J = {j: p_welle(j, JE_DURCHGANG) for j in range(S_)}
    UC = 0.5 * (U_LO + U_HI)
    return dict(feld=[U_LO, U_HI, V_A, V_B], je_durchgang=JE_DURCHGANG, abschnitte=ABSCHNITTE, dichte=DICHTE)


def ring_zahl(b):
    return 3 if b in (16, 18) else 2


def sigma0(b):
    return sum(_abschnitte(k) for k in range(16, b))


def ring_radius(m, b, k):
    P = np.array([v[:3] for v in m['v'][k * S_:(k + 1) * S_]], float)
    return float(np.mean(np.hypot(P[:, 0], P[:, 2])))


def breite(r):
    w = DICHTE * JE_DURCHGANG * r * 2.0 * np.pi / S_
    assert w <= (U_HI - U_LO) + 0.05, ('Ring breiter als das Feld', r, w)
    return w


def _sehne(m, b, i):
    """Teilungs-Ecke i -> (Ring k, Ecke j, t) der Ringsehne j -> j+1, auf der sie liegt; None, wenn keine (<= 2 E.)."""
    nr = ring_zahl(b)
    q = np.array(m['v'][i][:3], float)
    best = None
    for k in range(nr):
        for j in range(S_):
            a = np.array(m['v'][k * S_ + j][:3], float); c = np.array(m['v'][k * S_ + (j + 1) % S_][:3], float)
            d = c - a
            t = float(np.clip((q - a) @ d / max(d @ d, 1e-9), 0.0, 1.0))
            e = float(np.linalg.norm(a + t * d - q))
            if best is None or e < best[0]:
                best = (e, k, j, t)
    if best[0] > 2.0:
        return None
    return best[1], best[2], best[3]


def _ring_k(b, k):
    return max(k, 1) if b == 16 else k                         # Mesh 16: Ring 0 = Ring 1 (Laenge 0)


def ecke_info(m, b, i):
    """-> (p, Ringradius, sigma) einer Ring-, Teilungs- oder Abschnitts-Ecke; None fuer Achs-Ecken."""
    nr = ring_zahl(b)
    if i in m.get('_r7b_mitte', {}):
        return m['_r7b_mitte'][i]
    sig = _plan(b)[0]
    if i < nr * S_:
        k, j = divmod(i, S_)
        return P_J[j], ring_radius(m, b, k), sigma0(b) + sig[k]
    if i < nr * S_ + 2:
        return None
    r = _sehne(m, b, i)
    assert r is not None, ('Teilungs-Ecke liegt auf keiner Ringsehne', b, i)
    k, j, t = r
    return (1 - t) * P_J[j] + t * P_J[(j + 1) % S_], ring_radius(m, b, k), sigma0(b) + sig[k]


def _sigma_von(m, b, i):
    """sigma einer Ring-, Teilungs- oder Abschnitts-Ecke (fuer mitte_teilen), None sonst."""
    nr = ring_zahl(b)
    if i in m.get('_r7b_mitte', {}):
        return m['_r7b_mitte'][i][2]
    sig = _plan(b)[0]
    if i < nr * S_:
        return sigma0(b) + sig[_ring_k(b, i // S_)]
    if i < nr * S_ + 2:
        return None
    r = _sehne(m, b, i)
    return None if r is None else sigma0(b) + sig[_ring_k(b, r[0])]


def mitte_teilen(ms, wurzel_keys):
    """Abschnitts-Ringe: jede Flaeche des Mantels (Viereck oder Dreieck), deren Ecken auf zwei sigma-Stufen liegen, die mehr als 1 auseinander
    sind, wird an den ganzzahligen Zwischenstufen geteilt (Laengskanten bei t = Stufe / Stufenabstand, gleiche neue Ecke fuer beide Nachbarn
    einer Kante, keine T-Kreuzung). Ort und Normale linear (Normale auf 4096), UV linear; das erste Teilstueck ersetzt die Flaeche an ihrem
    Index, die weiteren stehen am Ende der Listen (Indizes der Wurzelflaechen von Mesh 16 - Marker S10s - bleiben). -> Zaehlung je Mesh."""
    zahl = {}
    for b in V_SCHWANZ:
        if not _plan(b)[1]:
            continue
        m = ms[b]
        ausn = wurzel_keys(m) if b == 16 else set()
        n0 = (len(m['v']), len(m['tf']), len(m['qf']))
        mitte = m.setdefault('_r7b_mitte', {})
        cache = {}

        def sg(i):
            return _sigma_von(m, b, i)

        def neue(i, j, c):
            """neue Ecke auf der Laengskante i -> j bei der sigma-Stufe c."""
            key = (min(i, j), max(i, j), c)
            if key not in cache:
                si, sj = sg(i), sg(j)
                t = (c - si) / float(sj - si)
                pa = np.array(m['v'][i][:3], float); pc = np.array(m['v'][j][:3], float)
                na = np.array(m['n'][i][:3], float); nc = np.array(m['n'][j][:3], float)
                nn = (1 - t) * na + t * nc; nn = nn / np.linalg.norm(nn) * 4096.0
                m['v'].append([int(round(float(x))) for x in (1 - t) * pa + t * pc] + [0])
                m['n'].append([int(round(float(x))) for x in nn] + [0])
                ia, ic = ecke_info(m, b, i), ecke_info(m, b, j)
                mitte[len(m['v']) - 1] = ((1 - t) * ia[0] + t * ic[0], (1 - t) * ia[1] + t * ic[1], c)
                cache[key] = len(m['v']) - 1
            return cache[key]

        def teile(poly):
            """poly = [(ecke, (u, v))] im Umlauf -> (Stueck bis zur Stufe c = unten + 1, Rest ab c) als Umlauf-Listen, oder None."""
            s = [sg(i) for i, _uv in poly]
            if any(x is None for x in s) or len(set(s)) != 2:
                return None
            lo, hi = min(s), max(s)
            if hi - lo < 2:
                return None
            c = lo + 1
            out = []
            for k in range(len(poly)):
                i, uv = poly[k]; i2, uv2 = poly[(k + 1) % len(poly)]
                out.append((i, uv, 'L' if s[k] == lo else 'H'))
                if s[k] != s[(k + 1) % len(poly)]:
                    t = (c - s[k]) / float(s[(k + 1) % len(poly)] - s[k])
                    out.append((neue(i, i2, c), (uv[0] + t * (uv2[0] - uv[0]), uv[1] + t * (uv2[1] - uv[1])), 'M'))
            km = [k for k, x in enumerate(out) if x[2] == 'M']
            a = [(i, uv) for i, uv, w in out[km[1]:] + out[:km[1]] if w in ('L', 'M')]
            h = [(i, uv) for i, uv, w in out[km[0]:] + out[:km[0]] if w in ('H', 'M')]
            return a, h

        def zerlegen(poly):
            r = teile(poly)
            if r is None:
                return [poly]
            return [r[0]] + zerlegen(r[1])

        def schreiben(stueck, alt_uv, ziel, art):
            n = len(stueck)
            assert n in (3, 4), stueck
            if n == 4:
                stueck = [stueck[0], stueck[1], stueck[3], stueck[2]]    # Umlauf -> Viereck-Reihenfolge 0-1-2 / 1-3-2
            fn = []
            for i, _uv in stueck:
                fn += [i, i]
            un = [0] * (12 if n == 4 else 9)
            un[2], un[5], un[8] = alt_uv[2], alt_uv[5], alt_uv[8]
            if n == 4:
                un[11] = alt_uv[11] if len(alt_uv) == 12 else 0
            for k, (_i, (u_, v_)) in enumerate(stueck):
                un[3 * k], un[3 * k + 1] = int(round(u_)), int(round(v_))
            if ziel is not None:
                (m['qf'] if art == 'v' else m['tf'])[ziel] = fn
                (m['quv'] if art == 'v' else m['tuv'])[ziel] = un
            elif n == 4:
                m['qf'].append(fn); m['quv'].append(un)
            else:
                m['tf'].append(fn); m['tuv'].append(un)

        for qi in range(n0[2]):
            if ('v%d' % qi) in ausn:
                continue
            f, uv = m['qf'][qi], m['quv'][qi]
            ie = list(f[1::2])
            assert list(f[0::2]) == ie
            poly = [(ie[0], (uv[0], uv[1])), (ie[1], (uv[3], uv[4])), (ie[3], (uv[9], uv[10])), (ie[2], (uv[6], uv[7]))]
            if teile(poly) is None:
                continue
            if not DIAGONALE:                   # Probe: Viereck quer geteilt (Teilstuecke mit eigener Diagonale)
                st = zerlegen(poly)
                alt = list(uv)
                schreiben(st[0], alt, qi, 'v')
                for x in st[1:]:
                    schreiben(x, alt, None, 'v')
                continue
            # Die Form bleibt: der Port zeichnet ein Viereck als Dreiecke 0-1-3 und 0-3-2 (Diagonale Ecke 0 - Ecke 3; platform/pc/main.c
            # ~2327 re15_render_textured_tri_lit, ebenso f_rast.dreiecke); ein nicht ebenes Viereck quer zu teilen verschob die Flaeche
            # (Probe 5a358bc9: Riegel S2 b Sattel 1,7 -> 3,0 px, S1 Max 6 -> 7; Umriss PV 135 px anders). Deshalb wird zuerst an dieser
            # Diagonale in die zwei gezeichneten Dreiecke zerlegt und jedes fuer sich geteilt (neue Ecken auch auf der Diagonale); jedes
            # Teilstueck liegt in der Ebene seines Dreiecks.
            d1 = [poly[0], poly[1], poly[2]]                # Ecken 0, 1, 3
            d2 = [poly[0], poly[2], poly[3]]                # Ecken 0, 3, 2
            st = zerlegen(d1) + zerlegen(d2)
            alt = list(uv)
            erst = [x for x in st if len(x) == 4]
            assert erst, ('kein Viereck-Teilstueck', b, qi)
            schreiben(erst[0], alt, qi, 'v')
            for x in st:
                if x is erst[0]:
                    continue
                if len(x) == 4:
                    schreiben(x, alt, None, 'v')
                else:
                    schreiben(x, [alt[0], alt[1], alt[2], alt[3], alt[4], alt[5], alt[6], alt[7], alt[8]], None, 'd')
        for di in range(n0[1]):
            if ('d%d' % di) in ausn:
                continue
            f, uv = m['tf'][di], m['tuv'][di]
            ie = list(f[1::2])
            if list(f[0::2]) != ie:
                continue
            poly = [(ie[k], (uv[3 * k], uv[3 * k + 1])) for k in range(3)]
            st = zerlegen(poly)
            if len(st) == 1:
                continue
            alt = list(uv)
            dr = [x for x in st if len(x) == 3]
            assert len(dr) == 1, ('Dreieck zerfaellt nicht in genau ein Dreieck', b, di)
            schreiben(dr[0], alt, di, 'd')
            for x in st:
                if len(x) == 4:
                    schreiben(x, alt, None, 'v')
        zahl[str(b)] = dict(ecken=len(m['v']) - n0[0], dreiecke=len(m['tf']) - n0[1], vierecke=len(m['qf']) - n0[2])
    return zahl


def uv_flaeche(m, b, ie):
    """UV (u, v) je Ecke einer Fleischflaeche (Ecken-Indizes ie) nach der Abbildung oben."""
    out = [None] * len(ie)
    ring = []
    for k, i in enumerate(ie):
        info = ecke_info(m, b, i)
        if info is None:
            continue
        p, r, sigma = info
        out[k] = (UC + p * breite(r) / 2.0, V_B if int(round(sigma)) % 2 else V_A)
        ring.append(k)
    for k, i in enumerate(ie):
        if out[k] is not None:
            continue
        # Achs-Ecke (Deckel-Mitte / Dorn-Spitze): Abstand zur Ebene der Ring-Ecken dieser Flaeche, ins Feld hinein
        u = float(np.mean([out[q][0] for q in ring]))
        v_r = float(np.mean([out[q][1] for q in ring]))
        Pr = np.array([m['v'][ie[q]][:3] for q in ring], float)
        abst = float(np.linalg.norm(np.array(m['v'][i][:3], float) - Pr.mean(0)))
        dv = DICHTE * abst
        v = v_r - dv if v_r >= 0.5 * (V_A + V_B) else v_r + dv
        v = float(np.clip(v, V_A, V_B))
        out[k] = (u, v)
        # Streckung <= STRECKUNG_MAX (Entscheid E4): reicht das Feld fuer den Abstand Achs-Ecke -> Ringkante nicht (v am Feldrand gekappt),
        # wird die Ringkante dieser Flaeche um ihre Mitte in u gestaucht (Probe U7: 58 Deckel-Dreiecke mit Streckung > 2, Feld 10 Zeilen
        # gegen 0,066 x 350 E. = 23 Zeilen)
        if len(ring) == 2 and abst > 1.0:
            a_, c_ = ring
            sehne = float(np.linalg.norm(np.array(m['v'][ie[a_]][:3], float) - np.array(m['v'][ie[c_]][:3], float)))
            d_u = abs(out[a_][0] - out[c_][0]) / max(sehne, 1.0)
            d_v = abs(v - v_r) / abst
            if d_v > 0 and d_u > STRECKUNG_MAX * d_v:
                f_ = STRECKUNG_MAX * d_v / d_u
                for q in ring:
                    out[q] = (u + f_ * (out[q][0] - u), out[q][1])
    return [(int(round(u)), int(round(v))) for u, v in out]


STRECKUNG_MAX = 2.0                   # Entscheid E4: "Streckung hoechstens 2"
