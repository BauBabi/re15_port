"""Rueckgewinnung der VORDERGRUND-REGION eines Cuts aus seinem Atlas.

Warum so: der Atlas ist eine dicht gepackte Sammlung von Bildschirm-Ausschnitten
(Q4: im Median nur 4 verschiedene Verschiebungen je Cut, aber im Median nur EINE
Zusammenhangskomponente — die Bloecke beruehren sich, komponentenweise Zuordnung
scheidet aus). Q1 hat gezeigt, dass der Atlasinhalt = Hintergrundinhalt an der
Zielstelle ist. Also wird je Verschiebung gesucht, nicht je Block:

  1. undurchsichtige Atlas-Pixel (Index != 0) = Suchmenge
  2. fuer JEDE ganzzahlige Verschiebung die MASKIERTE normalisierte Kreuzkorrelation
     zwischen Atlas und Hintergrund berechnen (per FFT, alle Terme in O(n log n))
  3. beste Verschiebung nehmen, alle Pixel zuordnen, die dort wirklich passen
  4. zugeordnete Pixel entfernen, wiederholen, bis nichts Gutes mehr uebrig ist

⛔ Erster Versuch scheiterte (IoU 0.011), weil er Atlas und Hintergrund GLOBAL
normierte: die durchsichtigen Pixel (Palettenindex 0, meist schwarz) stellen die
Mehrheit des Atlas und verschoben Mittelwert und Streuung so stark, dass zwei
unvergleichbare Signale verglichen wurden. Die Statistik MUSS ueber die Maske
laufen — und zwar auf beiden Seiten, je Verschiebung neu.
"""
import numpy as np
from scipy.signal import fftconvolve

def _corr(img, kern):
    """Kreuzkorrelation: Ergebnis[s] = sum_p kern[p]*img[p+s], Index s+(kh-1,kw-1)."""
    return fftconvolve(img, kern[::-1, ::-1], mode='full')

def _integral(b):
    return np.pad(np.cumsum(np.cumsum(b.astype(np.int32),0),1), ((1,0),(1,0)))

def _full_patches(mask, k):
    """Positionen, an denen ein kxk-Fenster VOLLSTAENDIG in der Maske liegt."""
    I = _integral(mask)
    h, w = mask.shape
    if h < k or w < k: return None
    s = I[k:, k:] - I[:-k, k:] - I[k:, :-k] + I[:-k, :-k]
    return s == k*k

def find_shifts(A, opaque, B, max_shifts=12, min_pixels=48, min_ncc=0.80,
                tol=0.55, patch_sizes=(28, 20, 14), tries=6):
    """-> Liste (dx, dy, zugeordnete Boolmaske im Atlas-Raster, NCC-Wert).

    HYPOTHESE UND PRUEFUNG statt Gesamtanpassung: eine Verschiebung kann nie alle
    Atlas-Pixel erklaeren (im Median gibt es 4). Der erste Versuch scheiterte genau
    daran — die maskierte NCC ueber ALLE Restpixel blieb unter der Schwelle und die
    Schleife brach in 82.5 % der Cuts sofort ab. Jetzt: ein kleiner, vollstaendig
    undurchsichtiger Ausschnitt liefert die HYPOTHESE (das ist der in Q2 auf 86 %
    Pixelgenauigkeit gemessene Weg), danach wird sie auf ALLEN Restpixeln geprueft
    und nur das Passende zugeordnet.
    """
    A = A.astype(np.float64); B = B.astype(np.float64)
    ah, aw = A.shape; bh, bw = B.shape
    rem = opaque.copy()
    out = []
    while rem.sum() >= min_pixels and len(out) < max_shifts:
        hyp = None
        for k in patch_sizes:
            ok = _full_patches(rem, k)
            if ok is None or not ok.any(): continue
            ys, xs = np.nonzero(ok)
            # gleichmaessig ueber die Restmenge streuen, nicht nur die erste Ecke
            order = np.argsort((ys.astype(np.int64) * 7919 + xs.astype(np.int64) * 104729) % 1000003)
            for t in range(min(tries, len(order))):
                py, px = int(ys[order[t]]), int(xs[order[t]])
                T = A[py:py+k, px:px+k]
                Tc = T - T.mean(); tn = np.sqrt((Tc*Tc).sum())
                if tn < 1e-6: continue
                num = fftconvolve(B, Tc[::-1, ::-1], mode='valid')
                s1  = fftconvolve(B, np.ones((k,k)), mode='valid')
                s2  = fftconvolve(B*B, np.ones((k,k)), mode='valid')
                var = s2 - s1*s1/(k*k); var[var < 1e-6] = 1e-6
                sc  = num/(np.sqrt(var)*tn)
                iy, ix = np.unravel_index(int(np.argmax(sc)), sc.shape)
                if sc[iy, ix] >= min_ncc:
                    hyp = (int(ix - px), int(iy - py), float(sc[iy, ix]))
                    break
            if hyp: break
        if not hyp: break
        dx, dy, score = hyp
        # PRUEFUNG der Hypothese auf allen Restpixeln, in z-Werten der zugehoerigen Menge
        ys, xs = np.nonzero(rem)
        by = ys + dy; bx = xs + dx
        ok = (by>=0)&(by<bh)&(bx>=0)&(bx<bw)
        if ok.sum() < min_pixels: break
        av = A[ys[ok], xs[ok]]; bv = B[by[ok], bx[ok]]
        za = (av - av.mean())/max(av.std(), 1e-6)
        zb = (bv - bv.mean())/max(bv.std(), 1e-6)
        keep_ok = np.abs(za - zb) < tol
        if keep_ok.sum() < min_pixels: break
        m = np.zeros_like(rem)
        m[ys[ok][keep_ok], xs[ok][keep_ok]] = True
        out.append((dx, dy, m, score))
        rem &= ~m
    return out

