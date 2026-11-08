"""SF V6 drei Punkte (Bauer, 2026-11-08): Haar ergraut, Schwanz bis zur Nutzer-Spitze, Kralle (W)-links hell.
Basis = EM30_V5f.EMD (sha256 784c8f05, Kopf V6 + Koerper V5f).

    python drei_bau.py [--basis EMD] [--aus DIR] [--json PROT] [--stufen hsk] [--name NAME] [--laengen JSON] [--radien-frei]
        -> DIR/EM30_V6.EMD (+ EM30_V4.EMD darauf, wenn --name fehlt und alle Stufen gebaut werden)

Nutzer 2026-11-08 (woertlich): "Ja, sieht insgesamt schon recht cool aus. Mache mal die 3 von dir genannten Punkte noch, und wir schauen
sie uns an." Die drei Punkte (Vorschlag des Orchestrators, vom Nutzer angenommen): 1. Haar ergrauen wie den Fleck, Struktur bleibt;
2. Schwanz bis zum Nutzerpunkt "Schwanzspitze Ende" verlaengern; 3. Kralle (W)-links aufhellen.

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Masse = Bildmessung an den Fotos
(SF_MESSBLATT_FOTO.md) und am Modell.

Stufe h) Haar (nur Texel, die ausschliesslich Mesh 9 nutzt, und nur Texel, die V6 nicht schon geaendert hat): Haarfeld = 8-zusammenhaengende
   Komponente(n) der gelblichen Texel (Paletten-b* >= HAAR_B in Zeile 486 = Original-Zeile 0) im Haarbereich (v <= 172 ueber den ganzen
   Kopf, u <= 76 bis v 186 = (W)-rechte Kopfseite und Schlaefe ueber Fleck und Braue, u <= 57 = Straehnen am linken Rand), die die obersten Haarzeilen
   (v <= 150) beruehren, mit gefuellten Loechern (dunkle Straehnen im Haar). Jeder Haartexel behaelt seinen Helligkeitsrang (L* der
   Zeile 486) unter den Haartexeln und bekommt den Index des grauen Flecks mit demselben Rang (wie Zone A, texel_erweitern: Fleck = Box
   u 58-74 v 186-226, L* <= 50, b* <= 10). Palette unveraendert.
Stufe s) Schwanz: Gliedlaengen 19-22 (relpos 20, 21, 22 im EMR = Laengen der Glieder 19, 20, 21; Glied 22 = Endring von Mesh 22) nach
   schwanz_laengen_v6.json (Suche schwanz_laenge_suche.py: Spitze, Beginn, Knick, Biegung in P2 an den Nutzerpunkten, Glieder 16-18
   unveraendert); Keyframe-Winkel, Bindversatz Knochen 16 und Kinderlisten bytegleich. Mesh k: Endring (und Apex in 22) entlang der
   Gliedachse um die Laengenaenderung verschoben (Form/Kippung des Rings bleibt), danach Radien je Gelenk mit schwanz_form.passen auf die
   Foto-Breite in P2 (wie V5f; Kette bis zum Endring von Mesh 22, drei_gemein.kette_punkte), bauch (Glied 18) wie V5f; Manschette wie V5f;
   danach Rohrenden mit flachen Deckeln geschlossen (kappen, Begruendung dort).
Stufe k) Kralle (W)-links: neue CLUT-Zeile 491 = Kopie der Zeile 490 (Klauenhand); nur die Krallenflaechen von Mesh 12 (alle Ecken
   y >= 330) zeigen darauf (CLUT-Feld). In 491 bekommt jeder Index, den die Krallentexel tragen, einen Eintrag aus der Referenz-2-Palette
   (Zeilen 480-485 aus EM30_HANGAR.EMD) oder der Original-Zeile 0: Kandidaten = graue/knochenfarbene Eintraege (C* <= 12, a* >= 0);
   Ziel je Index = (Paletten-L* + Verschiebung, Foto-a*/b* 4,9 / 1,7), naechster Kandidat (dE76). Die Verschiebung wird per Halbierung
   so gesucht, dass die Krallenpixel in P1 (Videoraum) im Mittel die Foto-L* treffen (Messblatt 6: Keil L* 30,3 / a* 4,9 / b* 1,7).
   Texel unveraendert; die Krallentexel teilt die Klauenhand mit Mesh 11/14/15 - die behalten ihre Zeilen (nichts anderes wird heller).
"""
import os, sys, json, argparse, hashlib, struct, copy
import numpy as np
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import drei_gemein as DG          # noqa: E402
import kopf_orig_bau as KB          # noqa: E402
import koerper_bau as KBAU          # noqa: E402
import texel_erweitern as TE        # noqa: E402
import schwanz_form as SF           # noqa: E402

