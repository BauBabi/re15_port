"""Riegel V4 Originaltexturen (Vorgabe des Orchestrators 2026-11-07, ersetzt Messblatt V4 Abschnitt 5 und die Video-Farbriegel F*/FO/FS/O7/
O9/O10) - Exit 1, wenn eine Zeile rot ist.

    python riegel_ref2tex.py <EMD> [--vorher EMD_vorher] [--lauf debug.log] [--json aus.json] [--md aus.md] [--kein-exit]

Bezug: Referenz 2 = re15_port/shared_assets/PSX/EMD/EM30_HANGAR.EMD (sha256 cfd1ac42..., Pin release/em30_hangar.sha256, Commit 84e8a4d97).
  T-K1 Koerper = Referenz 2: Meshes 0-8 und 10-15 mit allen Ecken, Normalen, Flaechen und UV-Feldern (inkl. CLUT-Feld und TPage) bytegleich;
       die CLUT-Zeilen 480-485 bytegleich, weitere Zeilen nur als Kopie einer Referenz-2-Zeile; jeder Texel, den eine Koerperflaeche
       abdeckt, bytegleich.
  T-K2 keine neue Farbe: jeder Texel, den eine Flaeche von Kopf (Mesh 9) oder Schwanz (Meshes 16-22) abdeckt, zeigt ueber die CLUT-Zeile
       seiner Flaeche einen 15-Bit-Wert, den es in einer CLUT-Zeile von Referenz 2 schon gibt; die Zeilen dieser Flaechen sind
       Referenz-2-Zeilen oder deren Kopie.
  T-K3 Schwanz ohne Quernaht und ohne einfarbige Bloecke (b2-Render Pose "85" = Vorlage-2-Kamera, Keyframe, roh): Lab-Mittel je 3 px
       Bogenlaenge entlang der projizierten Gelenkkette; der groesste Sprung zwischen Nachbarabschnitten verschiedener Glieder ist
       hoechstens so gross wie der groesste Sprung innerhalb eines Glieds. Je Glied Std(L*) >= 0,5 x Std(L*) des Oberarms Id 13 im selben
       Render und >= 16 verschiedene Texel-Indizes. Wurzel: Sprung Id 16 <-> Rumpf Id 7/8 an ihrer Grenze <= groesster Sprung innerhalb
       eines Glieds (Vorgabe 2 "Wurzel geht ohne Kante in den Ruecken ueber").
  T-K3d (Runde 2, Sicht-Pruefung Runde 1 b2b) Wurzel Id 16 ohne Streifen: waagerechter / senkrechter mittlerer |dL*|-Sprung je
       Teilflaeche (ganz, obere 14 Zeilen, 4 Quadranten) in der Spanne derselben Teilflaechen an Rumpf Id 8 und Oberarm Id 13; Glieder
       17-22 als INFO (texelgleich Runde 1).
  T-K4 Kopf-Merkmale an den Messblatt-Orten (Runde 2): K1, K3, K4, K6, K10, S2 wie riegel_v4.py (Messblatt V4 Abschnitt 7); die
       Video-Farbziele von K2, K5, K7, K8, K9, S1, S3, S4 sind ueberholt (Orchestrator Runde 2 (a): Abnahme gegen Ref2, nicht gegen das
       Video) und durch relative bzw. Ref2-Bedingungen ersetzt (Haarfeld <= Ref2-Kopf P50 - 8, Nase >= Haar + 8, Kinn-Textur >= Ref2-Haut
       - 8, Kuppe a* >= 11 und |dL*| zu Ref2 <= 8, Auge blass). tk4_r1 = Fassung der Runde 1 (Gegenprobe).
  T-K5 (Runde 2) Kopf gegen Referenz 2 im rohen Render (Lab je Id-Maske, Verfahren der Sicht-Pruefung Runde 1), Pose 135 und 85:
       a) Grundton L* P50 ausserhalb des Haarfelds (vorn KV2 + K6-Box, seitlich KS2) wie Ref2 +- 4; b) Grundton/Rumpf >= Ref2 - 0,2;
       c) Anteil b* < 0 <= 0,02; d) dunkle Textur (Albedo = Render / weisser Pruefkoerper, L* < 25, nur Bildpunkte mit Licht L* >= 40)
       ausserhalb des Haarfelds <= Ref2 + 0,05. Der ganze Kopf (mit Haarfeld) steht als INFO daneben.
  Runde 3 (Kopf nach Nutzerbeschreibung, 2026-11-07): T-K2 erweitert (Ref2-Farbe oder belegte Ableitung); T-K4 Kopfzeilen K5/K6/K7/K9/S3/S4 nach
       der Lesung grau links bis Nase / Haar braeunlich rechts / Kuppe dunkel rotbraun, relativ zum Kopf-Median; T-K5 Grundton = graue Zone;
       NEU T-K9 Zonen (tk9) und T-K8 Aenderungsumfang gegen den Vorstand (--vorher). Zonen am EMD = Klasse des Texel-Index (zonen_emd).
  Form (muessen gruen bleiben): G1/M1-M4 (riegel_v4.machart), T1-T6, TA, T5q (riegel_v4.schwanz), TK (riegel_v4.schwanz_form), mit --lauf
       T7a-d und Boden (t7_koerperfest_v4.py). Die Video-Farbriegel T8/T9, TQ, TN, TV, TR, F*, FS, FO, FR, AF, KS, K11 entfallen (Vorgabe).
Kein RE-Befund (die Sichel-Form gibt es auf der Disc nicht); keine Zahl hier ist eine @0x-Konstante.
"""
import os, sys, json, argparse, hashlib
import numpy as np, cv2
from scipy import ndimage as nd
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v4_gemein as G
import b2
import birkin_formen as BF
import emd_ansichtsblatt as EA
import birkin_buehne as BB
import riegel_v4 as RV
import ref2tex_v4 as RT

zeile = RV.zeile
SCHW = G.SCHWANZ
KOERPER = RT.KOERPER
REF2_SHA = 'cfd1ac42'


def _flaechen_maske(ms, meshes, shape):
    """Je (Zeile) die Texelmaske der Flaechen der Meshes."""
    out = {}
    for i in meshes:
        for uv in list(ms[i].tuv) + list(ms[i].quv):
            z = uv[2] >> 6
            m = out.setdefault(z, np.zeros(shape, np.uint8))
            cv2.fillPoly(m, [RT.flaeche(uv)], 1)
    return {z: m > 0 for z, m in out.items()}


