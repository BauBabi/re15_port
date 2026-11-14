"""riegel_r7c.py - Riegel der Runde 7c (Einbau der P5-Stufe, Pflicht-Korrekturen K-a..K-e) fuer riegel_v8.py (Option --r7c-*).

Die Form existiert auf keiner Disc: jede Zahl hier ist PORT-WAHL und traegt den Wortlaut des Auftrags (Entscheide E9-E13 des Orchestrators,
2026-11-13) oder eine Messung (sfv8/r7c/NOTIZ.md). Neue Datei; riegel_r6.py und die aelteren Riegel bleiben in ihrer Wirkung unveraendert -
diese Datei liest deren Ergebnisse und setzt das Urteil nach E9-E13 daneben (Zeilen "Runde 7c"); die SUMME zaehlt dann diese Urteile.

S16 (E9)  Luecke Ruecken/Kopf. Vorderansicht = Kopf-Vorwaertsachse (Knochen 9, lokales x) zur Kamera cos >= 0,4 (E9; riegel_r6.vorn_cos am
          V7-Modell). Volles Soll (21 px / 7 tief / 3,2 px, luecke_r5) mit der Einzelbild-Regel E3 nur fuer Seiten- und Rueckansichten
          (cos < 0,4) und die Foto-Posen PV/P1/P2; die Vorderansichten als KLASSE gegen V7 (Anzahl ueber Soll und deren Summe, offline und in
          echten Bild-Dumps, hoechstens wie V7, gemessen mit demselben Code im selben Lauf). Gedrehte Foto-Posen ohne Szenen-Gegenstueck
          (PV+k, P2+k) mit cos < 0,4 sind GELB (E13, mit Zahl). Ein Szenenbild, das V7 im Soll hat und dessen Kerbe bei V7 von derselben
          Silhouette begrenzt ist (Spaltenprofil, artefakt()) - die Kerbe zaehlt nur ueber die Klassen der Randpunkte -, ist ein Mess-Artefakt
          (E13: genannt, nicht gewertet).
          In jeder Ansicht verboten: helle Zacke mit dunkler V-Kerbe direkt neben dem Kopf in zwei oder mehr Bildern in Folge (E9). Mass:
          Kerben-Stueck rechts neben dem Kopf mit mehr als 21 Bildpunkten UND groesster Tiefe ueber 3,2 px (man sieht hinein: luecke_r5
          SOLL_SUMME / SOLL_MAXTIEFE), dessen Mesh-8-Wand jenseits des Stuecks (4 px) im echten Bild mittlere Helligkeit L >= L_HELL hat;
          Folge = zwei aufeinanderfolgende Framedumps.
          (b)/(c) verdeckte V7-Gesichts-Bildpunkte: durch Mesh 8 = 0 (offline 177 Ansichten und echte Bilder F225-F450, Auftrag K-c);
          durch andere Meshes = Erbe (E12), hoechstens wie der Eingang.
S18 (E10/E11/E12)  (a1) wie riegel_r6.s18, aber die Ecken 17, 30 und 46 von Mesh 8 sind frei (E10), wenn: Kragen 0 (Mesh-8-Bildpunkte im
          Kopffenster von vorn mit Ort y8 < -1270) UND Rahmen 0 (Mesh 9 ohne angehaengte Flaechen) UND Buckel-Rot S3 (f) >= 50 % UND 0 von
          Mesh 8 verdeckte Gesichts-Bildpunkte gegen V7 UND keine Kappe (S11 a) UND Kopf ruhig (Kippen A-B-A im Kopffenster <= Eingang).
          (a3) Ergaenzungsdreiecke, die ein Viereck mit gleichen Ecken, Normalen, Texeln und Zeile vervollstaendigen, zaehlen als vorhandene
          Flaechen mit r4-Gegenstueck (E11). (b) helle Bildpunkte (420,200)-(640,400) F344-F378: ROT nur bei einem zusammenhaengenden hellen
          Keil >= 20 Bildpunkte, die r4 im selben Bild nicht hat (E12); einzelne helle Punkte = GELB mit Zahl.
S3 / S9 (E12)  Pruefpunkte, die schon im Eingang ROT sind und nachweislich aus Meshes ausser 8/9 kommen (S3 (e) K, S9 F354), sind GELB "Erbe",
          solange sie nicht schlechter als der Eingang sind und alle anderen Teilpunkte gruen sind.
S21 (neu, K-a)  (a) fremde Texel in Mesh 8: (Zeile, Texel) unter Mesh-8-Flaechen, die weder der Eingang (Stand 7b) noch V8 r4 noch V7 (alle
          Meshes) auf derselben Zeile benutzen (ZULASSUNG: ausdruecklich zugelassene Texel, derzeit keine): Soll 0. (b) oranger Fleck: Bilder F225-F450 mit >= ORANGE_MIN orange Bildpunkten
          (Maske des Sicht-Pruefers R3: HSV H 16-40 Grad, S > 0,43, V > 0,59, Kasten x 420-760 / y 180-420), die im Eingangs-Lauf nicht
          im Umkreis von 2 px orange sind: Soll 0.

Marker-Lauf "LM" (lm_bauen, Muster des Pruefers R3 mark.py): Flaechenklassen als deckende CLUT-Farben (Zeilen 480-486, Eintrag 0 bleibt 0):
  0 andere Meshes 480 gruen, 1 Mesh 8 mit Schwerpunkt y8 >= -971 481 rot, 2 Mesh 8 y8 < -971 (Ruecken) 482 blau, 3 Mesh 9 lok x < 150 484
  magenta, 4 Mesh 9 Gesicht x >= 150 485 tuerkis, 5 Mesh 9 angehaengt 486 gelb.   python riegel_r7c.py lm <in.emd> <aus.emd>
"""
import os, sys, subprocess
import numpy as np
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402

