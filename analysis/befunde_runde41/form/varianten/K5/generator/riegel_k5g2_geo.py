#!/usr/bin/env python3
"""Riegel K5g2 Runde 2, NUR Geometrie (Streifen O1 = Mulde, Krallen E2/O2, Technik). Textur, Helligkeit, Augen, Haende prueft der Bauer danach.

  python riegel_k5g2_geo.py [<EMD>]        Standard: K5/k5g/K5g2/geo/BAHNSTEIG_FORM.EMD
  Rueckgabe 1, sobald ein Riegel ROT ist oder eine Gegenprobe NICHT rot wird (stumpfer Riegel).

Jeder Riegel ist eine Funktion (Zustand) -> (gut, Text). Die Gegenprobe ist DIESELBE Funktion auf einem Zustand, der den Fehler hat, und muss
ROT sein. Die Messung ueber ALLE Bilder ALLER Clips in fuenf Ansichten laeuft getrennt (kf_k5g2_mulde_mass.py lauf, rund 12 Minuten); dieser
Riegel liest ihre Ergebnisdatei geo/lauf_geo.json und prueft die Herkunft (sha256 der gemessenen EMD = sha256 der geprueften EMD).

Schranken (Herleitung in k5g/K5g2/GEOMETRIE.md, Abschnitt Riegel):
  O1 (i)    {22,23} gegen {10,21}: keine Ansicht ueber 20 Streifen-Pixel; Stichprobe 1425 Ansichten hoechstens 1 % von K5e_f1.
  O1 (ii)   {22,23} gegen {10,21,16}: Mindestziel - die Paare 22>16 und 16>22 (alle Teile gerastert) je UNTER K5e_f1. Das Ziel
            "keine Ansicht ueber 20" steht als Zahl dabei, ist aber kein Riegel (Teil 16 bleibt K5e_f1, GEOMETRIE.md Schulter).
  O1 (iii)  Summe aller Paare mit 22 oder 23 hoechstens ein Viertel von K5e_f1.
  O1 (iv)   Umriss in Bild 1 und Bild 2 gegen die Nutzer-Masken: die Mulde senkt die Deckung (IoU) nicht.
  O1 (vi)   Dreieckshaushalt TEXTRI_QUEUE_MAX 8192 (render_pc.c:226): groesste Zahl gezeichneter Form-Dreiecke + Spielerin + Waffe, und
            die im Spielfenster gezaehlten Bilder (geo/dreiecke_spiel.json) bleiben darunter.
  O1 (vii)  Dehnung der Textur auf der Mulde gegen die Mutterflaeche: nicht schlechter als der Stand der Mulden-Tabelle.
  E2 / O2   rechts 19, 20, 29, 31 = Spiegelnetze der linken 14, 15, 28, 30 (reine Drehung an der eigenen Wurzel); 28 = K5c-Form;
            kein Stachel in Bild 2 und Bild 1 (keine rechte Kralle weiter als 6 Bildpunkte ausserhalb von Video-Figur, Schriftzug und
            Modellumriss ohne die rechten Krallen); alle vier rechten Krallen in Bild 2 sichtbar.
  Technik   EDD/EMR/Clips bytegleich K5e_f1; EMD = Ausgabe des Generators; Netze ausserhalb 10, 21, 22 und der fuenf Krallen = K5e_f1;
            Kopf 22: nur eingezogene Ecken, Flaechen/UV/Normalen gleich; jede Muldenflaeche erbt CLUT-Zeile und Seite der Mutter, neue
            UV nur bei den gelisteten Flaechen.
"""
import copy
import hashlib
import json
import os
import sys

sys.dont_write_bytecode = True
import numpy as np
import cv2

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import kf_k5g2_geo as G  # noqa: E402
import kf_k5g2_mulde as M  # noqa: E402
import kf_k5g2_mulde_mass as MM  # noqa: E402

FE = G.FE
K5 = G.K5
FORM = os.path.normpath(os.path.join(K5, "..", ".."))
GEO = os.path.join(K5, "k5g", "K5g2", "geo")
LAUF = os.path.join(GEO, "lauf_geo.json")
LAUF_K5E = os.path.join(GEO, "lauf_k5e_f1.json")
LAUF_MULDE = os.path.join(GEO, "lauf_mulde.json")
SPIEL = os.path.join(GEO, "dreiecke_spiel.json")
K5C = os.path.join(FORM, "stufe3", "basis_k5c", "FORM.EMD")
MASKEN2 = os.path.join(K5, "zonen_nutzer", "v2", "krallen_kandidat", "krallen_masken_bild2.npz")

