"""SF V8 Runde 4: Messung in ECHTEN Framedumps des Port-Renderers mit Marker-Assets (Verfahren wie Zahlen-Pruefer r3).

    python szene_marker.py bauen EMD ZIELORDNER          -> MK_A.EMD, MK_B.EMD, GR.EMD
    python szene_marker.py wurzel DIR_MK_A DIR_MK_B [F0 F1]   (Standard F328 F346; Framedump-Nummern)
    python szene_marker.py schlitz DIR_GR DIR_GR_V7 [F0 F1]   (Standard F344 F378)

Zahlen-Pruefer r3 (woertlich): "MK_A: CLUT-Zeile 487 (nur Schwanz) magenta. MK_B: wie MK_A, dazu die 22 Wurzelflaechen von Mesh 16 (Band
Ring 0 -> Ring 1 und Kegel) auf Zeile 480 ... Wurzel sichtbar = magenta in A und nicht in B. Das ist eine echte z-Test-Zaehlung des
Port-Renderers." und "GR_*: alle Zeilen gruen, fuer die Silhouette". Laeufe: v4/lauf_v4.sh mit EMD-Overlay (Spiel-Asset bleibt).
Schlitz: nicht-gruene Bildpunkte mit Gruen links und rechts (je <= 4 px, gleiche Zeile) und darunter (<= 4 px) - eine schmale Kerbe in
der Figur, nur in den obersten OBEN_PX Zeilen der Figur (Zahlen-Pruefer r3: "links neben dem Kopf"; darunter erzeugt der anders
haengende Schwanz zwischen Bein und Hand schmale Luecken, Lauf sfv8/r4 F352-F358 y 442-477); V7 im selben Bild zum Vergleich.
Kein RE-Wert.
"""
import os, sys, glob
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402

MAGENTA = 0x7C1F
OBEN_PX = 120                         # Schlitz-Fenster: oberste 120 Bildzeilen der Figur (960 x 720; Kopf und Ruecken in F344-F378)
GRUEN = 0x03E0


def wurzel_flaechen(m):
    # Runde 6b: die an der Grenze 110 / 120 geteilten Flaechen (unterseite_r5.teilen) haengen ihre neuen Ecken hinter die 35 Ecken des
    # Glieds (3 Ringe zu 11 + 2 Achs-Ecken); sie liegen auf Ring 1 / Ring 2 des Bands Ring 1 -> Ring 2 und der Deckel, nie in der Wurzel
    # (gemessen an den Assets 382bbfe7 und 95a783a6: ohne diese Zeile 23 statt 22 Wurzelflaechen). Die Liste ist auch Eingabe des Baus
    # (unterseite_r5.wurzel_keys): seit dieser Zeile ist das Bandstueck v22 von Mesh 16 (Ecken 35, 37, 36, 38) keine Wurzelflaeche mehr
    # und liegt wie die uebrige Unterseite auf der Fleisch-Zeile 495 (Asset 382bbfe7: noch Zeile 487; ab 95a783a6: Zeile 495)
    ring2 = set(range(22, 34)) | set(range(35, len(m['v'])))
    return (['d%d' % i for i, f in enumerate(m['tf']) if not (set(f[1::2]) & ring2)] +
            ['v%d' % i for i, f in enumerate(m['qf']) if not (set(f[1::2]) & ring2)])


def bauen(emd, ziel):
    blob = open(emd, 'rb').read()
    os.makedirs(ziel, exist_ok=True)
    for name in ('MK_A', 'MK_B', 'GR'):
        d, _o, ms, tim = KB.teile(blob)
        t = KB.tim_teile(tim)
        c = t['clut']
        if name == 'GR':
            c[:] = np.where(c == 0, 0, (c & 0x8000) | GRUEN)
        else:
            z = 487 - 480
            c[z] = np.where(c[z] == 0, 0, (c[z] & 0x8000) | MAGENTA)
            # Runde 7b (K2): die Wurzelflaechen liegen bis Runde 7a alle auf Zeile 487 (dann aendert diese Zeile nichts); seit 7b tragen die 7
            # Kegel-Dreiecke unter der Grenze die Fleisch-Zeile - im Marker kommen sie auf 487 (magenta), damit S10s sie weiter zaehlt
            for k in wurzel_flaechen(ms[16]):
                AV.setze_zeile(AV.flaeche(ms[16], k), 487)
            if name == 'MK_B':
                for k in wurzel_flaechen(ms[16]):
                    AV.setze_zeile(AV.flaeche(ms[16], k), 480)
        open(os.path.join(ziel, name + '.EMD'), 'wb').write(KB.packen(blob, d, ms, KB.tim_packen(t)))
    print('MK_A/MK_B/GR in', ziel, 'Wurzelflaechen', len(wurzel_flaechen(KB.teile(blob)[2][16])))


