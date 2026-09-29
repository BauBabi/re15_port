#!/usr/bin/env python3
"""tuer_rezepte.py - Runde 33 / Thema T, Stufe 2: Textur-Rezepte der restlichen Port-Tuerarchive.

Wird von tuer_archiv_bauen.py geladen (REZEPTE.update(tuer_rezepte.REZEPTE)). Jedes Rezept bekommt das
Basis-Archiv (dict name/rgba/wert/md1/tim) und liefert (rgba 256x128x4, Protokoll).

⛔ PORT-WAHL, KEINE Original-Adresse. Die Texturen sind Bauentscheidungen; belegt/gemessen sind:
  * Lage der gemalten Merkmale: aus den entzerrten RE1.5-Ausschnitten der Runde 31
    (build/r31_tueren/t1/re15_seiten/*_entz.png, 128x218, Griffkante -> u klein; Raster-Ansicht
    build/r33_tueren/ueber/raster_*.png) - Zahlen je Rezept im Kommentar;
  * Farbe: Median ueber die gewaehlten Ausschnitte in einer Box (farbe_re15), Texel = gemalt * 128 / c
    mit c = 73 (Blattflaeche, tuer_archiv_bauen.C_BLATT; psx-spx GPU:1438-1446) - "gezeigt = gemalt";
  * Zeichnung: RE2-Vorlage (Korn, Kanten) oder ein Relief aus Grundformen an den gemessenen Lagen.
Wo ein Merkmal im Ausschnitt nicht aufloesbar ist, wird es NICHT erfunden (Protokoll "nicht gemalt").
"""
import glob
import os

import numpy as np
from PIL import Image, ImageDraw

import tuer_archiv_bauen as tab

LUM = tab.LUM
BV = tab.BLATT_V          # 218
SS = 4                    # Ueberabtastung fuer Kantenglaettung der Grundformen


# ==============================================================================================
# Messen (RE1.5)
# ==============================================================================================
def ausschnitt(seite, cut, griff):
    f = glob.glob(os.path.join(tab._t1_seiten(), "%s_*_%s_entz.png" % (seite, cut)))
    if not f:
        raise SystemExit("Ausschnitt %s %s fehlt" % (seite, cut))
    a = np.asarray(Image.open(f[0]).convert("RGB"), np.float64)
    return a[:, ::-1] if griff == "rechts" else a


def farbe_re15(auswahl, box):
    """Median-RGB je Ausschnitt in box (u0, v0, u1, v1, Texturlage), dann Median ueber die Ausschnitte."""
    u0, v0, u1, v1 = box
    w = [np.median(ausschnitt(*a)[v0:v1, u0:u1].reshape(-1, 3), 0) for a in auswahl]
    return np.median(np.array(w), 0), [np.round(x, 1).tolist() for x in w]


def hintergrund(raum, cut):
    """ROH-Hintergrund extracted/PSX/STAGEn/ROOMrrr/ROOMrrrcc.bmp (320x240). NICHT die *_voll.png der
    Runde 31: dort ist der Tuerumriss rot eingezeichnet (255,40,40) und verfaelscht jede Farbmessung."""
    return os.path.join(tab._extracted(), "STAGE%s" % raum[0], "ROOM%s" % raum[:3], "ROOM%s%02d.bmp" % (raum[:3], cut))


def farbe_bild(pfad, box, maske=None):
    """Median-RGB in einer Box eines Hintergrundbilds (x0, y0, x1, y1); maske(rgb)->bool waehlt Punkte."""
    a = np.asarray(Image.open(pfad).convert("RGB"), np.float64)
    x0, y0, x1, y1 = box
    r = a[y0:y1, x0:x1].reshape(-1, 3)
    if maske is not None:
        r = r[maske(r)]
    return np.median(r, 0), len(r)


def extracted(rel):
    return os.path.join(tab._extracted(), rel)


# ==============================================================================================
# Zeichnen (Grundformen mit Kantenglaettung) - alles in "gemalt"-Farben, Feld (256, 128, 3) float
# ==============================================================================================
def _maske(zeichne):
    m = Image.new("L", (128 * SS, 256 * SS), 0)
    zeichne(ImageDraw.Draw(m))
    a = np.asarray(m, np.float64) / 255.0
    return a.reshape(256, SS, 128, SS).mean((1, 3))


def m_rechteck(u0, v0, u1, v1, r=0):
    """Deckung eines (abgerundeten) Rechtecks [u0,u1) x [v0,v1) in Texeln."""
    def z(d):
        box = [u0 * SS, v0 * SS, u1 * SS - 1, v1 * SS - 1]
        if r > 0:
            d.rounded_rectangle(box, radius=r * SS, fill=255)
        else:
            d.rectangle(box, fill=255)
    return _maske(z)


def m_ring(u0, v0, u1, v1, breite, r=0):
    aussen = m_rechteck(u0, v0, u1, v1, r)
    innen = m_rechteck(u0 + breite, v0 + breite, u1 - breite, v1 - breite, max(0, r - breite))
    return np.clip(aussen - innen, 0, 1)


def m_polygon(pts):
    return _maske(lambda d: d.polygon([(x * SS, y * SS) for x, y in pts], fill=255))


def m_ellipse(u0, v0, u1, v1):
    return _maske(lambda d: d.ellipse([u0 * SS, v0 * SS, u1 * SS - 1, v1 * SS - 1], fill=255))


def auftragen(feld, maske, farbe):
    """feld = feld * (1 - m) + farbe * m (farbe RGB oder Feld gleicher Form)."""
    m = maske[..., None]
    f = np.asarray(farbe, np.float64)
    return feld * (1 - m) + (f if f.ndim == 3 else f[None, None, :]) * m


def faktor(feld, maske, k):
    """Helligkeit in der Maske mit k multiplizieren (weich)."""
    return feld * (1 + (k - 1) * maske)[..., None]


def relief_rechteck(feld, u0, v0, u1, v1, breite=2, hell=1.25, dunkel=0.6, erhaben=True, r=0):
    """Abgeschraegter Rand: Licht von links oben (wie die RE2-Tuertexturen). erhaben=True: oben/links hell,
    unten/rechts dunkel; vertieft umgekehrt."""
    aussen = m_rechteck(u0, v0, u1, v1, r)
    innen = m_rechteck(u0 + breite, v0 + breite, u1 - breite, v1 - breite, max(0, r - breite))
    ring = np.clip(aussen - innen, 0, 1)
    vv, uu = np.mgrid[0:256, 0:128]
    cu, cv = (u0 + u1) / 2.0, (v0 + v1) / 2.0
    # Seite des Rings: oben/links (Abstand zum oberen/linken Rand kleiner) vs unten/rechts
    ol = ((vv - v0) < (v1 - vv)) & ((uu - u0) < (u1 - uu)) | \
         (((vv - v0) < (v1 - vv)) & ((vv - v0) <= (u1 - uu))) | \
         (((uu - u0) < (u1 - uu)) & ((uu - u0) <= (v1 - vv)))
    k_ol, k_ur = (hell, dunkel) if erhaben else (dunkel, hell)
    k = np.where(ol, k_ol, k_ur)
    return feld * (1 + (k - 1) * ring)[..., None]


def linie(feld, pts, farbe, breite=1.0):
    def z(d):
        d.line([(x * SS, y * SS) for x, y in pts], fill=255, width=max(1, int(breite * SS)))
    return auftragen(feld, _maske(z), farbe)


# ==============================================================================================
# Vorlagen / Umfaerben
# ==============================================================================================
def re2_rgba(name):
    d, o, md1, tim = tab.re2_lesen(name)
    rgba, wert = tab.rgba_aus_tim(tim)
    return rgba, md1


def lum(a):
    return np.asarray(a, np.float64)[..., :3] @ LUM


def umfaerben(rgb, maske, ziel, staerke=1.0, mittel=None):
    """Helligkeitsverlauf (Zeichnung) von rgb behalten, mittlere Helligkeit und Farbton = ziel (gemalt).
    staerke < 1 flacht die Zeichnung ab. Rueckgabe gemalt-Feld (nur in maske geaendert)."""
    L = lum(rgb)
    m = maske > 0
    Lm = float(L[m].mean()) if mittel is None else mittel
    zl = float(np.asarray(ziel) @ LUM)
    rel = 1.0 + staerke * (L / max(Lm, 1e-3) - 1.0)
    neu = rel[..., None] * np.asarray(ziel, np.float64)[None, None, :]
    out = np.asarray(rgb, np.float64)[..., :3].copy()
    out[m] = neu[m]
    return out


def korn_aus(rgb, sigma=1.5, kappe=3.0):
    """relatives Feinkorn L/Tiefpass(L) - 1 (wie tab.blech_korn, ohne Flicken), ganze Textur."""
    L = lum(rgb)
    rel = L / np.maximum(tab.blur(L, sigma), 1.0) - 1.0
    s = rel.std()
    return np.clip(rel, -kappe * s, kappe * s)


def flicken(feld, ziel, quelle):
    """Rechteck ziel (u0, v0, u1, v1) mit dem gleich grossen Rechteck ab quelle (u, v) ueberdecken."""
    u0, v0, u1, v1 = ziel
    qu, qv = quelle
    feld = feld.copy()
    feld[v0:v1, u0:u1] = feld[qv:qv + (v1 - v0), qu:qu + (u1 - u0)]
    return feld


def zeilen_kacheln(feld, ziel, qv0, qv1):
    """Rechteck ziel mit den Zeilen qv0..qv1 derselben Spalten kacheln (senkrechte Kanten laufen durch)."""
    u0, v0, u1, v1 = ziel
    feld = feld.copy()
    h = qv1 - qv0
    for v in range(v0, v1):
        feld[v, u0:u1] = feld[qv0 + (v - v0) % h, u0:u1]
    return feld


