#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""tor_vermessung.py - Vermessung des Gelaendertors von ROOM1170 ueber die Raumkameras.

NUR LESEND gegenueber dem Spiel: liest ROOM1170.RDT und die drei Hintergruende, schreibt
ausschliesslich nach build/tor_1170/ (vermessung.json, vermessung_*.png).

Aufruf
    python re15_port/tools/tor/tor_vermessung.py alles      # alles rechnen, JSON + Bilder schreiben
    python re15_port/tools/tor/tor_vermessung.py kamera     # nur die Kamerasaetze
    python re15_port/tools/tor/tor_vermessung.py versatz    # Versatz Bereich A -> Bereich B
    python re15_port/tools/tor/tor_vermessung.py ebene      # Torebene + Empfindlichkeit
    python re15_port/tools/tor/tor_vermessung.py cut12      # Pruefung der Cut-12-Kamera, Massstab

Grundlagen (Herkunft jeder Formel)
    Kamerasatz, Blickmatrix, H, Projektion     tor_kamera.py (Python-Fassung von
                                               re15_port/engine/src/camera_common.c:36-115)
    Kanten laengs der Fluchtlinien             tor_kanten.py
    Boden y = -7200                            Band 4 * 0x708 (re15_collision.c:186), Tuer-Saetze
                                               ROOM1170.RDT @0x1206 / @0x135A

Bezeichnungen
    Bereich A   Landeplatz-Seite, Weltkoordinaten der Cuts 0..5 (Cut 0 sieht das Tor)
    Bereich B   Laufsteg-Seite, Weltkoordinaten der Cuts 8..12 (Cut 11, Cut 12)
    hi / lo     Torseite mit grossem / kleinem Welt-x.  Cut 0: hi = Bild rechts.
                Cut 11 und Cut 12: hi = Bild links.
    u, v, w     Torebenen-Koordinaten: u laengs des Tors von der Achse des hi-Rahmenrohrs
                nach lo, v Hoehe ueber dem Boden y=-7200, w Dicke (senkrecht zur Ebene).

