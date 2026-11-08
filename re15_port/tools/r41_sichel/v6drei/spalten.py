"""SF V6 drei Punkte: Spalten-Pruefung der Schwanzgelenke ueber eine Stichprobe der 912 Keyframes (Bauer, 2026-11-08).

    python spalten.py --emd EMD [--n 50] [--ohne-manschette] [--bild DIR] [--json DATEI]

Verfahren (Bildmessung, kein RE-Befund): Die Schwanzglieder 16-22 sind offene Rohre (nur der Deckel in 16 und die Spitze in 22 sind zu);
geschlossen wirkt der Schwanz, solange kein offenes Rohrende aus dem Nachbarglied herausschaut. Eine Spalte ist genau die Stelle, an der
man in ein Rohr HINEIN sieht. Gemessen mit einem Tiefenpuffer ueber ALLE Schwanzdreiecke (Projektion wie b2.dreiecke_ids: GTE,
Abbildung ins Videobild; Vorder-/Rueckseite nach dem NCLIP-Vorzeichen wie dort): Luecke = Pixel, an denen die naechste Flaeche eine
Rueckseite (= Innenwand) ist und die nach einer Oeffnung 3 x 3 bleiben (eine Spalte ist mindestens 3 px breit; 1-2 px breite
Innenwand-Saeume liegen an fast streifenden Flaechen der Umrisskante, deren NCLIP auf ganzzahligen Bildpunkten kippt - V5f, sfv7/sp3_tafel.png). (Das Malerverfahren des Rasterers taugt dafuer nicht: lange Dreiecke mit weit entfernter mittlerer Tiefe
lassen Innenwaende durchscheinen - Lauf 2, sfv7/sp2_*: in V5f jedes Bild "luecke".) Je Keyframe der Stichprobe (n gleich
verteilte VERSCHIEDENE Keyframe-Indizes; 912 = Zahl der verschiedenen Keyframes ueber alle 21 Clips) und je Ansicht (Spielkamera 50A0/12,
Lage P2, Gier P2 + 0/90/180/270 Grad) wird nur der Schwanz gerendert (ohne Licht). Zuordnung zum Gelenk = naechstes projiziertes Gelenk.
Gegenprobe: dieselbe Messung ohne Manschette (--ohne-manschette: Anfangsringe auf y = 0) muss Luecken finden.
Verworfen (Bauer 2026-11-08, Bilder sfv7/sp_cmp2/3.png): Kugelscheibe mit Gelenkradius gegen den Umriss - zaehlte schmale/versetzte
Rohrumrisse und vom Schwanz umschlossenen Hintergrund (Schleife) als Luecke.
"""
import os, sys, argparse, copy, json
import numpy as np, cv2
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import drei_gemein as DG          # noqa: E402
import schwanz_form as SF           # noqa: E402
import kopf_orig_bau as KB          # noqa: E402

G, V, b2, BB = DG.G, DG.V, DG.b2, DG.BB
KERN = np.ones((3, 3), np.uint8)    # Oeffnung 3 x 3: Innenwand-Saeume an der Umrisskante (1-2 px, NCLIP auf ganzzahligen Bildpunkten) zaehlen nicht


def keyframes(mo, n):
    """n gleich verteilte VERSCHIEDENE Keyframes, je Keyframe das erste (Clip, Bild), das ihn zeigt."""
    erst = {}
    for c, (_f0, k) in enumerate(mo.clips):
        for b in range(k):
            erst.setdefault(int(BB.kf_von_clip(mo, c, b)), (c, b))
    kfs = sorted(erst)
    idx = np.unique(np.round(np.linspace(0, len(kfs) - 1, n)).astype(int))
    return len(kfs), [erst[kfs[i]] for i in idx]


