"""SF V8 Stufe a) Adern (W)-rechts aus dem Original-RE1.5-Birkin (Messblatt V8 Abschnitt 6c).

Nutzer 2026-11-08 (woertlich): "man sieht ebenfalls beim vorlage Bild, das die Rechte Seite auch mehr rottöne hat, also quasi mehr adern an
verschiedenen Stellen, am Arm und Körper, nicht nur grau. Übernehme die die Textur Sachen die du dafür brauchst vom originalen Birkin von
Resident Evil 1.5". Regel (Memory "Texturen nie aus Videobildern eichen", Nachtrag 2026-11-08): Adern = Original-Texel des EM30
(CDEMD0.EMS Typ 0x30) mit der Original-Palette der Zeile; keine gemalten Inseln, keine Eichung je Fleck/Region am Video.

Quelle: Original EM30 = birkin_formen.lade_em30() (CDEMD0.EMS Blob 16), TIM 384 x 256, 8 bpp, CLUT-Zeilen 480-482.
Zeilen (Messblatt 6c):
  X1 = V7-Zeile 480 (graue Haut V5) + jeder Eintrag der Original-Zeile 480 mit Paletten-a* >= 15 und L* <= 38, bytegleich (15 Bit + STP)
  X2 = V7-Zeile 481 + die entsprechenden Eintraege der Original-Zeile 481
  angehaengt als Zeilen 480 + n und 481 + n (n = Zeilen der Basis, V7: 13 -> 493 / 494; 15 Zeilen, Port-Texturhoehe 3840 <= 4096).
  Lab wie Messblatt (m15/m17): LUT = Kanal x 8 (b2.modell), v4_gemein.lab.
Flaechen (adern_v8.json, aus adern_suche.py):
  Mesh 13 (Oberarm (W)-rechts, Knochen 13 relpos z -1136): alle Flaechen auf Zeile 480 -> X1.
  Mesh 8: die in adern_v8.json genannten Flaechen -> X1 (Zeile 480) bzw. X2 (481), UV formgleich verschoben (je UV-Insel ein Schub
  (du, dv) auf ein aderreiches Fenster derselben Textur-Seite und Zeile; Ziel-Texel = Original-Texel, Riegel S7a).
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


def zeilen_neu(t_clut):
    _o, _ms, to = original()
    X = []
    for k in (0, 1):
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


def adern(ms, t, par, prot):
    n = int(t['clut'].shape[0])
    (x1, n1), (x2, n2) = zeilen_neu(t['clut'])
    t['clut'] = np.vstack([t['clut'], x1[None], x2[None]])
    z1, z2 = 480 + n, 481 + n
    c13 = 0
    for uv in ms[13]['tuv'] + ms[13]['quv']:
        if (uv[2] >> 6) & 0x1FF == 480:
            setze_zeile(uv, z1); c13 += 1
    c8 = []
    for e in par['mesh8']:
        uv = flaeche(ms[8], e['f'])
        alt = (uv[2] >> 6) & 0x1FF
        setze_zeile(uv, z1 if alt == 480 else z2)
        if e.get('du') or e.get('dv'):
            schiebe(uv, int(e.get('du', 0)), int(e.get('dv', 0)))
        c8.append(e['f'])
    prot['adern'] = dict(zeile_x1=z1, zeile_x2=z2, ader_eintraege_480=n1, ader_eintraege_481=n2, mesh13_flaechen=c13,
                         mesh8_flaechen=c8, quelle=par.get('quelle'))


def herkunft(blob, ref_blob):
    """Riegel S7a: (1) jeder Eintrag der Zeilen ab Basis-Zeilenzahl = V7-Zeile 480/481 (gleicher Index) oder Original-Eintrag (a* >= 15,
    L* <= 38) derselben Original-Zeile, 15 Bit gleich; (2) alle uebrigen Zeilen und alle Texel bytegleich V7 ausser Mesh 9 (S5) und dem
    Schwanz; (3) jeder Texel unter einer Flaeche, deren UV sich gegen V7 geaendert hat (Mesh 8/13), ist ein Original-Texel."""
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, timr = KB.teile(ref_blob)
    t, tr = KB.tim_teile(tim), KB.tim_teile(timr)
    _oo, _om, to = original()
    nr = tr['clut'].shape[0]
    zeilen_ok = bool(np.array_equal(t['clut'][:nr], tr['clut']))
    neu_ok, n_ader = True, 0
    for k, z in enumerate(t['clut'][nr:]):
        if k > 1:
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
    for b in (8, 13):
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
