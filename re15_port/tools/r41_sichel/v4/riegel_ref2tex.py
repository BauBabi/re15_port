"""Riegel V4 Originaltexturen (Vorgabe des Orchestrators 2026-11-07, ersetzt Messblatt V4 Abschnitt 5 und die Video-Farbriegel F*/FO/FS/O7/
O9/O10) - Exit 1, wenn eine Zeile rot ist.

    python riegel_ref2tex.py <EMD> [--lauf debug.log] [--json aus.json] [--md aus.md] [--kein-exit]

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
    return [zeile('T-K2 keine neue Farbe (Kopf, Schwanz)', 'jeder abgedeckte Texel von Mesh 9 und 16-22: 15-Bit-Wert in der Ref2-CLUT; '
                  'Zeilen = Ref2-Zeile oder Kopie', '%d von %d Texeln mit Farbe ausserhalb Ref2; %d verschiedene Farben; %s' % (
                      neu, ges, len(farben), '; '.join('%s: %s' % (k, ', '.join(v)) for k, v in zinfo.items())),
                  neu == 0 and zeilen_ok)]


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


def _kopf_fenster(mo, P):
    zv, iv, (img, ids, V, b) = RV.kopf_vorn(mo, P)
    zs, is_, (img_s, ids_s, V_s, b_s) = RV.kopf_seite(mo, P)
    kb, kbs = G.kopfbox(ids), G.kopfbox(ids_s)
    fv = {}
    for k in ('KV1', 'KV2', 'KV3', 'KV5'):
        fv[k] = RV.fenster_lab(V, kb, RV.FENSTER[k])[0]
    bx = G.fenster(kb, RV.FENSTER['KV4'])
    m = np.zeros(ids.shape, bool); m[bx[1]:bx[3] + 1, bx[0]:bx[2] + 1] = True
    m &= RV.er(ids == 9, 2)
    fv['KV4'] = RV.lab1(V[m].mean(0))
    fs = {k: RV.fenster_lab(V_s, kbs, RV.FENSTER[k])[0] for k in ('KS1', 'KS2')}
    # Haarfeld im Bild: vorn Fenster KV2 und die K6-Box (Fleckbox an der Kopfmitte gespiegelt), seitlich Fenster KS2 (Messblatt V4
    # Abschnitt 7: dort MUSS es dunkel sein); am Ref2-Kopf dieselben Fenster an seiner Kopfbox
    hf = {}
    m_ = np.zeros(ids.shape, bool); b2_ = G.fenster(kb, RV.FENSTER['KV2']); m_[b2_[1]:b2_[3] + 1, b2_[0]:b2_[2] + 1] = True
    xm = 0.5 * (kb[0] + kb[2]); bx0, by0, bx1, by1 = b['bbox']
    m_[by0:by1 + 1, int(round(2 * xm - bx1)):int(round(2 * xm - bx0)) + 1] = True
    hf['135'] = m_
    m_ = np.zeros(ids_s.shape, bool); b3_ = G.fenster(kbs, RV.FENSTER['KS2']); m_[b3_[1]:b3_[3] + 1, b3_[0]:b3_[2] + 1] = True
    hf['85'] = m_
    roh = {}
    for pose, (im_, id_) in (('135', (img, ids)), ('85', (img_s, ids_s))):
        Lab = G.lab(im_.astype(float)); km = id_ == 9; k_, r_ = Lab[km], Lab[id_ == 8]
        ka = Lab[km & ~hf[pose]]
        # Albedo = Render / weisser Pruefkoerper (gleiches Licht, Texel = 248) x 248: die Texturhelligkeit ohne die Schattierung der Kopfform
        # (die Geometrie ist nach Vorgabe 4 die von V4 Runde 5 und nicht die von Ref2)
        w_, _ = G.zeichne(mo, P[pose], weiss=True)
        alb = G.lab(np.clip(im_.astype(float) / np.maximum(w_.astype(float), 1.0) * 248.0, 0, 255))[..., 0]
        # nur gut messbare Bildpunkte: weisser Pruefkoerper L* >= 40 (darunter ist der Quotient von der 8-Bit-Rundung bestimmt)
        aa = alb[km & ~hf[pose] & (G.lab(w_.astype(float))[..., 0] >= 40)]
        roh[pose] = dict(p50=float(np.median(k_[:, 0])), verh=float(k_[:, 0].mean() / r_[:, 0].mean()),
                         d20=float((k_[:, 0] < 20).mean()), viol=float((k_[:, 2] < 0).mean()), Lab=Lab, ids=id_,
                         p50_a=float(np.median(ka[:, 0])), verh_a=float(ka[:, 0].mean() / r_[:, 0].mean()), d20_a=float((ka[:, 0] < 20).mean()),
                         d20_h=float((Lab[km & hf[pose]][:, 0] < 20).mean()) if (km & hf[pose]).any() else 0.0, anteil_h=float(hf[pose][km].mean()),
                         a25_a=float((aa < 25).mean()), a_p50_a=float(np.median(aa)), alb=alb)
    # K8-Fenster (KV4, Kopf-Id 9 erodiert) auch als Albedo
    fv['KV4_alb'] = float(np.median(roh['135']['alb'][m]))
    return dict(zv={z['mass'].split()[0]: z for z in zv}, zs={z['mass'].split()[0]: z for z in zs}, iv=iv, is_=is_, fv=fv, fs=fs,
                roh=roh, auge_v=RV.lab1(b['rgb']), auge_s=RV.lab1(b_s['rgb']), kb=kb)


def ref2_kopf(P):
    if not _ref2:
        _ref2.update(_kopf_fenster(b2.modell(G.REF2), P))
    return _ref2


def _f(l):
    return '/'.join('%.1f' % v for v in l)


def tk4(mo, P):
    """T-K4 (Kopf-Merkmale an den Messblatt-Orten) und T-K5 (Kopf gegen Referenz 2), Runde 2."""
    R = ref2_kopf(P)
    N = _kopf_fenster(mo, P)
    zv, zs, fv, fs = N['zv'], N['zs'], N['fv'], N['fs']
    rv, rs = R['fv'], R['fs']
    out = []
    w = lambda z, n: dict(z, mass='T-K4 ' + n)
    out.append(w(zv['K1'], 'K1 Augen vorn'))
    la = N['auge_v']
    out.append(zeile('T-K4 K2 Augenfleck vorn', 'L* >= KV5 + 20; blass (a* <= 15, b* >= 0); Video-Farbziel 46,9/7,2/-1,1 ueberholt',
                     '%s; KV5 %.1f' % (_f(la), fv['KV5'][0]), la[0] >= fv['KV5'][0] + 20 and la[1] <= 15 and la[2] >= 0))
    out.append(w(zv['K3'], 'K3 Augenfleck Groesse'))
    out.append(w(zv['K4'], 'K4 Augenlage vorn'))
    g5 = R['roh']['135']['p50'] - 8
    out.append(zeile('T-K4 K5 Haarfeld vorn (KV2)', 'L* <= Auge - 25 und <= Ref2-Kopf L* P50 - 8 (%.1f); Video-Ziel 13,3/7,4/1,5 ueberholt' % g5,
                     '%s (Auge %.1f)' % (_f(fv['KV2']), la[0]), fv['KV2'][0] <= la[0] - 25 and fv['KV2'][0] <= g5))
    out.append(w(zv['K6'], 'K6 zweites Auge verdeckt'))
    out.append(zeile('T-K4 K7 Nase (KV3)', 'L* zwischen Haarfeld und Auge, >= Haarfeld + 8 (eigenes Merkmal neben dem Haar); Video-Ziel ueberholt',
                     '%s (Haar %.1f, Auge %.1f)' % (_f(fv['KV3']), fv['KV2'][0], la[0]),
                     fv['KV2'][0] + 8 <= fv['KV3'][0] <= la[0]))
    d8 = G.de00(fv['KV4'], rv['KV4'])
    # Kinn = Ref2-Haut, kein dunkles Maul: Textur (Albedo) mindestens Ref2-Kopfhaut - 8 (die Ref2-Kopftexel liegen nicht ortsgleich auf dem
    # V4-Kopf - die Quelle ist gekachelt -, deshalb gegen den Hautton von Ref2, nicht gegen Ref2s Kinnstelle; die steht zum Vergleich da)
    ga = R['roh']['135']['a_p50_a'] - 8
    out.append(zeile('T-K4 K8 Kinn/Maulzone (KV4, Kopf-Id 9 erodiert)', 'kein dunkles Maul: Albedo-L* P50 >= Ref2-Kopfhaut %.1f - 8; Farbe dE00 <= 10 '
                     'zu Ref2 KV4 (%s, Albedo %.1f); Video-Ziel 9,9/9,9/3,8 ueberholt' % (ga + 8, _f(rv['KV4']), rv['KV4_alb']),
                     'Albedo %.1f; %s dE00 %.1f' % (fv['KV4_alb'], _f(fv['KV4']), d8), d8 <= 10 and fv['KV4_alb'] >= ga))
    out.append(zeile('T-K4 K9 Kopf-Oberseite rot vorn (KV1)', 'a* >= 11 (Messblatt); |dL*| zu Ref2 <= 8 (Ref2 %s); Video-Ziel ueberholt' % _f(rv['KV1']),
                     _f(fv['KV1']), fv['KV1'][1] >= 11 and abs(fv['KV1'][0] - rv['KV1'][0]) <= 8))
    out.append(w(zv['K10'], 'K10 Kopfbox vorn'))
    z1 = zs['S1']
    ls = N['auge_s']
    w_, h_ = [float(x) for x in z1['ist'].split('; ')[1].split(' H')[0].split(' x ')]
    nk = int(z1['ist'].split('; ')[-1].split()[0])
    out.append(zeile('T-K4 S1 Auge seitlich', '1 heller Fleck; 0,028-0,052 x 0,040-0,070 H; blass (a* <= 15, b* >= 0); Video-Farbziel ueberholt',
                     '%s; %.3f x %.3f H; %d Komp.' % (_f(ls), w_, h_, nk),
                     nk == 1 and 0.028 <= w_ <= 0.052 and 0.040 <= h_ <= 0.070 and ls[1] <= 15 and ls[2] >= 0))
    out.append(w(zs['S2'], 'S2 Augenlage seitlich'))
    g3 = R['roh']['85']['p50'] - 8
    ab = float(zs['S3']['ist'].split('Abstand ')[1].split()[0])
    out.append(zeile('T-K4 S3 Haarkante seitlich (KS2)', 'L* <= Ref2-Kopf L* P50 (Pose 85) - 8 (%.1f); Abstand zum Fleck <= 0,01 H; '
                     'Video-Ziel 19,1/1,2/1,1 ueberholt' % g3, '%s; Abstand %.3f H' % (_f(fs['KS2']), ab), fs['KS2'][0] <= g3 and ab <= 0.01))
    out.append(zeile('T-K4 S4 Kopf-Oberseite rot seitlich (KS1)', 'a* >= 11 (Messblatt); |dL*| zu Ref2 <= 8 (Ref2 %s); Video-Ziel ueberholt' % _f(rs['KS1']),
                     _f(fs['KS1']), fs['KS1'][1] >= 11 and abs(fs['KS1'][0] - rs['KS1'][0]) <= 8))
    # T-K5 Kopf gegen Referenz 2 (roher Render, Lab je Id-Maske; Verfahren der Sicht-Pruefung Runde 1). Grundton = Kopf AUSSERHALB des
    # Haarfelds (vorn KV2 + K6-Box, seitlich KS2), weil dort nach K5/K6/S3 dunkel sein muss; der ganze Kopf steht als INFO daneben.
    for pose in ('135', '85'):
        n_, r_ = N['roh'][pose], R['roh'][pose]
        out.append(zeile('T-K5a Kopf-Grundton L* P50 Pose %s' % pose, 'ausserhalb des Haarfelds wie Ref2 (dort %.1f, ganzer Kopf %.1f) +- 4' % (
            r_['p50_a'], r_['p50']), '%.1f (Haarfeld %.0f %% der Kopfpixel; Albedo %.1f, Ref2 %.1f)' % (
            n_['p50_a'], 100 * n_['anteil_h'], n_['a_p50_a'], r_['a_p50_a']), abs(n_['p50_a'] - r_['p50_a']) <= 4))
        out.append(zeile('T-K5b Kopf-Grundton/Rumpf Pose %s' % pose, 'ausserhalb des Haarfelds >= Ref2 - 0,2 (Ref2 dort %.2f, ganzer Kopf %.2f)' % (
            r_['verh_a'], r_['verh']), '%.2f' % n_['verh_a'], n_['verh_a'] >= r_['verh_a'] - 0.2))
        out.append(zeile('T-K5c violett Pose %s' % pose, 'Anteil b* < 0 <= 0,02 (Ref2 %.2f)' % r_['viol'], '%.3f' % n_['viol'], n_['viol'] <= 0.02))
        out.append(zeile('T-K5d dunkle Textur nur im Haarfeld Pose %s' % pose, 'Anteil Albedo-L* < 25 ausserhalb des Haarfelds <= Ref2 + 0,05 '
                         '(Ref2 %.2f)' % r_['a25_a'], '%.2f (Render L* < 20 ausserhalb %.2f, Ref2 %.2f; im Haarfeld %.2f)' % (
                             n_['a25_a'], n_['d20_a'], r_['d20_a'], n_['d20_h']), n_['a25_a'] <= r_['a25_a'] + 0.05))
        out.append(zeile('T-K5 ganzer Kopf Pose %s' % pose, 'Ref2 L* P50 %.1f, Kopf/Rumpf %.2f, L* < 20 %.2f' % (r_['p50'], r_['verh'], r_['d20']),
                         'L* P50 %.1f, Kopf/Rumpf %.2f, L* < 20 %.2f' % (n_['p50'], n_['verh'], n_['d20']), True, info=True,
                         note='mit Haarfeld; nicht gewertet, Grundton in T-K5a/b'))
    return out, dict(vorn=N['iv'], seite=N['is_'])


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


def pruefen(emd, lauf=None):
    P = G.posen()
    R = RT.atlas(G.REF2); A = RT.atlas(emd)
    mo = b2.modell(emd)
    Z = []
    Z += tk1(A, R)
    Z += tk2(A, R)
    z3, i3 = tk3(mo, P, A); Z += z3
    z4, i4 = tk4(mo, P); Z += z4
    zf, st = form(emd, mo, P, lauf); Z += zf
    return Z, dict(tk3=i3, tk4=i4, schwanz={k: v for k, v in st.items() if k not in ('C', 'breite', 'maske')})


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd')
    ap.add_argument('--lauf')
    ap.add_argument('--json'); ap.add_argument('--md')
    ap.add_argument('--kein-exit', action='store_true')
    a = ap.parse_args()
    Z, info = pruefen(a.emd, a.lauf)
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
