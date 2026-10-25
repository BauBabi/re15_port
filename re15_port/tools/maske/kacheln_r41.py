"""Runde 41 Spur M, Nachbesserung 3 (Abnahme 2, Maengel 1 und 2): Generator-Rechtecke der Stages 3..6, deren Objekt einen
KOERPER hat (Kollisionszelle + gemessene Hoehe), werden in SENKRECHTE TEILRECHTECKE zerlegt, jedes mit der Tiefe seines
Koerperstuecks.

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 3".

BEFUND (Mass "zu fern", test_r37_masken36 fern): eine Figur HINTER dem Koerper einer gezeichneten Modell-Maske blieb sichtbar
(1 360 606 Figurpunkte, Leon + Elza). Ursachen: (1) falsche Zelle (30A0 obere Trennwandfelder -> Rueckwand; Zuordnung von Hand,
zuordnung_r41.json); (2) EINE Tiefe je Rechteck = max bucket ueber alle Punkte (Regel NB1/NB2) - bei einem zurueckweichenden
Gegenstand liegt die Tiefe des fernen Endes auch ueber dem nahen Ende, eine Figur hinter dem NAHEN Teil wird nicht verdeckt;
(3) Zellentyp 2 (Raute des Resolvers, push_diag2) war im Python-Modell ein Kasten (koerper_r41.py Kopf).

REGEL (je Cut, je Koerper):
  1. Koerper = Zelle + Hoehe: aus der Zuordnung von Hand (zuordnung_r41.json; Hoehe an ALLEN Objekten derselben Zelle im Cut
     gemessen, koerper_r41.hoehe_messen), sonst aus den Modell-Zeilen (tests/unit/gen_r41_masken36_zellen.inc) bzw. dem Bericht
     von zellen3d_r41.py fuer die weiter ausgeschlossenen Modell-Rechtecke (OFFEN 9). Typ-2-Zellen werden mit der Raute neu
     gemessen. Kein Koerper, wenn die Kamera ueber dem Grundriss steht (Nahfeld wie r41_koerper_davor -1).
  2. Rechtecke des Koerpers = alle Rechtecke, deren deckende Punkte in Objekten dieses Koerpers liegen (alte und Saum-Rechtecke).
  3. Je Bildpunkt p des Koerpers: Eintritt e(p) und Austritt a(p) des Sehstrahls (koerper_r41.ein_aus, Form des Resolvers).
     Ein Teilrechteck mit Tiefe D verdeckt eine Figur an p genau dann, wenn D < bucket(vz_Figur) (re15_pri_mask_occludes:
     @0x8002565c otz>>4, ZSF3 341 @0x80066c70, Masken-OT = depth @0x80039650). D = max bucket(e) ueber die Koerperpunkte des
     Teils -> eine Figur VOR dem Koerper (vz < e) wird nie verdeckt; eine Figur HINTER dem Koerper (vz > a) wird verdeckt, wenn
     D < bucket(a(p)). Punkte mit bucket(a(p)) <= D heissen VERLETZT.
  4. Spaltengrenzen je Koerper: dynamische Programmierung ueber die Bildspalten, Ziel = wenigste verletzte Koerperpunkte bei K
     Streifen; jedes Rechteck des Koerpers wird an den Grenzen geteilt (Teilrechteck = gleiche Atlas-Abbildung, Quelle und Ziel
     um dieselbe Spaltenzahl verschoben -> gleiche Texel, gleiche Gruppe, Lage im Bild unveraendert). Platz: hoechstens 105
     Rechtecke je Cut (re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT); K je Koerper gierig nach dem groessten Gewinn je Rechteck.
  5. Ein Teil ohne Koerperpunkt behaelt die Tiefe seines Rechtecks. Rechtecke auf der Rueckholliste (--zurueck: vorher
     gezeichnet, nach der Zerlegung vom Ausschluss genommen) bleiben ungeteilt mit alter Tiefe.
  6. Danach rechnet test_r37_masken36 "erzeuge" den Ausschluss neu (3D-Urteil mit den Modell-Zeilen dieses Werkzeugs).
Atlas (_PRIcc.TIM) und Gruppen-Anker bleiben unveraendert; Leon- und Elza-Datei bleiben bytegleich.

Aufruf:
  python re15_port/tools/maske/kacheln_r41.py --quelle <MASKS vorher> --aus <Ausschluss vorher> --tabelle <Modell-Tabelle vorher>
         --bericht <zellen3d_r41.json> --cache <Regionen-Cache> --out <Ordner> --tabelle-aus <neue Modell-Tabelle>
         [--zurueck <json>] [ROOMxxxx ...]
"""
import argparse
import json
import os
import re
import struct
import sys

import numpy as np
from scipy import ndimage

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import geom                      # noqa: E402
import koerper_r41 as K          # noqa: E402
import maskenbild                # noqa: E402
import schliessen_r41 as SR      # noqa: E402
import tiefe_zelle_r41 as TZ     # noqa: E402
import zellen3d_r41 as Z3        # noqa: E402

PSX = SR.PSX
MAX_MASKEN = 105                 # re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT (Original-Maximum RDT-Header Byte[7])
K_MAX = 40                       # Obergrenze der DP je Koerper (Rechenaufwand; der Platz begrenzt frueher)


