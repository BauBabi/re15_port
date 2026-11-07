"""Runde 2: Bau mit Parametern + Masse der Pruefung r1 (Kopf-Helligkeit, Haelften, Auge als dunkle Pupille mit Glanz, KV1/KV2, K4/K10/S2/KS2, Zaehne)."""
import os, sys, json
import numpy as np, cv2
T = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_sichel/re15_port/tools'
for p in (T, T + '/r41_sichel/v4', T + '/r41_sichel/re2kopf', T + '/r41_sichel/v2'):
    sys.path.insert(0, p)
import v4_gemein as G, b2, kopf_re2_bau as KB, kopf_re2_mess as KM, riegel_v4 as RV
P = G.posen()
FAN = [19, 20, 41]


def messen(mo):
    M = KM.messen(mo, P, bilder=True)
    out = {}
    for h in ('135', '85'):
        a = M[h]; il, ids = a['img'], a['ids']
        ir, _ = G.zeichne(mo, P[h], licht=False)
        Ll = G.lab(il.astype(float))[..., 0]; Lr = G.lab(ir.astype(float))[..., 0]
        V = G.videoraum(il); LabV = G.lab(V); Lv = LabV[..., 0]
        k = ids == 9; r8 = ids == 8; H = a['H']; kb = a['kopfbox']; x0, y0, x1, y1 = kb; xm = 0.5 * (x0 + x1)
        X = np.arange(640)[None, :].repeat(480, 0)
        li, re_ = k & (X < xm), k & (X >= xm)
        d = dict(K10=a.get('K10'), kopf_p50_lit=float(np.median(Ll[k])), rumpf_p50_lit=float(np.median(Ll[r8])),
                 kopf_rumpf=float(Ll[k].mean() / Ll[r8].mean()), kopf_p50_v=float(np.median(Lv[k])),
                 haelften_lit=(float(np.median(Ll[li])), float(np.median(Ll[re_]))), haelften_raw=(float(np.median(Lr[li])), float(np.median(Lr[re_]))),
                 haelften_v=(float(np.median(Lv[li])), float(np.median(Lv[re_]))))
        if h == '135':
            d['K4_marker'] = a.get('K4'); d['auge_px'] = a['auge']['px'] if 'auge' in a else 0
            d['zaehne'] = (a['zaehne']['px'], a['zaehne'].get('anteil'))
            for kv in ('KV1', 'KV2', 'KV3', 'KV5'):
                l, bx = RV.fenster_lab(V, kb, RV.FENSTER[kv]); d[kv] = [round(float(v), 1) for v in l]
            _i, idf = G.zeichne(KM.teil(mo, FAN), P[h], licht=False)
            fan = (idf == 9) & k
            if fan.any():
                ys, xs = np.nonzero(fan); cx, cy = int(xs.mean()), int(ys.mean())
                umg = k & ~fan
                umg[:max(cy - 12, 0)] = False; umg[cy + 13:] = False; umg[:, :max(cx - 12, 0)] = False; umg[:, cx + 13:] = False
                core = a['ma'] if 'ma' in a and a['ma'].any() else fan
                d['auge'] = dict(fan_px=int(fan.sum()), fan_L_lit=float(np.median(Ll[fan])), fan_L_v=float(np.median(Lv[fan])),
                                 umg_L_lit=float(np.median(Ll[umg])) if umg.any() else None, umg_L_v=float(np.median(Lv[umg])) if umg.any() else None,
                                 kern_Lmax_lit=float(Ll[core].max()), kern_L_v_max=float(Lv[core].max()),
                                 fan_wh_H=((xs.ptp() + 1) / H, (ys.ptp() + 1) / H), fan_sp_K4=((xm - xs.mean()) / H, (ys.mean() - y0) / H))
                win = cv2.blur(Lv.astype(np.float32), (3, 3)); er = cv2.erode(k.astype(np.uint8), np.ones((3, 3), np.uint8)) > 0
                cand = np.where(er & li, win, 1e9); iy, ix = np.unravel_index(np.argmin(cand), cand.shape)
                d['dunkelster_links'] = dict(xy=(int(ix), int(iy)), L=float(win[iy, ix]), im_faecher=bool(fan[iy, ix]),
                                             K4=((xm - ix) / H, (iy - y0) / H))
                bx0, bx1 = int(xs.min()), int(xs.max()); by0, by1 = int(ys.min()), int(ys.max())
                sp = (int(round(2 * xm - bx1)), by0, int(round(2 * xm - bx0)), by1)
                d['K6_spiegel_L_v'] = float(RV.lab1(G.mittel(V, sp))[0])
        else:
            d['S2'] = a.get('S2'); d['KS2'] = (a['KS2']['kopf'][0], a['KS2']['kopf_anteil'], a['kopf_p50_v']) if 'KS2' in a else None
        out[h] = d
    return out, M


def kurz(o):
    a, b = o['135'], o['85']
    s = 'Kopf %.1f (Rumpf %.1f, K/R %.2f, V %.1f) | Haelften lit %.1f/%.1f raw %.1f/%.1f V %.1f/%.1f' % (
        a['kopf_p50_lit'], a['rumpf_p50_lit'], a['kopf_rumpf'], a['kopf_p50_v'], *a['haelften_lit'], *a['haelften_raw'], *a['haelften_v'])
    if 'auge' in a:
        au = a['auge']
        s += ' | Auge fan L %.1f vs Umg %.1f (V %.1f/%.1f) Kern max %.1f; dunkelster links im Faecher %s K4 %.3f/%.3f' % (
            au['fan_L_lit'], au['umg_L_lit'] or 0, au['fan_L_v'], au['umg_L_v'] or 0, au['kern_Lmax_lit'],
            a['dunkelster_links']['im_faecher'], *a['dunkelster_links']['K4'])
    s += ' | KV1 %s KV2 %s KV3 %s K6 %.1f | K10 %.3fx%.3f K4m %s Z %s | 85: S2 %s KS2 %s' % (
        a.get('KV1'), a.get('KV2'), a.get('KV3'), a.get('K6_spiegel_L_v', 0), *a['K10'], a.get('K4_marker'), a.get('zaehne'), b.get('S2'), b.get('KS2'))
    return s


def param_mit(over):
    p = json.loads(json.dumps(KB.PARAM))
    for kk, vv in over.items():
        if isinstance(vv, dict) and isinstance(p.get(kk), dict):
            for k2, v2 in vv.items():
                if isinstance(v2, dict):
                    p[kk][k2].update(v2)
                else:
                    p[kk][k2] = v2
        else:
            p[kk] = vv
    return p


if __name__ == '__main__':
    overs = [json.loads(a) for a in sys.argv[1:] if a.startswith('{')]
    for over in (overs or [{}]):
        p = param_mit(over)
        neu, prot, _ = KB.bauen(p)
        mo = b2.modell(neu)
        o, M = messen(mo)
        print(json.dumps(over), '->', kurz(o)); sys.stdout.flush()
