"""SF V8 Riegel S1-S8 (Messblatt V8 Abschnitt 7, analysis/befunde_runde41/SF_MESSBLATT_V8.md, Commit 09b04bc1c).

    python riegel_v8.py [EMD] [--v7 EMD_V7] [--v5f EMD_V5F] [--szene debug.log] [--aus TXT] [--json JSON]
        Exit 0 = alle Riegel gruen, 1 = mindestens einer rot. Standard-EMD = Spiel-Asset EM30_HANGAR.EMD dieses Baums.

Pruefung in PV (Clip 17 Bild 47 Gier 587 x -8246 z -21266), Spielkamera 50A0 Cut 12, RDT-Licht (f_rast.mit_bg, Hintergrund BSS).
Kein RE-Befund; Schwellen = Messblatt V8 Abschnitt 7 (Nutzer-Skizze Darstellung.png, Vorlage n1439). Vorlage-Daten (Stationen T,
Figur-Maske, Oberkontur, Nutzerlinien) aus SF_belege/v8/messung/daten (nur gelesen).

S1 Kontur T     Aussenkante Modell entlang der 22 T-Normalen gegen die Vorlage-Kante (T + 11 px, wo nicht messbar): Mittel <= 4, Max <= 8
S2 Uebergang J  (a) Oberkontur x 330-385 gegen die Vorlage-Maske Mittel <= 4, Max <= 6; (b) Sattel der Modell-Oberkontur unter ihrer
                oberen Huelle x 300-420 <= 2; (c) Rumpf ueber der Schwanz-Oberkante an der Wurzel <= 3
S3 Ruecken/H/K  (a) Kuppenprominenz <= 4; (b) Huegel H 5-10 px ueber der Sehne x 405 -> 447 bei x 428-445; (c) keine weitere Erhebung
                >= 3 px in x 330-460; (d) Sattel Ruecken -> Kopf (x 380-447) <= 2; (e) Kopf-Vorderkante y 212-244 <= 3 px von der Geraden
S4 Breite       Ringdurchmesser in PV quer zur Achse: Wurzel >= 55, s 90-130 >= 36, monoton fallend (+-2), Ende <= 10
S5 Gesicht      Mesh 9 (Ecken, Normalen, Flaechen, UV, CLUT-Feld), Zeile 486, alle Mesh-9-Texel bytegleich V7
S6 Boden        Spitze + Dornspitze Mesh 22 in 0 von allen Keyframes unter der tiefsten Koerperecke; Szene (--szene) alle Gelenke y <= 0
S7 Adern        (a) Herkunft der Eintraege und Texel; (b) Oberarm 13 35-59 % a* >= 10; (c) Rumpf 8 lok x < 0 25-80 %; (d) Ruhe
S8 Technik      CLUT-Zeilen <= 16, 256 x Zeilen <= 4096, CLUT-Felder gueltig, 23 Meshes, 23 Knochen, 21 Clips
Runde 3 (Vorgaben O1-O6 des Orchestrators nach Pruefung V8 r2; Gegenproben V7 und V8 r2 muessen rot sein):
S3 (f) Huegel rot: oberste 8 px der Figur x 428-445 (PV, RDT-Licht, Videoraum) Anteil a* >= 10 >= 40 % (Vorlage 58,3 %, Zahlen-Pruefer r2)
S4 (b) unten: Ringdurchmesser bei s 140-160 >= 27 px (Vorlage s 140 33 px, Zeile y 270 ~30 px), bei s 170-200 >= 9 px (Vorlage s 200 9 px)
S6 (c) Szene (--szene): groesster Gelenkwinkel der Kette in allen Takten <= 25,5 Grad (Bauplan 6a 25 Grad + 12-Bit-Rundung; O6)
S7 Runde 3: (a) adern_v8.herkunft_r3 (jeder neue Eintrag = Original-EM30-Zeile, jeder Texel unter den Flaechen der neuen Zeilen = Original-
                Texel, kein Feld C); (b) Oberarm 13 >= V7 (22,8 %, freigegebener Stand) und <= Original-EM30 gleiche Pose; (c) Rumpf 8 (W)-rechts
                (lok z < 0, Achsenkorrektur Dossier V8) 25-80 % (Vorlage 76,3 %) und Unterarm 14 <= Original; (d) rote 8er-Flecken unter weissem
                Licht auf den Flaechen der neuen Zeilen (ohne Huegel), je 1000 sichtbare Bildpunkte, <= Original-EM30 auf DENSELBEN Flaechen; das
                Bild des V8 vorher auf die Flaeche des Originals verkleinert (gleiche Aufloesung je Modelleinheit).
                Gemessen und nur ausgegeben (Dossier V8 Runde 3, Offen): Oberarm im Vorlage-Band 35-59 % ist ohne Feld C nicht erreichbar (der
                Rotanteil des Original-Oberarms 57,7 % ist der Augapfel); Rumpf "<= Original gleiche Pose" (49,3 %) misst bei gleicher Textur je
                Flaeche die andere Geometrie mit (V7-Rumpf x 1,25, in PV 3826 statt 1963 Bildpunkte (W)-rechts).
S9 Vorderansicht (O3): Oberkontur Knochen 0-15 x 395-515 in P1 (Foto 1) und Szene Takt F344/F354/F364 hoechstens 2 px ueber V7, keine neue
                konvexe Ecke (r3_gemein.spitzen: >= 2,5 px ueber dem Mittel der Nachbarn bei +-4, nicht in V7 bei +-3 Spalten)
S10 Wurzel von hinten (O5): Szene Takt F320-F332 (Clip 11, Birkin von hinten), Maler-Reihenfolge wie im Spiel (f_rast): 0 sichtbare Bildpunkte
                der Wurzelflaechen von Mesh 16 (Kegel d11-d21 und Rohr v0-v10 = Flaechen ohne Ecke auf Ring 2)
Runde 4 (Befunde der Pruefung V8 r3, SF_PRUEFUNG_V8_r3_zahlen.md / _sicht.md; Gegenproben V7, V8 r2 und V8 r3 muessen rot sein):
S7 Runde 4 (O1-Wortlaut; Zahlen-Pruefer r3: das Band war "zu nur Info abgeschwaecht"): (a) adern_r4.herkunft_r4 (zusaetzlich: kein Texel unter
                einer Flaeche der neuen Zeilen ausserhalb der Texel, die Original-Flaechen derselben Zeile benutzen; geaenderte UV nur auf neuen
                Zeilen); (b) Oberarm 13 im Vorlage-Band 35-59 % UND <= Original-EM30 gleiche Pose; (c) Rumpf 8 (W)-rechts (lok z < 0) 25-80 % UND
                <= Original gleiche Pose, Unterarm 14 <= Original; (d) rote Flecken (a* >= 15, >= 4 px, weisses Licht, PV, Knochen 8 (W)-rechts
                + 13 und 8 (W)-rechts + 13 + 14) je 1000 sichtbare Bildpunkte <= Original-EM30 (gleiche Messung, ohne Verkleinern)
S9 (b) Schlitz (Zahlen-Pruefer r3 O3: 807 Loch-Pixel in F344-F378, "Offline in P1 sind es 0 px"): r4_gemein.schlitz in P1 und Szene Takt
                F336-F372 (jeder zweite) - Bildpunkte, die Knochen 0-15 ohne Rueckseiten-Culling belegen und mit nicht, und die V7 dort nicht hat: 0
S10 Runde 4: alle Takte F314-F345 (Framedump F320-F351; Zahlen-Pruefer: "Der Bauer misst nur Takt F320-F332")
S11 Kappe/Kacheln (Sicht-Pruefer r3 (2)): (a) kraeftig rote Rumpfpixel (a* >= 30, Knochen 8, RDT-Licht) ueber der Kopfoberkante + 5 in P1 und
                Szene Takt F354 <= KAPPE_SOLL (V7 0, Original 0, V8 r3 306 / 214); (b) groesster zusammenhaengender Fleck a* >= 30 auf Knochen 8 von
                hinten (Takt F328 / F332) <= Original-EM30
S12 Unterarmband (Sicht-Pruefer r3 (3): "Knochen 14 in P2 hat 384 rote px (y 299-367), genau wie V8 r2 (384), V7 hat 0"): rote Bildpunkte
                (r - g > 35 und r - b > 25, RDT-Licht) auf Knochen 14 in P2 <= V7 + BAND_REST
S13 Naht (Sicht-Pruefer r3 (3): "harte Farbnaht rosa/grau ... Das Original ist rundum einheitlich"): keine Flaeche von Mesh 8 ausser der Brust
                (Zeile 488) auf einer V7-Zeile 480/481 - der Rumpf traegt rundum die Original-Zeilen
Runde 5 P4 (Nutzer 2026-11-09 nach der V8-Vorschau: "Die Unterseite des großen Schwanzes ist fleischig wenn man in die Vorlagen Bilder schaut.
                Nur die oberseite ist durchweg grau."; Messung und Regel unterseite_r5.py, Messblatt V8 Abschnitt 10; Gegenprobe V8 r4 muss rot sein):
S15 Unterseite  (a) Herkunft: jede Schwanzflaeche ausserhalb Zeile 487 auf der Zeile = Original-Zeile 482, Seite 2, Texel = Original und von
                Original-Flaechen der Zeile 482 benutzt (0 fremde Texel); (b) jede Unterseiten-Flaeche (Mittel |theta| < 130) auf dieser Zeile, die
                Fleischflaechen (Mittel < 100) mit Fleisch-Texeln (a* >= 15), jede Oberseiten-Flaeche auf Zeile 487 mit der UV des Baus ohne Stufe u;
                (c) PV und P2: Fleisch-Texel-Anteil an den Schwanz-Bildpunkten |theta| <= 100 >= S15_SOLL, |theta| >= 135 = 0; (d) groesster L*-Sprung
                zwischen 15-Grad-Baendern theta 90-180 <= Vorlage
Szene (Optionen --mk-a/--mk-b/--gr/--gr-v7, echte Framedumps mit Marker-Assets, szene_marker.py): S10s Wurzel F328-F346 = 0; S9s Schlitz
                F344-F378 Ueberschuss gegen V7 <= SCHLITZ_SZENE_SOLL
"""
import os, sys, json, argparse
import numpy as np
from scipy import ndimage
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import v8_gemein as V                 # noqa: E402
import kopf_orig_bau as KB            # noqa: E402
import birkin_formen as BF            # noqa: E402
import emd_ansichtsblatt as EA        # noqa: E402
import f_rast as FR                   # noqa: E402
import v4_gemein as G                 # noqa: E402
import r3_gemein as R3                # noqa: E402

