"""Familienmasse K5f: Kontrast/Buntheit/Extreme + Ruhe R1-R4 (ruhe_mass.masse). Lab = OpenCV float (wie Messblatt RUHE/V4F)."""
import sys, math
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_form/analysis/befunde_runde41/form/varianten/K5/ruhe/skripte')
import numpy as np, cv2
import rl, ruhe_mass as RM

DUNKEL, HELL = 10.0, 50.0

def kontrast(img, msk, ero=2, fenster=5):
    k = 2 * ero + 1
    v = cv2.erode(msk.astype(np.uint8), np.ones((k, k), np.uint8)) > 0
    L = RM.lab(img)
    l = L[..., 0]; C = np.hypot(L[..., 1], L[..., 2])
    f = fenster
    vf = v.astype(np.float64)
    n = cv2.boxFilter(vf, -1, (f, f), normalize=False, borderType=cv2.BORDER_CONSTANT)
    s1 = cv2.boxFilter(l * vf, -1, (f, f), normalize=False, borderType=cv2.BORDER_CONSTANT)
    s2 = cv2.boxFilter(l * l * vf, -1, (f, f), normalize=False, borderType=cv2.BORDER_CONSTANT)
    voll = v & (n >= f * f - 0.5)
    var = np.maximum(s2[voll] / n[voll] - (s1[voll] / n[voll]) ** 2, 0)
    lk = float(np.sqrt(var).mean()) if voll.any() else float('nan')
    lv, cv_ = l[v], C[v]
    med = float(np.median(lv))
    return dict(px=int(v.sum()), L_mittel=round(float(lv.mean()), 2), L_std=round(float(lv.std()), 2),
                L_p5=round(float(np.percentile(lv, 5)), 2), L_p50=round(med, 2), L_p95=round(float(np.percentile(lv, 95)), 2),
                L_cv=round(float(lv.std() / max(lv.mean(), 1e-6)), 3),
                lokal5=round(lk, 2), lokal5_rel=round(lk / max(float(lv.mean()), 1e-6), 3), lokal_px=int(voll.sum()),
                C_mittel=round(float(cv_.mean()), 2), C_std=round(float(cv_.std()), 2), C_p90=round(float(np.percentile(cv_, 90)), 2),
                dunkel_pct=round(100 * float((lv < DUNKEL).mean()), 1), hell_pct=round(100 * float((lv > HELL).mean()), 1),
                dunkel_rel_pct=round(100 * float((lv < 0.5 * med).mean()), 1), hell_rel_pct=round(100 * float((lv > 2.0 * med).mean()), 1))

def alles(img, msk):
    d = kontrast(img, msk)
    r = RM.masse(img, msk)
    for k in ('S5', 'F_fremd', 'fremd_pct', 'spanne90', 'block_ton_streu', 'h0', 'F', 'F_grau_hell', 'F_hell'):
        d[k] = r[k]
    return d