def ausbessern(feld, ziel, qv0, qv1):
    """Wie zeilen_kacheln, danach je Zeile und Kanal auf den Verlauf zwischen den Zeilen direkt ueber und
    unter dem Rechteck gebracht (linear) - sonst bliebe ein heller/dunkler Flicken (Vorschau 1 angesehen)."""
    u0, v0, u1, v1 = ziel
    alt = np.asarray(feld, np.float64)
    neu = zeilen_kacheln(alt, ziel, qv0, qv1)
    oben = alt[max(0, v0 - 3):v0, u0:u1].reshape(-1, alt.shape[-1]).mean(0) if v0 > 0 else None
    unten = alt[v1:v1 + 3, u0:u1].reshape(-1, alt.shape[-1]).mean(0) if v1 < alt.shape[0] else None
    if oben is None:
        oben = unten
    if unten is None:
        unten = oben
    for v in range(v0, v1):
        t = (v - v0 + 0.5) / max(1, v1 - v0)
        soll = oben * (1 - t) + unten * t
        ist = neu[v, u0:u1].reshape(-1, neu.shape[-1]).mean(0)
        neu[v, u0:u1] = neu[v, u0:u1] * (soll / np.maximum(ist, 1e-3))
    return neu


def texel(gemalt, c=None):
    """Texel = gemalt * 128 / c (c = 73), gekappt; Kappungen gezaehlt. Farbton bleibt beim Kappen:
    je Punkt skaliert, wenn ein Kanal ueber 255 ginge."""
    c = tab.C_BLATT if c is None else c
    t = np.asarray(gemalt, np.float64) * 128.0 / c
    mx = t.max(-1, keepdims=True)
    gek = int((mx[..., 0] > 255.5).sum())
    t = np.where(mx > 255, t * (255.0 / np.maximum(mx, 1e-6)), t)
    return np.clip(t, 0, 255), gek


def zusammensetzen(basis_rgba, gemalt, bereich=None, c=None):
    """gemalt (256,128,3) -> Texel in basis_rgba (nur bereich-Maske, Standard Blatt v 0..217).
    Kappungen werden nur im Bereich gezaehlt."""
    if bereich is None:
        bereich = np.zeros((256, 128), bool)
        bereich[:BV] = True
    t, _ = texel(gemalt, c)
    gek = int(((np.asarray(gemalt, np.float64) * 128.0 / (tab.C_BLATT if c is None else c)).max(-1) > 255.5)[bereich].sum())
    out = basis_rgba.copy()
    out[bereich, :3] = np.round(t[bereich]).astype(np.uint8)
    out[bereich, 3] = 255
    return out, gek


def beleuchtung(auswahl, sigma=18.0, nur_zeilen=True):
    """grobe Lichtverteilung der gemalten Blaetter (Median der normierten Ausschnitte, stark geglaettet),
    Mittel 1. nur_zeilen: nur senkrechter Verlauf (Oben/Unten), waagerecht flach."""
    fs = []
    for a in auswahl:
        L = lum(ausschnitt(*a))
        fs.append(L / max(float(np.median(L[20:200, 16:112])), 1.0))
    F = np.median(np.array(fs), 0)
    if nur_zeilen:
        z = tab.blur(np.median(F[:, 16:112], 1)[:, None], sigma)[:, 0]
        F = np.repeat(z[:, None], 128, 1)
    else:
        F = tab.blur(F, sigma)
    F = F / F[10:208, 8:120].mean()
    out = np.ones((256, 128))
    out[:BV] = F
    return out


def protokoll_farbe(proto, name, rgb, einzeln=None):
    proto[name] = [round(float(x), 1) for x in rgb]
    if einzeln is not None:
        proto[name + "_je_ausschnitt"] = einzeln


def vorlage_korn(name, sigma=1.5):
    rgba, _ = re2_rgba(name)
    return korn_aus(rgba, sigma)


def mit_korn(gemalt, korn, k=0.5):
    return gemalt * (1.0 + k * korn)[..., None]


def feld_voll(rgb):
    f = np.zeros((256, 128, 3))
    f[:] = np.asarray(rgb, np.float64)
    return f


# ==============================================================================================
# Griff-Schild (Pilot-Punkt a): silbernes Rechteckschild unter dem Druecker flach (DOOR07 Mesh 1)
# ==============================================================================================
# Gemessen in den 19 G1-Ausschnitten (build/r33_tueren/ueber/g1_griff_zoom.png, Texturlage): Schild
# senkrechtes Rechteck u ~4..14 x v ~116..136 (unscharf, Aufweitung ~2 Texel je Seite), Hebel u 14..32
# waagerecht. Druecker-Mesh DOOR07: Anhaengepunkt z -3404 -> u = (3599-3404)/3599*128 = 6,9; y -3352 ->
# v 111 (Pilot); Hebel 509 lang = 18 Texel (u 7..25). Das Schild wird auf die Lage des 3D-Hebels gesetzt
# (sonst saesse der Hebel neben seinem Schild): 8 x 16 Texel (gemessene Groesse minus Unschaerfe),
# Mitte u 7, v 111 -> u 3..11, v 103..119.
SCHILD_07 = (3, 103, 11, 119)
# DOOR1D-Druecker (Mesh 1 = DOOR07.m1) am 1D-Anhaengepunkt (130,-2850,-3410): u = (3599-3410)/3599*128 = 6,7,
# v = 111 + (3352-2850)*218/6602 = 127,6 -> Schild gleicher Groesse u 3..11 v 120..136 (G4-Ausschnitte S045/
# S049/S104: Druecker je auf einem senkrechten Schild an der Fuge, wie G1).
SCHILD_1D = (3, 120, 11, 136)


def schild_aufs_blatt(out, farbe, box):
    """Schild in 'gemalt'-Farbe auf ein fertiges Blatt (Texel) setzen: Texel -> gemalt -> Schild -> Texel."""
    g = out[..., :3].astype(np.float64) * tab.C_BLATT / 128.0
    g = griff_schild(g, farbe, box)
    u0, v0, u1, v1 = box
    t, _ = texel(g)
    out = out.copy()
    out[v0 - 1:v1 + 1, u0 - 1:u1 + 1, :3] = np.round(t[v0 - 1:v1 + 1, u0 - 1:u1 + 1]).astype(np.uint8)
    return out


def griff_schild(gemalt, farbe, box=SCHILD_07):
    u0, v0, u1, v1 = box
    g = auftragen(gemalt, m_rechteck(u0, v0, u1, v1, 1), farbe)
    return relief_rechteck(g, u0, v0, u1, v1, 1, 1.3, 0.55, True, 1)


# ==============================================================================================
# REZEPTE
# ==============================================================================================
REZEPTE = {}


def rezept(k):
    def dek(f):
        REZEPTE[k] = f
        return f
    return dek


# ---------------------------------------------------------------------------------------------
# G1 (P07G, Pilot) + Treppenhausseiten (P07T): glattes Blech + Druecker silbern + Schild
# ---------------------------------------------------------------------------------------------
G1T_AUSWAHL = [("S023", "c00", "rechts"), ("S024", "c02", "rechts"), ("S025", "c05", "rechts")]


def _g1_blatt(basis, auswahl):
    rgba = basis["rgba"]
    out, proto = tab.rezept_glattes_blech(rgba, rgba, auswahl, schild=None,
                                          flicken=(tab.D07_SCHLITZE, tab.D07_SCHILD_FLICKEN))
    farbe, n = tab.re15_griff_farbe(auswahl, (0, 90, 40, 150), [(44, 104)])
    # Schild (Pilot-Punkt a): gemalt in der Farbe des gemalten Griffs, auf das Blatt (Texel = gemalt*128/c)
    blatt_gemalt = out[..., :3].astype(np.float64) * tab.C_BLATT / 128.0
    blatt_gemalt = griff_schild(blatt_gemalt, farbe)
    u0, v0, u1, v1 = SCHILD_07
    t, _ = texel(blatt_gemalt)
    out[v0 - 1:v1 + 1, u0 - 1:u1 + 1, :3] = np.round(t[v0 - 1:v1 + 1, u0 - 1:u1 + 1]).astype(np.uint8)
    out, maske, gp = tab.griff_umfaerben(out, basis["md1"], [1], farbe)
    gp["griff_ausschnitte"] = n
    gp["schild"] = dict(box=list(SCHILD_07), herkunft="g1_griff_zoom: Schild u 4..14 v 116..136 (unscharf) -> "
                        "8x16 Texel an der Lage des Druecker-Meshes (u 7, v 111)")
    proto.update(gp)
    proto["_frei"] = maske
    return out, proto


@rezept("P07G")
def rezept_P07G(basis):
    return _g1_blatt(basis, tab.G1_AUSWAHL)


@rezept("P07T")
def rezept_P07T(basis):
    """Treppenhausseiten ROOM1060 (S023/S024/S025): hell graugruen gemalt (g1_re15_entz.jpg), die
    Gegenseiten S056/S039/S015 dunkel -> eigenes Archiv je Seite (Pilot-Punkt c)."""
    return _g1_blatt(basis, G1T_AUSWAHL)


