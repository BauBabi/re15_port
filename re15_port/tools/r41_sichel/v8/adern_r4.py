"""SF V8 Runde 4: Texturen nach den Befunden der Pruefung V8 r3 -> adern_v8.json (modus "original_r4"; Bau: adern_v8.adern).

    python adern_r4.py [--kette JSON] [--ruecken JSON] [--aus JSON] [--basis EMD_V7]

Nutzer 2026-11-08 (woertlich, bindend): "man sieht ebenfalls beim vorlage Bild, das die Rechte Seite auch mehr rottöne hat, also quasi mehr
adern an verschiedenen Stellen, am Arm und Körper, nicht nur grau. Übernehme die die Textur Sachen die du dafür brauchst vom originalen
Birkin von Resident Evil 1.5"; Skizze "Little red hill from the back", "smooth straight to head without further hills....".
Regel (Memory "Texturen nie aus Videobildern eichen"): Texel und Paletteneintraege nur aus dem Original-EM30; das Video liefert nur Orte.

Befunde der Pruefung V8 r3 und Umsetzung (PORT-WAHL, jede Wahl unten mit Messung):
  Naht rosa/grau (Sicht-Pruefer (3): "Die (W)-rechte Haelfte ist rosa, die (W)-linke grau ... Das Original-EM30 ist rundum einheitlich rosa";
     Vorschlag "Original-Zeilen beidseitig"): ALLE Flaechen von Mesh 8 auf den V7-Zeilen 480/481 (nicht die Brust 488, Nutzer V5 "Die roten
     Stellen sind korrekt auf der brust") -> Original-Zeile 493/494 mit eigener (= Original-) UV.
  Oberarm 13 (Zahlen-Pruefer: "24,3 % ... Soll Vorlage-Band 35-59 % UND hoechstens Original 57,5 %"): wie Runde 3 alle Flaechen ausser Feld C
     mit eigener UV auf 493; dazu die GANZE Feld-C-Gruppe (24 Flaechen, Schulterkuppe, eine UV-Insel) mit EINEM Schub auf Original-Oberarm-Haut
     (Texel, die Original-Flaechen von Mesh 13 mit Zeile 480 ausserhalb Feld C benutzen) -> Zeile 493. Feld C selbst (Augapfel, Nutzer V5
     "Auch der rote Fleck oben auf der rechten Schulter, den gibt es in der Vorlage ganz offensichtlich nicht") bleibt unbenutzt.
     Wahl: unter allen passenden Schueben (Bild PV gerendert) Oberarm-Rotanteil in OBERARM_BAND, dann wenigste rote Flecken auf Knochen 13,
     dann |Anteil - OBERARM_ZIEL| klein, dann kleinster Schub.
  Unterarmband (Sicht-Pruefer (3): "Das gerade rote Unterarmband ist aber unveraendert da: Knochen 14 in P2 hat 384 rote px"; Herkunft:
     Klingenleiste des Original-Birkin, Flaechen v9/v10/d19/d22): alle Flaechen mit eigener UV auf 495 ausser der Band-Gruppe; die Band-Gruppe
     mit einem Schub (und Skalierung, falls kein Schub passt) auf Original-Unterarm-Haut (Texel der Original-Flaechen von Mesh 14 ohne die
     Klingenleiste) -> 495. Wahl: groesste Skalierung (1,0; 0,9; ... 0,5), dann kleinster Anteil a* >= 15, dann kleinster Schub.
  Huegel und Kappe (Sicht-Pruefer (2): "von vorn liegt eine kantige ROTE Kappe hinter und ueber dem Kopf"; "Von hinten ... zwei grosse rote
     Kacheln mit harten Kanten"; Zahlen-Pruefer O2: Huegel-Texel zur Haelfte aus einem Bereich, den das Original nicht nutzt): die Flaechen,
     die in PV die obersten 8 px x 428-445 bilden, als EINE Gruppe mit einem Schub auf Texel, die Original-Flaechen mit Zeile 480 benutzen
     (nicht Feld C) -> 493. Wahl (Bild gerendert): rote Kappe von vorn (P1, Szene Takt F354) <= KAPPE_MAX, groesster kraeftig roter Fleck von
     hinten (Takt F328/F332) <= Original-EM30, Huegel rot (S3f) >= HUEGEL_MIN, dann |S3f - Vorlage 58,3| klein, dann kleinster Schub.
     Vorher (gemessen sfv8/r4 t14.py: auch ohne Huegel-Schub ist der groesste Fleck von hinten 764 px gegen 296 am Original - Fleisch am
     oberen Ruecken, Flaechen v16/v13/v1/d50 des V7-Rumpfs, der 1,25-mal groesser ist als der Original-Rumpf): Gruppe "rueckenfleisch",
     gierig die Flaeche mit den meisten Bildpunkten im groessten Fleck, ein Schub auf Original-Haut (haut_wahl), bis der Fleck <= Original ist.
  Rumpf (W)-rechts (Zahlen-Pruefer: "62,2 % gegen Original in gleicher Pose 49,4 %"): reicht das nicht, gierig die (W)-rechte Flaeche von Mesh 8
     mit den meisten roten Bildpunkten in PV (a* >= 10 wie Riegel S7c; nicht Huegel, nicht Rueckenfleisch, nicht Brust) in die Gruppe
     "blass_rechts"; die Gruppe bekommt EINEN Schub auf Original-Haut mit Anteil a* >= 10 nahe HAUT_ZIEL, bis Rumpf (W)-rechts <= Original -
     RUMPF_RAND (der V7-Rumpf ist 1,25-mal groesser als der Original-Rumpf, gleiche Texel bedecken in PV mehr Bild); danach, solange die
     roten Flecken je 1000 px (Riegel S7d) nicht FLECKEN_RAND unter dem Original liegen, die Flaeche mit den meisten Flecken.
Kein RE-Wert; PORT-WAHL.
"""
import os, sys, json, argparse, copy
import numpy as np
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import adern_v8 as AV                 # noqa: E402
import adern_r3 as A3                 # noqa: E402
import f_rast as FR                   # noqa: E402
import v4_gemein as G                 # noqa: E402
import r3_gemein as R3                # noqa: E402
import birkin_formen as BF            # noqa: E402
from scipy import ndimage             # noqa: E402