def dreiecke_tiefe(mo, pose):
    """Schwanzdreiecke (Knochen 16-22) mit Bildpunkten (Videoraum, Gleitkomma) und Kamera-z je Ecke, Vorder-/Rueckseite nach dem
    NCLIP-Vorzeichen des Rasterers (b2.dreiecke_ids: Dreieck m0 <= 0 bzw. Viereck m0 < 0 = Rueckseite)."""
    r, kam, M, _li = b2.raum()
    kf = BB.kf_von_clip(mo, pose['clip'], pose['bild'])
    g = int(pose['gier'])
    sn, cs = DG.b2.EA.sin_q12(g), DG.b2.EA.cos_q12(g)
    Y = np.array([[cs, 0, sn], [0, 4096, 0], [-sn, 0, cs]], dtype=np.int64)
    R, T = mo.pose(kf)
    lage = np.array((pose['x'], pose.get('y', 0), pose['z']), dtype=np.int64)
    aus = []
    for b in DG.SCHWANZ:
        m = mo.meshes[b]
        W = (Y @ R[b]) >> 12
        t = ((Y @ T[b]) >> 12) + lage
        VW, vT = (kam.V @ W) >> 12, ((kam.V @ t) >> 12) + kam.t
        for verts, faces, viereck in ((m.tv, m.tf, 0), (m.qv, m.qf, 1)):
            if not faces:
                continue
            q = (VW @ np.array([x[:3] for x in verts], dtype=np.int64).T) >> 12
            vx, vy, vz = q[0] + vT[0], q[1] + vT[1], q[2] + vT[2]
            sx, sy = BB._gte(vx, vy, vz, kam.h)
            X, Yv = BB.abbilden(sx.astype(float) + BB.PHASE, sy.astype(float) + BB.PHASE, M)
            for f in faces:
                ie = f[1::2]
                if any(vz[i] < 64 for i in ie):
                    continue
                p = [(int(sx[i]), int(sy[i])) for i in ie]
                m0 = BB._mac0(p[0], p[1], p[2])
                rueck = (m0 < 0) if viereck else (m0 <= 0)
                if viereck:
                    for a_, b_, c_ in ((0, 1, 3), (0, 3, 2)):
                        aus.append(([(X[ie[k]], Yv[ie[k]], float(vz[ie[k]])) for k in (a_, b_, c_)], rueck, b))
                else:
                    aus.append(([(X[i], Yv[i], float(vz[i])) for i in ie], rueck, b))
    return aus


