"""Tiefe je Bildpunkt AUS DER GEOMETRIE und Rechtecke mit EINER Tiefenstufe.

Phase 2 des Nutzer-Befunds vom 2026-09-19 (analysis/befunde_2026-09-19/pri-masken-audit.md,
Abschnitt 4, Punkte 4 und 5; Gegenpruefung pri-masken-audit.skeptiker.md). Gemessen war dort:

  * DEPTH_FACTOR 0,90 auf die Spaltenregel liess die Maske 10 % NAEHER behaupten als ihre
    eigene Bodenlinie -> wer 0..10 % VOR dem Objekt stand, wurde voll verdeckt (1.4 b);
  * der Kachel-MEDIAN ueber 8..32 px breite Quadrate gab einer diagonalen Bodenlinie EINE
    Tiefe je Kachel -> Schachbrett "halb transparent" (1.4 d);
  * Kacheln ohne Bodentreffer (Sehstrahl ueber dem Horizont) wurden VERWORFEN -> Loecher
    (1.4 c, ROOM1000 C3: 719 von 1076 Punkten ohne Maske).

Hier gilt deshalb OHNE Faktor (der Faktor wird in kalib_geometrie.py an den Kuenstler-
Masken GEMESSEN, nicht gesetzt):
  (i)   ist das Objekt eine SCA-Sperrzelle (Mehrheit seiner Standpunkte liegt in ihr, s.
        zelle_zuordnen), kommt die Tiefe je Bildpunkt aus dem Sehstrahl gegen diese Zelle:
        Wandzelle als Saeule (kollisionstiefe_schnell), Moebel als Quader mit GEMESSENER
        Hoehe (quader_tiefe; Hoehe = IoU-Maximum der Quader-Silhouette gegen die
        Freistellung, Gegenprobe Oberkanten-Lage — zwei Messungen, s. hoehe_messen);
  (ii)  sonst je Bildspalte der Bodenkontakt (unterste opake Zeile) MIT Huellenpruefung
        (Weltpunkt in der SCA-Huelle) und Spalten-Erbe statt Verwerfen;
  (iii) je Bildzeile das Senkrechten-Profil durch den Standpunkt der Spalte
        (geom.vz_der_senkrechten) — was ueber seinem Standpunkt aufragt, ist NAEHER.

Und die Zerlegung (5): die Tiefenkarte wird in STUFEN der Breite `tol` geschnitten, jede
Stufe in Streifen ENTLANG des Tiefengradienten (Spalten- oder Zeilenstreifen), jedes
Rechteck traegt nur die Bildpunkte SEINER Stufe (eigene Deckung im Atlas) und als Tiefe
die in kalib_geometrie.py gemessene Statistik (MAX/MEDIAN/MIN — Hypothesen, an 3583
Kuenstler-Rechtecken verglichen). Ueberlauf ueber 105 Rechtecke / 65536 Atlaspunkte ist
ein FEHLER (Ausnahme), kein Anlass zu vergroebern.

RASTERUNG UND SCHWELLE — die des ORIGINALS, nicht die alte PC-Naeherung depth*64:
  * Figur: OT-Index = otz>>4 (@0x8002565c sra v1,v1,4), otz = (1023*vz)>>12 (ZSF3 = 341,
    @0x80066c70 addiu t0,zero,341 / @0x80066c74 ctc2 t0,cr29) -> bucket(vz) = (1023*vz)>>16.
  * Maske: OT-Index = depth x1 (@0x80039650-60), und die Masken werden als LETZTE Prims des
    Bilds eingehaengt (@0x8001ce54 jal 0x80039590; AddPrim haengt vorne ein) -> im GLEICHEN
    Bucket liegt die Figur obenauf.
  => Maske verdeckt gdw. depth < bucket(vz), d.h. ab vz >= (depth+1)*65536/1023
     (Bucket-Breite 64,0625, nicht 64).
  Rasterung eines Maskenpunkts mit Flaechentiefe vz: depth = bucket(vz). Damit wird eine
  Figur, die VOR der Flaeche steht (vz_f < vz), nie verdeckt (bucket monoton), und eine
  Figur dahinter ab dem naechsten Bucket.
"""
import numpy as np
from scipy import ndimage