GRENZE = 8192                   # TEXTRI_QUEUE_MAX, re15_port/platform/pc/src/render_pc.c:226
SPIELERIN, WAFFE = 773, 86      # PL08 und Waffe, aus den Dateien gezaehlt (GEOMETRIE.md O1 (vi))
UEBER20 = 20                    # O1 (i)/(ii): Streifen-Pixel je Ansicht
ANTEIL_I = 0.01                 # O1 (i): Stichprobe hoechstens 1 % von K5e_f1
STICH_1425 = 3346               # O1 (i): 1 % von K5e_f1 auf der 1425er Stichprobe des Orchestrators (Vorgabe). Die Auswahl ist nicht
                                # nachstellbar (GEOMETRIE.md); sie ist aber ein Teil ALLER Ansichten, also gilt: Summe ueber alle <= 3346
ANTEIL_III = 0.25               # O1 (iii): deutlich darunter = hoechstens ein Viertel
STACHEL_PX = 6.0                # O2: weiteste Stelle einer rechten Kralle ausserhalb der erlaubten Flaeche
ERLAUBT = (10, 21, 22) + tuple(G.KRALLEN_TEILE)
# O1 (vii): Stand der Mulden-Tabelle (GEOMETRIE.md Textur). Der geerbte Stand (Gegenprobe, mit sichtbaren Schlieren) liegt klar darueber.
DEHNUNG = {10: dict(max=16.0, ueber={"8": 2, "4": 20}), 21: dict(max=4.5, ueber={"4": 0, "2": 14})}
LEER = str()

ZEILEN = []


def sha(b):
    return hashlib.sha256(b).hexdigest()


def riegel(name, ergebnis, gegen):
    """ergebnis = (gut, Text); gegen = Liste (Name, (gut, Text)) - jede Gegenprobe muss ROT (gut == False) sein."""
    gut, text = ergebnis
    stumpf = [n for n, (g, _t) in gegen if g]
    ok = bool(gut) and not stumpf
    print("%s  %s: %s" % ("GRUEN" if ok else ("ROT  " if not gut else "STUMPF"), name, text))
    for n, (g, t) in gegen:
        print("         Gegenprobe %s -> %s: %s" % (n, "GRUEN (Riegel stumpf!)" if g else "ROT wie verlangt", t))
    ZEILEN.append((name, ok, len(gegen), len(gegen) - len(stumpf)))
    sys.stdout.flush()


# ------------------------------------------------------------------ Technik
def t_abschnitte(emd, emd0):
    d0, d1 = FE.verzeichnis(emd0)[1], FE.verzeichnis(emd)[1]
    gut = list(d0) == list(d1) and emd0[8:d0[7]] == emd[8:d1[7]]
    return gut, "Abschnitte 0-6 (EDD, EMR, Clips; %d Byte vor den Netzen) %s K5e_f1" % (d0[7] - 8, "bytegleich" if gut else "WEICHEN AB von")


def t_generator(emd):
    neu = G.geometrie_bytes()
    return emd == neu, "EMD %d Byte sha256 %s, Generator liefert %s" % (len(emd), sha(emd)[:16], "dasselbe" if emd == neu else "sha256 " + sha(neu)[:16])


def t_fremde(form, form0):
    ab = [t for t in range(len(form0["meshes"])) if t not in ERLAUBT and form["meshes"][t] != form0["meshes"][t]]
    kn = form["knochen"] == form0["knochen"]
    return (not ab) and kn, "%d Netze ausserhalb %s (darunter Schulter 16, Unterkiefer 23, links 14/15/30, alle hinteren) gleich K5e_f1%s; Knochen %s" % (
        len(form0["meshes"]) - len(ERLAUBT), list(ERLAUBT), LEER if not ab else " - ABWEICHEND: %s" % ab, "gleich" if kn else "ABWEICHEND")