BRUST = 488
OBERARM_BAND = (38.0, 54.0)           # Zahlen-Pruefer r3: Band 35-59 % und <= Original 57,5 %; je 3 Punkte Abstand zu den Grenzen
OBERARM_ZIEL = 46.0                   # Mitte von 35 .. 57,5
KAPPE_MAX = 0                         # Bildpunkte a* >= 30 ueber der Kopfoberkante + 5 (V7 0, Original 0, V8 r3 306 / 214; Sicht-Pruefer
                                      # r3: "hoechstens auf V7-Niveau (0)")
HUEGEL_MIN = 45.0                     # S3f; Vorlage 58,3 %, Riegel-Schwelle 40
HUEGEL_VORLAGE = 58.3
RUMPF_RAND = 2.0
RUMPF_MIN = 30.0                      # Rumpf (W)-rechts nie unter 30 % (Riegel S7c Untergrenze 25 %)
FLECKEN_RAND = 0.1                    # rote Flecken je 1000 px mindestens 0,1 unter dem Original (Riegel S7d, Zahlen-Pruefer r3 GELB)
HAUT_ZIEL = 0.40                      # Anteil a* >= 10 der Zieltexel von "rueckenfleisch"/"blass_rechts" (erlaubte Original-Texel Seite 0:
                                      # Median a* 10,9, also etwa die Haelfte >= 10; etwas blasser als der Schnitt, damit Rumpf (W)-rechts sinkt)
SKALEN = (1.0, 0.9, 0.8, 0.7, 0.6, 0.5)
BAND = ('v9', 'v10', 'd19', 'd22')    # Mesh 14: in P2 rot (sfv8/r4 mass4.band14: v9 300, v10 62, d19 11, d22 11 px bei V8 r3)


# ------------------------------------------------------------------ Texel-Mengen
_C = {}


def original():
    if 'o' not in _C:
        _o, MO, TO = AV.original()
        _C['o'] = (MO, TO)
    return _C['o']


def nutzung(meshes, zeile):
    MO, _TO = original()
    mk = np.zeros((256, 384), bool)
    for b, m in enumerate(MO):
        if meshes is not None and b not in meshes:
            continue
        for uv in m['tuv'] + m['quv']:
            if (uv[2] >> 6) & 0x1FF == zeile:
                mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    return mk


def feld_c_maske():
    m = np.zeros((256, 384), bool)
    m[A3.FELD_C[3]:A3.FELD_C[4] + 1, A3.FELD_C[1]:A3.FELD_C[2] + 1] = True
    return m


def gruppe_maske(uvs):
    mk = np.zeros((256, 384), bool)
    for uv in uvs:
        mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
    return mk


def a_karte(k):
    _MO, TO = original()
    return AV.lab15(TO['clut'][k])[TO['pix']][..., 1]


