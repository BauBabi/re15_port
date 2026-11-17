"""SF V8 Runde 7e, Stufe f (Kette v8_bau.py --stufen ...pf): Handfarbe der linken Hand "wie in der Vorlage" - grauer Handruecken und
graues unteres Unterarm-Drittel, rote Fingerkrallen.

    python handfarbe_r7e.py <eingang.emd> <ausgang.emd>
    python handfarbe_r7e.py suche [<eingang.emd>]          (rechnet MASSSTAB / SCHUB nach; Ausgabe = Beleg der Konstanten)

Nutzerentscheid 2026-11-14 (woertlich, auf die Frage "dunkelrot-violett wie jetzt oder grau mit roten Spitzen wie auf Ihrer Vorlage?"):
"wie in der Vorlage." Vorlage = Version_80%/birkin20-24.png (nur gelesen): grauer Handruecken, nur die Finger/Krallen rot
(Sicht-Pruefer Runde 7, analysis/befunde_runde41/SF_PRUEFUNG_V8_r7_sicht.md (4b)/(4c)).

Gemessen vor dem Bau (SF_belege/v8/runde7e/messung_vorher.txt; riegel_handfarbe.py am Stand 5c902d49, echte Framedumps F368-F375):
  Handruecken 34-36 % rote Bildpunkte (a* >= 10; Vorlage 0,1-2,4 %), 15 % violett (b* < -5); Ursache: Mesh 12 liegt mit 34 Flaechen auf
  Zeile 490 = Referenz-2-Zeile 482 OHNE die V5-Grau-Regel (V5 wie Foto b: "im Foto rote Handflaeche"), Texel u 4-39 / v 51-86 (Seite 2).
  Unterarm unteres Drittel 0 % rot, aber L* 6,8-7,2 gegen 24-25 oben: die 8 Flaechen von Mesh 11, die das untere Drittel beruehren, lesen
  die dunkle Handgelenk-/Handzone der Textur (v 25-87, Zeile 482 L* 8,5-13,6), die 17 uebrigen die graue Unterarmzone (v 0-56).

Was die Stufe tut (NUR das UV-/CLUT-Feld der Flaechen von Mesh 11 und Mesh 12; Ecken, Normalen, Flaechen, Knochen, Clips, Texturbild und
alle CLUT-Zeilen bleiben bytegleich, ebenso alle anderen Meshes, Mesh 9 = V7-Kopf):
  Gruppe = die 34 Flaechen der Hand (Mesh 12 ohne die 24 Krallenflaechen: Handruecken, Daumenballen, Kastenfinger mit Kappe v16 - V7
  linke Hand: "Kappe v16 (Kastenfinger, keine Kralle) -> 490 wie die Hand") UND die 8 Flaechen von Mesh 11 mit einer Ecke im unteren
  Drittel der Unterarmlaenge (y > y_min + 2/3 (y_max - y_min); gemessen d0 d1 d16 d17 v1 v2 v3 v4).
  1. CLUT-Feld der Gruppe -> Zeile 482 = die Zeile, auf der alle 25 Flaechen des Unterarms Mesh 11 liegen (Referenz-2-Zeile 482 mit der
     V5-Grau-Regel: Eintraege mit Paletten-a* >= 10 grau). Krallen bleiben: Daumenkralle 491 (grau), Fingerkrallen 492 (rot).
  2. Je Teilgruppe EINE UV-Abbildung (gleicher Massstab in u und v, ein Schub; Teilgruppen "hand" = die 34 Flaechen von Mesh 12,
     "unterarm" = die 8 Flaechen von Mesh 11):
        u' = floor((M * (u - U0) + 100 * SU + 50) / 100),  v' = floor((M * (v - V0) + 100 * SV + 50) / 100)      (ganzzahlig)
     (U0, V0) = kleinstes u / v der Teilgruppe. Die Abbildung legt die Texel der Teilgruppe (Lage zueinander wie bei Capcom) in die
     GRAU-ZONE N = die Texel, die die 17 Flaechen des uebrigen Unterarms (Mesh 11 in den oberen zwei Dritteln, Original-EM30-UV =
     Asset-UV) benutzen (1462 Texel, Seite 2, u 5-38, v 0-56). Gewaehlt nach Geometrie, nicht nach Farbe: der groesste Massstab M/100, bei
     dem KEIN Texel unter einer Flaeche der Teilgruppe ausserhalb N liegt (KB.raster wie Riegel S7 a); gibt es dafuer mehrere Schuebe,
     der kleinste (SU, SV) in dieser Reihenfolge. Ausgabe von "suche" = Beleg (SF_belege/v8/runde7e/suche.txt).
     Verworfen (Messung runde7e/kandidaten.txt): K1b nur CLUT-Feld (Hand auf eigenen Texeln, Zeile 482) - Hand fast schwarz (L* 8,4,
     Perzentile gegen die Vorlage 0,388 / 0,325 statt 0,278 / 0,190); eine gemeinsame Abbildung fuer beide Teilgruppen (groesster
     Massstab nur 0,69 statt 0,92 / 0,82 = gestreckte Texel).
  Damit stammt jeder Texel unter der Gruppe aus der Original-Textur und wird schon von Original-Flaechen derselben Zeile (Unterarm) benutzt;
  keine neue Farbe, keine Eichung an Video oder Nutzerbild (E4). Die Grenze grau/rot liegt am Uebergang Hand -> Fingerkrallen.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. PORT-WAHL (Nutzerentscheid oben;
Zahlen aus der Messung und der Suche).
"""
import os
import sys
import hashlib
import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402,F401  (Pfade fuer kopf_orig_bau / birkin_formen)
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402
import hand_r6 as H6                  # noqa: E402