def t_kopf(form, form0):
    a, b = form["meshes"][22], form0["meshes"][22]
    fl = a["dreiecke"] == b["dreiecke"] and a["vierecke"] == b["vierecke"] and a["normalen"] == b["normalen"] and len(a["ecken"]) == len(b["ecken"])
    weg = [float(np.linalg.norm(np.array(x, float) - np.array(y, float))) for x, y in zip(a["ecken"], b["ecken"]) if list(x) != list(y)]
    gut = fl and len(weg) == 36
    return gut, "Kopf 22: Flaechen, UV, CLUT, Normalen %s K5e_f1; %d von %d Ecken versetzt (Soll 36 eingezogene Hinterkopf-Ecken), weitester Weg %.1f" % (
        "gleich" if fl else "UNGLEICH", len(weg), len(a["ecken"]), max(weg) if weg else 0.0)


def t_erbe(tab, stufe1, form0):
    """Jede Flaeche der Teile 10/21: CLUT-Zeile und Seite der Mutter; Ecken/Flaechen = Stufe 1; UV anders als geerbt nur, wenn gelistet."""
    art = ("dreiecke", "vierecke")
    hk = {(a, b, c): (x, y, z) for a, b, c, x, y, z in tab["herkunft"]}
    fehler, text = [], []
    for t in M.TEILE_TEXTUR:
        netz, alt = tab["teile"][str(t)], stufe1["meshes"][t]
        liste = set((int(a), int(i)) for a, i in tab["uv_neu"][str(t)])
        if [list(e) for e in netz["ecken"]] != [list(e) for e in alt["ecken"]]:
            fehler.append("Teil %d: Ecken ungleich Stufe 1" % t)
        n = neu = 0
        for a in (0, 1):
            if len(netz[art[a]]) != len(alt[art[a]]):
                fehler.append("Teil %d: Flaechenzahl ungleich Stufe 1" % t)
                continue
            for i, f in enumerate(netz[art[a]]):
                n += 1
                mt, ma, mi = hk[(t, a, i)]
                mu = form0["meshes"][mt][art[ma]][mi]
                if int(f["clut"]) != int(mu["clut"]) or int(f["page"]) != int(mu["page"]):
                    fehler.append("%d|%d|%d: CLUT/Seite nicht die der Mutter %d|%d|%d" % (t, a, i, mt, ma, mi))
                g = alt[art[a]][i]
                if list(f["e"]) != list(g["e"]) or list(f["n"]) != list(g["n"]):
                    fehler.append("%d|%d|%d: Ecken/Normalen-Verweise ungleich Stufe 1" % (t, a, i))
                if [list(x) for x in f["uv"]] != [list(x) for x in g["uv"]]:
                    neu += 1
                    if (a, i) not in liste:
                        fehler.append("%d|%d|%d: UV neu, steht aber nicht in uv_neu" % (t, a, i))
        text.append("Teil %d: %d Flaechen, %d mit neu verteilten UV (Liste %d)" % (t, n, neu, len(liste)))
        if neu != len(liste):
            fehler.append("Teil %d: Liste uv_neu nennt %d Flaechen, neu verteilt sind %d" % (t, len(liste), neu))
    return not fehler, "; ".join(text) + ("; alle erben CLUT-Zeile und Seite der Mutter" if not fehler else " - FEHLER (%d): %s" % (len(fehler), "; ".join(fehler[:3])))


# ------------------------------------------------------------------ O1 aus dem Lauf ueber alle Bilder
_L = {}


def lauf_laden(pfad):
    if pfad not in _L:
        _L[pfad] = (json.load(open(pfad)), MM.summe(pfad, still=True))
    return _L[pfad]


def stichprobe(d, les):
    """Die 1425er Stichprobe des Orchestrators (Clips 10 und 12 ganz, sonst jedes 4. Bild) aus dem Lauf ueber alle Bilder."""
    n = s = 0
    for an in MM.ANSICHTEN:
        v = [r for r in d["erg"] if r["ansicht"] == an]
        sel = [r for r in v if r["clip"] in (10, 12)] + [r for r in v if r["clip"] not in (10, 12)][::4]
        n += len(sel)
        s += sum(sum(r[les]["streif"]) for r in sel)
    return n, s


def herkunft(d, sha_soll, bilder):
    if d["sha256"] != sha_soll:
        return "Messung gehoert zu einer ANDEREN EMD (sha256 %s); " % d["sha256"][:16]
    if d["schritt_rest"] != 1 or len(d["erg"]) != len(MM.ANSICHTEN) * bilder:
        return "Messung deckt NICHT alle Bilder (%d Ansichten, Soll %d); " % (len(d["erg"]), len(MM.ANSICHTEN) * bilder)
    return LEER


