"""SF V5 wie Foto - Koerper (Bauer, 2026-11-08): Schwanz dick und dunkel gruenlich-grau, Rot nur auf Brust und oberem Arm,
Krallen der (W)-linken Hand in Fotogroesse. Basis = EM30_V5k.EMD (V6 Kopf aus dem Original, sha256 99703bbd), Kopf unveraendert.

    python koerper_bau.py [--basis EMD] [--aus DIR] [--json PROT] [--stufen a,b,c] [--name NAME]
        -> DIR/EM30_V5f.EMD (+ EM30_V4.EMD darauf, wenn --name fehlt und alle Stufen gebaut werden)

Nutzer 2026-11-08 (woertlich, bindend): (1) "In der Seitenansicht sieht man auch weder das Auge, noch den dicken schwanz...";
(3) "im original ist der Schanz wie man sieht gräulich und vom rücken aus geschlossen ein zusammenhängendes stück Das direkt I. den
Rücken übergeht. wir haben da wieder diverse komische huckel..." und "dunkel grünlich"; (5) "Auch der rote Fleck oben auf der rechten
Schulter, den gibt es in der Vorlage ganz offensichtlich nicht. In der Vorlage ist es grau wie der Rest..."; (8) "Die roten Stellen
sind korrekt auf der brust und am oberen linken Arm sonst dominiert im original eher das grau."; (9) "Und die klauen der linken Hand
sieht irgendwie falsch aus und zu klein." mit Klarstellung "gemeint ist die kleine Klaue auf der linken Hand des Wesens."

Kein RE-Befund: die Sichel-Form gibt es auf keiner Disc; keine Zahl hier ist eine @0x-Konstante. Masse = Bildmessung an den Fotos
(SF_MESSBLATT_FOTO.md, Commit a909bd2cd) und am Modell (v4_gemein.zeichne).

Stufe a) Schwanz (Meshes 16-22): Ringe nach schwanz_form.setzen mit den Radien SCHWANZ_PARAM (schwanz_suche.py, Protokoll im
   Dossier); CLUT-Feld aller Schwanzflaechen -> Zeile 487. Zeile 487 = Kopie der Zeile 480 (Referenz 2), darin jeder Index, den die
   Schwanztexel tragen, durch den Eintrag der Sichel-Zeile 484 (Referenz 2, Mesh 15; Messblatt 5: Sichel 19,2 / -0,9 / 2,7) mit
   demselben Helligkeitsrang ersetzt (Rang der Paletten-L* unter den Schwanz-Indizes -> gleicher Quantil unter den Sichel-Indizes,
   15-Bit-Wert bytegleich aus Zeile 484). Texel bytegleich.
Stufe b) Rot nur Brust und oberer Arm (Messblatt 4.3/4.4: "oberer linker Arm" = Bild-links in Foto 2 = (W)-rechter Oberarm,
   Knochen 13; "rechte Schulter" = Foto 1 Bild-links = (W)-rechts, Flaechen 13/d30, d32, d33 und 8/v24, 8/v26):
   * Zeilen 488 = Kopie Referenz-2-Zeile 481, 489 = Kopie 480, 490 = Kopie 482 (bytegleich).
   * Rot behalten (CLUT-Feld -> Kopie): Mesh 8 Flaechen der Brust (f_analyse.teil am Flaechenschwerpunkt, ausser 8/v26), Mesh 13
     alle Flaechen ausser der Schulterkuppe (13/d30, d32, d33 und jede Flaeche von 13, deren in P1 sichtbare Pixel zu >= 50 % in der
     Foto-1-Region Schulter Bild-links x 296-355 y 175-250 liegen, kuppe_13), Mesh 12 (Klauenhand, im Foto rote Handflaeche, Messblatt 6).
   * In den Zeilen 480, 481, 482 wird jeder rote Eintrag (Paletten-a* >= ROT_A) grau: L* gleich (nach der 5-Bit-Rundung nie heller),
     a*/b* = graue Rumpfhaut im Foto 2 (Messblatt 5: 23,8 / 3,9 / 1,5; "Grau der umgebenden Haut" wie im Foto - der
     Median der grauen Paletten-Eintraege, Zeile 480 a*/b* 3,1 / 5,2, ergab im Bild ein Braun, Leiste lt/20 Lauf 1); 5-Bit-Wert = naechster
     Nachbar (+-2 je Kanal), der nicht heller ist; C* bleibt <= P90 der grauen Eintraege (keine neue Buntheit, Riegel T-K1d). Alpha-/
     STP-Bit unveraendert, Index 0 / schwarze Eintraege bleiben.
   Texel bytegleich; Mehrfachnutzung der Texel ist damit egal, die Zeile haengt an der Flaeche.
Stufe c) Krallen (W)-links (Mesh 12, Klauenhand): je Kralle (f_krallen.krallen, Flaechen mit allen Ecken y >= 330) die Ecken um die
   Krallenwurzel (Ecke mit kleinstem y) gestreckt, Faktor = Soll-Laenge / projizierte Laenge in Pose P1 (Foto 1). Soll = 0,036 H
   (Messblatt 6: heller Krallenkeil 12 x 11 px in n1476 = 0,040 x 0,036 H1; Laenge = Hoehe des Keils in Zeigerichtung), nie kuerzer
   als heute. Flaechen, UV, Normalen, Textur unveraendert.
"""
import os, sys, json, argparse, hashlib, copy
import numpy as np
from skimage.color import rgb2lab, lab2rgb
HIER = os.path.dirname(os.path.abspath(__file__))
R41 = os.path.dirname(HIER)
sys.path.insert(0, HIER); sys.path.insert(0, os.path.join(R41, 'v6kopf')); sys.path.insert(0, os.path.join(R41, 'foto'))
import kopf_orig_bau as KB          # noqa: E402
import schwanz_form as SF            # noqa: E402