S7_ADER_A, S7_ADER_L = 15.0, 38.0     # Messblatt 6c: Ader-Eintrag a* >= 15, L* <= 38
S15_SOLL = 30.0                       # = unterseite_r5.S15_UNTEN_SOLL (Vorlage theta 70-110 a* >= 10: PV 25,7-48,8 %, P2 15,6-31,5 %; V8 r4 0 %)


# ------------------------------------------------------------------ Render
def render(blob, pose=V.PV, licht=True):
    mo = V.modell(blob)
    A = FR.mit_bg(mo, pose, licht=licht)
    return mo, A


def oben(bone, x0, x1, ausser=()):
    o = {}
    for x in range(x0, x1 + 1):
        ys = np.nonzero((bone[:, x] >= 0) & ~np.isin(bone[:, x], list(ausser)))[0]
        if len(ys):
            o[x] = int(ys.min())
    return o


def huckel(prof, prom=3, w=25):
    ks = sorted(prof); h = -np.array([prof[k] for k in ks], float)
    H = []
    for i in range(len(ks)):
        l = h[max(0, i - w):i]; r = h[i + 1:i + 1 + w]
        if len(l) == 0 or len(r) == 0:
            continue
        if h[i] >= l.max() and h[i] > r.max():
            p = min(h[i] - l.min(), h[i] - r.min())
            if p >= prom:
                H.append(dict(x=int(ks[i]), y=int(-h[i]), prom=round(float(p), 1)))
    return H


def sattel(prof, a, b):
    xs = np.array([x for x in range(a, b + 1) if x in prof]); ys = np.array([prof[x] for x in xs], float)
    pts = sorted(zip(xs, -ys)); Hh = []
    for p in pts:
        while len(Hh) >= 2 and (Hh[-1][0] - Hh[-2][0]) * (p[1] - Hh[-2][1]) - (Hh[-1][1] - Hh[-2][1]) * (p[0] - Hh[-2][0]) >= 0:
            Hh.pop()
        Hh.append(p)
    hull = np.interp(xs, [h[0] for h in Hh], [-h[1] for h in Hh]); dep = ys - hull
    i = int(np.argmax(dep))
    return round(float(dep.max()), 1), int(xs[i])


def huegel(prof, a, b):
    xs = np.arange(a, b + 1); ys = np.array([prof[x] for x in xs], float); ch = np.interp(xs, [a, b], [ys[0], ys[-1]])
    h = ch - ys; i = int(np.argmax(h))
    return round(float(h.max()), 1), int(xs[i])


# ------------------------------------------------------------------ S1-S4
def s1(bone):
    st = V.stationen()
    e = V.aussenkante(bone >= 0, st)
    d = [(x - s['ziel']) if x is not None else 99.0 for x, s in zip(e, st)]
    m, mx = float(np.mean(np.abs(d))), float(np.max(np.abs(d)))
    return dict(mittel=round(m, 1), max=round(mx, 1), d=[round(x, 1) for x in d], ok=m <= 4.0 and mx <= 8.0)


def s2_s3(mo, bone):
    ov = {int(k): v for k, v in V.lade('m13b_oben.json')['oben_vorlage_maske'].items()}
    om = oben(bone, 280, 470)
    da = [om[x] - ov[x] for x in range(330, 386) if x in om and x in ov]
    a = dict(mittel=round(float(np.mean(np.abs(da))), 1), max=int(np.max(np.abs(da))))
    b = sattel(om, 300, 420)
    c = j_huckel(mo)
    r2 = dict(a=a, b=b, c=c, ok=a['mittel'] <= 4 and a['max'] <= 6 and b[0] <= 2 and c['huckel_px'] <= 3)
    hk = huckel({x: om[x] for x in range(330, 461) if x in om}, prom=3, w=20)
    kuppe = [h for h in hk if 385 <= h['x'] <= 425]
    k_prom = max([h['prom'] for h in kuppe], default=0.0)
    H = huegel(om, 405, 447)
    weitere = [h for h in hk if not (428 <= h['x'] <= 445) and not (385 <= h['x'] <= 425)]   # Kuppe -> (a), H -> (b)
    sd = sattel(om, 380, 447)
    fm = {}
    for y in range(212, 244):
        xs = np.nonzero(bone[y, 420:500] >= 0)[0]
        if len(xs):
            fm[y] = 420 + int(xs.max())
    ys = np.array(sorted(fm)); xs = np.array([fm[y] for y in ys], float)
    cf = np.polyfit(ys, xs, 1); k_abw = round(float(np.abs(xs - np.polyval(cf, ys)).max()), 1)
    r3 = dict(a_kuppe=k_prom, b_huegel=H, c_weitere=weitere, d_sattel_kopf=sd, e_K=k_abw,
              ok=k_prom <= 4 and 5 <= H[0] <= 10 and 428 <= H[1] <= 445 and not weitere and sd[0] <= 2 and k_abw <= 3)
    return r2, r3, om


S3F_MIN = 40.0                         # Runde 3 (O2): Vorlage 58,3 % (Zahlen-Pruefer r2 huegel_rot.txt); Groessenordnung der Vorlage


def huegel_rot(A):
    """S3 (f): Anteil a* >= 10 (Videoraum wie S7) in den obersten 8 px der Figur, x 428-445 (Zahlen-Pruefer r2 Befund 3)."""
    img, bone = A['img'][:480], A['bone'][:480]
    LM = G.lab(G.videoraum(img))
    v = []
    for x in range(428, 446):
        ys = np.nonzero(bone[:, x] >= 0)[0]
        if len(ys):
            v += [LM[y, x, 1] >= 10 for y in range(ys.min(), ys.min() + 8) if bone[y, x] >= 0]
    return round(float(np.mean(v) * 100), 1)