# ---------------------------------------------------------------------------------------------
# G4 (P1DG): Pilot + eigener Druecker in der gemalten G4-Grifffarbe (Pilot-Punkt b)
# ---------------------------------------------------------------------------------------------
# Druecker DOOR1D (Mesh 1 = DOOR07.m1) im Selbst-Tausch: GEMESSEN wie der Pilot die G4-Abweichung
# bestimmt hat (hellste Griffpunkte > Blatt + 25/30, Median): Sequenz S045/S104 (Bogen der Stufe 2, echte
# exe) L 117,9 / 116,2 gegen gemalt L 99,4 (G1 mit derselben Messung: 76,4 gegen 77,3 = 0,99). Ziel "gezeigt
# = gemalt" -> Grifftexel-Ziel x 99,4/117,9 = 0,843 (Kalibrierung an der Messung, keine Original-Konstante).
# 2. Messung mit 0,843: S045/S104 L 106,8 / 109,8 (Mittel 108,3; die Kontrastgrenze macht die Antwort
# nichtlinear) -> nochmals x 99,4/108,3.
GRIFF_KAL_1D = (99.4 / 117.9) * (99.4 / 108.3)


@rezept("P1DG")
def rezept_P1DG(basis):
    out, proto = tab.rezept_P1DG(basis)
    farbe = np.array(proto["griff_gemalt_g4"], np.float64)
    out = schild_aufs_blatt(out, farbe, SCHILD_1D)
    proto["schild"] = list(SCHILD_1D)
    out, maske, gp = tab.griff_umfaerben(out, basis["md1"], [1], farbe * GRIFF_KAL_1D)
    gp["griff_kalibrierung"] = round(GRIFF_KAL_1D, 3)
    proto.update(gp)
    proto["_frei"] = maske
    proto["griff_hinweis"] = ("Selbst-Tausch: Grund-Drehung DOOR07 (waagerecht), Mesh + Textur aus P1DG selbst "
                              "(DOOR1D.m1 = DOOR07.m1 bytegleich) - Druecker in der gemalten G4-Farbe")
    return out, proto


# ---------------------------------------------------------------------------------------------
# G2 (P06F) Fabrik-Stahltuer: umlaufende gerundete Randnut + zwei vertiefte, breit dunkel gerahmte Felder
# ---------------------------------------------------------------------------------------------
# Lage (raster_G2.png, S189 c08 + S213 c06, Zeilen-/Spaltenprofil S189): Randnut u 15..19 / 102..104
# (dunkel 0,5 der Flaeche, Glanzkante innen), oben v 8..10; Felder mit dunklem Rahmen (0,3..0,4) u 27..31 /
# 84..90, oberes Feld v 28..92, unteres v 111..~188; Feldinneres 0,7..0,85 der Flaeche. Symmetrisch
# gesetzt (Blatt-UV u 0..127 = ganze Breite): Nut u 15..112, Felder u 28..99.
G2_AUSWAHL = [("S189", "c08", "links"), ("S213", "c06", "rechts"), ("S196", "c00", "rechts"),
              ("S203", "c00", "links"), ("S208", "c06", "rechts"), ("S202", "c05", "rechts"),
              ("S184", "c01", "links"), ("S198", "c00", "links")]


def _fabrik_blatt(basis, felder, nut=True, erhaben=False, auswahl=G2_AUSWAHL, box=(40, 40, 85, 85)):
    farbe, einzeln = farbe_re15(auswahl, box)          # Feldinneres
    proto = {}
    protokoll_farbe(proto, "gemalt_feld", farbe, einzeln)
    # vertiefte Felder: Flaeche = Feld / 0,85 (S189-Profil); erhabene (S188): Feldflaeche 1,04 x Flaeche
    flaeche = farbe / (1.04 if erhaben else 0.85)
    g = feld_voll(flaeche)
    if nut:
        g = auftragen(g, m_ring(15, 9, 113, 211, 3, r=8), flaeche * 0.5)
        g = auftragen(g, m_ring(18, 12, 110, 208, 1, r=6), flaeche * 1.12)   # Glanzkante innen
    for u0, v0, u1, v1 in felder:
        if erhaben:
            g = auftragen(g, m_rechteck(u0, v0, u1, v1, 2), flaeche * 1.04)
            g = relief_rechteck(g, u0, v0, u1, v1, 4, 1.18, 0.6, True, 2)
            g = auftragen(g, m_ring(u0 - 1, v0 - 1, u1 + 1, v1 + 1, 1, 3), flaeche * 0.55)   # Fuge (S188)
        else:
            g = auftragen(g, m_rechteck(u0, v0, u1, v1, 3), flaeche * 0.35)            # breiter Rahmen
            g = auftragen(g, m_rechteck(u0 + 6, v0 + 6, u1 - 6, v1 - 6, 2), farbe)     # Feldinneres
            g = relief_rechteck(g, u0 + 6, v0 + 6, u1 - 6, v1 - 6, 1, 0.7, 1.25, True, 2)  # Innenkante
            g = auftragen(g, m_ring(u0 - 1, v0 - 1, u1 + 1, v1 + 1, 1, 4), flaeche * 1.15)
    licht = beleuchtung(auswahl[:4])
    d07, _ = re2_rgba("DOOR07")
    korn, _ = tab.blech_korn(d07, (tab.D07_SCHLITZE, tab.D07_SCHILD_FLICKEN))
    kf = np.zeros((256, 128))
    kf[:BV] = korn
    g = mit_korn(g * licht[..., None], kf, 0.6)
    # gemalte Griffplatte des DOOR06 (u 0..24 v 96..128, der Buegelgriff Mesh 1 sitzt darauf; RE1.5 S189:
    # "Stangengriff auf Griffplatte an der linken Kante auf halber Hoehe") - umgefaerbt auf die Flaeche
    platte = np.zeros((256, 128), bool)
    platte[96:128, 0:24] = True
    g = np.where(platte[..., None], umfaerben(basis["rgba"], platte, flaeche * 0.9, staerke=0.9), g)
    out, gek = zusammensetzen(basis["rgba"], g)
    proto.update(felder=felder, randnut=nut, erhaben=erhaben, gekappt=gek, griffplatte="DOOR06 u 0..24 v 96..128",
                 licht="senkrechter Verlauf der gemalten Blaetter (Median, sigma 18)",
                 korn="DOOR07-Blech (Schlitze geflickt) x 0,6")
    return out, proto


@rezept("P06F")
def rezept_P06F(basis):
    return _fabrik_blatt(basis, [(28, 28, 100, 94), (28, 111, 100, 189)])


@rezept("P06U")
def rezept_P06U(basis):
    """S188 c00 (raster_G2.png, gespiegelt): gepraegte (erhabene) Felder mit abgeschraegtem Rand, oben
    hohes Rechteck u 22..104 v 16..100, unten kleiner u 30..97 v 112..190; keine Randnut, keine Nieten."""
    return _fabrik_blatt(basis, [(22, 16, 106, 100), (30, 112, 98, 190)], nut=False, erhaben=True,
                         auswahl=[("S188", "c00", "rechts")], box=(40, 30, 88, 90))


# ---------------------------------------------------------------------------------------------
# G3 (P1B3) DOOR23-Panzer-Doppeltuer: DOOR23-Blatt je Fluegel (Ausbuchtung an der Fuge = u klein),
# Farbe aus den RE1.5-Seiten; Riegelstange = Griff-Tausch DOOR23 am DOOR23-Anhaengepunkt (Versatz).
# ---------------------------------------------------------------------------------------------
G3_AUSWAHL = [("S136", "c08", "links"), ("S141", "c00", "links"), ("S155", "c03", "links"),
              ("S156", "c00", "links"), ("S158", "c08", "links")]


def _panzer(basis, auswahl):
    d23, _ = re2_rgba("DOOR23")
    # RE1.5-Farbe: Doppeltuer-Ausschnitt zeigt beide Fluegel; Flaeche beider Fluegel ausserhalb der Fuge
    fs = []
    for a in auswahl:
        x = ausschnitt(*a)
        fs.append(np.median(np.concatenate([x[40:180, 8:40], x[40:180, 88:120]], 1).reshape(-1, 3), 0))
    farbe = np.median(np.array(fs), 0)
    proto = {}
    protokoll_farbe(proto, "gemalt_fluegel", farbe, [np.round(f, 1).tolist() for f in fs])
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(d23, m, farbe)
    out, gek = zusammensetzen(basis["rgba"], g)
    proto.update(vorlage="DOOR23 v 0..217 (achteckiges Profil + Ausbuchtung bei u klein = Fuge in 1B)", gekappt=gek)
    return out, proto


@rezept("P1B3")
def rezept_P1B3(basis):
    """Seiten S136 (ROOM2000) und S141 (ROOM2030): braun/olivbraun gemalt."""
    return _panzer(basis, [("S136", "c08", "links"), ("S136", "c13", "links"), ("S141", "c00", "links"),
                           ("S141", "c15", "links")])


@rezept("P1BD")
def rezept_P1BD(basis):
    """Seiten in ROOM2070 (S155 S156 S157 S158): im kalten blaugruenen Licht dunkel gemalt - anders als
    die Gegenseiten -> eigenes Archiv je Seite (wie Pilot-Punkt c)."""
    return _panzer(basis, [("S155", "c03", "links"), ("S156", "c00", "links"), ("S157", "c04", "links"),
                           ("S158", "c08", "links")])


def d1a_ohne_kasten():
    """DOOR1A-Blatt ohne gemalten Kartenleser-Kasten (raster_D1A.png: u 6..27 v 84..140). Ersatz = die
    gespiegelte rechte Seite desselben Feldes (Spalten 127-u): die innere Rahmenlinie (links u 12..14,
    rechts u 113..115) laeuft so an der richtigen Stelle durch."""
    d1a, _ = re2_rgba("DOOR1A")
    f = d1a[..., :3].astype(np.float64)
    f[82:142, 4:29] = f[82:142, 123:98:-1]
    # die rechte Feldseite liegt im Schatten (Bogen der Stufe 2: dunkler Streifen neben den Drueckern) ->
    # je Zeile auf die Helligkeit der unveraenderten Feldflaeche daneben (u 29..40) bringen
    L = f[..., :3] @ LUM
    for v in range(82, 142):
        k = L[v, 29:41].mean() / max(L[v, 16:29].mean(), 1.0)
        f[v, 4:29] *= k
    return f