BASIS = os.environ.get('RE15_V6D_BASIS', os.path.join(KB.AUS, 'EM30_V5f.EMD'))     # sha256 784c8f05 (V5f)
AUS = KB.AUS
Z_KOPF, Z_HAND, Z_KRALLE = 486, 490, 491
HAAR_B = 6.0
HAAR_BEREICH = dict(v_oben=172, u_seite=76, v_seite=186, u_rand=57, v_start=150)
KRALLE_FOTO = (30.3, 4.9, 1.7)       # Messblatt 6: heller Krallenkeil n1476 x 481-492 y 350-360
KRALLE_KAND_C, KRALLE_KAND_A = 12.0, 0.0


def sha(b):
    return hashlib.sha256(b).hexdigest()


def _param(n):
    j = json.load(open(os.path.join(HIER, n)))
    return {int(k) if str(k).isdigit() else k: v for k, v in j.items()}


def kopf_frei(ms):
    nutz = KB.texel_nutzung(ms)
    fremd = np.zeros_like(nutz[9])
    for i, mk in nutz.items():
        if i != 9:
            fremd |= mk
    return nutz[9] & ~fremd


def original_pix():
    o, _m9, _to = KB.original_kopf()
    return KB.tim_teile(KB.teile(o)[3])['pix']


# ------------------------------------------------------------------ h) Haar
def haar_maske(pix, lab, frei, geaendert):
    p = HAAR_BEREICH
    vv, uu = np.indices(pix.shape)
    bereich = (vv <= p['v_oben']) | ((uu <= p['u_seite']) & (vv <= p['v_seite'])) | (uu <= p['u_rand'])
    bereich &= frei & ~geaendert
    gelb = bereich & (lab[pix][..., 2] >= HAAR_B)
    kom, _n = ndimage.label(gelb, structure=np.ones((3, 3)))
    oben = set(int(x) for x in np.unique(kom[(vv <= p['v_start']) & gelb])) - {0}
    m = np.isin(kom, sorted(oben))
    return ndimage.binary_fill_holes(m) & bereich


def haar(t, ms):
    """Haartexel -> Fleck-Indizes gleichen Helligkeitsrangs. Rueckgabe (pix_neu, prot, maske)."""
    pix = t['pix'].copy()
    frei = kopf_frei(ms)
    po = original_pix()
    geaendert = pix != po
    lab = TE.pal_lab(t['clut'][Z_KOPF - 480])
    L = lab[pix][..., 0]
    # Fleck wie texel_erweitern (Zone A): aus der ORIGINAL-Textur, Box u 58-74 v 186-226, L* <= 50, b* <= 10, nur Mesh 9
    Lo, Bo = lab[po][..., 0], lab[po][..., 2]
    x0, y0, x1, y1 = TE.PARAM['fleck_box']
    fb = np.zeros(pix.shape, bool); fb[y0:y1 + 1, x0:x1 + 1] = True
    fleck = fb & (Lo <= TE.PARAM['fleck_l']) & (Bo <= TE.PARAM['haar_b']) & frei
    f_sort = po[fleck][np.argsort(Lo[fleck], kind='stable')]
    m = haar_maske(pix, lab, frei, geaendert) & ~fleck
    ya, xa = np.nonzero(m)
    rang = np.argsort(np.argsort(L[ya, xa], kind='stable'), kind='stable')
    q = (rang + 0.5) / max(len(rang), 1)
    alt = pix.copy()
    pix[ya, xa] = f_sort[np.minimum((q * len(f_sort)).astype(int), len(f_sort) - 1)]
    g = pix != alt
    lab_alt, lab_neu = lab[alt[m]], lab[pix[m]]
    prot = dict(regel=dict(HAAR_BEREICH, haar_b=HAAR_B), haartexel=int(m.sum()), geaendert=int(g.sum()),
                box=[int(xa.min()), int(ya.min()), int(xa.max()), int(ya.max())] if len(xa) else None,
                fleck_texel=int(fleck.sum()), fleck_indizes=sorted(set(int(i) for i in f_sort)),
                nicht_frei=int((g & ~frei).sum()), schon_v6=int((g & geaendert).sum()),
                lab_alt_p50=[round(float(x), 1) for x in np.median(lab_alt, 0)], lab_neu_p50=[round(float(x), 1) for x in np.median(lab_neu, 0)],
                b_alt_p90=round(float(np.percentile(lab_alt[:, 2], 90)), 1), b_neu_p90=round(float(np.percentile(lab_neu[:, 2], 90)), 1))
    return pix, prot, m