BASIS = os.environ.get('RE15_V5F_BASIS', os.path.join(KB.AUS, 'EM30_V5k.EMD'))      # sha256 99703bbd (V6 Kopf)
AUS = KB.AUS
SCHWANZ = tuple(range(16, 23))
Z_SCHWANZ, Z_481, Z_480, Z_482 = 487, 488, 489, 490
ROT_A, GRAU_A = 10.0, 6.0
GRAU_AB = (3.9, 1.5)                      # Messblatt 5: graue Rumpfhaut Foto 2 (a* < 6) 23,8 / 3,9 / 1,5
KUPPE_13 = ('d30', 'd32', 'd33')          # Messblatt 4.4: Schulter (W)-rechts (Foto 1), Knochen 13 (dazu kuppe_13(): Lauf 1 zeigte
# mit nur diesen drei Flaechen in der Foto-1-Region "Schulter Bild-links" weiter 33 % Rot aus 13/v11, d18, v6, v3, d3, d17, d16)
KUPPE_REGION = (296, 175, 355, 250)       # Messblatt 4.1: Foto 1 Schulter/Oberarm Bild-links = (W)-rechts, Foto rot 0,5 %
KUPPE_ANTEIL, KUPPE_PX = 0.5, 8
SCHULTER_8 = ('v24', 'v26')               # Messblatt 4.4: Schulter (W)-rechts (Foto 1), Knochen 8
KRALLE_SOLL_H = 0.036                     # Messblatt 6: Keil 11 px hoch / H1
# Schwanz-Radien (schwanz_suche.py Lauf 3, Saat 9, 600 Schritte, Start 281,275,200,170,66,72,0,35,1,2,0,03, V5F_BA_MAX 2,0,
# V5F_BO_MAX 0,1; Protokoll schwanz_suche_s9.json): Gelenk 0..7 + bauch, Datei schwanz_param_v5f.json
def _param():
    j = json.load(open(os.path.join(HIER, 'schwanz_param_v5f.json')))
    return {int(k) if k.isdigit() else k: v for k, v in j.items()}


SCHWANZ_PARAM = _param()


def sha(b):
    return hashlib.sha256(b).hexdigest()


def lut8(v):
    v = np.asarray(v, np.int64)
    return np.stack([(v & 31) * 8, ((v >> 5) & 31) * 8, ((v >> 10) & 31) * 8], -1)