EIN_COMMIT = 'db62602be'              # Eingang der Runde 7c = Stand 7b (Spiel-Asset 87b02681, Commit db62602be)
VORN_COS = 0.4                        # E9: "Ansichten mit Kopf-Vorwaertsachse zur Kamera cos >= 0,4"
FOTO = ('PV', 'P1', 'P2')             # E9: volles Soll fuer die Foto-Posen
F_LUECKE = (235, 378)                 # luecke_r5.ansichten: Szene F235-F378
F_SZENE = (225, 450)                  # riegel_r6.s16_szene: F225-F450
VORDERGRUND = (316, 333)              # Pruefer r6_zahlen / Bank P5 (VORDERGRUND): F316-F333 verdeckt der Vordergrund die Figur - keine Luecke
FIG = 30                              # Figur = Summe |LM - plain| > 30 (Pruefer-Zahlen R3 szene.py; Lauf deterministisch, Hintergrund bitgleich)
NAH = 12                              # = luecke_r5.NAH
Y_RUECKEN = -971                      # = luecke_r5.Y_RUECKEN (Kopfgelenk)
GESICHT_X = 150                       # = luecke_r5.GESICHT_X
L_HELL = 84.0                         # Zacke: Mesh-8-Wand rechts neben dem Kopf, mittlere Helligkeit 0..255 (0,299 R + 0,587 G + 0,114 B).
                                      # Gemessen an den rechten Kerben F304-F313 (sfv8/r7c/pz/zacke2.py): V7 61-78 (Hoechstwert 78,4), R2 90-132
                                      # (Mindestwert 90,3), r4 84-115, r6 93-108; Schwelle zwischen V7 und R2
KRAGEN_Y = -1270                      # Bank P5 K-VORN (a): r4-Hoehe -1263 - 7 E.
E10_ECKEN = (17, 30, 46)              # E10: Vorder- und Seitenkante der Buckelplatte neben dem Kopf
HELL_KEIL = 20                        # E12: ">= 20 zusammenhaengende Bildpunkte, die r4 im selben Bild nicht hat"
ORANGE_MIN = 3                        # oranger Fleck: >= 3 neue orange Bildpunkte in einem Bild (Eingang 7b: groesstes Stueck 4 px, R3 bis 17)
ZULASSUNG = set()                     # (Zeile, Texel-y, Texel-x) ausdruecklich zugelassener Texel; leer: die 3 Texel von q2 aus der UV-Mitfuehrung
                                      # der Ecke 46 (P5 R2/R3) sind seit p5_stufe.KORR_UV q2 wieder Original-Texel (sfv8/r7c NOTIZ)
LM_ZEILE = {0: 480, 1: 481, 2: 482, 3: 484, 4: 485, 5: 486}
LM_FARBE = {480: 0x03E0, 481: 0x001F, 482: 0x7C00, 484: 0x7C1F, 485: 0x7FE0, 486: 0x03FF}


def git_blob(commit):
    return subprocess.check_output(['git', 'show', commit + ':re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD'], cwd=HIER)


# ------------------------------------------------------------------ Marker
def lm_bauen(blob, v7_blob):
    d, _o, ms, tim = KB.teile(blob)
    m79 = KB.teile(v7_blob)[2][9]
    n7 = {'tf': len(m79['tf']), 'qf': len(m79['qf'])}
    for b, m in enumerate(ms):
        for F, U in (('tf', 'tuv'), ('qf', 'quv')):
            for i, f in enumerate(m[F]):
                P = np.array([m['v'][j][:3] for j in f[1::2]], float).mean(0)
                if b == 8:
                    k = 2 if P[1] < Y_RUECKEN else 1
                elif b == 9:
                    k = 5 if i >= n7[F] else (4 if P[0] >= GESICHT_X else 3)
                else:
                    k = 0
                uv = m[U][i]
                uv[2] = (LM_ZEILE[k] << 6) | (uv[2] & 0x3F)
    t = KB.tim_teile(tim)
    c = t['clut']
    for z, col in LM_FARBE.items():
        c[z - 480] = np.where(c[z - 480] == 0, 0, (c[z - 480] & 0x8000) | col)
    return KB.packen(blob, d, ms, KB.tim_packen(t))


def ppm(ordner, f):
    from PIL import Image
    p = os.path.join(ordner, 'f%06d.ppm' % f)
    if not os.path.exists(p):
        p = os.path.join(ordner, 'fd', 'f%06d.ppm' % f)
    return np.asarray(Image.open(p).convert('RGB')).astype(np.int32) if os.path.exists(p) else None


def klassen(lm, plain):
    mx = lm.max(-1)
    on = lm > np.maximum(0.5 * mx, 40)[..., None]
    r, g, b = on[..., 0], on[..., 1], on[..., 2]
    k = np.full(mx.shape, 0, np.int8)
    k[r & ~g & ~b] = 1; k[b & ~r & ~g] = 2; k[r & b & ~g] = 3; k[g & b & ~r] = 4; k[r & g & ~b] = 5; k[g & ~r & ~b] = 0
    k[np.abs(lm - plain).sum(-1) <= FIG] = -1
    return k


def klein(k):
    ys = np.clip(np.round(np.arange(480) * 1.5 + 0.25).astype(int), 0, k.shape[0] - 1)
    xs = np.clip(np.round(np.arange(640) * 1.5 + 0.25).astype(int), 0, k.shape[1] - 1)
    return k[ys][:, xs]


def lauf_klassen(lm_dir, plain_dir, f0, f1):
    out = {}
    for f in range(f0, f1 + 1):
        a, b = ppm(lm_dir, f), ppm(plain_dir, f)
        if a is not None and b is not None:
            out[f] = klassen(a, b)
    return out


# ------------------------------------------------------------------ Luecke aus Klassen (Pruefer-Zahlen R3 szene.mass) + Zacke
def _kerben(k):
    from skimage.morphology import convex_hull_image
    S = k >= 0; H = k >= 3; R = k == 2; m8 = (k == 1) | (k == 2)
    S8 = np.ones((3, 3), bool)
    if not H.any():
        return S, H, R, m8, np.zeros_like(S), []
    dH = ndimage.distance_transform_cdt(~H, metric='chessboard'); Hn = dH <= NAH; Hd = dH <= 1
    hull = convex_hull_image(H | (m8 & Hn), offset_coordinates=False)
    G = np.zeros_like(S); stuecke = []
    lab, n = ndimage.label(hull & ~S, S8)
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if (cd & Hd).any() and (cd & R).any():
            G |= c; stuecke.append(c)
    pad = 4; Sp = np.pad(S, pad)
    zu = ndimage.binary_erosion(ndimage.binary_dilation(Sp, np.ones((5, 5), bool)), np.ones((5, 5), bool))[pad:-pad, pad:-pad]
    lab, n = ndimage.label(zu & ~S & ~G, S8)
    for i in range(1, n + 1):
        c = lab == i; cd = ndimage.binary_dilation(c, S8)
        if (cd & R).any() and (c & Hn).any():
            G |= c
    return S, H, R, m8, G, stuecke