def o1_i(pfad, sha_soll, bilder):
    d, s = lauf_laden(pfad)
    d0, _s0 = lauf_laden(LAUF_K5E)
    h = herkunft(d, sha_soll, bilder)
    n, st = stichprobe(d, "ab4")
    _n0, st0 = stichprobe(d0, "ab4")
    a = s["ab4"]
    gut = not h and a["ueber20"] == 0 and st <= ANTEIL_I * st0 and a["summe"] <= STICH_1425
    return gut, ("%s%d Ansichten: ueber %d Streifen-Pixel %d, groesste %d (%s Clip %d Bild %d), Summe ueber alle %d (Schranke %d = 1 %% von K5e_f1 auf der 1425er Stichprobe, die ein Teil aller Ansichten ist); "
                "nachgestellte Stichprobe %d Ansichten %d (K5e_f1 %d, 1 %% = %d)") % (
        h, len(d["erg"]), UEBER20, a["ueber20"], a["groesste"][0], a["groesste"][1], a["groesste"][2], a["groesste"][3], a["summe"],
        STICH_1425, n, st, st0, int(ANTEIL_I * st0))


def o1_ii(pfad, sha_soll, bilder):
    d, s = lauf_laden(pfad)
    _d0, s0 = lauf_laden(LAUF_K5E)
    h = herkunft(d, sha_soll, bilder)
    p, p0 = s["paare"], s0["paare"]
    gut = not h and p.get("22>16", 0) < p0["22>16"] and p.get("16>22", 0) < p0["16>22"]
    a = s["ab5"]
    return gut, "%sMindestziel: 22>16 %d (K5e_f1 %d), 16>22 %d (K5e_f1 %d). Ziel (kein Riegel): Ansichten ueber %d: %d, groesste %d (%s Clip %d Bild %d), Summe %d" % (
        h, p.get("22>16", 0), p0["22>16"], p.get("16>22", 0), p0["16>22"], UEBER20, a["ueber20"], a["groesste"][0], a["groesste"][1],
        a["groesste"][2], a["groesste"][3], a["summe"])


def o1_iii(pfad, sha_soll, bilder):
    d, s = lauf_laden(pfad)
    _d0, s0 = lauf_laden(LAUF_K5E)
    h = herkunft(d, sha_soll, bilder)
    gut = not h and s["kopf_summe"] <= ANTEIL_III * s0["kopf_summe"]
    return gut, "%salle Paare mit Kopf 22 oder Unterkiefer 23: %d, K5e_f1 %d (%.1f %%, Schranke %d %%)" % (
        h, s["kopf_summe"], s0["kopf_summe"], 100.0 * s["kopf_summe"] / s0["kopf_summe"], int(100 * ANTEIL_III))


def port_dreiecke(form):
    return sum(len(m["dreiecke"]) + 2 * len(m["vierecke"]) for m in form["meshes"])


def o1_vi(pfad, sha_soll, bilder, spiel=None):
    d, s = lauf_laden(pfad)
    h = herkunft(d, sha_soll, bilder)
    dr = s["dreiecke_max"]
    summe = dr[0] + SPIELERIN + WAFFE
    gut = not h and summe <= GRENZE
    text = "%smeiste gezeichnete Form-Dreiecke %d (%s Clip %d Bild %d) + Spielerin %d + Waffe %d = %d von %d" % (
        h, dr[0], dr[1], dr[2], dr[3], SPIELERIN, WAFFE, summe, GRENZE)
    if spiel is not None:
        if not os.path.exists(spiel):
            return False, text + "; Zaehlung im Spielfenster FEHLT (%s)" % os.path.basename(spiel)
        z = json.load(open(spiel, encoding="utf-8"))
        if z["emd_sha256"] != sha_soll:
            return False, text + "; Zaehlung im Spielfenster gehoert zu einer ANDEREN EMD"
        for L in z["laeufe"]:
            gut = gut and L["bilder"] >= 30 and L["max_gesamt"] < GRENZE
            text += "; im Spiel Raum %s Clip %d Bild %d: hoechstens %d Dreiecke je Bild (Form %d, Rest %d; %d Bilder gezaehlt)" % (
                L["raum"], L["clip"], L["bild"], L["max_gesamt"], L["max_form"], L["max_rest"], L["bilder"])
    return gut, text