def bauen_ko(emd, ziel_datei):
    """Runde 6b (S16 b/c an den Szenenbildern): Marker-Asset KO = <emd> mit magenta Gesicht. Gesichtsflaechen von Mesh 9 (Schwerpunkt lokales
    x >= 150 = ruecken_r5.GESICHT_X) bleiben auf ihrer CLUT-Zeile 486, die deckend magenta wird; die uebrigen Flaechen von Mesh 9 (Hinterkopf)
    wandern auf Zeile 480. Ein Bildpunkt, der im V7-Lauf magenta ist und im Lauf des Prueflings nicht, ist ein verdeckter Gesichts-Bildpunkt."""
    blob = open(emd, 'rb').read()
    d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    c = t['clut']
    for b, m in enumerate(ms):
        if b != 9:
            assert not any((uv[2] >> 6) & 0x1FF == 486 for uv in list(m['tuv']) + list(m['quv'])), ('Zeile 486 ausserhalb von Mesh 9', b)
    m = ms[9]
    ng = nh = 0
    for kind, F in (('d', m['tf']), ('v', m['qf'])):
        for i, f in enumerate(F):
            if np.mean([m['v'][j][0] for j in f[1::2]]) >= 150:
                ng += 1
            else:
                AV.setze_zeile(AV.flaeche(m, '%s%d' % (kind, i)), 480)
                nh += 1
    z = 486 - 480
    c[z] = np.where(c[z] == 0, 0, (c[z] & 0x8000) | MAGENTA)
    os.makedirs(os.path.dirname(os.path.abspath(ziel_datei)), exist_ok=True)
    open(ziel_datei, 'wb').write(KB.packen(blob, d, ms, KB.tim_packen(t)))
    print('KO', ziel_datei, 'Gesichtsflaechen', ng, 'Hinterkopf', nh)


def bild(p):
    from PIL import Image
    return np.asarray(Image.open(p).convert('RGB')).astype(int)


def mag(a):
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    return (np.minimum(r, b) >= 24) & (g * 4 <= np.minimum(r, b))


def gr(a):
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    return (g >= 20) & (r * 3 <= g) & (b * 3 <= g)


def wurzel(da, db, f0=328, f1=346):
    tot, row = 0, []
    for f in range(f0, f1 + 1):
        pa, pb = os.path.join(da, 'fd', 'f%06d.ppm' % f), os.path.join(db, 'fd', 'f%06d.ppm' % f)
        if not (os.path.exists(pa) and os.path.exists(pb)):
            continue
        w = int((mag(bild(pa)) & ~mag(bild(pb))).sum())
        tot += w; row.append((f, w))
    print('Wurzel sichtbar F%d-F%d (960x720): Summe %d' % (f0, f1, tot), row)
    return tot, row


def schlitz_maske(g, w=4):
    """Nicht-gruene Bildpunkte mit Gruen links UND rechts innerhalb w px in derselben Zeile und Gruen darunter innerhalb w px (schmale
    senkrechte Kerbe/Loch in der Figur)."""
    H, W = g.shape
    def nah(dx, dy):
        out = np.zeros_like(g)
        for k in range(1, w + 1):
            s = np.zeros_like(g)
            ys = slice(max(0, -dy * k), H - max(0, dy * k)); yd = slice(max(0, dy * k), H - max(0, -dy * k))
            xs = slice(max(0, -dx * k), W - max(0, dx * k)); xd = slice(max(0, dx * k), W - max(0, -dx * k))
            s[ys, xs] = g[yd, xd]
            out |= s
        return out
    return ~g & nah(-1, 0) & nah(1, 0) & nah(0, 1)


def schlitz(dn, d7, f0=344, f1=378):
    tot, tot7, row = 0, 0, []
    for f in range(f0, f1 + 1):
        pn, p7 = os.path.join(dn, 'fd', 'f%06d.ppm' % f), os.path.join(d7, 'fd', 'f%06d.ppm' % f)
        if not (os.path.exists(pn) and os.path.exists(p7)):
            continue
        g, g7 = gr(bild(pn)), gr(bild(p7))
        h, h7 = schlitz_maske(g), schlitz_maske(g7)
        oben = min(int(np.nonzero(g.any(1))[0].min()), int(np.nonzero(g7.any(1))[0].min())) + OBEN_PX
        h[oben:] = False; h7[oben:] = False                    # nur Ruecken/Kopf (Befund "links neben dem Kopf"), nicht Schwanz/Beine
        n, n7 = int(h.sum()), int(h7.sum())
        tot += max(0, n - n7); tot7 += n7; row.append((f, n, n7))
    print('Schlitz F%d-F%d (960x720): Ueberschuss gegen V7 (Summe je Bild max(0, neu - V7)) %d (V7 selbst %d)' % (f0, f1, tot, tot7), row)
    return tot, tot7, row


if __name__ == '__main__':
    a = sys.argv[1:]
    if a[0] == 'bauen':
        bauen(a[1], a[2])
    elif a[0] == 'wurzel':
        wurzel(a[1], a[2], *(int(x) for x in a[3:5]))
    elif a[0] == 'ko':
        bauen_ko(a[1], a[2])
    elif a[0] == 'schlitz':
        schlitz(a[1], a[2], *(int(x) for x in a[3:5]))