def mass(k):
    S, H, R, m8, G, _st = _kerben(k)
    if not G.any():
        return dict(px=0, tief=0, max_tiefe=0.0)
    d = ndimage.distance_transform_edt(~((~S) & ~G))[G]
    return dict(px=int(G.sum()), tief=int((d >= 1.5).sum()), max_tiefe=round(float(d.max()), 1))


def im_soll(m):
    import luecke_r5 as L
    return L.im_soll(m['px'], m['tief'], m['max_tiefe'])


def zacke(k, plain_klein):
    """-> Liste der rechten Kerben-Stuecke ueber dem Soll mit heller Mesh-8-Wand: (px, tief, max_tiefe, L_wand)."""
    S, H, R, m8, G, st = _kerben(k)
    if not st:
        return []
    hx = np.nonzero(H)[1].mean()
    Lb = plain_klein @ np.array([0.299, 0.587, 0.114])
    out = []
    for c in st:
        ys, xs = np.nonzero(c); cx = xs.mean()
        if cx <= hx:
            continue
        w = ndimage.binary_dilation(c, np.ones((3, 3), bool), iterations=4) & m8
        w[:, :int(np.ceil(cx))] = False
        if not w.any():
            continue
        d = ndimage.distance_transform_edt(~((~S) & ~c))[c]
        m = dict(px=int(c.sum()), tief=int((d >= 1.5).sum()), max_tiefe=round(float(d.max()), 1))
        lw = float(Lb[w].mean())
        import luecke_r5 as L
        if m['px'] > L.SOLL_SUMME and m['max_tiefe'] > L.SOLL_MAXTIEFE and lw >= L_HELL:
            out.append((m['px'], m['tief'], m['max_tiefe'], round(lw, 1)))
    return out


# ------------------------------------------------------------------ S16 nach E9 / E13
def cos_je_ansicht(v7_blob):
    import luecke_r5 as L
    import riegel_r6 as R6
    mo7 = V.modell(v7_blob)
    return {nm: R6.vorn_cos(mo7, p) for nm, p in L.ansichten('alle')}


def luecke_offline(blob):
    """luecke_r5.messen_parallel (177 Ansichten, 7 Varianten) mit Zwischenspeicher je sha256 unter SFV8."""
    import json
    import luecke_r5 as L
    sha = V.sha(blob)
    pj = os.path.join(V.SFV8, 'riegel_r7c_luecke_%s.json' % sha[:16])
    if os.path.exists(pj):
        return json.load(open(pj))
    pe = os.path.join(V.SFV8, 'riegel_s16_%s.EMD' % sha[:12])
    os.makedirs(V.SFV8, exist_ok=True)
    open(pe, 'wb').write(blob)
    z = L.zusammen(L.messen_parallel(pe, 'alle', 7, 12))
    json.dump(z, open(pj, 'w'))
    return z


def e9_offline(ueber_kand, ueber_v7, cos):
    def teile(u):
        t = dict(vorn=[], foto=[], seite=[], nicht_spiel=[])
        for x in u:
            nm = x[0]
            if nm in FOTO:
                t['foto'].append(tuple(x))
            elif cos[nm] >= VORN_COS:
                t['vorn'].append(tuple(x))
            elif nm.startswith('F'):
                t['seite'].append(tuple(x))
            else:
                t['nicht_spiel'].append(tuple(x))
        return t
    k, v = teile(ueber_kand), teile(ueber_v7)
    import riegel_r6 as R6
    e3, grund = R6.e3(k['seite'])
    sk, sv = sum(x[1] for x in k['vorn']), sum(x[1] for x in v['vorn'])
    ok_vorn = len(k['vorn']) <= len(v['vorn']) and sk <= sv
    return dict(kand=k, v7=v, vorn=(len(k['vorn']), sk), vorn_v7=(len(v['vorn']), sv), ok_vorn=ok_vorn, foto_ok=not k['foto'],
                seite_e3=e3, seite_grund=grund, ok=ok_vorn and not k['foto'] and e3 != 'ROT', gelb=bool(k['nicht_spiel']))


ARTEFAKT_SPALTEN = 0.8                # E13 "spaltengleich": Anteil der Kerben-Spalten, deren Silhouetten-Kante ober- und unterhalb der Kerbe bei
                                      # V7 auf +- 1 px gleich liegt. Gemessen (sfv8/r7c NOTIZ): F340 12 von 14 Spalten (P5-Pruefer R3: Kopf-
                                      # Oberkante / Mesh-8-Unterkante identisch, Mess-Artefakt), F312 5 von 11 (echte Kerbe), F311 9 von 11


def _profil(k, G, x):
    S = k >= 0
    ys = np.nonzero(G[:, x])[0]
    oben = np.nonzero(S[:ys.min(), x])[0]
    unten = np.nonzero(S[ys.max() + 1:, x])[0]
    return (int(oben.max()) if oben.size else -1, int(unten.min() + ys.max() + 1) if unten.size else -1)