def o1_vi_form(form, rest, gezeichnet, von):
    """Hochrechnung ohne Lauf: von den Port-Dreiecken der Form wird derselbe Anteil gezeichnet wie im gemessenen dichtesten Bild."""
    n = port_dreiecke(form)
    summe = (n * gezeichnet + von - 1) // von + rest
    return summe <= GRENZE, "Port-Dreiecke der Form %d, davon gezeichnet wie gemessen %d von %d, + Rest %d = %d von %d" % (n, gezeichnet, von, rest, summe, GRENZE)


# ------------------------------------------------------------------ O1 (iv) Umriss, O1 (vii) Dehnung
_MO = {}


def modell(emd):
    k = sha(emd)
    if k not in _MO:
        mo = FE.modell(emd, open(G.GEO_BASIS_TIM, "rb").read())
        _MO[k] = (mo,) + tuple(G.posen(mo))
    return _MO[k]


def umriss(emd):
    aus = {}
    an = G.ansichten()
    tim = open(G.GEO_BASIS_TIM, "rb").read()
    for name, c, bi, mk in (("bild2", 12, 0, "maske_bild2.png"), ("bild1", 0, 11, "maske_bild1.png")):
        kam, lage, gier, _H, _W, _ab = an[name]
        soll = cv2.imread(os.path.join(FORM, mk), cv2.IMREAD_GRAYSCALE) > 127
        _img, msk = G.FB.buehne(emd, tim, c, bi, lage, gier, kam, licht=None)
        m = np.asarray(msk) > 0
        if m.ndim == 3:
            m = m.any(2)
        aus[name] = (float((m & soll).sum()) / max(int((m | soll).sum()), 1), m)
    return aus


def o1_iv(emd, emd_ohne_mulde):
    a, b = umriss(emd), umriss(emd_ohne_mulde)
    gut, text = True, []
    for name in ("bild2", "bild1"):
        gut = gut and a[name][0] >= b[name][0] - 0.0005
        text.append("%s Deckung %.4f, ohne Mulde %.4f (weg %d, dazu %d Bildpunkte)" % (
            name, a[name][0], b[name][0], int((b[name][1] & ~a[name][1]).sum()), int((a[name][1] & ~b[name][1]).sum())))
    return gut, "; ".join(text)


def o1_vii(form0, form, hk):
    gut, text = True, []
    for t in M.TEILE_TEXTUR:
        b = M.dehnung_bericht(form0["meshes"][t], form["meshes"][t], hk, t)["mulde"]
        mx = b["gegen_mutter_laengste"]["max"]
        ok = mx <= DEHNUNG[t]["max"] and all(b["gegen_mutter_ueber"][k] <= v for k, v in DEHNUNG[t]["ueber"].items())
        gut = gut and ok
        text.append("Teil %d: %d Port-Dreiecke auf geteilten Muettern, groesste Dehnung gegen die Mutter %.2f (Schranke %.1f), ueber 8/4/2: %d/%d/%d (Schranke %s), UV entartet %d" % (
            t, b["port_dreiecke"], mx, DEHNUNG[t]["max"], b["gegen_mutter_ueber"]["8"], b["gegen_mutter_ueber"]["4"], b["gegen_mutter_ueber"]["2"],
            ", ".join("ueber %s: %d" % kv for kv in sorted(DEHNUNG[t]["ueber"].items())), b["uv_entartet"]))
    return gut, "; ".join(text)