def skaliert(uvs, sk):
    n = lambda uv: 3 if len(uv) == 9 else 4
    u0 = min(uv[3 * i] for uv in uvs for i in range(n(uv))); v0 = min(uv[3 * i + 1] for uv in uvs for i in range(n(uv)))
    out = []
    for uv in uvs:
        q = list(uv)
        for i in range(n(uv)):
            q[3 * i] = int(round(u0 + (q[3 * i] - u0) * sk)); q[3 * i + 1] = int(round(v0 + (q[3 * i + 1] - v0) * sk))
        out.append(q)
    return out


def schuebe(uvs, erlaubt):
    """Alle ganzzahligen (du, dv), fuer die die Vereinigung der Gruppe ganz in `erlaubt` liegt (auf derselben Textur-Seite)."""
    from scipy.signal import fftconvolve
    po = (uvs[0][5] & 0xF) * 128
    Gm = gruppe_maske(uvs)[:, po:po + 128]
    E = erlaubt[:, po:po + 128]
    ys, xs = np.nonzero(Gm)
    y0, y1, x0, x1 = ys.min(), ys.max(), xs.min(), xs.max()
    g = Gm[y0:y1 + 1, x0:x1 + 1].astype(float)
    bad = fftconvolve((~E).astype(float), g[::-1, ::-1], mode='valid')
    vv, uu = np.nonzero(bad < 0.5)
    return sorted((int(u - x0), int(v - y0)) for v, u in zip(vv, uu)), Gm, po


# ------------------------------------------------------------------ Messen
def kappe_vorn(mo, pose):
    A = FR.mit_bg(mo, pose); b = A['bone'][:480]; L = G.lab(A['img'][:480].astype(float))
    top = int(np.nonzero((b == 9).any(1))[0].min()) + 5
    m = (b == 8) & (L[..., 1] >= 30)
    m[top:] = False
    return int(m.sum())


def haut_wahl(a0, Gm, s):
    """Rangfolge eines Schubs auf Seite 0 fuer "rueckenfleisch" und "blass_rechts": Original-Haut mit Adern wie die Umgebung (Anteil a* >= 10
    nahe HAUT_ZIEL in Schritten von 0,05), dann wenig kraeftiges Rot (a* >= 30), dann kleinster Schub. Probe sfv8/r4 T2/T3: der kleinste
    Anteil a* >= 10 ergab helle Streifen-Kacheln am Schwanzansatz."""
    a = a0[:, :128][np.roll(np.roll(Gm, s[1], 0), s[0], 1)]
    return (round(abs(float((a >= 10).mean()) - HAUT_ZIEL) / 0.05), round(float((a >= 30).mean()), 2), s[0] * s[0] + s[1] * s[1], s)


def kappe_flaechen(mo, pose):
    """Flaechen-Schluessel je Bildpunkt der Kappe (wie kappe_vorn)."""
    A = FR.mit_bg(mo, pose); b = A['bone'][:480]; L = G.lab(A['img'][:480].astype(float))
    top = int(np.nonzero((b == 9).any(1))[0].min()) + 5
    m = (b == 8) & (L[..., 1] >= 30)
    m[top:] = False
    return list(A['flaeche'][:480][m])


def kachel_hinten(mo, pose):
    A = FR.mit_bg(mo, pose); b = A['bone'][:480]; L = G.lab(A['img'][:480].astype(float))
    m = (b == 8) & (L[..., 1] >= 30)
    lab_, n = ndimage.label(m, structure=np.ones((3, 3)))
    return int(max(ndimage.sum(m, lab_, range(1, n + 1)))) if n else 0


def flecken13(mo):
    A = FR.zeichne(mo, V.PV, None, licht=False, knochen=[13])
    b = A['bone'][:480]; L = G.lab(A['img'][:480].astype(float))
    rot = (b == 13) & (L[..., 1] >= 15)
    lab_, n = ndimage.label(rot, structure=np.ones((3, 3)))
    gr = ndimage.sum(rot, lab_, range(1, n + 1)) if n else []
    return int(sum(1 for g in gr if g >= 4))


def rot_pv(blob):
    import riegel_v8 as RG
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV)
    return RG.rot_anteile(blob, A), RG.huegel_rot(A), A


# ------------------------------------------------------------------ Bau einer Probe
def anwenden(blob, par):
    d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    AV.adern(ms, t, par, {})
    return KB.packen(blob, d, ms, KB.tim_packen(t))


def keys(m):
    return [('d%d' % i, u) for i, u in enumerate(m['tuv'])] + [('v%d' % i, u) for i, u in enumerate(m['quv'])]