# ---------------------------------------------------------------------------------------------
# G4b/G4c (P1DK, P1DL): Stahlrahmen-Doppeltuer. DOOR1A-Rahmen (Leisten u 8..16 / 112..120, Querriegel)
# ohne Kartenleser-Kasten; Farbe dunkelgrau (RE1.5), Druecker waagerecht (Selbst-Tausch).
# ---------------------------------------------------------------------------------------------
def _stahlrahmen(basis, auswahl, box, zwei_felder):
    f = d1a_ohne_kasten()
    if zwei_felder:
        # T045: hohes Feld oben + Querriegel + unteres Feld (S080/S121): oberen Querriegel v 70..80 weg
        # (Zeilen v 30..60 kacheln), mittlerer Querriegel bleibt (v 142..152 in DOOR1A)
        f = ausbessern(f, (14, 68, 116, 84), 30, 60)
    farbe, einzeln = farbe_re15(auswahl, box)
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(f, m, farbe, staerke=0.85)
    out, gek = zusammensetzen(basis["rgba"], g)
    # Druecker (DOOR1D Mesh 1, Selbst-Tausch der Grund-Drehung): gemalte Grifffarbe
    gf, n = tab.re15_griff_farbe(auswahl, (0, 90, 40, 150), [(44, 104)])
    out = schild_aufs_blatt(out, gf, SCHILD_1D)
    proto["schild"] = list(SCHILD_1D)
    out, maske, gp = tab.griff_umfaerben(out, basis["md1"], [1], gf * GRIFF_KAL_1D)   # wie P1DG kalibriert
    gp["griff_kalibrierung"] = round(GRIFF_KAL_1D, 3)
    proto.update(gp)
    proto["griff_ausschnitte"] = n
    proto["_frei"] = maske
    proto.update(vorlage="DOOR1A (Stahlrahmen) ohne Kartenleser%s" % (", oberer Querriegel weg" if zwei_felder else ""),
                 gekappt=gek)
    return out, proto


@rezept("P1DK")
def rezept_P1DK(basis):
    # T014: S022 c04 (Flurseite, teal beleuchtet), S030 c03 (Hofseite, dunkel); Doppeltuer-Ausschnitte
    return _stahlrahmen(basis, [("S022", "c04", "links"), ("S030", "c03", "links"), ("S022", "c03", "links")],
                        (60, 40, 110, 180), False)


@rezept("P1DL")
def rezept_P1DL(basis):
    return _stahlrahmen(basis, [("S121", "c08", "links"), ("S080", "c08", "links"), ("S092", "c02", "links"),
                                ("S093", "c07", "links")], (60, 40, 110, 180), True)


# ---------------------------------------------------------------------------------------------
# G5 (P04B): DOOR04 blau -> braunes Holz (Lage der Kassetten + Maserung bleiben; S063 c00 frontal)
# ---------------------------------------------------------------------------------------------
@rezept("P04B")
def rezept_P04B(basis):
    auswahl = [("S063", "c00", "links")]          # S059 nur steil/dunkel (Merkmale R31)
    farbe, einzeln = farbe_re15(auswahl, (10, 20, 60, 200))
    proto = {}
    protokoll_farbe(proto, "gemalt_holz", farbe, einzeln)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    rgba = basis["rgba"]
    g = umfaerben(rgba, m, farbe)
    # Beschlag-Messingschilder auf dem Blatt (u 0..16, v 72..84 und 124..140, gelb) behalten ihren Farbton
    gelb = (rgba[..., 0].astype(int) > rgba[..., 2].astype(int) + 30) & m
    g[gelb] = rgba[gelb, :3].astype(np.float64) * tab.C_BLATT / 128.0
    out, gek = zusammensetzen(rgba, g)
    proto.update(gekappt=gek, messing_texel=int(gelb.sum()))
    return out, proto


# ---------------------------------------------------------------------------------------------
# G5b (P0CD): helle Holz-Doppeltuer mit hohem schmalem Drahtglasfenster + Panikstange (gemalt)
# ---------------------------------------------------------------------------------------------
# S041 c03 Vollbild (build/r33_tueren/ueber/s041voll.png, 3fach): Tuer x 428..620, y 160..318, je Fluegel
# 96 px; Fenster 20..45 % der Fluegelbreite von der Fuge, 8..63 % der Hoehe; Panikstange bei 69 % Hoehe
# von 11 % bis 90 % der Fluegelbreite. Texturlage (Fuge = u klein): Fenster u 26..57 v 17..137, Stange
# v 147..153 u 14..115.
@rezept("P0CD")
def rezept_P0CD(basis):
    pfad = hintergrund("10C0", 3)
    holz, n = farbe_bild(pfad, (145, 60, 158, 100))           # Fluegelflaeche zwischen Fenster und Rand
    glas, _ = farbe_bild(pfad, (161, 62, 166, 86))
    proto = {}
    protokoll_farbe(proto, "gemalt_holz", holz)
    protokoll_farbe(proto, "gemalt_glas", glas)
    rgba = basis["rgba"]
    d0c = rgba[..., :3].astype(np.float64)
    # Maserung: DOOR0C unteres Feld (u 20..110 v 100..200) gekachelt ueber das Blatt (Relief der Felder weg)
    inn = Image.fromarray(np.clip(d0c[110:190, 30:98], 0, 255).astype(np.uint8)).resize((128, 256), Image.BICUBIC)
    korn = korn_aus(np.asarray(inn, np.float64), 2.0)
    g = mit_korn(feld_voll(holz), korn, 0.6)
    g = relief_rechteck(g, 0, 0, 128, BV, 2, 1.2, 0.7, True)          # Kante des Fluegels
    g = auftragen(g, m_rechteck(24, 15, 59, 139, 1), holz * 0.55)      # Fensterrahmen
    g = auftragen(g, m_rechteck(26, 17, 57, 137), glas)
    vv, uu = np.mgrid[0:256, 0:128]
    raute = ((((uu + vv) % 6) == 0) | (((uu - vv) % 6) == 0)) & (uu >= 26) & (uu < 57) & (vv >= 17) & (vv < 137)
    g[raute] = g[raute] * 0.55                                           # Drahtgitter (Rauten)
    g = relief_rechteck(g, 24, 15, 59, 139, 2, 0.6, 1.2, True, 1)
    stange = np.array([150.0, 150.0, 150.0]) * float(holz @ LUM) / 120.0
    g = auftragen(g, m_rechteck(14, 147, 116, 153, 2), holz * 0.35)     # Panikstange (dunkel, gemalt)
    g = auftragen(g, m_rechteck(14, 147, 116, 149, 1), stange)
    out, gek = zusammensetzen(rgba, g)
    proto.update(fenster=[26, 17, 57, 137], panikstange=[14, 147, 116, 153], gekappt=gek,
                 herkunft="S041 c03 Vollbild: Fenster 20..45 %% Breite ab Fuge, 8..63 %% Hoehe; Stange 69 %%")
    return out, proto


# ---------------------------------------------------------------------------------------------
# G6b (P16R): Leiter rostbraun (wie P16M: Helligkeit der RE2-Leiter, Farbton gemalt)
# ---------------------------------------------------------------------------------------------
@rezept("P16R")
def rezept_P16R(basis):
    # ROOM11A00.bmp: Leiterpunkte (S088 c00, Leiter x 126..150 im 320x240-Bild, entz zeigt die Holme)
    pfad = hintergrund("11A0", 0)
    a = np.asarray(Image.open(pfad).convert("RGB"), np.float64)
    reg = a[40:200, 120:160].reshape(-1, 3)
    L = reg @ LUM
    # Holme/Sprossen dunkler als die gruene Wand dahinter: Punkte mit R >= G (nicht gruenstichig) und L > 15
    sel = reg[(reg[:, 0] >= reg[:, 1] - 2) & (L > 15)]
    ton = np.median(sel / np.maximum(sel @ LUM, 1)[:, None], 0)
    rgba = basis["rgba"].copy()
    m = tab.uv_maske(basis["md1"], range(len(basis["md1"].meshes))) & (rgba[..., 3] > 0)
    Lb = lum(rgba)
    neu = np.clip(Lb[..., None] * ton[None, None, :], 0, 255)
    rgba[m, :3] = np.round(neu[m]).astype(np.uint8)
    return rgba, dict(rost_verhaeltnis=[round(float(x), 3) for x in ton], punkte=int(len(sel)),
                      texel_umgefaerbt=int(m.sum()), herkunft="S088 c00 Vollbild x 120..160 y 40..200, R >= G-2, L > 15")


# ---------------------------------------------------------------------------------------------
# G7 (P1EL): Lamellen statt senkrechter Staebe; G7b (P1EU): Staebe nur im unteren Drittel
# ---------------------------------------------------------------------------------------------
# DOOR1E Mesh 0 (Gitter) liest u 0..127 v 0..80 (UV-Bild): Rahmen + 5 senkrechte Staebe, Durchsicht =
# Texel 0 (schwarz, nicht gezeichnet). RE1.5: S048/S154 dunkelgraue waagerechte Lamellen im Rahmen.
def _gitter_bereich(rgba):
    return (np.arange(256)[:, None] < 82) & np.ones((1, 128), bool)