# ------------------------------------------------------------------ Krallen
def k_spiegel(form, rezept):
    gut, text = True, []
    for links, rechts in G.KRALLEN_PAARE:
        Mq = np.array(rezept["rechts"][str(rechts)]["matrix_q12"], float) / 4096.0
        dreh = float(np.abs(Mq @ Mq.T - np.eye(3)).max()) < 0.002 and np.linalg.det(Mq) > 0
        soll = G.kralle_spiegelnetz(form["meshes"][links], rezept["rechts"][str(rechts)]["matrix_q12"])
        gleich = form["meshes"][rechts] == soll
        L, R = np.array(form["meshes"][links]["ecken"], float), np.array(form["meshes"][rechts]["ecken"], float)
        if len(L) == len(R):
            dl = np.linalg.norm(L[:, None] - L[None], axis=2)
            dr = np.linalg.norm(R[:, None] - R[None], axis=2)
            ab = float(np.abs(dl - dr).max())
            lang = (float(np.linalg.norm(L, axis=1).max()), float(np.linalg.norm(R, axis=1).max()))
        else:
            ab, lang = 1e9, (0.0, 0.0)
        ok = dreh and gleich and ab <= 2.0
        gut = gut and ok
        text.append("%d->%d %s, Eckenabstaende weichen hoechstens %.2f ab, Laenge %.1f/%.1f" % (links, rechts, "Spiegelnetz" if gleich else "KEIN Spiegelnetz", ab, lang[0], lang[1]))
    return gut, "; ".join(text)


def k_28(form, form0, k5c, rezept):
    m, k = form["meshes"][28], k5c["meshes"][28]
    gut = m["ecken"] == k["ecken"] and m["normalen"] == k["normalen"] and m["dreiecke"] == form0["meshes"][28]["dreiecke"] and \
        m["vierecke"] == form0["meshes"][28]["vierecke"] and [list(e) for e in rezept["kralle28"]["ecken"]] == [list(e) for e in k["ecken"]]
    return gut, "Kralle 28: Ecken und Normalen %s der K5c-Form (stufe3/basis_k5c/FORM.EMD), Flaechen/UV wie K5e_f1" % ("gleich" if gut else "UNGLEICH")


def teilkarte(emd, name, clip, bild, ohne=()):
    mo, cb, R, T = modell(emd)
    kam, lage, gier, H, W, ab = G.ansichten()[name]
    i = MM.bild_index(cb, clip, bild)
    tris = G.dreiecke_teile(mo, R[i], T[i], lage, gier, kam, teile=tuple(t for t in range(len(mo.meshes)) if t not in ohne))
    return G.keil_raster(tris, H, W, ab)[0]


def stachel_mass(emd):
    """Je rechter Kralle in Bild 2 und Bild 1: sichtbar, ausserhalb der erlaubten Flaeche (Bildpunkte), weiteste Stelle (Bildpunkte)."""
    km = np.load(MASKEN2)
    aus = {}
    for name, clip, bild, dazu in (("bild2", 12, 0, km["figur"] | km["wasserzeichen"]), ("bild1", 0, 11, None)):
        erl = teilkarte(emd, name, clip, bild, ohne=G.KRALLEN_RECHTS) >= 0
        if dazu is not None:
            erl = erl | dazu
        D = cv2.distanceTransform((~erl).astype(np.uint8), cv2.DIST_L2, cv2.DIST_MASK_PRECISE)
        mt = teilkarte(emd, name, clip, bild)
        for r in G.KRALLEN_RECHTS:
            m = mt == r
            neu = m & ~erl
            aus[(name, r)] = (int(m.sum()), int(neu.sum()), float(D[neu].max()) if neu.any() else 0.0)
    return aus


def k_stachel(emd):
    z = stachel_mass(emd)
    weit = max(v[2] for v in z.values())
    sichtbar = all(z[("bild2", r)][0] > 0 for r in G.KRALLEN_RECHTS)
    text = []
    for name in ("bild2", "bild1"):
        text.append("%s: " % name + ", ".join("%d: %d sichtbar, %d ausserhalb, %.1f weit" % ((r,) + z[(name, r)]) for r in G.KRALLEN_RECHTS))
    return weit <= STACHEL_PX and sichtbar, "; ".join(text) + "; Schranke %.0f Bildpunkte, alle vier in Bild 2 sichtbar: %s" % (STACHEL_PX, "ja" if sichtbar else "NEIN")