UNTERARM, HAND = 11, 12
ZEILE_GRAU = 482                      # CLUT-Zeile aller 25 Flaechen von Mesh 11 (Asset 5c902d49 und V7, gemessen runde7e/messung_vorher.txt)
ZEILE_HAND_ALT = 490                  # Eingang: die 34 Handflaechen von Mesh 12 (riegel_hand S17 f, Runde 7d)
SEITE = 2                             # Textur-Seite von Mesh 11 und 12 (alle Flaechen, Asset und Original)
DRITTEL = 2.0 / 3.0                   # "unteres Unterarm-Drittel" (Sicht-Pruefer r7 (4b), Auftrag Runde 7e)
KASTEN = frozenset(range(32, 40))     # Ecken des Kastenfingers (Flaechen v15-v24 von Mesh 12), keine Kralle
INNEN = {'T': 22, 'A': 49, 'B': 31, 'C': 44}   # innere Spitze der Doppelpyramide je Kralle (riegel_hand.zeilen_pruefen)

# Ergebnis von "suche" am Eingang 5c902d49 (= Stand vor dieser Stufe; UV von Mesh 11/12 seit V7 = Original-EM30), Beleg
# SF_belege/v8/runde7e/suche.txt: je Teilgruppe genau EIN gueltiger Schub beim groessten Massstab.
ABBILDUNG = dict(hand=(92, 4, 51, 6, 6),                  # (M, U0, V0, SU, SV): Mesh 12, 34 Flaechen, u 4-39 v 51-86 -> u 6-38 v 6-38
                 unterarm=(82, 0, 25, 5, 3))              # Mesh 11, 8 Flaechen, u 0-38 v 25-87 -> u 5-36 v 3-54


def klasse12(f):
    """Flaeche von Mesh 12 -> 'daumenkralle' (T), 'finger' (Kralle A/B/C), 'kastenfinger' oder 'hand'."""
    ecken = set(f[1::2])
    for n in ('T', 'A', 'B', 'C'):
        sp, ring = H6.KRALLEN[n]
        if ecken <= ({sp, INNEN[n]} | set(ring)) and ecken & set(ring):
            return 'daumenkralle' if n == 'T' else 'finger'
    return 'kastenfinger' if ecken & KASTEN else 'hand'


def _flaechen(m):
    for F, U, k in (('tf', 'tuv', 'd'), ('qf', 'quv', 'v')):
        for i, f in enumerate(m[F]):
            yield '%s%d' % (k, i), f, m[U][i]


def unterarm_unten(m11):
    """Schluessel der Flaechen von Mesh 11 mit einer Ecke im unteren Drittel der Unterarmlaenge (y waechst zur Hand hin)."""
    ys = [v[1] for v in m11['v']]
    grenze = min(ys) + DRITTEL * (max(ys) - min(ys))
    return [k for k, f, _uv in _flaechen(m11) if max(m11['v'][j][1] for j in f[1::2]) > grenze]


def gruppe(ms):
    """{Teilgruppe: [(Mesh, Schluessel, uv)]}: 'hand' = Mesh 12 ohne Krallen, 'unterarm' = Mesh 11 unteres Drittel."""
    unten = set(unterarm_unten(ms[UNTERARM]))
    return dict(hand=[(HAND, k, uv) for k, f, uv in _flaechen(ms[HAND]) if klasse12(f) in ('hand', 'kastenfinger')],
                unterarm=[(UNTERARM, k, uv) for k, f, uv in _flaechen(ms[UNTERARM]) if k in unten])


def _n(uv):
    return 3 if len(uv) == 9 else 4


def abbilden(uv, m=None, u0=None, v0=None, su=None, sv=None):
    m, u0, v0, su, sv = (M if m is None else m), (U0 if u0 is None else u0), (V0 if v0 is None else v0), (SU if su is None else su), \
        (SV if sv is None else sv)
    out = list(uv)
    for j in range(_n(uv)):
        out[3 * j] = (m * (uv[3 * j] - u0) + 100 * su + 50) // 100
        out[3 * j + 1] = (m * (uv[3 * j + 1] - v0) + 100 * sv + 50) // 100
    return out