def lab15(v):
    return rgb2lab(lut8(v).reshape(-1, 1, 3) / 255.0).reshape(-1, 3)


def zu15(lab_, stp):
    """Lab -> 15-Bit (5 Bit je Kanal, LUT = Wert x 8), STP-Bit stp: unter den 5-Bit-Nachbarn (+-2 je Kanal) des gerundeten Werts der
    mit dem kleinsten Farbabstand zu lab_, der nicht heller ist als lab_[0] ("gleiche bzw. geringere Helligkeit")."""
    rgb = np.clip(lab2rgb(np.array(lab_, float).reshape(1, 1, 3)).reshape(3), 0, 1) * 255.0
    q0 = np.clip(np.round(rgb / 8.0), 0, 31).astype(int)
    best = None
    for dr in range(-2, 3):
        for dg in range(-2, 3):
            for db in range(-2, 3):
                q = np.clip(q0 + [dr, dg, db], 0, 31)
                v = int(q[0] | (q[1] << 5) | (q[2] << 10))
                ln = lab15(v)[0]
                if ln[0] > lab_[0] + 1e-6:
                    continue
                d = float(np.linalg.norm(ln - np.array(lab_, float)))
                if best is None or d < best[0]:
                    best = (d, v)
    v = best[1] if best else int(q0[0] | (q0[1] << 5) | (q0[2] << 10))
    return v | (stp << 15)


def flaechen_schluessel(m):
    return [('d%d' % i, uv) for i, uv in enumerate(m['tuv'])] + [('v%d' % i, uv) for i, uv in enumerate(m['quv'])]


def setze_zeile(uv, z):
    uv[2] = (z << 6) | (uv[2] & 0x3F)


def schwanz_palette(t, ms):
    """Zeile 487: Kopie 480; Schwanz-Indizes -> Sichel-Eintraege (Zeile 484, Mesh 15) gleichen Helligkeitsrangs."""
    c = t['clut']
    nutz = {}
    for i in list(SCHWANZ) + [15]:
        for _k, uv in flaechen_schluessel(ms[i]):
            mk = KB.raster(uv, 3 if len(uv) == 9 else 4, (uv[5] & 0xF) * 128)
            nutz.setdefault(i, set()).update(int(x) for x in t['pix'][mk])
    sw = sorted(set().union(*[nutz[i] for i in SCHWANZ]))
    si = sorted(nutz[15])
    L_sw = lab15(c[0][sw])[:, 0]; L_si = lab15(c[4][si])[:, 0]
    rang_sw = np.argsort(np.argsort(L_sw, kind='stable'), kind='stable') / max(len(sw) - 1, 1)
    si_sort = [si[k] for k in np.argsort(L_si, kind='stable')]
    neu = c[0].copy()
    zuordnung = []
    for ix, r in zip(sw, rang_sw):
        j = si_sort[int(round(r * (len(si_sort) - 1)))]
        neu[ix] = c[4][j]
        zuordnung.append((int(ix), int(j)))
    return neu, dict(schwanz_indizes=len(sw), sichel_indizes=len(si), zuordnung=zuordnung,
                     lab_alt_p50=[round(float(x), 1) for x in np.median(lab15(c[0][sw]), 0)],
                     lab_neu_p50=[round(float(x), 1) for x in np.median(lab15(neu[sw]), 0)])


def grau_zeile(zeile15):
    """Rote Eintraege (a* >= ROT_A) -> Grau (L* gleich bzw. tiefer, a*/b* = GRAU_AB = graue Rumpfhaut im Foto)."""
    lab_ = lab15(zeile15 & 0x7FFF)
    grau = lab_[(lab_[:, 1] < GRAU_A) & (lab_[:, 0] > 0)]
    ga, gb = GRAU_AB
    neu = zeile15.copy(); liste = []
    for ix in range(len(zeile15)):
        if lab_[ix, 1] >= ROT_A and (zeile15[ix] & 0x7FFF):
            v = zu15((lab_[ix, 0], ga, gb), int(zeile15[ix]) >> 15)
            neu[ix] = v
            ln = lab15(v & 0x7FFF)[0]
            liste.append(dict(index=ix, alt=int(zeile15[ix]), neu=int(v), lab_alt=[round(float(x), 1) for x in lab_[ix]],
                              lab_neu=[round(float(x), 1) for x in ln]))
    return neu, dict(grau_ab=[round(ga, 2), round(gb, 2)], grau_eintraege=int(len(grau)), geaendert=liste)