def grundwahl(ms):
    fl = {'8': [k for k, uv in keys(ms[8]) if (uv[2] >> 6) & 0x1FF in (480, 481)],
          '13': [k for k, uv in keys(ms[13]) if (uv[2] >> 6) & 0x1FF in (480, 489) and not A3.in_feld_c(uv)],
          '14': [k for k, uv in keys(ms[14]) if (uv[2] >> 6) & 0x1FF == 482 and k not in BAND]}
    fc = [k for k, uv in keys(ms[13]) if (uv[2] >> 6) & 0x1FF in (480, 489) and A3.in_feld_c(uv)]
    return fl, fc


def par_mit(fl, gruppen, quelle=''):
    return dict(quelle=quelle, modus='original_r4', zeilen=[0, 1, 2], flaechen=fl, gruppen=gruppen)


def ohne(fl, mesh, weg):
    f = {k: list(v) for k, v in fl.items()}
    f[mesh] = [k for k in f[mesh] if k not in weg]
    return f


def huegel_flaechen(blob):
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.PV)
    bone, fl = A['bone'][:480], A['flaeche'][:480]
    zahl = {}
    for x in range(428, 446):
        ys = np.nonzero(bone[:, x] >= 0)[0]
        for y in range(ys.min(), ys.min() + 8):
            if bone[y, x] == 8:
                zahl[fl[y, x]] = zahl.get(fl[y, x], 0) + 1
    _d, _o, ms, _t = KB.teile(blob)
    ok = lambda k: (AV.flaeche(ms[8], k)[2] >> 6) & 0x1FF in (480, 493) and AV.flaeche(ms[8], k)[5] & 0xF == 0   # nicht Brust (488), Seite 0
    return sorted([k for k, n in zahl.items() if n >= A3.HUEGEL_MIN_PX and ok(k)]), zahl


def flecken_flaechen(blob):
    """Je Flaeche von Mesh 8 (W)-rechts: Zahl der roten Flecken (Riegel S7d: a* >= 15, >= 4 px, weisses Licht, PV), deren meiste Bildpunkte
    auf ihr liegen."""
    A = FR.zeichne(V.modell(blob), V.PV, None, licht=False)
    b, fl, lok = A['bone'][:480], A['flaeche'][:480], A['lok'][:480]
    L = G.lab(A['img'][:480].astype(float))
    sel = np.isin(b, (8, 13, 14)) & ~((b == 8) & (lok[..., 2] >= 0))
    rot = sel & (L[..., 1] >= 15)
    lab_, n = ndimage.label(rot, structure=np.ones((3, 3)))
    out = {}
    for i in range(1, n + 1):
        m = lab_ == i
        if m.sum() < 4:
            continue
        ks = {}
        for x, y in zip(b[m], fl[m]):
            ks[(int(x), y)] = ks.get((int(x), y), 0) + 1
        (bk, key) = max(sorted(ks), key=lambda q: ks[q])
        if bk == 8:
            out[key] = out.get(key, 0) + 1
    return out


def rot_flaechen_rechts(blob):
    """Je Flaeche von Mesh 8 (W)-rechts (lok z < 0 im Bild, wie Riegel S7c): Zahl der Bildpunkte a* >= 10 (Videoraum, RDT-Licht, PV)."""
    A = FR.mit_bg(V.modell(blob), V.PV)
    b, fl, lok = A['bone'][:480], A['flaeche'][:480], A['lok'][:480]
    LM = G.lab(G.videoraum(A['img'][:480]))
    m = (b == 8) & (lok[..., 2] < 0) & (LM[..., 1] >= 10)
    out = {}
    for k in fl[m]:
        out[k] = out.get(k, 0) + 1
    return out