def region_from_atlas(idx, atlas_rgb_img, bg_rgb_img, **kw):
    """-> (Bildschirmmaske 240x320 bool, Anteil zugeordneter Atlas-Pixel)."""
    lum = lambda a: a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114
    opaque = (idx != 0)
    if opaque.sum() < 32: return np.zeros((240,320),bool), 0.0
    shifts = find_shifts(lum(atlas_rgb_img), opaque, lum(bg_rgb_img), **kw)
    screen = np.zeros((240,320), bool); assigned = 0
    for dx, dy, m, sc in shifts:
        ys, xs = np.nonzero(m); by = ys+dy; bx = xs+dx
        ok = (by>=0)&(by<240)&(bx>=0)&(bx<320)
        screen[by[ok], bx[ok]] = True
        assigned += int(ok.sum())
    return screen, assigned/float(opaque.sum())


def find_blocks(A, opaque, B, block=16, stride=8, min_ncc=0.90):
    """Block-Abstimmung: jeder undurchsichtige Atlas-Block sucht seine Stelle im Bild.

    Robuster als die gierige Zerlegung in globale Verschiebungen: dort pflanzt sich
    ein Fehlgriff in alle folgenden Runden fort, weil falsch zugeordnete Pixel aus
    der Restmenge verschwinden. Hier ist jeder Block unabhaengig, traegt seinen
    eigenen NCC-Wert als Vertrauensmass, und Bloecke unter der Schwelle fallen
    einfach weg statt etwas Falsches zu behaupten.

    -> Liste (ax, ay, dx, dy, score) je angenommenem Block.
    """
    A = A.astype(np.float64); B = B.astype(np.float64)
    ah, aw = A.shape; bh, bw = B.shape
    if bh < block or bw < block: return []
    ones = np.ones((block, block))
    s1 = fftconvolve(B,   ones, mode='valid')
    s2 = fftconvolve(B*B, ones, mode='valid')
    n  = block*block
    var = s2 - s1*s1/n
    var[var < 1e-6] = 1e-6
    sd = np.sqrt(var)
    I = _integral(opaque)
    out = []
    for ay in range(0, ah-block+1, stride):
        for ax in range(0, aw-block+1, stride):
            cnt = I[ay+block, ax+block] - I[ay, ax+block] - I[ay+block, ax] + I[ay, ax]
            if cnt != n: continue                      # nur vollstaendig undurchsichtige Bloecke
            T = A[ay:ay+block, ax:ax+block]
            Tc = T - T.mean(); tn = np.sqrt((Tc*Tc).sum())
            if tn < 1e-6: continue
            sc = fftconvolve(B, Tc[::-1, ::-1], mode='valid')/(sd*tn)
            iy, ix = np.unravel_index(int(np.argmax(sc)), sc.shape)
            v = float(sc[iy, ix])
            if v >= min_ncc:
                out.append((ax, ay, int(ix-ax), int(iy-ay), v))
    return out


def region_by_blocks(idx, atlas_rgb_img, bg_rgb_img, **kw):
    """-> (Bildschirmmaske, Anteil abgedeckter undurchsichtiger Atlas-Pixel, Blockzahl).

    Jedes Atlas-Pixel bekommt die Verschiebung des BESTEN Blocks, der es ueberdeckt;
    danach wird pixelweise gegen das Bild geprueft, sodass keine Zuordnung ohne
    Deckung stehen bleibt.
    """
    lum = lambda a: a[...,0]*0.299 + a[...,1]*0.587 + a[...,2]*0.114
    opaque = (idx != 0)
    if opaque.sum() < 32: return np.zeros((240,320),bool), 0.0, 0
    A = lum(atlas_rgb_img); B = lum(bg_rgb_img)
    blocks = find_blocks(A, opaque, B, **kw)
    ah, aw = idx.shape
    best = np.full((ah, aw), -1.0)
    sx_  = np.zeros((ah, aw), np.int32); sy_ = np.zeros((ah, aw), np.int32)
    bsz  = kw.get('block', 16)
    for ax, ay, dx, dy, v in blocks:
        sl = (slice(ay, ay+bsz), slice(ax, ax+bsz))
        upd = best[sl] < v
        best[sl] = np.where(upd, v, best[sl])
        sx_[sl]  = np.where(upd, dx, sx_[sl])
        sy_[sl]  = np.where(upd, dy, sy_[sl])
    have = opaque & (best >= 0)
    screen = np.zeros((240,320), bool)
    ys, xs = np.nonzero(have)
    by = ys + sy_[ys, xs]; bx = xs + sx_[ys, xs]
    ok = (by>=0)&(by<240)&(bx>=0)&(bx<320)
    screen[by[ok], bx[ok]] = True
    return screen, (have.sum()/float(opaque.sum())), len(blocks)