# ------------------------------------------------------------------ s) Schwanz
def emr_laengen(blob, d, laengen):
    """relpos (0, L, 0) der Knochen glied + 1 im EMR (Sektion 2) setzen: laengen = {glied: L} fuer glied 16..21."""
    b = bytearray(blob)
    alt = {}
    for glied, L in laengen.items():
        o = d[2] + 8 + (glied + 1) * 6
        x, y, z = struct.unpack_from('<3h', b, o)
        assert x == 0 and z == 0, (glied + 1, x, y, z)
        alt[glied] = y
        struct.pack_into('<3h', b, o, 0, int(L), 0)
    return bytes(b), alt


def glied_laengen(ms, laengen):
    """Endring von Mesh k (und Apex in 22) um L_neu - L_alt entlang y verschieben (Kopie)."""
    ms = copy.deepcopy(ms)
    for glied, L in laengen.items():
        m = ms[glied]
        rg = SF.ringe(m)
        voll = [x for x in rg if len(x[0]) == SF.S_]
        idx_end, c_end, _r = voll[-1]
        dy = float(L) - c_end[1]
        for i in idx_end:
            m['v'][i][1] = int(round(m['v'][i][1] + dy))
        for idx, c, _r in rg:
            if len(idx) == 1 and c[1] > 0:
                m['v'][idx[0]][1] = int(round(m['v'][idx[0]][1] + dy))
    return ms


def schwanz(blob, d, ms, tim, laengen, radien=None):
    prot = {}
    l22 = [x for x in SF.ringe(ms[22]) if len(x[0]) == SF.S_][-1][1][1]
    blob2, alt = emr_laengen(blob, d, {g: L for g, L in laengen.items() if g < 22})
    prot['laengen_alt'] = {str(g): int(alt.get(g, round(l22))) for g in laengen}
    prot['laengen_neu'] = {str(g): int(L) for g, L in laengen.items()}
    ms2 = glied_laengen(ms, laengen)
    pack = lambda m_: KB.packen(blob2, d, m_, tim)
    bauch = KBAU.SCHWANZ_PARAM['bauch']                 # V5f: Zwischenring Glied 18 (Lage, Anteil, Versatz) unveraendert
    setzen0 = SF.setzen
    SF.setzen = lambda m_, rad: setzen0(m_, dict(rad, bauch=rad.get('bauch', bauch)))
    try:
        if radien is None:
            ms3, pp = SF.passen(ms2, pack)
            prot['radien'] = pp
            prot['radien']['radien_neu']['bauch'] = bauch
        else:
            ms3 = SF.setzen(ms2, radien)
            prot['radien'] = dict(radien_neu={str(k): v for k, v in dict(radien, bauch=radien.get('bauch', bauch)).items()})
    finally:
        SF.setzen = setzen0
    ms3, prot['kappen'] = kappen(ms3)
    return blob2, ms3, prot