import geom
from geom import (vz_at_floor, welt_am_boden, quader_tiefe, kollisionstiefe_schnell,
                  sca_zellen, sca_sperrzellen)

MAX_RECTS = geom.MAX_MASKS_PER_CUT       # 105, RDT-Kopf Byte[7] spielweit (re15_pri.h)
ATLAS_W = ATLAS_H = 256                  # srcX/srcY sind u8 (@0x80039408/0x80039418)
ATLAS_PX = ATLAS_W * ATLAS_H
KANTEN = (8, 10, 12, 16, 20, 24, 32, 40, 48, 64, 96, 128, None)   # None = ein Streifen
BUCKET = 65536.0 / 1023.0                # vz-Breite eines OT-Buckets (ZSF3 = 341)


def bucket(vz):
    """OT-Index der Figur fuer Kamera-z vz: (1023*vz)>>16 (@0x8002565c, ZSF3 @0x80066c70)."""
    return np.floor(1023.0 * np.asarray(vz, np.float64) / 65536.0).astype(np.int32)


def rastern(vz):
    """Flaechen-Kamera-z -> Maskentiefe = bucket(vz), auf 1..1023 geklemmt (s. Kopf).
    Punkte OHNE Kamera-z (vz <= 0) bleiben 0 = keine Tiefe — nie 1 (= immer vorne)."""
    v = np.asarray(vz, np.float64)
    return np.where(v > 0, np.clip(bucket(v), 1, 1023), 0).astype(np.int32)


def verdeckt(depth, vz):
    """Original-Regel: die Maske (depth) verdeckt einen Figurpunkt der Kamera-z vz gdw.
    depth < bucket(vz). Arrays oder Zahlen."""
    return np.asarray(depth) < bucket(vz)


def schwelle_vz(depth):
    """Kleinste Kamera-z, die von `depth` verdeckt wird: (depth+1)*65536/1023."""
    return (np.asarray(depth, np.float64) + 1.0) * BUCKET


def huelle(rdt):
    """Ausdehnung des Raums = Huelle aller SCA-Zellen (x0, x1, z0, z1)."""
    zs = sca_zellen(rdt)
    if not zs:
        return (-10 ** 9, 10 ** 9, -10 ** 9, 10 ** 9)
    xs = [c[0] for c in zs] + [c[0] + c[2] for c in zs]
    zz = [c[1] for c in zs] + [c[1] + c[3] for c in zs]
    return (min(xs), max(xs), min(zz), max(zz))


# ----------------------------------------------------------------------------
# (ii) Standpunkte je Spalte
# ----------------------------------------------------------------------------
def standpunkte(region, R, t, H, y0, hull, bodenkante=None, fuss=None, aufrecht=None):
    """-> (stand, eigene, ohne)

    stand : {x: (wx, wz, vz)} fuer JEDE Spalte mit Objektpixeln (geerbt, wo noetig)
    eigene: Spalten mit EIGENEM gueltigem Bodenkontakt
    ohne  : Spalten, die geerbt haben (kein Treffer / ausserhalb der Huelle / ausserhalb
            der bodenkante)

    fuss=y      : gemessene Bodenzeile fuer Objekte, deren Silhouette den Kontakt nicht
                  zeigt (Fahne, Stativ) -> EIN Standpunkt in der Schwerpunktspalte.
    aufrecht    : True -> EIN Standpunkt am tiefsten Punkt der Silhouette; Zahl -> wie fuss.
    bodenkante  : (x0, x1) Spalten, in denen die Unterkante wirklich der Boden ist.
    """
    hx0, hx1, hz0, hz1 = hull
    xs = [int(x) for x in np.nonzero(region.any(0))[0]]
    stand, eigene, ohne = {}, [], []
    if not xs:
        return stand, eigene, ohne

    def punkt(sx, sy):
        vz = vz_at_floor(R, t, H, sx, sy, y0)
        if not vz:
            return None
        P = welt_am_boden(R, t, H, sx, sy, y0)
        if P is None or not (hx0 <= P[0] <= hx1 and hz0 <= P[1] <= hz1):
            return None
        return (P[0], P[1], vz)

    if fuss is not None or aufrecht is not None:
        cx = float(np.mean(xs)) + 0.5
        if aufrecht is True:
            yb = float(int(np.nonzero(region.any(1))[0].max()))
        else:
            yb = float(int(fuss if fuss is not None else aufrecht))
        sy = min(yb, 239.0) + 0.5
        einzel = punkt(cx, sy)
        if einzel is None:
            # ausserhalb der Huelle: trotzdem der Sehstrahl-Punkt (Fahne vor der Wand)
            vz = vz_at_floor(R, t, H, cx, sy, y0)
            P = welt_am_boden(R, t, H, cx, sy, y0) if vz else None
            einzel = (P[0], P[1], vz) if P else None
        if einzel is None:
            return {}, [], xs
        for x in xs:
            stand[x] = einzel
        return stand, list(xs), []

    bx0, bx1 = (int(bodenkante[0]), int(bodenkante[1])) if bodenkante else (-10 ** 9, 10 ** 9)
    for x in xs:
        rows = np.nonzero(region[:, x])[0]
        yb = int(rows.max())
        p = punkt(x + 0.5, min(yb, 239) + 0.5) if bx0 <= x <= bx1 else None
        if p is None:
            ohne.append(x)
        else:
            stand[x] = p
            eigene.append(x)
    if not stand:
        return {}, [], xs
    for x in ohne:
        stand[x] = stand[min(eigene, key=lambda g: abs(g - x))]
    return stand, eigene, ohne