# ------------------------------------------------------------------ Suche
def suche(geo, log=print, feld_c=None):
    _d, _o, ms, _t = KB.teile(geo)
    fl, fc = grundwahl(ms)
    MO, TO = original()
    _o, _mo, to = AV.original()
    same = KB.tim_teile(KB.teile(geo)[3])['pix'] == to['pix']
    FC = feld_c_maske()
    gruppen = []
    # 1) Band (Mesh 14)
    bandmask = gruppe_maske([AV.flaeche(MO[14], k) for k in BAND])
    E14 = nutzung([14], 482) & same & ~bandmask
    a2 = a_karte(2)
    wahl = None
    for sk in SKALEN:
        g = skaliert([AV.flaeche(ms[14], k) for k in BAND], sk)
        sh, Gm, po = schuebe(g, E14)
        if not sh:
            continue
        best = min(sh, key=lambda s: (round(float((a2[:, po:po + 128][np.roll(np.roll(Gm, s[1], 0), s[0], 1)] >= 15).mean()), 4),
                                      s[0] * s[0] + s[1] * s[1], s))
        Gs = np.roll(np.roll(Gm, best[1], 0), best[0], 1)
        wahl = dict(name='unterarmband', mesh=14, flaechen=list(BAND), skal=sk, schub=list(best),
                    rot15=round(float((a2[:, po:po + 128][Gs] >= 15).mean()), 4), erlaubt='Original-Mesh-14-Texel (Zeile 482) ohne Klingenleiste')
        break
    log('Band', wahl)
    gruppen.append(wahl)
    # 2) Feld C (Mesh 13)
    E13 = nutzung([13], 480) & same & ~FC
    E13[:, 128:] = False
    gfc = [AV.flaeche(ms[13], k) for k in fc]
    sh, Gm, po = schuebe(gfc, E13)
    log('Feld C: %d Flaechen, %d Schuebe' % (len(fc), len(sh)))
    fl1 = {k: list(v) for k, v in fl.items()}
    erg = []
    for du, dv in ([tuple(feld_c)] if feld_c else sh):          # --feld-c: nur Nachrechnen eines frueheren Ergebnisses
        par = par_mit(fl1, gruppen + [dict(name='feld_c', mesh=13, flaechen=fc, skal=1.0, schub=[du, dv])])
        bl = anwenden(geo, par)
        r, _h, _A = rot_pv(bl)
        erg.append((du, dv, r['o13'], flecken13(V.modell(bl))))
    ok = [e for e in erg if OBERARM_BAND[0] <= e[2] <= OBERARM_BAND[1]]
    best = min(ok, key=lambda e: (e[3], round(abs(e[2] - OBERARM_ZIEL), 1), e[0] * e[0] + e[1] * e[1], (e[0], e[1])))
    log('Feld C Wahl', best, 'von', len(ok), 'im Band')
    gruppen.append(dict(name='feld_c', mesh=13, flaechen=fc, skal=1.0, schub=[best[0], best[1]], oberarm=best[2], flecken13=best[3],
                        erlaubt='Original-Mesh-13-Texel (Zeile 480) ausserhalb Feld C', kandidaten=len(sh), im_band=len(ok)))
    # 3) Rueckenfleisch (Mesh 8): groesster kraeftig roter Fleck von hinten <= Original-EM30 (Sicht-Pruefer r3 (2) "Von hinten ... zwei grosse
    #    rote Kacheln"; Vorschlag "Von hinten soll die groesste rote Flaeche hoechstens so gross sein wie am Original-EM30"). Gierig: die Flaeche
    #    mit den meisten Bildpunkten im groessten Fleck (Takt F328/F332, ohne Huegel-Flaechen) kommt in die Gruppe; die Gruppe bekommt EINEN
    #    Schub auf Original-Haut wie die Umgebung (haut_wahl), bis der Fleck <= Original ist.
    hf0, _hz = huegel_flaechen(anwenden(geo, par_mit(fl1, gruppen)))
    E0 = nutzung(None, 480) & same & ~FC
    E0[:, 128:] = False
    a0 = a_karte(0)
    o = V.modell(BF.lade_em30())
    sp_ = R3.szene_posen()
    k_orig = {f: kachel_hinten(o, sp_[f]) for f in (328, 332)}
    rf, gruppe_rf = [], None
    for _runde in range(12):
        g = gruppen + ([gruppe_rf] if gruppe_rf else [])
        bl = anwenden(geo, par_mit(ohne(fl1, '8', rf), g))
        mo = V.modell(bl)
        gross = None
        for f in (328, 332):
            A = FR.mit_bg(mo, sp_[f]); b = A['bone'][:480]; L = G.lab(A['img'][:480].astype(float))
            m = (b == 8) & (L[..., 1] >= 30)
            lab_, n = ndimage.label(m, structure=np.ones((3, 3)))
            if not n:
                continue
            sz = ndimage.sum(m, lab_, range(1, n + 1)); i = int(np.argmax(sz)) + 1
            if sz.max() > k_orig[f] and (gross is None or sz.max() - k_orig[f] > gross[0]):
                zahl = {}
                for k in A['flaeche'][:480][lab_ == i]:
                    if k not in hf0 and k not in rf:
                        zahl[k] = zahl.get(k, 0) + 1
                gross = (float(sz.max() - k_orig[f]), f, zahl)
        log('Rueckenfleisch: Gruppe %s, Ueberschuss %s' % (rf, None if gross is None else (gross[0], gross[1])))
        if gross is None or not gross[2]:
            break
        rf.append(max(sorted(gross[2]), key=lambda k: gross[2][k]))
        uvs = [AV.flaeche(ms[8], k) for k in rf]
        if len(set(uv[5] & 0xF for uv in uvs)) > 1:
            log('Rueckenfleisch: Flaechen auf verschiedenen Seiten', rf); rf.pop(); break
        sh, Gm, po = schuebe(uvs, E0)
        if not sh:
            log('Rueckenfleisch: kein Schub fuer', rf); rf.pop(); break
        s_ = min(sh, key=lambda s: haut_wahl(a0, Gm, s))
        gruppe_rf = dict(name='rueckenfleisch', mesh=8, flaechen=list(rf), skal=1.0, schub=list(s_),
                         erlaubt='Texel der Original-Flaechen mit Zeile 480, Seite 0, ohne Feld C', kachel_original=k_orig)
    # 3b) dieselbe Gruppe nimmt die Flaechen auf, die von vorn kraeftig rot ueber dem Kopf liegen (Sicht-Pruefer r3 (2): "rote Pixel ueber der
    #     Kopfoberkante in P1/F354 hoechstens auf V7-Niveau (0)"; gemessen sfv8/r4: ohne Huegel-Schub 5 px aus d50/v0, Fleisch am oberen Ruecken)
    for _runde in range(6):
        g = gruppen + ([gruppe_rf] if gruppe_rf else [])
        mo = V.modell(anwenden(geo, par_mit(ohne(fl1, '8', rf), g)))
        zahl = {}
        for pose in (R3.P1, sp_[354]):
            for k in kappe_flaechen(mo, pose):
                if k not in hf0 and k not in rf:
                    zahl[k] = zahl.get(k, 0) + 1
        log('Kappe von vorn (ohne Huegel): %s' % zahl)
        if not zahl:
            break
        rf.append(max(sorted(zahl), key=lambda k: zahl[k]))
        uvs = [AV.flaeche(ms[8], k) for k in rf]
        sh, Gm, po = schuebe(uvs, E0) if len(set(uv[5] & 0xF for uv in uvs)) == 1 else ([], None, 0)
        if not sh:
            log('Rueckenfleisch: kein Schub fuer', rf); rf.pop(); break
        s_ = min(sh, key=lambda s: haut_wahl(a0, Gm, s))
        gruppe_rf = dict(name='rueckenfleisch', mesh=8, flaechen=list(rf), skal=1.0, schub=list(s_),
                         erlaubt='Texel der Original-Flaechen mit Zeile 480, Seite 0, ohne Feld C', kachel_original=k_orig)
    if gruppe_rf:
        gruppen.append(gruppe_rf)
        fl1 = ohne(fl1, '8', rf)
    # 4) Huegel (Mesh 8)
    hf, hzahl = huegel_flaechen(anwenden(geo, par_mit(fl1, gruppen)))
    log('Huegel-Flaechen', hf, hzahl)
    gh = [AV.flaeche(ms[8], k) for k in hf]
    sh, Gm, po = schuebe(gh, E0)
    vor = []
    for du, dv in sh:
        a = a0[:, :128][np.roll(np.roll(Gm, dv, 0), du, 1)]
        vor.append((round(float(((a >= 10) & (a < 30)).mean()), 3), round(float((a >= 30).mean()), 3), du, dv))
    vor = [v for v in vor if v[1] <= 0.15]
    vor.sort(key=lambda v: (-v[0], v[2] * v[2] + v[3] * v[3]))
    log('Huegel: %d Schuebe, %d mit a*>=30 <= 15 %%' % (len(sh), len(vor)))
    fl2 = ohne(fl1, '8', hf)
    herg = []
    for iv, v in enumerate(vor[:400]):                  # in Bloecken zu 80, bis >= 5 Schuebe alle Bedingungen erfuellen
        if iv and iv % 80 == 0 and sum(1 for e in herg if e[3] <= KAPPE_MAX and e[4] <= 0 and e[2] >= HUEGEL_MIN) >= 5:
            break
        g = gruppen + [dict(name='huegel', mesh=8, flaechen=hf, skal=1.0, schub=[v[2], v[3]])]
        bl = anwenden(geo, par_mit(fl2, g))
        mo = V.modell(bl)
        r, s3f, _A = rot_pv(bl)
        kp = max(kappe_vorn(mo, R3.P1), kappe_vorn(mo, R3.szene_posen()[354]))
        kh = max(kachel_hinten(mo, R3.szene_posen()[f]) - k_orig[f] for f in (328, 332))
        herg.append((v[2], v[3], s3f, kp, kh, r['r8z']))
    json.dump(herg, open(os.path.join(V.SFV8, 'adern_r4_huegel.json'), 'w'))
    ok = [e for e in herg if e[3] <= KAPPE_MAX and e[4] <= 0 and e[2] >= HUEGEL_MIN]
    log('Huegel: %d gerendert, %d erfuellen (Kappe <= %d, Kachel <= Original %s, S3f >= %.0f)' % (len(herg), len(ok), KAPPE_MAX, k_orig, HUEGEL_MIN))
    best = min(ok, key=lambda e: (round(abs(e[2] - HUEGEL_VORLAGE), 1), e[0] * e[0] + e[1] * e[1], (e[0], e[1])))
    log('Huegel Wahl', best)
    gruppen.append(dict(name='huegel', mesh=8, flaechen=hf, skal=1.0, schub=[best[0], best[1]], s3f=best[2], kappe=best[3], kachel=best[4],
                        kachel_original=k_orig, erlaubt='Texel der Original-Flaechen mit Zeile 480, Seite 0, ohne Feld C'))
    # 5) Rumpf (W)-rechts gegen das Original
    par = par_mit(fl2, gruppen)
    r, _s3f, _A = rot_pv(anwenden(geo, par))
    ro, _x, _y = rot_pv(BF.lade_em30())
    log('Rumpf (W)-rechts %.1f, Original %.1f' % (r['r8z'], ro['r8z']))
    import riegel_v8 as RG
    fo = {k: RG.rote_flecken(BF.lade_em30(), knochen=k) for k in ((8, 13), (8, 13, 14))}
    dichte = lambda t_: 1000.0 * t_[0] / max(t_[2], 1)

    def flecken_ueber(bl):
        """Ueberschuss der roten Flecken je 1000 px gegen das Original (Riegel S7d, beide Knochen-Sets); <= 0 = erfuellt."""
        return max(dichte(RG.rote_flecken(bl, knochen=k)) - dichte(fo[k]) for k in fo)

    sf, g_bl = [], None
    bl = anwenden(geo, par)
    fu = flecken_ueber(bl)
    log('Flecken-Ueberschuss je 1000 px %.2f' % fu)
    for _runde in range(16):
        if r['r8z'] <= ro['r8z'] - RUMPF_RAND and fu <= -FLECKEN_RAND:
            break
        if r['r8z'] <= RUMPF_MIN:
            log('Blass (W)-rechts: Rumpf (W)-rechts erreicht die Untergrenze'); break
        g = gruppen + ([g_bl] if g_bl else [])
        bl = anwenden(geo, par_mit(ohne(fl2, '8', sf), g))
        if r['r8z'] > ro['r8z'] - RUMPF_RAND:
            rotz = rot_flaechen_rechts(bl)                                  # Rotanteil zuerst
        else:
            rotz = flecken_flaechen(bl)                                     # dann die Flaechen mit den meisten roten Flecken
        kand = [k for k in sorted(rotz, key=lambda k: (-rotz[k], k)) if k not in hf and k not in sf and k not in rf]
        neu = None
        for k in kand:
            uvs = [AV.flaeche(ms[8], q) for q in sf + [k]]
            if len(set(uv[5] & 0xF for uv in uvs)) > 1:
                continue
            sh, Gm, po = schuebe(uvs, E0)
            if not sh:
                continue
            s_ = min(sh, key=lambda s: haut_wahl(a0, Gm, s))
            neu = (k, s_)
            break
        if neu is None:
            log('Blass (W)-rechts: keine weitere Flaeche mit Schub'); break
        sf.append(neu[0])
        g_bl = dict(name='blass_rechts', mesh=8, flaechen=list(sf), skal=1.0, schub=list(neu[1]),
                    erlaubt='Texel der Original-Flaechen mit Zeile 480, Seite 0, ohne Feld C')
        bl = anwenden(geo, par_mit(ohne(fl2, '8', sf), gruppen + [g_bl]))
        r, _s, _A = rot_pv(bl)
        fu = flecken_ueber(bl)
        log('Blass (W)-rechts: %s -> Rumpf (W)-rechts %.1f, Flecken-Ueberschuss %.2f' % (sf, r['r8z'], fu))
    if g_bl:
        gruppen = gruppen + [g_bl]
        fl2 = ohne(fl2, '8', sf)
    return par_mit(fl2, gruppen)