# ------------------------------------------------------------------ Ablauf
def main():
    pfad = sys.argv[1] if len(sys.argv) > 1 else G.GEO_ZIEL
    emd = open(pfad, "rb").read()
    emd0 = open(G.GEO_BASIS, "rb").read()
    if sha(emd0) != G.GEO_BASIS_SHA:
        raise SystemExit("Basis ist nicht K5e_f1")
    form, form0 = FE.lese_emd(emd), FE.lese_emd(emd0)
    k5c = FE.lese_emd(open(K5C, "rb").read())
    rezept = json.load(open(G.GEO_KRALLEN, encoding="utf-8"))
    tab = json.load(open(G.GEO_TABELLE, encoding="utf-8"))
    _e, _f0, stufe1, hk1, _q = M.arbeitsform()
    hk = {(a, b, c): (x, y, z) for a, b, c, x, y, z in tab["herkunft"]}
    bilder = len(modell(emd0)[1])
    s_emd, s0 = sha(emd), G.GEO_BASIS_SHA
    print("== Riegel K5g2 Geometrie: %s  %d Byte  sha256 %s  (%d Bilder in %d Clips)" % (
        pfad.replace(chr(92), "/"), len(emd), s_emd, bilder, len(set(c for c, _b, _k in modell(emd0)[1]))))

    # --- Technik
    kaputt = bytearray(emd)
    kaputt[FE.verzeichnis(emd)[1][7] // 2] ^= 1
    riegel("Technik Clips", t_abschnitte(emd, emd0), [("ein Bit in den Clips gekippt", t_abschnitte(bytes(kaputt), emd0))])
    riegel("Technik Generator", t_generator(emd), [("EMD ohne Krallen-Stufe", t_generator(G.geometrie_bytes(stufen=("mulde",))))])
    f = copy.deepcopy(form)
    f["meshes"][16]["ecken"][0] = [f["meshes"][16]["ecken"][0][0] + 1] + list(f["meshes"][16]["ecken"][0][1:])
    g = copy.deepcopy(form)
    g["meshes"][24]["ecken"][0] = [g["meshes"][24]["ecken"][0][0] + 1] + list(g["meshes"][24]["ecken"][0][1:])
    riegel("Technik uebrige Netze", t_fremde(form, form0), [("Schulter 16: eine Ecke um 1 versetzt", t_fremde(f, form0)),
                                                            ("hinteres Teil 24: eine Ecke um 1 versetzt", t_fremde(g, form0))])
    f = copy.deepcopy(form)
    fl = f["meshes"][22]["vierecke"][0]
    fl["uv"] = [[fl["uv"][0][0] + 1, fl["uv"][0][1]]] + [list(x) for x in fl["uv"][1:]]
    g = copy.deepcopy(form)
    i = [k for k, (x, y) in enumerate(zip(form["meshes"][22]["ecken"], form0["meshes"][22]["ecken"])) if list(x) == list(y)][0]
    g["meshes"][22]["ecken"][i] = list(g["meshes"][22]["ecken"][i][:2]) + [g["meshes"][22]["ecken"][i][2] + 5]
    riegel("Technik Kopf 22", t_kopf(form, form0), [("eine UV am Kopf um 1 versetzt", t_kopf(f, form0)), ("eine weitere Kopf-Ecke versetzt", t_kopf(g, form0))])
    t2 = copy.deepcopy(tab)
    t2["teile"]["10"]["dreiecke"][5]["clut"] += 64
    t3 = copy.deepcopy(tab)
    t3["uv_neu"]["21"] = t3["uv_neu"]["21"][1:]
    riegel("Technik Textur-Erbe", t_erbe(tab, stufe1, form0), [("eine Flaeche mit fremder CLUT-Zeile", t_erbe(t2, stufe1, form0)),
                                                              ("ein Eintrag aus der Liste uv_neu gestrichen", t_erbe(t3, stufe1, form0))])
    k0 = FE.lese_emd(emd0)
    k1 = FE.lese_emd(emd0)
    G.mulde_auf_form(k1)          # K5e_f1 + Mulden-Tabelle = Soll der Netze 10, 16, 21, 22
    gl = all(k1["meshes"][t] == form["meshes"][t] for t in G.MULDEN_TEILE)
    gegen = all(k1["meshes"][t] == k0["meshes"][t] for t in G.MULDEN_TEILE)
    riegel("Technik Tabelle", (gl, "Netze 10, 16, 21, 22 der EMD %s der Mulden-Tabelle (k5g2_mulde_tabelle.json)" % ("gleich" if gl else "UNGLEICH")),
           [("K5e_f1", (gegen, "Netze 10, 16, 21, 22 von K5e_f1 gegen die Tabelle: %s" % ("gleich" if gegen else "ungleich")))])

    # --- O1 Streifen
    if not os.path.exists(LAUF):
        riegel("O1 Messung", (False, "geo/lauf_geo.json FEHLT (kf_k5g2_mulde_mass.py lauf)"), [])
    else:
        riegel("O1 (i) Kopf gegen Brust/Hals", o1_i(LAUF, s_emd, bilder), [("K5e_f1", o1_i(LAUF_K5E, s0, bilder)),
                                                                           ("Messung einer anderen EMD (lauf_mulde.json)", o1_i(LAUF_MULDE, s_emd, bilder))])
        riegel("O1 (ii) mit Schulter 16", o1_ii(LAUF, s_emd, bilder), [("K5e_f1", o1_ii(LAUF_K5E, s0, bilder))])
        riegel("O1 (iii) alle Paare am Kopf", o1_iii(LAUF, s_emd, bilder), [("K5e_f1", o1_iii(LAUF_K5E, s0, bilder))])
        dick = copy.deepcopy(form)
        for t in (10, 21):
            for art in ("dreiecke", "vierecke"):
                dick["meshes"][t][art] = dick["meshes"][t][art] * 4
        rest = SPIELERIN + WAFFE
        dmax = lauf_laden(LAUF)[1]["dreiecke_max"][0]
        voll = os.path.join(os.environ.get("TEMP", GEO), "riegel_k5g2_geo_voll_%d.json" % os.getpid())
        if os.path.exists(SPIEL):
            z = json.load(open(SPIEL, encoding="utf-8"))
            z["laeufe"][0]["max_gesamt"] = GRENZE
            json.dump(z, open(voll, "w", encoding="utf-8"))
        riegel("O1 (vi) Dreieckshaushalt", o1_vi(LAUF, s_emd, bilder, SPIEL),
               [("Mulde noch zweimal unterteilt (Flaechen von 10 und 21 mal 4)", o1_vi_form(dick, rest, dmax, port_dreiecke(form))),
                ("Zaehlung im Spielfenster fehlt", o1_vi(LAUF, s_emd, bilder, SPIEL + ".fehlt")),
                ("Zaehlung im Spielfenster: Liste voll (ein Bild mit %d)" % GRENZE, o1_vi(LAUF, s_emd, bilder, voll))])
        if os.path.exists(voll):
            os.remove(voll)
    ohne = G.geometrie_bytes(stufen=("krallen",))
    weg = copy.deepcopy(form)
    weg["meshes"][22]["ecken"] = [[0, 0, 0] for _e in weg["meshes"][22]["ecken"]]
    riegel("O1 (iv) Umriss", o1_iv(emd, ohne), [("Kopf 22 ganz eingezogen", o1_iv(G.emd_mit_md1(emd0, weg), ohne))])
    riegel("O1 (vii) Dehnung der Textur", o1_vii(form0, form, hk), [("geerbte UV (Stufe 1, vor der Neuverteilung)", o1_vii(form0, stufe1, hk1))])

    # --- Krallen
    riegel("E2 Spiegelnetze rechts", k_spiegel(form, rezept), [("K5e_f1", k_spiegel(form0, rezept))])
    riegel("E2 Kralle 28", k_28(form, form0, k5c, rezept), [("K5e_f1 (28 eingezogen)", k_28(form0, form0, k5c, rezept))])
    nur = copy.deepcopy(rezept)
    for r in nur["rechts"]:
        nur["rechts"][r]["matrix_q12"] = [[4096, 0, 0], [0, 4096, 0], [0, 0, 4096]]
    fn = FE.lese_emd(emd0)
    G.krallen_form(fn, nur)
    riegel("O2 kein Stachel", k_stachel(emd), [("nur gespiegelt, ohne Drehung", k_stachel(G.emd_mit_md1(emd0, fn))), ("K5e_f1", k_stachel(emd0))])

    rot = [n for n, ok, _a, _b in ZEILEN if not ok]
    gp = sum(a for _n, _ok, a, _b in ZEILEN)
    gpr = sum(b for _n, _ok, _a, b in ZEILEN)
    print("RIEGEL K5g2 GEOMETRIE: %d von %d gruen, %d von %d Gegenproben rot wie verlangt%s" % (
        len(ZEILEN) - len(rot), len(ZEILEN), gpr, gp, LEER if not rot else " - ROT/STUMPF: " + ", ".join(rot)))
    return 1 if rot else 0


if __name__ == "__main__":
    sys.exit(main())