# ----------------------------------------------------------------------------
# (iii) Senkrechten-Profil
# ----------------------------------------------------------------------------
def profil_spalte(R, t, H, wx, wz, ys):
    """Kamera-z der senkrechten Weltlinie durch (wx, ?, wz) in den Bildzeilen ys (Array).
    Vektorisierte Fassung von geom.vz_der_senkrechten (gleiche Formel)."""
    a = R[4] / 4096.0
    b = (R[3] * wx + R[5] * wz) / 4096.0 + t[1]
    c = R[7] / 4096.0
    d = (R[6] * wx + R[8] * wz) / 4096.0 + t[2]
    sy = np.asarray(ys, np.float64) + 0.5 - 120.0
    n = sy * c - H * a
    with np.errstate(divide="ignore", invalid="ignore"):
        Y = (H * b - sy * d) / n
        vz = c * Y + d
    ok = (np.abs(n) > 1e-9) & np.isfinite(vz) & (vz > 1)
    return vz, ok


def _erben(vz, region):
    """Punkte des Objekts ohne Wert erben den naechsten Punkt MIT Wert (Distanztransformation)."""
    fehlt = region & (vz <= 0)
    hit = vz > 0
    if fehlt.any() and hit.any():
        _, (iy, ix) = ndimage.distance_transform_edt(~hit, return_indices=True)
        vz[fehlt] = vz[iy[fehlt], ix[fehlt]]
    return vz


def tiefe_profil(region, R, t, H, stand):
    """Kamera-z je Bildpunkt (ii)+(iii): je Spalte ihr Standpunkt, je Zeile das Senkrechten-
    Profil. Punkte ohne gueltiges Profil erben den naechsten gueltigen Punkt desselben
    Objekts. -> float (240x320), 0 = kein Objekt."""
    vzm = np.zeros((240, 320), np.float64)
    for x, (wx, wz, vz0) in stand.items():
        rows = np.nonzero(region[:, x])[0]
        if len(rows) == 0:
            continue
        vz, ok = profil_spalte(R, t, H, wx, wz, rows)
        if ok.any():
            vzm[rows[ok], x] = vz[ok]
    return _erben(vzm, region)


# ----------------------------------------------------------------------------
# (i) Zelle: Zuordnung ueber die Standpunkte, Hoehe gemessen
# ----------------------------------------------------------------------------
def _segment_in_zelle(P0, P1, zx, zz, zw, zd):
    """Trifft die Strecke P0->P1 (Weltpunkte auf dem Boden) das Zellrechteck? (Liang-Barsky)"""
    x0, z0 = P0; x1, z1 = P1
    dx, dz = x1 - x0, z1 - z0
    t0, t1 = 0.0, 1.0
    for p, q in ((-dx, x0 - zx), (dx, zx + zw - x0), (-dz, z0 - zz), (dz, zz + zd - z0)):
        if abs(p) < 1e-12:
            if q < 0:
                return False
            continue
        r = q / p
        if p < 0:
            if r > t1:
                return False
            t0 = max(t0, r)
        else:
            if r < t0:
                return False
            t1 = min(t1, r)
    return t0 <= t1