def tk1(A, R):
    """T-K1: Koerper bytegleich (Geometrie + UV-Felder, CLUT-Zeilen, abgedeckte Texel)."""
    Z = []
    def feld(m):
        return (tuple(map(tuple, m.tv)), tuple(map(tuple, m.tn)), tuple(map(tuple, m.tf)), tuple(map(tuple, m.qv)), tuple(map(tuple, m.qn)),
                tuple(map(tuple, m.qf)), tuple(map(tuple, m.tuv)), tuple(map(tuple, m.quv)))
    abw = [b for b in KOERPER if feld(A['ms'][b]) != feld(R['ms'][b])]
    Z.append(zeile('T-K1a Koerper-Meshes = Ref2', 'Meshes 0-8, 10-15: Ecken, Normalen, Flaechen, UV inkl. CLUT-Feld und TPage bytegleich',
                   'abweichend: %s' % (abw or 'keine'), not abw))
    cr, ca = R['clut'], A['clut']
    gleich = ca.shape[1] == cr.shape[1] and ca.shape[0] >= cr.shape[0] and bool((ca[:cr.shape[0]] == cr).all())
    extra = []
    for z in range(cr.shape[0], ca.shape[0]):
        kopie = [480 + j for j in range(cr.shape[0]) if (ca[z] == cr[j]).all()]
        extra.append((480 + z, kopie[0] if kopie else None))
    ok_extra = all(k is not None for _z, k in extra)
    Z.append(zeile('T-K1b CLUT = Ref2', 'Zeilen 480-%d bytegleich; weitere Zeilen nur als Kopie' % (479 + cr.shape[0]),
                   'Zeilen 480-%d %s; TIM %d Zeilen; weitere %s' % (479 + cr.shape[0], 'gleich' if gleich else 'ABWEICHEND', ca.shape[0],
                                                                  ['%d=Kopie %s' % e for e in extra] or 'keine'), gleich and ok_extra))
    mk = _flaechen_maske(R['ms'], KOERPER, R['tex'].shape)
    alle = np.zeros(R['tex'].shape, bool)
    for m in mk.values():
        alle |= m
    diff = alle & (A['tex'] != R['tex'])
    aussen = (~alle) & (A['tex'] != R['tex'])
    Z.append(zeile('T-K1c Koerper-Texel = Ref2', 'jeder von einer Koerperflaeche (Meshes 0-8, 10-15) abgedeckte Texel bytegleich',
                   '%d von %d abgedeckten Texeln abweichend; ausserhalb der Koerperflaechen geaendert %d Texel (Kopf-/Schwanzflaechen)' % (
                       int(diff.sum()), int(alle.sum()), int(aussen.sum())), not diff.any()))
    return Z


def tk2(A, R):
    """T-K2: Farben der Kopf-/Schwanz-Texel liegen in der Referenz-2-Palette."""
    ref = set(int(x) for x in np.unique(R['clut'] & 0x7FFF))
    zeilen_ok, neu, ges, farben = True, 0, 0, set()
    zinfo = {}
    for name, meshes in (('Kopf', [9]), ('Schwanz', list(SCHW))):
        mk = _flaechen_maske(A['ms'], meshes, A['tex'].shape)
        n_neu = n = 0
        for z, m in mk.items():
            zi = z - 480
            kopie = None
            if 0 <= zi < A['clut'].shape[0]:
                kopie = [480 + j for j in range(R['clut'].shape[0]) if (A['clut'][zi] == R['clut'][j]).all()]
            if not kopie:
                zeilen_ok = False
            werte = (A['clut'][zi][A['tex'][m]] & 0x7FFF) if 0 <= zi < A['clut'].shape[0] else np.zeros(int(m.sum()), int)
            nicht = np.array([int(w) not in ref for w in werte])
            n_neu += int(nicht.sum()); n += int(m.sum())
            farben |= set(int(w) for w in werte)
            zinfo.setdefault(name, []).append('Zeile %d (%s) %d Texel' % (z, ('Kopie/gleich %s' % kopie) if kopie else 'NEU', int(m.sum())))
        neu += n_neu; ges += n
        zinfo[name].append('nicht in Ref2: %d' % n_neu)
    # Runde 3: erlaubt sind auch belegte Ableitungen (entsaettigte / abgedunkelte Ref2-Hautfarben, Liste 'ableitungen' im Protokoll
    # ref2tex_protokoll.json mit Herkunft und Zahl); Runde 3 braucht keine (Protokoll: ableitungen = []).
    abl = _ableitungen()
    neu_abl = 0
    if abl:
        neu_abl = neu; neu = 0
        for name, meshes in (('Kopf', [9]), ('Schwanz', list(SCHW))):
            mk = _flaechen_maske(A['ms'], meshes, A['tex'].shape)
            for z, m in mk.items():
                zi = z - 480
                werte = (A['clut'][zi][A['tex'][m]] & 0x7FFF) if 0 <= zi < A['clut'].shape[0] else np.zeros(int(m.sum()), int)
                neu += int(np.array([int(w) not in ref and int(w) not in abl for w in werte]).sum())
    return [zeile('T-K2 Ref2-Farbe oder belegte Ableitung (Kopf, Schwanz)', 'jeder abgedeckte Texel von Mesh 9 und 16-22: 15-Bit-Wert in der Ref2-CLUT oder '
                  'eine im Protokoll belegte Ableitung (%d); Zeilen = Ref2-Zeile oder Kopie' % len(abl), '%d von %d Texeln mit Farbe ausserhalb Ref2 und Ableitungen'
                  ' (%d Ableitungs-Texel); %d verschiedene Farben; %s' % (neu, ges, neu_abl, len(farben), '; '.join('%s: %s' % (k, ', '.join(v)) for k, v in zinfo.items())),
                  neu == 0 and zeilen_ok)]


def _ableitungen():
    try:
        p = json.load(open(RT.PROT))
        return set(int(x['wert15']) for x in p.get('ableitungen', []))
    except Exception:
        return set()


def _bogen(mo, pose, ids):
    """Bogenlaenge (px) je Schwanzpixel entlang der projizierten Gelenkkette (Keyframe der Pose)."""
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    Q = b2.projiziere(pose, b2.kette_punkte(mo, kf))
    seg = np.diff(Q, axis=0); L = np.linalg.norm(seg, axis=1); s0 = np.r_[0, np.cumsum(L)]
    ys, xs = np.nonzero(np.isin(ids, SCHW))
    p = np.stack([xs + 0.5, ys + 0.5], 1)
    best = np.full(len(p), np.inf); s = np.zeros(len(p))
    for k in range(len(seg)):
        t = np.clip(((p - Q[k]) @ seg[k]) / max(L[k] ** 2, 1e-9), 0, 1)
        d = np.linalg.norm(p - (Q[k] + t[:, None] * seg[k]), axis=1)
        b = d < best
        best[b] = d[b]; s[b] = s0[k] + t[b] * L[k]
    return ys, xs, s