@rezept("P1EL")
def rezept_P1EL(basis):
    rgba = basis["rgba"].copy()
    auswahl = [("S048", "c05", "links"), ("S154", "c02", "links"), ("S154", "c04", "links")]
    # Lamellenfarbe: Lueftung im oberen Teil des Umrisses (Box u 30..100 v 10..60 der entz-Bilder)
    farbe, einzeln = farbe_re15(auswahl, (30, 8, 100, 60))
    proto = {}
    protokoll_farbe(proto, "gemalt_lamellen", farbe, einzeln)
    f = rgba[..., :3].astype(np.float64)
    loch = (rgba[..., 3] == 0)
    # Innenfeld des Rahmens (Staebe + Loecher): u 8..120, v 8..72 (DOOR1E-Textur)
    u0, v0, u1, v1 = 8, 8, 120, 72
    rahmen_farbe = farbe * 2.0     # Rahmen um die Lamellen heller (S154 c02/c04)
    g = umfaerben(f, _gitter_bereich(rgba) & ~loch, rahmen_farbe, staerke=0.8)
    # Lamellenblech = helle Punkte der Lueftung (80. Perzentil der Box), Spalt = dunkle (20.)
    hell, dunkel = [], []
    for a in auswahl:
        r = ausschnitt(*a)[8:60, 30:100].reshape(-1, 3)
        Lr = r @ LUM
        hell.append(np.median(r[Lr >= np.percentile(Lr, 80)], 0))
        dunkel.append(np.median(r[Lr <= np.percentile(Lr, 20)], 0))
    blech, spalt = np.median(np.array(hell), 0), np.median(np.array(dunkel), 0)
    protokoll_farbe(proto, "gemalt_blech", blech)
    protokoll_farbe(proto, "gemalt_spalt", spalt)
    for v in range(v0, v1):
        ph = (v - v0) % 8                     # Lamelle alle 8 Texel: 5 Blech (Licht oben), 3 Spalt
        if ph < 5:
            g[v, u0:u1] = blech * (1.1 - 0.08 * ph)
        else:
            g[v, u0:u1] = spalt
    out = rgba.copy()
    t, gek = texel(g)
    sel = _gitter_bereich(rgba)
    out[sel, :3] = np.round(t[sel]).astype(np.uint8)
    innen = np.zeros((256, 128), bool)
    innen[v0:v1, u0:u1] = True
    out[innen, 3] = 255                        # Lamellen deckend (keine Durchsicht mehr)
    proto.update(lamellen="alle 8 Texel (5 Blech, 3 Spalt) in u 8..120 v 8..72", gekappt=gek,
                 herkunft="S048/S154: waagerechte Lamellen im Rahmen (raster, fokus_P1E.png)")
    return out, proto


@rezept("P1EU")
def rezept_P1EU(basis):
    """S145/S152: dunkle Oeffnung, senkrechte Staebe mit Oberholm nur im unteren Drittel: obere zwei
    Drittel des Innenfelds = Durchsicht (Texel 0), unten die DOOR1E-Staebe mit Oberholm."""
    rgba = basis["rgba"].copy()
    auswahl = [("S145", "c00", "links"), ("S152", "c03", "links")]
    farbe, einzeln = farbe_re15(auswahl, (30, 30, 100, 80))    # Staebe im Umriss (dunkel)
    farbe = np.maximum(farbe, 1.0)
    proto = {}
    protokoll_farbe(proto, "gemalt_staebe", farbe, einzeln)
    loch = rgba[..., 3] == 0
    f = rgba[..., :3].astype(np.float64)
    sel = _gitter_bereich(rgba) & ~loch
    g = umfaerben(f, sel, farbe, staerke=0.9)
    out = rgba.copy()
    t, gek = texel(g)
    out[sel, :3] = np.round(t[sel]).astype(np.uint8)
    # Innenfeld u 12..116: oben (v 8..50) durchsichtig, Oberholm v 50..55
    out[8:50, 12:116, :3] = 0
    out[8:50, 12:116, 3] = 0
    hol, _ = texel(feld_voll(farbe * 1.1))
    out[50:55, 10:118, :3] = np.round(hol[50:55, 10:118]).astype(np.uint8)
    out[50:55, 10:118, 3] = 255
    proto.update(durchsicht="u 12..116 v 8..50 (Texel 0)", oberholm="v 50..55", gekappt=gek)
    proto["_texel0"] = True
    return out, proto


# ---------------------------------------------------------------------------------------------
# G8 (P25G): Aufzugtuer glatt grau - Schild, Warnaufkleber, Griffmulde, Rippen weg
# ---------------------------------------------------------------------------------------------
@rezept("P25G")
def rezept_P25G(basis):
    rgba = basis["rgba"]
    f = rgba[..., :3].astype(np.float64)
    f = ausbessern(f, (40, 36, 84, 90), 140, 190)       # Schild SHAFT TYPE-L (Mittelfeld)
    f = ausbessern(f, (36, 100, 88, 132), 140, 190)     # Warnaufkleber
    f = ausbessern(f, (4, 104, 30, 176), 30, 95)        # Griffmulde links
    f = ausbessern(f, (92, 120, 112, 176), 30, 100)     # Rippen rechts
    f = ausbessern(f, (46, 186, 80, 204), 140, 170)     # Pfeil unten
    auswahl = [("S013", "c04", "links"), ("S040", "c00", "links"), ("S057", "c00", "links")]
    farbe, einzeln = farbe_re15(auswahl, (30, 30, 100, 190))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(f, m, farbe, staerke=0.8)
    out, gek = zusammensetzen(rgba, g)
    proto.update(entfernt="Schild, Warnaufkleber, Griffmulde, Rippen, Pfeil (Zeilen desselben Feldes gekachelt)",
                 gekappt=gek)
    return out, proto


# ---------------------------------------------------------------------------------------------
# G9 (P14A): Maschendrahttor, Rahmen hellgrau (S195 c04); Rauten bleiben (groebere Rauten nicht aufloesbar
# genug, um eine Teilung zu messen -> nicht erfunden)
# ---------------------------------------------------------------------------------------------
@rezept("P14A")
def rezept_P14A(basis):
    rgba = basis["rgba"]
    pfad = hintergrund("3070", 4)
    a = np.asarray(Image.open(pfad).convert("RGB"), np.float64)
    # heller grauer Rahmen der Oeffnung: helle, wenig bunte Punkte links im Bild (Pfosten x 262..280)
    reg = a[70:190, 42:54].reshape(-1, 3)
    L = reg @ LUM
    sel = reg[(L > np.percentile(L, 60)) & (np.abs(reg[:, 0] - reg[:, 2]) < 25)]
    rahmen = np.median(sel, 0)
    proto = {}
    protokoll_farbe(proto, "gemalt_rahmen", rahmen)
    f = rgba[..., :3].astype(np.float64)
    Lf = lum(f)
    loch = rgba[..., 3] == 0
    # Rahmen + Mittelriegel geometrisch (raster_G11f DOOR14): Aussenrechteck u 2..122 v 2..228 ohne die
    # beiden Gitterfelder u 16..112 v 10..106 / 120..222; Gitterdraht bleibt grau
    braun = np.zeros((256, 128), bool)
    braun[2:228, 2:122] = True
    braun[10:106, 16:112] = False
    braun[120:222, 16:112] = False
    braun &= ~loch
    g = umfaerben(f, braun, rahmen, staerke=0.8)
    out = rgba.copy()
    t, gek = texel(g)
    out[braun, :3] = np.round(t[braun]).astype(np.uint8)
    proto.update(rahmen_texel=int(braun.sum()), gekappt=gek,
                 herkunft="S195 c04 Vollbild Pfosten x 42..54 y 70..190, helle unbunte Punkte")
    return out, proto


# ---------------------------------------------------------------------------------------------
# G10 (P2DS): Hubbuehne - Bedientafel + rote Lampe nicht zeichnen (Texel 0), Warnrand bleibt
# ---------------------------------------------------------------------------------------------
@rezept("P2DS")
def rezept_P2DS(basis):
    rgba = basis["rgba"].copy()
    md1 = basis["md1"]
    # DOOR2D Mesh 1 = Bedientafel (UV u 0..95 v 193..255, gruen im UV-Bild), Lampe = Mesh 2-Teil
    # u 96..127 v 193..255 (rot/blau). Texel 0 -> die GPU zeichnet 0x0000 nie (psx-spx).
    m1 = tab.uv_maske(md1, [1])
    lampe = np.zeros((256, 128), bool)
    lampe[193:256, 96:128] = True
    lampe &= tab.uv_maske(md1, [2])
    weg = m1 | lampe
    # Texel, die auch Mesh 0 (Buehne) liest, bleiben
    weg &= ~tab.uv_maske(md1, [0])
    rgba[weg, :3] = 0
    rgba[weg, 3] = 0
    return rgba, dict(nicht_gezeichnet=int(weg.sum()), herkunft="S218 c01: leerer Schacht, keine Tafel/Lampe gemalt",
                      _frei=weg, _texel0=True)