def grau_zone(ms_orig=None):
    """Texel (Seite 2, Bild 256 x 384) der Flaechen von Mesh 11 in den oberen zwei Dritteln, Original-EM30 (UV und Ecken)."""
    ms_orig = ms_orig or KB.teile(BF.lade_em30())[2]
    m11 = ms_orig[UNTERARM]
    unten = set(unterarm_unten(m11))
    N = np.zeros((256, 384), bool)
    for k, f, uv in _flaechen(m11):
        if k not in unten:
            N |= KB.raster(uv, _n(uv), (uv[5] & 0xF) * 128)
    return N


def texel(uvs):
    T = np.zeros((256, 384), bool)
    for uv in uvs:
        T |= KB.raster(uv, _n(uv), (uv[5] & 0xF) * 128)
    return T


def suche(blob, log=print):
    """Je Teilgruppe: groesster Massstab M/100 (100 .. 50), bei dem die Teilgruppe nach abbilden() 0 Texel ausserhalb N hat; Schub (SU, SV)
    aufsteigend, der erste gueltige gilt. Ursprung (U0, V0) = kleinstes u / v der Teilgruppe."""
    g = gruppe(KB.teile(blob)[2])
    N = grau_zone()
    ys, xs = np.nonzero(N)
    log('Grau-Zone N: %d Texel, u %d-%d, v %d-%d' % (int(N.sum()), xs.min() - 256, xs.max() - 256, ys.min(), ys.max()))
    out = {}
    for name in ('hand', 'unterarm'):
        uvs = [uv for _b, _k, uv in g[name]]
        u0 = min(uv[3 * j] for uv in uvs for j in range(_n(uv)))
        v0 = min(uv[3 * j + 1] for uv in uvs for j in range(_n(uv)))
        log('%s: %d Flaechen (%s), UV u %d-%d v %d-%d' % (name, len(uvs), ' '.join(k for _b, k, _u in g[name]), u0,
            max(uv[3 * j] for uv in uvs for j in range(_n(uv))), v0, max(uv[3 * j + 1] for uv in uvs for j in range(_n(uv)))))
        for m in range(100, 49, -1):
            ok = []
            for su in range(int(xs.min()) - 256, int(xs.max()) - 256 + 1):
                for sv in range(int(ys.min()), int(ys.max()) + 1):
                    if not (texel([abbilden(uv, m, u0, v0, su, sv) for uv in uvs]) & ~N).any():
                        ok.append((su, sv))
            if ok:
                log('   groesster Massstab M = %d/100, gueltige Schuebe (SU, SV): %s -> gewaehlt %s' % (m, ok, ok[0]))
                out[name] = (m, u0, v0, ok[0][0], ok[0][1])
                break
    return out


def stufe(blob):
    """Eingang -> Ausgang (bytes). Prueft den Eingang (Zeilen, Gruppe) und das Ergebnis (0 Texel ausserhalb N)."""
    d, _o, ms, tim = KB.teile(blob)
    g = gruppe(ms)
    assert (len(g['hand']), len(g['unterarm'])) == (34, 8), 'Gruppe %d / %d statt 34 / 8' % (len(g['hand']), len(g['unterarm']))
    neu = []
    for name in ('hand', 'unterarm'):
        for b, k, uv in g[name]:
            z = (uv[2] >> 6) & 0x1FF
            assert z == (ZEILE_GRAU if b == UNTERARM else ZEILE_HAND_ALT), 'Mesh %d %s Zeile %d' % (b, k, z)
            assert (uv[5] & 0xF) == SEITE, 'Mesh %d %s Seite %d' % (b, k, uv[5] & 0xF)
            uv2 = abbilden(uv, *ABBILDUNG[name])
            assert all(0 <= x <= 255 for j in range(_n(uv)) for x in (uv2[3 * j], uv2[3 * j + 1]))
            uv2[2] = (ZEILE_GRAU << 6) | (uv[2] & 0x3F)
            uv[:] = uv2
            neu.append(uv2)
    fremd = int((texel(neu) & ~grau_zone()).sum())
    assert fremd == 0, '%d Texel ausserhalb der Grau-Zone' % fremd
    return KB.packen(blob, d, ms, tim)


def main():
    if sys.argv[1] == 'suche':
        blob = open(sys.argv[2] if len(sys.argv) > 2 else V.ASSET, 'rb').read()
        print('Eingang sha256', hashlib.sha256(blob).hexdigest())
        print(suche(blob))
        return
    blob = open(sys.argv[1], 'rb').read()
    neu = stufe(blob)
    open(sys.argv[2], 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s)' % (sys.argv[2], len(neu), hashlib.sha256(neu).hexdigest()))


if __name__ == '__main__':
    main()