def tim_mit_index(tb, idx):
    """8-bpp-TIM: CLUT-Block bytegleich lassen, Indexbild ersetzen (Leser wie maskenbild.lies_tim_bytes)."""
    magic, flag = struct.unpack_from("<II", tb, 0)
    if magic != 0x10:
        raise SystemExit("kein TIM")
    o = 8
    if flag & 8:
        blen = struct.unpack_from("<I", tb, o)[0]
        o += blen
    blen, px, py, pw, ph = struct.unpack_from("<IHHHH", tb, o)
    if idx.shape != (ph, pw * 2):
        raise SystemExit("TIM-Groesse %s != %s" % (idx.shape, (ph, pw * 2)))
    return tb[:o + 12] + idx.astype(np.uint8).tobytes() + tb[o + 12 + pw * 2 * ph:]


def tabelle_lesen(pfad):
    out = {}
    rx = re.compile(r"\{ 0x([0-9A-F]{4}),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+),\s*(\d+),\s*(\d+), (\d),\s*(\d+),\s*(-?\d+), (\d),"
                    r"\s*(\d+), 0x([0-9A-F]{5}), \{ ([^}]*) \} \},\s*/\* IoU ([0-9.]+)")
    for m in rx.finditer(open(pfad).read()):
        g = m.groups()
        out[(int(g[0], 16), int(g[1]), int(g[2]))] = dict(
            x=int(g[3]), z=int(g[4]), w=int(g[5]), d=int(g[6]), typ=int(g[7]), band=int(g[8]), hoehe=int(g[9]),
            saeule=int(g[10]), tiefe=int(g[11]), off=int(g[12], 16), iou=float(g[14]))
    return out


def tabelle_schreiben(pfad, zeilen):
    with open(pfad, "w", newline="\n") as t:
        t.write("/* Erzeugt von re15_port/tools/maske/kacheln_r41.py (Nachbesserung 3) auf zellen3d_r41.py (Nachbesserung 2) - NICHT von\n"
                " * Hand aendern. Runde 41 Spur M: Generator-Rechtecke der Stages 3..6 mit Zellen-Modell (Koerper).\n"
                " * Je Zeile: Raum (Leon-Datei; Elza xxx1 ist bytegleich), Cut, Rechteck, SCA-Zelle x z breite tiefe, Typ, Band,\n"
                " * gemessene Hoehe (IoU-Maximum, negativ = nach oben), Saeule, Tiefe im MSK; Beleg: RDT-Offset und die\n"
                " * 12 Bytes des SCA-Eintrags dort (u16 breite, u16 tiefe, s16 x, s16 z, typ, u0, u1, floor); Kommentar: IoU.\n"
                " * Benutzt von tests/unit/test_r37_masken36.c (r41_koerper_davor) und tests/unit/test_r41_masken36.c (zellen3d). */\n"
                "static const r41_zellen_modell_t r41_masken36_zellen[] = {\n")
        for (rid, cut, i, zl, typ, band, hoehe, saeule, neu, o, b, iou) in sorted(zeilen, key=lambda z: (z[0], z[1], z[2])):
            t.write("    { 0x%04X, %2d, %3d, %6d, %6d, %5d, %5d, %d, %2d, %5d, %d, %3d, 0x%05X, { %s } },   /* IoU %.2f */\n"
                    % (rid, cut, i, zl[0], zl[1], zl[2], zl[3], typ, band, hoehe, 1 if saeule else 0, neu, o,
                       ", ".join("0x%02x" % c for c in b), iou))
        t.write("    { 0x0000, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 } }   /* Ende */\n};\n")


def dp_streifen(hit, be, ba, c0, c1, k_max, fig=None):
    """Spaltengrenzen fuer K = 1..k_max Streifen ueber die Spalten c0..c1-1. -> {K: (verletzt, [(a, b, D)])}.
    Kosten eines Streifens [a, b) (D = max bucket(Eintritt) im Streifen): ohne fig die Koerperpunkte mit bucket(Austritt) <= D;
    mit fig (320 x 1025 Zaehler je Bildspalte und Stufe, test_r37_masken36 hinter) die erreichbaren Figurpunkte HINTER dem Koerper,
    deren Stufe bucket(vz - 450) <= D ist - die blieben trotz des Teilrechtecks sichtbar."""
    n = c1 - c0
    NB = 1024 + 1
    emax = np.where(hit, be, -1)
    spm = emax[:, c0:c1].max(0).astype(np.int64)        # max Eintritt je Spalte
    cnt = np.zeros((n, NB), np.int64)                  # je Spalte: Zahl der Punkte mit Stufe <= D
    for j in range(n):
        if fig is not None:
            cnt[j] = np.cumsum(fig[c0 + j])
            continue
        v = np.clip(ba[hit[:, c0 + j], c0 + j], 0, NB - 1).astype(np.int64)
        if len(v):
            h = np.bincount(v, minlength=NB)
            cnt[j] = np.cumsum(h)
    P = np.zeros((n + 1, NB), np.int64)
    P[1:] = np.cumsum(cnt, 0)
    C = np.zeros((n + 1, n + 1), np.int64)
    DD = np.full((n + 1, n + 1), -1, np.int64)
    for a in range(n):
        D = np.maximum.accumulate(spm[a:])               # D fuer b = a+1 .. n
        bs = np.arange(a + 1, n + 1)
        DD[a, a + 1:] = D
        Dc = np.clip(D, 0, NB - 1)
        C[a, a + 1:] = np.where(D >= 0, P[bs, Dc] - P[a, Dc], 0)
    INF = 1 << 60
    best = np.full((k_max + 1, n + 1), INF, np.int64)
    arg = np.zeros((k_max + 1, n + 1), np.int64)
    best[0, 0] = 0
    out = {}
    for k in range(1, k_max + 1):
        for b in range(1, n + 1):
            v = best[k - 1, :b] + C[:b, b]
            a = int(np.argmin(v))
            best[k, b] = v[a]
            arg[k, b] = a
        if best[k, n] >= INF:
            continue
        gr, b, kk = [], n, k
        while kk > 0:
            a = int(arg[kk, b])
            gr.append((c0 + a, c0 + b, int(DD[a, b])))
            b = a
            kk -= 1
        out[k] = (int(best[k, n]), gr[::-1])
        if best[k, n] == 0:
            break
    return out


