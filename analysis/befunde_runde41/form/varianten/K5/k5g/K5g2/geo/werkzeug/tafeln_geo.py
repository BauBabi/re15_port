#!/usr/bin/env python3
"""Tafeln der Geometrie-Stufe K5g2 Runde 2 (roh): 10 Krallen, 11 Streifen, 13 Mulde.

  python tafeln_geo.py <zielordner> [10] [11] [13]

Alle Bilder des Modells sind die Buehne des Ports (re15_port/tools/form_buehne.py) unter weissem Licht mit der Textur von K5e_f1,
ohne Aufhellung. Die Marken stammen aus dem Raster wie im Spiel gemalt (Malreihenfolge nach mittlerer Tiefe, kein Tiefenpuffer)
gegen das Raster nach echter Tiefe. Die Fensterbilder (geo/spiel/) sind echte Spielfenster, nicht nachgebaut.
"""
import os
import sys

sys.dont_write_bytecode = True
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
GEO = os.path.normpath(os.path.join(HIER, ".."))
GEN = os.path.normpath(os.path.join(GEO, "..", "..", "..", "generator"))
sys.path.insert(0, GEN)
import kf_k5g2_geo as G  # noqa: E402

FE, FB = G.FE, G.FB
FORM = os.path.normpath(os.path.join(G.K5, "..", ".."))
TIM0 = open(G.GEO_BASIS_TIM, "rb").read()
E0 = open(G.GEO_BASIS, "rb").read()                                    # K5e_f1 (heute im Spiel)
EG = open(G.GEO_ZIEL, "rb").read()                                     # K5g2 Geometrie
EK = G.geometrie_bytes(stufen=("krallen",))                            # K5e_f1 + neue Krallen, ohne Mulde
EA = open(os.path.join(GEO, "vergleich", "KRALLEN_VERGLEICH_A.EMD"), "rb").read()   # Krallen Vergleich A (nicht gewaehlt)
_fm = FE.lese_emd(open(os.path.join(GEO, "vergleich", "M2_A.EMD"), "rb").read())
_f0 = FE.lese_emd(E0)
for _i in (10, 21, 22):                                                # M2_A.EMD traegt auch alte Krallen: hier nur seine Mulde auf K5e_f1
    _f0["meshes"][_i] = _fm["meshes"][_i]
EM = bytes(G.emd_mit_md1(E0, _f0))                                     # Mulde m2_a (Stand vor Runde 2), sonst K5e_f1
MASKEN2 = np.load(os.path.join(G.K5, "zonen_nutzer", "v2", "krallen_kandidat", "krallen_masken_bild2.npz"))
AN = G.ansichten()
HANDR, HANDL = (18, 19, 20, 29, 31), (13, 14, 15, 28, 30)
KOPF, RUMPF = (22, 23), (10, 21, 16)
GRAU = 20
_MO, _TK, _BI = {}, {}, {}


def modell(emd):
    k = hash(emd)
    if k not in _MO:
        mo = FE.modell(emd, TIM0)
        _MO[k] = (mo,) + tuple(G.posen(mo))
    return _MO[k]


def teilkarte(emd, name, clip, bild, teile=None):
    """(gemalt, nach Tiefe): je Bildpunkt das Teil, das das Spiel malt, und das Teil, das wirklich vorn liegt. -1 = leer."""
    k = (hash(emd), name, clip, bild, teile)
    if k not in _TK:
        mo, cb, R, T = modell(emd)
        kam, lage, gier, H, W, ab = AN[name]
        i = [j for j, (c, b, _k) in enumerate(cb) if c == clip and b == bild][0]
        tris = G.dreiecke_teile(mo, R[i], T[i], lage, gier, kam, teile=teile or tuple(range(len(mo.meshes))))
        r = G.keil_raster(tris, H, W, ab)
        _TK[k] = (r[0], r[1])
    return _TK[k]


def bild(emd, name, clip, bild_):
    k = (hash(emd), name, clip, bild_)
    if k not in _BI:
        kam, lage, gier, _H, _W, _ab = AN[name]
        img, _msk = FB.buehne(emd, TIM0, clip, bild_, lage, gier, kam, licht=None)
        _BI[k] = np.asarray(img)[:, :, :3].copy()
    return _BI[k]