def kappen(ms):
    """Rohrenden schliessen (Spalten-Pruefung spalten.py: bei Knicken ueber ~27 Grad schaut ein offenes Rohrende aus dem Nachbarglied -
    V5f 72 von 200 Bildern, Leiste sfv7/sp4_EM30_V5f.EMD_tafel.png): Glied 16-21 bekommen am Endring, Glied 17-22 am Anfangsring
    (Manschette) einen flachen Deckel aus 11 Dreiecken um eine neue Ecke in der Ringmitte (Normale +-y). Umlauf wie die Mantelflaechen
    (geometrische Normale (v1-v0)x(v2-v0) zeigt nach innen, gemessen an allen Mantelflaechen der Glieder 16-22), UV/CLUT/TPage der
    angrenzenden Mantelflaeche (Schwanzfeld, Zeile 487). Die Deckel liegen im Nachbarglied; sichtbar werden sie nur dort, wo vorher die
    Innenwand zu sehen war. Ringe/Manschette/Radien bleiben wie gesetzt."""
    ms = copy.deepcopy(ms)
    liste = []
    for g in DG.SCHWANZ:
        m = ms[g]
        voll = [x for x in SF.ringe(m) if len(x[0]) == SF.S_]
        P = np.array([v[:3] for v in m['v']], float)
        mantel = list(zip(m['qf'], m['quv'])) + list(zip(m['tf'], m['tuv']))
        for art, (idx, c, _r) in (('ende', voll[-1]), ('anfang', voll[0])):
            if (art == 'ende' and g == 22) or (art == 'anfang' and g == 16):
                continue
            a = len(m['v'])
            m['v'].append([int(round(c[0])), int(round(c[1])), int(round(c[2])), 0])
            m['n'].append([0, 4096 if art == 'ende' else -4096, 0, 0])
            achse = np.array([0.0, 1.0 if art == 'ende' else -1.0, 0.0])
            for k in range(SF.S_):
                i0, i1 = idx[k], idx[(k + 1) % SF.S_]
                uv_q = next(uv for f, uv in mantel if i0 in f[1::2] and i1 in f[1::2])
                n_ = np.cross(P[i1] - P[i0], c - P[i0])
                f = [i0, i0, i1, i1, a, a] if n_ @ achse < 0 else [i1, i1, i0, i0, a, a]
                m['tf'].append(f)
                m['tuv'].append([uv_q[0], uv_q[1], uv_q[2], uv_q[3], uv_q[4], uv_q[5], uv_q[6], uv_q[7], 0])
            liste.append(dict(glied=g, art=art, ecke=a, mitte=[round(float(x), 1) for x in c]))
    return ms, liste


# ------------------------------------------------------------------ k) Kralle
def kralle_kandidaten():
    ref2 = open(KB.REF2, 'rb').read()
    tr = KB.tim_teile(KB.teile(ref2)[3])
    _o, _m9, to = KB.original_kopf()
    werte = [int(v) for z in tr['clut'] for v in z] + [int(v) for v in to['clut'][0]]
    werte = sorted(set(v & 0x7FFF for v in werte if v & 0x7FFF))
    lab_ = KBAU.lab15(np.array(werte))
    C = np.hypot(lab_[:, 1], lab_[:, 2])
    ok = (C <= KRALLE_KAND_C) & (lab_[:, 1] >= KRALLE_KAND_A)
    return np.array(werte)[ok], lab_[ok]


def kralle_zeile(zeile490, idx, shift, kand):
    werte, klab = kand
    neu = zeile490.copy()
    lab_ = KBAU.lab15(zeile490[idx] & 0x7FFF)
    for ix, l in zip(idx, lab_):
        ziel = np.array([l[0] + shift, KRALLE_FOTO[1], KRALLE_FOTO[2]])
        j = int(np.argmin(np.linalg.norm(klab - ziel, axis=1)))
        neu[ix] = (int(werte[j]) & 0x7FFF) | (int(zeile490[ix]) & 0x8000)
    return neu


def kralle_pixel(blob, pose_name='P1'):
    """(Lab-Mittel Videoraum, Pixelzahl, Maske, Lab-Bild, Render-Dict) der Krallenflaechen von Mesh 12 in der Pose."""
    import f_rast as FR
    mo = DG.V.modell(blob)
    A = FR.mit_bg(mo, DG.V.posen()[pose_name])
    _d, _, ms, _t = KB.teile(blob)
    kk = set(DG.krallen_flaechen(ms[12]))
    fl = A['flaeche'][:480]
    sel = (A['bone'][:480] == 12) & np.vectorize(lambda s: s in kk)(fl)
    Lm = DG.G.lab(DG.G.videoraum(A['img'][:480]))
    return (Lm[sel].mean(0) if sel.any() else np.full(3, np.nan)), int(sel.sum()), sel, Lm, A


