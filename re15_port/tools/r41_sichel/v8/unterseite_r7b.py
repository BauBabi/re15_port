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
  Feld: Seite 2, Zeile 495 (= Original-Zeile 482), u U_LO-U_HI x v V_A-V_B = das zusammenhaengende obere Fleischstueck des Original-Unterarms:
        jede Texelzeile v 95-109 ist bei u 3-38 zu >= 78 % Fleisch (a* >= 15), und alle Texel u 4-37 x v 95-109 sind von Original-Flaechen
        der Zeile 482 benutzt. Darunter (v 110-117) trennt ein unbenutzter Schlitz (u 18-33, schraeg) zwei Original-UV-Inseln (gemessen,
        Nutzungsmaske im Dossier); ein Feld bis v 116 (erster Versuch) legte Texel in diesen Schlitz.
  laengs: v entlang der Gliedachse; jedes Glied laeuft V_A (Gelenk) -> V_B (Mitte) -> V_A (Gelenk). Die Mitte ist ein Ring: bei Mesh 18 der
        vorhandene Mittelring, bei den Meshes 16 (Band Ring 1 -> 2) und 17, 19-22 (Band Ring 0 -> 1) ein neuer Ring auf halber Laenge jeder
        Laengskante (mitte_teilen: Ort, Normale und UV gemittelt, die Form bleibt). Ohne ihn reichen 15 Zeilen nicht fuer 541-710 E. Glied
        (0,021-0,028 je E.); mit ihm 15 Zeilen auf 250-355 E. (0,042-0,060 = 0,6- bis 0,85-fach). An jedem Gelenk und jedem Mittelring stossen
        dieselben Texelzeilen aneinander (keine Naht).
  um den Umfang: u = Feldmitte + p(Ecke) x W(Ring) / 2. p ist eine Dreieckswelle ueber die Ringecken j (11 je Ring, alle 32,7 Grad), die an
        den Ringecken j = 9 (theta ~ -90) und j = 4 (theta ~ +104) bei -1 und an j = 1 (theta ~ +6, Innenkante des Bogens) bei +1 umkehrt:
        3 Flaechen je Durchgang durch das Feld. Gespiegelt wird nur AN Ringecken (je Dreieck affin, keine Stauchung); an der Spiegelkante
        stossen gleiche Texel aneinander. W(Ring) = DICHTE x 3 x Ringradius x 2 pi / 11: gleiche Dichte auf jedem Ring, an einem Gelenk gleiche
        Breite auf beiden Seiten (gleicher Radius), Trapez im Glied wie die Geometrie.
  Ring-Deckel und Dorn (Achs-Ecke): u = Mittel ihrer Ring-Ecken, v um DICHTE x Abstand zur Ringebene ins Feld hinein (Runde 6: alle drei
        Ecken auf einer Zeile = Strich-Abbildung, Streckung bis 62).
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
# Rolle der Ringe je Mesh: 'g' = Gelenk (v V_A), 'm' = Mitte (v V_B). Mesh 16: Ring 0 liegt seit Runde 4 auf Ring 1 (Laenge 0, gemessen
# sfv8/r7b/laengen.py) und zaehlt als Ring 1 (_ring_von); sein sichtbares Band Ring 1 -> 2 (Unterseite 250-555 E.) bekommt wie die Meshes 17,
# 19-22 (516-659 E.) einen Mittelring. Mesh 18 hat einen (Ring 1; Baender 222-282 / 317-402 E.).
ROLLEN = {16: 'ggg', 18: 'gmg'}
TEILEN = {16: (1, 2), 17: (0, 1), 19: (0, 1), 20: (0, 1), 21: (0, 1), 22: (0, 1)}   # Band, das einen Mittelring bekommt


def p_welle(j, n):
    """Dreieckswelle ueber die Ringecken: Schritt q ab Ecke j 9 (theta ~ -92) im Umlauf j 9, 10, 0, 1, ... (j 8 = q -1); Umkehr alle n Ecken
    (-1 bei q = 0, +1 bei q = n, ...)."""
    q = (j - 9) % S_
    if q > 8:
        q -= S_
    x = (q / float(n)) % 2.0
    return -1.0 + 2.0 * x if x <= 1.0 else 3.0 - 2.0 * x


