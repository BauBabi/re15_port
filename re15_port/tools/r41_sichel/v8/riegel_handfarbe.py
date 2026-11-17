#!/usr/bin/env python3
"""R41 Spur SF, V8 Runde 7e: Riegel "Handfarbe" - grauer Handruecken, rote Finger an der linken Hand (Mesh 12) und am linken Unterarm
(Mesh 11), gemessen in ECHTEN Framedumps des Port-Renderers gegen die fuenf Nutzerbilder birkin20-24.

    python riegel_handfarbe.py marker EMD ZIEL.EMD
    python riegel_handfarbe.py messen EMD --plain DIR --hf DIR [--aus TXT] [--json JSON] [--name NAME]

Nutzerentscheid 2026-11-14 (woertlich, auf die Frage "dunkelrot-violett wie jetzt oder grau mit roten Spitzen wie auf Ihrer Vorlage?"):
"wie in der Vorlage." Vorlage = Version_80%/birkin20-24.png (nur gelesen): grauer Handruecken, nur die Finger/Krallen rot
(Sicht-Pruefer Runde 7, SF_PRUEFUNG_V8_r7_sicht.md (4b)).

Marker "HF" (Verfahren wie szene_marker.py / riegel_r7c.lm_bauen: Flaechenklassen als deckende CLUT-Farben, Eintrag 0 bleibt 0):
    Mesh 11 (Unterarm)                         -> Zeile 483 blau     0x7C00
    Mesh 12 ohne Krallen (Hand, Daumenballen)  -> Zeile 485 gruen    0x03E0
    Mesh 12 Fingerkrallen A, B, C              -> Zeile 488 magenta  0x7C1F
    Mesh 12 Daumenkralle T                     -> Zeile 489 cyan     0x7FE0
    Mesh 12 Kastenfinger (Flaechen an den Ecken 32-39, Finger unter Kralle B, keine Kralle) -> Zeile 486 gelb 0x03FF
    alle anderen Flaechen                      -> Zeile 480 rot      0x001F   (verdecken die Hand wie im Spiel)
Die Klassen haengen nur an den Ecken (Krallen = Doppelpyramiden hand_r6.KRALLEN), nicht an CLUT-Zeile oder UV - der Marker ist fuer
den alten und den neuen Stand derselbe. Lauf: v4/lauf_v4.sh mit dem Marker als EMD-Overlay, FD wie der Szenenlauf.

Messung je Paar (Spielbild F | Nutzerbild): F368 | birkin20, F369 | birkin22, F371 | birkin23, F373 | birkin21, F375 | birkin24
(hand_szene.PAARE, F = h + 243), dazu F367 | birkin20 (Bild des Sicht-Pruefers r7, der F368 nicht aufgenommen hatte).
  Modell (Framedump 960 x 720): Klassen aus dem HF-Lauf (gesaettigte Markerfarbe UND Summe |HF - plain| > 30, wie riegel_r7c.klassen);
    W = Mitte der Bildpunkte, an denen Unterarm (blau) und Hand (gruen/magenta/cyan) aneinanderstossen; Finger-Achse W -> Schwerpunkt
    der Fingerkrallen (magenta) (hand_mass: "wahre Finger-Achse = Handgelenk -> Mitte der Krallenwurzeln"); Krallenwurzel S6 = erste Stelle
    entlang der Achse, ab der 2 Videopixel lang mindestens die Haelfte des groessten Laufs quer zur Achse Fingerkralle ist (Regel
    hand_mass.modell, dort mit Zeile 492).
  Vorlage: hand_mass.vorlage (Arm-Maske, E, W, T, Beginn der roten Zone = S6).
  Bereiche: Handruecken = Hand zwischen W und S6 (mit Daumen), Finger = Hand ab S6; Unterarm unteres Drittel = Unterarm zwischen W und
    einem Drittel der Unterarmlaenge (Vorlage: Strecke W -> E; Modell: W bis zum fernsten Unterarm-Bildpunkt entlang W -> Schwerpunkt
    des Unterarms).
  Farbe des Bildpunkts (plain-Lauf bzw. Nutzerbild): CIE-Lab (v4_gemein.lab); "rot" = a* >= 10 (dieselbe Schwelle wie S3 f, S7,
    Messblatt V8 6c); sonst "grau".
Urteil (GRUEN, wenn alle drei in JEDEM der fuenf Paare gelten; F367 nur Angabe):
  (a) Handruecken: Rot-Anteil <= groesster Rot-Anteil des Handrueckens der fuenf Vorlagebilder;
  (b) Finger: Rot-Anteil >= kleinster Rot-Anteil der Finger der fuenf Vorlagebilder (die Krallen bleiben rot);
  (c) Unterarm unteres Drittel: Rot-Anteil <= groesster Rot-Anteil desselben Bereichs der fuenf Vorlagebilder.
Dazu als Angabe: Median L*, a*, b* je Bereich, und L* des Handrueckens gegen das obere Unterarm-Drittel desselben Bilds.
Gegenprobe: der Stand 5c902d49 (dunkelrot-violette Hand, Zeile 490) muss in (a) oder (c) ROT sein.

Farbe dient hier nur als Messung (Anteile gegen die Vorlage), nie als Farbsoll fuer Texel (E4: keine Eichung am Video/an Nutzerbildern).
PORT-WAHL: keine @0x-Konstante (die Sichel-Form steht auf keiner Disc).
"""
import os, sys, json, argparse
import numpy as np
from scipy import ndimage

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import hand_mass as HM                # noqa: E402
import v4_gemein as G                 # noqa: E402
import handfarbe_r7e as HF            # noqa: E402