def zerlegen(rechteck, grenzen, richtung="x"):
    """Rechteck an Streifengrenzen -> Liste (dx, dy, breite, hoehe) der Teile. richtung "x": Bildspalten, "y": Bildzeilen."""
    if richtung == "x":
        P, L = rechteck["X"], rechteck["w"]
    else:
        P, L = rechteck["Y"], rechteck["h"]
    teile = []
    for a, b, _D in grenzen:
        lo, hi = max(a, P), min(b, P + L)
        if hi > lo:
            teile.append((lo - P, hi - lo))
    # Stuecke des Rechtecks ausserhalb aller Streifen (Bildrand) dem naechsten Teil zuschlagen
    if not teile:
        teile = [(0, L)]
    if teile[0][0] > 0:
        teile[0] = (0, teile[0][0] + teile[0][1])
    end = teile[-1][0] + teile[-1][1]
    if end < L:
        teile[-1] = (teile[-1][0], L - teile[-1][0])
    if richtung == "x":
        return [(o, 0, l, rechteck["h"]) for o, l in teile]
    return [(0, o, rechteck["w"], l) for o, l in teile]


def cut_bearbeiten(room, cut, blob, sek, idx, rdt, cam, cache, aus_set, modelle, bericht_cut, zuordnung, zurueck, rid,
                   auswahl=None, hinter=None):
    """-> (neue Sektion bytes oder None, Modell-Zeilen, Bericht)"""
    v = geom.cut_view(rdt, cam, cut)
    if not v or v[2] <= 0:
        return None, [], {"grund": "keine Ansicht"}, None
    R, t, H = v
    masks = sek["masks"]
    n = len(masks)
    recs = []
    op = idx != 0
    for i, m in enumerate(masks):
        X, Y = SR.bildlage(sek, m)
        sx, sy, w, h, dep = m[0], m[1], m[6], m[7], m[4]
        d = np.zeros((240, 320), bool)
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 > x0 and y1 > y0:
            sub = op[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
            if sub.shape == (y1 - y0, x1 - x0):
                d[y0:y1, x0:x1] = sub
        recs.append(dict(i=i, sx=sx, sy=sy, X=X, Y=Y, w=w, h=h, dep=dep, gi=m[8], d=d))
    grown = SR.gewachsen(None, None, None, cache)
    per, _alle = SR.region_im_bild(grown)
    labs = {sh: ndimage.label(f, structure=np.ones((3, 3)))[0] for sh, f in per.items()}
    for r in recs:
        r["obj"] = None
        if not r["d"].any():
            continue
        shs = [sh for sh, f in per.items() if f[r["d"]].all()]
        if shs:
            sh = shs[0]
            r["obj"] = (sh, tuple(sorted(set(int(c) for c in labs[sh][r["d"]]))))
    for r in recs:
        # alt = Generator-Rechteck an seiner BSS-Stelle: Ziel - Quelle == Verschiebung (schliessen_r41 Probe 1); sonst Saum-Rechteck
        r["saum"] = r["obj"] is None or (r["X"] - r["sx"], r["Y"] - r["sy"]) != r["obj"][0]
    # ---- Koerper je Objekt (sh, comp) ----
    koerper = {}          # schluessel -> dict(typ, x, z, w, d, band, hoehe, saeule, off, bytes, iou, quelle)
    obj_koerper = {}      # (sh, comp) -> schluessel
    c = TZ.kameraort(R, t)

    def nahfeld(z):
        return z[0] <= c[0] <= z[0] + z[2] and z[1] <= c[2] <= z[1] + z[3]

    # (a) Zuordnung von Hand
    hand = zuordnung.get((room, cut), [])
    gruppen_hand = {}
    for e in hand:
        sh = tuple(e["verschiebung"])
        if sh not in labs:
            raise SystemExit("%s C%d Zuordnung %s: Verschiebung %s nicht in der Region" % (room, cut, e.get("objekt"), sh))
        comp = int(labs[sh][e["punkt"][1], e["punkt"][0]])
        if not comp:
            raise SystemExit("%s C%d Zuordnung %s: Punkt %s nicht im Objekt" % (room, cut, e.get("objekt"), e["punkt"]))
        gruppen_hand.setdefault(e["zelle"], []).append((sh, comp, e))
    for zoff, lst in gruppen_hand.items():
        z, band = TZ.zelle_von_hand(rdt, lst[0][2])
        if nahfeld(z):
            continue
        mess = np.zeros((240, 320), bool)
        for sh, comp, _e in lst:
            mess |= labs[sh] == comp
        hm = K.hoehe_messen(mess, R, t, H, z[4], z[0], z[1], z[2], z[3], band)
        if hm is None:
            continue
        o, b = Z3.sca_beleg(rdt, z[:4], z[4], band)
        key = ("hand", zoff)
        koerper[key] = dict(typ=int(z[4]), x=z[0], z=z[1], w=z[2], d=z[3], band=band, hoehe=int(hm[0]), saeule=bool(hm[2]),
                            off=o, bytes=b, iou=float(hm[1]), quelle="Hand %s" % zoff)
        for sh, comp, _e in lst:
            obj_koerper[(sh, comp)] = key
    # (b) Modell-Zeilen + Bericht (OFFEN 9)
    auto = {}
    for r in recs:
        mz = modelle.get((rid, cut, r["i"]))
        if mz is None:
            for k in bericht_cut:
                if k["i"] == r["i"] and k.get("zellen") and k.get("zurueck"):
                    z0 = k["zellen"][0]
                    if all((cz["zelle"], cz["typ"], cz["band"], cz["hoehe"], cz["saeule"]) ==
                           (z0["zelle"], z0["typ"], z0["band"], z0["hoehe"], z0["saeule"]) for cz in k["zellen"]):
                        mz = dict(x=z0["zelle"][0], z=z0["zelle"][1], w=z0["zelle"][2], d=z0["zelle"][3], typ=z0["typ"],
                                  band=z0["band"], hoehe=z0["hoehe"], saeule=int(z0["saeule"]), iou=z0["iou"])
        if mz is None or r["obj"] is None:
            continue
        sh, comps = r["obj"]
        for comp in comps:
            if (sh, comp) in obj_koerper and obj_koerper[(sh, comp)][0] == "hand":
                continue
            auto.setdefault((sh, comp), set()).add((mz["x"], mz["z"], mz["w"], mz["d"], mz["typ"], mz["band"], mz["hoehe"],
                                                     mz["saeule"], mz.get("iou", 0.0)))
    for (sh, comp), ks in auto.items():
        if len(set(k[:8] for k in ks)) != 1:
            continue                                   # Objekt mit verschiedenen Zellen: kein Koerper
        x, z_, w, d, typ, band, hoehe, saeule, iou = sorted(ks)[0]
        if nahfeld((x, z_, w, d)):
            continue
        if typ == 2:                                   # Raute neu messen (koerper_r41 Kopf)
            hm = K.hoehe_messen(labs[sh] == comp, R, t, H, typ, x, z_, w, d, band)
            if hm is None:
                continue
            hoehe, iou, saeule = int(hm[0]), float(hm[1]), bool(hm[2])
        o, b = Z3.sca_beleg(rdt, (x, z_, w, d), typ, band)
        key = ("auto", x, z_, w, d, typ, band, hoehe, int(saeule), sh, comp)
        koerper[key] = dict(typ=typ, x=x, z=z_, w=w, d=d, band=band, hoehe=hoehe, saeule=bool(saeule), off=o, bytes=b, iou=iou,
                            quelle="Modell")
        obj_koerper[(sh, comp)] = key
    # ---- Rechtecke je Koerper ----
    grp = {}
    for r in recs:
        r["k"] = None
        if r["obj"] is None or r["i"] in zurueck:
            continue
        sh, comps = r["obj"]
        ks = set(obj_koerper.get((sh, cc)) for cc in comps)
        if len(ks) == 1 and None not in ks:
            r["k"] = ks.pop()
            grp.setdefault(r["k"], []).append(r)
    if auswahl is not None:
        # nur Koerper mit gemessenen "zu fern"-Punkten (test_r37_masken36 fern), Zuordnungen von Hand und Koerper mit vorher
        # ausgeschlossenen Rechtecken (OFFEN 9: Modell da, aber nicht jeder Punkt hinter dem Koerper -> Teile nur aus Koerperpunkten)
        grp = {key: rl for key, rl in grp.items()
               if key[0] == "hand" or any(r["i"] in auswahl for r in rl) or any(r["i"] in aus_set for r in rl)}
    bericht = {"koerper": {}, "n_vorher": n}
    if not grp:
        return None, [], bericht, None
    # ---- Eintritt/Austritt, DP je Koerper und Richtung ----
    loes = {}
    for key, rl in grp.items():
        kp = koerper[key]
        ve, va, ok = K.ein_aus(R, t, H, kp["typ"], kp["x"], kp["z"], kp["w"], kp["d"], kp["band"], kp["hoehe"], kp["saeule"])
        be, ba = K.bucket(ve), K.bucket(va)
        U = np.zeros((240, 320), bool)
        for r in rl:
            U |= r["d"]
        hit = U & ok
        if not hit.any():
            continue
        sol = {}
        for richtung in ("x", "y"):
            fig = hinter.get((rid, cut, kp["off"], kp["hoehe"], richtung)) if hinter else None
            if richtung == "x":
                ach = np.nonzero(hit.any(0))[0]
                sol["x"] = dp_streifen(hit, be, ba, int(ach.min()), int(ach.max()) + 1, K_MAX, fig)
            else:
                ach = np.nonzero(hit.any(1))[0]
                sol["y"] = dp_streifen(hit.T, be.T, ba.T, int(ach.min()), int(ach.max()) + 1, K_MAX, fig)
        saum_px = np.zeros((240, 320), bool)
        for r in rl:
            if r["saum"]:
                saum_px |= r["d"]
        loes[key] = dict(be=be, ba=ba, ok=ok, hit=hit, sol=sol, rl=rl, saum_px=saum_px)

    def saum_bereiche(gr):
        """Bereiche der Streifen fuer Saum-Punkte (aeusserste Streifen bis an den Bildrand verlaengert)."""
        out = []
        for j, (a_, b_, _D) in enumerate(gr):
            out.append((-10 ** 6 if j == 0 else a_, 10 ** 6 if j == len(gr) - 1 else b_))
        return out

    texel = {}
    for r in recs:
        if r["saum"]:
            ys, xs = np.nonzero(r["d"])
            for y, x in zip(ys, xs):
                texel.setdefault((int(y), int(x)), int(idx[r["sy"] + (y - r["Y"]), r["sx"] + (x - r["X"])]))

    def bauen(wahl):
        """wahl: {schluessel: (richtung, K, zusammenlegen, trennen, zeilen)} -> (neue Rechtecke, neuer Atlas) oder (None, None),
        wenn der Atlas voll ist. zusammenlegen: Saum-Rechtecke des Koerpers je Streifen zusammenlegen; trennen: dabei Koerperpunkte und
        Punkte ausserhalb getrennt; zeilen: vorher ausgeschlossene alte Rechtecke quer zur Streifenrichtung in reine Koerper-Laeufe
        und Rest-Laeufe teilen (OFFEN 9). Rechteck: (sx, sy, X, Y, w, h, tiefe, gruppe oder None, herkunft, schluessel oder None)."""
        entfernt = set()
        for key, (ri, k, zus, tr_, zl) in wahl.items():
            if zus:
                entfernt |= set(r["i"] for r in loes[key]["rl"] if r["saum"])
        belegt = np.zeros(idx.shape, bool)
        for r in recs:
            if r["i"] not in entfernt:
                belegt[r["sy"]:r["sy"] + r["h"], r["sx"]:r["sx"] + r["w"]] = True
        idx_neu = idx.copy()
        idx_neu[~belegt] = 0                          # Flaechen entfernter Saum-Rechtecke freigeben
        neu = []
        for r in recs:
            key = r["k"]
            if key is None or key not in wahl:
                # unveraendert; eine vorhandene Modell-Zeile wandert mit (neuer Index, gleiche Zelle und Tiefe)
                neu.append((r["sx"], r["sy"], r["X"], r["Y"], r["w"], r["h"], r["dep"], r["gi"], [r["i"]],
                            ("zeile", r["i"]) if (rid, cut, r["i"]) in modelle else None))
                continue
            L = loes[key]
            ri, k, zus, tr_, zl = wahl[key]
            gr = L["sol"][ri][k][1]
            if r["saum"]:
                if zus:
                    continue
                th = r["d"] & L["ok"]                 # Saum-Rechteck bleibt, Tiefe aus dem Koerper
                D = int(L["be"][th].max()) if th.any() else int(r["dep"])
                neu.append((r["sx"], r["sy"], r["X"], r["Y"], r["w"], r["h"], D, r["gi"], [r["i"]], key if th.any() else None))
                continue
            for (dx, dy, ww, hh) in zerlegen(r, gr, ri):
                teil = np.zeros((240, 320), bool)
                x0, x1 = max(0, r["X"] + dx), min(320, r["X"] + dx + ww)
                y0, y1 = max(0, r["Y"] + dy), min(240, r["Y"] + dy + hh)
                if x1 > x0 and y1 > y0:
                    teil[y0:y1, x0:x1] = r["d"][y0:y1, x0:x1]
                stuecke = [(dx, dy, ww, hh)]
                if zl and r["i"] in aus_set:
                    # quer zur Streifenrichtung: Laeufe nur mit Koerperpunkten / mit Punkten ausserhalb des Koerpers
                    quer = hh if ri == "x" else ww
                    rein = []
                    for q in range(quer):
                        if ri == "x":
                            Yy = r["Y"] + dy + q
                            zr = teil[Yy, :] if 0 <= Yy < 240 else np.zeros(320, bool)
                            zo = L["ok"][Yy, :] if 0 <= Yy < 240 else np.zeros(320, bool)
                        else:
                            Xx = r["X"] + dx + q
                            zr = teil[:, Xx] if 0 <= Xx < 320 else np.zeros(240, bool)
                            zo = L["ok"][:, Xx] if 0 <= Xx < 320 else np.zeros(240, bool)
                        rein.append(bool(zr.any()) and int((zr & zo).sum()) == int(zr.sum()))
                    stuecke, q0 = [], 0
                    for q in range(1, quer + 1):
                        if q == quer or rein[q] != rein[q0]:
                            if ri == "x":
                                stuecke.append((dx, dy + q0, ww, q - q0))
                            else:
                                stuecke.append((dx + q0, dy, q - q0, hh))
                            q0 = q
                for (ex, ey, ew, eh) in stuecke:
                    tz = np.zeros((240, 320), bool)
                    x0, x1 = max(0, r["X"] + ex), min(320, r["X"] + ex + ew)
                    y0, y1 = max(0, r["Y"] + ey), min(240, r["Y"] + ey + eh)
                    if x1 > x0 and y1 > y0:
                        tz[y0:y1, x0:x1] = r["d"][y0:y1, x0:x1]
                    th = tz & L["ok"]
                    D = int(L["be"][th].max()) if th.any() else int(r["dep"])
                    neu.append((r["sx"] + ex, r["sy"] + ey, r["X"] + ex, r["Y"] + ey, ew, eh, D, r["gi"], [r["i"]],
                                key if th.any() else None))
        for key in sorted(wahl, key=str):
            ri, k, zus, tr_, zl = wahl[key]
            if not zus:
                continue
            L = loes[key]
            gr = L["sol"][ri][k][1]
            stuecke = []
            for (a_, b_) in saum_bereiche(gr):
                m = np.zeros((240, 320), bool)
                if ri == "x":
                    lo, hi = max(0, a_), min(320, b_)
                    if hi > lo:
                        m[:, lo:hi] = L["saum_px"][:, lo:hi]
                else:
                    lo, hi = max(0, a_), min(240, b_)
                    if hi > lo:
                        m[lo:hi, :] = L["saum_px"][lo:hi, :]
                stuecke += [m & L["ok"], m & ~L["ok"]] if tr_ else [m]
            for m in stuecke:
                if not m.any():
                    continue
                ys, xs = np.nonzero(m)
                teile = [m]
                if SR.frei_platz(belegt, int(xs.max() - xs.min() + 1), int(ys.max() - ys.min() + 1)) is None:
                    lab, nl = ndimage.label(m, structure=np.ones((3, 3)))
                    teile = [lab == j for j in range(1, nl + 1)]
                for tm in teile:
                    ys, xs = np.nonzero(tm)
                    X0, Y0 = int(xs.min()), int(ys.min())
                    w, h = int(xs.max() - X0 + 1), int(ys.max() - Y0 + 1)
                    pl = SR.frei_platz(belegt, w, h)
                    if pl is None:
                        return None, None
                    ax, ay = pl
                    belegt[ay:ay + h, ax:ax + w] = True
                    idx_neu[ay:ay + h, ax:ax + w] = 0
                    for y, x in zip(ys, xs):
                        idx_neu[ay + (y - Y0), ax + (x - X0)] = texel[(int(y), int(x))]
                    th = tm & L["ok"]
                    herk = sorted(set(r["i"] for r in L["rl"] if r["saum"] and (r["d"] & tm).any()))
                    D = int(L["be"][th].max()) if th.any() else max(recs[i]["dep"] for i in herk)
                    neu.append((ax, ay, X0, Y0, w, h, D, None, herk, key if th.any() else None))
        return neu, idx_neu

    def verletzt(key, w):
        ri, k = w[0], w[1]
        return loes[key]["sol"][ri][k][0]

    # Start: je Koerper die erste machbare Wahl mit K = 1 (Saum zusammen+getrennt, zusammen, stehen lassen; OFFEN-9-Teilung an/aus)
    optionen1 = [(True, True, True), (True, False, True), (False, False, True), (False, False, False)]
    wahl = {key: ("x", 1, False, False, False) for key in loes}
    neu, idx_neu = bauen(wahl)                        # == Quelle (jedes Rechteck ein Teil, Tiefe aus dem Koerper)
    if neu is None or len(neu) > MAX_MASKEN:
        raise SystemExit("%s C%d: Grundfall passt nicht (%s)" % (room, cut, None if neu is None else len(neu)))
    for key in sorted(loes, key=str):
        for (zus, tr_, zl) in optionen1:
            w2 = dict(wahl)
            w2[key] = ("x", 1, zus, tr_, zl)
            n2, i2 = bauen(w2)
            if n2 is not None and len(n2) <= MAX_MASKEN:
                wahl, neu, idx_neu = w2, n2, i2
                break
    # Gierig: je Schritt der Koerper/Wahl mit dem groessten Rueckgang der verletzten Punkte je zusaetzlichem Rechteck
    while True:
        best = None
        for key, L in loes.items():
            v0 = verletzt(key, wahl[key])
            for ri in ("x", "y"):
                for k2 in sorted(L["sol"][ri])[:12]:
                    dv = v0 - L["sol"][ri][k2][0]
                    if dv <= 0:
                        continue
                    for (zus, tr_, zl) in optionen1:
                        w2 = dict(wahl)
                        w2[key] = (ri, k2, zus, tr_, zl)
                        n2, i2 = bauen(w2)
                        if n2 is None or len(n2) > MAX_MASKEN:
                            continue
                        q = dv / max(len(n2) - len(neu), 0.5)
                        if best is None or q > best[0]:
                            best = (q, w2, n2, i2)
                        break
        if best is None:
            break
        _q, wahl, neu, idx_neu = best
    if len(neu) > MAX_MASKEN:
        raise SystemExit("%s C%d: %d Rechtecke > %d" % (room, cut, len(neu), MAX_MASKEN))
    # Gruppen: alte Gruppen mit neuer Zahl (Reihenfolge und Anker bleiben), neue Saum-Stuecke je eine eigene Gruppe am Ende
    gcount = [0] * len(sek["groups"])
    for e in neu:
        if e[7] is not None:
            gcount[e[7]] += 1
    alte = [e for e in neu if e[7] is not None]
    neue_saum = [e for e in neu if e[7] is None]
    alte.sort(key=lambda e: e[7])                     # stabil: innerhalb der Gruppe bleibt die Reihenfolge
    groups = [(gcount[g], sek["groups"][g][2], sek["groups"][g][3]) for g in range(len(sek["groups"])) if gcount[g] > 0]
    for e in neue_saum:
        groups.append((1, e[2] - e[0], e[3] - e[1]))   # Anker = Ziel - Quelle (wie schliessen_r41)
    alle = alte + neue_saum
    sec = geom.pack_section(groups, [(e[0], e[1], e[2], e[3], e[4], e[5], e[6]) for e in alle])
    zeilen = []
    for j, e in enumerate(alle):
        if e[9] is None:
            continue
        if e[9][0] == "zeile":                        # mitgewanderte Zeile eines unveraenderten Rechtecks
            mz = modelle[(rid, cut, e[9][1])]
            o, b_ = Z3.sca_beleg(rdt, (mz["x"], mz["z"], mz["w"], mz["d"]), mz["typ"], mz["band"])
            zeilen.append((rid, cut, j, (mz["x"], mz["z"], mz["w"], mz["d"]), mz["typ"], mz["band"], mz["hoehe"], mz["saeule"],
                           e[6], o, b_, mz["iou"]))
            continue
        kp = koerper[e[9]]
        zeilen.append((rid, cut, j, (kp["x"], kp["z"], kp["w"], kp["d"]), kp["typ"], kp["band"], kp["hoehe"], kp["saeule"], e[6],
                       kp["off"], kp["bytes"], kp["iou"]))
    for key, L in loes.items():
        kp = koerper[key]
        ri, k, zus, tr_, zl = wahl[key]
        bericht["koerper"]["%s" % (key,)] = dict(quelle=kp["quelle"], zelle=[kp["x"], kp["z"], kp["w"], kp["d"]], typ=kp["typ"],
                                                  band=kp["band"], hoehe=kp["hoehe"], saeule=kp["saeule"], iou=round(kp["iou"], 3),
                                                  rechtecke=[r["i"] for r in L["rl"]], richtung=ri, K=k, saum_zusammen=zus,
                                                  saum_getrennt=tr_, offen9_teilung=zl,
                                                  verletzt_1=L["sol"]["x"][1][0], verletzt=L["sol"][ri][k][0],
                                                  koerperpunkte=int(L["hit"].sum()), streifen=L["sol"][ri][k][1],
                                                  figurdaten=bool(hinter and (rid, cut, kp["off"], kp["hoehe"], ri) in hinter))
    bericht["n_nachher"] = len(alle)
    bericht["herkunft"] = [e[8] for e in alle]
    return sec, zeilen, bericht, idx_neu


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--quelle", required=True)
    ap.add_argument("--aus", required=True)
    ap.add_argument("--tabelle", required=True)
    ap.add_argument("--bericht", default=None, help="zellen3d_r41.json (Modelle der weiter ausgeschlossenen Rechtecke)")
    ap.add_argument("--cache", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--tabelle-aus", required=True)
    ap.add_argument("--zurueck", default=None, help="json {raum: {cut: [rechteck, ...]}}: ungeteilt lassen (Index der Quelle)")
    ap.add_argument("--zuordnung", default=os.path.join(HIER, "zuordnung_r41.json"))
    ap.add_argument("--fern", default=None, help="Ausgabe von 'test_r37_masken36 fern': nur Koerper mit zu fernen Punkten (+ Hand)")
    ap.add_argument("--hinter", default=None, help="Ausgabe von 'test_r37_masken36 hinter': Figurpunkte hinter den Koerpern je Spalte")
    a = ap.parse_args()
    hinter = {}
    if a.hinter:
        for zl in open(a.hinter, encoding="utf-8", errors="replace"):
            f = zl.split()
            if len(f) == 8 and f[0] in ("H", "R"):
                ri = "x" if f[0] == "H" else "y"
                key = (int(f[1], 16), int(f[2]), int(f[3]), int(f[4]), ri)
                if key not in hinter:
                    hinter[key] = np.zeros((320 if ri == "x" else 240, 1025), np.int64)
                hinter[key][int(f[5]), min(int(f[6]), 1024)] += int(f[7])
    fern = {}
    if a.fern:
        for zl in open(a.fern, encoding="utf-8", errors="replace"):
            m = re.match(r"\s*ROOM([0-9A-F]{4}) C\s*(\d+)\s*: zu fern .*\|(.*)$", zl)
            if m and not int(m.group(1), 16) & 1:
                fern[(int(m.group(1), 16), int(m.group(2)))] = set(int(x) for x in re.findall(r" R(\d+) \d+ \(", m.group(3)))
    zuordnung = {}
    for room, cuts in json.load(open(a.zuordnung, encoding="utf-8")).items():
        if room.startswith("ROOM"):
            for cut, liste in cuts.items():
                zuordnung[(room, int(cut))] = liste
    aus = TZ.ausschluss(a.aus)
    modelle = tabelle_lesen(a.tabelle)
    berichtz = json.load(open(a.bericht)) if a.bericht else {}
    zur = json.load(open(a.zurueck)) if a.zurueck else {}
    os.makedirs(a.out, exist_ok=True)
    alle_zeilen, gesamt = [], {}
    # Zeilen der Raeume/Cuts, die nicht bearbeitet werden, bleiben (Index unveraendert)
    bearbeitet = set()
    for f in sorted(os.listdir(a.quelle)):
        if not re.match(r"ROOM[3-6][0-9A-F]{2}0\.MSK$", f):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        if a.raeume and room not in a.raeume:
            continue
        blob = open(os.path.join(a.quelle, f), "rb").read()
        if open(os.path.join(a.quelle, "ROOM%04X.MSK" % (rid | 1)), "rb").read() != blob:
            raise SystemExit("%s: Elza-Datei nicht bytegleich" % room)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "%s.RDT" % room), "rb").read()
        cam = struct.unpack_from("<I", rdt, 0x24)[0]
        secs, n_cut = SR.sektionen(blob)
        neue = {}
        tims = {}
        b_raum = {}
        for cut, sek in sorted(secs.items()):
            tim = os.path.join(a.quelle, "%s_PRI%02d.TIM" % (room, cut))
            if not os.path.exists(tim):
                continue
            idx = maskenbild.lies_tim(tim)[0]
            cache = os.path.join(a.cache, "%s_%02d.npz" % (room, cut))
            if not os.path.exists(cache):
                continue
            zr = set(int(i) for i in zur.get(room, {}).get(str(cut), []))
            ausw = None
            if a.fern:
                ausw = set(fern.get((rid, cut), set()))
            sec, zeilen, b, idx_neu = cut_bearbeiten(room, cut, blob, sek, idx, rdt, cam, cache, aus.get((rid, cut), set()),
                                                     modelle, berichtz.get(room, {}).get(str(cut), []), zuordnung, zr, rid, ausw,
                                                     hinter)
            if sec is not None:
                neue[cut] = sec
                tims[cut] = tim_mit_index(open(tim, "rb").read(), idx_neu)
                bearbeitet.add((rid, cut))
                alle_zeilen += zeilen
                b_raum[cut] = b
                print("%s C%-2d Koerper %2d | Rechtecke %3d -> %3d | verletzt %6d -> %6d | Modell-Zeilen %3d" % (
                    room, cut, len(b["koerper"]), b["n_vorher"], b["n_nachher"],
                    sum(k["verletzt_1"] for k in b["koerper"].values()), sum(k["verletzt"] for k in b["koerper"].values()),
                    len(zeilen)), flush=True)
        offs = struct.unpack_from("<%dI" % n_cut, blob, 12)
        alt_secs = {}
        for cut in range(n_cut):
            if offs[cut]:
                ende = min([o for o in offs if o > offs[cut]] + [len(blob)])
                alt_secs[cut] = blob[offs[cut]:ende]
        alt_secs.update(neue)
        out = geom.pack_container(alt_secs, n_cut)
        for r in (rid, rid | 1):
            open(os.path.join(a.out, "ROOM%04X.MSK" % r), "wb").write(out)
            for cut, tb in tims.items():
                open(os.path.join(a.out, "ROOM%04X_PRI%02d.TIM" % (r, cut)), "wb").write(tb)
        gesamt[room] = b_raum
    # Modell-Zeilen: bearbeitete Cuts neu, uebrige unveraendert
    for (rid, cut, i), m in modelle.items():
        if (rid, cut) in bearbeitet:
            continue
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "ROOM%04X.RDT" % rid), "rb").read()
        o, b = Z3.sca_beleg(rdt, (m["x"], m["z"], m["w"], m["d"]), m["typ"], m["band"])
        alle_zeilen.append((rid, cut, i, (m["x"], m["z"], m["w"], m["d"]), m["typ"], m["band"], m["hoehe"], m["saeule"],
                            m["tiefe"], o, b, m["iou"]))
    tabelle_schreiben(a.tabelle_aus, alle_zeilen)
    json.dump(gesamt, open(os.path.join(a.out, "kacheln_r41.json"), "w"), indent=1, default=str)
    print("Tabelle: %d Zeilen -> %s" % (len(alle_zeilen), a.tabelle_aus), flush=True)


if __name__ == "__main__":
    main()