def j_huckel(mo):
    """S2c (Messblatt 7 "Rumpf ueber der Schwanz-Oberkante an der Wurzel", Werkzeug m26: Rumpf (Knochen 0-15) und Schwanz (16-22) getrennt
    gerastert, PV). Praezisiert (V8-Bau): Naht = rechteste Spalte x 330-420, in der die Schwanz-Oberkante nicht unter der Rumpf-Oberkante
    liegt; Hoehe = groesster Abstand Rumpf-Oberkante ueber der Schwanz-Oberkante in den Spalten Naht +- 15, in denen beide da sind.
    (m26 setzte die Schwanz-Oberkante hinter ihrem rechten Ende als Gerade fort; das misst den im Rumpf liegenden Rohrrand mit - V7 34 px
    bei x 410, Messblatt-Tabelle nennt fuer V7 "6-14" = x 360-405. Diese Fassung misst V7 an der sichtbaren Naht.)"""
    mr = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16))); ms = V.b2.umriss_schnell(mo, V.PV, knochen=list(range(16, 23)))
    top = lambda m, x: int(np.nonzero(m[:, x])[0].min()) if m[:, x].any() else None
    tr = {x: top(mr, x) for x in range(300, 460)}; ts = {x: top(ms, x) for x in range(300, 460)}
    oben = [x for x in range(330, 421) if ts[x] is not None and tr[x] is not None and ts[x] <= tr[x]]
    if not oben:
        return dict(naht_x=None, huckel_px=99.0, bei_x=None)
    xn = max(oben)
    H = [(ts[x] - tr[x], x) for x in range(xn - 15, xn + 16) if ts.get(x) is not None and tr.get(x) is not None]
    h = max(H)
    return dict(naht_x=xn, huckel_px=round(float(h[0]), 1), bei_x=int(h[1]))


def ringe_bild(mo, pose=V.PV):
    """Je Schwanzring (Mesh 16-22, 11 Ecken): Bildmitte, Durchmesser quer zur Gliedachse im Bild (px)."""
    R, T = V.pose_r_t(mo, pose)
    out = []
    for g in range(7):
        b = 16 + g
        m = mo.meshes[b]
        P = np.array([v[:3] for v in m.tv], float)
        Rb = R[b].astype(float) / 4096.0; Tb = T[b].astype(float)
        a0 = V.proj(pose, (np.array([[0, 0, 0], [0, 400, 0]], float) @ Rb.T) + Tb)
        ax = a0[1] - a0[0]; ax /= max(np.linalg.norm(ax), 1e-9); q = np.array([-ax[1], ax[0]])
        nr = len(P) // 11
        for r in range(nr):
            Q = V.proj(pose, (P[r * 11:(r + 1) * 11] @ Rb.T) + Tb)
            w = (Q - Q.mean(0)) @ q
            out.append(dict(glied=b, ring=r, mitte=[round(float(x), 1) for x in Q.mean(0)], d=round(float(w.max() - w.min()), 1)))
    return out


def s4(mo):
    rg = ringe_bild(mo)
    st = V.stationen()
    P = np.array([s['p'] for s in st]); sv = np.array([s['s'] for s in st])
    for r in rg:
        r['s'] = int(sv[np.argmin(np.linalg.norm(P - np.array(r['mitte']), axis=1))])
    # Ringe in Kettenfolge (Endring von k und Anfangsring von k+1 liegen am selben Gelenk: das Gelenk nimmt das Maximum)
    # Wurzel = groesster Ring von Glied 16 (V8: Ring 0 ist das verjuengte Rohrende im Rumpf, v8_bau.wurzel_mesh); die Folge beginnt am
    # Wurzelring (die Ringe davor liegen im Rumpf)
    g16 = [r for r in rg if r['glied'] == 16]
    iw = max(range(len(g16)), key=lambda i: g16[i]['d'])
    folge = [r['d'] for r in rg[iw:]]
    wurzel = g16[iw]['d']
    mitte = [r['d'] for r in rg if 90 <= r['s'] <= 130 and r['glied'] <= 19]
    ende = rg[-1]['d']
    mono = all(b <= a + 2.0 for a, b in zip(folge, folge[1:]))
    u1 = [r['d'] for r in rg if 140 <= r['s'] <= 160]
    u2 = [r['d'] for r in rg if 170 <= r['s'] <= 200]
    unten = dict(s140_160=u1, s170_200=u2, ok=bool(u1) and min(u1) >= 27 and bool(u2) and min(u2) >= 9)     # Runde 3 (O4)
    return dict(wurzel=wurzel, s90_130=mitte, ende=ende, folge=folge, monoton=mono, unten=unten,
                ok=wurzel >= 55 and len(mitte) > 0 and min(mitte) >= 36 and mono and ende <= 10 and unten['ok'], ringe=rg)


# ------------------------------------------------------------------ S5
def s5(blob, ref_blob):
    _d, _o, ms, tim = KB.teile(blob); _d2, _o2, mr, timr = KB.teile(ref_blob)
    t, tr = KB.tim_teile(tim), KB.tim_teile(timr)
    mesh = ms[9] == mr[9]
    z486 = bool(t['clut'].shape[0] > 6 and tr['clut'].shape[0] > 6 and np.array_equal(t['clut'][6], tr['clut'][6]))
    nutz = KB.texel_nutzung([mr[9]])[0]
    texel = bool(t['pix'].shape == tr['pix'].shape and np.array_equal(t['pix'][nutz], tr['pix'][nutz]))
    return dict(mesh9=bool(mesh), zeile486=z486, texel=texel, n_texel=int(nutz.sum()), ok=bool(mesh and z486 and texel))


# ------------------------------------------------------------------ S6
def s6(blob, szene=None):
    mo = V.modell(blob)
    s = mo.skel
    m22 = mo.meshes[22]
    P22 = np.array([v[:3] for v in m22.tv], float)
    dorn = P22[np.argmax(P22[:, 1])]
    L = float(np.linalg.norm(np.array(s.rel[22], float)))
    unter = []
    for k in range(s.kf_count):
        R, T = mo.pose(k)
        y = -1e9
        for b in range(16):
            m = mo.meshes[b]
            if m.tv:
                Pb = np.array([v[:3] for v in m.tv], float)
                y = max(y, float(((Pb @ (R[b].astype(float) / 4096.0).T) + T[b].astype(float))[:, 1].max()))
        R22 = R[22].astype(float) / 4096.0
        sp = T[22].astype(float) + R22[:, 1] * L
        dn = T[22].astype(float) + R22 @ dorn
        if max(sp[1], dn[1]) > y:
            unter.append(k)
    sz = None
    if szene and os.path.exists(szene):
        tief, n = -1e9, 0
        for z in open(szene, errors='replace'):
            if 'Schwanz B y' in z:
                n += 1
                for g in z.split()[6:]:
                    try:
                        tief = max(tief, int(g.split(',')[1]))
                    except (IndexError, ValueError):
                        pass
        import re as _re
        wmax = 0.0
        for z in open(szene, errors='replace'):
            if 'Schwanz B y' in z:
                pts = np.array([tuple(map(int, m.groups())) for m in _re.finditer(r'\d\((-?\d+),(-?\d+),(-?\d+)\)', z)], float)
                if len(pts) == 8:
                    dd = np.diff(pts, axis=0); dd /= np.linalg.norm(dd, axis=1)[:, None]
                    wmax = max(wmax, max(float(np.degrees(np.arccos(np.clip((dd[j] * dd[j + 1]).sum(), -1, 1)))) for j in range(6)))
        sz = dict(takte=n, tiefstes_y=tief, unter_boden=int(tief > 0), gelenk_max_grad=round(wmax, 1))
    ok = not unter and (sz is None or (sz['tiefstes_y'] <= 0 and sz['gelenk_max_grad'] <= 25.5))
    return dict(keyframes=s.kf_count, unter=len(unter), beispiele=unter[:10], szene=sz, ok=ok)


# ------------------------------------------------------------------ S7
def rot_anteile(blob, A=None):
    """Anteil a* >= 10 (Videoraum, RDT-Licht, PV) je Teil: Oberarm 13, Unterarm 14, Rumpf 8 lok z < 0 ((W)-rechts), Rumpf 8 lok x < 0."""
    if A is None:
        A = FR.mit_bg(V.modell(blob), V.PV)
    img, bone, lok = A['img'][:480], A['bone'][:480], A['lok'][:480]
    LM = G.lab(G.videoraum(img))
    f = lambda m: round(float((LM[m][:, 1] >= 10).mean() * 100), 1) if m.any() else 0.0
    return dict(o13=f(bone == 13), u14=f(bone == 14), r8z=f((bone == 8) & (lok[..., 2] < 0)), r8x=f((bone == 8) & (lok[..., 0] < 0)))