ZEILE = dict(rest=480, unterarm=483, hand=485, finger=488, daumenkralle=489, kastenfinger=486)
FARBE = {480: 0x001F, 483: 0x7C00, 485: 0x03E0, 488: 0x7C1F, 489: 0x7FE0, 486: 0x03FF}
PAARE_F = ((368, 20), (369, 22), (371, 23), (373, 21), (375, 24))
EXTRA_F = ((367, 20),)
FIG = 30                              # riegel_r7c.FIG: Figur = Summe |Marker - plain| > 30
ROT_A = 10.0                          # a* >= 10 = rot (riegel_v8 S3 f / S7, Messblatt V8 6c)
FD_SKALA = 1.5                        # Framedump 960 x 720 = 1,5 x Videopixel 640 x 480


def krallen_klasse(f):
    """Flaeche von Mesh 12 -> 'finger' (Kralle A/B/C), 'daumenkralle' (T), 'kastenfinger' oder 'hand' (Regel der Stufe f)."""
    return HF.klasse12(f)


def marker(blob):
    d, _o, ms, tim = KB.teile(blob)
    zahl = {}
    for b, m in enumerate(ms):
        for F, U in (('tf', 'tuv'), ('qf', 'quv')):
            for i, f in enumerate(m[F]):
                k = 'unterarm' if b == 11 else (krallen_klasse(f) if b == 12 else 'rest')
                zahl[k] = zahl.get(k, 0) + 1
                uv = m[U][i]
                uv[2] = (ZEILE[k] << 6) | (uv[2] & 0x3F)
    t = KB.tim_teile(tim)
    c = t['clut']
    for z, col in FARBE.items():
        c[z - 480] = np.where(c[z - 480] == 0, 0, (c[z - 480] & 0x8000) | col)
    return KB.packen(blob, d, ms, KB.tim_packen(t)), zahl


# ------------------------------------------------------------------ Bilder
def ppm(ordner, f):
    from PIL import Image
    for p in (os.path.join(ordner, 'fd', 'f%06d.ppm' % f), os.path.join(ordner, 'f%06d.ppm' % f)):
        if os.path.exists(p):
            return np.asarray(Image.open(p).convert('RGB')).astype(np.int32)
    return None


def klassen(hf, plain):
    mx = hf.max(-1)
    on = hf > np.maximum(0.5 * mx, 40)[..., None]
    r, g, b = on[..., 0], on[..., 1], on[..., 2]
    fig = np.abs(hf - plain).sum(-1) > FIG
    return dict(unterarm=fig & b & ~r & ~g, hand=fig & g & ~r & ~b, finger=fig & r & b & ~g, daumenkralle=fig & g & b & ~r,
                kastenfinger=fig & r & g & ~b)


def _einheit(v):
    v = np.asarray(v, float)
    return v / max(float(np.linalg.norm(v)), 1e-9)