# ---------------------------------------------------------------------------------------------
# G11a (P07R): schlichte rostbraune Stahltuer - DOOR22-Rost ohne Nietrand und ohne D-Riegel
# ---------------------------------------------------------------------------------------------
@rezept("P07R")
def rezept_P07R(basis):
    d22, _ = re2_rgba("DOOR22")
    f = d22[..., :3].astype(np.float64)
    # D-Riegel u 12..34 v 100..134 (raster_G11e) -> Zeilen darunter derselben Spalten
    f = ausbessern(f, (8, 98, 38, 136), 140, 190)
    # Nietrand (u 0..10 / 118..127, v 0..12 / 206..217) weg: Inneres u 12..116 v 14..204 auf das Blatt ziehen
    inn = Image.fromarray(np.clip(f[14:204, 12:116], 0, 255).astype(np.uint8)).resize((128, BV), Image.BICUBIC)
    f2 = f.copy()
    f2[:BV] = np.asarray(inn, np.float64)
    # S089 c09 + S090 c12 (Blatt dort u 8..48, raster_G11d); S089 c12 liegt im gruenen Licht auf der Wand
    auswahl = [("S089", "c09", "rechts"), ("S090", "c12", "rechts")]
    farbe, einzeln = farbe_re15(auswahl, (8, 30, 48, 180))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(f2, m, farbe, staerke=0.8)
    out, gek = zusammensetzen(basis["rgba"], g)
    # "duenner dunkler Druecker rechts" (Merkmale R31 S089/S090): Druecker in halber Blatthelligkeit
    out, maske, gp = tab.griff_umfaerben(out, basis["md1"], [1], farbe * 0.5)
    proto.update(gp)
    proto["_frei"] = maske
    proto.update(vorlage="DOOR22 ohne Nietrand (Inneres gestreckt) und ohne gemalten D-Riegel", gekappt=gek)
    return out, proto


# ---------------------------------------------------------------------------------------------
# G11b (P1AP): DOOR23-Profil, Ausbuchtung gerade gezogen, braun; Druecker DOOR1A (Kasten = Mesh)
# ---------------------------------------------------------------------------------------------
@rezept("P1AP")
def rezept_P1AP(basis):
    d23, _ = re2_rgba("DOOR23")
    f = d23[..., :3].astype(np.float64)
    # Ausbuchtung u 0..42 v 70..162 (raster_G11e DOOR23) -> gerade Profilkante: Zeilen v 30..68 kacheln
    f = ausbessern(f, (0, 68, 44, 164), 30, 68)
    auswahl = [("S151", "c05", "links"), ("S167", "c08", "rechts"), ("S143", "c13", "rechts"),
               ("S139", "c00", "links")]
    farbe, einzeln = farbe_re15(auswahl, (40, 30, 100, 120))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(f, m, farbe)
    # dunkle senkrechte Platte unter dem Druecker (S151 c05: u ~20..30, v ~90..112; Druecker-Mesh DOOR1A
    # Anhaengepunkt z -3162 -> u 15,5, y -3204 -> v 116): Platte u 12..20 v 104..130
    g = auftragen(g, m_rechteck(12, 104, 20, 130, 1), farbe * 0.35)
    out, gek = zusammensetzen(basis["rgba"], g)
    proto.update(vorlage="DOOR23 v 0..217, Ausbuchtung durch gerade Profilzeilen ersetzt", platte=[12, 104, 20, 130],
                 gekappt=gek)
    return out, proto


# ---------------------------------------------------------------------------------------------
# G11c (P1DO): orange Blechtuer - DOOR1D, Gitter -> schmales dunkles Schild + gelbes Warndreieck
# ---------------------------------------------------------------------------------------------
@rezept("P1DO")
def rezept_P1DO(basis):
    rgba = basis["rgba"]
    f = rgba[..., :3].astype(np.float64)
    f = ausbessern(f, (30, 20, 100, 66), 70, 90)        # Lueftungsgitter u 32..96 v 26..60 weg
    auswahl = [("S153", "c00", "rechts"), ("S144", "c13", "links"), ("S153", "c01", "rechts")]
    farbe, einzeln = farbe_re15(auswahl, (40, 90, 110, 170))
    gelb, _ = farbe_re15([("S153", "c00", "rechts")], (68, 74, 78, 84))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    protokoll_farbe(proto, "gemalt_dreieck", gelb)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(f, m, farbe, staerke=0.8)
    # raster_G11a S153 (gespiegelt) / S144: Schild u 40..92 v 44..54, Dreieck Mitte u ~66, v 68..86
    g = auftragen(g, m_rechteck(42, 45, 90, 52, 1), farbe * 0.45)
    g = auftragen(g, m_polygon([(57, 86), (75, 86), (66, 68)]), gelb)
    g = linie(g, [(57, 86), (75, 86), (66, 68), (57, 86)], farbe * 0.25, 1.0)
    g = auftragen(g, m_rechteck(65, 74, 67, 82), farbe * 0.25)          # Ausrufezeichen
    out, gek = zusammensetzen(rgba, g)
    proto.update(schild=[42, 45, 90, 52], dreieck=[[57, 86], [75, 86], [66, 68]], gekappt=gek,
                 bleibt="Griffkasten + Sockel (DOOR1D, gemalt gleich)")
    return out, proto