def kuppe_13(blob):
    """Schulterkuppe von Knochen 13 aus dem Bild: Flaechen von Mesh 13, deren in Pose P1 (Foto 1) sichtbare Pixel zu >= KUPPE_ANTEIL
    in der Messblatt-Region KUPPE_REGION liegen (mindestens KUPPE_PX Pixel dort), dazu KUPPE_13 (Messblatt 4.4)."""
    import v6_gemein as V
    import f_rast as FR
    mo = V.modell(blob)
    A = FR.mit_bg(mo, V.posen()['P1'])
    bone, fl = A['bone'][:480], A['flaeche'][:480]
    x0, y0, x1, y1 = KUPPE_REGION
    ges, inn = {}, {}
    for y, x in zip(*np.nonzero(bone == 13)):
        k = fl[y, x]; ges[k] = ges.get(k, 0) + 1
        if x0 <= x <= x1 and y0 <= y <= y1:
            inn[k] = inn.get(k, 0) + 1
    return sorted(set(KUPPE_13) | {k for k, n in inn.items() if n >= KUPPE_PX and n / ges[k] >= KUPPE_ANTEIL}),         {k: [inn.get(k, 0), ges[k]] for k in ges if inn.get(k, 0)}


def rot_behalten(ms, kuppe=KUPPE_13):
    """Liste (mesh, schluessel, alte Zeile, neue Zeile) der Flaechen, die die Referenz-2-Palette behalten."""
    import f_analyse as FA
    aus = []
    for mi in (8, 12, 13):
        m = ms[mi]
        P = np.array([v[:3] for v in m['v']], float)
        for (k, uv), f in zip(flaechen_schluessel(m), m['tf'] + m['qf']):
            z = uv[2] >> 6
            if mi == 8:
                if k in SCHULTER_8 or FA.teil(8, P[list(f[1::2])].mean(0)) != 'Brust':
                    continue
            if mi == 13 and k in kuppe:
                continue
            ziel = {480: Z_480, 481: Z_481, 482: Z_482}.get(z)
            if ziel is not None:
                aus.append((mi, k, z, ziel))
    return aus


def krallen_strecken(ms, blob_pack):
    """Stufe c: Krallen von Mesh 12 auf KRALLE_SOLL_H (Pose P1)."""
    import v6_gemein as V
    import f_krallen as FK
    import f_analyse as FA
    G = V.G
    mo = V.modell(blob_pack(ms))
    pose = V.posen()['P1']
    _img, _ids, iv = G.zeichne(mo, pose, voll=True)
    Hp = G.h_port(iv)[2]
    m = ms[12]
    P = np.array([v[:3] for v in m['v']], float)
    liste = []
    for Q in FK.krallen(mo):
        tip = Q[np.argmax(Q[:, 1])]; root = Q[np.argmin(Q[:, 1])]
        pp = FA.welt(mo, pose, 12, [root, tip])
        L = float(np.linalg.norm(pp[1] - pp[0])) / Hp
        s = max(1.0, KRALLE_SOLL_H / L)
        qs = set(map(tuple, Q.astype(int)))
        idx = [i for i in range(len(P)) if tuple(P[i].astype(int)) in qs]
        for i in idx:
            m['v'][i][:3] = [int(round(x)) for x in root + s * (P[i] - root)]
        liste.append(dict(wurzel=root.astype(int).tolist(), spitze_alt=tip.astype(int).tolist(),
                          spitze_neu=[int(round(x)) for x in root + s * (tip - root)], ecken=len(idx), laenge_alt_H=round(L, 4),
                          faktor=round(s, 4)))
    return dict(soll_H=KRALLE_SOLL_H, H_port_P1=int(Hp), krallen=liste)