def _s6(Hm, Fi, W, T, skala):
    """Regel hand_mass.modell: erste Stelle entlang W -> T, ab der 2 Videopixel lang mindestens die Haelfte des groessten Laufs quer zur
    Achse Fingerkralle ist. Rueckgabe Abstand von W in Bildpunkten der Maske (None: keine solche Stelle)."""
    r2, M2, (e2,), _w = HM.drehen(Hm, W, T, extra=[Fi.astype(float)])
    yW = int(round(HM.punkt(M2, W)[1]))
    prof = HM.zeilen_profil(r2, yW)
    ant = [float(e2[yW + i, HM.groesster(p[5])[0]:HM.groesster(p[5])[1]].mean()) if p else 0.0 for i, p in enumerate(prof)]
    n = int(round(2.0 / skala * HM.ZOOM))
    ir = next((i for i in range(max(len(ant) - n, 0)) if min(ant[i:i + n]) >= 0.5), None)
    return None if ir is None else ir / HM.ZOOM


def _grenze_farbe(M, a_kanal, W, T, n):
    """Grenze grau/rot nach der Regel der Vorlage (hand_mass.vorlage): je Zeile quer zur Achse W -> T das mittlere a* im groessten Lauf;
    grau = Median des ersten Viertels, rot = 95. Perzentil; Grenze = erste Zeile, ab der n Zeilen (= 1 Videopixel) lang a* >= Mitte.
    Rueckgabe Abstand von W in Bildpunkten der Maske."""
    r2, M2, (a2,), _w = HM.drehen(M, W, T, extra=[a_kanal])
    yW = int(round(HM.punkt(M2, W)[1]))
    p2 = HM.zeilen_profil(r2, yW)
    am = np.array([float(a2[yW + i, HM.groesster(p[5])[0]:HM.groesster(p[5])[1]].mean()) if p else 0.0 for i, p in enumerate(p2)])
    grau, rot = float(np.median(am[:len(am) // 4])), float(np.percentile(am, 95))
    schw = 0.5 * (grau + rot)
    ir = next((i for i in range(len(am) - n) if am[i:i + n].min() >= schw), int(0.55 * len(am)))
    return ir / HM.ZOOM


def bereiche_modell(k, rgb=None):
    U = k['unterarm']
    Hm = k['hand'] | k['finger'] | k['daumenkralle'] | k['kastenfinger']
    Fi = k['finger']
    if not U.any() or not Hm.any() or not Fi.any():
        return None
    S8 = np.ones((3, 3), bool)
    rand = (ndimage.binary_dilation(U, S8) & Hm) | (ndimage.binary_dilation(Hm, S8) & U)
    if not rand.any():
        return None
    ys, xs = np.nonzero(rand)
    W = np.array([xs.mean(), ys.mean()])
    fy, fx = np.nonzero(Fi)
    ax = _einheit(np.array([fx.mean(), fy.mean()]) - W)
    hy, hx = np.nonzero(Hm)
    th = (hx - W[0]) * ax[0] + (hy - W[1]) * ax[1]
    T = W + ax * float(th.max())
    s6 = _s6(Hm, Fi, W, T, 1.0 / FD_SKALA)
    if s6 is None:
        return None
    uy, ux = np.nonzero(U)
    du = _einheit(np.array([ux.mean(), uy.mean()]) - W)
    tu = (ux - W[0]) * du[0] + (uy - W[1]) * du[1]
    Lu = float(tu.max())
    yy, xx = np.mgrid[0:U.shape[0], 0:U.shape[1]]
    t_all = (xx - W[0]) * ax[0] + (yy - W[1]) * ax[1]
    tu_all = (xx - W[0]) * du[0] + (yy - W[1]) * du[1]
    out = dict(handruecken=Hm & (t_all >= 0) & (t_all < s6), finger=Hm & (t_all >= s6),
               unterarm_unten=U & (tu_all <= Lu / 3.0), unterarm_oben=U & (tu_all >= 2.0 * Lu / 3.0),
               W=W.round(1).tolist(), T=T.round(1).tolist(), s6_px=round(s6 / FD_SKALA, 1), hand_px=round(float(th.max()) / FD_SKALA, 1),
               unterarm_px=round(Lu / FD_SKALA, 1))
    if rgb is not None:                                        # Grenze grau/rot im Bild nach der Regel der Vorlage (1 Videopixel = 1,5 FD-Punkte)
        a_k = np.zeros(U.shape, np.float32)
        a_k[Hm] = G.lab(rgb[Hm].astype(float))[:, 1]
        out['grenze_px'] = round(_grenze_farbe(Hm, a_k, W, T, int(round(FD_SKALA * HM.ZOOM))) / FD_SKALA, 1)
    return out


def bereiche_vorlage(v):
    m = v['_maske']
    E, W, T = (np.array(v[k], float) for k in ('E', 'W', 'T'))
    ax = _einheit(T - W)
    s6 = v['bis_krallenwurzel'] / 0.5                        # Videopixel -> Bildpunkte des Nutzerbilds (doppelte Groesse)
    yy, xx = np.mgrid[0:m.shape[0], 0:m.shape[1]]
    t = (xx - W[0]) * ax[0] + (yy - W[1]) * ax[1]
    du = _einheit(E - W)
    Lu = float(np.linalg.norm(E - W))
    tu = (xx - W[0]) * du[0] + (yy - W[1]) * du[1]
    lab = G.lab(v['_bild'][..., 2::-1].astype(float))
    g = _grenze_farbe(m, lab[..., 1].astype(np.float32), W, T, 2 * HM.ZOOM) * 0.5   # 2 Bildpunkte = 1 Videopixel (hand_mass.vorlage)
    return dict(handruecken=m & (t >= 0) & (t < s6), finger=m & (t >= s6), unterarm_unten=m & (tu > 0) & (tu <= Lu / 3.0),
                unterarm_oben=m & (tu >= 2.0 * Lu / 3.0) & (tu <= Lu), s6_px=round(v['bis_krallenwurzel'], 1), hand_px=v['hand_laenge'],
                unterarm_px=v['unterarm_laenge'], grenze_px=round(g, 1))


def farben(rgb, M):
    if not M.any():
        return dict(px=0, rot=None, L=None, a=None, b=None)
    lab = G.lab(rgb[M].astype(float))
    return dict(px=int(M.sum()), rot=round(float((lab[:, 1] >= ROT_A).mean()), 3), L=round(float(np.median(lab[:, 0])), 1),
                a=round(float(np.median(lab[:, 1])), 1), b=round(float(np.median(lab[:, 2])), 1))


BEREICHE = ('handruecken', 'finger', 'unterarm_unten', 'unterarm_oben')


def vorlage_alle():
    out = {}
    for k in sorted({b for _f, b in PAARE_F + EXTRA_F}):
        v = HM.vorlage(k)
        bb = bereiche_vorlage(v)
        rgb = v['_bild'][..., 2::-1].astype(float)
        out[k] = dict({n: farben(rgb, bb[n]) for n in BEREICHE}, s6_px=bb['s6_px'], hand_px=bb['hand_px'], unterarm_px=bb['unterarm_px'],
                      s6_anteil=round(bb['s6_px'] / bb['hand_px'], 3), grenze_px=bb['grenze_px'], grenze_anteil=round(bb['grenze_px'] / bb['hand_px'], 3))
    return out


def modell_alle(plain, hf):
    out = {}
    for f, _b in PAARE_F + EXTRA_F:
        a, h = ppm(plain, f), ppm(hf, f)
        if a is None or h is None:
            out[f] = None
            continue
        bb = bereiche_modell(klassen(h, a), rgb=a)
        if bb is None:
            out[f] = None
            continue
        out[f] = dict({n: farben(a, bb[n]) for n in BEREICHE}, W=bb['W'], T=bb['T'], s6_px=bb['s6_px'], hand_px=bb['hand_px'],
                      unterarm_px=bb['unterarm_px'], s6_anteil=round(bb['s6_px'] / bb['hand_px'], 3), grenze_px=bb['grenze_px'],
                      grenze_anteil=round(bb['grenze_px'] / bb['hand_px'], 3))
    return out


def urteil(mod, vor):
    paare = [k for k, _b in PAARE_F]
    va = [vor[b]['handruecken']['rot'] for _f, b in PAARE_F]
    vb = [vor[b]['finger']['rot'] for _f, b in PAARE_F]
    vc = [vor[b]['unterarm_unten']['rot'] for _f, b in PAARE_F]
    soll = dict(a=max(va), b=min(vb), c=max(vc))
    fehlt = [f for f in paare if mod.get(f) is None]
    a_aus = [f for f in paare if mod.get(f) and mod[f]['handruecken']['rot'] > soll['a']]
    b_aus = [f for f in paare if mod.get(f) and mod[f]['finger']['rot'] < soll['b']]
    c_aus = [f for f in paare if mod.get(f) and mod[f]['unterarm_unten']['rot'] > soll['c']]
    return dict(soll=soll, fehlt=fehlt, a_aus=a_aus, b_aus=b_aus, c_aus=c_aus,
                a_ok=not a_aus and not fehlt, b_ok=not b_aus and not fehlt, c_ok=not c_aus and not fehlt,
                ok=not (a_aus or b_aus or c_aus or fehlt))


def zeilen(name, sha, mod, vor, u):
    gr = lambda ok: 'GRUEN' if ok else 'ROT'
    z = ['Riegel Handfarbe (R7e, Nutzerentscheid 2026-11-14 "wie in der Vorlage") - %s sha256 %s: %s' % (name, sha, gr(u['ok']))]
    z.append('   Soll aus den fuenf Vorlagebildern: (a) Handruecken rot <= %.3f; (b) Finger rot >= %.3f; (c) Unterarm unteres Drittel rot <= %.3f'
             ' (rot = a* >= %.0f)' % (u['soll']['a'], u['soll']['b'], u['soll']['c'], ROT_A))
    for f, b in PAARE_F + EXTRA_F:
        v, m = vor[b], mod.get(f)
        def t(x):
            return '%5.3f (L %4.1f a %4.1f b %5.1f, %d px)' % (x['rot'], x['L'], x['a'], x['b'], x['px']) if x['rot'] is not None else '-'
        z.append('   F%d%s | birkin%d: Handruecken rot %s | Vorlage %s' % (f, '' if (f, b) in PAARE_F else ' (Angabe)', b,
                 t(m['handruecken']) if m else 'fehlt', t(v['handruecken'])))
        if m:
            z.append('        Finger rot %s | Vorlage %s' % (t(m['finger']), t(v['finger'])))
            z.append('        Unterarm unteres Drittel rot %s | Vorlage %s' % (t(m['unterarm_unten']), t(v['unterarm_unten'])))
            z.append('        Unterarm oberes Drittel %s | Vorlage %s' % (t(m['unterarm_oben']), t(v['unterarm_oben'])))
            z.append('        (d) Grenze grau/rot nach der Regel der Vorlage bei %.3f der Handlaenge (Vorlage %.3f); Krallenwurzel S6 (Marker) %.3f; '
                     'L* Handruecken / L* Unterarm oben %.2f (Vorlage %.2f), unteres / oberes Drittel %.2f (Vorlage %.2f)'
                     % (m['grenze_anteil'], v['grenze_anteil'], m['s6_anteil'], m['handruecken']['L'] / m['unterarm_oben']['L'],
                        v['handruecken']['L'] / v['unterarm_oben']['L'], m['unterarm_unten']['L'] / m['unterarm_oben']['L'],
                        v['unterarm_unten']['L'] / v['unterarm_oben']['L']))
    z.append('   (a) Handruecken ueber dem Soll: %s -> %s' % (u['a_aus'] or 'keine', gr(u['a_ok'])))
    z.append('   (b) Finger unter dem Soll: %s -> %s' % (u['b_aus'] or 'keine', gr(u['b_ok'])))
    z.append('   (c) Unterarm unteres Drittel ueber dem Soll: %s -> %s' % (u['c_aus'] or 'keine', gr(u['c_ok'])))
    if u['fehlt']:
        z.append('   fehlende Bilder: %s' % u['fehlt'])
    return z


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('was', choices=('marker', 'messen'))
    ap.add_argument('emd')
    ap.add_argument('ziel', nargs='?')
    ap.add_argument('--plain'); ap.add_argument('--hf')
    ap.add_argument('--aus'); ap.add_argument('--json'); ap.add_argument('--name', default='Pruefling')
    a = ap.parse_args()
    blob = open(a.emd, 'rb').read()
    if a.was == 'marker':
        neu, zahl = marker(blob)
        open(a.ziel, 'wb').write(neu)
        print('HF-Marker', a.ziel, V.sha(neu), 'aus', V.sha(blob), 'Flaechen je Klasse', dict(sorted(zahl.items())))
        return
    vor = vorlage_alle()
    mod = modell_alle(a.plain, a.hf)
    u = urteil(mod, vor)
    z = zeilen(a.name, V.sha(blob), mod, vor, u)
    print('\n'.join(z))
    if a.aus:
        open(a.aus, 'w', encoding='utf-8', newline='\n').write('\n'.join(z) + '\n')
    if a.json:
        json.dump(dict(modell={str(k): v for k, v in mod.items()}, vorlage={str(k): v for k, v in vor.items()}, urteil=u),
                  open(a.json, 'w', encoding='utf-8', newline='\n'), indent=1)
    sys.exit(0 if u['ok'] else 1)


if __name__ == '__main__':
    main()