def zelle_zuordnen(region, eigene, R, t, H, y0, zellen, hull):
    """-> (zelle, anteil) oder (None, anteil). Die Zelle, in der die MEHRHEIT der eigenen
    Standpunkte liegt. Der Standpunkt einer Spalte ist nicht ein Punkt, sondern die
    Strecke zwischen den Bodenpunkten der Zeilen yb+0.5 und yb+1.5 (die Unsicherheit
    EINES Bildpunkts): ein Objekt, das eine Zelle IST, hat seinen Bodenkontakt AUF der
    Zellkante — ein reiner Punkt-im-Rechteck-Test traefe die Haelfte davon nicht."""
    if not zellen or not eigene:
        return None, 0.0
    hx0, hx1, hz0, hz1 = hull
    strecken = []
    for x in eigene:
        rows = np.nonzero(region[:, x])[0]
        yb = min(int(rows.max()), 239)
        P0 = welt_am_boden(R, t, H, x + 0.5, yb + 0.5, y0)
        P1 = welt_am_boden(R, t, H, x + 0.5, yb + 1.5, y0)
        if P0 is None:
            continue
        if P1 is None or not (hx0 <= P1[0] <= hx1 and hz0 <= P1[1] <= hz1):
            P1 = P0
        strecken.append((P0, P1))
    if not strecken:
        return None, 0.0
    best, best_n = None, 0
    for z in zellen:
        n = sum(1 for (P0, P1) in strecken if _segment_in_zelle(P0, P1, z[0], z[1], z[2], z[3]))
        if n > best_n:
            best, best_n = z, n
    anteil = best_n / float(len(strecken))
    if anteil < 0.5:
        return None, anteil
    return best, anteil


def quader_auf_band(R, t, H, X0, X1, Z0, Z1, hoehe, y0):
    """geom.quader_tiefe mit BASIS y0: der Quader steht auf der Bodenebene y0 des Bands
    (PSX-Y nach unten, hoehe negativ), Deckel bei y0+hoehe. geom.quader_tiefe setzt die Basis
    fest auf y=0 — auf dem Podest y=-14400 (ROOM1060/10A0) traf so kein Sehstrahl den Quader
    (gemessen 2026-09-19: IoU 0,00, 0 von 9860 Punkten, Tiefe 1)."""
    _R = np.array(R, float).reshape(3, 3) / 4096.0
    _Ri = np.linalg.inv(_R)
    _c = _Ri.dot(-np.array(t, float))
    _sx, _sy = np.meshgrid(np.arange(320) - 160.0, np.arange(240) - 120.0)
    _d = np.stack([_sx, _sy, np.full_like(_sx, float(H))], -1) @ _Ri.T
    best = np.full((240, 320), np.inf)
    y_ob, y_un = y0 + hoehe, y0

    def _eintragen(s, q):
        vz = (q[..., 0] * R[6] + q[..., 1] * R[7] + q[..., 2] * R[8]) / 4096.0 + t[2]
        ok = (s > 0) & np.isfinite(vz) & (vz > 64) & (vz < best)
        best[ok] = vz[ok]

    for achse, wert, lo, hi, oa in ((0, X0, Z0, Z1, 2), (0, X1, Z0, Z1, 2),
                                    (2, Z0, X0, X1, 0), (2, Z1, X0, X1, 0)):
        dd = _d[..., achse]
        with np.errstate(divide="ignore", invalid="ignore"):
            s = (wert - _c[achse]) / dd
            q = _c + s[..., None] * _d
        gut = (np.isfinite(s) & (np.abs(dd) > 1e-9) & (q[..., oa] >= lo) & (q[..., oa] <= hi)
               & (q[..., 1] <= y_un) & (q[..., 1] >= y_ob))
        _eintragen(np.where(gut, s, -1.0), np.nan_to_num(q))
    with np.errstate(divide="ignore", invalid="ignore"):
        s = (y_ob - _c[1]) / _d[..., 1]
        q = _c + s[..., None] * _d
    gut = (np.isfinite(s) & (np.abs(_d[..., 1]) > 1e-9)
           & (q[..., 0] >= X0) & (q[..., 0] <= X1) & (q[..., 2] >= Z0) & (q[..., 2] <= Z1))
    _eintragen(np.where(gut, s, -1.0), np.nan_to_num(q))
    treffer = np.isfinite(best)
    return np.where(treffer, best, 0.0), treffer