def bauen(basis=BASIS, stufen='abc', schwanz_param=None):
    blob = open(basis, 'rb').read()
    d, _do, ms, tim = KB.teile(blob)
    t = KB.tim_teile(tim)
    prot = dict(basis=basis, basis_sha256=sha(blob), stufen=stufen)
    assert t['clut'].shape[0] == 7, 'Basis hat %d CLUT-Zeilen (erwartet 7 = V5k)' % t['clut'].shape[0]
    pack = lambda m: KB.packen(blob, d, m, tim)
    ref = t['clut'].copy()
    extra = []
    if 'a' in stufen:
        par = schwanz_param or SCHWANZ_PARAM
        if par:
            r_alt = SF.radien_lesen(ms)
            ms = SF.setzen(ms, par)
            prot['schwanz_form'] = dict(radien_alt={str(k): round(v, 1) for k, v in r_alt.items()},
                                        radien_neu={str(k): v for k, v in par.items()})
        z487, prot['schwanz_palette'] = schwanz_palette(t, ms)
        extra.append(z487)
        for i in SCHWANZ:
            for _k, uv in flaechen_schluessel(ms[i]):
                setze_zeile(uv, Z_SCHWANZ)
    else:
        extra.append(ref[0].copy())
    if 'b' in stufen:
        extra += [ref[1].copy(), ref[0].copy(), ref[2].copy()]          # 488 = 481, 489 = 480, 490 = 482
        kuppe, prot['kuppe_13_pixel'] = kuppe_13(blob)
        prot['kuppe_13'] = kuppe
        beh = rot_behalten(ms, kuppe)
        for mi, k, z, ziel in beh:
            for kk, uv in flaechen_schluessel(ms[mi]):
                if kk == k:
                    setze_zeile(uv, ziel)
        prot['rot_behalten'] = [dict(mesh=mi, flaeche=k, zeile_alt=z, zeile_neu=ziel) for mi, k, z, ziel in beh]
        prot['grau'] = {}
        for zi, z in ((0, 480), (1, 481), (2, 482)):
            t['clut'][zi], prot['grau'][str(z)] = grau_zeile(ref[zi])
    t['clut'] = np.vstack([t['clut']] + [e[None] for e in extra])
    tim_neu = KB.tim_packen(t)
    if 'c' in stufen:
        prot['krallen'] = krallen_strecken(ms, lambda m: KB.packen(blob, d, m, tim_neu))
    neu = KB.packen(blob, d, ms, tim_neu)
    prot['tim'] = dict(clut_zeilen=int(t['clut'].shape[0]), port_texturhoehe=int(t['ih'] * t['clut'].shape[0]))
    prot['emd'] = dict(bytes=len(neu), sha256=sha(neu))
    return neu, prot


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--basis', default=BASIS); ap.add_argument('--aus', default=AUS); ap.add_argument('--json')
    ap.add_argument('--stufen', default='abc'); ap.add_argument('--name'); ap.add_argument('--param', help='JSON mit Schwanz-Radien')
    a = ap.parse_args()
    par = None
    if a.param:
        j = json.load(open(a.param))
        par = {int(k) if k.isdigit() else k: v for k, v in j.items()}
    neu, prot = bauen(a.basis, a.stufen, par)
    name = a.name or 'EM30_V5f.EMD'
    os.makedirs(a.aus, exist_ok=True)
    open(os.path.join(a.aus, name), 'wb').write(neu)
    if not a.name and a.stufen == 'abc':
        open(os.path.join(a.aus, 'EM30_V4.EMD'), 'wb').write(neu)
    print('geschrieben %s (%d Byte, sha256 %s)' % (os.path.join(a.aus, name), len(neu), prot['emd']['sha256']))
    print('TIM', prot['tim'])
    if a.json:
        json.dump(prot, open(a.json, 'w'), indent=1, default=lambda o: o.tolist() if hasattr(o, 'tolist') else str(o))


if __name__ == '__main__':
    main()