def artefakt(kk, k7):
    """E13: die Kerbe des Prueflings ist bei V7 von derselben Silhouette begrenzt (Spaltenprofil: letzte Silhouetten-Zeile ueber und erste unter
    der Kerbe je Spalte, +- 1 px, in >= ARTEFAKT_SPALTEN der Spalten) - sie zaehlt nur ueber die Klassen der Randpunkte (P5-Pruefer R3 zu F340:
    "Spaltenprofil Kopf-Oberkante / Mesh-8-Unterkante identisch"). Aufrufer prueft zusaetzlich: V7 hat das Bild im Soll."""
    G = _kerben(kk)[4]
    if not G.any():
        return False
    xs = np.unique(np.nonzero(G)[1])
    gleich = 0
    for x in xs:
        a, b = _profil(kk, G, x), _profil(k7, G, x)
        gleich += abs(a[0] - b[0]) <= 1 and abs(a[1] - b[1]) <= 1
    return gleich >= ARTEFAKT_SPALTEN * len(xs)


def e9_echt(Kk, K7, plain_dir, cos_szene):
    """Kk / K7: {f: Klassen 960x720} aus lauf_klassen (F235-F378)."""
    import riegel_r6 as R6
    def liste(K, mit_art=None):
        vorn, seite, art = [], [], []
        for f in range(F_LUECKE[0], F_LUECKE[1] + 1):
            if f not in K or VORDERGRUND[0] <= f <= VORDERGRUND[1]:
                continue
            m = mass(klein(K[f]))
            if im_soll(m):
                continue
            x = ('F%d' % f, m['px'], m['tief'], m['max_tiefe'])
            if cos_szene['F%d' % f] >= VORN_COS:
                if mit_art is not None and f in mit_art:
                    k7 = klein(mit_art[f])
                    if im_soll(mass(k7)) and artefakt(klein(K[f]), k7):
                        art.append(x); continue
                vorn.append(x)
            else:
                seite.append(x)
        return vorn, seite, art
    vk, sk, ak = liste(Kk, K7)
    v7, s7, _a7 = liste(K7)
    e3, grund = R6.e3(sk)
    z, folgen, vorher = [], [], False
    for f in range(F_LUECKE[0], F_LUECKE[1] + 1):
        if f not in Kk or VORDERGRUND[0] <= f <= VORDERGRUND[1]:
            vorher = False
            continue
        p = ppm(plain_dir, f)
        st = zacke(klein(Kk[f]), klein(p).astype(float)) if p is not None else []
        if st:
            z.append(('F%d' % f, st))
            if vorher:
                folgen.append('F%d' % f)
        vorher = bool(st)
    ok_vorn = len(vk) <= len(v7) and sum(x[1] for x in vk) <= sum(x[1] for x in v7)
    return dict(vorn=(len(vk), sum(x[1] for x in vk)), vorn_v7=(len(v7), sum(x[1] for x in v7)), vorn_liste=vk, artefakte=ak,
                seite=sk, seite_e3=e3, seite_grund=grund, zacke=z, zacke_folgen=folgen,
                ok=ok_vorn and e3 != 'ROT' and not folgen, ok_vorn=ok_vorn)


def gesicht_echt(Kk, K7, Ke):
    """V7-Gesicht (Klasse 4), im Pruefling nicht Gesicht: nach Klasse des Prueflings (1/2 = Mesh 8)."""
    out = dict(mesh8=0, rest=0, rest_ein=0, bilder_mesh8=[], bilder_rest=[])
    for f in sorted(Kk):
        if f not in K7:
            continue
        v = (K7[f] == 4) & (Kk[f] != 4)
        n8 = int((v & np.isin(Kk[f], (1, 2))).sum()); nr = int(v.sum()) - n8
        out['mesh8'] += n8; out['rest'] += nr
        if n8:
            out['bilder_mesh8'].append((f, n8))
        if nr:
            out['bilder_rest'].append((f, nr))
        if Ke is not None and f in Ke:
            ve = (K7[f] == 4) & (Ke[f] != 4)
            out['rest_ein'] += int((ve & ~np.isin(Ke[f], (1, 2))).sum())
    out['ok'] = out['mesh8'] == 0 and (Ke is None or out['rest'] <= out['rest_ein'])
    return out


def kippen(K):
    """A-B-A-Kippen Kopf (3/4/5) / Ruecken (2) / sonst im Kopffenster (Muster Bank P5 voll_p5.kst, Klassen des Prueflings-Markers)."""
    def kl(k):
        c = np.full(k.shape, 0, np.int8); c[k < 0] = -1; c[k == 2] = 8; c[k >= 3] = 9
        return c
    summe = 0
    fs = sorted(K)
    for a_, b_, c_ in zip(fs, fs[1:], fs[2:]):
        if not (b_ == a_ + 1 and c_ == b_ + 1):
            continue
        a, b, c = kl(K[a_]), kl(K[b_]), kl(K[c_])
        if not (b == 9).any():
            continue
        ys, xs = np.nonzero(b == 9)
        r = max(ys.max() - ys.min() + 1, xs.max() - xs.min() + 1) // 2
        w = np.zeros(b.shape, bool)
        w[max(0, ys.min() - r):ys.max() + r + 1, max(0, xs.min() - r):xs.max() + r + 1] = True
        summe += int((w & (a == c) & (b != a) & (np.isin(a, (8, 9)) | np.isin(b, (8, 9))) & (a >= 0) & (b >= 0)).sum())
    return summe


# ------------------------------------------------------------------ S21 Texel / orange
def fremde_texel(blob, refs):
    belegt = {}
    for r in refs:
        for m in KB.teile(r)[2]:
            for uv, n in [(u, 3) for u in m['tuv']] + [(u, 4) for u in m['quv']]:
                z = (uv[2] >> 6) & 0x1FF
                belegt.setdefault(z, np.zeros((256, 384), bool))
                belegt[z] |= KB.raster(uv, n, (uv[5] & 0xF) * 128)
    m8 = KB.teile(blob)[2][8]
    fremd, wo = 0, {}
    for i, (uv, n) in enumerate([(u, 3) for u in m8['tuv']] + [(u, 4) for u in m8['quv']]):
        z = (uv[2] >> 6) & 0x1FF
        mk = KB.raster(uv, n, (uv[5] & 0xF) * 128) & ~belegt.get(z, np.zeros((256, 384), bool))
        pts = [(z, int(y), int(x)) for y, x in zip(*np.nonzero(mk)) if (z, int(y), int(x)) not in ZULASSUNG]
        if pts:
            nm = ('d%d' % i) if i < len(m8['tuv']) else ('v%d' % (i - len(m8['tuv'])))
            wo[nm] = len(pts); fremd += len(pts)
    return dict(fremd=fremd, wo=wo, zulassung=len(ZULASSUNG), ok=fremd == 0)