# ---------------------------------------------------------------------------------------------
# G11d (P26W): einteiliges Schott (DOOR26 V2/V3 = Mesh 0, UV u 0..62) dunkel, zwei Warnstreifen, Rad rot
# ---------------------------------------------------------------------------------------------
@rezept("P26W")
def rezept_P26W(basis):
    rgba = basis["rgba"]
    md1 = basis["md1"]
    auswahl = [("S164", "c02", "links"), ("S164", "c01", "links"), ("S164", "c03", "links")]
    farbe, einzeln = farbe_re15(auswahl, (20, 20, 108, 200))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    m0 = tab.uv_maske(md1, [0])
    rad = tab.uv_maske(md1, [3])
    f = rgba[..., :3].astype(np.float64)
    korn = korn_aus(f, 1.5)
    g = mit_korn(feld_voll(farbe), korn, 0.5)
    # Warnstreifen bei ~25 % und ~50 % der Hoehe (S164, Merkmale R31): v 50..60 und 104..114, schraeg
    vv, uu = np.mgrid[0:256, 0:128]
    # Gelb der Streifen: gelbliche Punkte im Umriss (ROOM20901.bmp x 94..120 y 43..95, R+G-2B > 30) -
    # gemalt sehr dunkel und entsaettigt (hoechstens ~57/59/41), so wird es auch gezeigt
    gelb, ng = farbe_bild(hintergrund("2090", 1), (94, 43, 120, 95),
                          lambda r: (r[:, 0] + r[:, 1] - 2 * r[:, 2]) > 30)
    protokoll_farbe(proto, "gemalt_streifen_gelb", gelb)
    for v0 in (50, 104):
        band = (vv >= v0) & (vv < v0 + 10) & (uu < 64)
        gelbp = ((uu + vv) // 4) % 2 == 0
        g[band & gelbp] = gelb
        g[band & ~gelbp] = farbe * 0.25
    # Rad (Mesh 3) rot: Helligkeitsverlauf des RE2-Rads, Farbton rot (S164 c02: rotes Rad, R >> G, B)
    rotp, n = farbe_bild(hintergrund("2090", 2), (218, 40, 248, 122),
                         lambda r: (r[:, 0] > r[:, 1] + 12) & (r[:, 0] > r[:, 2] + 12))
    protokoll_farbe(proto, "gemalt_rad", rotp)
    g = np.where(rad[..., None], umfaerben(f, rad, rotp), g)
    bereich = m0 | rad
    out, gek = zusammensetzen(rgba, g, bereich)
    proto.update(warnstreifen="v 50..60, 104..114 (u 0..63)", rad_punkte=n, streifen_punkte=ng, gekappt=gek)
    proto["_frei"] = rad
    return out, proto


# ---------------------------------------------------------------------------------------------
# G11e (P24B): beige Labortuer - Aushang weg, Fenster flacher/schmaler
# ---------------------------------------------------------------------------------------------
@rezept("P24B")
def rezept_P24B(basis):
    rgba = basis["rgba"]
    f = rgba[..., :3].astype(np.float64)
    f = ausbessern(f, (62, 76, 100, 120), 130, 170)      # Aushang u 64..96 v 78..118
    f = ausbessern(f, (22, 22, 94, 72), 130, 170)        # altes Fenster u 26..90 v 25..68
    auswahl = [("S183", "c00", "rechts")]
    farbe, einzeln = farbe_re15(auswahl, (40, 70, 100, 170))
    glas, _ = farbe_re15(auswahl, (50, 48, 86, 58))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    protokoll_farbe(proto, "gemalt_glas", glas)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    g = umfaerben(f, m, farbe, staerke=0.4)
    # Trittblech / dunkler unterer Streifen (S183 c00 u 40..110 v 185..205: L 51 gegen Blatt 142)
    tritt, _ = farbe_re15(auswahl, (40, 185, 110, 205))
    protokoll_farbe(proto, "gemalt_tritt", tritt)
    tm = np.zeros((256, 128), bool)
    tm[178:BV] = True
    g = np.where(tm[..., None], umfaerben(f, tm, tritt, staerke=0.6), g)
    # Griffkasten: Texel, die Blatt UND Griff-Mesh lesen (u 5..30 v 113..130 u. a.) -> gemalter dunkler
    # Kasten (S183 c00 u 16..30 v 100..140: 46/48/64)
    kasten_f, _ = farbe_re15(auswahl, (16, 100, 30, 140))
    protokoll_farbe(proto, "gemalt_kasten", kasten_f)
    ueber = tab.uv_maske(basis["md1"], [0]) & tab.uv_maske(basis["md1"], [1])
    kasten = np.zeros((256, 128), bool)
    kasten[90:145, 0:32] = True
    kasten |= ueber
    kasten[BV:] = False
    g = np.where(kasten[..., None], umfaerben(f, kasten, kasten_f, staerke=0.7), g)
    proto["_frei"] = ueber
    # neues Fenster (S183 c00 gespiegelt, raster_G11b): u 40..96 v 44..62, Rahmen hell
    g = auftragen(g, m_rechteck(38, 42, 98, 64, 2), farbe * 0.8)
    g = auftragen(g, m_rechteck(40, 44, 96, 62, 1), glas)
    g = relief_rechteck(g, 38, 42, 98, 64, 2, 1.1, 0.6, True, 2)
    out, gek = zusammensetzen(rgba, g)
    proto.update(fenster=[40, 44, 96, 62], gekappt=gek,
                 hinweis="helle Tuer: gemalt L %.0f > 145 = hellster zeigbarer Wert bei c = 73 ohne Flag 0x1000 "
                         "(Skript bleibt bytegleich) -> gekappte Texel, Farbton erhalten" % float(farbe @ LUM))
    return out, proto


# ---------------------------------------------------------------------------------------------
# G11f/g (P07D, P07H): Toilettentuer - dunkles Blatt, Feld mit abgeschnittener Ecke, Piktogramm, blaues Schild
# ---------------------------------------------------------------------------------------------
def _wc(basis, auswahl, feld, schild, blau, figur_farbe, figur):
    rgba = basis["rgba"]
    farbe, einzeln = farbe_re15(auswahl, (88, 60, 120, 200))
    weiss, _ = farbe_re15(auswahl, (schild[0] + 3, schild[1] + 3, schild[0] + 8, schild[3] - 3))
    blau_f, _ = farbe_re15(auswahl, (blau[0] + 4, blau[1] + 4, blau[2] - 4, blau[3] - 4))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    protokoll_farbe(proto, "gemalt_schild", weiss)
    protokoll_farbe(proto, "gemalt_blau", blau_f)
    korn, _ = tab.blech_korn(rgba, (tab.D07_SCHLITZE, tab.D07_SCHILD_FLICKEN))
    g = np.zeros((256, 128, 3))
    g[:BV] = farbe * (1 + 0.4 * korn)[..., None]
    g *= beleuchtung(auswahl)[..., None]
    u0, v0, u1, v1, ecke = feld
    g = auftragen(g, m_polygon([(u0, v0), (u1 - ecke, v0), (u1, v0 + ecke), (u1, v1), (u0, v1)]), farbe * 0.25)
    g = auftragen(g, m_rechteck(*schild, r=2), weiss)
    g = auftragen(g, figur, figur_farbe)
    g = auftragen(g, m_rechteck(*blau, r=1), blau_f)
    bu0, bv0, bu1, bv1 = blau                  # zwei helle Figuren (Rollstuhl + Mensch), unscharf gemalt
    g = auftragen(g, m_ellipse(bu0 + 8, bv0 + 8, bu0 + 18, bv0 + 30), blau_f * 2.2)
    g = auftragen(g, m_ellipse(bu1 - 18, bv0 + 8, bu1 - 8, bv0 + 30), blau_f * 2.2)
    out, gek = zusammensetzen(rgba, g)
    gf, n = tab.re15_griff_farbe(auswahl, (0, 90, 40, 150), [(44, 104)])
    out, maske, gp = tab.griff_umfaerben(out, basis["md1"], [1], gf)
    proto.update(gp)
    proto["_frei"] = maske
    proto.update(feld=list(feld), schild=list(schild), blaues_schild=list(blau), gekappt=gek)
    return out, proto


@rezept("P07D")
def rezept_P07D(basis):
    """S236 c00 (raster_G11a): Feld u 44..84 v 24..58 Ecke oben rechts, Schild u 50..76 v 80..112 (orange
    Frau), blaues Schild u 42..86 v 120..160."""
    auswahl = [("S236", "c00", "links")]
    orange, _ = farbe_re15(auswahl, (60, 88, 66, 104))
    figur = m_ellipse(60, 84, 66, 90) + m_polygon([(58, 108), (68, 108), (65, 91), (61, 91)])
    return _wc(basis, auswahl, (44, 24, 84, 58, 8), (50, 80, 76, 112), (42, 120, 86, 160), orange, np.clip(figur, 0, 1))


@rezept("P07H")
def rezept_P07H(basis):
    """S232 c02: Feld u 44..84 v 28..62, Schild u 52..78 v 86..112 (blauer Mann), blaues Schild u 44..86 v 128..162."""
    auswahl = [("S232", "c02", "links")]
    blau_m, _ = farbe_re15(auswahl, (62, 92, 68, 106))
    figur = m_ellipse(62, 89, 68, 95) + m_rechteck(61, 95, 69, 110)
    return _wc(basis, auswahl, (44, 28, 84, 62, 8), (52, 86, 78, 112), (44, 128, 86, 162), blau_m, np.clip(figur, 0, 1))


# ---------------------------------------------------------------------------------------------
# G11h (P27S Laborseite, P27K Gangseite): DOOR27 zweiteilige Labor-Schiebetuer
# ---------------------------------------------------------------------------------------------
# DOOR27-Textur (UV-Bild): Teil A (Mesh 0) u 0..100 v 0..100 (links, Plakette unten links v 80..95),
# Teil B (Mesh 1) u 0..100 v 105..208 (Kasten oben rechts, Schlitz, WARNING-Aufkleber u 56..98 v 150..170).
def _labor(basis, auswahl, strahlen):
    rgba = basis["rgba"]
    md1 = basis["md1"]
    f = rgba[..., :3].astype(np.float64)
    farbe, einzeln = farbe_re15(auswahl, (10, 90, 60, 140))
    proto = {}
    protokoll_farbe(proto, "gemalt", farbe, einzeln)
    bereich = tab.uv_maske(md1, [0, 1, 2])
    f = ausbessern(f, (8, 76, 70, 98), 20, 60)          # Plakette Teil A weg
    f = ausbessern(f, (54, 148, 100, 172), 175, 200)    # WARNING-Aufkleber weg
    g = umfaerben(f, bereich, farbe, staerke=0.8)
    # Schildfarben aus S273 c00 (Laborseite): Gelb u 4..38 v 30..70 = 80/102/19; Rot = rote Punkte darin
    gelb, _ = farbe_re15([("S273", "c00", "links")], (4, 30, 38, 70))
    rot, _ = farbe_bild(hintergrund("5060", 0), (0, 0, 320, 240),
                        lambda r: (r[:, 0] > r[:, 1] + 60) & (r[:, 0] > r[:, 2] + 60))
    # auf das Licht der Seite: Verhaeltnis Blatt dieser Seite / Blatt der Laborseite
    lab, _ = farbe_re15([("S273", "c00", "links")], (10, 90, 60, 140))
    k = float(farbe @ LUM) / max(float(lab @ LUM), 1.0)
    gelb, rot = gelb * min(k, 1.6), rot * min(k, 1.6)
    protokoll_farbe(proto, "gemalt_gelb", gelb)
    protokoll_farbe(proto, "gemalt_rot", rot)
    if strahlen:
        # S273 c00: gelbes Schild 'CAUTION' mit drei roten Strahlenfluegeln oben links im grossen Fluegel
        # (u 0..40 v 26..74 des 128er Ausschnitts) -> Teil A u 10..60 v 18..66
        # UV-Sonde (tuer_uv_sonde.py 27 0): der linke Teil liest u 0..100 v 0..100 fuer seine obere Flaeche,
        # der Kasten unten links liest u 0..80 v 7..70 NOCHMAL -> Schild nur in v 74..98 (sonst doppelt; v 72/73 liest zusaetzlich die Unterkante des Kastens und ein Streifen oben rechts - gerastert mit tor_helligkeit, Bild 20 und 150: v 74..98 nur im linken oberen Feld)
        g = auftragen(g, m_rechteck(4, 74, 44, 98, 1), gelb)
        c = (24, 87)
        for w in (30, 150, 270):              # Strahlenzeichen: Fluegel oben links, oben rechts, unten
            a0 = np.radians(w - 30)
            a1 = np.radians(w + 30)
            pts = [c, (c[0] + 11 * np.cos(a0), c[1] - 11 * np.sin(a0)), (c[0] + 11 * np.cos(a1), c[1] - 11 * np.sin(a1))]
            g = auftragen(g, m_polygon(pts), rot)
        g = auftragen(g, m_ellipse(21, 84, 27, 90), gelb)
        g = auftragen(g, m_ellipse(22, 85, 26, 89), rot)
    # Teil B: rotes Warndreieck unter dem Fensterkasten (S273: rechts ~u 92..104 v 60..72; S305: Mitte)
    g = auftragen(g, m_polygon([(66, 140), (82, 140), (74, 126)]), rot)
    # gelb-schwarzer Aufkleber unten rechts (S273/S305)
    vv, uu = np.mgrid[0:256, 0:128]
    auf = (uu >= 44) & (uu < 80) & (vv >= 160) & (vv < 176)     # rechter Teil liest u 3..81 v 106..183
    streif = ((uu + vv) // 3) % 2 == 0
    g[auf & streif] = gelb
    g[auf & ~streif] = farbe * 0.2
    innen = (uu >= 48) & (uu < 76) & (vv >= 163) & (vv < 173)
    g[innen] = gelb * 0.9
    out, gek = zusammensetzen(rgba, g, bereich)
    proto.update(strahlenschild=strahlen, gekappt=gek)
    proto["_frei"] = bereich            # Mesh 1/2 = zweiter Tuerteil/Kanten, gehoeren zum Blatt
    return out, proto


@rezept("P27S")
def rezept_P27S(basis):
    return _labor(basis, [("S273", "c00", "links"), ("S274", "c00", "links")], True)


@rezept("P27K")
def rezept_P27K(basis):
    """Gangseite ROOM5040 (S269 c06): blaugrau."""
    return _labor(basis, [("S269", "c06", "links")], False)


@rezept("P27O")
def rezept_P27O(basis):
    """Gangseite ROOM5120 (S305 c06): gleiche Malerei, orange gemalt (Raumlicht) -> eigenes Archiv."""
    return _labor(basis, [("S305", "c06", "links")], False)


# ---------------------------------------------------------------------------------------------
# G11i (P1AZ): Zug-Innentuer graugruen - zwei Felder, senkrechte Rahmenleiste an der Griffseite
# ---------------------------------------------------------------------------------------------
@rezept("P1AZ")
def rezept_P1AZ(basis):
    """S322 c06 (raster_G11b, gespiegelt: Griff -> u klein): oberes Feld u 28..104 v 8..106 (heller),
    Mittelleiste v 106..120 mit Hebelgriff, unteres Feld v 120..210; senkrechte Rahmenleiste u 20..28."""
    rgba = basis["rgba"]
    d1a, _ = re2_rgba("DOOR1A")
    q = d1a[..., :3].astype(np.float64)
    f = q.copy()

    def zeilen(ziel0, ziel1, q0, q1):
        blk = Image.fromarray(np.clip(q[q0:q1], 0, 255).astype(np.uint8)).resize((128, ziel1 - ziel0), Image.BICUBIC)
        f[ziel0:ziel1] = np.asarray(blk, np.float64)
    # DOOR1A: Rahmen oben + oberes Feld v 0..72, Querriegel v 72..84, unteres Feld + Rahmen v 150..218
    zeilen(0, 106, 0, 72)
    zeilen(106, 120, 72, 84)
    zeilen(120, BV, 150, BV)
    auswahl = [("S322", "c06", "rechts"), ("S322", "c07", "rechts")]
    farbe, einzeln = farbe_re15(auswahl, (40, 130, 100, 200))
    oben, _ = farbe_re15(auswahl, (40, 20, 100, 90))
    proto = {}
    protokoll_farbe(proto, "gemalt_unten", farbe, einzeln)
    protokoll_farbe(proto, "gemalt_oben", oben)
    m = np.zeros((256, 128), bool)
    m[:BV] = True
    # Licht: oben heller (S322: oberes Feld 102/100/78, unteres 35/37/28) - weicher Verlauf statt Kante
    t = np.clip((np.arange(256) - 40) / 100.0, 0, 1)[:, None, None]
    ziel = oben[None, None, :] * (1 - t) + farbe[None, None, :] * t
    L = lum(f)
    rel = 1.0 + 0.8 * (L / max(float(L[m].mean()), 1e-3) - 1.0)
    g = rel[..., None] * ziel
    g = auftragen(g, m_rechteck(20, 4, 28, 214), ziel * 0.8)
    g = relief_rechteck(g, 20, 4, 28, 214, 1, 1.25, 0.6, True)
    out, gek = zusammensetzen(rgba, g)
    proto.update(rahmenleiste=[20, 4, 28, 214], teilung="oberes Feld v 0..106, Mittelleiste 106..120, unteres 120..218",
                 gekappt=gek,
                 nicht_gemalt="dunkles Rechteck oben (im Ausschnitt nicht abgrenzbar)")
    return out, proto


# ---------------------------------------------------------------------------------------------
# G11j (P07M): Gittertuer - DOOR14-Maschendraht (Loecher Texel 0) im braunen Holz-/Rostrahmen auf dem
# Standardblatt (DOOR07-Basis, Druecker am Mittelriegel)
# ---------------------------------------------------------------------------------------------
@rezept("P07M")
def rezept_P07M(basis):
    d14, _ = re2_rgba("DOOR14")
    # DOOR14 Mesh 0 liest u 3..127 v 3..225 (UV-Bild) -> auf das Blatt u 0..127 v 0..217 (naechster Nachbar,
    # damit Loecher = Texel 0 bleiben)
    q = d14[3:225, 3:124]
    img = Image.fromarray(q[..., :3].astype(np.uint8)).resize((128, BV), Image.NEAREST)
    alpha = Image.fromarray(q[..., 3].astype(np.uint8)).resize((128, BV), Image.NEAREST)
    f = np.zeros((256, 128, 3))
    f[:BV] = np.asarray(img, np.float64)
    a = np.zeros((256, 128))
    a[:BV] = np.asarray(alpha, np.float64)
    auswahl = [("S087", "c07", "rechts"), ("S087", "c05", "rechts"), ("S087", "c08", "rechts")]
    farbe, einzeln = farbe_re15(auswahl, (2, 20, 14, 200))       # Rahmen an der Griffkante
    proto = {}
    protokoll_farbe(proto, "gemalt_rahmen", farbe, einzeln)
    braun = (f[..., 0] > f[..., 2] + 8) & (a > 0)
    braun[BV:] = False
    g = umfaerben(f, braun, farbe, staerke=0.8)
    out = basis["rgba"].copy()
    t, gek = texel(g)
    blatt = np.zeros((256, 128), bool)
    blatt[:BV] = True
    out[blatt, :3] = np.round(t[blatt]).astype(np.uint8)
    out[blatt, 3] = np.where(a[blatt] > 0, 255, 0)
    out[blatt & (a == 0), :3] = 0
    proto.update(vorlage="DOOR14 u 3..123 v 3..224 -> Blatt (naechster Nachbar, Loecher Texel 0)",
                 rahmen_texel=int(braun.sum()), gekappt=gek)
    proto["_texel0"] = True
    return out, proto


# ==============================================================================================
# MD1-Ergaenzungen (PORT-WAHL): nur wo eine Textur allein die gemalte Gestalt NICHT zeigen kann
# ==============================================================================================
def md1_lamellenplatte(md1):
    """P1EL (G7, RE1.5 S048/S154: waagerechte LAMELLEN im Rahmen). Die RE2-Lueftungsgitter 1E/33/35 sind
    3D-Staebe mit ECHTEN Luecken (tuer_uv_sonde.py 1E 0: schwarz zwischen den Staeben = kein Dreieck;
    DOOR1E Mesh 0: 6 achteckige Staebe z 163..1603, x 100..200, y 81..1116, Rahmen z 3..18/1782..1797,
    y 0..15/1185..1200); RE2 hat keine Lamellenklappe. Eine Textur bemalt nur die Stabflaechen (Bogen 1:
    Karomuster). Darum: eine Platte (vorn + hinten je 2 Dreiecke) in der Stabmitte x = 150 ueber die
    Rahmenoeffnung z 18..1782, y 15..1185 - sie faehrt mit Mesh 0 (Klappe) mit. UV wie die Stabflaechen
    (v = 5 + (y-81)*68/1035, u = 124,7 - 0,0606*z aus den Stab-Dreiecken, z. B. (200,1116,209)->(112,73),
    (200,81,209)->(112,5)), damit Platte und Stab an jeder Hoehe dieselbe Lamellenzeile zeigen.
    Umlauf wie die vorhandenen Stab-Dreiecke mit Normale +x (Kreuzprodukt x < 0, NCLIP @0x800148d0)."""
    import do2_format as fmt
    m = md1.meshes[0]
    n_vorn = next(i for i, n in enumerate(m.normals) if tuple(n[:3]) == (4096, 0, 0))
    n_hinten = next(i for i, n in enumerate(m.normals) if tuple(n[:3]) == (-4096, 0, 0))
    ref = m.tri_tex[0]                      # clut 0x7800, tpage 0x80 wie alle Dreiecke des Meshes
    k0 = len(m.vertices)
    ecken = [(150, 15, 18, 0), (150, 15, 1782, 0), (150, 1185, 18, 0), (150, 1185, 1782, 0)]   # A B C D

    def uv(e):
        u = int(round(124.7 - 0.0606 * e[2]))
        v = int(round(5 + (e[1] - 81) * 68 / 1035.0))
        return max(0, min(127, u)), max(0, min(255, v))
    A, B, C, D = range(k0, k0 + 4)
    m.vertices = list(m.vertices) + ecken
    tris = [((A, B, C), n_vorn), ((B, D, C), n_vorn), ((A, C, B), n_hinten), ((B, C, D), n_hinten)]
    alt = (len(m.tris), len(m.tri_tex))
    for (a, b, c), n in tris:
        m.tris = list(m.tris) + [(n, a, n, b, n, c)]
        ua, ub, uc = uv(ecken[a - k0]), uv(ecken[b - k0]), uv(ecken[c - k0])
        m.tri_tex = list(m.tri_tex) + [(ua[0], ua[1], ref[2], ub[0], ub[1], ref[5], uc[0], uc[1], 0)]
    # Kreuzprodukt der Vorderseite: x < 0 wie die Stab-Dreiecke mit Normale +x
    import numpy as np
    for (a, b, c), n in tris[:2]:
        pa, pb, pc = [np.array(ecken[i - k0][:3]) for i in (a, b, c)]
        assert np.cross(pb - pa, pc - pa)[0] < 0
    neu = md1.schreiben()
    # die Basis-Meshes bleiben der Anfang (Pruefung in probe_r33_tueren "archive")
    rueck = fmt.Md1.lesen(neu)
    assert len(rueck.meshes) == len(md1.meshes)
    assert list(rueck.meshes[0].tris[:alt[0]]) == list(m.tris[:alt[0]])
    return neu, dict(kurz="Lamellenplatte Mesh 0 (+4 Ecken, +4 Dreiecke: x 150, z 18..1782, y 15..1185)",
                     ecken=ecken, dreiecke=[list(t) for t in m.tris[alt[0]:]],
                     uv=[list(t) for t in m.tri_tex[alt[1]:]])


MD1_ERGAENZUNG = {"P1EL": md1_lamellenplatte}
