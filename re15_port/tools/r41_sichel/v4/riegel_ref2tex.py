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
  T-K4 Kopf-Merkmale an den Messblatt-Orten: K1-K10 und S1-S4 wie riegel_v4.py (Messblatt V4 Abschnitt 7). K8 wird an den Kopf-Bildpunkten
       (Id 9, 2 px erodiert wie alle Id-Masken in riegel_v4) im Fenster KV4 gemessen: das Fenster reicht bis 1,05 der Kopfbox und liegt am
       Port zur Haelfte auf dem Rumpf Id 8, der nach Vorgabe 1 Referenz 2 ist (hell; die Weichzeichnung sigma 1,5 traegt ihn 1-2 px in den
       Kopf); der Wortlaut (Fenster ganz) wird als INFO mit ausgegeben.
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
    return Z, dict(stat=[(int(b), [round(float(x), 1) for x in l], int(i)) for b, l, i in stat], bild=(img, ids))


def tk4(mo, P):
    """T-K4: K1-K10, S1-S4 (riegel_v4), K8 an den Kopf-Bildpunkten."""
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
            n_ges = int(m.sum()); n8 = int((m & (ids == 8)).sum())
            m &= RV.er(ids == 9, 2)                     # Kopf-Id 9, 2 px erodiert (Regel "2 px erodiertes Fenster" wie riegel_v4)
            la = RV.lab1(V[m].mean(0)); d8 = G.de00(la, RV.ZIEL['kinn'])
            hell = float((G.lab(V)[m][:, 0] > 35).mean())
            out.append(zeile('T-K4 K8 Kinn/Maulzone (Kopf-Id 9 erodiert im Fenster KV4)', '9,9/9,9/3,8 dE00 <= 6; Anteil L*>35 <= 0,05',
                             '%s dE00 %.1f; Anteil %.3f (%d Kopf-Bildpunkte)' % ('/'.join('%.1f' % v for v in la), d8, hell, int(m.sum())),
                             d8 <= 6 and hell <= 0.05))
            out.append(dict(z, mass='T-K4 K8 Messblatt-Wortlaut (Fenster ganz)', ok=True, info=True,
                            note='Fenster %d px, davon %d Rumpf Id 8 = Referenz 2 (Vorgabe 1); nicht gewertet' % (n_ges, n8)))
            continue
        out.append(dict(z, mass='T-K4 ' + n))
    out += [dict(z, mass='T-K4 ' + z['mass']) for z in zs]
    return out, dict(vorn=iv, seite=is_)


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