def s7_r3(blob, A, ref_blob):
    import adern_v8 as AV
    a = AV.herkunft_r3(blob, ref_blob)
    n = rot_anteile(blob, A)
    o = rot_anteile(BF.lade_em30())
    _d, _o, ms, tim = KB.teile(blob)
    nr = KB.tim_teile(KB.teile(ref_blob)[3])['clut'].shape[0]
    fl = set()
    for b in (8, 13, 14):
        for key, uv in [('d%d' % i, u) for i, u in enumerate(ms[b]['tuv'])] + [('v%d' % i, u) for i, u in enumerate(ms[b]['quv'])]:
            ur = AV.flaeche(KB.teile(ref_blob)[2][b], key)
            if (uv[2] >> 6) & 0x1FF >= 480 + nr and [uv[0], uv[1], uv[3], uv[4]] == [ur[0], ur[1], ur[3], ur[4]]:
                fl.add((b, key))                                   # Flaechen der neuen Zeilen mit Original-UV (ohne Huegel-Schub)
    filt = lambda b, key, zeile: (b, key) in fl
    n_o, px_o, a_o = flecken_auf(V.modell(BF.lade_em30()), filt)
    _n, _p, a_v0 = flecken_auf(V.modell(blob), filt)
    sk = float(np.sqrt(a_o / max(a_v0, 1)))                              # V7-Koerper x 1,2-1,25 groesser als das Original (sfv8/r3 e1.py)
    n_v, px_v, a_v = flecken_auf(V.modell(blob), filt, skal=min(1.0, sk)) if fl else (0, 0, 0)   # V7: keine Flaechen auf neuen Zeilen
    dv, do = 1000.0 * n_v / max(a_v, 1), 1000.0 * n_o / max(a_o, 1)     # je 1000 sichtbare Bildpunkte, gleiche Aufloesung
    d = dict(flaechen=len(fl), flecken_original=n_o, flecken_v8=n_v, rotpx_original=px_o, rotpx_v8=px_v, flaeche_original=a_o, flaeche_v8=a_v,
             dichte_original=round(do, 2), dichte_v8=round(dv, 2), ok=bool(fl) and dv <= do + 1e-9)
    b_ok = V7_OBERARM <= n['o13'] <= o['o13']
    c_ok = 25 <= n['r8z'] <= 80 and n['u14'] <= o['u14']
    return dict(a=a, neu=n, original=o, b_oberarm13=n['o13'], c_rumpf8_lokz=n['r8z'], c_rumpf8_lokx_info=n['r8x'], unterarm14=n['u14'], d=d,
                ok=bool(a['ok'] and b_ok and c_ok and d['ok']))


V7_OBERARM = 22.8                     # Messblatt V8 0 / riegel_v8_V7_gegenprobe.txt: Oberarm 13 in PV beim freigegebenen V7


def flecken_auf(mo, filt, skal=1.0):
    """Rote 8er-Komponenten (>= 4 px, a* >= 15) unter weissem Licht in PV, nur Flaechen mit filt(b, key, zeile); skal < 1: Bild vorher
    flaechentreu verkleinert (cv2.INTER_AREA), damit beide Modelle in derselben Aufloesung je Modelleinheit gezaehlt werden."""
    import cv2
    A = FR.zeichne(mo, V.PV, None, licht=False, filt=filt)
    img, bone = A['img'][:480], A['bone'][:480]
    if skal < 1.0:
        sz = (int(round(640 * skal)), int(round(480 * skal)))
        img = cv2.resize(img, sz, interpolation=cv2.INTER_AREA)
        bone = cv2.resize((bone >= 0).astype(np.uint8), sz, interpolation=cv2.INTER_NEAREST).astype(np.int16) - 1
    Lab = G.lab(img.astype(float))
    rot = (bone >= 0) & (Lab[..., 1] >= 15)
    lab_, n = ndimage.label(rot, structure=np.ones((3, 3)))
    gr = ndimage.sum(rot, lab_, range(1, n + 1)) if n else []
    return int(sum(1 for g in gr if g >= 4)), int(rot.sum()), int((bone >= 0).sum())


def s9(blob, ref_blob):
    """Runde 3 (O3) Vorderansicht: P1 und Szene von vorn, Oberkontur Knochen 0-15 x 395-515 gegen V7."""
    mo, m7 = V.modell(blob), V.modell(ref_blob)
    sp = R3.szene_posen()
    out, ok = [], True
    for nm, pose in [('P1', R3.P1)] + [('F%d' % f, sp[f]) for f in R3.VORN_TAKTE]:
        o7 = R3.oben(V.b2.umriss_schnell(m7, pose, knochen=list(range(16))), 395, 515)
        o = R3.oben(V.b2.umriss_schnell(mo, pose, knochen=list(range(16))), 395, 515)
        ueber = max([o7[x] - o[x] for x in o if x in o7] + [0])
        sp7 = R3.spitzen(o7)
        neu = [x for x in R3.spitzen(o) if not any(abs(x - y) <= 3 for y in sp7)]
        out.append(dict(ansicht=nm, ueber_v7_px=int(ueber), neue_ecken=neu))
        ok = ok and ueber <= 2 and not neu
    return dict(ansichten=out, ok=ok)


def s10(blob):
    """Runde 3 (O5) Wurzel von hinten: sichtbare Bildpunkte der Mesh-16-Flaechen ohne Ecke auf Ring 2 (Kegel + Rohr); Runde 4: Takt F314-F345."""
    mo = V.modell(blob)
    m = mo.meshes[16]
    ring2 = set(range(22, 33)) | {33}
    wurzel = set(['d%d' % i for i, f in enumerate(m.tf) if not (set(f[1::2]) & ring2)] +
                 ['v%d' % i for i, f in enumerate(m.qf) if not (set(f[1::2]) & ring2)])
    sp = R3.szene_posen()
    out, summe = [], 0
    for f in S10_TAKTE:
        A = FR.zeichne(mo, sp[f], None, licht=False)
        b, fl = A['bone'][:480], A['flaeche'][:480]
        n = int(sum(1 for k in fl[b == 16] if k in wurzel))
        out.append(dict(takt=f, px=n)); summe += n
    return dict(takte=out, summe=summe, flaechen=sorted(wurzel), ok=summe == 0)


KAPPE_SOLL = 0
BAND_REST = 20
SCHLITZ_SZENE_SOLL = 20                # 960 x 720, 35 Bilder: < 1 px je Bild Kantenrauschen (V8 r3 381, V8 r2 432, V7 0; sfv8/r4 Pruefer-Laeufe pr3)
S10_TAKTE = tuple(range(314, 346))


def s7_r4(blob, A, ref_blob):
    import adern_r4 as A4
    a = A4.herkunft_r4(blob, ref_blob)
    n = rot_anteile(blob, A)
    o = rot_anteile(BF.lade_em30())
    fv = {k: rote_flecken(blob, knochen=k) for k in ((8, 13), (8, 13, 14))}
    fo = {k: rote_flecken(BF.lade_em30(), knochen=k) for k in ((8, 13), (8, 13, 14))}
    dichte = lambda t: round(1000.0 * t[0] / max(t[2], 1), 2)
    d = dict(v8_13=dict(neu=fv[(8, 13)], dichte=dichte(fv[(8, 13)]), original=fo[(8, 13)], dichte_original=dichte(fo[(8, 13)])),
             v8_13_14=dict(neu=fv[(8, 13, 14)], dichte=dichte(fv[(8, 13, 14)]), original=fo[(8, 13, 14)], dichte_original=dichte(fo[(8, 13, 14)])))
    d['ok'] = all(d[k]['dichte'] <= d[k]['dichte_original'] for k in ('v8_13', 'v8_13_14'))
    b_ok = 35.0 <= n['o13'] <= 59.0 and n['o13'] <= o['o13']
    c_ok = 25.0 <= n['r8z'] <= 80.0 and n['r8z'] <= o['r8z'] and n['u14'] <= o['u14']
    return dict(a=a, neu=n, original=o, b_oberarm13=n['o13'], c_rumpf8_lokz=n['r8z'], c_rumpf8_lokx_info=n['r8x'], unterarm14=n['u14'], d=d,
                b_ok=b_ok, c_ok=c_ok, ok=bool(a['ok'] and b_ok and c_ok and d['ok']))