def kralle(blob, d, ms, t):
    """Zeile 491 + CLUT-Feld der Krallenflaechen; Verschiebung per Halbierung auf Foto-L*."""
    kl = DG.krallen_flaechen(ms[12])
    mk = np.zeros(t['pix'].shape, bool)
    for k, uv in KBAU.flaechen_schluessel(ms[12]):
        if k in kl:
            mk |= KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            KBAU.setze_zeile(uv, Z_KRALLE)
    idx = sorted(set(int(x) for x in t['pix'][mk]))
    kand = kralle_kandidaten()
    z490 = t['clut'][Z_HAND - 480].copy()
    clut0 = t['clut'].copy()
    lo, hi = 0.0, 60.0
    verlauf = []
    for _ in range(12):
        sh = (lo + hi) / 2
        t['clut'] = np.vstack([clut0, kralle_zeile(z490, idx, sh, kand)[None]])
        b_ = KB.packen(blob, d, ms, KB.tim_packen(t))
        lab_, n, _sel, _L, _A = kralle_pixel(b_)
        verlauf.append(dict(verschiebung=round(sh, 3), lab=[round(float(x), 2) for x in lab_], px=n))
        if lab_[0] < KRALLE_FOTO[0]:
            lo = sh
        else:
            hi = sh
    sh = min(verlauf, key=lambda v: abs(v['lab'][0] - KRALLE_FOTO[0]))['verschiebung']
    z491 = kralle_zeile(z490, idx, sh, kand)
    t['clut'] = np.vstack([clut0, z491[None]])
    return dict(flaechen=kl, texel=int(mk.sum()), indizes=len(idx), kandidaten=int(len(kand[0])), verschiebung=sh, verlauf=verlauf,
                pal_alt_p50=[round(float(x), 1) for x in np.median(KBAU.lab15(z490[idx] & 0x7FFF), 0)],
                pal_neu_p50=[round(float(x), 1) for x in np.median(KBAU.lab15(z491[idx] & 0x7FFF), 0)])


def bauen(basis=BASIS, stufen='hsk', laengen=None, radien='datei'):
    blob = open(basis, 'rb').read()
    d, _do, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    assert t['clut'].shape[0] == 11, 'Basis hat %d CLUT-Zeilen (erwartet 11 = V5f)' % t['clut'].shape[0]
    prot = dict(basis=basis, basis_sha256=sha(blob), stufen=stufen)
    if 'h' in stufen:
        t['pix'], prot['haar'], _m = haar(t, ms)
    if 's' in stufen:
        L = laengen or _param('schwanz_laengen_v6.json')
        R_ = radien
        if isinstance(R_, str):
            f_ = os.path.join(HIER, 'schwanz_radien_v6.json')
            R_ = _param('schwanz_radien_v6.json') if os.path.exists(f_) else None
        blob, ms, prot['schwanz'] = schwanz(blob, d, ms, KB.tim_packen(t), {int(k): v for k, v in L.items()}, R_)
    if 'k' in stufen:
        prot['kralle'] = kralle(blob, d, ms, t)
    tim_neu = KB.tim_packen(t)
    neu = KB.packen(blob, d, ms, tim_neu)
    prot['tim'] = dict(clut_zeilen=int(t['clut'].shape[0]), port_texturhoehe=int(t['ih'] * t['clut'].shape[0]))
    prot['emd'] = dict(bytes=len(neu), sha256=sha(neu))
    return neu, prot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=BASIS); ap.add_argument('--aus', default=AUS); ap.add_argument('--json')
    ap.add_argument('--stufen', default='hsk'); ap.add_argument('--name')
    ap.add_argument('--laengen', help='JSON Glied -> Laenge (Standard schwanz_laengen_v6.json)')
    ap.add_argument('--radien-frei', action='store_true', help='Radien neu mit schwanz_form.passen (statt schwanz_radien_v6.json)')
    a = ap.parse_args()
    L = json.load(open(a.laengen)) if a.laengen else None
    neu, prot = bauen(a.basis, a.stufen, L, None if a.radien_frei else 'datei')
    name = a.name or 'EM30_V6.EMD'
    os.makedirs(a.aus, exist_ok=True)
    open(os.path.join(a.aus, name), 'wb').write(neu)
    if not a.name and a.stufen == 'hsk':
        open(os.path.join(a.aus, 'EM30_V4.EMD'), 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s)' % (os.path.join(a.aus, name), len(neu), prot['emd']['sha256']))
    print('TIM', prot['tim'])
    for k in ('haar', 'kralle'):
        if k in prot:
            print(k, {kk: v for kk, v in prot[k].items() if kk not in ('verlauf', 'fleck_indizes', 'flaechen')})
    if 'schwanz' in prot:
        print('schwanz', prot['schwanz']['laengen_alt'], '->', prot['schwanz']['laengen_neu'], prot['schwanz']['radien'].get('radien_neu'))
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