def hoehe_messen(region, R, t, H, z, y0=0):
    """Hoehe der Zelle als Quader, GEMESSEN an der Freistellung. -> (hoehe, iou, kante, saeule)
    oder None, wenn die Zelle die Freistellung bei KEINER Hoehe trifft (dann ist die
    Zuordnung falsch und das Objekt laeuft ueber das Profil).

    Messung 1 (iou): fuer h in -100..-4000 die Silhouette des Quaders (quader_auf_band)
    gegen die Freistellung, Jaccard; grob 100er-Schritte, dann +-100 in 25ern.
    Messung 2 (kante): mittlerer Abstand der Oberkante der Quader-Silhouette zur
    Oberkante der Freistellung je Bildspalte (Bildzeilen), bei der gemessenen Hoehe.
    saeule: True, wenn die IoU bis zum Rand des Messbereichs monoton waechst — dann ist
    das Ding hoeher als jeder Quader hier (Wand bis zur Decke) und wird als Saeule
    ohne Deckel gerechnet."""
    X0, X1, Z0, Z1 = z[0], z[0] + z[2], z[1], z[1] + z[3]
    reg = region.astype(bool)

    def iou(h):
        _, tr = quader_auf_band(R, t, H, X0, X1, Z0, Z1, h, y0)
        u = float((tr & reg).sum())
        v = float((tr | reg).sum())
        return (u / v) if v else 0.0, tr

    grob = list(range(-100, -4001, -100))
    werte = [(h, iou(h)[0]) for h in grob]
    hb, ib = max(werte, key=lambda q: q[1])
    if ib <= 0.0:
        return None
    saeule = (hb == grob[-1]) and all(werte[i][1] <= werte[i + 1][1] + 1e-9
                                      for i in range(len(werte) - 6, len(werte) - 1))
    fein = [(h, iou(h)[0]) for h in range(hb + 100, hb - 101, -25) if h < 0]
    hb, ib = max(fein + [(hb, ib)], key=lambda q: q[1])
    _, tr = iou(hb)
    kante = None
    xs = np.nonzero(reg.any(0) & tr.any(0))[0]
    if len(xs):
        d = []
        for x in xs:
            ro = np.nonzero(reg[:, x])[0].min()
            rq = np.nonzero(tr[:, x])[0].min()
            d.append(abs(int(ro) - int(rq)))
        kante = float(np.mean(d))
    return hb, ib, kante, saeule


def tiefe_zelle(region, R, t, H, z, hoehe, saeule, y0=0):
    """Kamera-z je Bildpunkt (i): Sehstrahl gegen die Zelle; Fehltreffer erben vom
    naechsten Treffer. -> (vz float 240x320, Zahl der Treffer); (None, 0) ohne Treffer."""
    X0, X1, Z0, Z1 = z[0], z[0] + z[2], z[1], z[1] + z[3]
    if saeule:
        vz = kollisionstiefe_schnell(R, t, H, [(z[0], z[1], z[2], z[3])]) * 64.0
        tr = vz > 0
    else:
        vz, tr = quader_auf_band(R, t, H, X0, X1, Z0, Z1, hoehe, y0)
    vzm = np.zeros((240, 320), np.float64)
    hit = region & tr
    if not hit.any():
        return None, 0
    vzm[hit] = vz[hit]
    return _erben(vzm, region), int(hit.sum())