def orange_maske(a):
    a = a[180:420, 420:760].astype(float) / 255.0
    mx, mn = a.max(-1), a.min(-1); d = mx - mn + 1e-9
    r, g, b = a[..., 0], a[..., 1], a[..., 2]
    h = np.where(mx == r, (g - b) / d % 6, np.where(mx == g, (b - r) / d + 2, (r - g) / d + 4)) * 60
    s = np.where(mx > 0, d / (mx + 1e-9), 0)
    return (h >= 16) & (h <= 40) & (s > 0.43) & (mx > 0.59)


def orange(plain_dir, plain_ein):
    bilder, summe, groesst = [], 0, 0
    for f in range(F_SZENE[0], F_SZENE[1] + 1):
        a, b = ppm(plain_dir, f), ppm(plain_ein, f)
        if a is None or b is None:
            continue
        m = orange_maske(a)
        neu = m & ~ndimage.binary_dilation(orange_maske(b), np.ones((3, 3), bool), iterations=2)
        n = int(neu.sum()); summe += n
        if neu.any():
            lab, k = ndimage.label(neu, np.ones((3, 3)))
            groesst = max(groesst, int(max(ndimage.sum(neu, lab, range(1, k + 1)))))
        if n >= ORANGE_MIN:
            bilder.append((f, n))
    return dict(bilder=bilder, summe=summe, groesst=groesst, ok=not bilder)


# ------------------------------------------------------------------ S18 nach E10 / E11 / E12
def ergaenzung_keys(m):
    """E11: Dreiecke (Index), die eine Haelfte (0,1,3) / (0,3,2) eines Vierecks mit gleichen Ecken, Normalen, UV, Zeile und Seite sind -> {d: v}."""
    out = {}
    for ti, (f, uv) in enumerate(zip(m['tf'], m['tuv'])):
        for qi, (q, qu) in enumerate(zip(m['qf'], m['quv'])):
            c = [(q[0], q[1]), (q[2], q[3]), (q[4], q[5]), (q[6], q[7])]
            t = [(qu[0], qu[1]), (qu[3], qu[4]), (qu[6], qu[7]), (qu[9], qu[10])]
            for h in ((0, 1, 3), (0, 3, 2)):
                if [x for j in h for x in c[j]] == list(f) and [x for j in h for x in t[j]] == [uv[0], uv[1], uv[3], uv[4], uv[6], uv[7]] \
                        and uv[2] == qu[2] and uv[5] == qu[5]:
                    out['d%d' % ti] = 'v%d' % qi
    return out


def s18_r7c(blob, ref_blob, r4, frei):
    """riegel_r6.s18 (a1/a3) mit E10 (frei = Ecken ohne a1-Pruefung) und E11; dazu Kragen (Bank P5 K-VORN a)."""
    import luecke_r5 as L
    import riegel_r6 as R6
    mo, mo7, mo4 = V.modell(blob), V.modell(ref_blob), V.modell(r4)
    m, m4 = KB.teile(blob)[2][8], KB.teile(r4)[2][8]
    erg = ergaenzung_keys(m)
    alt = set(['d%d' % i for i in range(min(len(m['tf']), len(m4['tf'])))] + ['v%d' % i for i in range(min(len(m['qf']), len(m4['qf'])))])
    alt_e11 = alt | set(k for k, v in erg.items() if v in alt)
    a1, a1_frei, a3, kragen, namen = [], [], [], [], []
    for nm, p in L.ansichten('alle'):
        if R6.vorn_cos(mo7, p) < R6.S18_COS:
            continue
        k7 = V.b2.umriss_schnell(mo7, p, knochen=[9])
        if not k7.any():
            continue
        namen.append(nm)
        ys, xs = np.nonzero(k7)
        r = max(ys.max() - ys.min() + 1, xs.max() - xs.min() + 1) // 2
        w = np.zeros(k7.shape, bool)
        w[max(0, ys.min() - r):ys.max() + r + 1, max(0, xs.min() - r):xs.max() + r + 1] = True
        A = L.zeichne(mo, p, L.VARIANTEN[0])
        b, fl = A['bone'][:480], A['flaeche'][:480]
        sel = (b == 8) & w[:b.shape[0], :b.shape[1]]
        n_kr = int((sel & (A['lok'][:480][..., 1] < KRAGEN_Y)).sum())
        if n_kr:
            kragen.append((nm, n_kr))
        keys = set(np.unique(fl[sel]).tolist())
        neu_px = int(sum(int((fl[sel] == k).sum()) for k in keys if k not in alt_e11))
        ecken = sorted(set(i for k in keys if k in alt for i in R6._ecken(m, k) if i < len(m4['v'])))
        if ecken:
            P1 = V.proj(p, V.b8_zu_modell(mo, np.array([m['v'][i][:3] for i in ecken], float), p))
            P0 = V.proj(p, V.b8_zu_modell(mo4, np.array([m4['v'][i][:3] for i in ecken], float), p))
            d = np.linalg.norm(np.asarray(P1, float)[:, :2] - np.asarray(P0, float)[:, :2], axis=1)
            rest = [(dd, e) for dd, e in zip(d, ecken) if e not in frei]
            fr = [(dd, e) for dd, e in zip(d, ecken) if e in frei]
            if rest and max(rest)[0] > R6.S18_PX:
                a1.append((nm, round(float(max(rest)[0]), 1), int(max(rest)[1])))
            if fr and max(fr)[0] > R6.S18_PX:
                a1_frei.append((nm, round(float(max(fr)[0]), 1), int(max(fr)[1])))
        if neu_px:
            a3.append((nm, neu_px))
    return dict(vorn=len(namen), a1=a1, a1_max=max([x[1] for x in a1], default=0.0), a1_frei=a1_frei,
                a1_frei_max=max([x[1] for x in a1_frei], default=0.0), a3=a3, a3_max=max([x[1] for x in a3], default=0),
                ergaenzung=erg, kragen=kragen, kragen_px=sum(x[1] for x in kragen))