Jeder Startwert unten ist ein BILDPUNKT, abgelesen in der NEAREST-Vergroesserung mit
Pixelraster (Kontrollbilder vermessung_merkmale_cutNN.png); gemessen wird erst danach,
subpixelgenau, am Helligkeits- bzw. Gelbprofil.
"""
import json
import os
import struct
import sys

import numpy as np
from scipy import ndimage, optimize
from PIL import Image, ImageDraw

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import tor_kamera as tk          # noqa: E402
import tor_kanten as tn          # noqa: E402

WURZEL = tk.WURZEL
AUS = os.path.join(WURZEL, "build", "tor_1170")
BODEN_Y = -7200.0
NY = np.array([0.0, 1.0, 0.0])
NZ = np.array([0.0, 0.0, 1.0])

# Tuer-Saetze aus dem Auftrag, Bytes im Dossier (kamerasaetze() druckt sie mit)
TUER = {
    "slot0": dict(off=0x1206, rechteck=(1500, 14400, 2100, 1700)),
    "slot6": dict(off=0x135A, rechteck=(-11940, -28450, 1750, 1200)),
}
# Ausschnitte des Nutzers (Auftrag): Cut -> (x, y, b, h)
AUSSCHNITT = {0: (133, 81, 30, 28), 11: (73, 60, 44, 36), 12: (107, 115, 111, 76)}


# =============================================================================
# Bild- und Messgrundlagen
# =============================================================================
class Szene(object):
    def __init__(self):
        self.rdt = tk.lade_rdt()
        self.k = {c: tk.kamera(self.rdt, c) for c in range(13)}
        self.bg = {c: tk.hintergrund(c) for c in (0, 11, 12)}
        self.L = {c: tn.helligkeit(self.bg[c]) for c in self.bg}
        self.G = {c: tn.gelbheit(self.bg[c]) for c in self.bg}
        self.fpx = {c: tn.fluchtpunkt(self.k[c], [1, 0, 0]) for c in self.bg}
        self.fpy = {c: tn.fluchtpunkt(self.k[c], [0, 1, 0]) for c in self.bg}

    def feld(self, cut, name):
        return self.L[cut] if name == "L" else self.G[cut]


def kante(sz, cut, name, a, b, achse, hb=2.0, feld="L"):
    """Stufenkante laengs der Fluchtrichtung (tor_kanten.messe_kante). -> dict oder None."""
    fp = sz.fpx[cut] if achse == "x" else sz.fpy[cut]
    e = tn.messe_kante(sz.feld(cut, feld), name, a, b, fp, halbbreite=hb)
    if e is None:
        return None
    return dict(name=name, cut=cut, art="kante", achse=achse, feld=feld,
                p=[float(e.p[0]), float(e.p[1])], d=[float(e.d[0]), float(e.d[1])],
                niveau=[float(e.stufe[0]), float(e.stufe[1])],
                haelften=[float(e.halb[0]), float(e.halb[1])], streu=float(e.teil))


def linie(sz, cut, name, a, b, achse, hb=3.0, pol=+1, feld="L", schritt=0.25):
    """Mittellinie eines hellen (pol=+1) oder dunklen (pol=-1) Bandes laengs der
    Fluchtrichtung: Mitte zwischen den beiden Halbwertsdurchgaengen des gemittelten
    Querprofils. -> dict(p, d, breite, niveau, streu) oder None."""
    F = sz.feld(cut, feld)
    fp = sz.fpx[cut] if achse == "x" else sz.fpy[cut]
    a = np.asarray(a, float)
    b = np.asarray(b, float)
    m = 0.5 * (a + b)
    quer = np.arange(-hb, hb + 1e-9, schritt)
    erg = None
    for _ in range(4):
        d = np.asarray(fp, float) - m
        d /= np.linalg.norm(d)
        if np.dot(d, b - a) < 0:
            d = -d
        n = np.array([-d[1], d[0]])
        lang = np.linalg.norm(b - a)
        ts = np.arange(-lang / 2, lang / 2 + 1e-9, 0.5)

        def profil(tt):
            px = m[0] + np.outer(tt, np.full_like(quer, d[0])) + np.outer(np.ones_like(tt), quer * n[0])
            py = m[1] + np.outer(tt, np.full_like(quer, d[1])) + np.outer(np.ones_like(tt), quer * n[1])
            return tn.abtasten(F, px, py).mean(0)

        def mitte(prof):
            p = pol * prof
            i0 = len(p) // 2
            w = int(round(1.5 / schritt))
            ip = i0 - w + int(np.argmax(p[i0 - w:i0 + w + 1]))
            gipfel = p[ip]
            gl, gr = p[:ip + 1].min(), p[ip:].min()

            def kreuz(seite):
                g = gl if seite < 0 else gr
                h = 0.5 * (gipfel + g)
                i = ip
                while 0 < i < len(p) - 1 and p[i] > h:
                    i += seite
                if p[i] > h:
                    return None
                j = i - seite
                t = (p[j] - h) / (p[j] - p[i])
                return quer[j] + t * (quer[i] - quer[j])
            kl, kr = kreuz(-1), kreuz(+1)
            if kl is None or kr is None:
                return None
            return 0.5 * (kl + kr), kr - kl, (pol * gl, pol * gipfel, pol * gr)

        r = mitte(profil(ts))
        if r is None:
            return None
        vt = []
        for teil in np.array_split(ts, 4):
            rr = mitte(profil(teil))
            if rr is not None:
                vt.append(rr[0])
        m = m + r[0] * n
        erg = dict(name=name, cut=cut, art="linie", achse=achse, feld=feld,
                   p=[float(m[0]), float(m[1])], d=[float(d[0]), float(d[1])],
                   breite=float(r[1]), niveau=[float(v) for v in r[2]],
                   streu=float(np.std(vt)) if len(vt) > 1 else float("nan"))
        if abs(r[0]) < 0.02:
            break
    return erg


def kante_frei(sz, cut, name, achse, lo, hi, a0, a1, feld="L", minstufe=15.0):
    """Kante OHNE Richtungsvorgabe: je Spalte (achse 'h') bzw. je Zeile (achse 'v') der
    50-%-Durchgang, danach Ausgleichsgerade. Dient dazu, die Fluchtpunkte aus dem BILD zu
    bestimmen und mit denen der Kamera zu vergleichen.
    -> dict(t = Mitte der Laufkoordinate, w = Kantenlage dort, m = Steigung, se, rms, n)"""
    F = sz.feld(cut, feld)
    ts, vs = [], []
    for t in range(lo, hi + 1):
        p = (F[a0:a1 + 1, t] if achse == "h" else F[t, a0:a1 + 1]).astype(float)
        l, h = p[:2].mean(), p[-2:].mean()
        if abs(h - l) < minstufe:
            continue
        s = p - 0.5 * (l + h)
        c = [i + s[i] / (s[i] - s[i + 1]) for i in range(len(p) - 1)
             if s[i] * s[i + 1] < 0 or s[i] == 0]
        if len(c) != 1:
            continue
        ts.append(t + 0.5)
        vs.append(a0 + c[0] + 0.5)
    ts, vs = np.array(ts), np.array(vs)
    if len(ts) < 5:
        return None
    for _ in range(3):
        A = np.vstack([ts - ts.mean(), np.ones_like(ts)]).T
        (m, b), _, _, _ = np.linalg.lstsq(A, vs, rcond=None)
        r = vs - (m * (ts - ts.mean()) + b)
        ok = np.abs(r) < max(0.5, 2.5 * r.std())
        if ok.all():
            break
        ts, vs = ts[ok], vs[ok]
    se = r.std() / max(1e-9, np.sqrt(((ts - ts.mean()) ** 2).sum()))
    return dict(name=name, cut=cut, art="kante_frei", achse=achse, feld=feld,
                bereich=[lo, hi, a0, a1], t=float(ts.mean()), w=float(b), m=float(m),
                se=float(se), rms=float(r.std()), n=int(len(ts)))


def wert_bei(e, t):
    """Lage einer frei gemessenen Kante bei Laufkoordinate t."""
    return e["w"] + e["m"] * (t - e["t"])


def querprofil(sz, cut, mitte, laenge, achse, hb, schritt=0.5, feld="L"):
    """Gemitteltes Querprofil. achse 'y': gemittelt laengs der Senkrechten (Fluchtpunkt Y),
    Laufkoordinate = Bild-x auf der Bezugszeile mitte[1]. achse 'x' entsprechend."""
    F = sz.feld(cut, feld)
    fp = sz.fpy[cut] if achse == "y" else sz.fpx[cut]
    m = np.asarray(mitte, float)
    d = fp - m
    d /= np.linalg.norm(d)
    q = np.array([1.0, 0.0]) if achse == "y" else np.array([0.0, 1.0])
    ts = np.arange(-laenge / 2.0, laenge / 2.0 + 1e-9, 0.5)
    qs = np.arange(-hb, hb + 1e-9, schritt)
    out = []
    for qq in qs:
        p0 = m + qq * q
        dd = fp - p0
        dd /= np.linalg.norm(dd)
        if np.dot(dd, d) < 0:
            dd = -dd
        out.append(tn.abtasten(F, p0[0] + ts * dd[0], p0[1] + ts * dd[1]).mean())
    return qs + (m[0] if achse == "y" else m[1]), np.array(out)


def halbwert(xs, p, i_gipfel, seite, grund):
    """Halbwertsdurchgang eines Profils vom Gipfel aus nach links (-1) / rechts (+1)."""
    h = 0.5 * (p[i_gipfel] + grund)
    i = i_gipfel
    while 0 < i < len(p) - 1 and (p[i] - h) * (p[i_gipfel] - h) > 0:
        i += seite
    j = i - seite
    t = (p[j] - h) / (p[j] - p[i])
    return xs[j] + t * (xs[i] - xs[j])


def ebene_aus_bildgerade(k, p, d):
    """Ebene durch den Kameraort und eine Bildgerade. -> (Normale, c) mit n.w = c."""
    p = np.asarray(p, float)
    d = np.asarray(d, float)
    r1 = k.strahl(*(p - 30 * d))
    r2 = k.strahl(*(p + 30 * d))
    n = np.cross(r1, r2)
    n /= np.linalg.norm(n)
    return n, float(n.dot(k.ort))


def auf_z(k, px, py, z):
    w, vz = k.auf_ebene(px, py, NZ, z)
    return w, vz


# =============================================================================
# 1. Kamerasaetze
# =============================================================================
ERWARTET = {
    0: dict(off=0x60, flag=0, fov=26684, pos=(-1170, -11682, 4050), ziel=(4266, -5058, 17892)),
    11: dict(off=0x1C0, flag=0, fov=26684, pos=(-15606, -11556, -20988), ziel=(-13266, -4644, -30204)),
    12: dict(off=0x1E0, flag=1, fov=26684, pos=(-10756, -7256, -23388), ziel=(-11916, -9444, -29804)),
}


def kamerasaetze(sz, druck=True):
    rdt = sz.rdt
    ncut = rdt[1]
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    aus = dict(datei="re15_port/shared_assets/PSX/STAGE1/ROOM1170.RDT", groesse=len(rdt),
               kopf_0x00=rdt[0:8].hex(" "), nCut=ncut, zeiger_0x24=rdt[0x24:0x28].hex(" "),
               tabelle_off=cam, cuts={}, raster18={})
    for c in range(ncut):
        k = sz.k[c]
        werte = list(k.pos) + list(k.ziel)
        aus["raster18"][str(c)] = [int(v % 18 == 0) for v in werte]
    for c in (0, 11, 12):
        k = sz.k[c]
        kid = tk.kamera(rdt, c, exakt=True)
        R = k.R
        ortho = float(np.abs(R.dot(R.T) - np.eye(3)).max())
        # Unterschied Port-Matrix (ganzzahlig) gegen dieselbe Konstruktion in Gleitkomma
        probe = np.array([k.ziel[0], BODEN_Y - 700.0, k.ziel[2]])
        a = k.bild(probe)
        b = kid.bild(probe)
        e = ERWARTET[c]
        gleich = (k.off == e["off"] and k.flag == e["flag"] and k.fov == e["fov"]
                  and tuple(k.pos) == e["pos"] and tuple(k.ziel) == e["ziel"])
        aus["cuts"][str(c)] = dict(
            off=k.off, roh=k.roh.hex(" "), flag=k.flag, fov=k.fov, pos=list(k.pos),
            ziel=list(k.ziel), pri_off=k.pri, H=k.H, dist_sqrt0=k.dist_q, horiz_sqrt0=k.horiz_q,
            sin_cos=dict(sp=k.sp, cp=k.cp, sy=k.sy, cy=k.cy), R_q12=list(k.Ri), t=list(k.ti),
            R_mal_ey=[float(v) for v in R.dot([0, 1, 0])],
            welt_unten_ist_bild_unten=bool(R.dot([0, 1, 0])[1] > 0),
            fluchtpunkt_x=[float(v) for v in sz.fpx[c]], fluchtpunkt_y=[float(v) for v in sz.fpy[c]],
            orthonormal_fehler=ortho,
            port_gegen_gleitkomma_px=[float(a[0] - b[0]), float(a[1] - b[1])],
            stimmt_mit_auftrag=bool(gleich),
            auf_18er_raster=bool(all(aus["raster18"][str(c)])))
    for n, t in TUER.items():
        aus[n] = dict(off=t["off"], roh=rdt[t["off"]:t["off"] + 32].hex(" "), rechteck=list(t["rechteck"]))
    if druck:
        print("== 1. Kamerasaetze  ROOM1170.RDT  (nCut=%d, Tabelle @0x%X, 32 B je Cut)" % (ncut, cam))
        for c in (0, 11, 12):
            d = aus["cuts"][str(c)]
            print(" Cut %2d @0x%03X  %s" % (c, d["off"], d["roh"]))
            print("        flag %d fov %d H %d  Ort %s  Ziel %s  pri @0x%X" % (
                d["flag"], d["fov"], d["H"], tuple(d["pos"]), tuple(d["ziel"]), d["pri_off"]))
            print("        R(Q12) %s  t %s" % (d["R_q12"], d["t"]))
            print("        R*ey = (%.4f, %.4f, %.4f) -> Welt-unten = Bild-unten: %s   wie im Auftrag: %s   18er-Raster: %s" % (
                d["R_mal_ey"][0], d["R_mal_ey"][1], d["R_mal_ey"][2], d["welt_unten_ist_bild_unten"],
                d["stimmt_mit_auftrag"], d["auf_18er_raster"]))
            print("        Fluchtpunkt X (%.1f, %.1f)  Y (%.1f, %.1f)   Port-Matrix gegen Gleitkomma: %.2f / %.2f px" % (
                d["fluchtpunkt_x"][0], d["fluchtpunkt_x"][1], d["fluchtpunkt_y"][0], d["fluchtpunkt_y"][1],
                d["port_gegen_gleitkomma_px"][0], d["port_gegen_gleitkomma_px"][1]))
        nicht = [c for c in range(ncut) if not all(aus["raster18"][str(c)])]
        print(" 18er-Raster (alle sechs Koordinaten durch 18 teilbar): nicht erfuellt von Cut %s" % nicht)
    return aus


# =============================================================================
# 2. Probe: SCA-Zellen und Tuer-Rechtecke ins Bild
# =============================================================================
def _linie3(d, k, a, b, F, farbe, x0=0, y0=0, n=80, breite=1):
    a = np.asarray(a, float)
    b = np.asarray(b, float)
    prev = None
    for t in np.linspace(0, 1, n):
        sx, sy, vz = k.bild(a + (b - a) * t)
        if vz > 50 and -400 < sx < 800 and -400 < sy < 700:
            cur = ((sx - x0) * F, (sy - y0) * F)
            if prev is not None:
                d.line([prev, cur], fill=farbe, width=breite)
            prev = cur
        else:
            prev = None


def probe(sz, druck=True):
    kopf, zellen = tk.sca_eindeutig(sz.rdt)
    aus = dict(sca_off=kopf["off"], sca_zaehler=list(kopf["counts"]), zellen_band4=[], bilder=[])
    for c in zellen:
        if c["band"] == 4:
            aus["zellen_band4"].append(dict(i=c["i"], off=c["off"], roh=c["roh"], x=c["x"], z=c["z"],
                                            w=c["w"], d=c["d"], typ=c["typ"], floor=c["floor"]))
    F = 4
    for cut in (0, 11, 12):
        k = sz.k[cut]
        im = Image.fromarray(sz.bg[cut]).resize((320 * F, 240 * F), Image.NEAREST)
        d = ImageDraw.Draw(im)
        for c in zellen:
            if c["band"] != 4:
                continue
            x, z, w, dp = c["x"], c["z"], c["w"], c["d"]
            if c["typ"] == 1:
                e = [(x, BODEN_Y, z), (x + w, BODEN_Y, z), (x + w, BODEN_Y, z + dp), (x, BODEN_Y, z + dp)]
                for i in range(4):
                    _linie3(d, k, e[i], e[(i + 1) % 4], F, (0, 255, 0))
            else:
                cx, cz, r = x + w / 2.0, z + dp / 2.0, w / 2.0
                al = np.linspace(0, 2 * np.pi, 33)
                for i in range(32):
                    _linie3(d, k, (cx + r * np.cos(al[i]), BODEN_Y, cz + r * np.sin(al[i])),
                            (cx + r * np.cos(al[i + 1]), BODEN_Y, cz + r * np.sin(al[i + 1])), F,
                            (0, 255, 255), n=4)
        for n, t in TUER.items():
            x, z, w, dp = t["rechteck"]
            e = [(x, BODEN_Y, z), (x + w, BODEN_Y, z), (x + w, BODEN_Y, z + dp), (x, BODEN_Y, z + dp)]
            for i in range(4):
                _linie3(d, k, e[i], e[(i + 1) % 4], F, (255, 0, 255), breite=2)
        ax, ay, ab, ah = AUSSCHNITT[cut]
        d.rectangle([ax * F, ay * F, (ax + ab) * F, (ay + ah) * F], outline=(255, 255, 255))
        pfad = os.path.join(AUS, "vermessung_probe_cut%02d.png" % cut)
        im.save(pfad)
        aus["bilder"].append(os.path.relpath(pfad, WURZEL).replace("\\", "/"))
    if druck:
        print("== 2. Probe: %d SCA-Zellen auf Band 4, Tuer-Rechtecke; Bilder %s" % (
            len(aus["zellen_band4"]), ", ".join(aus["bilder"])))
    return aus


# =============================================================================
# 3. Versatz Bereich A -> Bereich B
# =============================================================================
def _fugen_x(sz, cut, xs, z0, z1, nz=60):
    k, Lu = sz.k[cut], sz.L[cut]
    zs = np.linspace(z0, z1, nz)
    out = np.full(len(xs), np.nan)
    for i, x in enumerate(xs):
        w = np.stack([np.full(nz, x), np.full(nz, BODEN_Y), zs], 1)
        sx, sy, vz = k.bild(w)
        ok = (vz > 0) & (sx > 1) & (sx < 318) & (sy > 1) & (sy < 238)
        if ok.sum() > nz * 0.8:
            out[i] = tn.abtasten(Lu, sx[ok], sy[ok]).mean()
    ok = ~np.isnan(out)
    q = out.copy()
    q[~ok] = out[ok].mean() if ok.any() else 0.0
    d = ndimage.uniform_filter1d(q, 5) - ndimage.median_filter(q, 41)
    res = []
    for i in range(3, len(xs) - 3):
        if ok[i] and d[i] < -7 and d[i] == d[i - 3:i + 4].min():
            res.append((float(xs[i]), float(d[i])))
    return res


def versatz(sz, druck=True):
    """Bereich A und Bereich B zeigen DIESELBE Szene, verschoben um T. T.z aus dem gelben
    Randstreifen des Landeplatzes (auf den Boden gemalt, laeuft laengs x, in Cut 0 und
    Cut 11 sichtbar), T.x aus den Bodenfugen laengs z; T.y = 0 (beide Boeden y=-7200)."""
    aus = dict()
    # --- gelber Streifen: vier Kanten, Gelbheit, laengs Fluchtpunkt X
    st = {}
    for cut, name, a, b, hb in ((0, "streifen_torseitig", (100, 219), (300, 158.6), 4.0),
                                (0, "streifen_platzseitig", (140, 232.4), (300, 178.3), 4.0),
                                (11, "streifen_platzseitig", (140, 43.5), (240, 50.5), 2.0),
                                (11, "streifen_torseitig", (140, 48.5), (240, 57.0), 2.0)):
        e = kante(sz, cut, name, a, b, "x", hb=hb, feld="G")
        k = sz.k[cut]
        p = np.array(e["p"])
        d = np.array(e["d"])
        z1 = k.auf_ebene(*(p - 30 * d), NY, BODEN_Y)[0][2]
        z2 = k.auf_ebene(*(p + 30 * d), NY, BODEN_Y)[0][2]
        e["z_boden"] = [float(z1), float(z2)]
        st["cut%d_%s" % (cut, name)] = e
    aus["streifen"] = st
    zA_t = np.mean(st["cut0_streifen_torseitig"]["z_boden"])
    zA_p = np.mean(st["cut0_streifen_platzseitig"]["z_boden"])
    zB_t = np.mean(st["cut11_streifen_torseitig"]["z_boden"])
    zB_p = np.mean(st["cut11_streifen_platzseitig"]["z_boden"])
    aus["streifenbreite_A"] = float(zA_t - zA_p)
    aus["streifenbreite_B"] = float(zB_t - zB_p)
    aus["Tz_torseitige_kante"] = float(zB_t - zA_t)
    aus["Tz_platzseitige_kante"] = float(zB_p - zA_p)
    aus["Tz"] = float(0.5 * (aus["Tz_torseitige_kante"] + aus["Tz_platzseitige_kante"]))
    # --- SCA-Gegenprobe: Zelle 2 (A, z0=15046, Tiefe 2700) gegen Steg zwischen Zelle 27 und 28 (B)
    kopf, zellen = tk.sca_eindeutig(sz.rdt)
    z2 = [c for c in zellen if c["i"] == 2][0]
    z27 = [c for c in zellen if c["i"] == 27][0]
    z28 = [c for c in zellen if c["i"] == 28][0]
    aus["sca"] = dict(A_grenze_z=z2["z"], A_zelle2_tiefe=z2["d"], B_grenze_z=z27["z"] + z27["d"],
                      B_steg_bis=z28["z"], B_stegbreite=z28["z"] - (z27["z"] + z27["d"]),
                      zelle2=z2["roh"], zelle27=z27["roh"], zelle28=z28["roh"],
                      zelle2_off=z2["off"], zelle27_off=z27["off"], zelle28_off=z28["off"])
    # --- Bodenfugen laengs z (x = const)
    fa = _fugen_x(sz, 0, np.arange(-6000, 8000, 20.0), 9000, 13500)
    fb = _fugen_x(sz, 11, np.arange(-24000, -4000, 20.0), -33500, -30000)
    aus["fugen_A_x"] = fa
    aus["fugen_B_x"] = fb
    if druck:
        print("== 3. Versatz A -> B")
        for n, e in st.items():
            print("  %-28s p=(%.2f,%.2f) Streuung %.2f px -> z am Boden %.0f / %.0f" % (
                n, e["p"][0], e["p"][1], e["streu"], e["z_boden"][0], e["z_boden"][1]))
        print("  Streifenbreite A %.0f, B %.0f   T.z = %.0f (torseitige Kante %.0f, platzseitige %.0f)" % (
            aus["streifenbreite_A"], aus["streifenbreite_B"], aus["Tz"], aus["Tz_torseitige_kante"],
            aus["Tz_platzseitige_kante"]))
        print("  SCA: A-Grenze z=%d (Zelle 2 @0x%X), B-Grenze z=%d (Zelle 27 @0x%X) -> in A-Koordinaten %.0f; Luecke %.0f" % (
            aus["sca"]["A_grenze_z"], z2["off"], aus["sca"]["B_grenze_z"], z27["off"],
            aus["sca"]["B_grenze_z"] - aus["Tz"], aus["sca"]["B_grenze_z"] - aus["Tz"] - aus["sca"]["A_grenze_z"]))
        print("  Fugen x (A):", [int(v[0]) for v in fa])
        print("  Fugen x (B):", [int(v[0]) for v in fb])
    return aus


# =============================================================================
# 4. Torebene
# =============================================================================
# Waagrechte Linien, die in Cut 0 UND in Cut 11 zu sehen sind: (Name, Cut-0-Start, Cut-11-Start)
# Start = zwei Bildpunkte auf der Linie, Halbbreite des Querprofils, Polaritaet.
STEREO_LINIEN = [
    # Gelaender neben dem Tor, Abschnitt mit kleinem x (Cut 0 links vom Tor, Cut 11 rechts)
    ("holm_oben_lo_nah", ((80, 85.5), (124, 81.8), 2.5, +1), ((135, 62), (200, 68), 2.5, +1)),
    ("holm_oben_lo_fern", ((20, 89.5), (75, 85.8), 2.5, +1), ((210, 67), (310, 77), 2.5, +1)),
    ("holm_mitte_lo", ((30, 100.5), (120, 93.8), 2.5, +1), ((170, 85), (300, 98.5), 3.0, +1)),
    # Tor
    ("tor_rohr_oben", ((138, 85.2), (158, 83.5), 2.5, +1), ((84, 65.2), (112, 67.6), 2.5, -1)),
    ("tor_rohr_unten", ((138, 105.6), (158, 104.0), 2.0, +1), ((88, 90.6), (110, 92.8), 2.0, +1)),
]


def stereo_linien(sz, Tz):
    """Jede Linie legt in jedem Cut eine Ebene durch den Kameraort fest; der Schnitt der
    beiden Ebenen (nach Verschiebung von A nach B) ist die Linie im Raum: Hoehe UND z."""
    k0, k11 = sz.k[0], sz.k[11]
    T = np.array([0.0, 0.0, Tz])
    aus = []
    for name, s0, s11 in STEREO_LINIEN:
        e0 = linie(sz, 0, name, s0[0], s0[1], "x", hb=s0[2], pol=s0[3])
        e11 = linie(sz, 11, name, s11[0], s11[1], "x", hb=s11[2], pol=s11[3])
        if e0 is None or e11 is None:
            aus.append(dict(name=name, fehler="nicht messbar"))
            continue
        n0, c0 = ebene_aus_bildgerade(k0, e0["p"], e0["d"])
        n1, c1 = ebene_aus_bildgerade(k11, e11["p"], e11["d"])
        c0b = c0 + n0.dot(T)
        A = np.array([[n0[1], n0[2]], [n1[1], n1[2]]])
        x = -11400.0
        y, z = np.linalg.solve(A, np.array([c0b - n0[0] * x, c1 - n1[0] * x]))
        # Empfindlichkeit: 0,3 px Versatz in je einem Cut
        emp = []
        for wer in (0, 1):
            ee0, ee11 = dict(e0), dict(e11)
            ziel = ee0 if wer == 0 else ee11
            nn = np.array([-ziel["d"][1], ziel["d"][0]])
            ziel["p"] = list(np.array(ziel["p"]) + 0.3 * nn)
            m0, d0 = ebene_aus_bildgerade(k0, ee0["p"], ee0["d"])
            m1, d1 = ebene_aus_bildgerade(k11, ee11["p"], ee11["d"])
            AA = np.array([[m0[1], m0[2]], [m1[1], m1[2]]])
            yy, zz = np.linalg.solve(AA, np.array([d0 + m0.dot(T) - m0[0] * x, d1 - m1[0] * x]))
            emp.append([float(yy - y), float(zz - z)])
        aus.append(dict(name=name, cut0=e0, cut11=e11, y=float(y), zB=float(z), zA=float(z - Tz),
                        hoehe=float(BODEN_Y - y), je_0p3px_cut0=emp[0], je_0p3px_cut11=emp[1]))
    return aus


def pfostenfuesse_cut0(sz):
    """Die Endpfosten stehen in Cut 0 hell vor dunklerem Boden. Laengs der Pfostenachse
    (Richtung Fluchtpunkt Y) wird der 50-%-Abfall gesucht und auf den Boden y=-7200 gelegt."""
    cut = 0
    k, L, fp = sz.k[cut], sz.L[cut], sz.fpy[cut]
    xs, p = querprofil(sz, cut, (148, 94.0), 12, "y", 19, 0.25, "L")
    aus = []
    for name, (a, b) in (("pfosten_lo", (131.0, 134.0)), ("pfosten_hi", (163.0, 165.5))):
        x0 = _band(xs, p, a, b, +1)["mitte"]
        p0 = np.array([x0, 94.0])
        d = (fp - p0) / np.linalg.norm(fp - p0)
        ts = np.arange(0.0, 22.0, 0.25)
        prof = np.array([np.mean([tn.abtasten(L, np.array([p0[0] + t * d[0] + q]), np.array([p0[1] + t * d[1]]))[0]
                                  for q in (-0.5, 0.0, 0.5)]) for t in ts])
        ys = p0[1] + ts * d[1]
        hoch = float(np.median(prof[(ys > 95) & (ys < 103)]))
        # Ende: letzter Punkt oberhalb des Halbwerts, danach mindestens 1,5 px darunter
        i_min = int(np.argmin(prof[ys > 104])) + int(np.sum(ys <= 104))
        tief = float(prof[i_min])
        h = 0.5 * (hoch + tief)
        i = i_min
        while i > 0 and prof[i] < h:
            i -= 1
        t = ts[i] + (prof[i] - h) / (prof[i] - prof[i + 1]) * (ts[i + 1] - ts[i])
        pf = p0 + t * d
        w, _ = k.auf_ebene(pf[0], pf[1], NY, BODEN_Y)
        aus.append(dict(name=name, x_bei_94=float(x0), x_fuss=float(pf[0]), y_fuss=float(pf[1]),
                        niveau=[hoch, tief], zA_boden=float(w[2]), xA_boden=float(w[0])))
    return aus


def bordstein_cut0(sz):
    """Helles Band hinter dem Gelaender (Cut 0), links und rechts vom Tor: Oberkante und
    Unterkante; die Unterkante wird als Fusslinie auf den Boden gelegt."""
    cut = 0
    k = sz.k[cut]
    aus = []
    # je Stelle: Fenster der Oberkante (dunkel -> hell) und der Unterkante (hell -> Boden)
    for x, fo, fu in ((122, (102.5, 107.0), (108.0, 114.5)), (175, (94.0, 100.0), (102.5, 106.0))):
        ys, q = querprofil(sz, cut, (x, 0.5 * (fo[0] + fu[1])), 10, "x", 0.5 * (fu[1] - fo[0]) + 1.0, 0.25, "L")
        yo = _stufe(ys, q, fo[0], fo[1])
        yu = _stufe(ys, q, fu[0], fu[1])
        fuss, _ = k.auf_ebene(x, yu, NY, BODEN_Y)
        oben, _ = k.auf_ebene(x, yo, NZ, fuss[2])
        aus.append(dict(x=x, y_oben=float(yo), y_unten=float(yu), zA_fuss=float(fuss[2]),
                        xA=float(fuss[0]), hoehe=float(BODEN_Y - oben[1])))
    return aus


def torebene(sz, vz, druck=True):
    Tz = vz["Tz"]
    k0, k11 = sz.k[0], sz.k[11]
    aus = dict(Tz=Tz)
    sl = stereo_linien(sz, Tz)
    aus["stereo"] = sl
    zs = np.array([s["zA"] for s in sl if "zA" in s])
    aus["zA_stereo_einzeln"] = [float(v) for v in zs]
    aus["zA_stereo_mittel"] = float(zs.mean())
    aus["zA_stereo_median"] = float(np.median(zs))
    aus["zA_stereo_streuung"] = float(zs.std(ddof=1))
    # --- Bodenannahme: Unterkante des Tors auf y=-7200 ?
    e0 = [s for s in sl if s["name"] == "tor_rohr_unten"][0]
    p0, p11 = e0["cut0"]["p"], e0["cut11"]["p"]

    def h0(zA):
        return BODEN_Y - auf_z(k0, p0[0], p0[1], zA)[0][1]

    def h11(zA):
        return BODEN_Y - auf_z(k11, p11[0], p11[1], zA + Tz)[0][1]
    aus["bodenannahme"] = dict(
        zA_wenn_cut0_unterrohr_auf_boden=float(optimize.brentq(h0, 14000, 17500)),
        zA_wenn_cut11_unterlinie_auf_boden=float(optimize.brentq(h11, 13000, 16500)),
        hoehe_unterrohr_stereo=e0["hoehe"], zA_unterrohr_stereo=e0["zA"])
    # --- Pfostenfuesse in Cut 0: Ende des hellen Pfostens laengs seiner Achse, auf den Boden gelegt
    aus["pfostenfuss_cut0"] = pfostenfuesse_cut0(sz)
    zf = np.array([f["zA_boden"] for f in aus["pfostenfuss_cut0"]])
    # --- Bordstein hinter dem Gelaender (weisses Band in Cut 0): Fusslinie auf dem Boden
    aus["bordstein_cut0"] = bordstein_cut0(sz)
    # --- SCA-Luecke
    a = vz["sca"]["A_grenze_z"]
    b = vz["sca"]["B_grenze_z"] - Tz
    aus["sca_luecke_A"] = [float(a), float(b)]
    aus["sca_luecke_mitte_A"] = float(0.5 * (a + b))
    # --- Ergebnis: Mittel aus Linien-Schnitten (Gelaender + Tor) und Pfostenfuessen
    alle = np.concatenate([zs, zf])
    aus["zA_alle_schaetzungen"] = [float(v) for v in alle]
    zA = float(round(alle.mean() / 10.0) * 10.0)
    aus["zA"] = zA
    aus["zB"] = zA + Tz
    aus["zA_streuung"] = float(alle.std(ddof=1))
    aus["unsicherheit"] = float(max(100.0, alle.std(ddof=1)))
    if druck:
        print("== 4. Torebene (z = const, das Gelaender laeuft laengs x)")
        for s in sl:
            if "zA" not in s:
                print("  %-20s nicht messbar" % s["name"])
                continue
            print("  %-20s Cut0 p=(%.2f,%.2f) str %.2f | Cut11 p=(%.2f,%.2f) str %.2f -> Hoehe %.0f, zA %.0f (zB %.0f); 0,3 px in Cut0: dz %+.0f, in Cut11: dz %+.0f" % (
                s["name"], s["cut0"]["p"][0], s["cut0"]["p"][1], s["cut0"]["streu"],
                s["cut11"]["p"][0], s["cut11"]["p"][1], s["cut11"]["streu"], s["hoehe"], s["zA"], s["zB"],
                s["je_0p3px_cut0"][1], s["je_0p3px_cut11"][1]))
        print("  Linien-Schnitte: Mittel %.0f, Median %.0f, Streuung %.0f;  mit Pfostenfuessen: Mittel %.0f, Streuung %.0f" % (
            aus["zA_stereo_mittel"], aus["zA_stereo_median"], aus["zA_stereo_streuung"], float(alle.mean()), aus["zA_streuung"]))
        bo = aus["bodenannahme"]
        print("  Bodenannahme (Unterkante auf y=-7200): Cut 0 verlangt zA=%.0f, Cut 11 verlangt zA=%.0f -> Widerspruch %.0f; Schnitt beider Sehebenen: Hoehe %.0f bei zA %.0f" % (
            bo["zA_wenn_cut0_unterrohr_auf_boden"], bo["zA_wenn_cut11_unterlinie_auf_boden"],
            bo["zA_wenn_cut0_unterrohr_auf_boden"] - bo["zA_wenn_cut11_unterlinie_auf_boden"],
            bo["hoehe_unterrohr_stereo"], bo["zA_unterrohr_stereo"]))
        for f in aus["pfostenfuss_cut0"]:
            print("  Pfostenfuss Cut 0 %-12s Achse bei (%.2f, 94) -> Ende des hellen Pfostens y=%.2f (Niveau %.0f -> %.0f) -> auf dem Boden zA %.0f, xA %.0f" % (
                f["name"], f["x_bei_94"], f["y_fuss"], f["niveau"][0], f["niveau"][1], f["zA_boden"], f["xA_boden"]))
        for f in aus["bordstein_cut0"]:
            print("  Bordstein Cut 0 bei x=%d: helles Band y %.2f .. %.2f -> Fusslinie auf dem Boden zA %.0f, Hoehe dort %.0f" % (
                f["x"], f["y_oben"], f["y_unten"], f["zA_fuss"], f["hoehe"]))
        print("  SCA-Luecke in A-Koordinaten: %.0f .. %.0f (Mitte %.0f)" % (a, b, 0.5 * (a + b)))
        print("  => Torebene zA = %.0f, zB = %.0f, Unsicherheit +-%.0f" % (aus["zA"], aus["zB"], aus["unsicherheit"]))
    return aus


# =============================================================================
# 5. Messung in Cut 0 und Cut 11 (RDT-Kamera, Rueckprojektion auf die Torebene)
# =============================================================================
def _gipfel(xs, p, x0, x1, pol=+1):
    m = (xs >= x0) & (xs <= x1)
    idx = np.where(m)[0]
    i = idx[np.argmax(pol * p[idx])]
    return i


def _band(xs, p, x0, x1, pol=+1, grund_l=None, grund_r=None):
    """Helles/dunkles Band im Profil: Gipfel in [x0,x1], Halbwertsdurchgaenge links/rechts.
    Grund = Minimum zwischen Gipfel und Fenster-Rand +-3 (je Seite getrennt)."""
    q = pol * p
    i = _gipfel(xs, p, x0, x1, pol)
    sch = xs[1] - xs[0]
    n3 = int(round(3.0 / sch))
    gl = q[max(0, i - n3):i + 1].min() if grund_l is None else pol * grund_l
    gr = q[i:i + n3 + 1].min() if grund_r is None else pol * grund_r
    l = halbwert(xs, q, i, -1, gl)
    r = halbwert(xs, q, i, +1, gr)
    return dict(gipfel=float(xs[i]), links=float(l), rechts=float(r), mitte=float(0.5 * (l + r)),
                breite=float(r - l), niveau=[float(pol * gl), float(p[i]), float(pol * gr)])


def _stufe(xs, p, x0, x1):
    """50-%-Durchgang einer Stufe zwischen den Plateaus an den Fensterraendern."""
    m = (xs >= x0) & (xs <= x1)
    xx, pp = xs[m], p[m]
    lo, hi = pp[:2].mean(), pp[-2:].mean()
    s = pp - 0.5 * (lo + hi)
    c = [xx[i] + s[i] / (s[i] - s[i + 1]) * (xx[i + 1] - xx[i]) for i in range(len(pp) - 1)
         if s[i] * s[i + 1] < 0 or s[i] == 0]
    if not c:
        return None
    c = np.array(c)
    return float(c[np.argmin(np.abs(c - 0.5 * (x0 + x1)))])


def messung_cut0(sz, zA, druck=True):
    """Alle Lagen als Bildkoordinate auf einer Bezugszeile/-spalte; Welt ueber die Torebene."""
    cut = 0
    k = sz.k[cut]
    M = dict()
    # senkrechte Glieder: Querprofil bei Bezugszeile y=94, gemittelt ueber 14 px laengs der Senkrechten
    xs, p = querprofil(sz, cut, (148, 94.0), 12, "y", 19, 0.25, "L")
    xg, g = querprofil(sz, cut, (148, 95.0), 6, "y", 19, 0.25, "G")
    M["bezugszeile"] = 94.0
    M["pfosten_lo"] = _band(xs, p, 131.0, 134.0, +1)
    M["rahmenrohr_hi"] = _band(xs, p, 159.5, 161.5, +1)
    M["pfosten_hi"] = _band(xs, p, 163.0, 165.5, +1)
    M["tafel_lo_kante"] = _stufe(xs, p, 138.8, 141.6)
    M["tafel_hi_kante"] = _stufe(xs, p, 156.4, 158.0)
    rand_lo = _band(xg, g, 138.0, 140.0, +1)
    M["rand_lo_gelb"] = rand_lo
    M["schild_lo_kante"] = rand_lo["links"]
    # hi-Rand: Gelb laeuft in das gelblich beleuchtete Rahmenrohr ueber -> nur der Anstieg
    M["schild_hi_gelbanstieg"] = _stufe(xg, g, 154.5, 158.6)
    # waagrechte Glieder: Querprofil bei Bezugsspalte x=147
    ys, q = querprofil(sz, cut, (147, 95.0), 10, "x", 13, 0.25, "L")
    yg, qg = querprofil(sz, cut, (147, 95.0), 10, "x", 13, 0.25, "G")
    M["bezugsspalte"] = 147.0
    M["rohr_oben"] = _band(ys, q, 83.5, 85.5, +1)
    M["rohr_unten"] = _band(ys, q, 104.0, 105.5, +1)
    M["rand_oben_gelb"] = _band(yg, qg, 88.0, 90.0, +1)
    M["rand_unten_gelb"] = _band(yg, qg, 99.5, 101.5, +1)
    M["schild_oben_kante"] = _stufe(ys, q, 87.0, 89.2)
    M["tafel_oben_kante"] = _stufe(ys, q, 90.0, 92.8)
    M["tafel_unten_kante"] = _stufe(ys, q, 98.0, 100.2)
    M["schild_unten_kante"] = M["rand_unten_gelb"]["rechts"]
    # Riegelkasten (weisser Fleck) bei (134..136.5, 91..93.5)
    xb, pb = querprofil(sz, cut, (135.3, 92.3), 2.0, "y", 4, 0.25, "L")
    yb, qb = querprofil(sz, cut, (135.3, 92.3), 2.0, "x", 4, 0.25, "L")
    M["fleck_lo"] = dict(x=_band(xb, pb, 134.5, 136.2, +1), y=_band(yb, qb, 91.5, 93.2, +1))
    W = _welt_cut(sz, cut, M, zA)
    if druck:
        _druck_cut("5a. Cut 0 (RDT-Kamera, Ebene zA=%.0f)" % zA, M, W)
    return dict(px=M, welt=W)


def messung_cut11(sz, zB, druck=True):
    cut = 11
    M = dict()
    xs, p = querprofil(sz, cut, (82, 80.0), 14, "y", 13, 0.25, "L")
    M["bezugszeile"] = 80.0
    M["pfosten_hi"] = _band(xs, p, 73.0, 75.0, -1)
    M["rahmenrohr_hi"] = _band(xs, p, 78.0, 79.8, -1)
    M["schild_hi_dunkel"] = _band(xs, p, 83.5, 85.2, -1)
    M["schild_hi_kante"] = M["schild_hi_dunkel"]["links"]
    # lo-Seite: Bezugszeile 87, danach laengs der Senkrechten auf Zeile 80 umgerechnet
    xl, pl = querprofil(sz, cut, (118, 87.0), 10, "y", 9.5, 0.25, "L")
    fp = sz.fpy[cut]

    def auf80(x):
        return x + (80.0 - 87.0) * (fp[0] - x) / (fp[1] - 87.0)
    for n, (a, b, pol) in dict(schild_lo_hell=(113.0, 115.0, +1), lo_dunkel=(120.0, 122.0, -1),
                               lo_hell=(123.5, 125.0, +1)).items():
        e = _band(xl, pl, a, b, pol)
        for kk in ("gipfel", "links", "rechts", "mitte"):
            e[kk + "_zeile87"] = e[kk]
            e[kk] = float(auf80(e[kk]))
        M[n] = e
    M["schild_lo_kante"] = M["schild_lo_hell"]["rechts"]
    ys, q = querprofil(sz, cut, (98, 78.0), 16, "x", 16, 0.25, "L")
    M["bezugsspalte"] = 98.0
    M["rohr_oben"] = _band(ys, q, 65.5, 67.5, -1)
    M["rohr_unten"] = _band(ys, q, 90.5, 92.0, +1)
    W = _welt_cut(sz, cut, M, zB)
    if druck:
        _druck_cut("5b. Cut 11 (RDT-Kamera, Ebene zB=%.0f)" % zB, M, W)
    return dict(px=M, welt=W)


def _welt_cut(sz, cut, M, z):
    """Bildlagen -> Torebene. Senkrechte Glieder: Welt-x (Bezugszeile), waagrechte: Hoehe
    ueber dem Boden (Bezugsspalte). u wird gegen die Achse des hi-Rahmenrohrs gezaehlt."""
    k = sz.k[cut]
    yz = M["bezugszeile"]
    xsp = M["bezugsspalte"]

    def wx(x):
        return float(auf_z(k, x, yz, z)[0][0])

    def hh(y):
        return float(BODEN_Y - auf_z(k, xsp, y, z)[0][1])
    x0 = wx(M["rahmenrohr_hi"]["mitte"])
    W = dict(z=z, x_rahmenrohr_hi=x0, u={}, v={}, breite={}, vz={})
    for n, e in M.items():
        if n in ("bezugszeile", "bezugsspalte", "fleck_lo"):
            continue
        waag = n.startswith("rohr_oben") or n.startswith("rohr_unten") or n.startswith("rand_oben") \
            or n.startswith("rand_unten") or n.endswith("_oben_kante") or n.endswith("_unten_kante")
        if isinstance(e, dict):
            if waag:
                W["v"][n] = hh(e["mitte"])
                W["v"][n + "_oberkante"] = hh(e["links"])
                W["v"][n + "_unterkante"] = hh(e["rechts"])
                W["breite"][n] = abs(hh(e["links"]) - hh(e["rechts"]))
            else:
                W["u"][n] = x0 - wx(e["mitte"])
                W["breite"][n] = abs(wx(e["links"]) - wx(e["rechts"]))
        elif e is not None:
            if waag:
                W["v"][n] = hh(e)
            else:
                W["u"][n] = x0 - wx(e)
    if "fleck_lo" in M:
        f = M["fleck_lo"]
        a = auf_z(k, f["x"]["links"], f["y"]["links"], z)[0]
        b = auf_z(k, f["x"]["rechts"], f["y"]["rechts"], z)[0]
        W["fleck_lo"] = dict(u=[float(x0 - a[0]), float(x0 - b[0])],
                             v=[float(BODEN_Y - a[1]), float(BODEN_Y - b[1])])
    # Massstab am Tor: Welteinheiten je Bildpunkt waagrecht / senkrecht, und vz
    m = (M["rahmenrohr_hi"]["mitte"] + (M.get("schild_lo_kante") or M["rahmenrohr_hi"]["mitte"])) / 2.0
    ymit = 0.5 * (M["rohr_oben"]["mitte"] + M["rohr_unten"]["mitte"])
    a, vz = auf_z(k, m, ymit, z)
    b, _ = auf_z(k, m + 1.0, ymit, z)
    c, _ = auf_z(k, m, ymit + 1.0, z)
    W["einheiten_je_px"] = dict(waagrecht=float(abs(b[0] - a[0])), senkrecht=float(abs(c[1] - a[1])),
                                bei=[float(m), float(ymit)], vz=float(vz), vz_durch_H=float(vz / k.H))
    return W


def _druck_cut(titel, M, W):
    print("== %s" % titel)
    e = W["einheiten_je_px"]
    print("  Massstab am Tor: %.1f Einheiten/px waagrecht, %.1f senkrecht, vz %.0f (vz/H %.1f)" % (
        e["waagrecht"], e["senkrecht"], e["vz"], e["vz_durch_H"]))
    for n in sorted(W["u"], key=lambda n: W["u"][n]):
        px = M[n]["mitte"] if isinstance(M[n], dict) else M[n]
        print("  u %-24s Bild-x %7.2f  u = %7.0f%s" % (
            n, px, W["u"][n], ("   Breite %.0f" % W["breite"][n]) if n in W["breite"] else ""))
    for n in sorted([n for n in W["v"] if not n.endswith("kante") or n.endswith("_kante")],
                    key=lambda n: -W["v"][n]):
        if n.endswith("_oberkante") or n.endswith("_unterkante"):
            continue
        px = M[n]["mitte"] if isinstance(M[n], dict) else M[n]
        print("  v %-24s Bild-y %7.2f  v = %7.0f%s" % (
            n, px, W["v"][n], ("   Dicke %.0f" % W["breite"][n]) if n in W["breite"] else ""))


# =============================================================================
# 6. Cut 12
# =============================================================================
# Frei gemessene Kanten in Cut 12: (Name, Achse, Laufbereich lo..hi, Suchfenster a0..a1, Feld, Mindeststufe)
C12_WAAG = [
    ("rohr_oben_aussen", 128, 195, 114, 121, "L", 15),
    ("rohr_oben_innen", 150, 182, 124, 130, "L", 15),
    ("tafel_unten", 144, 190, 171, 177, "L", 12),
    ("schild_unten", 135, 200, 177, 182, "L", 12),
    ("rohr_unten_hell_oben", 128, 205, 185, 189, "L", 15),
    ("rohr_unten_unten", 128, 210, 189, 194, "L", 15),
]
C12_SENK = [
    ("pfosten_hi_links", 112, 186, 94, 101, "L", 15),
    ("pfosten_hi_rechts", 112, 148, 103, 110, "L", 15),
    ("rahmenrohr_hi_links", 130, 148, 107, 112, "L", 8),
    ("rahmenrohr_hi_hellgrenze", 140, 174, 112, 118, "L", 15),
    ("rahmenrohr_hi_rechts", 138, 174, 118, 124, "L", 15),
    ("schild_hi", 140, 176, 129, 135, "L", 6),
    ("tafel_hi", 144, 171, 139, 145, "L", 15),
    ("tafel_lo", 144, 171, 190, 196, "L", 15),
    ("schild_lo", 142, 177, 200, 207, "L", 15),
    ("duennrohr_lo_links", 140, 168, 211, 215, "L", 8),
    ("duennrohr_lo_rechts", 140, 168, 216, 221, "L", 8),
]
REF12 = (164.5, 155.0)      # Bezugspunkt der Ebenen-Abbildung in Cut 12


class Ebene12(object):
    """Abbildung Bild <-> Torebene fuer Cut 12 aus den beiden im BILD gemessenen
    Fluchtpunkten (waagrecht vx, senkrecht vy) und einem Massstab s am Bezugspunkt.
    Laengs jeder Achse gilt die eindimensionale projektive Beziehung
        Welt = s * t / (1 - t / T),   t = Bildabstand vom Bezugspunkt, T = Abstand des Fluchtpunkts."""

    def __init__(self, vx, vy, s_h=1.0, s_v=1.0, ref=REF12):
        self.vx = np.asarray(vx, float)
        self.vy = np.asarray(vy, float)
        self.ref = np.asarray(ref, float)
        self.s_h, self.s_v = s_h, s_v
        self.eh = (self.vx - self.ref) / np.linalg.norm(self.vx - self.ref)
        ev = self.vy - self.ref
        self.Tv = -np.linalg.norm(ev)             # Fluchtpunkt liegt OBEN, t zaehlt nach unten
        self.ev = -ev / np.linalg.norm(ev)
        self.Th = np.linalg.norm(self.vx - self.ref)

    @staticmethod
    def _schnitt(p, q, a, b):
        """Schnitt der Geraden p-q mit a-b."""
        A = np.array([[q[0] - p[0], a[0] - b[0]], [q[1] - p[1], a[1] - b[1]]])
        t = np.linalg.solve(A, np.array([a[0] - p[0], a[1] - p[1]]))
        return p + t[0] * (q - p)

    def t(self, x, y):
        p = np.array([x, y], float)
        qh = self._schnitt(p, self.vy, self.ref, self.vx)
        qv = self._schnitt(p, self.vx, self.ref, self.vy)
        return float((qh - self.ref).dot(self.eh)), float((qv - self.ref).dot(self.ev))

    def roh(self, x, y):
        """Ebenen-Koordinaten in Bildpunkten am Bezugspunkt (s = 1): (nach lo, nach unten)."""
        th, tv = self.t(x, y)
        return th / (1.0 - th / self.Th), tv / (1.0 - tv / self.Tv)

    def welt(self, x, y):
        a, b = self.roh(x, y)
        return self.s_h * a, self.s_v * b

    def bild(self, a, b):
        """Umkehrung von roh()."""
        th = a / (1.0 + a / self.Th)
        tv = b / (1.0 + b / self.Tv)
        qh = self.ref + th * self.eh
        qv = self.ref + tv * self.ev
        return self._schnitt(qh, self.vy, qv, self.vx)


def _fluchtpunkt_aus_kanten(kanten, achse):
    """Ausgleichs-Schnittpunkt frei gemessener Geraden (gewichtet mit ihrer Laenge)."""
    A, b, g = [], [], []
    for e in kanten:
        if achse == "h":      # y = w + m (x - t)  ->  m x - y = m t - w
            A.append([e["m"], -1.0])
            b.append(e["m"] * e["t"] - e["w"])
        else:                 # x = w + m (y - t)  ->  x - m y = w - m t
            A.append([1.0, -e["m"]])
            b.append(e["w"] - e["m"] * e["t"])
        g.append(np.sqrt(e["n"]))
    A, b, g = np.array(A), np.array(b), np.array(g)
    lsg, _, _, _ = np.linalg.lstsq(A * g[:, None], b * g, rcond=None)
    return lsg


def messung_cut12(sz, c0, c11, eb, druck=True):
    cut = 12
    k = sz.k[cut]
    aus = dict()
    K = {}
    for n, lo, hi, a0, a1, feld, ms in C12_WAAG:
        K[n] = kante_frei(sz, cut, n, "h", lo, hi, a0, a1, feld, ms)
    for n, lo, hi, a0, a1, feld, ms in C12_SENK:
        K[n] = kante_frei(sz, cut, n, "v", lo, hi, a0, a1, feld, ms)
    # Kanten mit Textur (Schraffur, Schrift) werden laengs der Fluchtrichtung gemessen
    fest = {}
    for n, a, b, hb, feld in (("tafel_oben", (144, 140.6), (188, 142.4), 2.0, "L"),
                              ("schild_oben", (150, 135.0), (182, 136.6), 2.5, "G"),
                              ("rohr_unten_dunkel_oben", (150, 183.9), (184, 184.0), 1.5, "L")):
        fest[n] = kante(sz, cut, n, a, b, "x", hb=hb, feld=feld)
    aus["kanten_frei"] = K
    aus["kanten_laengs"] = fest
    # --- Fluchtpunkte aus dem Bild
    waag = [K[n] for n in ("rohr_oben_aussen", "rohr_oben_innen", "schild_unten",
                           "rohr_unten_hell_oben", "rohr_unten_unten") if K[n]]
    vx = _fluchtpunkt_aus_kanten(waag, "h")
    senk_tor = [K[n] for n in ("rahmenrohr_hi_rechts", "schild_hi", "tafel_hi", "tafel_lo", "schild_lo",
                               "duennrohr_lo_links", "duennrohr_lo_rechts") if K[n]]
    vy = _fluchtpunkt_aus_kanten(senk_tor, "v")
    aus["fluchtpunkt_x_bild"] = [float(v) for v in vx]
    aus["fluchtpunkt_y_bild"] = [float(v) for v in vy]
    aus["fluchtpunkt_x_rdt"] = [float(v) for v in sz.fpx[cut]]
    aus["fluchtpunkt_y_rdt"] = [float(v) for v in sz.fpy[cut]]
    aus["steigungen_senkrecht"] = []
    for n, *_ in C12_SENK:
        e = K[n]
        if not e:
            continue
        aus["steigungen_senkrecht"].append(dict(
            name=n, x=e["w"], y=e["t"], gemessen=e["m"], fehler=e["se"],
            rdt=float((e["w"] - sz.fpy[cut][0]) / (e["t"] - sz.fpy[cut][1])),
            bild=float((e["w"] - vy[0]) / (e["t"] - vy[1]))))
    aus["steigungen_waagrecht"] = []
    for n, *_ in C12_WAAG:
        e = K[n]
        if not e:
            continue
        aus["steigungen_waagrecht"].append(dict(
            name=n, x=e["t"], y=e["w"], gemessen=e["m"], fehler=e["se"],
            rdt=float((sz.fpx[cut][1] - e["w"]) / (sz.fpx[cut][0] - e["t"]))))
    # Brennweite, die zu den Bild-Fluchtpunkten gehoert (Bildmitte 160/120, quadratische Pixel)
    c = np.array([160.0, 120.0])
    f2 = -float((vx - c).dot(vy - c))
    aus["H_aus_fluchtpunkten"] = float(np.sqrt(f2)) if f2 > 0 else None
    # --- Abbildung
    E = Ebene12(vx, vy)
    xr = REF12[0]
    yr = REF12[1]

    def hy(n):       # Bild-y einer waagrechten Kante auf der Bezugssenkrechten
        e = K.get(n) or None
        if e:
            return wert_bei(e, xr)
        f = fest[n]
        return f["p"][1] + (xr - f["p"][0]) * f["d"][1] / f["d"][0]

    def vx_(n):      # Bild-x einer senkrechten Kante auf der Bezugswaagrechten
        return wert_bei(K[n], yr)
    Y = {n: hy(n) for n in ("rohr_oben_aussen", "rohr_oben_innen", "schild_oben", "tafel_oben",
                            "tafel_unten", "schild_unten", "rohr_unten_dunkel_oben",
                            "rohr_unten_hell_oben", "rohr_unten_unten")}
    X = {n: vx_(n) for n, *_ in C12_SENK if K[n]}
    aus["bild_y_bei_x164p5"] = Y
    aus["bild_x_bei_y155"] = X
    # rohe Ebenenkoordinaten (px am Bezugspunkt)
    ry = {n: E.roh(xr, y)[1] for n, y in Y.items()}
    rx = {n: E.roh(x, yr)[0] for n, x in X.items()}
    aus["roh_v"] = ry
    aus["roh_u"] = rx
    # --- Massstab aus Ankern: Hoehen aus Cut 0 und Cut 11 (RDT-Kameras, Torebene)
    rohr_oben_mitte = 0.5 * (ry["rohr_oben_aussen"] + ry["rohr_oben_innen"])
    rohr_unten_mitte = 0.5 * (ry["rohr_unten_dunkel_oben"] + ry["rohr_unten_unten"])
    anker = []
    v0, v11 = c0["welt"]["v"], c11["welt"]["v"]
    anker.append(dict(name="Rohr oben Mitte -> Rohr unten Mitte", quelle="Cut 0",
                      welt=v0["rohr_oben"] - v0["rohr_unten"], roh=rohr_unten_mitte - rohr_oben_mitte, art="v"))
    anker.append(dict(name="Rohr oben Mitte -> Schild Unterkante", quelle="Cut 0",
                      welt=v0["rohr_oben"] - v0["schild_unten_kante"], roh=ry["schild_unten"] - rohr_oben_mitte, art="v"))
    anker.append(dict(name="Schild Oberkante -> Schild Unterkante", quelle="Cut 0",
                      welt=v0["schild_oben_kante"] - v0["schild_unten_kante"],
                      roh=ry["schild_unten"] - ry["schild_oben"], art="v"))
    anker.append(dict(name="Rohr oben Mitte -> Rohr unten Mitte", quelle="Cut 11",
                      welt=v11["rohr_oben"] - v11["rohr_unten"], roh=rohr_unten_mitte - rohr_oben_mitte, art="v"))
    sl = {s["name"]: s for s in eb["stereo"] if "hoehe" in s}
    anker.append(dict(name="Rohr oben Mitte -> Rohr unten Mitte", quelle="Schnitt Cut 0 x Cut 11",
                      welt=sl["tor_rohr_oben"]["hoehe"] - sl["tor_rohr_unten"]["hoehe"],
                      roh=rohr_unten_mitte - rohr_oben_mitte, art="v"))
    u0, u11 = c0["welt"]["u"], c11["welt"]["u"]
    rohr_hi_mitte = 0.5 * (rx["rahmenrohr_hi_links"] + rx["rahmenrohr_hi_rechts"])
    anker.append(dict(name="Schildbreite (hi-Kante -> lo-Kante)", quelle="Cut 11",
                      welt=u11["schild_lo_kante"] - u11["schild_hi_kante"], roh=rx["schild_lo"] - rx["schild_hi"], art="u"))
    anker.append(dict(name="Rahmenrohr hi Mitte -> Schild lo-Kante", quelle="Cut 11",
                      welt=u11["schild_lo_kante"], roh=rx["schild_lo"] - rohr_hi_mitte, art="u"))
    anker.append(dict(name="Rahmenrohr hi Mitte -> Schild lo-Kante", quelle="Cut 0",
                      welt=u0["schild_lo_kante"], roh=rx["schild_lo"] - rohr_hi_mitte, art="u"))
    anker.append(dict(name="Rahmenrohr hi Mitte -> Tafel lo-Kante", quelle="Cut 0",
                      welt=u0["tafel_lo_kante"], roh=rx["tafel_lo"] - rohr_hi_mitte, art="u"))
    for a in anker:
        a["s"] = float(a["welt"] / a["roh"])
    aus["anker"] = anker
    sv = np.array([a["s"] for a in anker if a["art"] == "v"])
    sh = np.array([a["s"] for a in anker if a["art"] == "u"])
    aus["s_v_mittel"] = float(sv.mean())
    aus["s_v_streuung"] = float(sv.std(ddof=1))
    aus["s_h_mittel"] = float(sh.mean())
    aus["s_h_streuung"] = float(sh.std(ddof=1))
    s = float(np.concatenate([sv, sh]).mean())
    aus["s"] = s
    aus["s_streuung"] = float(np.concatenate([sv, sh]).std(ddof=1))
    # Was die RDT-Kamera von Cut 12 an derselben Stelle liefert
    w, vz = auf_z(k, xr, yr, eb["zB"])
    w1, _ = auf_z(k, xr + 1, yr, eb["zB"])
    w2, _ = auf_z(k, xr, yr + 1, eb["zB"])
    aus["rdt_kamera_am_bezugspunkt"] = dict(vz=float(vz), vz_durch_H=float(vz / k.H),
                                            einheiten_je_px_waagrecht=float(abs(w1[0] - w[0])),
                                            einheiten_je_px_senkrecht=float(abs(w2[1] - w[1])),
                                            verhaeltnis_zu_s=float(abs(w1[0] - w[0]) / s))
    # Hoehen, die die RDT-Kamera von Cut 12 auf der Torebene ergaebe
    aus["rdt_kamera_hoehen"] = {n: float(BODEN_Y - auf_z(k, xr, y, eb["zB"])[0][1]) for n, y in Y.items()}
    # --- Ebenenkoordinaten: u ab Achse hi-Rahmenrohr, v ueber Boden; Hoehenanker = Rohr oben Mitte
    v_anker = float(np.mean([v0["rohr_oben"], v11["rohr_oben"], sl["tor_rohr_oben"]["hoehe"]]))
    aus["v_anker_rohr_oben_mitte"] = dict(cut0=v0["rohr_oben"], cut11=v11["rohr_oben"],
                                          schnitt=sl["tor_rohr_oben"]["hoehe"], mittel=v_anker)
    E.s_h = E.s_v = s
    aus["ebene12"] = dict(ref=list(REF12), vx=[float(v) for v in vx], vy=[float(v) for v in vy], s=s,
                          roh_u0=rohr_hi_mitte, roh_v0=rohr_oben_mitte, v0=v_anker)

    def uv(x, y):
        a, b = E.roh(x, y)
        return float(s * (a - rohr_hi_mitte)), float(v_anker - s * (b - rohr_oben_mitte))
    aus["u"] = {n: float(s * (rx[n] - rohr_hi_mitte)) for n in rx}
    aus["v"] = {n: float(v_anker - s * (ry[n] - rohr_oben_mitte)) for n in ry}
    aus["_uv"] = uv
    aus["_E"] = E
    if druck:
        print("== 6. Cut 12")
        print("  Fluchtpunkt waagrecht: Bild (%.0f, %.1f), RDT-Kamera (%.0f, %.1f)" % (
            vx[0], vx[1], sz.fpx[cut][0], sz.fpx[cut][1]))
        print("  Fluchtpunkt senkrecht: Bild (%.0f, %.0f), RDT-Kamera (%.0f, %.0f)   -> H aus den Bild-Fluchtpunkten %s (RDT: %d)" % (
            vy[0], vy[1], sz.fpy[cut][0], sz.fpy[cut][1],
            ("%.0f" % aus["H_aus_fluchtpunkten"]) if aus["H_aus_fluchtpunkten"] else "-", k.H))
        for e in aus["steigungen_senkrecht"]:
            print("    senkrecht %-26s x=%.2f: dx/dy gemessen %+.4f +-%.4f | RDT-Kamera %+.4f" % (
                e["name"], e["x"], e["gemessen"], e["fehler"], e["rdt"]))
        for e in aus["steigungen_waagrecht"]:
            print("    waagrecht %-26s y=%.2f: dy/dx gemessen %+.4f +-%.4f | RDT-Kamera %+.4f" % (
                e["name"], e["y"], e["gemessen"], e["fehler"], e["rdt"]))
        for a in anker:
            print("  Anker %-42s (%-22s) Welt %7.0f / Bild %6.2f px -> s = %.2f" % (
                a["name"], a["quelle"], a["welt"], a["roh"], a["s"]))
        print("  s senkrecht %.2f +-%.2f, waagrecht %.2f +-%.2f  => s = %.2f +-%.2f Einheiten/px am Bezugspunkt" % (
            aus["s_v_mittel"], aus["s_v_streuung"], aus["s_h_mittel"], aus["s_h_streuung"], s, aus["s_streuung"]))
        r = aus["rdt_kamera_am_bezugspunkt"]
        print("  RDT-Kamera Cut 12 auf der Torebene: vz %.0f, %.2f / %.2f Einheiten/px -> %.2f-fach zu gross" % (
            r["vz"], r["einheiten_je_px_waagrecht"], r["einheiten_je_px_senkrecht"], r["verhaeltnis_zu_s"]))
    return aus


# =============================================================================
# 7. Einzelteile in Cut 12 (Ecke, Aufhaengung, Fuesse, Schraffur)
# =============================================================================
def _erste_abweichung(prof, himmel, schwelle):
    """Index (Subpixel) des ersten Punkts eines Profils, der um mehr als schwelle vom
    Himmel abweicht; das Profil beginnt im Himmel."""
    ab = np.abs(prof - himmel)
    idx = np.where(ab > schwelle)[0]
    if len(idx) == 0 or idx[0] == 0:
        return None
    i = idx[0]
    a, b = ab[i - 1], ab[i]
    return (i - 1) + (schwelle - a) / (b - a)


def _kontur_ecke(L, himmel, schwelle=9.0):
    """Aussen- und Innenkontur der Rahmenecke oben/hi in Cut 12, jeweils vom Himmel her
    angetastet (Pixelmitte = Index + 0,5).
    aussen: Zeilen 121..130 von links (Start x=108, im Himmelsspalt zwischen Pfosten und
            Rohr), Spalten 116..126 von oben (Start y=111).
    innen:  Zeilen 126..135 von rechts (Start x=131), Spalten 125..129 von unten (Start y=132)."""
    pa, pi = [], []
    for y in range(121, 131):
        t = _erste_abweichung(L[y, 108:125].astype(float), himmel, schwelle)
        if t is not None:
            pa.append((108 + t + 0.5, y + 0.5))
    for x in range(116, 127):
        t = _erste_abweichung(L[111:125, x].astype(float), himmel, schwelle)
        if t is not None:
            pa.append((x + 0.5, 111 + t + 0.5))
    for y in range(126, 136):
        t = _erste_abweichung(L[y, 131:118:-1].astype(float), himmel, schwelle)
        if t is not None:
            pi.append((131 - t + 0.5, y + 0.5))
    for x in range(125, 130):
        t = _erste_abweichung(L[132:120:-1, x].astype(float), himmel, schwelle)
        if t is not None:
            pi.append((x + 0.5, 132 - t + 0.5))
    return np.array(pa), np.array(pi)


def _kreis(P):
    """Freier Ausgleichskreis."""
    A = np.c_[2 * P[:, 0], 2 * P[:, 1], np.ones(len(P))]
    b = (P ** 2).sum(1)
    (a, bb, c), _, _, _ = np.linalg.lstsq(A, b, rcond=None)
    r = np.sqrt(c + a * a + bb * bb)
    res = np.sqrt((P[:, 0] - a) ** 2 + (P[:, 1] - bb) ** 2) - r
    return float(a), float(bb), float(r), float(res.std())


def _kreis_tangential(P, x_senk, y_waag):
    """Kreis, der die senkrechte Kante x = x_senk und die waagrechte Kante y = y_waag
    beruehrt (Mitte = (x_senk + r, y_waag + r)); r so, dass die Konturpunkte am besten
    auf dem Kreis liegen."""
    def rest(r):
        return np.sqrt((P[:, 0] - (x_senk + r)) ** 2 + (P[:, 1] - (y_waag + r)) ** 2) - r
    rs = np.arange(3.0, 30.0, 0.05)
    q = np.array([np.sqrt((rest(r) ** 2).mean()) for r in rs])
    i = int(np.argmin(q))
    return float(rs[i]), float(q[i])


def einzelteile_cut12(sz, c12, druck=True):
    cut = 12
    L = sz.L[cut]
    uv = c12["_uv"]
    s = c12["s"]
    aus = dict()
    # --- Ecke oben/hi: Aussen- und Innenkontur gegen den Himmel (Helligkeit 18..20)
    himmel = float(np.median(L[105:113, 125:200]))
    aus["himmel_helligkeit"] = himmel
    pa, pi = _kontur_ecke(L, himmel)
    K = c12["kanten_frei"]
    xa = wert_bei(K["rahmenrohr_hi_links"], 135.0)
    xi = wert_bei(K["rahmenrohr_hi_rechts"], 137.0)
    ya = wert_bei(K["rohr_oben_aussen"], 127.0)
    yi = wert_bei(K["rohr_oben_innen"], 130.0)
    ra, qa = _kreis_tangential(pa, xa, ya)
    ri, qi = _kreis_tangential(pi, xi, yi)
    fa = _kreis(pa)
    fi = _kreis(pi)
    dicke_gerade = xi - xa
    aus["ecke_hi_oben"] = dict(
        aussen=dict(mitte=[xa + ra, ya + ra], radius_px=ra, rms=qa, n=len(pa), tangenten=[xa, ya],
                    frei=dict(mitte=[fa[0], fa[1]], radius_px=fa[2], rms=fa[3]),
                    punkte=[[float(a), float(b)] for a, b in pa]),
        innen=dict(mitte=[xi + ri, yi + ri], radius_px=ri, rms=qi, n=len(pi), tangenten=[xi, yi],
                   frei=dict(mitte=[fi[0], fi[1]], radius_px=fi[2], rms=fi[3]),
                   punkte=[[float(a), float(b)] for a, b in pi]),
        rohrdicke_gerade_px=dicke_gerade,
        radius_aussen=ra * s, radius_innen=ri * s,
        radius_mittellinie=(ra - 0.5 * dicke_gerade) * s)
    # --- Aufhaengung: dunkle Laschen zwischen Rohr oben und Schild, Zeilen 129..133
    zeile = L[129:134, :].mean(0)
    xs = np.arange(320) + 0.5
    auf = {}
    for n, (a, b) in dict(lasche_hi=(136, 149), lasche_lo=(187, 198)).items():
        e = _band(xs, zeile, a + 2, b - 2, -1, grund_l=himmel, grund_r=himmel)
        auf[n] = e
    aus["aufhaengung"] = auf
    # --- Fuesse: Zeilen 181..183 (zwischen Schild-Unterkante und Rohr unten)
    zeile = L[181:184, :].mean(0)
    fu = {}
    for n, (a, b) in dict(fuss_hi=(137, 150), fuss_lo=(190, 200), block_lo=(203, 214)).items():
        seg = zeile[a:b + 1]
        idx = np.where(seg > 10)[0]
        fu[n] = dict(links=float(a + idx[0]), rechts=float(a + idx[-1] + 1), helligkeit_mittel=float(seg[idx].mean()),
                     helligkeit_max=float(seg.max()), x_max=float(a + int(np.argmax(seg)) + 0.5))
    aus["fuesse"] = fu
    # --- lo-Seite: heller Streifen (Innenflanke), schwarzes Glied dahinter, Kasten darueber
    aus["lo_seite"] = lo_seite_cut12(sz, c12, himmel)
    # --- Schraffur: Periode und Winkel in den vier Randstreifen
    aus["schraffur"] = schraffur(sz, c12)
    if druck:
        e = aus["ecke_hi_oben"]
        print("== 7. Einzelteile Cut 12   (Himmel-Helligkeit %.1f)" % himmel)
        print("  Ecke oben/hi, Kreis an die geraden Kanten angelegt: aussen r %.2f px (rms %.2f, n %d, Mitte %.1f/%.1f); innen r %.2f px (rms %.2f, n %d, Mitte %.1f/%.1f)" % (
            e["aussen"]["radius_px"], e["aussen"]["rms"], e["aussen"]["n"], e["aussen"]["mitte"][0], e["aussen"]["mitte"][1],
            e["innen"]["radius_px"], e["innen"]["rms"], e["innen"]["n"], e["innen"]["mitte"][0], e["innen"]["mitte"][1]))
        print("     freier Kreis: aussen r %.2f (rms %.2f), innen r %.2f (rms %.2f); Rohrdicke am geraden Stueck %.2f px" % (
            e["aussen"]["frei"]["radius_px"], e["aussen"]["frei"]["rms"], e["innen"]["frei"]["radius_px"],
            e["innen"]["frei"]["rms"], e["rohrdicke_gerade_px"]))
        print("     -> Radius aussen %.0f, innen %.0f, Mittellinie (aussen - halbe Rohrdicke) %.0f" % (
            e["radius_aussen"], e["radius_innen"], e["radius_mittellinie"]))
        for n, a in auf.items():
            print("  %-10s x %.2f .. %.2f  (Breite %.2f px = %.0f)" % (n, a["links"], a["rechts"], a["breite"], a["breite"] * s))
        for n, a in fu.items():
            print("  %-10s x %.1f .. %.1f  Helligkeit %.0f (max %.0f bei x=%.1f)" % (
                n, a["links"], a["rechts"], a["helligkeit_mittel"], a["helligkeit_max"], a["x_max"]))
        lo = aus["lo_seite"]
        print("  lo-Seite: heller Streifen u %.0f..%.0f; schwarzes Glied bis x=%.2f (u %.0f); Kasten px %s -> u %.0f..%.0f, v %.0f..%.0f" % (
            c12["u"]["duennrohr_lo_links"], c12["u"]["duennrohr_lo_rechts"], lo["schwarzes_glied_px"][1], lo["schwarzes_glied_u"][1],
            lo["kasten_px"], lo["kasten_u"][0], lo["kasten_u"][1], lo["kasten_v"][0], lo["kasten_v"][1]))
        print("     Lichtkante des oberen Rohrs endet bei x=%.0f (u %.0f); helles Band des unteren Rohrs endet bei %s" % (
            lo["rohr_oben_lichtkante_ende_px"], lo["rohr_oben_lichtkante_ende_u"],
            ", ".join("y%d: %.1f" % (a, b) for a, b in lo["rohr_unten_hell_ende_px"])))
        if "spalt_lo_schwarz_oberkante_px" in lo:
            print("     Spalt Schild/lo-Rohr: Himmel nur bis y=%.2f (v %.0f), darunter schwarz; im hi-Spalt reicht der Himmel bis y=%.0f (v %.0f)" % (
                lo["spalt_lo_schwarz_oberkante_px"][1], lo["spalt_lo_schwarz_oberkante_v"],
                lo["spalt_hi_himmel_bis_px"], lo["spalt_hi_himmel_bis_v"]))
        print("     Symmetrie: Schildmitte u %.0f; hi-Rohr gespiegelt u %.0f..%.0f; Innenkante lo gemessen %.0f, gespiegelt %.0f; Spalt Schild-Rohr hi %.0f, lo %.0f" % (
            lo["schildmitte_u"], lo["spiegel_hi_rohr"][0], lo["spiegel_hi_rohr"][1], lo["innenkante_lo_gemessen"],
            lo["innenkante_lo_gespiegelt"], lo["spalt_schild_rohr_hi"], lo["spalt_schild_rohr_lo"]))
        sc = aus["schraffur"]
        for n in ("rand_hi", "rand_lo", "rand_oben", "rand_unten"):
            r = sc[n]
            print("  Schraffur %-10s Periode laengs des Rands %.2f px = %.0f, Versatz quer %.2f px je px -> Winkel %.1f Grad (Kontrast %.0f)" % (
                n, r["periode_px"], r["periode"], r["versatz_je_px"], r["winkel_grad"], r["kontrast"]))
    return aus


def lo_seite_cut12(sz, c12, himmel):
    """Die lo-Seite des Rahmens liegt im Schatten des Pfostens. Messbar sind:
      - der 3,4 px breite, vom Himmel beleuchtete Streifen (Kanten aus C12_SENK),
      - das schwarze Glied rechts davon bis zum naechsten Himmel (Zeilen 137..140),
      - der Kasten am Pfosten (schwarz vor Himmel), seine helle linke Kante,
      - das Ende der beleuchteten Oberkante des oberen Rohrs und des hellen Bands des unteren.
    Dazu die Spiegelung des hi-Rohrs an der Schildmitte (Symmetrie-Annahme)."""
    L = sz.L[12]
    u = c12["u"]
    uv = c12["_uv"]
    aus = dict()
    z = L[137:141, :].mean(0)
    # schwarzes Glied: von x=217 nach rechts bis wieder Himmel
    i = 218
    while i < 240 and z[i] < 0.5 * himmel:
        i += 1
    rechts = i - 1 + (0.5 * himmel - z[i - 1]) / (z[i] - z[i - 1]) + 0.5
    aus["schwarzes_glied_px"] = [float(c12["bild_x_bei_y155"]["duennrohr_lo_rechts"]), float(rechts)]
    aus["schwarzes_glied_zeilen"] = [137, 140]
    aus["schwarzes_glied_u"] = [u["duennrohr_lo_rechts"], uv(rechts, 138.5)[0]]
    # Kasten: schwarz vor Himmel, Zeilen 114..135; linke helle Kante, rechte Kante gegen Himmel
    k = L[114:136, :].mean(0)
    j = 205
    while j < 225 and abs(k[j] - himmel) < 5:
        j += 1
    i = 232
    while i > 215 and abs(k[i] - himmel) < 5:
        i -= 1
    sp = L[:, 218:228].mean(1)
    o = 105
    while o < 125 and sp[o] > 8:
        o += 1
    # Unterkante: dort, wo links unter dem Kasten (Spalten 213..216) wieder Himmel erscheint
    sp2 = L[:, 213:217].mean(1)
    un = 128
    while un < 145 and sp2[un + 1] < 8:
        un += 1
    aus["kasten_px"] = [float(j), float(o), float(i + 1), float(un + 1)]
    a = uv(j, 125.0)
    b = uv(i + 1, 125.0)
    aus["kasten_u"] = [a[0], b[0]]
    aus["kasten_v"] = [uv(220.0, un + 1)[1], uv(220.0, o)[1]]
    # Ende der beleuchteten Oberkante des oberen Rohrs (Zeilen 121..123) und des hellen Bands unten
    ob = L[121:124, :].max(0)
    i = 200
    while i < 230 and ob[i] > 30:
        i += 1
    aus["rohr_oben_lichtkante_ende_px"] = float(i)
    aus["rohr_oben_lichtkante_ende_u"] = uv(i, 122.5)[0]
    enden = []
    for y in range(187, 192):
        zl = L[y, :]
        i = 205
        ref = float(np.median(zl[195:205]))
        while i < 230 and zl[i] > 0.5 * ref:
            i += 1
        enden.append([y, float(i - 1 + (zl[i - 1] - 0.5 * ref) / max(1e-6, (zl[i - 1] - zl[i])) + 0.5)])
    aus["rohr_unten_hell_ende_px"] = enden
    # Spalt zwischen Schild und lo-Rohr: oben Himmel, darunter schwarz. Oberkante des Schwarzen:
    e = kante_frei(sz, 12, "spalt_lo_oberkante", "h", 204, 212, 138, 146, "L", 8)
    if e:
        aus["spalt_lo_schwarz_oberkante_px"] = [e["t"], e["w"], e["m"]]
        aus["spalt_lo_schwarz_oberkante_v"] = uv(e["t"], e["w"])[1]
    # Gegenstueck hi-Spalt: wie weit reicht dort der Himmel nach unten? (Spalten 122..130)
    sp = L[:, 122:131].mean(1)
    y = 140
    while y < 190 and abs(sp[y] - himmel) < 5:
        y += 1
    aus["spalt_hi_himmel_bis_px"] = float(y)
    aus["spalt_hi_himmel_bis_v"] = uv(126.0, float(y))[1]
    # Symmetrie
    mitte = 0.5 * (u["schild_hi"] + u["schild_lo"])
    aus["schildmitte_u"] = mitte
    aus["spiegel_hi_rohr"] = [2 * mitte - u["rahmenrohr_hi_rechts"], 2 * mitte - u["rahmenrohr_hi_links"]]
    aus["innenkante_lo_gemessen"] = u["duennrohr_lo_links"]
    aus["innenkante_lo_gespiegelt"] = 2 * mitte - u["rahmenrohr_hi_rechts"]
    aus["spalt_schild_rohr_hi"] = u["schild_hi"] - u["rahmenrohr_hi_rechts"]
    aus["spalt_schild_rohr_lo"] = u["duennrohr_lo_links"] - u["schild_lo"]
    return aus


def _periode(sig):
    """Grundperiode eines Streifensignals ueber die Autokorrelation (Subpixel per Parabel)."""
    x = sig - sig.mean()
    n = len(x)
    ac = np.array([np.dot(x[:n - k], x[k:]) / (n - k) for k in range(0, n // 2 + 2)])
    ac /= ac[0] if ac[0] != 0 else 1.0
    # erstes Maximum nach dem ersten Minimum
    i = 1
    while i < len(ac) - 1 and ac[i] > ac[i + 1]:
        i += 1
    j = i
    while j < len(ac) - 1 and not (ac[j] >= ac[j - 1] and ac[j] >= ac[j + 1]):
        j += 1
    if j >= len(ac) - 1:
        return None, None
    a, b, c = ac[j - 1], ac[j], ac[j + 1]
    den = a - 2 * b + c
    off = 0.5 * (a - c) / den if den != 0 else 0.0
    return float(j + off), float(b)


def _versatz(s1, s2, maxv):
    """Verschiebung von s2 gegen s1 (px), Maximum der Kreuzkorrelation, Subpixel."""
    a = s1 - s1.mean()
    b = s2 - s2.mean()
    ks = np.arange(-maxv, maxv + 1)
    cc = []
    for k in ks:
        if k >= 0:
            cc.append(np.dot(a[:len(a) - k], b[k:]) / (len(a) - k))
        else:
            cc.append(np.dot(a[-k:], b[:len(b) + k]) / (len(a) + k))
    cc = np.array(cc)
    j = int(np.argmax(cc))
    if 0 < j < len(cc) - 1:
        aa, bb, c = cc[j - 1], cc[j], cc[j + 1]
        den = aa - 2 * bb + c
        off = 0.5 * (aa - c) / den if den != 0 else 0.0
    else:
        off = 0.0
    return float(ks[j] + off)


def schraffur(sz, c12):
    """Gelb-schwarze Schraffur im Rand des Schilds (Cut 12). Gemessen wird auf der
    HELLIGKEIT (Farbe liegt nur in 2x2-Bloecken vor): Periode laengs des Rands ueber die
    Autokorrelation, Winkel ueber den Versatz zweier paralleler Abtastlinien."""
    L = sz.L[12]
    s = c12["s"]
    X = c12["bild_x_bei_y155"]
    Y = c12["bild_y_bei_x164p5"]
    aus = dict()

    def senkrecht(name, x0, x1, y0, y1):
        ys = np.arange(y0, y1 + 1e-9, 0.5)
        xm = 0.5 * (x0 + x1)
        q = 0.25 * (x1 - x0)
        a = tn.abtasten(L, np.full_like(ys, xm - q), ys)
        b = tn.abtasten(L, np.full_like(ys, xm + q), ys)
        m = tn.abtasten(L, np.full_like(ys, xm), ys)
        per, guete = _periode(m)
        v = _versatz(a, b, 8) * 0.5 / (2 * q)        # px laengs je px quer
        return dict(name=name, bereich=[x0, x1, y0, y1], periode_px=per * 0.5 if per else None, guete=guete,
                    versatz_je_px=v, kontrast=float(np.percentile(m, 90) - np.percentile(m, 10)))

    def waagrecht(name, x0, x1, y0, y1):
        xs = np.arange(x0, x1 + 1e-9, 0.5)
        ym = 0.5 * (y0 + y1)
        q = 0.25 * (y1 - y0)
        a = tn.abtasten(L, xs, np.full_like(xs, ym - q))
        b = tn.abtasten(L, xs, np.full_like(xs, ym + q))
        m = tn.abtasten(L, xs, np.full_like(xs, ym))
        per, guete = _periode(m)
        v = _versatz(a, b, 8) * 0.5 / (2 * q)
        return dict(name=name, bereich=[x0, x1, y0, y1], periode_px=per * 0.5 if per else None, guete=guete,
                    versatz_je_px=v, kontrast=float(np.percentile(m, 90) - np.percentile(m, 10)))
    aus["rand_hi"] = senkrecht("rand_hi", X["schild_hi"] + 1.0, X["tafel_hi"] - 1.0, Y["tafel_oben"], Y["tafel_unten"])
    aus["rand_lo"] = senkrecht("rand_lo", X["tafel_lo"] + 1.0, X["schild_lo"] - 1.0, Y["tafel_oben"], Y["tafel_unten"])
    aus["rand_oben"] = waagrecht("rand_oben", X["tafel_hi"], X["tafel_lo"], Y["schild_oben"] + 1.0, Y["tafel_oben"] - 1.0)
    aus["rand_unten"] = waagrecht("rand_unten", X["tafel_hi"], X["tafel_lo"], Y["tafel_unten"] + 1.0, Y["schild_unten"] - 1.0)
    for n, r in aus.items():
        if r["periode_px"]:
            r["periode"] = r["periode_px"] * s
            # Streifen mit Periode P laengs des Rands und Versatz v (laengs je quer):
            # Streifenrichtung gegen die Randrichtung: tan = 1 / |v|
            v = r["versatz_je_px"]
            r["winkel_grad"] = float(np.degrees(np.arctan2(1.0, abs(v)))) if v != 0 else 90.0
            r["streifenbreite_senkrecht"] = float(0.5 * r["periode"] * np.sin(np.radians(r["winkel_grad"])))
        else:
            r["periode"] = None
            r["winkel_grad"] = None
    return aus


# =============================================================================
# 8. Empfindlichkeit, Gegenueberstellung, Bauteile
# =============================================================================
def empfindlichkeit(sz, eb):
    """Aenderung der Tormasse, wenn die Torebene um +100 Einheiten (zu groesserem z)
    verschoben wird - je Cut, fuer ein Mass von der Groesse des Tors."""
    aus = {}
    for cut, z, px in ((0, eb["zA"], ((134.5, 94.0), (161.0, 94.0), (147.0, 84.4), (147.0, 104.8))),
                       (11, eb["zB"], ((78.8, 80.0), (120.5, 80.0), (98.0, 66.4), (98.0, 91.3))),
                       (12, eb["zB"], ((110.0, 155.0), (218.0, 155.0), (164.5, 118.5), (164.5, 191.4)))):
        k = sz.k[cut]

        def masse(zz):
            a = auf_z(k, px[0][0], px[0][1], zz)[0]
            b = auf_z(k, px[1][0], px[1][1], zz)[0]
            c = auf_z(k, px[2][0], px[2][1], zz)[0]
            d = auf_z(k, px[3][0], px[3][1], zz)[0]
            return abs(b[0] - a[0]), abs(d[1] - c[1]), BODEN_Y - c[1], BODEN_Y - d[1]
        m0 = masse(z)
        m1 = masse(z + 100.0)
        aus["cut%d" % cut] = dict(
            breite=m0[0], hoehe=m0[1], oberkante=m0[2], unterkante=m0[3],
            d_breite=m1[0] - m0[0], d_hoehe=m1[1] - m0[1], d_oberkante=m1[2] - m0[2], d_unterkante=m1[3] - m0[3],
            d_breite_prozent=100.0 * (m1[0] - m0[0]) / m0[0], d_hoehe_prozent=100.0 * (m1[1] - m0[1]) / m0[1])
    return aus


def gegenueberstellung(c0, c11, c12):
    """Dieselben Masse aus Cut 0, Cut 11 (RDT-Kamera + Torebene) und Cut 12 (Verhaeltnisse
    mal Massstab s) nebeneinander. u ab Achse hi-Rahmenrohr, v ueber Boden."""
    u0, u11, u12 = c0["welt"]["u"], c11["welt"]["u"], c12["u"]
    v0, v11, v12 = c0["welt"]["v"], c11["welt"]["v"], c12["v"]
    zeilen = []

    def z(name, art, a, b, c, hinweis=""):
        werte = [w for w in (a, b, c) if w is not None]
        zeilen.append(dict(name=name, art=art, cut0=a, cut11=b, cut12=c,
                           spanne=(max(werte) - min(werte)) if len(werte) > 1 else None, hinweis=hinweis))
    z("pfosten_hi (Achse)", "u", u0["pfosten_hi"], u11["pfosten_hi"],
      0.5 * (u12["pfosten_hi_links"] + u12["pfosten_hi_rechts"]),
      "Cut 12: heller Pfosten 97,6..106,4 px; ob das der Gelaenderpfosten ist, ist OFFEN")
    z("schild hi-Kante", "u", u0["schild_hi_gelbanstieg"], u11["schild_hi_kante"], u12["schild_hi"],
      "Cut 0: nur der Gelb-Anstieg, der hi-Rand geht in das gelblich beleuchtete Rohr ueber")
    z("tafel hi-Kante", "u", u0["tafel_hi_kante"], None, u12["tafel_hi"], "Cut 0 zeigt die ANDERE Schildseite")
    z("tafel lo-Kante", "u", u0["tafel_lo_kante"], None, u12["tafel_lo"], "Cut 0 zeigt die ANDERE Schildseite")
    z("schild lo-Kante", "u", u0["schild_lo_kante"], u11["schild_lo_kante"], u12["schild_lo"])
    z("rahmen lo (Rohr)", "u", None, u11["lo_dunkel"],
      0.5 * (u12["duennrohr_lo_links"] + u12["duennrohr_lo_rechts"]),
      "Cut 11: dunkle Linie; Cut 12: duenner heller Streifen")
    z("pfosten_lo (Achse)", "u", u0["pfosten_lo"], u11["lo_hell"], None,
      "Cut 11: helle Flanke des Pfostens")
    z("rohr oben (Mitte)", "v", v0["rohr_oben"], v11["rohr_oben"],
      0.5 * (v12["rohr_oben_aussen"] + v12["rohr_oben_innen"]), "Cut 12 ist hier auf das Mittel verankert")
    z("schild Oberkante", "v", v0["schild_oben_kante"], None, v12["schild_oben"])
    z("tafel Oberkante", "v", v0["tafel_oben_kante"], None, v12["tafel_oben"], "Cut 0 zeigt die ANDERE Schildseite")
    z("tafel Unterkante", "v", v0["tafel_unten_kante"], None, v12["tafel_unten"], "Cut 0 zeigt die ANDERE Schildseite")
    z("schild Unterkante", "v", v0["schild_unten_kante"], None, v12["schild_unten"])
    z("rohr unten (Mitte)", "v", v0["rohr_unten"], v11["rohr_unten"],
      0.5 * (v12["rohr_unten_dunkel_oben"] + v12["rohr_unten_unten"]))
    z("schild Breite", "mass", u0["schild_lo_kante"] - u0["schild_hi_gelbanstieg"],
      u11["schild_lo_kante"] - u11["schild_hi_kante"], u12["schild_lo"] - u12["schild_hi"])
    z("schild Hoehe", "mass", v0["schild_oben_kante"] - v0["schild_unten_kante"], None,
      v12["schild_oben"] - v12["schild_unten"])
    z("rahmen Hoehe Mitte-Mitte", "mass", v0["rohr_oben"] - v0["rohr_unten"], v11["rohr_oben"] - v11["rohr_unten"],
      0.5 * (v12["rohr_oben_aussen"] + v12["rohr_oben_innen"]) - 0.5 * (v12["rohr_unten_dunkel_oben"] + v12["rohr_unten_unten"]))
    z("pfosten_hi -> rahmenrohr_hi", "mass", -u0["pfosten_hi"], -u11["pfosten_hi"],
      -0.5 * (u12["pfosten_hi_links"] + u12["pfosten_hi_rechts"]))
    return zeilen


def rohrdurchmesser(sz, eb, c0, c11, c12):
    """D = Breite_px * z_view / H (RDT-Kamera) fuer Cut 0 und Cut 11; fuer Cut 12 zusaetzlich
    mit dem Massstab s, weil die RDT-Kamera von Cut 12 das Bild nicht beschreibt."""
    aus = []
    for cut, c, z in ((0, c0, eb["zA"]), (11, c11, eb["zB"])):
        k = sz.k[cut]
        M = c["px"]
        for n, achse in (("rohr_oben", "h"), ("rohr_unten", "h"), ("rahmenrohr_hi", "v"),
                         ("pfosten_hi", "v"), ("pfosten_lo", "v"), ("lo_dunkel", "v")):
            if n not in M:
                continue
            e = M[n]
            if achse == "h":
                px, py = M["bezugsspalte"], e["mitte"]
            else:
                px, py = e["mitte"], M["bezugszeile"]
            w, vz = auf_z(k, px, py, z)
            aus.append(dict(cut=cut, name=n, breite_px=e["breite"], vz=float(vz), H=k.H,
                            D=float(e["breite"] * vz / k.H),
                            art="Halbwertsbreite eines %s Bandes" % ("hellen" if e["niveau"][1] > e["niveau"][0] else "dunklen")))
    k = sz.k[12]
    Y, X = c12["bild_y_bei_x164p5"], c12["bild_x_bei_y155"]
    s = c12["s"]
    for n, a, b, px, py, art in (
            ("rohr_oben", Y["rohr_oben_aussen"], Y["rohr_oben_innen"], REF12[0], None, "beide Kanten gegen den Himmel"),
            ("rohr_oben_helle_flanke", 0.0, 0.0, REF12[0], None, "untere, vom Licht getroffene Flanke"),
            ("rohr_unten", Y["rohr_unten_dunkel_oben"], Y["rohr_unten_unten"], REF12[0], None, "dunkle Oberkante bis helle Unterkante"),
            ("rohr_unten_helle_flanke", Y["rohr_unten_hell_oben"], Y["rohr_unten_unten"], REF12[0], None, "heller Teil"),
            ("rahmenrohr_hi", X["rahmenrohr_hi_links"], X["rahmenrohr_hi_rechts"], None, REF12[1], "beide Kanten gegen den Himmel"),
            ("rahmenrohr_hi_helle_flanke", X["rahmenrohr_hi_hellgrenze"], X["rahmenrohr_hi_rechts"], None, REF12[1], "vom Licht getroffene Flanke"),
            ("rahmenrohr_lo_sichtbar", X["duennrohr_lo_links"], X["duennrohr_lo_rechts"], None, REF12[1], "duenner heller Streifen"),
            ("pfosten_hi_hell", X["pfosten_hi_links"], X["pfosten_hi_rechts"], None, REF12[1], "heller Teil; linke Kante gegen Dunkel, nicht gegen Himmel")):
        if n == "rohr_oben_helle_flanke":
            # Grenze blaugrau/gelblich im oberen Rohr: Stufe im Querprofil
            ys, q = querprofil(sz, 12, (REF12[0], 122.5), 40, "x", 6, 0.25, "L")
            a = _stufe(ys, q, 119.0, 122.5)
            b = Y["rohr_oben_innen"]
            if a is None:
                continue
        breite = abs(b - a)
        if px is None:
            w, vz = auf_z(k, 0.5 * (a + b), py, eb["zB"])
        else:
            w, vz = auf_z(k, px, 0.5 * (a + b), eb["zB"])
        aus.append(dict(cut=12, name=n, breite_px=float(breite), von=float(a), bis=float(b), vz_rdt=float(vz), H=k.H,
                        D_rdt=float(breite * vz / k.H), D=float(breite * s), art=art))
    return aus


def bauteile(sz, eb, c0, c11, c12, et):
    """Bauteil-Verzeichnis. Lage und Masse aus Cut 12 (Verhaeltnisse) mal Massstab s
    (aus Cut 0 / Cut 11); Quell-Pixelrechtecke je Cut."""
    s = c12["s"]
    u, v = c12["u"], c12["v"]
    X, Y = c12["bild_x_bei_y155"], c12["bild_y_bei_x164p5"]
    M0, M11 = c0["px"], c11["px"]
    D = float(v["rohr_oben_aussen"] - v["rohr_oben_innen"])             # Rohrdicke (oberes Rohr, beide Kanten gegen Himmel)
    e = et["ecke_hi_oben"]
    u_hi_aussen = u["rahmenrohr_hi_links"]
    lo = et["lo_seite"]
    # lo-Aussenkante: NICHT gemessen (schwarz vor schwarz). Gesetzt wird die Spiegelung des
    # hi-Rohrs an der Schildmitte; die gemessene Innenkante trifft die gespiegelte auf wenige Einheiten.
    u_lo_aussen = lo["spiegel_hi_rohr"][1]
    v_oben = v["rohr_oben_aussen"]
    v_unten = v["rohr_unten_unten"]
    auf, fu = et["aufhaengung"], et["fuesse"]
    uv = c12["_uv"]

    def ux(x):
        return uv(x, REF12[1])[0]
    teile = []
    teile.append(dict(
        name="rahmen", art="Rohr", beweglich=True,
        beschreibung="geschlossener Rohrrahmen mit runden Ecken (Aussenkontur)",
        u=[u_hi_aussen, u_lo_aussen], v=[v_unten, v_oben], w=[-D / 2, D / 2],
        rohrdurchmesser=D, eckenradius_aussen=e["radius_aussen"], eckenradius_innen=e["radius_innen"],
        eckenradius_mittellinie=e["radius_mittellinie"],
        u_lo_aussen_herkunft="Spiegelung des hi-Rohrs an der Schildmitte (Annahme Symmetrie)",
        u_lo_sichtbarer_streifen=[u["duennrohr_lo_links"], u["duennrohr_lo_rechts"]],
        u_lo_schwarzes_glied_bis=lo["schwarzes_glied_u"][1],
        lichte_weite=[u["rahmenrohr_hi_rechts"], u["duennrohr_lo_links"]],
        lichte_hoehe=[v["rohr_unten_dunkel_oben"], v["rohr_oben_innen"]],
        px=dict(cut0=[M0["fleck_lo"]["x"]["links"] - 1.0, M0["rohr_oben"]["links"], M0["rahmenrohr_hi"]["rechts"], M0["rohr_unten"]["rechts"]],
                cut11=[M11["rahmenrohr_hi"]["links"], M11["rohr_oben"]["links"], M11["lo_dunkel"]["rechts"], M11["rohr_unten"]["rechts"]],
                cut12=[X["rahmenrohr_hi_links"], Y["rohr_oben_aussen"], X["duennrohr_lo_rechts"], Y["rohr_unten_unten"]])))
    teile.append(dict(
        name="rahmenrohr_oben", art="Rohr", beweglich=True, u=[u_hi_aussen, u_lo_aussen],
        v=[v["rohr_oben_innen"], v["rohr_oben_aussen"]], w=[-D / 2, D / 2], rohrdurchmesser=D,
        px=dict(cut0=[134.0, M0["rohr_oben"]["links"], 161.0, M0["rohr_oben"]["rechts"]],
                cut11=[79.0, M11["rohr_oben"]["links"], 117.0, M11["rohr_oben"]["rechts"]],
                cut12=[X["rahmenrohr_hi_links"], Y["rohr_oben_aussen"], X["duennrohr_lo_rechts"], Y["rohr_oben_innen"]])))
    teile.append(dict(
        name="rahmenrohr_unten", art="Rohr", beweglich=True, u=[u_hi_aussen, u_lo_aussen],
        v=[v["rohr_unten_unten"], v["rohr_unten_dunkel_oben"]], w=[-D / 2, D / 2],
        rohrdurchmesser=float(v["rohr_unten_dunkel_oben"] - v["rohr_unten_unten"]),
        sichtbar_hell=[v["rohr_unten_unten"], v["rohr_unten_hell_oben"]],
        px=dict(cut0=[136.0, M0["rohr_unten"]["links"], 160.0, M0["rohr_unten"]["rechts"]],
                cut11=[84.0, M11["rohr_unten"]["links"], 113.0, M11["rohr_unten"]["rechts"]],
                cut12=[X["rahmenrohr_hi_links"], Y["rohr_unten_dunkel_oben"], X["duennrohr_lo_rechts"], Y["rohr_unten_unten"]])))
    teile.append(dict(
        name="rahmenrohr_hi", art="Rohr", beweglich=True,
        u=[u["rahmenrohr_hi_links"], u["rahmenrohr_hi_rechts"]], v=[v_unten, v_oben], w=[-D / 2, D / 2],
        rohrdurchmesser=float(u["rahmenrohr_hi_rechts"] - u["rahmenrohr_hi_links"]),
        beleuchtete_flanke=[u["rahmenrohr_hi_hellgrenze"], u["rahmenrohr_hi_rechts"]],
        px=dict(cut0=[M0["rahmenrohr_hi"]["links"], 85.0, M0["rahmenrohr_hi"]["rechts"], 104.0],
                cut11=[M11["rahmenrohr_hi"]["links"], 66.0, M11["rahmenrohr_hi"]["rechts"], 91.0],
                cut12=[X["rahmenrohr_hi_links"], 135.0, X["rahmenrohr_hi_rechts"], 178.0])))
    teile.append(dict(
        name="rahmenrohr_lo", art="Rohr", beweglich=True,
        u=[u["duennrohr_lo_links"], u_lo_aussen], v=[v_unten, v_oben], w=[-D / 2, D / 2],
        sichtbar=[u["duennrohr_lo_links"], u["duennrohr_lo_rechts"]],
        hinweis="Innenkante gemessen; Aussenkante = Spiegelung des hi-Rohrs (Annahme). In Cut 12 ist nur die 3,4 px breite, vom Himmel beleuchtete Innenflanke zu sehen, der Rest ist schwarz vor schwarz; die Ecken der lo-Seite sind nicht messbar",
        px=dict(cut0=[134.0, 86.0, 136.5, 104.0], cut11=[M11["lo_dunkel"]["links_zeile87"], 70.0, M11["lo_dunkel"]["rechts_zeile87"], 94.0],
                cut12=[X["duennrohr_lo_links"], 138.0, X["duennrohr_lo_rechts"], 170.0])))
    teile.append(dict(
        name="schild", art="Platte", beweglich=True, beschreibung="Warnschild, beidseitig bedruckt",
        u=[u["schild_hi"], u["schild_lo"]], v=[v["schild_unten"], v["schild_oben"]], w=None,
        breite=float(u["schild_lo"] - u["schild_hi"]), hoehe=float(v["schild_oben"] - v["schild_unten"]),
        px=dict(cut0=[M0["schild_lo_kante"], M0["schild_oben_kante"], M0["tafel_hi_kante"] + 1.5, M0["schild_unten_kante"]],
                cut11=[M11["schild_hi_kante"], 72.0, M11["schild_lo_hell"]["rechts_zeile87"], 91.0],
                cut12=[X["schild_hi"], Y["schild_oben"], X["schild_lo"], Y["schild_unten"]])))
    teile.append(dict(
        name="texttafel", art="Platte", beweglich=True, beschreibung="graue Texttafel im Schild (Laufsteg-Seite)",
        u=[u["tafel_hi"], u["tafel_lo"]], v=[v["tafel_unten"], v["tafel_oben"]], w=None,
        breite=float(u["tafel_lo"] - u["tafel_hi"]), hoehe=float(v["tafel_oben"] - v["tafel_unten"]),
        px=dict(cut0=[M0["tafel_lo_kante"], M0["tafel_oben_kante"], M0["tafel_hi_kante"], M0["tafel_unten_kante"]],
                cut11=None,
                cut12=[X["tafel_hi"], Y["tafel_oben"], X["tafel_lo"], Y["tafel_unten"]])))
    sc = et["schraffur"]
    teile.append(dict(
        name="schraffurrand", art="Platte", beweglich=True, beschreibung="gelb-schwarzer Rand des Schilds",
        breite_hi=float(u["tafel_hi"] - u["schild_hi"]), breite_lo=float(u["schild_lo"] - u["tafel_lo"]),
        breite_oben=float(v["schild_oben"] - v["tafel_oben"]), breite_unten=float(v["tafel_unten"] - v["schild_unten"]),
        schraffur={n: dict(periode=sc[n]["periode"], winkel_grad=sc[n]["winkel_grad"], periode_px=sc[n]["periode_px"])
                   for n in sc},
        px=dict(cut12=[X["schild_hi"], Y["schild_oben"], X["schild_lo"], Y["schild_unten"]])))
    for n, a in auf.items():
        teile.append(dict(
            name="aufhaengung_" + n, art="Platte", beweglich=True, beschreibung="dunkle Lasche zwischen Rohr oben und Schild",
            u=[ux(a["links"]), ux(a["rechts"])], v=[v["schild_oben"], v["rohr_oben_innen"]], w=None,
            breite=float(ux(a["rechts"]) - ux(a["links"])), hoehe=float(v["rohr_oben_innen"] - v["schild_oben"]),
            px=dict(cut12=[a["links"], 127.0, a["rechts"], 135.0])))
    for n, a in fu.items():
        if n == "block_lo":
            continue
        teile.append(dict(
            name=n, art="Platte", beweglich=True, beschreibung="Stuetze zwischen Schild-Unterkante und Rohr unten",
            u=[ux(a["links"]), ux(a["rechts"])], v=[v["rohr_unten_dunkel_oben"], v["schild_unten"]], w=None,
            breite=float(ux(a["rechts"]) - ux(a["links"])), hoehe=float(v["schild_unten"] - v["rohr_unten_dunkel_oben"]),
            px=dict(cut12=[a["links"], 180.0, a["rechts"], 184.0])))
    b = fu["block_lo"]
    teile.append(dict(
        name="block_lo", art="Platte", beweglich=None,
        beschreibung="heller Block neben dem lo-Fuss, zwischen Schild-Unterkante und Rohr unten; Zuordnung OFFEN",
        u=[ux(b["links"]), ux(b["rechts"])], v=[v["rohr_unten_dunkel_oben"], v["schild_unten"]], w=None,
        px=dict(cut12=[b["links"], 180.0, b["rechts"], 187.0])))
    teile.append(dict(
        name="kasten_am_pfosten_lo", art="Platte", beweglich=False,
        beschreibung="schwarzer Kasten vor dem Himmel am lo-Pfosten, helle linke Kante",
        u=lo["kasten_u"], v=lo["kasten_v"], w=None,
        px=dict(cut12=lo["kasten_px"])))
    if "spalt_lo_schwarz_oberkante_v" in lo:
        teile.append(dict(
            name="dunkle_flaeche_im_lo_spalt", art="Platte", beweglich=None,
            beschreibung="zwischen Schild und lo-Rohr ist unterhalb dieser Hoehe kein Himmel zu sehen (im hi-Spalt schon); Torteil (Riegel-/Schlossblech) oder Hintergrund - OFFEN",
            u=[u["schild_lo"], u["duennrohr_lo_links"]], v=[v["rohr_unten_dunkel_oben"], lo["spalt_lo_schwarz_oberkante_v"]], w=None,
            px=dict(cut12=[X["schild_lo"], lo["spalt_lo_schwarz_oberkante_px"][1], X["duennrohr_lo_links"], 180.0])))
    f0 = c0["welt"]["fleck_lo"]
    teile.append(dict(
        name="heller_fleck_lo_cut0", art="Platte", beweglich=None,
        beschreibung="sehr heller Fleck an der lo-Seite (Helligkeit bis 250), nur in Cut 0; Riegel/Schloss oder Glanzlicht - OFFEN",
        u=[min(f0["u"]), max(f0["u"])], v=[min(f0["v"]), max(f0["v"])], w=None,
        px=dict(cut0=[M0["fleck_lo"]["x"]["links"], M0["fleck_lo"]["y"]["links"],
                      M0["fleck_lo"]["x"]["rechts"], M0["fleck_lo"]["y"]["rechts"]])))
    # feststehend: Pfosten aus Cut 0 / Cut 11 (in Cut 12 nicht sicher zuzuordnen)
    w0, w11 = c0["welt"], c11["welt"]
    teile.append(dict(
        name="pfosten_hi", art="Rohr", beweglich=False, beschreibung="Endpfosten des Gelaenders an der hi-Seite",
        u_cut0=w0["u"]["pfosten_hi"], u_cut11=w11["u"]["pfosten_hi"],
        breite_cut0=w0["breite"]["pfosten_hi"], breite_cut11=w11["breite"]["pfosten_hi"],
        v=[None, None], px=dict(cut0=[M0["pfosten_hi"]["links"], 77.0, M0["pfosten_hi"]["rechts"], 106.0],
                                cut11=[M11["pfosten_hi"]["links"], 66.0, M11["pfosten_hi"]["rechts"], 90.0],
                                cut12=None)))
    teile.append(dict(
        name="pfosten_lo", art="Rohr", beweglich=False, beschreibung="Endpfosten des Gelaenders an der lo-Seite",
        u_cut0=w0["u"]["pfosten_lo"], u_cut11_dunkel=w11["u"]["lo_dunkel"], u_cut11_hell=w11["u"]["lo_hell"],
        breite_cut0=w0["breite"]["pfosten_lo"],
        v=[None, None], px=dict(cut0=[M0["pfosten_lo"]["links"], 84.0, M0["pfosten_lo"]["rechts"], 110.0],
                                cut11=[M11["lo_dunkel"]["links_zeile87"], 64.0, M11["lo_hell"]["rechts_zeile87"], 95.0],
                                cut12=None)))
    return dict(
        ursprung="Fusspunkt der Achse des hi-Rahmenrohrs auf dem Boden y=-7200 (Bereich B: zB = %.0f, Bereich A: zA = %.0f)" % (eb["zB"], eb["zA"]),
        achsen="u laengs des Tors von hi nach lo (Welt -x), v nach oben (Welt -y), w senkrecht zur Torebene (Welt z)",
        massstab_cut12=s, teile=teile)


def massstabsanker(sz, eb, c12):
    """Spielerhoehe (Trefferkasten PSX.EXE @0x80073e94 = Datei 0x64694; Skelettwurzel
    PL00.PLD @Datei 0xC68) gegen Gelaender und Tor."""
    exe = open(os.path.join(WURZEL, "info", "Re1.5", "PSX.EXE"), "rb").read()
    o = 0x64694
    t = struct.unpack_from("<6h", exe, o)
    pld = open(os.path.join(WURZEL, "re15_port", "shared_assets", "PSX", "PLD", "PL00.PLD"), "rb").read()
    w = struct.unpack_from("<3h", pld, 0xC68)
    sl = {s["name"]: s for s in eb["stereo"] if "hoehe" in s}
    return dict(trefferkasten_off=o, trefferkasten_roh=exe[o:o + 12].hex(" "), trefferkasten=list(t),
                trefferkasten_hoehe=2 * t[4], skelettwurzel_off=0xC68, skelettwurzel_roh=pld[0xC68:0xC6E].hex(" "),
                skelettwurzel_y=w[1],
                holm_oben=float(np.mean([sl["holm_oben_lo_nah"]["hoehe"], sl["holm_oben_lo_fern"]["hoehe"]])),
                holm_mitte=sl["holm_mitte_lo"]["hoehe"], tor_oberkante=c12["v"]["rohr_oben_aussen"],
                tor_unterkante=c12["v"]["rohr_unten_unten"],
                holm_oben_durch_hueftgelenk=float(np.mean([sl["holm_oben_lo_nah"]["hoehe"], sl["holm_oben_lo_fern"]["hoehe"]]) / -w[1]),
                tor_oberkante_durch_hueftgelenk=float(c12["v"]["rohr_oben_aussen"] / -w[1]))


def _rund(o, n=2):
    if isinstance(o, dict):
        return {k: _rund(v, n) for k, v in o.items() if not str(k).startswith("_")}
    if isinstance(o, (list, tuple)):
        return [_rund(v, n) for v in o]
    if isinstance(o, (float, np.floating)):
        return None if not np.isfinite(o) else round(float(o), n)
    if isinstance(o, (np.integer,)):
        return int(o)
    if isinstance(o, np.ndarray):
        return _rund(o.tolist(), n)
    return o


# =============================================================================
# 9. Kontrollbilder
# =============================================================================
def _lupe(sz, cut, x0, y0, x1, y1, F, gamma=0.6):
    a = sz.bg[cut].astype(float) / 255.0
    a = (255.0 * a ** gamma).clip(0, 255).astype(np.uint8)
    M = 26
    im = Image.new("RGB", ((x1 - x0) * F + M, (y1 - y0) * F + M), (40, 40, 40))
    im.paste(Image.fromarray(a[y0:y1, x0:x1]).resize(((x1 - x0) * F, (y1 - y0) * F), Image.NEAREST), (M, M))
    d = ImageDraw.Draw(im)
    for x in range(x0, x1 + 1):
        if x % 5 == 0:
            X = M + (x - x0) * F
            d.text((X - 8, 2), str(x), fill=(255, 255, 0))
            d.line([(X, M - 5), (X, M)], fill=(255, 255, 255))
    for y in range(y0, y1 + 1):
        if y % 5 == 0:
            Y = M + (y - y0) * F
            d.text((1, Y - 5), str(y), fill=(255, 255, 0))
            d.line([(M - 5, Y), (M, Y)], fill=(255, 255, 255))

    def P(x, y):
        return (M + (x - x0) * F, M + (y - y0) * F)
    return im, d, P


def bild_merkmale(sz, c0, c11, c12, et):
    pf = []
    # --- Cut 12
    im, d, P = _lupe(sz, 12, 90, 100, 235, 200, 8)
    K = c12["kanten_frei"]
    for n, e in K.items():
        if not e:
            continue
        lo, hi = e["bereich"][0], e["bereich"][1]
        if e["achse"] == "h":
            d.line([P(lo, wert_bei(e, lo)), P(hi + 1, wert_bei(e, hi + 1))], fill=(255, 0, 255), width=1)
        else:
            d.line([P(wert_bei(e, lo), lo), P(wert_bei(e, hi + 1), hi + 1)], fill=(0, 255, 0), width=1)
    for n, e in c12["kanten_laengs"].items():
        if e:
            p = np.array(e["p"])
            dd = np.array(e["d"])
            d.line([P(*(p - 20 * dd)), P(*(p + 20 * dd))], fill=(255, 128, 0), width=1)
    e = et["ecke_hi_oben"]
    for art, farbe in (("aussen", (0, 255, 255)), ("innen", (255, 255, 0))):
        k = e[art]
        al = np.radians(np.arange(95, 176, 2))
        pts = [P(k["mitte"][0] + k["radius_px"] * np.cos(a), k["mitte"][1] - k["radius_px"] * np.sin(a)) for a in al]
        d.line(pts, fill=farbe, width=1)
        for p in k["punkte"]:
            q = P(*p)
            d.ellipse([q[0] - 2, q[1] - 2, q[0] + 2, q[1] + 2], outline=farbe)
    for n, a in et["aufhaengung"].items():
        d.rectangle([P(a["links"], 127.0), P(a["rechts"], 135.0)], outline=(255, 255, 255))
    for n, a in et["fuesse"].items():
        d.rectangle([P(a["links"], 180.0), P(a["rechts"], 184.2)], outline=(255, 255, 255))
    pfad = os.path.join(AUS, "vermessung_merkmale_cut12.png")
    im.save(pfad)
    pf.append(pfad)
    # --- Cut 0 / Cut 11: jedes Merkmal eine Linie mit Nummer; Legende im JSON (bilder_legende)
    legende = {}
    for cut, c, fen, F in ((0, c0, (124, 74, 172, 114), 20), (11, c11, (62, 50, 132, 104), 14)):
        im, d, P = _lupe(sz, cut, fen[0], fen[1], fen[2], fen[3], F, gamma=0.8)
        M = c["px"]
        fp = sz.fpy[cut]
        fx = sz.fpx[cut]
        yz = M["bezugszeile"]
        xsp = M["bezugsspalte"]
        leg = []
        nr = 0
        for n in sorted(M):
            e = M[n]
            if n in ("bezugszeile", "bezugsspalte", "fleck_lo") or e is None:
                continue
            waag = n.startswith("rohr_oben") or n.startswith("rohr_unten") or n.startswith("rand_oben")                 or n.startswith("rand_unten") or n.endswith("_oben_kante") or n.endswith("_unten_kante")
            band = isinstance(e, dict)
            wv = e["mitte"] if band else e
            nr += 1
            farbe = (255, 60, 60) if band else (0, 255, 0)
            if waag:
                a0 = np.array([xsp, wv])
                r = (fx - a0) / np.linalg.norm(fx - a0)
                if r[0] < 0:
                    r = -r
                p1, p2 = a0 - 11 * r, a0 + 11 * r
                d.line([P(*p1), P(*p2)], fill=farbe)
                q = P(*(a0 + (12.0 + (nr % 3) * 1.6) * r))
                d.text((q[0], q[1] - 5), str(nr), fill=(255, 255, 255))
            else:
                a0 = np.array([wv, yz])
                r = (fp - a0) / np.linalg.norm(fp - a0)
                if r[1] < 0:
                    r = -r
                lang = 11.0 if cut == 0 else 13.0
                d.line([P(*(a0 - lang * r)), P(*(a0 + lang * r))], fill=farbe)
                q = P(*(a0 - (lang + 1.0 + (nr % 3) * 1.4) * r))
                d.text((q[0] - 4, q[1] - 5), str(nr), fill=(255, 255, 255))
            leg.append("%d=%s (%s %.2f)" % (nr, n, "y" if waag else "x", wv))
        if "fleck_lo" in M:
            f = M["fleck_lo"]
            d.rectangle([P(f["x"]["links"], f["y"]["links"]), P(f["x"]["rechts"], f["y"]["rechts"])], outline=(0, 255, 255))
        legende["cut%d" % cut] = leg
        pfad = os.path.join(AUS, "vermessung_merkmale_cut%02d.png" % cut)
        im.save(pfad)
        pf.append(pfad)
    return [os.path.relpath(p, WURZEL).replace("\\", "/") for p in pf], legende


def bild_rueckprojektion(sz, eb, c0, c11, c12, bt, Tx):
    """Das Bauteil-Verzeichnis (aus Cut 12) zurueck in alle drei Cuts: Cut 12 ueber die
    Ebenen-Abbildung, Cut 0 und Cut 11 ueber die RDT-Kamera und die Torebene."""
    pf = []
    E = c12["_E"]
    s = c12["s"]
    e12 = c12["ebene12"]

    def uv_zu_bild12(u, v):
        a = u / s + e12["roh_u0"]
        b = (e12["v0"] - v) / s + e12["roh_v0"]
        return E.bild(a, b)
    rechtecke = []
    for t in bt["teile"]:
        if t.get("u") and t.get("v") and None not in t["u"] and None not in t["v"]:
            rechtecke.append((t["name"], t["u"], t["v"], t["art"]))
    farben = dict(Rohr=(0, 255, 0), Platte=(255, 255, 0))
    for cut, fen, F in ((12, (90, 100, 235, 200), 8), (0, (124, 74, 172, 114), 20), (11, (62, 50, 132, 104), 14)):
        im, d, P = _lupe(sz, cut, fen[0], fen[1], fen[2], fen[3], F, gamma=0.7)
        k = sz.k[cut]
        if cut == 0:
            z = eb["zA"]
            xg = c0["welt"]["x_rahmenrohr_hi"]
        elif cut == 11:
            z = eb["zB"]
            xg = c11["welt"]["x_rahmenrohr_hi"]
        for name, u, v, art in rechtecke:
            if name == "rahmen":
                continue
            ecken = [(u[0], v[0]), (u[1], v[0]), (u[1], v[1]), (u[0], v[1]), (u[0], v[0])]
            pts = []
            for (uu, vv) in ecken:
                if cut == 12:
                    q = uv_zu_bild12(uu, vv)
                    pts.append(P(q[0], q[1]))
                else:
                    sx, sy, vz = k.bild(np.array([xg - uu, BODEN_Y - vv, z]))
                    pts.append(P(sx, sy))
            d.line(pts, fill=farben[art], width=1)
        pfad = os.path.join(AUS, "vermessung_rueckprojektion_cut%02d.png" % cut)
        im.save(pfad)
        pf.append(pfad)
    return [os.path.relpath(p, WURZEL).replace("\\", "/") for p in pf]


# =============================================================================
# 10. RDT: eigenes 3D-Modell fuer das Tor?
# =============================================================================
def rdt_modelle(sz, druck=True):
    """Objektmodelle des Raums: Kopf @0x00 (nOmodel), Zeigertabelle @0x30 -> je Modell
    8 B (TIM-Zeiger, MD1-Zeiger). MD1: Kopf 12 B (tex_off, merker, gruppen), je Mesh zwei
    Gruppen zu 28 B (Dreiecke, Vierecke), Vertices 8 B (s16 x, y, z, pad)."""
    rdt = sz.rdt
    aus = dict(kopf=dict(nSprite=rdt[0], nCut=rdt[1], nOmodel=rdt[2], nItem=rdt[3], nDoor=rdt[4],
                         nRoom_at=rdt[5], reverb_lv=rdt[6], roh=rdt[0:8].hex(" ")))
    tab = struct.unpack_from("<I", rdt, 0x30)[0]
    aus["md1_zeigertabelle_off"] = tab
    aus["md1_zeigertabelle_roh"] = rdt[tab:tab + 8 * rdt[2]].hex(" ")
    aus["modelle"] = []
    for i in range(rdt[2]):
        tim, m = struct.unpack_from("<II", rdt, tab + 8 * i)
        tex_off, merker, gr = struct.unpack_from("<3I", rdt, m)
        pkt = []
        nd = nv = 0
        for g in range(gr // 2):
            o = m + 12 + g * 56
            tvo, tvc, tno, tnc, tpo, tpc, tto = struct.unpack_from("<7I", rdt, o)
            qpc = struct.unpack_from("<7I", rdt, o + 28)[5]
            nd += tpc
            nv += qpc
            for j in range(tvc):
                pkt.append(struct.unpack_from("<hhh", rdt, m + 12 + tvo + 8 * j))
        P = np.array(pkt)
        aus["modelle"].append(dict(
            i=i, tim_off=tim, md1_off=m, md1_kopf=rdt[m:m + 12].hex(" "), meshes=gr // 2, punkte=len(P),
            dreiecke=nd, vierecke=nv, x=[int(P[:, 0].min()), int(P[:, 0].max())],
            y=[int(P[:, 1].min()), int(P[:, 1].max())], z=[int(P[:, 2].min()), int(P[:, 2].max())],
            ausdehnung=[int(np.ptp(P[:, 0])), int(np.ptp(P[:, 1])), int(np.ptp(P[:, 2]))]))
    if druck:
        print("== 10. RDT-Kopf @0x00: %s -> nSprite %d nCut %d nOmodel %d nItem %d nDoor %d nRoom_at %d reverb %d" % (
            aus["kopf"]["roh"], rdt[0], rdt[1], rdt[2], rdt[3], rdt[4], rdt[5], rdt[6]))
        print("   Modell-Zeigertabelle @0x%X: %s" % (tab, aus["md1_zeigertabelle_roh"]))
        for mo in aus["modelle"]:
            print("   Modell %d: MD1 @0x%X (%s) TIM @0x%X  %d Punkte %d Dreiecke %d Vierecke  Ausdehnung x %d  y %d  z %d" % (
                mo["i"], mo["md1_off"], mo["md1_kopf"], mo["tim_off"], mo["punkte"], mo["dreiecke"], mo["vierecke"],
                mo["ausdehnung"][0], mo["ausdehnung"][1], mo["ausdehnung"][2]))
    return aus


# =============================================================================
def alles(druck=True):
    os.makedirs(AUS, exist_ok=True)
    sz = Szene()
    E = dict(werkzeug="re15_port/tools/tor/tor_vermessung.py alles",
             boden_y=BODEN_Y, ausschnitte={str(k): list(v) for k, v in AUSSCHNITT.items()})
    E["kamera"] = kamerasaetze(sz, druck)
    E["probe"] = probe(sz, druck)
    E["versatz"] = versatz(sz, druck)
    E["ebene"] = torebene(sz, E["versatz"], druck)
    zA, zB = E["ebene"]["zA"], E["ebene"]["zB"]
    c0 = messung_cut0(sz, zA, druck)
    c11 = messung_cut11(sz, zB, druck)
    # T.x aus Pfosten hi und Rahmenrohr hi (beide in Cut 0 und Cut 11 eindeutig)
    k0, k11 = sz.k[0], sz.k[11]
    tx = []
    for n in ("pfosten_hi", "rahmenrohr_hi"):
        a = auf_z(k0, c0["px"][n]["mitte"], c0["px"]["bezugszeile"], zA)[0][0]
        b = auf_z(k11, c11["px"][n]["mitte"], c11["px"]["bezugszeile"], zB)[0][0]
        tx.append(float(b - a))
    E["versatz"]["Tx_aus_pfosten_hi_und_rahmenrohr_hi"] = tx
    E["versatz"]["Tx"] = float(np.mean(tx))
    fa = [v[0] for v in E["versatz"]["fugen_A_x"]]
    fb = [v[0] for v in E["versatz"]["fugen_B_x"]]
    paare = []
    for a in fa:
        for b in fb:
            if abs((b - a) - E["versatz"]["Tx"]) < 400:
                paare.append([a, b, b - a])
    E["versatz"]["Tx_fugenpaare"] = paare
    if druck:
        print("   T.x aus Pfosten hi / Rahmenrohr hi: %s -> %.0f;  Fugenpaare (A, B, Differenz): %s" % (
            ["%.0f" % v for v in tx], E["versatz"]["Tx"], [[int(v) for v in p] for p in paare]))
    c12 = messung_cut12(sz, c0, c11, E["ebene"], druck)
    et = einzelteile_cut12(sz, c12, druck)
    E["cut0"] = c0
    E["cut11"] = c11
    E["cut12"] = c12
    E["cut12_einzelteile"] = et
    E["empfindlichkeit_je_100"] = empfindlichkeit(sz, E["ebene"])
    # dasselbe fuer das fertige Modell (Cut-12-Verhaeltnisse mal s): Ebene +100 und -100
    E["empfindlichkeit_modell"] = {}
    for dz in (-100.0, 100.0):
        eb2 = dict(E["ebene"])
        eb2["zA"] = zA + dz
        eb2["zB"] = zB + dz
        a = messung_cut0(sz, zA + dz, False)
        b = messung_cut11(sz, zB + dz, False)
        c = messung_cut12(sz, a, b, eb2, False)
        E["empfindlichkeit_modell"]["%+d" % dz] = dict(
            s=c["s"], s_v=c["s_v_mittel"], s_h=c["s_h_mittel"],
            anker_s=[x["s"] for x in c["anker"]],
            rahmen_hoehe=c["v"]["rohr_oben_aussen"] - c["v"]["rohr_unten_unten"],
            schild_breite=c["u"]["schild_lo"] - c["u"]["schild_hi"],
            rohr_oben_mitte=0.5 * (c["v"]["rohr_oben_aussen"] + c["v"]["rohr_oben_innen"]),
            unterkante=c["v"]["rohr_unten_unten"])
    E["empfindlichkeit_modell"]["0"] = dict(
        s=c12["s"], s_v=c12["s_v_mittel"], s_h=c12["s_h_mittel"],
        anker_s=[x["s"] for x in c12["anker"]],
        rahmen_hoehe=c12["v"]["rohr_oben_aussen"] - c12["v"]["rohr_unten_unten"],
        schild_breite=c12["u"]["schild_lo"] - c12["u"]["schild_hi"],
        rohr_oben_mitte=0.5 * (c12["v"]["rohr_oben_aussen"] + c12["v"]["rohr_oben_innen"]),
        unterkante=c12["v"]["rohr_unten_unten"])
    E["bauteile"] = bauteile(sz, E["ebene"], c0, c11, c12, et)
    E["gegenueberstellung"] = gegenueberstellung(c0, c11, c12)
    E["rohrdurchmesser"] = rohrdurchmesser(sz, E["ebene"], c0, c11, c12)
    E["massstabsanker"] = massstabsanker(sz, E["ebene"], c12)
    E["rdt"] = rdt_modelle(sz, druck)
    bm, legende = bild_merkmale(sz, c0, c11, c12, et)
    E["bilder"] = E["probe"]["bilder"] + bm \
        + bild_rueckprojektion(sz, E["ebene"], c0, c11, c12, E["bauteile"], E["versatz"]["Tx"])
    E["bilder_legende"] = legende
    if druck:
        print("== 8. Empfindlichkeit: Ebene um +100 verschoben")
        for n, e in E["empfindlichkeit_je_100"].items():
            print("  %-6s Breite %.0f (%+.1f = %+.2f %%), Hoehe %.0f (%+.1f = %+.2f %%), Oberkante %+.1f, Unterkante %+.1f" % (
                n, e["breite"], e["d_breite"], e["d_breite_prozent"], e["hoehe"], e["d_hoehe"], e["d_hoehe_prozent"],
                e["d_oberkante"], e["d_unterkante"]))
        for n in ("-100", "0", "+100"):
            e = E["empfindlichkeit_modell"][n]
            print("  Modell bei Ebene %5s: s %.2f (senkrecht %.2f, waagrecht %.2f)  Rahmenhoehe %.0f  Schildbreite %.0f  Rohr oben Mitte %.0f  Unterkante %.0f" % (
                n, e["s"], e["s_v"], e["s_h"], e["rahmen_hoehe"], e["schild_breite"], e["rohr_oben_mitte"], e["unterkante"]))
        print("== 8b. Gegenueberstellung (u ab Achse hi-Rahmenrohr, v ueber Boden)")
        for g in E["gegenueberstellung"]:
            def f(v):
                return "%7.0f" % v if v is not None else "      -"
            print("  %-4s %-28s Cut0 %s  Cut11 %s  Cut12 %s   Spanne %s  %s" % (
                g["art"], g["name"], f(g["cut0"]), f(g["cut11"]), f(g["cut12"]), f(g["spanne"]), g["hinweis"]))
        print("== 8c. Rohrdurchmesser")
        for r in E["rohrdurchmesser"]:
            if r["cut"] == 12:
                print("  Cut 12 %-28s %.2f px (%.2f..%.2f)  mal s -> %.0f   [mit RDT-Kamera vz %.0f/H %d -> %.0f]  %s" % (
                    r["name"], r["breite_px"], r["von"], r["bis"], r["D"], r["vz_rdt"], r["H"], r["D_rdt"], r["art"]))
            else:
                print("  Cut %2d %-28s %.2f px * vz %.0f / H %d -> %.0f  (%s)" % (
                    r["cut"], r["name"], r["breite_px"], r["vz"], r["H"], r["D"], r["art"]))
        m = E["massstabsanker"]
        print("== 8d. Massstabsanker: Spieler Trefferkasten %s (Hoehe %d), Skelettwurzel y %d; Gelaenderholm oben %.0f, Holm mitte %.0f, Tor-Oberkante %.0f" % (
            m["trefferkasten_roh"], m["trefferkasten_hoehe"], m["skelettwurzel_y"], m["holm_oben"], m["holm_mitte"], m["tor_oberkante"]))
        print("== 9. Bauteile (u ab Achse hi-Rahmenrohr, v ueber Boden; aus Cut 12 mit s = %.2f)" % c12["s"])
        for t in E["bauteile"]["teile"]:
            if t.get("u") and t.get("v") and None not in t["u"] and None not in t["v"]:
                print("  %-22s %-6s u %7.0f .. %7.0f  v %6.0f .. %6.0f   (%.0f x %.0f)" % (
                    t["name"], t["art"], t["u"][0], t["u"][1], t["v"][0], t["v"][1],
                    abs(t["u"][1] - t["u"][0]), abs(t["v"][1] - t["v"][0])))
    pfad = os.path.join(AUS, "vermessung.json")
    with open(pfad, "w", encoding="utf-8") as f:
        json.dump(_rund(E), f, indent=1, ensure_ascii=False)
    if druck:
        print("geschrieben: %s (%d Bilder)" % (os.path.relpath(pfad, WURZEL).replace("\\", "/"), len(E["bilder"])))
    return E


if __name__ == "__main__":
    was = sys.argv[1] if len(sys.argv) > 1 else "alles"
    if was == "alles":
        alles(True)
    else:
        sz = Szene()
        if was == "kamera":
            kamerasaetze(sz, True)
            rdt_modelle(sz, True)
        elif was == "versatz":
            versatz(sz, True)
        elif was == "ebene":
            torebene(sz, versatz(sz, False), True)
        elif was == "cut12":
            v = versatz(sz, False)
            e = torebene(sz, v, False)
            c12 = messung_cut12(sz, messung_cut0(sz, e["zA"], False), messung_cut11(sz, e["zB"], False), e, True)
            einzelteile_cut12(sz, c12, True)
        else:
            print(__doc__)
            sys.exit(2)