def tk3(mo, P, A):
    """T-K3: Quernaht/Bloecke/Wurzel am Render Pose 85 (roh)."""
    pose = P['85']
    img, ids = G.zeichne(mo, pose)
    Lab = G.lab(img.astype(float))
    ys, xs, s = _bogen(mo, pose, ids)
    bins = (s // 3).astype(int)
    stat = []
    for b_ in np.unique(bins):
        m = bins == b_
        if m.sum() < 6:
            continue
        idb = ids[ys[m], xs[m]]
        werte, n = np.unique(idb, return_counts=True)
        stat.append((b_, Lab[ys[m], xs[m]].mean(0), int(werte[np.argmax(n)])))
    innen, naht = [], []
    for (b0, l0, i0), (b1, l1, i1) in zip(stat, stat[1:]):
        if b1 != b0 + 1:
            continue
        d = float(np.linalg.norm(l1 - l0))
        (naht if i0 != i1 else innen).append((d, i0, i1, b0))
    mi = max(d for d, *_ in innen) if innen else 0.0
    mn = max(naht, key=lambda t: t[0]) if naht else (0.0, 0, 0, 0)
    Z = [zeile('T-K3a Schwanz ohne Quernaht', 'groesster Sprung (dE76, 3-px-Abschnitte entlang der Kette) zwischen Gliedern <= groesster '
               'Sprung innerhalb eines Glieds', 'zwischen Gliedern max %.1f (Id %d|%d bei %d px), innerhalb max %.1f; je Gelenk %s' % (
                   mn[0], mn[1], mn[2], mn[3] * 3, mi, ', '.join('%d|%d %.1f' % (a, b, d) for d, a, b, _ in naht)), mn[0] <= mi + 1e-9)]
    sd13 = float(Lab[ids == 13][:, 0].std()) if (ids == 13).any() else 0.0
    # verschiedene Texel-Indizes je Glied (abgedeckte Texel)
    zeilen = []
    ok_b = True
    for b_ in SCHW:
        m = ids == b_
        sd = float(Lab[m][:, 0].std()) if m.sum() > 3 else float('nan')
        tm = _flaechen_maske(A['ms'], [b_], A['tex'].shape)
        idx = set()
        for z, mm in tm.items():
            idx |= set(int(x) for x in np.unique(A['tex'][mm]))
        ok = (m.sum() <= 3 or sd >= 0.5 * sd13) and len(idx) >= 16
        ok_b &= ok
        zeilen.append('Id %d Std %.1f / %d Indizes' % (b_, sd, len(idx)))
    Z.append(zeile('T-K3b keine einfarbigen Bloecke', 'je Glied Std(L*) >= 0,5 x Oberarm Id 13 (%.1f -> %.1f) und >= 16 Texel-Indizes' % (sd13, 0.5 * sd13),
                   '; '.join(zeilen), ok_b))
    # Wurzel: Id 16 gegen Rumpf 7/8 an der Grenze (je 3 px Band)
    w16 = ids == 16; r78 = np.isin(ids, (7, 8))
    a = w16 & nd.binary_dilation(r78, iterations=3); b = r78 & nd.binary_dilation(w16, iterations=3)
    if a.sum() >= 4 and b.sum() >= 4:
        dw = float(np.linalg.norm(Lab[a].mean(0) - Lab[b].mean(0)))
        Z.append(zeile('T-K3c Wurzel ohne Kante', 'Sprung Id 16 <-> Rumpf Id 7/8 (je 3-px-Band an der Grenze) <= groesster Sprung innerhalb '
                       'eines Glieds', 'Sprung %.1f (Wurzel %s / Rumpf %s, %d / %d px), Grenze %.1f' % (
                           dw, '/'.join('%.1f' % v for v in Lab[a].mean(0)), '/'.join('%.1f' % v for v in Lab[b].mean(0)), int(a.sum()),
                           int(b.sum()), mi), dw <= mi + 1e-9))
    else:
        Z.append(zeile('T-K3c Wurzel ohne Kante', 'Sprung Id 16 <-> Rumpf <= groesster Sprung innerhalb eines Glieds',
                       'Grenze nicht sichtbar (%d / %d px)' % (int(a.sum()), int(b.sum())), False))
    # T-K3d Anisotropie je Teilflaeche (Sicht-Pruefung Runde 1 b2b: Oberseite der Wurzel senkrecht gestreift, waagerecht 7,45 / senkrecht
    # 4,36 = 1,7; Orchestrator Runde 2 (b) "wie am Rumpf, um 0,8-1,0"): Verhaeltnis mittlerer |dL*| waagerecht / senkrecht (Nachbarpaare
    # innerhalb der Teilflaeche, roher Render Pose 85) fuer Id 16 ganz, die oberen 14 Zeilen und die 4 Quadranten (Teilung am Median);
    # Spanne = kleinster bis groesster Wert derselben Teilflaechen an Rumpf Id 8 und Oberarm Id 13 im selben Render.
    tf = {b_: teilflaechen_aniso(Lab[..., 0], ids == b_) for b_ in (8, 13, 16)}
    ref_w = [v for b_ in (8, 13) for v in tf[b_].values() if np.isfinite(v)]
    lo, hi = min(ref_w), max(ref_w)
    w16 = tf[16]
    ok_d = all(np.isfinite(v) and lo <= v <= hi for v in w16.values())
    Z.append(zeile('T-K3d Wurzel Id 16 ohne Streifen (Anisotropie je Teilflaeche)', 'waagerecht/senkrecht je Teilflaeche in der Spanne von Rumpf '
                   'Id 8 und Oberarm Id 13 (%.2f-%.2f)' % (lo, hi), '; '.join('%s %.2f' % (k, v) for k, v in w16.items()), ok_d))
    for b_ in SCHW[1:]:
        t_ = teilflaechen_aniso(Lab[..., 0], ids == b_)
        Z.append(zeile('T-K3d Glied Id %d (Anisotropie)' % b_, 'ab Texelzeile 56 texelgleich Runde 1 (Orchestrator Runde 2 (c)); nur Ausgabe',
                       '; '.join('%s %.2f' % (k, v) for k, v in t_.items()), True, info=True))
    return Z, dict(stat=[(int(b), [round(float(x), 1) for x in l], int(i)) for b, l, i in stat], bild=(img, ids))


def _aniso(L, m):
    h = m[:, 1:] & m[:, :-1]; v = m[1:, :] & m[:-1, :]
    dh = np.abs(np.diff(L, axis=1))[h]; dv = np.abs(np.diff(L, axis=0))[v]
    if len(dh) < 20 or len(dv) < 20:
        return float('nan')
    return float(dh.mean() / max(dv.mean(), 1e-6))


def teilflaechen_aniso(L, m):
    """Anisotropie (waagerecht / senkrecht) einer Id-Maske: ganz, obere 14 Zeilen, 4 Quadranten (Median-Teilung)."""
    if m.sum() < 40:
        return {}
    yy, xx = np.nonzero(m)
    top = m.copy(); top[yy.min() + 14:] = False
    Y, X = np.mgrid[0:m.shape[0], 0:m.shape[1]]
    ym, xm = np.median(yy), np.median(xx)
    out = dict(ganz=_aniso(L, m), oben14=_aniso(L, top))
    for qy, ny in ((0, 'o'), (1, 'u')):
        for qx, nx in ((0, 'l'), (1, 'r')):
            out['Q' + ny + nx] = _aniso(L, m & ((Y >= ym) == bool(qy)) & ((X >= xm) == bool(qx)))
    return out


def tk4_r1(mo, P):
    """T-K4 Runde 1: K1-K10, S1-S4 (riegel_v4) mit den Video-Zielen, K8 an den Kopf-Bildpunkten. Ueberholt (Sicht-Pruefung Runde 1 b1,
    Orchestrator Runde 2 (a): Abnahme gegen Ref2, nicht gegen das Video); bleibt fuer die Gegenprobe."""
    zv, iv, (img, ids, V, _b) = RV.kopf_vorn(mo, P)
    zs, is_, _ = RV.kopf_seite(mo, P)
    out = []
    for z in zv:
        n = z['mass']
        if n.startswith('K11'):
            continue
        if n.startswith('K8'):
            kb = G.kopfbox(ids)
            bx = G.fenster(kb, RV.FENSTER['KV4'])
            m = np.zeros(ids.shape, bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
            m &= RV.er(ids == 9, 2)
            la = RV.lab1(V[m].mean(0)); d8 = G.de00(la, RV.ZIEL['kinn'])
            hell = float((G.lab(V)[m][:, 0] > 35).mean())
            out.append(zeile('T-K4 K8 Kinn/Maulzone (Kopf-Id 9 erodiert im Fenster KV4)', '9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05',
                             '%s dE00 %.1f; Anteil %.3f (%d Kopf-Bildpunkte)' % ('/'.join('%.1f' % v for v in la), d8, hell, int(m.sum())),
                             d8 <= 6 and hell <= 0.05))
            continue
        out.append(dict(z, mass='T-K4 ' + n))
    out += [dict(z, mass='T-K4 ' + z['mass']) for z in zs]
    return out, dict(vorn=iv, seite=is_)


# Runde 2: Kopf-Abnahme gegen Referenz 2 (Orchestrator Runde 2 (a)); die Video-Farbziele der Kopfzeilen (K2, K5, K7, K8, K9, S1, S3, S4:
# Lab-Werte aus Vorlage 1/2) sind ueberholt (Messblatt V4 Abschnitt 7, Nachtrag Originaltexturen Runde 2). Orte, Groessen, Komponenten und
# die relativen Bedingungen bleiben. Ref2-Vergleichswerte im selben Messverfahren (Videoraum-Fenster bzw. roher Render) am Spiel-Asset.
_ref2 = {}


def _kopf_fenster(mo, P, hf_zonen=None):
    """Fenster-/Rohmasse des Kopfes beider Posen. hf_zonen: Haarfeld-Masken je Pose aus den Zonen des EMD (Runde 3: Haar + Kuppe);
    ohne (Referenz 2): Haarfeld = Messblatt-Fenster KV2 + K6-Box (vorn) bzw. KS2 (seitlich)."""
    zv, iv, (img, ids, V, b) = RV.kopf_vorn(mo, P)
    zs, is_, (img_s, ids_s, V_s, b_s) = RV.kopf_seite(mo, P)
    kb, kbs = G.kopfbox(ids), G.kopfbox(ids_s)
    fv = {}
    for k in ('KV1', 'KV2', 'KV3', 'KV5'):
        fv[k] = RV.fenster_lab(V, kb, RV.FENSTER[k])[0]
    fv['KV3r'] = RV.fenster_lab(V, kb, (0.62, 0.66, 0.85, 0.81))[0]        # Haar rechts neben der Nase, gleiche Hoehe (K7, Runde 3)
    bx = G.fenster(kb, RV.FENSTER['KV4'])
    m = np.zeros(ids.shape, bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
    m &= RV.er(ids == 9, 2)
    fv['KV4'] = RV.lab1(V[m].mean(0))
    fs = {k: RV.fenster_lab(V_s, kbs, RV.FENSTER[k])[0] for k in ('KS1', 'KS2')}
    hf = {}
    if hf_zonen is None:
        m_ = np.zeros(ids.shape, bool); b2_ = G.fenster(kb, RV.FENSTER['KV2']); m_[b2_[1]:b2_[3] + 1, b2_[0]:b2_[2] + 1] = True
        xm = 0.5 * (kb[0] + kb[2]); bx0, by0, bx1, by1 = b['bbox']
        m_[by0:by1 + 1, int(round(2 * xm - bx1)):int(round(2 * xm - bx0)) + 1] = True
        hf['135'] = m_
        m_ = np.zeros(ids_s.shape, bool); b3_ = G.fenster(kbs, RV.FENSTER['KS2']); m_[b3_[1]:b3_[3] + 1, b3_[0]:b3_[2] + 1] = True
        hf['85'] = m_
    else:
        hf = hf_zonen
    roh = {}
    for pose, (im_, id_) in (('135', (img, ids)), ('85', (img_s, ids_s))):
        Lab = G.lab(im_.astype(float)); km = id_ == 9; k_, r_ = Lab[km], Lab[id_ == 8]
        ka = Lab[km & ~hf[pose]]
        w_, _ = G.zeichne(mo, P[pose], weiss=True)
        alb = G.lab(np.clip(im_.astype(float) / np.maximum(w_.astype(float), 1.0) * 248.0, 0, 255))[..., 0]
        aa = alb[km & ~hf[pose] & (G.lab(w_.astype(float))[..., 0] >= 40)]
        roh[pose] = dict(p50=float(np.median(k_[:, 0])), verh=float(k_[:, 0].mean() / r_[:, 0].mean()),
                         d20=float((k_[:, 0] < 20).mean()), viol=float((k_[:, 2] < 0).mean()), Lab=Lab, ids=id_,
                         p50_a=float(np.median(ka[:, 0])), verh_a=float(ka[:, 0].mean() / r_[:, 0].mean()), d20_a=float((ka[:, 0] < 20).mean()),
                         d20_h=float((Lab[km & hf[pose]][:, 0] < 20).mean()) if (km & hf[pose]).any() else 0.0, anteil_h=float(hf[pose][km].mean()),
                         a25_a=float((aa < 25).mean()), a_p50_a=float(np.median(aa)), alb=alb,
                         p50_v=float(np.median(G.lab(V if pose == '135' else V_s)[km][:, 0])),
                         p50_v_g=float(np.median(G.lab(V if pose == '135' else V_s)[km & ~hf[pose]][:, 0])) if (km & ~hf[pose]).any() else float('nan'),
                         p50_v_h=float(np.median(G.lab(V if pose == '135' else V_s)[km & hf[pose]][:, 0])) if (km & hf[pose]).any() else float('nan'))
    fv['KV4_alb'] = float(np.median(roh['135']['alb'][m]))
    # K6-Box (Fleckbox an der Kopfmitte gespiegelt) im Videoraum: Mittel und Maximum
    xm = 0.5 * (kb[0] + kb[2]); bx0, by0, bx1, by1 = b['bbox']
    sp = (int(round(2 * xm - bx1)), by0, int(round(2 * xm - bx0)), by1)
    L6 = G.lab(V)[sp[1]:sp[3] + 1, sp[0]:sp[2] + 1, 0]
    return dict(zv={z['mass'].split()[0]: z for z in zv}, zs={z['mass'].split()[0]: z for z in zs}, iv=iv, is_=is_, fv=fv, fs=fs,
                roh=roh, auge_v=RV.lab1(b['rgb']), auge_s=RV.lab1(b_s['rgb']), kb=kb, kbs=kbs, k6=dict(box=sp, mittel=float(L6.mean()), max=float(L6.max())),
                img=img, ids=ids, V=V, img_s=img_s, ids_s=ids_s, V_s=V_s, blob_v=b, blob_s=b_s)


def ref2_kopf(P):
    if not _ref2:
        _ref2.update(_kopf_fenster(b2.modell(G.REF2), P))
    return _ref2


def _f(l):
    return '/'.join('%.1f' % v for v in l)


# Runde 3 (Kopf nach Nutzerbeschreibung, 2026-11-07): die Kopfzeilen K5, K6, K7, K9, S3, S4 folgen der Lesung (1)-(3) - Haar braeunlich,
# etwa 25 % dunkler als der Kopf (nicht schwarz), Nase = Zonengrenze, Kuppe hoechstens wenig heller als der Kopf, rotbraun. Bezug ist der
# Kopf-Median (L* P50 der Kopf-Id 9) im selben Bild (Videoraum fuer die Fensterzeilen, roher Render fuer T-K9). Video-Zahlen nur Richtung.
HAAR_REL_BAND = (0.62, 0.90)        # KV2 / Haarflaeche: L* relativ zum Kopf-Median (Lesung: Video Haar 12,9 bei Median 17,3 = 0,75)
KUPPE_DL_MAX = 8.0                  # KV1 / KS1 (Videoraum): hoechstens Kopf-Median + 8 (roh: + 6, T-K9c)
HAAR_A_MIN = 8.0                    # Kuppe rotbraun: a* >= 8 (Video 15,4 / 15,3 als Richtung)


def tk4(mo, P, hf_zonen=None, N=None):
    """T-K4 (Kopf-Merkmale an den Messblatt-Orten) und T-K5 (Kopf gegen Referenz 2), Runde 3."""
    R = ref2_kopf(P)
    N = N or _kopf_fenster(mo, P, hf_zonen)
    zv, zs, fv, fs = N['zv'], N['zs'], N['fv'], N['fs']
    rv, rs = R['fv'], R['fs']
    out = []
    w = lambda z, n: dict(z, mass='T-K4 ' + n)
    out.append(w(zv['K1'], 'K1 Augen vorn'))
    la = N['auge_v']
    out.append(zeile('T-K4 K2 Augenfleck vorn', 'L* >= KV5 + 20; hell und unbunt (a* <= 15, b* >= 0); Video 46,9/7,2/-1,1 nur Richtung',
                     '%s; KV5 %.1f' % (_f(la), fv['KV5'][0]), la[0] >= fv['KV5'][0] + 20 and la[1] <= 15 and la[2] >= 0))
    out.append(w(zv['K3'], 'K3 Augenfleck Groesse'))
    out.append(w(zv['K4'], 'K4 Augenlage vorn'))
    pv = N['roh']['135']['p50_v']
    lo, hi = HAAR_REL_BAND[0] * pv, HAAR_REL_BAND[1] * pv
    out.append(zeile('T-K4 K5 Haarfeld vorn (KV2)', 'braeunlich: L* %.1f-%.1f (%.2f-%.2f x Kopf-Median %.1f, Videoraum), a* > 0, b* > 0; Video 13,3/7,4/1,5 nur Richtung' % (
        lo, hi, HAAR_REL_BAND[0], HAAR_REL_BAND[1], pv), '%s' % _f(fv['KV2']), lo <= fv['KV2'][0] <= hi and fv['KV2'][1] > 0 and fv['KV2'][2] > 0))
    k6 = N['k6']
    out.append(zeile('T-K4 K6 zweites Auge verdeckt', 'Spiegelstelle in der Haarflaeche: Mittel L* <= Kopf-Median - 3 (%.1f), kein heller Fleck (max L* <= Auge - 15 = %.1f)' % (
        pv - 3, la[0] - 15), 'Mittel %.1f, max %.1f (Box %s)' % (k6['mittel'], k6['max'], k6['box']), k6['mittel'] <= pv - 3 and k6['max'] <= la[0] - 15))
    ph, pg = N['roh']['135']['p50_v_h'], N['roh']['135']['p50_v_g']
    # Nase = Merkmal an der Zonengrenze, mitteldunkel: zwischen den Nachbarfenstern GLEICHER HOEHE (links die graue Wange KV5, rechts das Haar
    # KV3r 0,62-0,85 x 0,66-0,81) - der Kopf hat in Pose 135 ein starkes Hell-Dunkel von oben nach unten (Lauf 4: KV2 29,1, KV3 24,3, KV4 19,7)
    # Vorlage 1 (Messblatt Abschnitt 2): Nase 18,2, Wange unter dem Auge KV5 18,9, Haar 13,3 -> Nase = Wange (-0,7), Nase = Haar + 4,9
    k3r = fv['KV3r']
    out.append(zeile('T-K4 K7 Nase (KV3)', 'Zonengrenze, mitteldunkel wie die Wange gleicher Hoehe: |L* - KV5| <= 4 (KV5 %.1f; Vorlage -0,7) und L* >= Haar rechts daneben '
                     'KV3r + 1 (%.1f; Vorlage +4,9); Zonen-P50 Videoraum Haar %.1f / grau %.1f' % (fv['KV5'][0], k3r[0], ph, pg), '%s' % _f(fv['KV3']),
                     abs(fv['KV3'][0] - fv['KV5'][0]) <= 4 and fv['KV3'][0] >= k3r[0] + 1))
    d8 = G.de00(fv['KV4'], rv['KV4'])
    ga = R['roh']['135']['a_p50_a'] - 8
    # Runde 3: das Kinn liegt in der grauen Zone (Lesung 1 "links ... bis Nase") - kein Farbziel Ref2-Kinn mehr (dE00 nur Ausgabe)
    out.append(zeile('T-K4 K8 Kinn/Maulzone (KV4, Kopf-Id 9 erodiert)', 'kein dunkles Maul in der Textur: Albedo-L* P50 >= Ref2-Kopfhaut %.1f - 8; b* >= 0 '
                     '(Ref2 KV4 %s nur Ausgabe; das Kinn liegt in Pose 135 im Schatten der V4-Kopfform)' % (ga + 8, _f(rv['KV4'])),
                     'Albedo %.1f; %s (dE00 zu Ref2 KV4 %.1f)' % (fv['KV4_alb'], _f(fv['KV4']), d8), fv['KV4_alb'] >= ga and fv['KV4'][2] >= 0))
    out.append(zeile('T-K4 K9 Kopf-Oberseite vorn (KV1)', 'dunkel rotbraun: a* >= %.0f, b* > 0; L* <= Kopf-Median + %.0f (%.1f) und >= Haar KV2; Video 22,6/15,4/5,2 nur Richtung' % (
        HAAR_A_MIN, KUPPE_DL_MAX, pv + KUPPE_DL_MAX), '%s' % _f(fv['KV1']),
        fv['KV1'][1] >= HAAR_A_MIN and fv['KV1'][2] > 0 and fv['KV2'][0] <= fv['KV1'][0] <= pv + KUPPE_DL_MAX))
    out.append(w(zv['K10'], 'K10 Kopfbox vorn'))
    z1 = zs['S1']
    ls = N['auge_s']
    w_, h_ = [float(x) for x in z1['ist'].split('; ')[1].split(' H')[0].split(' x ')]
    nk = int(z1['ist'].split('; ')[-1].split()[0])
    out.append(zeile('T-K4 S1 Auge seitlich', '1 heller Fleck; 0,028-0,052 x 0,040-0,070 H; hell und unbunt (a* <= 15, b* >= 0)',
                     '%s; %.3f x %.3f H; %d Komp.' % (_f(ls), w_, h_, nk),
                     nk == 1 and 0.028 <= w_ <= 0.052 and 0.040 <= h_ <= 0.070 and ls[1] <= 15 and ls[2] >= 0))
    out.append(w(zs['S2'], 'S2 Augenlage seitlich'))
    ps = N['roh']['85']['p50_v']
    ab = float(zs['S3']['ist'].split('Abstand ')[1].split()[0])
    out.append(zeile('T-K4 S3 Haarkante seitlich (KS2)', 'dunkler Streifen vor dem Auge: L* <= Kopf-Median (Pose 85, Videoraum %.1f) - 5; Abstand zum Fleck <= 0,01 H; '
                     'Video 19,1/1,2/1,1 nur Richtung' % ps, '%s; Abstand %.3f H' % (_f(fs['KS2']), ab), fs['KS2'][0] <= ps - 5 and ab <= 0.01))
    out.append(zeile('T-K4 S4 Kopf-Oberseite seitlich (KS1)', 'dunkel rotbraun: a* >= %.0f, b* > 0; L* <= Kopf-Median (Pose 85) + %.0f (%.1f); Video 27,6/15,3/3,3 nur Richtung' % (
        HAAR_A_MIN, KUPPE_DL_MAX, ps + KUPPE_DL_MAX), _f(fs['KS1']), fs['KS1'][1] >= HAAR_A_MIN and fs['KS1'][2] > 0 and fs['KS1'][0] <= ps + KUPPE_DL_MAX))
    for pose in ('135', '85'):
        n_, r_ = N['roh'][pose], R['roh'][pose]
        out.append(zeile('T-K5a Kopf-Grundton L* P50 Pose %s' % pose, 'graue Zone (ausserhalb Haar/Kuppe) wie Ref2 (dort %.1f, ganzer Kopf %.1f) +- 4' % (
            r_['p50_a'], r_['p50']), '%.1f (Haar+Kuppe %.0f %% der Kopfpixel; Albedo %.1f, Ref2 %.1f)' % (
            n_['p50_a'], 100 * n_['anteil_h'], n_['a_p50_a'], r_['a_p50_a']), abs(n_['p50_a'] - r_['p50_a']) <= 4))
        out.append(zeile('T-K5b Kopf-Grundton/Rumpf Pose %s' % pose, 'graue Zone >= Ref2 - 0,2 (Ref2 dort %.2f, ganzer Kopf %.2f)' % (
            r_['verh_a'], r_['verh']), '%.2f' % n_['verh_a'], n_['verh_a'] >= r_['verh_a'] - 0.2))
        out.append(zeile('T-K5c violett Pose %s' % pose, 'Anteil b* < 0 <= 0,02 (Ref2 %.2f)' % r_['viol'], '%.3f' % n_['viol'], n_['viol'] <= 0.02))
        out.append(zeile('T-K5d dunkle Textur nur in Haar/Kuppe Pose %s' % pose, 'Anteil Albedo-L* < 25 in der grauen Zone <= Ref2 + 0,05 '
                         '(Ref2 %.2f)' % r_['a25_a'], '%.2f (Render L* < 20 ausserhalb %.2f, Ref2 %.2f; in Haar/Kuppe %.2f)' % (
                             n_['a25_a'], n_['d20_a'], r_['d20_a'], n_['d20_h']), n_['a25_a'] <= r_['a25_a'] + 0.05))
        out.append(zeile('T-K5 ganzer Kopf Pose %s' % pose, 'Ref2 L* P50 %.1f, Kopf/Rumpf %.2f, L* < 20 %.2f' % (r_['p50'], r_['verh'], r_['d20']),
                         'L* P50 %.1f, Kopf/Rumpf %.2f, L* < 20 %.2f' % (n_['p50'], n_['verh'], n_['d20']), True, info=True,
                         note='gewertet in T-K9e'))
    return out, dict(vorn=N['iv'], seite=N['is_'])


# ---------------------------------------------------------------------------------------------------- Runde 3: Zonen, T-K9, T-K8
def zonen_emd(A, kp):
    """Zonenkarten (Texel, 1 grau / 2 Haar / 3 Kuppe / 0 sonst) der Kopfbilder eines EMD aus den Klassen der Ref2-Zeile 480 (ref2tex_v4.klassen_r3,
    disjunkt) - die Zone eines Texels ist die Klasse seines Index. So ist die Zone am EMD selbst messbar (auch fuer Gegenproben)."""
    K, _u, _n = RT.klassen_r3(RT.atlas(G.REF2))
    kl = np.zeros(256, np.uint8)
    for zi, k in ((1, 'grau'), (2, 'braun'), (3, 'kuppe')):
        kl[K[k]] = zi
    pb = 128 * (kp['page'] & 0xF)
    out = {}
    for name in ('vorn', 'seite'):
        u0, u1 = kp[name]['u']; v0, v1 = kp[name]['v']
        out[name] = kl[A['tex'][v0:v1 + 1, pb + u0:pb + u1 + 1]]
    return out


def zonen_bild(mo, kp, P, zk):
    """Zone je Bildpunkt (Kopf) fuer Pose 135 und 85 (roher Render ohne Licht, Kenn-CLUT)."""
    out = {}
    for pose in ('135', '85'):
        zm, ids = RT.zonen_render(mo, kp, P[pose], zk)
        out[pose] = dict(zm=zm, ids=ids)
    return out


def _grad(L):
    gx = cv2.Sobel(L.astype(np.float32), cv2.CV_32F, 1, 0, ksize=3); gy = cv2.Sobel(L.astype(np.float32), cv2.CV_32F, 0, 1, ksize=3)
    return np.hypot(gx, gy) / 8.0


def tk9(mo, P, kp, zb=None, zk=None, N=None):
    """T-K9 Zonen (Runde 3, roher Render Pose 135, Lab je Bildpunkt): grau (Chroma, L*, Lage links bis Nase), braun (L*, a*/b* > 0, Flaeche,
    Rand, Kanten), Kuppe (L* gegen Median, Farbton), Auge/Nase/Seite, ganzer Kopf, Rundum-Oval."""
    if zk is None:
        zk = zonen_emd(RT.atlas(mo_blob(mo)), kp)
    zb = zb or zonen_bild(mo, kp, P, zk)
    R = ref2_kopf(P)
    out = []
    img, ids = G.zeichne(mo, P['135'])
    Lab = G.lab(img.astype(float)); L = Lab[..., 0]; C = np.hypot(Lab[..., 1], Lab[..., 2])
    k = ids == 9; zm = zb['135']['zm']
    assert (zb['135']['ids'] == ids).all()
    kb = G.kopfbox(ids); x0, y0, x1, y1 = kb
    nx = x0 + 0.5 * (RV.FENSTER['KV3'][0] + RV.FENSTER['KV3'][2]) * (x1 - x0)         # Nase = Mitte des Fensters KV3 (x)
    g, h_, q = k & (zm == 1), k & (zm == 2), k & (zm == 3)
    haar = h_ | q
    n = float(k.sum())
    p50 = float(np.median(L[k])); rumpf = ids == 8
    cg = float(np.median(C[g])) if g.any() else float('nan'); cr = float(np.median(C[rumpf]))
    lg = float(np.median(L[g])) if g.any() else float('nan')
    X = np.arange(ids.shape[1])[None, :].repeat(ids.shape[0], 0)
    links = k & (X < nx)
    lage_g = float((X[g] < nx).mean()) if g.any() else 0.0
    anteil_links = float(g[links].mean()) if links.any() else 0.0
    r_p50 = R['roh']['135']['p50']
    # Lage: in Vorlage 1 beginnt die rote Kuppe bei 0,42 der Kopfbox-Breite, die Nase liegt bei 0,53-0,56 (Messblatt Abschnitt 2) - oben links
    # der Nase ist also ein Teil Kuppe, dazu der strukturfolgende Rand (+- 2-3 px): links der Nase mindestens 0,70 grau; von den grauen Pixeln
    # hoechstens 0,20 rechts der Nase (Rand und Kinn unter der Nase)
    out.append(zeile('T-K9a graue Zone (Pose 135)', 'Chroma P50 <= 0,5 x Rumpf Id 8 (%.1f -> %.1f); L* P50 = Ref2-Kopf %.1f +- 5; Lage: >= 0,80 der grauen Pixel links der Nase (x %.0f), '
                     '>= 0,70 der Kopfpixel links der Nase grau (Rest Kuppe oben, Rand)' % (cr, 0.5 * cr, r_p50, nx),
                     'Chroma %.1f; L* %.1f; links %.2f; Kopf links grau %.2f; Anteil %.2f der Kopfpixel (%d)' % (cg, lg, lage_g, anteil_links, g.sum() / n, g.sum()),
                     g.any() and cg <= 0.5 * cr and abs(lg - r_p50) <= 5 and lage_g >= 0.80 and anteil_links >= 0.70))
    # braun: Helligkeit, Farbe, Flaeche
    lh = float(np.median(L[h_])) if h_.any() else float('nan'); ah = float(np.median(Lab[h_][:, 1])) if h_.any() else 0.0; bh = float(np.median(Lab[h_][:, 2])) if h_.any() else 0.0
    fl = float(haar.sum() / n)
    out.append(zeile('T-K9b Haarflaeche braun (Pose 135)', 'L* P50 %.2f-%.2f x Kopf-Median (%.1f -> %.1f-%.1f, "um 22-24"); a* > 0, b* > 0; Flaeche Haar+Kuppe 0,40-0,55 der Kopfpixel' % (
        HAAR_REL_BAND[0], HAAR_REL_BAND[1], p50, HAAR_REL_BAND[0] * p50, HAAR_REL_BAND[1] * p50),
        'L* %.1f (%.2f x Median), a* %.1f, b* %.1f; Flaeche %.2f (Haar %.2f, Kuppe %.2f)' % (lh, lh / p50 if p50 else 0, ah, bh, fl, h_.sum() / n, q.sum() / n),
        h_.any() and HAAR_REL_BAND[0] * p50 <= lh <= HAAR_REL_BAND[1] * p50 and ah > 0 and bh > 0 and 0.40 <= fl <= 0.55))
    # Rand: Gradient an der Grenze grau <-> Haar/Kuppe; Kanten: x der Grenze je Zeile gegen eine Gerade
    gm = _grad(L)
    rand = (nd.binary_dilation(g, iterations=1) & haar) | (nd.binary_dilation(haar, iterations=1) & g)
    rand &= nd.binary_erosion(k, iterations=1)
    rg = float(gm[rand].mean()) if rand.any() else float('nan')
    rk = float(gm[RV.er(R['roh']['135']['ids'] == 9)].mean())
    xs_, ys_ = [], []
    for y in range(y0, y1 + 1):
        row_g = np.nonzero(g[y])[0]; row_h = np.nonzero(haar[y])[0]
        if len(row_g) and len(row_h) and row_h.max() > row_g.min():
            # Grenze = erste Haar-Spalte rechts der letzten grauen Spalte links davon
            cand = row_h[row_h > row_g.min()]
            xs_.append(float(cand.min())); ys_.append(float(y))
    if len(xs_) >= 8:
        pf = np.polyfit(ys_, xs_, 1); res = np.array(xs_) - np.polyval(pf, ys_); std_k = float(res.std())
    else:
        std_k = 0.0
    out.append(zeile('T-K9b Rand und Kanten (Pose 135)', 'Randgradient (Sobel/8, L*) 3-8 ("um 6", Ref2-Kopf innen %.1f); Grenze je Zeile: Std zur Geraden > 4 px (keine gerade Kante)' % rk,
                     'Rand %.1f (%d px); Kante Std %.1f px (%d Zeilen)' % (rg, rand.sum(), std_k, len(xs_)), rand.any() and 3.0 <= rg <= 8.0 and std_k > 4.0))
    # Kuppe
    lq = float(np.median(L[q])) if q.any() else float('nan'); aq = float(np.median(Lab[q][:, 1])) if q.any() else 0.0; bq = float(np.median(Lab[q][:, 2])) if q.any() else 0.0
    out.append(zeile('T-K9c Kuppe dunkel rotbraun (Pose 135)', 'L* P50 <= Kopf-Median + 6 (%.1f) und >= Haar (%.1f); a* >= %.0f, b* > 0; Anteil > 0,03 der Kopfpixel' % (p50 + 6, lh, HAAR_A_MIN),
                     'L* %.1f, a* %.1f, b* %.1f; Anteil %.2f' % (lq, aq, bq, q.sum() / n), q.any() and lh <= lq <= p50 + 6 and aq >= HAAR_A_MIN and bq > 0 and q.sum() / n > 0.03))
    # Auge / Nase / Seite
    N = N or _kopf_fenster(mo, P, dict((p_, (zb[p_]['zm'] >= 2) & (zb[p_]['ids'] == 9)) for p_ in ('135', '85')))
    sp = N['blob_v']['sp']; z_auge = int(zm[int(round(sp[1])), int(round(sp[0]))])
    bx3 = G.fenster(kb, RV.FENSTER['KV3']); z3 = zm[bx3[1]:bx3[3] + 1, bx3[0]:bx3[2] + 1]; z3 = z3[z3 > 0]
    g3 = float((z3 == 1).mean()) if len(z3) else 0.0
    kbs = N['kbs']; bxs = G.fenster(kbs, RV.FENSTER['KS2']); zs2 = zb['85']['zm'][bxs[1]:bxs[3] + 1, bxs[0]:bxs[2] + 1]; zs2 = zs2[zs2 > 0]
    Ls = G.lab(N['img_s'].astype(float))[..., 0]; ks_ = N['ids_s'] == 9
    ks2_m = np.zeros(ks_.shape, bool); ks2_m[bxs[1]:bxs[3] + 1, bxs[0]:bxs[2] + 1] = True; ks2_m &= ks_
    l_ks2 = float(Ls[ks2_m].mean()) if ks2_m.any() else float('nan'); p50_s = float(np.median(Ls[ks_]))
    ok4 = N['zv']['K4']['ok']; ok2 = N['zs']['S2']['ok']
    out.append(zeile('T-K9d Auge / Nase / Seite', 'Augenfleck (K1-Schwerpunkt) in der grauen Zone und K4 OK; Nase: Fenster KV3 zu 0,3-0,7 grau (Grenze); S2 OK; '
                     'KS2 (roh) <= Kopf-Median Pose 85 - 4 (%.1f)' % (p50_s - 4),
                     'Auge Zone %d, K4 %s; KV3 grau %.2f; S2 %s; KS2 L* %.1f (Zonen grau %.2f, Haar %.2f, Kuppe %.2f)' % (
                         z_auge, 'OK' if ok4 else 'ROT', g3, 'OK' if ok2 else 'ROT', l_ks2, *(float((zs2 == c).mean()) if len(zs2) else 0.0 for c in (1, 2, 3))),
                     z_auge == 1 and ok4 and 0.3 <= g3 <= 0.7 and ok2 and l_ks2 <= p50_s - 4))
    verh = float(L[k].mean() / L[rumpf].mean())
    out.append(zeile('T-K9e ganzer Kopf (Pose 135)', 'L* P50 >= 23; Kopf/Rumpf (Mittel L*) >= 0,9 (Ref2 %.1f / %.2f)' % (r_p50, R['roh']['135']['verh']),
                     'L* P50 %.1f; Kopf/Rumpf %.2f' % (p50, verh), p50 >= 23 and verh >= 0.9))
    # Rundum: Gier +120 / +180 Grad (neutrales Licht, Pose 85 gedreht wie Leiste 08): kein Fleck L* < 15 ueber 100 px
    p0 = dict(P['85']); flecken = []
    for dg in (120, 180):
        pp = dict(p0, gier=(p0['gier'] + int(round(dg * 4096 / 360))) & 0xFFF)
        im, idr = b2.zeichne(mo, pp, None, licht=False)
        Lr = G.lab(im.astype(float))[..., 0]
        dunkel = (idr == 9) & (Lr < 15)
        lb, nn = nd.label(dunkel)
        gr = int(max(nd.sum(dunkel, lb, range(1, nn + 1)))) if nn else 0
        flecken.append((dg, gr, int(dunkel.sum()), int((idr == 9).sum())))
    out.append(zeile('T-K9f Rundum kein schwarzes Oval', 'Gier +120 / +180 Grad (neutral): groesster Fleck L* < 15 im Kopf <= 100 px',
                     '; '.join('+%d: Fleck %d px (dunkel %d von %d)' % f for f in flecken), all(f[1] <= 100 for f in flecken)))
    info = dict(zonen={p_: {c: float((zb[p_]['zm'][zb[p_]['ids'] == 9] == ci).mean()) for c, ci in (('grau', 1), ('haar', 2), ('kuppe', 3))} for p_ in ('135', '85')},
                nase_x=float(nx), kante=dict(std=std_k, zeilen=len(xs_)))
    return out, info


_blobs = {}


def mo_blob(mo):
    """EMD-Bytes zu einem Modell (fuer zonen_emd ohne Pfad)."""
    return _blobs.get(id(mo))


def tk8(emd, vorher, kp):
    """T-K8 Aenderungsumfang gegen den Vorstand (EMD_V4ot2): alle Sektionen ausser dem TIM bytegleich (Geometrie 0); CLUT bytegleich; Texel
    nur in den Kopf-UV-Flaechen (vorn / seite) geaendert (Zahl, Bbox), Schwanz und alles uebrige bytegleich."""
    a, v = open(emd, 'rb').read(), open(vorher, 'rb').read()
    da, dv = EA.emd_dir(a), EA.emd_dir(v)
    gleich = [i for i in range(8) if a[da[i]:da[i + 1]] == v[dv[i]:dv[i + 1]]]
    A, V = RT.atlas(emd), RT.atlas(vorher)
    clut_gleich = A['clut'].shape == V['clut'].shape and bool((A['clut'] == V['clut']).all())
    diff = A['tex'] != V['tex']
    pb = 128 * (kp['page'] & 0xF)
    kopf = np.zeros(diff.shape, bool); boxen = {}
    for name in ('vorn', 'seite'):
        u0, u1 = kp[name]['u']; v0, v1 = kp[name]['v']
        kopf[v0:v1 + 1, pb + u0:pb + u1 + 1] = True
        d_ = diff[v0:v1 + 1, pb + u0:pb + u1 + 1]
        ys, xs = np.nonzero(d_)
        boxen[name] = (int(d_.sum()), [int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max())] if len(xs) else None)
    aussen = int((diff & ~kopf).sum())
    return [zeile('T-K8 Aenderungsumfang gegen %s' % os.path.basename(vorher), 'Sektionen 0-7 (Geometrie, Skelett, Clips ...) bytegleich; CLUT bytegleich; Texel nur in den Kopf-UV-Flaechen',
                  'Sektionen gleich %s; CLUT %s; Kopf-Texel geaendert vorn %d (Bbox %s), seite %d (Bbox %s); ausserhalb %d' % (
                      gleich, 'gleich' if clut_gleich else 'ABWEICHEND', boxen['vorn'][0], boxen['vorn'][1], boxen['seite'][0], boxen['seite'][1], aussen),
                  gleich == list(range(8)) and clut_gleich and aussen == 0)]


def form(emd, mo, P, lauf):
    Z = list(RV.machart(emd))
    zt, st, _ = RV.schwanz(mo, P)
    for z in zt:
        if z['mass'].split()[0] in ('T8', 'T9'):
            continue
        Z.append(z)
    zq, _iq = RV.schwanz_form(mo, P)
    Z += [z for z in zq if z['mass'].startswith('TK')]
    if lauf:
        import t7_koerperfest_v4 as T7K
        Z += T7K.riegel(T7K.messen(emd, lauf))
    return Z, st


def pruefen(emd, lauf=None, vorher=None):
    P = G.posen()
    R = RT.atlas(G.REF2); A = RT.atlas(emd)
    mo = b2.modell(emd)
    _blobs[id(mo)] = A['blob']
    kp = BF._variante_v4()['kopf_v4']
    Z = []
    Z += tk1(A, R)
    Z += tk2(A, R)
    z3, i3 = tk3(mo, P, A); Z += z3
    zk = zonen_emd(A, kp); zb = zonen_bild(mo, kp, P, zk)
    hf = dict((p_, (zb[p_]['zm'] >= 2) & (zb[p_]['ids'] == 9)) for p_ in ('135', '85'))
    N = _kopf_fenster(mo, P, hf)
    z4, i4 = tk4(mo, P, hf, N); Z += z4
    z9, i9 = tk9(mo, P, kp, zb, zk, N); Z += z9
    if vorher:
        Z += tk8(emd, vorher, kp)
    zf, st = form(emd, mo, P, lauf); Z += zf
    return Z, dict(tk3=i3, tk4=i4, tk9=i9, schwanz={k: v for k, v in st.items() if k not in ('C', 'breite', 'maske')})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd')
    ap.add_argument('--lauf')
    ap.add_argument('--vorher', help='Vorstand (EMD) fuer T-K8 Aenderungsumfang')
    ap.add_argument('--json'); ap.add_argument('--md')
    ap.add_argument('--kein-exit', action='store_true')
    a = ap.parse_args()
    Z, info = pruefen(a.emd, a.lauf, a.vorher)
    info['tk3'].pop('bild', None)
    rot = [z for z in Z if not z['ok']]
    sha = hashlib.sha256(open(a.emd, 'rb').read()).hexdigest()
    print('EMD %s sha256 %s (Ref2 %s)' % (a.emd, sha[:8], hashlib.sha256(open(G.REF2, 'rb').read()).hexdigest()[:8]))
    for z in Z:
        print('%-4s %-46s soll: %-60s ist: %s %s' % ('INFO' if z.get('info') else ('OK' if z['ok'] else 'ROT'), z['mass'], z['soll'], z['ist'],
                                                     ('[' + z['note'] + ']') if z.get('note') else ''))
    print('ROT: %d von %d (INFO-Zeilen nicht gezaehlt: %d)' % (len(rot), len([z for z in Z if not z.get('info')]),
                                                            len([z for z in Z if z.get('info')])))
    if a.json:
        json.dump(dict(emd=a.emd, sha256=sha, zeilen=Z, info=info), open(a.json, 'w'), indent=1,
                  default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    if a.md:
        with open(a.md, 'w', encoding='utf-8') as f:
            f.write('EMD sha256 %s\n\n| Mass | Soll | Ist | |\n|---|---|---|---|\n' % sha[:8])
            for z in Z:
                f.write('| %s | %s | %s | %s |\n' % (z['mass'], z['soll'], z['ist'] + ((' (' + z['note'] + ')') if z.get('note') else ''),
                                                     'INFO' if z.get('info') else ('OK' if z['ok'] else 'ROT')))
    if rot and not a.kein_exit:
        sys.exit(1)


if __name__ == '__main__':
    main()