def hell_e12(plain_dir, plain_r4):
    import riegel_r6 as R6
    x0, y0, x1, y1 = R6.S18_KASTEN
    rot, gelb = [], []
    for f in range(R6.S18_BILDER[0], R6.S18_BILDER[1] + 1):
        a, b = ppm(plain_dir, f), ppm(plain_r4, f)
        if a is None or b is None:
            continue
        def maske(im):
            im = im[y0:y1, x0:x1]
            R, B = im[..., 0], im[..., 2]
            return (B >= R - 6) & (B > 120) & (R > 110)
        neu = maske(a) & ~ndimage.binary_dilation(maske(b), np.ones((3, 3), bool))
        if not neu.any():
            continue
        lab, n = ndimage.label(neu, np.ones((3, 3)))
        gr = ndimage.sum(neu, lab, range(1, n + 1))
        if max(gr) >= HELL_KEIL:
            rot.append((f, int(max(gr))))
        else:
            gelb.append((f, int(neu.sum()), int(max(gr))))
    return dict(rot=rot, gelb=gelb, ok=not rot)


# ------------------------------------------------------------------ Einbau
def einbauen(out, blob, ref_blob, r4, a):
    """out = Ergebnis von riegel_v8.alles mit riegel_r6.einbauen; a = argparse-Namen (r7c_*)."""
    import luecke_r5 as L
    import riegel_v8 as RV
    ein = open(a.r7c_ein, 'rb').read() if a.r7c_ein else git_blob(EIN_COMMIT)
    r = dict(ein_sha256=V.sha(ein))
    cos = cos_je_ansicht(ref_blob)
    # S16 offline: V7 mit demselben Code
    z7 = luecke_offline(ref_blob)
    r['e9_off'] = e9_offline(out['S16']['ueber_soll'], z7['ueber_soll'], cos)
    import riegel_r6 as R6
    v = out['S16'].get('verdeckt', {})
    ve = R6.s16_verdeckt(ein, ref_blob)
    je_e = {x[0]: x[2].get(8, 0) for x in ve['ueber']}
    neu8 = [(x[0], x[2].get(8, 0), je_e.get(x[0], 0)) for x in v.get('ueber', []) if x[2].get(8, 0) > je_e.get(x[0], 0)]
    r['verdeckt_off'] = dict(mesh8=int(v.get('knochen', {}).get(8, 0)), mesh8_ein=int(ve['knochen'].get(8, 0)), mesh8_neu=neu8,
                             rest={k: c for k, c in v.get('knochen', {}).items() if k != 8},
                             rest_ein={k: c for k, c in ve['knochen'].items() if k != 8}, ansichten=v.get('ueber', []))
    # echte Bilder
    Kk = K7 = Ke = None
    if a.r7c_lm and a.r7c_plain and a.r7c_lm_v7 and a.r7c_plain_v7:
        Kk = lauf_klassen(a.r7c_lm, a.r7c_plain, *F_SZENE)
        K7 = lauf_klassen(a.r7c_lm_v7, a.r7c_plain_v7, *F_SZENE)
        Ke = lauf_klassen(a.r7c_lm_ein, a.r7c_plain_ein, *F_SZENE) if (a.r7c_lm_ein and a.r7c_plain_ein) else None
        sp = {('F%d' % f): cos['F%d' % f] for f in range(F_LUECKE[0], F_LUECKE[1] + 1)}
        r['e9_echt'] = e9_echt({f: k for f, k in Kk.items() if F_LUECKE[0] <= f <= F_LUECKE[1]},
                               {f: k for f, k in K7.items() if F_LUECKE[0] <= f <= F_LUECKE[1]}, a.r7c_plain, sp)
        r['gesicht_echt'] = gesicht_echt(Kk, K7, Ke)
        r['kippen'] = kippen(Kk)
        r['kippen_ein'] = kippen(Ke) if Ke is not None else None
    # S16-Urteil
    e9o, e9e = r['e9_off'], r.get('e9_echt')
    ok16 = e9o['ok'] and not r['verdeckt_off']['mesh8_neu']
    if e9e is not None:
        ok16 = ok16 and e9e['ok'] and r['gesicht_echt']['mesh8'] == 0
    gelb16 = e9o['gelb'] or bool(r['verdeckt_off']['rest']) or r['verdeckt_off']['mesh8'] > 0 or (e9e is not None and (e9e['artefakte'] or r['gesicht_echt']['rest']))
    r['S16_status'] = ('ROT' if not ok16 else ('GELB - E9/E12/E13' if gelb16 else 'GRUEN')) if e9e is not None else \
        ('ROT' if not ok16 else 'GELB - ohne echte Bilder')
    # S18
    s18 = s18_r7c(blob, ref_blob, r4, frei=E10_ECKEN)
    bedingungen = dict(kragen=s18['kragen_px'] == 0,
                       rahmen=len(KB.teile(blob)[2][9]['tf']) == len(KB.teile(ref_blob)[2][9]['tf']) and
                       len(KB.teile(blob)[2][9]['qf']) == len(KB.teile(ref_blob)[2][9]['qf']),
                       buckel=out['S3'].get('f_huegel_rot', 0) >= RV.S3F_MIN,
                       gesicht=not r['verdeckt_off']['mesh8_neu'] and (r.get('gesicht_echt') is None or r['gesicht_echt']['mesh8'] == 0),
                       kappe=max(out['S11']['kappe'].values()) <= RV.KAPPE_SOLL,
                       ruhig=(r.get('kippen') is not None and r.get('kippen_ein') is not None and r['kippen'] <= r['kippen_ein']))
    frei = E10_ECKEN if all(bedingungen.values()) else ()
    s18['e10_bedingungen'] = bedingungen
    s18['e10_frei'] = list(frei)
    a1_urteil = s18['a1'] if frei else (s18['a1'] + s18['a1_frei'])
    if a.r7c_plain and a.r7c_plain_r4:
        s18['b_e12'] = hell_e12(a.r7c_plain, a.r7c_plain_r4)
    ok18 = not a1_urteil and not s18['a3'] and s18['kragen_px'] == 0 and s18.get('b_e12', {}).get('ok', False)
    s18['status'] = 'ROT' if not ok18 else ('GELB - E12' if s18.get('b_e12', {}).get('gelb') else 'GRUEN')
    r['S18'] = s18
    # S3 / S9 Erbe (E12)
    mo_e, A_e = RV.render(ein)
    s2e, s3e, _om = RV.s2_s3(mo_e, A_e['bone'][:480])
    s9e = RV.s9(ein, ref_blob)
    s9e['b_schlitz'] = RV.s9b(ein, ref_blob)
    s3 = out['S3']
    nur_e = (s3['a_kuppe'] <= 4 and 5 <= s3['b_huegel'][0] <= 10 and not s3['c_weitere'] and s3['d_sattel_kopf'][0] <= 2 and
             s3.get('f_huegel_rot', 0) >= RV.S3F_MIN)
    r['S3_status'] = 'GRUEN' if s3['ok'] else ('GELB - Erbe' if (nur_e and s3['e_K'] <= s3e['e_K'] + 1e-9) else 'ROT')
    r['S3_ein_K'] = s3e['e_K']
    ein9 = {x['ansicht']: x for x in s9e['ansichten']}
    s9_rot = []
    for x in out['S9']['ansichten']:
        e = ein9.get(x['ansicht'], dict(ueber_v7_px=0, neue_ecken=[]))
        neu = [c for c in x['neue_ecken'] if not any(abs(c - y) <= 3 for y in e['neue_ecken'])]
        if (x['ueber_v7_px'] > 2 and x['ueber_v7_px'] > e['ueber_v7_px']) or neu:
            s9_rot.append((x['ansicht'], x['ueber_v7_px'], e['ueber_v7_px'], neu))
    sb, sbe = out['S9']['b_schlitz']['summe'], s9e['b_schlitz']['summe']
    r['S9_ein'] = dict(ansichten=[(x['ansicht'], x['ueber_v7_px'], x['neue_ecken']) for x in s9e['ansichten']], schlitz=sbe)
    r['S9_rot'] = s9_rot
    r['S9_status'] = 'GRUEN' if out['S9']['ok'] else ('GELB - Erbe' if (not s9_rot and sb <= sbe) else 'ROT')
    # S21
    s21 = dict(a=fremde_texel(blob, [ein, r4, ref_blob]))
    if a.r7c_plain and a.r7c_plain_ein:
        s21['b'] = orange(a.r7c_plain, a.r7c_plain_ein)
    s21['ok'] = s21['a']['ok'] and s21.get('b', {}).get('ok', False)
    r['S21'] = s21
    out['R7c'] = r
    # Urteile in die Summe
    out['S16']['ok'] = r['S16_status'] != 'ROT'
    out['S16']['status'] = r['S16_status']
    out['S18']['ok'] = s18['status'] != 'ROT'
    out['S18']['status'] = s18['status']
    out['S3']['ok'] = r['S3_status'] != 'ROT'
    out['S3']['status'] = r['S3_status']
    out['S9']['ok'] = r['S9_status'] != 'ROT'
    out['S9']['status'] = r['S9_status']
    out['S21'] = dict(ok=s21['ok'], status='GRUEN' if s21['ok'] else 'ROT')
    return out


