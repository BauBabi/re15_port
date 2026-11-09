"""SF V8 Stufe a) Adern (W)-rechts aus dem Original-RE1.5-Birkin (Messblatt V8 Abschnitt 6c; Runde 2 nach Pruefung V8 r1).

Nutzer 2026-11-08 (woertlich): "man sieht ebenfalls beim vorlage Bild, das die Rechte Seite auch mehr rottöne hat, also quasi mehr adern an
verschiedenen Stellen, am Arm und Körper, nicht nur grau. Übernehme die die Textur Sachen die du dafür brauchst vom originalen Birkin von
Resident Evil 1.5". Regel (Memory "Texturen nie aus Videobildern eichen", Nachtrag 2026-11-08): Adern = Original-Texel des EM30
(CDEMD0.EMS Typ 0x30) mit der Original-Palette der Zeile; keine gemalten Inseln, keine Eichung je Fleck/Region am Video.

Quelle: Original EM30 = birkin_formen.lade_em30() (CDEMD0.EMS Blob 16), TIM 384 x 256, 8 bpp, CLUT-Zeilen 480-482.
Zeilen (Messblatt 6c; Runde 2 dazu X3 fuer den Unterarm-Wulst):
  Xk = V7-Zeile 480 + k (k = 0, 1, 2) + jeder Eintrag der Original-Zeile 480 + k mit Paletten-a* >= 15 und L* <= 38, bytegleich
  angehaengt als Zeilen 480 + n + k (n = Zeilen der Basis, V7: 13 -> 493 / 494 / 495; 16 Zeilen, Port-Texturhoehe 4096 <= 4096).
  Lab wie Messblatt (m15/m17): LUT = Kanal x 8 (b2.modell), v4_gemein.lab.
Flaechen (adern_v8.json, aus adern_suche.py; Runde 2): GANZE UV-Inseln von V7 (alle Flaechen einer Zeile und Textur-Seite, die eine
  UV-Ecke teilen) wechseln die Zeile, UV unveraendert - keine neuen Naehte. Runde 1 hatte 9 Teil-Inseln mit eigenem UV-Schub; Sicht-
  Pruefer V8 r1: "Flickenteppich: Die Streifen enden hart an den Dreieckskanten, gleiche V-Adern wiederholen sich, zwischen den 9
  UV-Inseln laufen Naehte", "Der helle Oberarm-Wulst und die untere Flanke sind unveraendert grau".
Texel und alle anderen Zeilen bleiben bytegleich. Kein RE-Wert; PORT-WAHL.
"""
import os, sys
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402
import v4_gemein as G                 # noqa: E402

ADER_A, ADER_L = 15.0, 38.0           # Messblatt 6c


def original():
    o = BF.lade_em30()
    _d, _o, ms, tim = KB.teile(o)
    return o, ms, KB.tim_teile(tim)


def lab15(z):
    z = np.asarray(z, np.int64)
    rgb = np.stack([(z & 31) << 3, ((z >> 5) & 31) << 3, ((z >> 10) & 31) << 3], -1).astype(float)
    return G.lab(rgb.reshape(-1, 1, 3)).reshape(-1, 3)


def ader_eintraege(zeile15):
    L = lab15(zeile15)
    return (L[:, 1] >= ADER_A) & (L[:, 0] <= ADER_L) & (np.asarray(zeile15) != 0)


def zeilen_neu(t_clut, k_liste=(0, 1)):
    _o, _ms, to = original()
    X = []
    for k in k_liste:
        z = t_clut[k].copy()
        a = ader_eintraege(to['clut'][k])
        z[a] = to['clut'][k][a]
        X.append((z, int(a.sum())))
    return X


def setze_zeile(uv, z):
    uv[2] = (z << 6) | (uv[2] & 0x3F)


def schiebe(uv, du, dv):
    n = 3 if len(uv) == 9 else 4
    for k in range(n):
        uv[3 * k] += du
        uv[3 * k + 1] += dv
        assert 0 <= uv[3 * k] <= 255 and 0 <= uv[3 * k + 1] <= 255, uv


def flaeche(m, key):
    i = int(key[1:])
    return m['tuv'][i] if key[0] == 'd' else m['quv'][i]


ORIGINAL_ZEILE = {480: 0, 481: 1, 482: 2, 489: 0}   # Runde 3: V7-Zeile -> Original-Zeile 480 + k (489 = Kopie der Referenz-2-Zeile 480, V5)