def zpuffer(tris, H=480, W=640):
    """Tiefenpuffer (1/z, perspektivisch richtig interpoliert): naechste Flaeche je Pixel; Rueckgabe (belegt, rueckseite, knochen)."""
    iz = np.zeros((H, W)); rue = np.zeros((H, W), bool); kn = np.full((H, W), -1, np.int16)
    for pts, rueck, b in tris:
        x = np.array([p[0] for p in pts]); y = np.array([p[1] for p in pts]); w = 1.0 / np.array([p[2] for p in pts])
        fl = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0])
        if abs(fl) < 1e-9:
            continue
        x0, x1 = max(int(np.floor(x.min())), 0), min(int(np.ceil(x.max())) + 1, W)
        y0, y1 = max(int(np.floor(y.min())), 0), min(int(np.ceil(y.max())) + 1, H)
        if x0 >= x1 or y0 >= y1:
            continue
        PX, PY = np.meshgrid(np.arange(x0, x1) + 0.5, np.arange(y0, y1) + 0.5)
        w1 = ((PX - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (PY - y[0])) / fl
        w2 = ((x[1] - x[0]) * (PY - y[0]) - (PX - x[0]) * (y[1] - y[0])) / fl
        w0 = 1.0 - w1 - w2
        inn = (w0 >= -1e-9) & (w1 >= -1e-9) & (w2 >= -1e-9)
        if not inn.any():
            continue
        z = w0 * w[0] + w1 * w[1] + w2 * w[2]
        s_ = iz[y0:y1, x0:x1]
        neu = inn & (z > s_)
        s_[neu] = z[neu]; rue[y0:y1, x0:x1][neu] = rueck; kn[y0:y1, x0:x1][neu] = b
    return kn >= 0, rue, kn


def pruefen(blob, n=50, giers=(0, 1024, 2048, 3072), bild=None):
    mo = V.modell(blob)
    P2 = V.posen()['P2']
    gesamt, kfs = keyframes(mo, n)
    aus = dict(keyframes_gesamt=gesamt, stichprobe=len(kfs), ansichten=len(giers), bilder=0, luecke_px=0, bilder_mit_luecke=0,
               schlimmste=[], je_gelenk={str(16 + j): dict(px=0, faelle=0) for j in range(1, 7)})
    for c, b in kfs:
        for dg in giers:
            pose = dict(P2, clip=c, bild=b, gier=(int(P2['gier']) + dg) % 4096)
            bel, rue, kn = zpuffer(dreiecke_tiefe(mo, pose))
            roh = bel & rue
            innen = cv2.dilate(cv2.erode(roh.astype(np.uint8), KERN), KERN).astype(bool) & roh
            aus['roh_px'] = aus.get('roh_px', 0) + int(roh.sum())
            aus['bilder'] += 1
            k = int(innen.sum())
            if not k:
                continue
            aus['bilder_mit_luecke'] += 1; aus['luecke_px'] += k
            q = b2.projiziere(pose, DG.kette_punkte(mo, BB.kf_von_clip(mo, c, b)))
            ys, xs = np.nonzero(innen)
            dj = np.hypot(xs[:, None] - q[None, 1:7, 0], ys[:, None] - q[None, 1:7, 1])
            je = {}
            for j in np.argmin(dj, 1):
                je[str(17 + int(j))] = je.get(str(17 + int(j)), 0) + 1
            for g_, v in je.items():
                aus['je_gelenk'][g_]['px'] += v; aus['je_gelenk'][g_]['faelle'] += 1
            aus['schlimmste'].append(dict(clip=c, bild=b, gier_plus=dg, px=k, je_gelenk=je))
            if bild and len(aus['schlimmste']) <= 8:
                os.makedirs(bild, exist_ok=True)
                v_ = np.zeros((480, 640, 3), np.uint8); v_[bel] = (150, 150, 150); v_[innen] = (255, 0, 255)
                x0, y0 = int(xs.mean()), int(ys.mean())
                cv2.imwrite(os.path.join(bild, 'luecke_c%d_b%d_g%d.png' % (c, b, dg)),
                            cv2.resize(v_[max(y0 - 40, 0):y0 + 40, max(x0 - 40, 0):x0 + 40], None, fx=4, fy=4,
                                       interpolation=cv2.INTER_NEAREST)[..., ::-1])
    aus['schlimmste'] = sorted(aus['schlimmste'], key=lambda e: -e['px'])[:12]
    return aus


def ohne_manschette(blob):
    d, _, ms, tim = KB.teile(blob)
    ms = copy.deepcopy(ms)
    for i in DG.SCHWANZ[1:]:
        rg = [x for x in SF.ringe(ms[i]) if len(x[0]) == SF.S_]
        idx, c, _r = rg[0]
        for k in idx:
            ms[i]['v'][k][1] = int(round(ms[i]['v'][k][1] - c[1]))
    return KB.packen(blob, d, ms, tim)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--emd', required=True); ap.add_argument('--n', type=int, default=50)
    ap.add_argument('--ohne-manschette', action='store_true'); ap.add_argument('--bild'); ap.add_argument('--json')
    a = ap.parse_args()
    blob = open(a.emd, 'rb').read()
    if a.ohne_manschette:
        blob = ohne_manschette(blob)
    r = pruefen(blob, a.n, bild=a.bild)
    print(json.dumps({k: v for k, v in r.items() if k != 'schlimmste'}), '\nschlimmste', r['schlimmste'][:6])
    if a.json:
        json.dump(r, open(a.json, 'w'), indent=1)


if __name__ == '__main__':
    main()