def zeilen(out):
    r = out.get('R7c')
    if not r:
        return []
    z = ['Runde 7c (Entscheide E9-E13, Pflicht-Korrekturen K-a..K-e; riegel_r7c.py), Eingang Stand 7b %s:' % r['ein_sha256'][:12]]
    e = r['e9_off']
    z.append('S16 E9 offline (177 Ansichten, 7 Varianten): vorn (cos >= %.1f) ueber Soll %d / Summe %d (V7 %d / %d, Soll <= V7) -> %s; Foto-Posen '
             'ueber Soll %s -> %s; Seite (Spiel) %s -> E3 %s %s; Nicht-Spiel-Ansichten (E13 GELB) %s'
             % (VORN_COS, e['vorn'][0], e['vorn'][1], e['vorn_v7'][0], e['vorn_v7'][1], 'GRUEN' if e['ok_vorn'] else 'ROT',
                e['kand']['foto'], 'GRUEN' if e['foto_ok'] else 'ROT', e['kand']['seite'], e['seite_e3'], e['seite_grund'],
                e['kand']['nicht_spiel']))
    z.append('   S16 E9 offline Liste vorn Pruefling: %s' % e['kand']['vorn'])
    vo = r['verdeckt_off']
    z.append('S16 (b)/(c) offline (riegel_r6.s16_verdeckt, je Bildpunkt lok x >= 150): V7-Gesicht verdeckt durch Mesh 8 %d (Eingang %d), mehr als '
             'der Eingang in %s (Soll keine, K-c); durch andere Meshes %s (Eingang %s, Erbe E12); Ansichten %s'
             % (vo['mesh8'], vo['mesh8_ein'], vo['mesh8_neu'], vo['rest'], vo['rest_ein'], vo['ansichten'][:8]))
    if 'e9_echt' in r:
        e = r['e9_echt']; g = r['gesicht_echt']
        z.append('S16 E9 echt (F235-F378 ohne F316-F333, Marker LM): vorn ueber Soll %d / Summe %d (V7 %d / %d) -> %s; Seite %s -> E3 %s; '
                 'Mess-Artefakte E13 (V7 im Soll, Spaltenprofil wie V7) %s'
                 % (e['vorn'][0], e['vorn'][1], e['vorn_v7'][0], e['vorn_v7'][1], 'GRUEN' if e['ok_vorn'] else 'ROT', e['seite'], e['seite_e3'],
                    e['artefakte']))
        z.append('   S16 E9 echt Liste vorn Pruefling: %s' % e['vorn_liste'])
        z.append('S16 Zacke + V-Kerbe rechts neben dem Kopf (Wand L >= %.0f, ueber Soll): Bilder %s; zwei oder mehr in Folge %s -> %s'
                 % (L_HELL, e['zacke'], e['zacke_folgen'], 'ROT' if e['zacke_folgen'] else 'GRUEN'))
        z.append('S16 Gesicht echt F225-F450 (V7-Gesicht nicht Gesicht): durch Mesh 8 %d %s (Soll 0, K-c); andere Meshes %d %s (Eingang %s, Erbe E12)'
                 % (g['mesh8'], g['bilder_mesh8'][:8], g['rest'], g['bilder_rest'][:8], g['rest_ein']))
    z.append('S16 Runde 7c -> %s' % r['S16_status'])
    s = r['S18']
    z.append('S18 Runde 7c: E10 Bedingungen %s -> frei %s; (a1) uebrige Ecken > 2 px gegen r4: %d Ansichten, groesste %.1f px %s; (a1 frei, Info) %d '
             'Ansichten, groesste %.1f px %s; (a3, E11 Ergaenzung %s) %d Ansichten, groesste %d px %s; Kragen %d px %s; (b, E12) %s -> %s'
             % (s['e10_bedingungen'], s['e10_frei'], len(s['a1']), s['a1_max'], s['a1'][:10], len(s['a1_frei']), s['a1_frei_max'], s['a1_frei'][:6],
                s['ergaenzung'], len(s['a3']), s['a3_max'], s['a3'][:10], s['kragen_px'], s['kragen'][:6],
                ('Keil >= %d px neu gegen r4: %s; einzelne helle Punkte (GELB) %d Bilder %s' % (HELL_KEIL, s['b_e12']['rot'], len(s['b_e12']['gelb']),
                                                                                         s['b_e12']['gelb'][:8])) if 'b_e12' in s else 'kein Lauf',
                s['status']))
    z.append('S3 Runde 7c (E12): (e) K %.1f, Eingang %.1f, uebrige Teilpunkte %s -> %s'
             % (out['S3']['e_K'], r['S3_ein_K'], 'gruen' if r['S3_status'] != 'ROT' else 'siehe S3', r['S3_status']))
    z.append('S9 Runde 7c (E12): Eingang %s, Schlitz Eingang %d / neu %d; schlechter als der Eingang %s -> %s'
             % (r['S9_ein']['ansichten'], r['S9_ein']['schlitz'], out['S9']['b_schlitz']['summe'], r['S9_rot'], r['S9_status']))
    s = r['S21']
    z.append('S21 Texel Mesh 8 (K-a): (a) fremde Texel (nicht in Eingang / r4 / V7 / Zulassung R2 %d) %d %s; (b) oranger Fleck: Bilder mit >= %d neuen orange '
             'Bildpunkten gegen den Eingangs-Lauf %s, Summe %s, groesstes Stueck %s -> %s'
             % (s['a']['zulassung'], s['a']['fremd'], s['a']['wo'], ORANGE_MIN, s.get('b', {}).get('bilder', 'kein Lauf'), s.get('b', {}).get('summe'),
                s.get('b', {}).get('groesst'), 'GRUEN' if s['ok'] else 'ROT'))
    return z