def adern_original(ms, t, par, prot):
    """Runde 3 (O1/O2, adern_r3.py): Zeilen Xk = Original-EM30-Zeile 480 + k bytegleich; jede genannte Flaeche zeigt auf X(Original-Zeile
    ihrer V7-Zeile), UV bleibt; Huegel-Flaechen: UV-Schub (ganze Flaeche) auf Original-Fleisch, Zeile X0."""
    n = int(t['clut'].shape[0])
    _o, _ms, to = original()
    ks = [int(k) for k in par.get('zeilen', (0, 1, 2))]
    t['clut'] = np.vstack([t['clut']] + [to['clut'][k][None].copy() for k in ks])
    ziel = {z: 480 + n + ks.index(k) for z, k in ORIGINAL_ZEILE.items() if k in ks}
    zahl = {}
    for b, keys in sorted(par['flaechen'].items(), key=lambda kv: int(kv[0])):
        for key in keys:
            uv = flaeche(ms[int(b)], key)
            alt = (uv[2] >> 6) & 0x1FF
            assert alt in ziel, (b, key, alt)
            setze_zeile(uv, ziel[alt])
            zahl[(int(b), ziel[alt])] = zahl.get((int(b), ziel[alt]), 0) + 1
    gr = []
    for g in par.get('gruppen', []):                 # Runde 4 (adern_r4.py): ganze UV-Gruppe, Skalierung um die kleinste Ecke, dann Schub
        uvs = [flaeche(ms[int(g['mesh'])], k) for k in g['flaechen']]
        u0 = min(uv[3 * i] for uv in uvs for i in range(3 if len(uv) == 9 else 4))
        v0 = min(uv[3 * i + 1] for uv in uvs for i in range(3 if len(uv) == 9 else 4))
        sk = float(g.get('skal', 1.0))
        for uv in uvs:
            alt = (uv[2] >> 6) & 0x1FF
            assert alt in ziel, (g['name'], alt)
            for i in range(3 if len(uv) == 9 else 4):
                uv[3 * i] = int(round(u0 + (uv[3 * i] - u0) * sk)) + int(g['schub'][0])
                uv[3 * i + 1] = int(round(v0 + (uv[3 * i + 1] - v0) * sk)) + int(g['schub'][1])
                assert 0 <= uv[3 * i] <= 127 and 0 <= uv[3 * i + 1] <= 255, (g['name'], uv)
            setze_zeile(uv, ziel[alt])
            zahl[(int(g['mesh']), ziel[alt])] = zahl.get((int(g['mesh']), ziel[alt]), 0) + 1
        gr.append('%s:%d' % (g['name'], len(uvs)))
    hg = []
    for h in par.get('huegel', []):
        uv = flaeche(ms[8], h['flaeche'])
        schiebe(uv, int(h['schub'][0]), int(h['schub'][1]))
        setze_zeile(uv, ziel[480])
        hg.append(h['flaeche'])
    prot['adern'] = dict(modus=par.get('modus', 'original'), zeilen={str(480 + k): 480 + n + i for i, k in enumerate(ks)},
                         flaechen={'%d->%d' % kz: c for kz, c in sorted(zahl.items())}, huegel=hg, gruppen=gr, quelle=par.get('quelle'))


def adern(ms, t, par, prot):
    """Runde 2: par = {'zeilen': [0, 1, 2], 'flaechen': {'<mesh>': ['d0', 'v3', ...]}} - jede genannte Flaeche wechselt von Zeile
    480 + k auf Xk = 480 + n + Index von k in 'zeilen'; UV bleibt."""
    if par.get('modus') in ('original', 'original_r4'):
        return adern_original(ms, t, par, prot)
    n = int(t['clut'].shape[0])
    ks = [int(k) for k in par.get('zeilen', (0, 1))]
    X = zeilen_neu(t['clut'], ks)
    t['clut'] = np.vstack([t['clut']] + [x[None] for x, _n in X])
    ziel = {480 + k: 480 + n + i for i, k in enumerate(ks)}
    zahl = {}
    for b, keys in sorted(par['flaechen'].items(), key=lambda kv: int(kv[0])):
        for key in keys:
            uv = flaeche(ms[int(b)], key)
            alt = (uv[2] >> 6) & 0x1FF
            assert alt in ziel, (b, key, alt)
            setze_zeile(uv, ziel[alt])
            zahl[(int(b), ziel[alt])] = zahl.get((int(b), ziel[alt]), 0) + 1
    prot['adern'] = dict(zeilen={str(480 + k): 480 + n + i for i, k in enumerate(ks)},
                         ader_eintraege={str(480 + k): nn for k, (_x, nn) in zip(ks, X)},
                         flaechen={'%d->%d' % kz: c for kz, c in sorted(zahl.items())}, quelle=par.get('quelle'))