# ----------------------------------------------------------------------------
# Gesamtmodell fuer EIN Objekt
# ----------------------------------------------------------------------------
def tiefe_geometrie(rdt, R, t, H, region, y0, band, o=None, zellen=None, bericht=None,
                    zelle_erlaubt=True):
    """-> (vz float 240x320, 0 = kein Objekt; dict mit Messwerten). Rastern macht der Aufrufer.

    o: das Auswahl-Objekt (Schluessel fuss / aufrecht / bodenkante / zelle / tiefe);
       None = reine Geometrie (Kalibrierung an Kuenstler-Silhouetten)."""
    o = o or {}
    info = {"quelle": None}
    region = np.asarray(region, bool)
    if not region.any():
        return None, info
    hull = huelle(rdt)
    if isinstance(o.get("tiefe"), int):
        vzm = np.zeros((240, 320), np.float64)
        # feste Tiefe: als Kamera-z die Bucket-Untergrenze, damit rastern() sie zurueckgibt
        vzm[region] = max(1, min(1023, int(o["tiefe"]))) * BUCKET + 1.0
        info.update(quelle="fest", tiefe=int(o["tiefe"]))
        return vzm, info
    auf = o.get("aufrecht")
    stand, eigene, ohne = standpunkte(region, R, t, H, y0, hull,
                                      bodenkante=o.get("bodenkante"), fuss=o.get("fuss"),
                                      aufrecht=(auf if auf not in ("spalten", None) else None))
    info.update(spalten=len(stand), eigene=len(eigene), geerbt=len(ohne))
    if not stand:
        info["quelle"] = "keine"
        return None, info
    zelle = None
    if zelle_erlaubt and o.get("fuss") is None and not isinstance(auf, int):
        if zellen is None:
            zellen = sca_sperrzellen(rdt, band) or []
        if o.get("zelle"):
            zq = [int(v) for v in o["zelle"]]
            zelle, anteil = (zq[0], zq[1], zq[2], zq[3], 1), 1.0
        else:
            zelle, anteil = zelle_zuordnen(region, eigene, R, t, H, y0, zellen, hull)
        info["zelle_anteil"] = anteil
    if zelle is not None:
        hm = hoehe_messen(region, R, t, H, zelle, y0)
        vzm, treffer = (None, 0)
        if hm is not None:
            hoehe, iou, kante, saeule = hm
            vzm, treffer = tiefe_zelle(region, R, t, H, zelle, hoehe, saeule, y0)
        if vzm is None:
            # Zelle trifft die Freistellung nicht -> Zuordnung verworfen, Profil.
            if bericht is not None:
                bericht.append("zelle x%d..%d z%d..%d (%.0f %% der Standpunkte) trifft die "
                               "Freistellung bei keiner Hoehe - verworfen, Profil"
                               % (zelle[0], zelle[0] + zelle[2], zelle[1], zelle[1] + zelle[3],
                                  100 * info["zelle_anteil"]))
            zelle = None
        else:
            d = rastern(vzm[region])
            info.update(quelle="zelle", zelle=tuple(int(v) for v in zelle[:4]), typ=int(zelle[4]),
                        hoehe=int(hoehe), iou=float(iou), kante=kante, saeule=bool(saeule),
                        treffer=treffer)
            if bericht is not None:
                bericht.append("zelle x%d..%d z%d..%d (Typ %d, %.0f %% der Standpunkte): Hoehe %d "
                               "(IoU %.2f, Oberkante %s px), %s, %d von %d Punkten getroffen, Tiefe %d..%d"
                               % (zelle[0], zelle[0] + zelle[2], zelle[1], zelle[1] + zelle[3], zelle[4],
                                  100 * info["zelle_anteil"], hoehe, iou,
                                  "-" if kante is None else "%.1f" % kante,
                                  "Saeule" if saeule else "Quader", treffer, int(region.sum()),
                                  int(d.min()), int(d.max())))
            return vzm, info
    vzm = tiefe_profil(region, R, t, H, stand)
    info["quelle"] = "profil"
    if bericht is not None:
        d = rastern(vzm[region])
        bericht.append("standpunkte: %d Spalten, davon %d eigene, %d geerbt%s; Profil-Tiefe %d..%d"
                       % (len(stand), len(eigene), len(ohne),
                          "" if not ohne else " (%s)" % geom._bereiche(sorted(ohne)),
                          int(d.min()), int(d.max())))
    return vzm, info