# ------------------------------------------------------------------ Herkunft (Riegel S7a Runde 4)
def herkunft_r4(blob, ref_blob):
    """Riegel S7a Runde 4 (O1 "jeder Eintrag und jeder Texel stammt aus dem Original-EM30"; Zahlen-Pruefer r3 O2 "Ziel-Texel der UV-Schuebe
    nutzt keine Original-Flaeche"): (1) Zeilen 480 .. 480 + n(V7) - 1 bytegleich V7; (2) neue Zeilen = Original-EM30-Zeile 480 + k (15 Bit),
    hoechstens 3; (3) alle Texel bytegleich V7; (4) jede Flaeche der Meshes 8/13/14 auf einer neuen Zeile: alle Texel unter ihr = Original-Texel
    UND von einer Original-Flaeche mit derselben Original-Zeile benutzt; (5) keine Flaeche auf einer neuen Zeile in Feld C; (6) Flaechen mit
    geaenderter UV nur auf neuen Zeilen."""
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, timr = KB.teile(ref_blob)
    t, tr = KB.tim_teile(tim), KB.tim_teile(timr)
    _MO, TO = original()
    nr = tr['clut'].shape[0]
    zeilen_ok = bool(np.array_equal(t['clut'][:nr], tr['clut']))
    neu = t['clut'][nr:]
    neu_ok = len(neu) <= 3 and all(np.array_equal(neu[k] & 0x7FFF, TO['clut'][k] & 0x7FFF) for k in range(len(neu)))
    pix_ok = bool(np.array_equal(t['pix'], tr['pix']))
    nutz = {k: nutzung(None, 480 + k) for k in range(len(neu))}
    n_fl = n_tex = n_fremd = n_anders = 0
    tex_ok, c_fehler, uv_fehler = True, 0, 0
    for b in (8, 13, 14):
        for key, uv in keys(ms[b]):
            ur = AV.flaeche(mr[b], key)
            anders = [uv[i] for i in range(len(uv)) if i % 3 != 2] != [ur[i] for i in range(len(ur)) if i % 3 != 2]
            z = (uv[2] >> 6) & 0x1FF
            if anders:
                n_anders += 1
                if z < 480 + nr:
                    uv_fehler += 1
            if z < 480 + nr:
                continue
            n_fl += 1
            if A3.in_feld_c(uv):
                c_fehler += 1
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            n_tex += int(mk.sum())
            if not np.array_equal(t['pix'][mk], TO['pix'][mk]):
                tex_ok = False
            n_fremd += int((mk & ~nutz[z - 480 - nr]).sum())
    return dict(alte_zeilen_gleich=zeilen_ok, neue_zeilen_original=bool(neu_ok), neue_zeilen=int(len(neu)), texel_bytegleich_v7=pix_ok,
                flaechen_neue_zeilen=n_fl, texel_unter_neuen=n_tex, texel_original=tex_ok, texel_ohne_original_flaeche=n_fremd,
                uv_geaenderte_flaechen=n_anders, uv_fehler=uv_fehler, feld_c_fehler=c_fehler,
                ok=bool(zeilen_ok and neu_ok and pix_ok and tex_ok and c_fehler == 0 and uv_fehler == 0 and n_fremd == 0))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'))
    ap.add_argument('--kette', default=os.path.join(HIER, 'kette_v8.json'))
    ap.add_argument('--ruecken', default=os.path.join(HIER, 'ruecken_v8.json'))
    ap.add_argument('--aus', default=os.path.join(HIER, 'adern_v8.json'))
    ap.add_argument('--feld-c', help='du,dv aus einem frueheren Lauf (nur zum Nachrechnen)')
    a = ap.parse_args()
    import v8_bau as VB
    basis = open(a.basis, 'rb').read()
    assert V.sha(basis) == V.V7_SHA, 'Basis muss V7 sein'
    geo, _p = VB.bauen(a.basis, 'srw', kette=json.load(open(a.kette)), ruecken=json.load(open(a.ruecken)))
    par = suche(geo, log=lambda *x: print(*x, flush=True), feld_c=[int(v) for v in a.feld_c.split(',')] if a.feld_c else None)
    par['quelle'] = ('adern_r4.py Runde 4 (Basis V7 %s, Kette %s, Ruecken %s): Mesh 8 ausser Brust auf Original-Zeilen, Gruppen Feld C / '
                     'Unterarmband / Rueckenfleisch / Huegel / blass (W)-rechts mit einem Schub auf Texel von Original-Flaechen' % (
                         V.V7_SHA[:8], os.path.basename(a.kette), os.path.basename(a.ruecken)))
    json.dump(par, open(a.aus, 'w'), indent=1)
    print({k: len(v) for k, v in par['flaechen'].items()}, [(g['name'], len(g['flaechen']), g.get('skal'), g['schub']) for g in par['gruppen']])


if __name__ == '__main__':
    main()