def herkunft_r3(blob, ref_blob):
    """Riegel S7a Runde 3 (O1: "jeder Eintrag und jeder Texel stammt aus dem Original-EM30"): (1) Zeilen 480 .. 480 + n(V7) - 1 bytegleich V7;
    (2) jede neue Zeile 480 + n(V7) + k = Original-EM30-Zeile 480 + k bytegleich (15 Bit), hoechstens 3; (3) alle Texel bytegleich V7;
    (4) jede Flaeche der Meshes 8/13/14 auf einer neuen Zeile: alle Texel unter ihr = Original-Texel; (5) Flaechen mit geaenderter UV nur
    auf Mesh 8 und nicht in Feld C (Seite 0, u 78-127, v 0-100); (6) keine Flaeche der Meshes 8/13 aus Feld C auf einer neuen Zeile."""
    import adern_r3 as A3
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, timr = KB.teile(ref_blob)
    t, tr = KB.tim_teile(tim), KB.tim_teile(timr)
    _oo, _om, to = original()
    nr = tr['clut'].shape[0]
    zeilen_ok = bool(np.array_equal(t['clut'][:nr], tr['clut']))
    neu = t['clut'][nr:]
    neu_ok = len(neu) <= 3 and all(np.array_equal(neu[k] & 0x7FFF, to['clut'][k] & 0x7FFF) for k in range(len(neu)))
    pix_ok = bool(np.array_equal(t['pix'], tr['pix']))
    tex_ok, n_tex, n_fl, uv_anders, c_fehler = True, 0, 0, 0, 0
    for b in (8, 13, 14):
        for key, uv in [('d%d' % i, u) for i, u in enumerate(ms[b]['tuv'])] + [('v%d' % i, u) for i, u in enumerate(ms[b]['quv'])]:
            ur = flaeche(mr[b], key)
            anders = [uv[i] for i in range(len(uv)) if i not in (2, 5) and i % 3 != 2] != [ur[i] for i in range(len(ur)) if i not in (2, 5) and i % 3 != 2]
            if anders:
                uv_anders += 1
                if b != 8 or A3.in_feld_c(uv):
                    c_fehler += 1
            if (uv[2] >> 6) & 0x1FF < 480 + nr:
                continue
            n_fl += 1
            if b in (8, 13) and A3.in_feld_c(uv):
                c_fehler += 1
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            n_tex += int(mk.sum())
            if not np.array_equal(t['pix'][mk], to['pix'][mk]):
                tex_ok = False
    return dict(alte_zeilen_gleich=zeilen_ok, neue_zeilen_original=bool(neu_ok), neue_zeilen=int(len(neu)), texel_bytegleich_v7=pix_ok,
                flaechen_neue_zeilen=n_fl, texel_unter_neuen=n_tex, texel_original=tex_ok, uv_geaenderte_flaechen=uv_anders,
                feld_c_fehler=c_fehler, ok=bool(zeilen_ok and neu_ok and pix_ok and tex_ok and c_fehler == 0))


def herkunft(blob, ref_blob):
    """Riegel S7a: (1) jeder Eintrag der Zeilen ab Basis-Zeilenzahl = V7-Zeile 480/481 (gleicher Index) oder Original-Eintrag (a* >= 15,
    L* <= 38) derselben Original-Zeile, 15 Bit gleich; (2) alle uebrigen Zeilen und alle Texel bytegleich V7 ausser Mesh 9 (S5) und dem
    Schwanz; (3) jeder Texel unter einer Flaeche, deren UV sich gegen V7 geaendert hat (Mesh 8/13/14), ist ein Original-Texel."""
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, timr = KB.teile(ref_blob)
    t, tr = KB.tim_teile(tim), KB.tim_teile(timr)
    _oo, _om, to = original()
    nr = tr['clut'].shape[0]
    zeilen_ok = bool(np.array_equal(t['clut'][:nr], tr['clut']))
    neu_ok, n_ader = True, 0
    for k, z in enumerate(t['clut'][nr:]):
        if k > 2:                                   # Runde 2: X1/X2/X3 (Zeilen 480/481/482)
            neu_ok = False
            break
        a = ader_eintraege(to['clut'][k])
        for i in range(len(z)):
            if z[i] == tr['clut'][k][i]:
                continue
            if a[i] and (int(z[i]) & 0x7FFF) == (int(to['clut'][k][i]) & 0x7FFF):
                n_ader += 1
                continue
            neu_ok = False
    pix_ok = bool(np.array_equal(t['pix'], tr['pix']))
    tex_ok, n_tex, geaendert = True, 0, 0
    for b in (8, 13, 14):
        for key, uv in [('d%d' % i, u) for i, u in enumerate(ms[b]['tuv'])] + [('v%d' % i, u) for i, u in enumerate(ms[b]['quv'])]:
            ur = flaeche(mr[b], key)
            if [uv[0], uv[1], uv[3], uv[4], uv[6], uv[7]] + ([uv[9], uv[10]] if len(uv) > 9 else []) == \
               [ur[0], ur[1], ur[3], ur[4], ur[6], ur[7]] + ([ur[9], ur[10]] if len(ur) > 9 else []):
                continue
            geaendert += 1
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            n_tex += int(mk.sum())
            if not np.array_equal(t['pix'][mk], to['pix'][mk]):
                tex_ok = False
    return dict(alte_zeilen_gleich=zeilen_ok, neue_zeilen_herkunft=neu_ok, ader_eintraege=n_ader, texel_bytegleich_v7=pix_ok,
                uv_geaenderte_flaechen=geaendert, texel_unter_geaenderten=n_tex, texel_original=tex_ok,
                ok=zeilen_ok and neu_ok and tex_ok)