# ----------------------------------------------------------------------------
# (5) Zerlegung: Stufen x Streifen, eigene Deckung je Rechteck
# ----------------------------------------------------------------------------
def _gradient(region, dep):
    m = region & (dep > 0)
    gx = np.abs(dep[:, 1:] - dep[:, :-1])[m[:, 1:] & m[:, :-1]]
    gy = np.abs(dep[1:, :] - dep[:-1, :])[m[1:, :] & m[:-1, :]]
    return (float(gx.mean()) if gx.size else 0.0), (float(gy.mean()) if gy.size else 0.0)


def _streifen(S, spalten, k):
    """Rechtecke (x, y, w, h) fuer die Punktmenge S: Streifen der Breite k (None = ganz)
    quer zur Ausdehnung, je Streifen der engste Kasten; gleich hohe Nachbarn verschmolzen."""
    ys, xs = np.nonzero(S)
    if len(ys) == 0:
        return []
    if not spalten:
        out = _streifen(S.T, True, k)
        return [(y, x, h, w) for (x, y, w, h) in out]
    x0, x1 = int(xs.min()), int(xs.max()) + 1
    step = (x1 - x0) if k is None else k
    rects = []
    for gx in range(x0, x1, step):
        gw = min(step, x1 - gx)
        sub = S[:, gx:gx + gw]
        if not sub.any():
            continue
        cols = np.nonzero(sub.any(0))[0]
        rows = np.nonzero(sub.any(1))[0]
        rx = gx + int(cols.min()); rw = int(cols.max() - cols.min()) + 1
        ry = int(rows.min()); rh = int(rows.max() - rows.min()) + 1
        if rects and rects[-1][1] == ry and rects[-1][3] == rh and rects[-1][0] + rects[-1][2] == rx:
            px, py, pw, ph = rects[-1]
            rects[-1] = (px, py, pw + rw, ph)
        else:
            rects.append((rx, ry, rw, rh))
    return rects