def s9b(blob, ref_blob):
    import r4_gemein as R4
    sl = R4.schlitz(V.modell(blob), V.modell(ref_blob))
    return dict(ansichten=sl, summe=int(sum(v for _n, v in sl)), ok=sum(v for _n, v in sl) == 0)


def s11(blob):
    import adern_r4 as A4
    mo, o = V.modell(blob), V.modell(BF.lade_em30())
    sp = R3.szene_posen()
    kp = dict(P1=A4.kappe_vorn(mo, R3.P1), F354=A4.kappe_vorn(mo, sp[354]))
    kh = {('F%d' % f): (A4.kachel_hinten(mo, sp[f]), A4.kachel_hinten(o, sp[f])) for f in (328, 332)}
    ok = max(kp.values()) <= KAPPE_SOLL and all(x <= y for x, y in kh.values())
    return dict(kappe=kp, kachel_neu_original=kh, ok=ok)


def band_px(blob):
    A = FR.mit_bg(V.modell(blob), R3.P2)
    b, im = A['bone'][:480], A['img'][:480].astype(int)
    return int(((b == 14) & (im[..., 0] - im[..., 1] > 35) & (im[..., 0] - im[..., 2] > 25)).sum())


def s12(blob, ref_blob):
    n, n7 = band_px(blob), band_px(ref_blob)
    return dict(px=n, v7=n7, ok=n <= n7 + BAND_REST)


def s13(blob):
    _d, _o, ms, _t = KB.teile(blob)
    m = ms[8]
    nt = len(m['tuv'])
    v7 = [('d%d' % i if i < nt else 'v%d' % (i - nt)) for i, uv in enumerate(m['tuv'] + m['quv']) if (uv[2] >> 6) & 0x1FF in (480, 481)]
    return dict(flaechen_v7_zeilen=len(v7), beispiele=v7[:8], ok=not v7)


# ------------------------------------------------------------------ Runde 5 (Pruefung V8 r4)
# S14 Brust von vorn (Sicht-Pruefer r4 (3): "Von vorn liegt rechts neben dem Gesicht ... eine graue Platte (Brust-Zeile 488, V7 nach O1). Sie
#     grenzt jetzt mit gerader harter Kante an die neue rosa Haut ... Darin stehen helle weisse Keile"; gemessen F354 dL 19 / da* 9, V7 dL 4;
#     fast weisse Pixel (L > 75, Chroma < 12) in den echten Framedumps F344-F372 V8 r4 2097 gegen V7 5). Vorgabe des Orchestrators: "Kanten-Sprung
#     Brust/Flanke von vorn (dL, da* entlang der Grenze) hoechstens wie V7 an derselben Stelle; Zahl reinweisser Texel in der Brust 0".
S14_POSEN = ('P1', 344, 354, 364)      # Foto-1-Pose und Szene von vorn (r3_gemein.VORN_TAKTE)
S14_SEG_MIN = 8                        # Grenzstueck = Paar (Brustflaeche, Nachbarflaeche) mit >= 8 Grenz-Bildpunkten (kuerzere: Ecken, Rauschen)
S14_FENSTER = 2                        # 5 x 5-Mittel je Seite (Mass des Sicht-Pruefers r4)
S14_WEISS_TAKTE = tuple(range(336, 373, 2))   # Szene von vorn (adern_r5.VORN_TAKTE)
S14_KANTE_RAND = 1.5                   # Vorgabe "hoechstens wie V7 an derselben Stelle". V7 ist grau an grau; die Original-EM30-Haut mit Adern hat an
                                       # derselben Grenze OHNE Naht (sfv8/r5 vA: Brust und alle Nachbarn mit Original-UV und Original-Zeile) dL
                                       # 3,60 / 3,50 / 2,87 / 3,04 gegen V7 2,18 / 2,30 / 2,20 / 2,11 (P1 / F344 / F354 / F364), also bis +1,42 -
                                       # Adern-Kontrast, kein Sprung. Rand 1,5 = dieser gemessene Unterschied, aufgerundet; da* hat ihn nicht noetig
                                       # (vA 1,8-2,3 < V7 2,9-3,2). Die strenge Lesart (Rand 0) gibt die Zeile "streng" mit aus.
S14_WEISS_RAND = 0                     # fast weisse Bildpunkte auf Knochen 8 von vorn hoechstens wie V7