def konfig(cfg):
    """unterseite_v8.json "fleisch_r7b": 1 (Standard oben) oder {"feld": [u_lo, u_hi, v_a, v_b], "je_durchgang": n}. Die Dichte um den Umfang
    folgt aus dem Feld: Feldbreite / (n x Flaechenbreite des Wurzelrings, r WURZEL_R)."""
    global U_LO, U_HI, V_A, V_B, UC, DICHTE, JE_DURCHGANG, P_J
    if isinstance(cfg, dict):
        U_LO, U_HI, V_A, V_B = [float(x) for x in cfg.get('feld', (U_LO, U_HI, V_A, V_B))]
        JE_DURCHGANG = int(cfg.get('je_durchgang', JE_DURCHGANG))
        DICHTE = round((U_HI - U_LO) / (JE_DURCHGANG * WURZEL_R * 2.0 * np.pi / S_), 4)
        P_J = {j: p_welle(j, JE_DURCHGANG) for j in range(S_)}
    UC = 0.5 * (U_LO + U_HI)
    return dict(feld=[U_LO, U_HI, V_A, V_B], je_durchgang=JE_DURCHGANG, dichte=DICHTE)


WURZEL_R = 437.5                      # Radius Mesh 16 Ring 1 am Stand a474188a (gemessen ring_radius; Kette unveraendert)


def ring_zahl(b):
    return 3 if b in (16, 18) else 2


def ring_radius(m, b, k):
    P = np.array([v[:3] for v in m['v'][k * S_:(k + 1) * S_]], float)
    return float(np.mean(np.hypot(P[:, 0], P[:, 2])))


def breite(r):
    w = DICHTE * JE_DURCHGANG * r * 2.0 * np.pi / S_
    assert w <= (U_HI - U_LO) + 0.05, ('Ring breiter als das Feld', r, w)
    return w


def ecke_info(m, b, i):
    """-> (p, Ringradius, Rolle 'g'/'m') einer Ring-, Teilungs- oder Mittel-Ecke; None fuer Achs-Ecken."""
    nr = ring_zahl(b)
    if i in m.get('_r7b_mitte', {}):
        return m['_r7b_mitte'][i]
    rollen = ROLLEN.get(b, 'gg')
    if i < nr * S_:
        k, j = divmod(i, S_)
        return P_J[j], ring_radius(m, b, k), rollen[k]
    if i < nr * S_ + 2:
        return None
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
    e, k, j, t = best
    assert e <= 2.0, ('Teilungs-Ecke liegt auf keiner Ringsehne', b, i, e)
    return (1 - t) * P_J[j] + t * P_J[(j + 1) % S_], ring_radius(m, b, k), rollen[k]