def _zerlege_objekt(region, dep, tol, spalten, k, statistik):
    """-> [(x, y, w, h, tiefe, opak)] fuer EIN Objekt; opak = (240x320 bool) nur die Punkte
    der eigenen Stufe innerhalb des Rechtecks; tiefe = statistik(dep[opak]).
    Rechtecke breiter/hoeher als das Atlasblatt (256) werden halbiert (u8-Adressierung)."""
    m = region & (dep > 0)
    stufe = np.where(m, (dep - 1) // max(1, tol), -1)
    out = []
    for lv in np.unique(stufe[m]):
        S = stufe == lv
        kasten = list(_streifen(S, spalten, k))
        while kasten:
            (x, y, w, h) = kasten.pop()
            if w > ATLAS_W:
                a = w // 2; kasten += [(x, y, a, h), (x + a, y, w - a, h)]; continue
            if h > ATLAS_H:
                a = h // 2; kasten += [(x, y, w, a), (x, y + a, w, h - a)]; continue
            op = np.zeros((240, 320), bool)
            op[y:y + h, x:x + w] = S[y:y + h, x:x + w]
            if not op.any():
                continue
            tiefe = int(statistik(dep[op]))
            out.append((x, y, w, h, tiefe, op))
    return out


def _packbar(rects, budget, kap):
    """Haelt diese Rechteckliste die ENGINE-Grenzen wirklich? Zahl <= budget und das
    Regalverfahren des Atlas (atlas.shelf_pack) bringt alle unter — die Flaechensumme allein
    reicht nicht (Verschnitt; gemessen 2026-09-19: 18 von 74 STAGE1-Cuts fielen mit
    Flaechensumme <= 65536 trotzdem durch die Packung)."""
    import atlas as _atlas
    if len(rects) > budget:
        return False
    place, rejected = _atlas.shelf_pack([(r[0], r[1], r[2], r[3]) for r in rects])
    return not rejected


def zerlegung(objekte, tol, budget=MAX_RECTS, kap=ATLAS_PX, kanten=KANTEN, bericht=None,
              statistik=np.max):
    """objekte: [(name, region, dep)] -> (rects, wahl) oder RuntimeError bei Ueberlauf.

    Fuer jedes Objekt wird die Streifenrichtung aus dem Tiefengradienten gewaehlt
    (groesserer mittlerer Sprung = quer dazu streifen) und die Streifenbreite k aus KANTEN
    so, dass Rechteckzahl (<= budget) und Atlasflaeche (<= kap) zusammen halten; unter
    allen haltbaren Kombinationen die mit der KLEINSTEN Atlasflaeche (engste Deckung).
    Haelt keine: RuntimeError mit den besten erreichten Zahlen — LAUT, nicht vergroebern."""
    import itertools
    optionen = []
    for (name, reg, dep) in objekte:
        gx, gy = _gradient(reg, dep)
        spalten = gx >= gy
        opts = []
        for k in kanten:
            rs = _zerlege_objekt(reg, dep, tol, spalten, k, statistik)
            if not rs:
                continue
            n = len(rs); fl = sum(r[2] * r[3] for r in rs)
            opts.append((k, n, fl, rs))
        if not opts:
            continue
        optionen.append((name, spalten, opts))
    if not optionen:
        return [], []
    best = None

    def rects_von(komb):
        out = []
        for i, j in enumerate(komb):
            out += optionen[i][2][j][3]
        return out

    if len(optionen) <= 5:
        kand = []
        for komb in itertools.product(*[range(len(o[2])) for o in optionen]):
            n = sum(optionen[i][2][j][1] for i, j in enumerate(komb))
            fl = sum(optionen[i][2][j][2] for i, j in enumerate(komb))
            if n <= budget and fl <= kap:
                kand.append((fl, n, komb))
        kand.sort()
        # engste Deckung zuerst; die erste, die WIRKLICH packt, gewinnt (hoechstens 300 Proben)
        for fl, n, komb in kand[:300]:
            if _packbar(rects_von(komb), budget, kap):
                best = (fl, n, komb)
                break
    else:
        wahl = [min(range(len(o[2])), key=lambda j: o[2][j][1]) for o in optionen]

        def summen(w):
            return (sum(optionen[i][2][j][1] for i, j in enumerate(w)),
                    sum(optionen[i][2][j][2] for i, j in enumerate(w)))
        n, fl = summen(wahl)
        if n <= budget:
            versuche = 0
            while versuche < 300:
                versuche += 1
                if fl <= kap and _packbar(rects_von(tuple(wahl)), budget, kap):
                    best = (fl, n, tuple(wahl)); break
                kand = None
                for i in range(len(wahl)):
                    for j in range(len(optionen[i][2])):
                        if j == wahl[i]:
                            continue
                        p = list(wahl); p[i] = j
                        pn, pf = summen(p)
                        if pn <= budget and pf < fl and (kand is None or pf < kand[0]):
                            kand = (pf, pn, p)
                if not kand:
                    break
                fl, n, wahl = kand
    if best is None:
        n_min = sum(min(o[1] for o in opt[2]) for opt in optionen)
        f_min = sum(min(o[2] for o in opt[2]) for opt in optionen)
        raise RuntimeError("Zerlegung haelt die Grenzen nicht (Zahl, Flaeche oder Packung): mindestens "
                           "%d Rechtecke (Grenze %d) bzw. mindestens %d Atlaspunkte (Grenze %d) bei Stufe %d"
                           % (n_min, budget, f_min, kap, tol))
    fl, n, komb = best
    rects = []
    wahl = []
    for i, j in enumerate(komb):
        name, spalten, opts = optionen[i]
        k, cn, cf, rs = opts[j]
        wahl.append((name, "spalten" if spalten else "zeilen", k, cn, cf))
        rects += rs
    if bericht is not None:
        bericht.append("zerlegung Stufe %d: %s -> %d Rechtecke, %d Atlaspunkte"
                       % (tol, ", ".join("%s=%s/%s" % (w[0][:14], w[1], "voll" if w[2] is None else w[2])
                                         for w in wahl), n, fl))
    return rects, wahl


def deckung_aus_rects(rects):
    d = np.zeros((240, 320), bool)
    for r in rects:
        d |= r[5]
    return d