def brust_flaechen():
    """Die 34 Brustflaechen von Mesh 8 (V7-Zeile 488) - Schluessel aus V7 (Referenz)."""
    import adern_r4 as A4
    _d, _o, ms, _t = KB.teile(open(os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD'), 'rb').read())
    return set(k for k, uv in A4.keys(ms[8]) if (uv[2] >> 6) & 0x1FF == 488)


def s14_pose(p):
    return R3.P1 if p == 'P1' else R3.szene_posen()[p]


def brust_kante(A, brust):
    """Kanten-Sprung Brust/Flanke: je Grenzstueck (Brustflaeche, Nachbarflaeche von Mesh 8, >= S14_SEG_MIN Grenz-Bildpunkte) der mittlere
    Unterschied der 5 x 5-Mittel (Lab, RDT-Licht) innen minus aussen; Rueckgabe laengengewichtetes Mittel |dL| und |da*|, groesstes |dL|.
    Gerade harte Kante = ein ganzes Grenzstueck mit gleichsinnigem Sprung (Platte); Adern-Kontrast mittelt sich entlang des Stuecks heraus."""
    b = A['bone'][:480]; fl = A['flaeche'][:480]
    L = G.lab(A['img'][:480].astype(float))
    br = (b == 8) & np.isin(fl, list(brust)); au = (b == 8) & ~br
    k = np.ones((2 * S14_FENSTER + 1, 2 * S14_FENSTER + 1))

    def mittel(m):
        s_ = np.stack([ndimage.convolve(L[..., c] * m, k, mode='constant') for c in range(3)], -1)
        n_ = ndimage.convolve(m.astype(float), k, mode='constant')
        return s_ / np.maximum(n_, 1)[..., None], n_
    mi, ni = mittel(br); ma, na = mittel(au)
    seg = {}
    H, W = b.shape
    for y, x in zip(*np.nonzero(br)):
        for dy, dx in ((0, 1), (1, 0), (0, -1), (-1, 0)):
            yy, xx = y + dy, x + dx
            if 0 <= yy < H and 0 <= xx < W and au[yy, xx] and ni[y, x] >= 5 and na[y, x] >= 5:
                seg.setdefault('%s|%s' % (fl[y, x], fl[yy, xx]), []).append(mi[y, x] - ma[y, x])
                break
    st = [(k_, len(v), float(np.mean([q[0] for q in v])), float(np.mean([q[1] for q in v]))) for k_, v in seg.items() if len(v) >= S14_SEG_MIN]
    n = sum(t[1] for t in st)
    mx = max(st, key=lambda t: abs(t[2]))
    return dict(px=n, dL=round(sum(t[1] * abs(t[2]) for t in st) / max(n, 1), 2), da=round(sum(t[1] * abs(t[3]) for t in st) / max(n, 1), 2),
                max_dL=round(abs(mx[2]), 1), max_bei=mx[0], stuecke=sorted([t[0] for t in st]))


def weiss_px(A):
    """Fast weisse Bildpunkte (L > 75, Chroma < 12, RDT-Licht; Mass des Sicht-Pruefers r4) auf Knochen 8."""
    b = A['bone'][:480]; L = G.lab(A['img'][:480].astype(float))
    return int(((b == 8) & (L[..., 0] > 75) & (np.hypot(L[..., 1], L[..., 2]) < 12)).sum())


def brust_rot(A, brust):
    """Rote Bildpunkte (a* >= 15, RDT-Licht) auf den Brustflaechen (Nutzer V5: "Die roten Stellen sind korrekt auf der brust")."""
    b = A['bone'][:480]; fl = A['flaeche'][:480]; L = G.lab(A['img'][:480].astype(float))
    return int(((b == 8) & np.isin(fl, list(brust)) & (L[..., 1] >= 15)).sum())


def s14_kante(blob, posen=S14_POSEN, brust=None):
    brust = brust or brust_flaechen()
    mo = V.modell(blob)
    return {str(p): brust_kante(FR.mit_bg(mo, s14_pose(p)), brust) for p in posen}


def s14(blob, ref_blob):
    import adern_r5 as A5
    brust = brust_flaechen()
    mo, m7 = V.modell(blob), V.modell(ref_blob)
    a, c = {}, {}
    for p in S14_POSEN:
        An, A7 = FR.mit_bg(mo, s14_pose(p)), FR.mit_bg(m7, s14_pose(p))
        a[str(p)] = (brust_kante(An, brust), brust_kante(A7, brust))
        c[str(p)] = (brust_rot(An, brust), brust_rot(A7, brust))
    a_ok = all(n['dL'] <= v['dL'] + S14_KANTE_RAND and n['da'] <= v['da'] for n, v in a.values())
    a_streng = all(n['dL'] <= v['dL'] and n['da'] <= v['da'] for n, v in a.values())
    wt = A5.weiss_rest(blob, nur=brust)
    wp = {}
    for p in ['P1'] + list(S14_WEISS_TAKTE):
        pose = s14_pose(p)
        wp[str(p)] = (weiss_px(FR.mit_bg(mo, pose)), weiss_px(FR.mit_bg(m7, pose)))
    b_ok = not wt and sum(n for n, _v in wp.values()) <= sum(v for _n, v in wp.values()) + S14_WEISS_RAND
    c_ok = all(n >= v for n, v in c.values())
    return dict(a_kante=a, b_weiss_texel_brust=wt, b_weiss_px=wp, c_rot=c, a_ok=a_ok, a_streng=a_streng, b_ok=b_ok, c_ok=c_ok,
                ok=bool(a_ok and b_ok and c_ok))


S14S_BILDER = tuple(range(344, 373))   # echte Framedumps F344-F372 (Sicht-Pruefer r4: "in den echten Framedumps F344-F372 im Fenster x 560-690 / y 260-330")
S14S_FENSTER = (560, 690, 260, 330)    # x0, x1, y0, y1 im 960 x 720-Framedump (Sicht-Pruefer r4)
S14S_RAND = 29                         # 29 Bilder zu je hoechstens 1 Bildpunkt Kantenrauschen


def s14s(fd_neu, fd_v7):
    """Fast weisse Bildpunkte (L > 75, Chroma < 12) in den echten Framedumps F344-F372 im Fenster des Sicht-Pruefers r4, neu gegen V7."""
    from PIL import Image
    x0, x1, y0, y1 = S14S_FENSTER
    zahl = []
    for d in (fd_neu, fd_v7):
        n = 0
        for f in S14S_BILDER:
            p = os.path.join(d, 'f%06d.ppm' % f)
            im = np.array(Image.open(p).convert('RGB'))[y0:y1, x0:x1].astype(float)
            L = G.lab(im)
            n += int(((L[..., 0] > 75) & (np.hypot(L[..., 1], L[..., 2]) < 12)).sum())
        zahl.append(n)
    return dict(neu=zahl[0], v7=zahl[1], ok=zahl[0] <= zahl[1] + S14S_RAND)


S2C_VARIANTEN = ((0.375, 0, 0), (0.375, 0.5, 0), (0.375, -0.5, 0), (0.375, 0, 0.5), (0.375, 0, -0.5), (0.0, 0, 0), (0.75, 0, 0))


def s2c_r5(mo, pose=V.PV):
    """S2c Runde 5 (Zahlen-Pruefer r4: "8,0 px bei x 376 ... robust in 7 Halbpixel-Varianten"; sein Rasterer zeichnet mit Rueckseiten-Culling
    und Texel-Transparenz wie f_rast, j_huckel nimmt den Umriss ohne Culling): Rumpf (Knochen 0-15) und Schwanz (16-22) getrennt mit f_rast
    gezeichnet (Culling, Maler-Reihenfolge), Naht = rechteste Spalte x 330-420 mit Schwanz-Oberkante <= Rumpf-Oberkante, Hoehe = groesster
    Abstand Rumpf ueber Schwanz in Naht +- 15. In 7 Varianten: Bildphase 0 / 0,375 / 0,75 und Verschiebung +-0,5 px in x oder y."""
    import birkin_buehne as BB
    orig = BB.abbilden
    out = []
    for off, dx, dy in S2C_VARIANTEN:
        def ab(x, y, M, off=off, dx=dx, dy=dy):
            X, Y_ = orig(np.asarray(x) - BB.PHASE + off, np.asarray(y) - BB.PHASE + off, M)
            return X + dx, Y_ + dy
        BB.abbilden = ab
        try:
            mr = FR.zeichne(mo, pose, knochen=list(range(16)), licht=False)['bone'][:480] >= 0
            ms_ = FR.zeichne(mo, pose, knochen=list(range(16, 23)), licht=False)['bone'][:480] >= 0
        finally:
            BB.abbilden = orig
        top = lambda m, x: int(np.nonzero(m[:, x])[0].min()) if m[:, x].any() else None
        tr = {x: top(mr, x) for x in range(300, 460)}; ts = {x: top(ms_, x) for x in range(300, 460)}
        ob = [x for x in range(330, 421) if ts[x] is not None and tr[x] is not None and ts[x] <= tr[x]]
        if not ob:
            out.append(dict(variante=(off, dx, dy), naht_x=None, huckel_px=99, bei_x=None))
            continue
        xn = max(ob)
        h = max((ts[x] - tr[x], x) for x in range(xn - 15, xn + 16) if ts.get(x) is not None and tr.get(x) is not None)
        out.append(dict(variante=(off, dx, dy), naht_x=xn, huckel_px=int(h[0]), bei_x=int(h[1])))
    return dict(varianten=out, max_px=max(v['huckel_px'] for v in out))


def szene_marker(mk_a, mk_b, gr, gr_v7):
    import szene_marker as SM
    out = {}
    if mk_a and mk_b:
        w, row = SM.wurzel(mk_a, mk_b, 328, 346)
        out['S10s'] = dict(summe=w, bilder=row, ok=w == 0)
    if gr and gr_v7:
        sl, sl7, row = SM.schlitz(gr, gr_v7, 344, 378)
        out['S9s'] = dict(ueberschuss=sl, v7=sl7, ok=sl <= SCHLITZ_SZENE_SOLL)
    return out


def s7(blob, A, ref_blob):
    import adern_v8 as AV
    mo = V.modell(blob)
    a = AV.herkunft(blob, ref_blob)
    img, bone, lok = A['img'][:480], A['bone'][:480], A['lok'][:480]
    LM = G.lab(G.videoraum(img))
    m13 = bone == 13
    m8 = (bone == 8) & (lok[..., 0] < 0)
    m8z = (bone == 8) & (lok[..., 2] < 0)
    b = round(float((LM[m13][:, 1] >= 10).mean() * 100), 1)
    c = round(float((LM[m8][:, 1] >= 10).mean() * 100), 1)
    cz = round(float((LM[m8z][:, 1] >= 10).mean() * 100), 1)
    d = ruhe(blob)
    return dict(a=a, b_oberarm13=b, c_rumpf8_lokx=c, c_rumpf8_lokz_info=cz, d=d,
                ok=a['ok'] and 35 <= b <= 59 and 25 <= c <= 80 and d['ok'])


def rote_flecken(blob_oder_mo, knochen=(8, 13), seite_z=True):
    mo = blob_oder_mo if hasattr(blob_oder_mo, 'skel') else V.modell(blob_oder_mo)
    A = FR.zeichne(mo, V.PV, None, licht=False)
    img, bone, lok = A['img'][:480], A['bone'][:480], A['lok'][:480]
    Lab = G.lab(img.astype(float))
    sel = np.isin(bone, knochen)
    if seite_z:
        sel &= ~((bone == 8) & (lok[..., 2] >= 0))
    rot = sel & (Lab[..., 1] >= 15)
    lab_, n = ndimage.label(rot, structure=np.ones((3, 3)))
    gr = ndimage.sum(rot, lab_, range(1, n + 1)) if n else []
    return int(sum(1 for g in gr if g >= 4)), int(rot.sum()), int(sel.sum())


def ruhe(blob):
    o = BF.lade_em30()
    n_o, px_o, sel_o = rote_flecken(o)
    n_v, px_v, sel_v = rote_flecken(blob)
    return dict(flecken_original=n_o, flecken_v8=n_v, rotpx_original=px_o, rotpx_v8=px_v, ok=n_v <= n_o)


# ------------------------------------------------------------------ S8
def s8(blob):
    mo = V.modell(blob)
    _d, _o, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    zeilen = int(t['clut'].shape[0])
    felder = set()
    for m in ms:
        for uv in m['tuv'] + m['quv']:
            felder.add((uv[2] >> 6) & 0x1FF)
    gueltig = all(480 <= z < 480 + zeilen for z in felder)
    r = dict(zeilen=zeilen, hoehe=int(t['ih'] * zeilen), felder=sorted(felder), gueltig=gueltig, meshes=len(ms),
             knochen=mo.skel.bone_count, clips=len(mo.clips))
    r['ok'] = zeilen <= 16 and r['hoehe'] <= 4096 and gueltig and r['meshes'] == 23 and r['knochen'] == 23 and r['clips'] == 21
    return r


def alles(blob, ref_blob, szene=None, mit_s7=True, marker=None, fd=None):
    mo, A = render(blob)
    bone = A['bone'][:480]
    out = dict(sha256=V.sha(blob))
    out['S1'] = s1(bone)
    out['S2'], out['S3'], _om = s2_s3(mo, bone)
    out['S2']['c_r4_info'] = out['S2']['c']                      # Runde 5: S2c mit Culling und 7 Halbpixel-Varianten (s2c_r5)
    out['S2']['c'] = s2c_r5(mo)
    out['S2']['c']['huckel_px'] = out['S2']['c']['max_px']
    out['S2']['c']['bei_x'] = max(out['S2']['c']['varianten'], key=lambda v: v['huckel_px'])['bei_x'] or -1
    out['S2']['ok'] = (out['S2']['a']['mittel'] <= 4 and out['S2']['a']['max'] <= 6 and out['S2']['b'][0] <= 2 and out['S2']['c']['max_px'] <= 3)
    out['S3']['f_huegel_rot'] = huegel_rot(A)
    out['S3']['ok'] = out['S3']['ok'] and out['S3']['f_huegel_rot'] >= S3F_MIN
    out['S4'] = s4(mo)
    out['S5'] = s5(blob, ref_blob)
    out['S6'] = s6(blob, szene)
    if mit_s7:
        out['S7'] = s7_r4(blob, A, ref_blob)
    out['S8'] = s8(blob)
    out['S9'] = s9(blob, ref_blob)
    out['S9']['b_schlitz'] = s9b(blob, ref_blob)
    out['S9']['ok'] = out['S9']['ok'] and out['S9']['b_schlitz']['ok']
    out['S10'] = s10(blob)
    out['S11'] = s11(blob)
    out['S12'] = s12(blob, ref_blob)
    out['S13'] = s13(blob)
    out['S14'] = s14(blob, ref_blob)
    out['S15'] = s15(blob, ref_blob)
    if fd:
        out['S14s'] = s14s(*fd)
    if marker:
        out.update(szene_marker(*marker))
    return out


def s15(blob, ref_blob):
    """Runde 5 (P4, Nutzer 2026-11-09: "Die Unterseite des großen Schwanzes ist fleischig ... Nur die oberseite ist durchweg grau"):
    unterseite_r5.s15_flaechen (a Herkunft, b Klassen), s15_bild (c), s15_ruhe (d)."""
    import unterseite_r5 as U5
    a = U5.s15_flaechen(blob, ref_blob)
    c = U5.s15_bild(blob)
    d = U5.s15_ruhe(blob)
    return dict(flaechen=a, bild=c, ruhe=d, ok=bool(a['ok'] and c['ok'] and d['ok']))


def zeilen(r):
    z = []
    s = r['S1']; z.append('S1 Kontur T: Mittel %.1f Max %.1f px (Soll <= 4 / <= 8) -> %s' % (s['mittel'], s['max'], 'GRUEN' if s['ok'] else 'ROT'))
    z.append('   d je Station (px, + = Modell weiter innen): %s' % s['d'])
    s = r['S2']; z.append('S2 Uebergang J: (a) Mittel %.1f Max %d (<= 4 / <= 6); (b) Sattel %.1f bei x %d (<= 2); (c) Naht Rumpf ueber Schwanz %d px bei x %d '
                          '(<= 3, groesster Wert der 7 Halbpixel-Varianten %s, mit Culling) -> %s'
                          % (s['a']['mittel'], s['a']['max'], s['b'][0], s['b'][1], s['c']['huckel_px'], s['c']['bei_x'],
                             [v['huckel_px'] for v in s['c']['varianten']] if 'varianten' in s['c'] else '-', 'GRUEN' if s['ok'] else 'ROT'))
    if 'c_r4_info' in s:
        z.append('   S2c Runde 4 (Umriss ohne Culling, nur Info): %.1f bei x %s' % (s['c_r4_info']['huckel_px'], s['c_r4_info']['bei_x']))
    s = r['S3']; z.append('S3 Ruecken/H/K: (a) Kuppe %.1f (<= 4); (b) H %.1f bei x %d (5-10, x 428-445); (c) weitere %s; (d) Sattel Kopf %.1f bei x %d (<= 2); (e) K %.1f (<= 3); (f) Huegel rot %.1f %% (>= %.0f) -> %s'
                          % (s['a_kuppe'], s['b_huegel'][0], s['b_huegel'][1], s['c_weitere'], s['d_sattel_kopf'][0], s['d_sattel_kopf'][1], s['e_K'],
                             s.get('f_huegel_rot', -1), S3F_MIN, 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S4']; z.append('S4 Breite: Wurzel %.1f (>= 55), s 90-130 %s (>= 36), Ende %.1f (<= 10), monoton %s; (b) unten s 140-160 %s (>= 27), s 170-200 %s (>= 9); Folge %s -> %s'
                          % (s['wurzel'], s['s90_130'], s['ende'], s['monoton'], s['unten']['s140_160'], s['unten']['s170_200'], s['folge'],
                             'GRUEN' if s['ok'] else 'ROT'))
    s = r['S5']; z.append('S5 Gesicht: Mesh 9 %s, Zeile 486 %s, Texel %s (%d) -> %s' % (s['mesh9'], s['zeile486'], s['texel'], s['n_texel'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S6']; z.append('S6 Boden: %d von %d Keyframes unter der tiefsten Koerperecke %s; Szene %s -> %s'
                          % (s['unter'], s['keyframes'], s['beispiele'], s['szene'], 'GRUEN' if s['ok'] else 'ROT'))
    if 'S7' in r:
        s = r['S7']
        d1, d2 = s['d']['v8_13'], s['d']['v8_13_14']
        z.append('S7 Adern (Runde 4): (a) %s; (b) Oberarm 13 %.1f %% (Vorlage-Band 35-59 UND <= Original %.1f) %s; (c) Rumpf 8 (W)-rechts lok z<0 '
                 '%.1f %% (25-80 UND <= Original gleiche Pose %.1f) [lok x<0 %.1f, Original %.1f], Unterarm 14 %.1f %% (<= Original %.1f) %s; '
                 '(d) Flecken je 1000 px 8+13 %.2f (%d / %d px; Original %.2f = %d / %d), 8+13+14 %.2f (%d / %d px; Original %.2f = %d / %d) %s -> %s'
                 % (('Herkunft ok %s' % {k: v for k, v in s['a'].items() if k != 'ok'}) if s['a']['ok'] else 'Herkunft FALSCH %s' % s['a'],
                    s['b_oberarm13'], s['original']['o13'], 'ok' if s['b_ok'] else 'ROT', s['c_rumpf8_lokz'], s['original']['r8z'],
                    s['c_rumpf8_lokx_info'], s['original']['r8x'], s['unterarm14'], s['original']['u14'], 'ok' if s['c_ok'] else 'ROT',
                    d1['dichte'], d1['neu'][0], d1['neu'][2], d1['dichte_original'], d1['original'][0], d1['original'][2],
                    d2['dichte'], d2['neu'][0], d2['neu'][2], d2['dichte_original'], d2['original'][0], d2['original'][2],
                    'ok' if s['d']['ok'] else 'ROT', 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S8']; z.append('S8 Technik: %d Zeilen, Hoehe %d, Felder gueltig %s, %d Meshes, %d Knochen, %d Clips -> %s'
                          % (s['zeilen'], s['hoehe'], s['gueltig'], s['meshes'], s['knochen'], s['clips'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S9']; z.append('S9 Vorderansicht: %s (je <= 2 px ueber V7, keine neue Ecke) -> %s'
                          % ('; '.join('%s %d px, neue Ecken %s' % (a['ansicht'], a['ueber_v7_px'], a['neue_ecken']) for a in s['ansichten']),
                             'GRUEN' if s['ok'] else 'ROT'))
    z.append('   S9 (b) Schlitz (Loch-Bildpunkte ohne V7-Gegenstueck, P1 + Szene Takt F336-F372): Summe %d (Soll 0) %s'
             % (r['S9']['b_schlitz']['summe'], [x for x in r['S9']['b_schlitz']['ansichten'] if x[1]]))
    s = r['S10']; z.append('S10 Wurzel von hinten (Takt F314-F345): sichtbare Wurzelflaechen-Bildpunkte Summe %d (Soll 0) %s -> %s'
                           % (s['summe'], ['F%d:%d' % (t['takt'], t['px']) for t in s['takte'] if t['px']], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S11']; z.append('S11 Kappe/Kacheln: (a) a* >= 30 ueber dem Kopf P1 %d, F354 %d (<= %d); (b) groesster Fleck von hinten (neu, Original) %s -> %s'
                           % (s['kappe']['P1'], s['kappe']['F354'], KAPPE_SOLL, s['kachel_neu_original'], 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S12']; z.append('S12 Unterarmband P2: %d rote px auf Knochen 14 (<= V7 %d + %d) -> %s' % (s['px'], s['v7'], BAND_REST, 'GRUEN' if s['ok'] else 'ROT'))
    s = r['S13']; z.append('S13 Naht: Flaechen von Mesh 8 (ohne Brust) auf V7-Zeilen 480/481: %d %s (Soll 0) -> %s'
                           % (s['flaechen_v7_zeilen'], s['beispiele'], 'GRUEN' if s['ok'] else 'ROT'))
    if 'S14' in r:
        s = r['S14']
        z.append('S14 Brust von vorn: (a) Kanten-Sprung Brust/Flanke dL / da* (neu | V7): %s (dL <= V7 + %.1f, da* <= V7) %s; (b) fast weisse Texel unter der Brust %s '
                 '(Soll 0), fast weisse Bildpunkte Knochen 8 von vorn neu %d / V7 %d (<= V7 + %d) %s; (c) rote Bildpunkte auf der Brust (neu | V7) %s '
                 '(>= V7) %s -> %s'
                 % ('; '.join('%s %.2f / %.2f | %.2f / %.2f (max %.1f %s)' % (p, n['dL'], n['da'], v['dL'], v['da'], n['max_dL'], n['max_bei'])
                              for p, (n, v) in s['a_kante'].items()), S14_KANTE_RAND, 'ok' if s['a_ok'] else 'ROT',
                    sum(s['b_weiss_texel_brust'].values()) if s['b_weiss_texel_brust'] else 0,
                    sum(n for n, _v in s['b_weiss_px'].values()), sum(v for _n, v in s['b_weiss_px'].values()), S14_WEISS_RAND,
                    'ok' if s['b_ok'] else 'ROT', '; '.join('%s %d | %d' % (p, n, v) for p, (n, v) in s['c_rot'].items()),
                    'ok' if s['c_ok'] else 'ROT', 'GRUEN' if s['ok'] else 'ROT'))
        z.append('   S14 (a) streng (dL und da* je <= V7, ohne Rand): %s' % ('erfuellt' if s['a_streng'] else 'NICHT erfuellt (dL ueber V7, siehe Rand)'))
    if 'S15' in r:
        s = r['S15']; a, c, d = s['flaechen'], s['bild']['ansichten'], s['ruhe']['ansichten']
        z.append('S15 Schwanz-Unterseite (P4): (a) Herkunft Zeile %s: fremde Texel %d, falsche Zeile/Seite %d; (b) Unterseite %d Flaechen, davon ohne '
                 'Fleisch %d, Fleisch-Texel %d; Oberseite %d Flaechen, Fehler %d; (c) Fleisch-Texel-Anteil |theta| <= 100 / >= 135: %s (Soll >= %.0f %% / 0); '
                 '(d) groesster L*-Sprung theta 90-180 Modell / Vorlage: %s -> %s'
                 % (a['zeile'], a['fremde_texel'], a['falsche_zeile'], a['unten'], a['unten_ohne_fleisch'], a['fleisch_texel'], a['oben'],
                    a['oben_fehler'], '; '.join('%s %.1f / %.1f %%' % (p, v['unten_fleisch'], v['oben_fleisch']) for p, v in c.items()),
                    S15_SOLL, '; '.join('%s %s / %s' % (p, v['sprung_modell'], v['sprung_vorlage']) for p, v in d.items()),
                    'GRUEN' if s['ok'] else 'ROT'))
        z.append('   S15 (d) L*-Profil theta 90-180 (15-Grad-Baender) Modell | Vorlage: %s'
                 % '; '.join('%s %s | %s' % (p, v['modell'], v['vorlage']) for p, v in d.items()))
    if 'S14s' in r:
        z.append('S14s fast weisse Bildpunkte in echten Framedumps F344-F372, Fenster x 560-690 / y 260-330 (960x720): neu %d, V7 %d (<= V7 + %d) -> %s'
                 % (r['S14s']['neu'], r['S14s']['v7'], S14S_RAND, 'GRUEN' if r['S14s']['ok'] else 'ROT'))
    if 'S10s' in r:
        z.append('S10s Wurzel in echten Framedumps F328-F346 (Marker, 960x720): %d (Soll 0) -> %s' % (r['S10s']['summe'], 'GRUEN' if r['S10s']['ok'] else 'ROT'))
    if 'S9s' in r:
        z.append('S9s Schlitz in echten Framedumps F344-F378 (gruene Silhouette, Ueberschuss gegen V7): %d (Soll <= %d) -> %s'
                 % (r['S9s']['ueberschuss'], SCHLITZ_SZENE_SOLL, 'GRUEN' if r['S9s']['ok'] else 'ROT'))
    rot = [k for k in ('S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8', 'S9', 'S10', 'S11', 'S12', 'S13', 'S14', 'S15', 'S10s', 'S9s', 'S14s')
           if k in r and not r[k]['ok']]
    z.append('SUMME: %d von %d gruen%s' % (len([k for k in r if k.startswith('S')]) - len(rot), len([k for k in r if k.startswith('S')]),
                                           ('; ROT: ' + ', '.join(rot)) if rot else ''))
    return z, rot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('emd', nargs='?', default=V.ASSET)
    ap.add_argument('--v7', default=None, help='Referenz V7 (Standard: git show 6a25db27b:EM30_HANGAR.EMD nach SFV8/emd/EM30_V7.EMD)')
    ap.add_argument('--szene'); ap.add_argument('--aus'); ap.add_argument('--json')
    ap.add_argument('--mk-a'); ap.add_argument('--mk-b'); ap.add_argument('--gr'); ap.add_argument('--gr-v7')
    ap.add_argument('--fd', help='Runde 5: Framedump-Ordner der Szene (neu) fuer S14s'); ap.add_argument('--fd-v7', help='Framedump-Ordner der V7-Szene')
    a = ap.parse_args()
    v7 = a.v7 or os.path.join(V.SFV8, 'emd', 'EM30_V7.EMD')
    ref = open(v7, 'rb').read()
    assert V.sha(ref) == V.V7_SHA, 'Referenz V7 %s hat sha256 %s' % (v7, V.sha(ref))
    blob = open(a.emd, 'rb').read()
    mk = (a.mk_a, a.mk_b, a.gr, a.gr_v7) if (a.mk_a or a.gr) else None
    r = alles(blob, ref, a.szene, marker=mk, fd=(a.fd, a.fd_v7) if (a.fd and a.fd_v7) else None)
    z, rot = zeilen(r)
    kopf = ['Riegel V8 (Messblatt V8 Abschnitt 7) fuer %s' % a.emd, 'sha256 %s' % r['sha256'], 'Referenz V7 %s' % V.V7_SHA]
    txt = '\n'.join(kopf + z)
    print(txt)
    if a.aus:
        open(a.aus, 'w', encoding='utf-8').write(txt + '\n')
    if a.json:
        json.dump(r, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))
    sys.exit(1 if rot else 0)


if __name__ == '__main__':
    main()