def _ring_von(m, b, i):
    """Ring-Index einer Ring- oder Teilungs-Ecke (fuer mitte_teilen), None sonst."""
    nr = ring_zahl(b)
    if i < nr * S_:
        return max(i // S_, 1) if b == 16 else i // S_          # Mesh 16: Ring 0 = Ring 1 (Laenge 0)
    if i < nr * S_ + 2:
        return None
    q = np.array(m['v'][i][:3], float)
    for k in range(nr):
        for j in range(S_):
            a = np.array(m['v'][k * S_ + j][:3], float); c = np.array(m['v'][k * S_ + (j + 1) % S_][:3], float)
            d = c - a
            t = float(np.clip((q - a) @ d / max(d @ d, 1e-9), 0.0, 1.0))
            if np.linalg.norm(a + t * d - q) <= 2.0:
                return max(k, 1) if b == 16 else k
    return None


def mitte_teilen(ms, wurzel_keys):
    """Mittelring in den Baendern TEILEN: jede Flaeche (Viereck oder Dreieck des Mantels), die Ecken auf beiden Ringen des Bands hat, wird
    an den Mittelpunkten ihrer Laengskanten geteilt (gleiche Mittel-Ecke fuer beide Nachbarn einer Kante, keine T-Kreuzung). Ort und
    Normale gemittelt (Normale auf 4096), UV gemittelt; das erste Teilstueck ersetzt die Flaeche an ihrem Index, das zweite steht am Ende
    der Liste (Indizes der Wurzelflaechen von Mesh 16 - Marker S10s - bleiben). -> Zaehlung je Mesh."""
    zahl = {}
    for b, (ka, kb) in TEILEN.items():
        m = ms[b]
        ausn = wurzel_keys(m) if b == 16 else set()
        n0 = (len(m['v']), len(m['tf']), len(m['qf']))
        mitte = m.setdefault('_r7b_mitte', {})
        cache = {}
        rv = {}

        def ring(i):
            if i not in rv:
                rv[i] = _ring_von(m, b, i)
            return rv[i]

        def mid(i, j):
            key = (min(i, j), max(i, j))
            if key not in cache:
                pa = np.array(m['v'][i][:3], float); pc = np.array(m['v'][j][:3], float)
                na = np.array(m['n'][i][:3], float); nc = np.array(m['n'][j][:3], float)
                nn = na + nc; nn = nn / np.linalg.norm(nn) * 4096.0
                m['v'].append([int(round(float(x))) for x in 0.5 * (pa + pc)] + [0])
                m['n'].append([int(round(float(x))) for x in nn] + [0])
                ia, ic = ecke_info(m, b, i), ecke_info(m, b, j)
                mitte[len(m['v']) - 1] = (0.5 * (ia[0] + ic[0]), 0.5 * (ia[1] + ic[1]), 'm')
                cache[key] = len(m['v']) - 1
            return cache[key]

        def teile(poly):
            """poly = [(ecke, (u, v))] in Umlaufreihenfolge -> (Stueck Ring ka, Stueck Ring kb) als Umlauf-Listen, oder None."""
            seiten = [ring(i) for i, _uv in poly]
            if ka not in seiten or kb not in seiten or any(s not in (ka, kb) for s in seiten):
                return None
            out = []
            for k in range(len(poly)):
                i, uv = poly[k]; i2, uv2 = poly[(k + 1) % len(poly)]
                out.append((i, uv, seiten[k]))
                if seiten[k] != seiten[(k + 1) % len(poly)]:
                    out.append((mid(i, i2), ((uv[0] + uv2[0]) / 2.0, (uv[1] + uv2[1]) / 2.0), 'M'))
            # Stueck kb beginnt nach der ersten Mittel-Ecke, damit es im Umlauf zusammenhaengt
            k0 = [k for k, x in enumerate(out) if x[2] == 'M'][0]
            c = [(i, uv) for i, uv, s in out[k0:] + out[:k0] if s in (kb, 'M')]
            k1 = [k for k, x in enumerate(out) if x[2] == 'M'][1]
            a = [(i, uv) for i, uv, s in out[k1:] + out[:k1] if s in (ka, 'M')]
            return a, c

        def schreiben(stueck, alt_uv, ziel_idx, art):
            n = len(stueck)
            assert n in (3, 4), stueck
            if n == 4:
                stueck = [stueck[0], stueck[1], stueck[3], stueck[2]]    # Umlauf -> Viereck-Reihenfolge 0-1-2 / 1-3-2
            fn = []
            for i, _uv in stueck:
                fn += [i, i]
            if n == 3:
                un = [0] * 9; un[2], un[5], un[8] = alt_uv[2], alt_uv[5], alt_uv[8]
            else:
                un = [0] * 12; un[2], un[5], un[8] = alt_uv[2], alt_uv[5], alt_uv[8]
                un[11] = alt_uv[11] if len(alt_uv) == 12 else 0
            for k, (_i, (u_, v_)) in enumerate(stueck):
                un[3 * k], un[3 * k + 1] = int(round(u_)), int(round(v_))
            if ziel_idx is not None:
                (m['qf'] if art == 'v' else m['tf'])[ziel_idx] = fn
                (m['quv'] if art == 'v' else m['tuv'])[ziel_idx] = un
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
            r = teile(poly)
            if r is None:
                continue
            a, c = r
            alt = list(uv)
            schreiben(a, alt, qi if len(a) == 4 else None, 'v')
            if len(a) != 4:
                raise AssertionError(('Viereck-Stueck ist kein Viereck', b, qi))
            schreiben(c, alt, None, 'v')
        for di in range(n0[1]):
            if ('d%d' % di) in ausn:
                continue
            f, uv = m['tf'][di], m['tuv'][di]
            ie = list(f[1::2])
            if list(f[0::2]) != ie:
                continue
            poly = [(ie[k], (uv[3 * k], uv[3 * k + 1])) for k in range(3)]
            r = teile(poly)
            if r is None:
                continue
            a, c = r
            alt = list(uv)
            # erstes Dreieck-Stueck ersetzt die Flaeche, das Viereck-Stueck kommt an die Vierecke
            dr, vi = (a, c) if len(a) == 3 else (c, a)
            schreiben(dr, alt, di, 'd')
            schreiben(vi, alt, None, 'v')
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
        p, r, rolle = info
        out[k] = (UC + p * breite(r) / 2.0, V_B if rolle == 'm' else V_A)
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
        out[k] = (u, float(np.clip(v, V_A, V_B)))
    return [(int(round(u)), int(round(v))) for u, v in out]