def teil(blob, ref_blob, r4, a):
    """Nur die Runde-7c-Punkte (fuer Gegenproben): die noetigen Teile von riegel_v8.alles (S3, S9, S11, S16 a und verdeckt) und einbauen."""
    import riegel_v8 as RV
    import riegel_r6 as R6
    mo, A = RV.render(blob)
    out = dict(sha256=V.sha(blob))
    _s2, out['S3'], _om = RV.s2_s3(mo, A['bone'][:480])
    out['S3']['f_huegel_rot'] = RV.huegel_rot(A)
    out['S3']['ok'] = out['S3']['ok'] and out['S3']['f_huegel_rot'] >= RV.S3F_MIN
    out['S9'] = RV.s9(blob, ref_blob)
    out['S9']['b_schlitz'] = RV.s9b(blob, ref_blob)
    out['S9']['ok'] = out['S9']['ok'] and out['S9']['b_schlitz']['ok']
    out['S11'] = RV.s11(blob)
    z = luecke_offline(blob)
    out['S16'] = dict(ueber_soll=z['ueber_soll'], ok=z['ok'], verdeckt=R6.s16_verdeckt(blob, ref_blob))
    out['S18'] = dict(ok=False)
    return einbauen(out, blob, ref_blob, r4, a)


if __name__ == '__main__':
    if sys.argv[1] == 'teil':          # python riegel_r7c.py teil <emd> <aus.txt> [--r4 R4.EMD] [--r7c-* wie riegel_v8.py]
        import argparse, json
        import riegel_r6 as R6
        ap = argparse.ArgumentParser()
        ap.add_argument('was'); ap.add_argument('emd'); ap.add_argument('aus'); ap.add_argument('--r4'); ap.add_argument('--v7')
        for k in ('lm', 'plain', 'lm-v7', 'plain-v7', 'lm-ein', 'plain-ein', 'plain-r4', 'ein'):
            ap.add_argument('--r7c-' + k)
        a = ap.parse_args()
        ref = open(a.v7 or os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'), 'rb').read()
        assert V.sha(ref) == V.V7_SHA
        blob = open(a.emd, 'rb').read()
        out = teil(blob, ref, R6.r4_blob(a.r4), a)
        z = ['Riegel Runde 7c (Teil, Gegenprobe) fuer %s' % a.emd, 'sha256 %s' % V.sha(blob)] + zeilen(out)
        st = {k: out[k].get('status', 'GRUEN' if out[k]['ok'] else 'ROT') for k in ('S3', 'S9', 'S16', 'S18', 'S21')}
        z.append('URTEILE Runde 7c: %s' % st)
        open(a.aus, 'w', encoding='utf-8').write('\n'.join(z) + '\n')
        json.dump(out.get('R7c'), open(a.aus.replace('.txt', '.json'), 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
        print('\n'.join(z))
    elif sys.argv[1] == 'lm':
        v7 = os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD')
        neu = lm_bauen(open(sys.argv[2], 'rb').read(), open(sys.argv[4] if len(sys.argv) > 4 else v7, 'rb').read())
        open(sys.argv[3], 'wb').write(neu)
        print('LM', sys.argv[3], V.sha(neu))