def kasten(karten, teile, rand=10):
    m = np.zeros(karten[0].shape, bool)
    for mt in karten:
        m |= np.isin(mt, teile)
    if not m.any():
        return None
    ys, xs = np.where(m)
    H, W = m.shape
    return (max(int(xs.min()) - rand, 0), max(int(ys.min()) - rand, 0), min(int(xs.max()) + rand + 1, W), min(int(ys.max()) + rand + 1, H))


def zoom_fuer(box, ziel=330, hoechst=6):
    return max(1, min(hoechst, ziel // max(box[2] - box[0], box[3] - box[1], 1)))


def kachel(img, box, f):
    x0, y0, x1, y1 = box
    return cv2.resize(img[y0:y1, x0:x1], ((x1 - x0) * f, (y1 - y0) * f), interpolation=cv2.INTER_NEAREST)


def titel(img, t, h=18, gross=0.42):
    b = max(img.shape[1], cv2.getTextSize(t, cv2.FONT_HERSHEY_SIMPLEX, gross, 1)[0][0] + 10)
    o = np.full((img.shape[0] + h, b, 3), GRAU, np.uint8)
    o[h:, :img.shape[1]] = img
    cv2.putText(o, t, (4, h - 5), cv2.FONT_HERSHEY_SIMPLEX, gross, (255, 255, 255), 1, cv2.LINE_AA)
    return o


def reihe(bl, ts):
    bl = [titel(b, t) for b, t in zip(bl, ts)]
    h = max(b.shape[0] for b in bl)
    aus = []
    for b in bl:
        aus += [np.concatenate([b, np.full((h - b.shape[0], b.shape[1], 3), GRAU, np.uint8)], 0), np.full((h, 4, 3), 255, np.uint8)]
    return np.concatenate(aus[:-1], 1)


def stapel(zs):
    w = max(z.shape[1] for z in zs)
    return np.concatenate([np.concatenate([z, np.full((z.shape[0], w - z.shape[1], 3), GRAU, np.uint8)], 1) for z in zs], 0)


def kopfzeile(z, t):
    return titel(z, t, 24, 0.5)


def schreib(p, img):
    cv2.imwrite(p, cv2.cvtColor(img, cv2.COLOR_RGB2BGR))
    print("geschrieben", p.replace(chr(92), "/"), img.shape, flush=True)


KRALLENFARBE = {13: (150, 150, 150), 18: (150, 150, 150), 14: (255, 60, 60), 19: (255, 60, 60), 15: (60, 255, 60), 20: (60, 255, 60),
                28: (80, 120, 255), 29: (80, 120, 255), 30: (255, 255, 0), 31: (255, 255, 0), 12: (110, 110, 110), 17: (110, 110, 110)}


def karte_bunt(mt):
    o = np.full(mt.shape + (3,), 40, np.uint8)
    o[mt >= 0] = (75, 75, 75)
    for t, c in KRALLENFARBE.items():
        o[mt == t] = c
    return o


def vorlage(n, schrift=False):
    o = cv2.cvtColor(cv2.imread(os.path.join(FORM, "bild%d.png" % n)), cv2.COLOR_BGR2RGB)
    if schrift:
        cs, _h = cv2.findContours(MASKEN2["wasserzeichen"].astype(np.uint8), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cv2.drawContours(o, cs, -1, (255, 0, 255), 1)
    return o


def marken(emd, name, clip, b, A, B, teile=None):
    """Bild mit Marken: rot = Gruppe A gemalt, obwohl Gruppe B vorn liegt; gelb = B gemalt, obwohl A vorn liegt. -> (Bild, rot, gelb) roh."""
    mt, pt = teilkarte(emd, name, clip, b, teile)
    rot = np.isin(mt, A) & np.isin(pt, B)
    gelb = np.isin(mt, B) & np.isin(pt, A)
    v = bild(emd, name, clip, b).copy()
    v[rot] = (255, 0, 0)
    v[gelb] = (255, 255, 0)
    return v, int(rot.sum()), int(gelb.sum())


def fenster(datei):
    return cv2.cvtColor(cv2.imread(os.path.join(GEO, "spiel", datei)), cv2.COLOR_BGR2RGB)


# ------------------------------------------------------------------ Tafel 10 Krallen
def tafel10(ziel):
    zs = []
    blicke = [("bild2", 12, 0, "Bild 2 (Schriftzug violett umrandet)", vorlage(2, True)), ("bild1", 0, 11, "Bild 1", vorlage(1)),
              ("spiel5080_vorn", 2, 0, "Spielkamera Raum 5080 von vorn, Ruhestellung", None),
              ("spiel5090", 27, 20, "Spielkamera Raum 5090, Griff nach der Spielerin", None),
              ("spiel5080", 2, 0, "Spielkamera Raum 5080 von der Seite, Ruhestellung", None)]
    for name, clip, b, t, vor in blicke:
        for seite, teile, emds, namen in (("RECHTS (vom Tier aus)", HANDR, (E0, EG, EA), ("vorher K5e_f1", "nachher K5g2", "Vergleich A (nicht gewaehlt)")),
                                          ("LINKS (vom Tier aus)", HANDL, (E0, EG), ("vorher K5e_f1", "nachher K5g2"))):
            karten = [teilkarte(e, name, clip, b)[0] for e in emds]
            box = kasten(karten, teile)
            if box is None:
                continue
            f = zoom_fuer(box, 260, 4)
            bl, ts = [], []
            if vor is not None:
                bl.append(kachel(vor, box, f))
                ts.append("Video-Standbild")
            for e, n in zip(emds, namen):
                bl.append(kachel(bild(e, name, clip, b), box, f))
                ts.append(n)
            for mt, n in zip(karten, namen):
                bl.append(kachel(karte_bunt(mt), box, f))
                ts.append(n + ": Krallen farbig")
            zs.append(kopfzeile(reihe(bl, ts), "%s - Hand vorn %s  [%s Clip %d Bild %d]" % (t, seite, name, clip, b)))
    # Preis: Krallen quer ueber der Hand -> Malreihenfolge Hand gegen Krallen
    for name, clip, b, t in (("spiel5080_vorn", 27, 7, "Preis der Drehung: Hand rechts wird ueber Krallen gemalt (groesste Stelle)"),
                             ("spiel5080_vorn", 2, 0, "dasselbe in Ruhestellung")):
        bl, ts = [], []
        karten = [teilkarte(e, name, clip, b)[0] for e in (E0, EG, EA)]
        box = kasten(karten, HANDR)
        f = zoom_fuer(box, 300, 4)
        for e, n in ((E0, "vorher K5e_f1"), (EG, "nachher K5g2"), (EA, "Vergleich A")):
            v, r, g = marken(e, name, clip, b, (18,), (19, 20, 29, 31))
            bl += [kachel(bild(e, name, clip, b), box, f), kachel(v, box, f)]
            ts += [n, "rot %d gelb %d Bildpunkte" % (r, g)]
        zs.append(kopfzeile(reihe(bl, ts), "%s  [%s Clip %d Bild %d]  rot = Hand gemalt, Kralle liegt vorn; gelb = umgekehrt" % (t, name, clip, b)))
    # echtes Spielfenster
    for vorher, nachher, name, clip, b, t in (("heute_vorn_r5080.png", "geo_vorn_r5080.png", "spiel5080_vorn", 12, 0, "Echtes Spielfenster Raum 5080 von vorn, Pose wie Bild 2"),
                                              ("heute_pose2_r5090_s11.png", "geo_pose2_r5090_s11.png", "spiel5090", 12, 0, "Echtes Spielfenster Raum 5090, Pose wie Bild 2")):
        for seite, teile in (("RECHTS", HANDR), ("LINKS", HANDL)):
            box = kasten([teilkarte(e, name, clip, b)[0] for e in (E0, EG)], teile, 14)
            if box is None:
                continue
            f = zoom_fuer(box, 300, 4)
            zs.append(kopfzeile(reihe([kachel(fenster(vorher), box, f), kachel(fenster(nachher), box, f), kachel(karte_bunt(teilkarte(EG, name, clip, b)[0]), box, f)],
                                      ["vorher K5e_f1 (Fenster)", "nachher K5g2 (Fenster)", "nachher: Krallen farbig"]), "%s - Hand vorn %s" % (t, seite)))
    schreib(os.path.join(ziel, "10_krallen.png"), stapel(zs))


# ------------------------------------------------------------------ Tafel 11 Streifen
def streifen_reihe(name, clip, b, t, A=KOPF, B=RUMPF, teile=(10, 21, 16, 22, 23), um=None, emds=None):
    emds = emds or ((E0, "vorher K5e_f1"), (EG, "nachher K5g2"))
    karten = [teilkarte(e, name, clip, b)[0] for e, _n in emds]
    box = kasten(karten, um or (A + B))
    f = zoom_fuer(box, 300, 4)
    bl, ts = [], []
    for e, n in emds:
        v, r, g = marken(e, name, clip, b, A, B, teile)
        bl += [kachel(bild(e, name, clip, b), box, f), kachel(v, box, f)]
        ts += [n, "%s: rot %d gelb %d" % (n.split()[0], r, g)]
    return kopfzeile(reihe(bl, ts), "%s  [%s Clip %d Bild %d]" % (t, name, clip, b))


def tafel11(ziel):
    zs = []
    leg = "rot = Kopf/Unterkiefer gemalt, obwohl Brust/Hals/Schulter vorn liegt; gelb = umgekehrt (roh, ohne 2 Bildpunkte Toleranz)"
    zs.append(titel(np.full((2, 1500, 3), GRAU, np.uint8), "STREIFEN AM KOPF. " + leg, 24, 0.5))
    for name, clip, b, t in (("bild2", 12, 0, "Pose und Blick wie Bild 2"), ("bild1", 0, 11, "Pose und Blick wie Bild 1"),
                             ("spiel5080_vorn", 12, 0, "Spielkamera Raum 5080 von vorn"), ("spiel5090", 12, 0, "Spielkamera Raum 5090"),
                             ("spiel5080", 0, 11, "Spielkamera Raum 5080 von der Seite"),
                             ("spiel5080_vorn", 27, 56, "Schlimmste Stelle von heute (2380 Streifen-Bildpunkte)")):
        zs.append(streifen_reihe(name, clip, b, t))
    zs.append(streifen_reihe("spiel5080_vorn", 12, 53, "GROESSTE VERBLIEBENE STELLE: Schulterlappen gegen Kopf (618 Streifen-Bildpunkte, offen)"))
    zs.append(streifen_reihe("spiel5080_vorn", 12, 2, "Groesste verbliebene Stelle Kopf gegen Brust/Hals (17 Bildpunkte)", B=(10, 21), teile=(10, 21, 22, 23)))
    zs.append(streifen_reihe("spiel5080_vorn", 11, 7, "KACHEL Unterkiefer gegen Kopf (nicht geaendert, offen): rot = Unterkiefer gemalt, Kopf liegt vorn; gelb = umgekehrt",
                             A=(23,), B=(22,), teile=None))
    zs.append(streifen_reihe("spiel5080_vorn", 12, 20, "KACHEL Arm rechts ueber Kopf (nur gezeigt, nicht geaendert): rot = Arm gemalt, Kopf liegt vorn",
                             A=(17,), B=(22,), teile=None, um=(17, 22)))
    zs.append(streifen_reihe("spiel5080_vorn", 9, 26, "Hand rechts ueber Kopf (nur gezeigt, nicht geaendert): rot = Hand gemalt, Kopf liegt vorn",
                             A=(18, 19, 20, 29, 31), B=(22,), teile=None, um=(18, 22)))
    for vorher, nachher, name, clip, b, t in (("heute_vorn_r5080_c27b56.png", "geo_vorn_r5080_c27b56.png", "spiel5080_vorn", 27, 56,
                                               "ECHTES SPIELFENSTER Raum 5080 von vorn, schlimmste Stelle von heute"),
                                              ("heute_vorn_r5080.png", "geo_vorn_r5080.png", "spiel5080_vorn", 12, 0, "ECHTES SPIELFENSTER Raum 5080 von vorn, Pose wie Bild 2"),
                                              ("heute_pose2_r5090_s11.png", "geo_pose2_r5090_s11.png", "spiel5090", 12, 0, "ECHTES SPIELFENSTER Raum 5090, Pose wie Bild 2"),
                                              ("heute_pose1_r5080.png", "geo_pose1_r5080.png", "spiel5080", 0, 11, "ECHTES SPIELFENSTER Raum 5080 von der Seite, Pose wie Bild 1")):
        box = kasten([teilkarte(e, name, clip, b)[0] for e in (E0, EG)], KOPF + RUMPF, 12)
        f = zoom_fuer(box, 420, 4)
        v, r, g = marken(E0, name, clip, b, KOPF, RUMPF, (10, 21, 16, 22, 23))
        zs.append(kopfzeile(reihe([kachel(fenster(vorher), box, f), kachel(fenster(nachher), box, f), kachel(v, box, f)],
                                  ["vorher K5e_f1 (Fenster)", "nachher K5g2 (Fenster)", "wo heute Streifen liegen (Modell, rot %d gelb %d)" % (r, g)]),
                            "%s  [Clip %d Bild %d]" % (t, clip, b)))
    schreib(os.path.join(ziel, "11_streifen_weg.png"), stapel(zs))


# ------------------------------------------------------------------ Tafel 13 Mulde sichtbar?
def mulden_reihe(name, clip, b, t, hoechst=4, ziel=300, um=(10, 21, 16)):
    karten = [teilkarte(e, name, clip, b)[0] for e in (E0, EG)]
    box = kasten(karten, um, 8)
    f = zoom_fuer(box, ziel, hoechst)
    a, m, g, k = bild(E0, name, clip, b), bild(EM, name, clip, b), bild(EG, name, clip, b), bild(EK, name, clip, b)
    d = np.abs(g.astype(int) - k.astype(int)).sum(2) > 24
    dm = np.abs(m.astype(int) - a.astype(int)).sum(2) > 24
    v = g.copy()
    v[d] = (255, 0, 0)
    kopf = np.isin(karten[0], KOPF) | np.isin(karten[1], KOPF)             # ganzes Bild 640x480; "ohne Kopf" = weder vorher noch nachher Kopf/Kiefer
    return kopfzeile(reihe([kachel(a, box, f), kachel(m, box, f), kachel(g, box, f), kachel(v, box, f)],
                           ["K5e_f1 (heute)", "m2_a: %d anders (ohne Kopf %d)" % (int(dm.sum()), int((dm & ~kopf).sum())), "K5g2 (neu)",
                            "rot: %d anders als ohne Mulde (ohne Kopf %d)" % (int(d.sum()), int((d & ~kopf).sum()))]),
                     "%s  [%s Clip %d Bild %d]" % (t, name, clip, b))


def tafel13(ziel):
    zs = [titel(np.full((2, 1500, 3), GRAU, np.uint8),
                "SIEHT MAN DIE MULDE, WENN DER KOPF WEIT WEG IST? Weisses Licht, gleiche Textur. Rot = anders als dasselbe Modell ohne Mulde.", 24, 0.5),
          titel(np.full((2, 1500, 3), GRAU, np.uint8),
                "Zahlen = Bildpunkte im ganzen Bild 640x480 mit Farbunterschied ueber 24 (Summe R+G+B). m2_a = Mulde vor Runde 2 (nur Brust, Hals, Hinterkopf), gegen K5e_f1 gezaehlt.", 24, 0.5)]
    zs.append(mulden_reihe("spiel5080_vorn", 11, 5, "PFLICHTBILD vergroessert: Raum 5080 von vorn", hoechst=6, ziel=420))
    for name, t in (("bild1", "Blick wie Bild 1"), ("bild2", "Blick wie Bild 2"), ("spiel5080", "Spielkamera Raum 5080 von der Seite"),
                    ("spiel5080_vorn", "Spielkamera Raum 5080 von vorn"), ("spiel5090", "Spielkamera Raum 5090")):
        zs.append(mulden_reihe(name, 26, 22, t + ", Kopf weit von der Mulde"))
    zs.append(mulden_reihe("spiel5080_vorn", 11, 23, "Groesster Verlust am Umriss (Hinterkopf eingezogen), Raum 5080 von vorn", um=(10, 21, 16, 22)))
    zs.append(mulden_reihe("spiel5080_vorn", 2, 0, "Ruhestellung, Raum 5080 von vorn", um=(10, 21, 16, 22)))
    zs.append(mulden_reihe("spiel5080", 2, 0, "Ruhestellung, Raum 5080 von der Seite", um=(10, 21, 16, 22)))
    schreib(os.path.join(ziel, "13_mulde_sichtbar.png"), stapel(zs))


if __name__ == "__main__":
    ziel = sys.argv[1]
    os.makedirs(ziel, exist_ok=True)
    welche = sys.argv[2:] or ["10", "11", "13"]
    if "10" in welche:
        tafel10(ziel)
    if "11" in welche:
        tafel11(ziel)
    if "13" in welche:
        tafel13(ziel)
