"""Gegenmodelle fuer ROOM10F0 C4/C5 bauen — als ECHTE Asset-Dateien (MSK + TIM).

⛔ WARUM NICHT als Filter: der Filter der Sonde (R22_MASKENFILTER) kann nur Texel
   WEGNEHMEN. Eine andere Quaderhoehe oder eine andere Dunkelregel aendert auch die
   TIEFEN und die Kachelung — das ist nur an einem wirklich gebauten Asset messbar.

⛔ EICHUNG: die Variante "aus" (alle Schalter neutral) MUSS Sektion und TIM der
   Auslieferung bitgenau reproduzieren. `python variante.py eich` prueft das.

Der Kern ist eine Kopie von raum.objekt_regionen mit drei Schaltern:
   ohne_doppel  — Quader-Eintraege streichen, deren Zelle schon ein szene-Lasso traegt
   hoehe        — Quaderhoehe: None = -1950 wie ausgeliefert, "kunst" = die kleinste
                  Hoehe, die die Freistellung dieser Zelle ganz enthaelt (gemessen),
                  int = fester Wert
   dunkel       = "voll"  (Regel 1 + Regel 2, wie ausgeliefert)
                | "saum"  (nur Regel 1, Kunst-Saum <= 7 in derselben Zelle)
                | "aus"   (kein Tiefschwarz)
"""
import json
import os
import struct
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
WURZEL = os.path.abspath(os.path.join(HIER, "..", "..", "..", ".."))
sys.path.insert(0, os.path.join(WURZEL, "re15_port", "tools", "maske"))
os.chdir(WURZEL)

import anwenden
import geom
import maske_aus_png
import maskenbild
import raum
from geom import load_bg, load_rdt
from scipy import ndimage as nd

CD = "re15_port/shared_assets/PSX"
PPM = "build/bg_ppm"
MASKS = os.path.join(CD, "MASKS")
DUNKEL_SCHWELLE = 45          # raum.py:327
SAUM = 7                      # raum.py:336-346
QUADER_HOEHE = -1950          # auswahl.json, dritte Runde 2026-09-09


def kleinste_hoehe(v, zelle, kunst):
    """Kleinste Quaderhoehe, deren Silhouette die Kunst dieser Zelle ganz enthaelt."""
    zx, zz, zw, zd, _ = zelle
    for h in range(-100, -2601, -25):
        _vz, tr = geom.quader_tiefe(v[0], v[1], v[2], zx, zx + zw, zz, zz + zd, h)
        if not (kunst & ~tr).any():
            return h
    return QUADER_HOEHE


def regionen(room, cut, e, ohne_doppel=False, hoehe=None, dunkel="voll"):
    """Kopie von raum.objekt_regionen (Stand 2026-09-21) mit den drei Schaltern.

    Nur die markierten Stellen weichen ab; alles andere ist Zeile fuer Zeile
    uebernommen, damit die Eichung bitgenau bleibt.
    """
    rid = int(room[4:], 16)
    rdt = open(os.path.join(CD, "STAGE%d" % (rid >> 12), "ROOM%04X.RDT" % rid), "rb").read()
    cam_off = struct.unpack_from("<I", rdt, 0x24)[0]
    aus = []
    sperr = geom.sca_sperrzellen(rdt, 0) or []
    v2 = geom.cut_view(rdt, cam_off, cut)
    bgL = load_bg(PPM, rid, cut)

    # --- Kunst-Etiketten (raum.py:291-322), unveraendert -------------------------
    # (fuer die Hoehen-Variante brauchen wir sie ZWEIMAL: einmal mit der
    #  Auslieferungshoehe, um die Zellenzuordnung identisch zu halten.)
    sil0 = {}
    for ki, (zx, zz, zw, zd, typ) in enumerate(sperr):
        if typ != 3:
            continue
        sil0[ki] = geom.quader_tiefe(v2[0], v2[1], v2[2], zx, zx + zw, zz, zz + zd, QUADER_HOEHE)
    kunst_label = np.full((240, 320), -1, int)
    lasso_teile = {}
    for o2 in (e.get("objekte") or []):
        if "png" not in o2 or not all(k in o2 for k in ("x", "y")):
            continue
        r2 = maske_aus_png.setze(o2["png"], o2["x"], o2["y"], o2.get("massstab", 1))
        if r2 is None:
            continue
        lab2, n2 = nd.label(r2)
        teile = []
        for k2 in range(1, n2 + 1):
            comp = lab2 == k2
            npx = int(comp.sum())
            if npx < 20:
                continue
            bf, bki = 0.0, None
            for ki, (vzq, trq) in sil0.items():
                f = float((trq & comp).sum()) / npx
                if f > bf:
                    bf, bki = f, ki
            if bki is None:
                continue
            kunst_label[comp] = bki
            teile.append((bki, comp))
        lasso_teile[id(o2)] = teile

    # --- SCHALTER "hoehe": die Silhouetten, mit denen weitergerechnet wird -------
    sil = {}
    for ki, (zx, zz, zw, zd, typ) in enumerate(sperr):
        if typ != 3:
            continue
        h = QUADER_HOEHE
        if isinstance(hoehe, int):
            h = hoehe
        elif hoehe == "kunst":
            eig = kunst_label == ki
            h = kleinste_hoehe(v2, sperr[ki], eig) if eig.any() else QUADER_HOEHE
        sil[ki] = geom.quader_tiefe(v2[0], v2[1], v2[2], zx, zx + zw, zz, zz + zd, h)
    hoehen = {}
    for ki in sil:
        zx, zz, zw, zd, _ = sperr[ki]
        hoehen[ki] = (QUADER_HOEHE if hoehe is None
                      else hoehe if isinstance(hoehe, int)
                      else (kleinste_hoehe(v2, sperr[ki], kunst_label == ki)
                            if (kunst_label == ki).any() else QUADER_HOEHE))

    # --- Dunkel-Etiketten (raum.py:323-357) + SCHALTER "dunkel" ------------------
    dunkel_label = np.full((240, 320), -1, int)
    if bgL is not None and sil and dunkel != "aus":
        dk = bgL.astype(int).sum(2) < DUNKEL_SCHWELLE
        # Regel 1 (raum.py:336-346): Kunst-Saum <= 7 in der Silhouette DERSELBEN Zelle.
        # "loch" ersetzt den Saum durch das, was der Kommentar sagt: die LOECHER der
        # Freistellung (vom Lasso umschlossene Flaechen), nicht ihre Aufweitung.
        if dunkel in ("voll", "saum", "loch", "nurloch", "ohnekunst", "fern"):
            naechste = np.full((240, 320), np.inf)
            for ki in sorted(set(int(x) for x in kunst_label[kunst_label >= 0])):
                if ki not in sil:
                    continue
                if dunkel in ("loch", "nurloch", "ohnekunst", "fern"):
                    eig = kunst_label == ki
                    loecher = nd.binary_fill_holes(eig) & ~eig
                    m = dk & sil[ki][1] & loecher
                    dunkel_label[m] = ki
                else:
                    dist = nd.distance_transform_edt(~(kunst_label == ki))
                    m = dk & sil[ki][1] & (dist <= SAUM) & (dist < naechste)
                    naechste[m] = dist[m]
                    dunkel_label[m] = ki
        if dunkel in ("voll", "regel2", "loch", "ohnekunst", "fern"):
            # "ohnekunst": Regel 2 darf nur Zellen bedienen, deren Vordergrund wir NICHT
            # kennen. Wo der Nutzer freigestellt hat, ist die Freistellung die Wahrheit.
            # "fern": Zellen MIT Kunst nur noch WEIT ausserhalb ihrer Kunst bedienen -
            # der Saum an der Kunstkante ist ihr Antialias, nicht ihr Gegenstand.
            rest = dk & (dunkel_label < 0)
            naechstes = np.full((240, 320), np.inf)
            for ki, (vzq, trq) in sil.items():
                eigen = kunst_label == ki
                if eigen.any():
                    if dunkel == "ohnekunst":
                        continue
                    if dunkel == "fern":
                        d = nd.distance_transform_edt(~eigen)
                        trq = trq & (d > SAUM)
                m = rest & trq & (vzq < naechstes)
                naechstes[m] = vzq[m]
                dunkel_label[m] = ki

    # --- Objekte (raum.py:358-558), auf die hier benutzten Quellen gekuerzt ------
    szene_zellen = set()
    for o in e.get("objekte") or []:
        if o.get("tiefe") == "szene":
            for ki, _c in lasso_teile.get(id(o), []):
                szene_zellen.add(ki)
    for o in e.get("objekte") or []:
        if "png" in o:
            r = maske_aus_png.setze(o["png"], o["x"], o["y"], o["massstab"])
            if r is None:
                continue
        elif "quader" in o:
            q = [int(x) for x in o["quader"]]
            eig = None
            for ki2, (zx2, zz2, zw2, zd2, typ2) in enumerate(sperr):
                if typ2 == 3 and zx2 == q[0] and zz2 == q[1]:
                    eig = ki2
                    break
            # --- SCHALTER "ohne_doppel" -----------------------------------------
            if ohne_doppel and eig is not None and eig in szene_zellen:
                continue
            h = q[4] if hoehen.get(eig) is None else hoehen[eig]
            vz, r = geom.quader_tiefe(v2[0], v2[1], v2[2], q[0], q[0] + q[2],
                                      q[1], q[1] + q[3], h)
            if o.get("nur_kunst"):
                if eig is None:
                    r = r & (dunkel_label >= 0)
                else:
                    r = r & ((kunst_label == eig) | (dunkel_label == eig))
        else:
            raise SystemExit("Quelle nicht unterstuetzt: %s" % o.get("name"))
        assert not o.get("plus") and not o.get("minus") and not e.get("minus")
        assert o.get("oben", 0) == 0 and o.get("grow", 0) == 0
        if not r.any():
            continue
        if o.get("tiefe") == "szene":
            for ki, comp in lasso_teile.get(id(o), []):
                zx, zz, zw, zd, typ = sperr[ki]
                teil = comp | (dunkel_label == ki)
                aus.append(("%s [Stuhl %d,%d]" % (o.get("name", "?")[:20], zx, zz),
                            teil, None, None, None, None, None, None, None,
                            [zx, zz, zw, zd, hoehen.get(ki, QUADER_HOEHE)], None))
            continue
        aus.append((o.get("name", "?"), r, o.get("fuss"), None, None, None,
                    o.get("flach"), o.get("tiefe"), o.get("zelle"),
                    ([q[0], q[1], q[2], q[3], h] if "quader" in o else o.get("quader")),
                    o.get("tiefenfaktor")))
    return aus


def bauen(zielverz, **schalter):
    os.makedirs(zielverz, exist_ok=True)
    rdt, _ = load_rdt(CD, "ROOM10F0")
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    auswahl = json.load(open(raum.AUSWAHL, encoding="utf-8"))["ROOM10F0"]
    alt = raum._container_lesen(os.path.join(MASKS, "ROOM10F0.MSK"))
    secs = {}
    info = {}
    for cut in (4, 5):
        e = raum.eintrag(auswahl[str(cut)])
        bg = load_bg(PPM, 0x10F0, cut)
        objekte = regionen("ROOM10F0", cut, e, **schalter)
        res = anwenden.bau_objektweise(rdt, cam, cut, objekte, bg, zielverz, "ROOM10F0")
        secs[cut] = res[0]
        flaeche = np.zeros((240, 320), bool)
        for x in objekte:
            flaeche |= x[1]
        info[cut] = {"rects": res[1], "soll": int(flaeche.sum()), "flaeche": flaeche}
    alt.update(secs)
    open(os.path.join(zielverz, "ROOM10F0.MSK"), "wb").write(geom.pack_container(alt, rdt[1]))
    return secs, info


def deckung_tiefe(verz, cut):
    """Deckung (bool) und Tiefe (int) je Bildpunkt aus einem Maskenbaum."""
    blob = open(os.path.join(verz, "ROOM10F0.MSK"), "rb").read()
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim(os.path.join(verz, "ROOM10F0_PRI%02d.TIM" % cut))[0]
    deck = np.zeros((240, 320), bool)
    tief = np.full((240, 320), 1 << 30, np.int64)
    for (sx, sy, X, Y, w, h, dep) in ms:
        x0, x1 = max(0, X), min(320, X + w)
        y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        if sub.shape != (y1 - y0, x1 - x0):
            continue
        m = sub != 0
        deck[y0:y1, x0:x1] |= m
        blk = tief[y0:y1, x0:x1].copy()
        np.copyto(blk, np.minimum(blk, dep), where=m)
        tief[y0:y1, x0:x1] = blk
    return deck, tief, len(ms)


if __name__ == "__main__":
    befehl = sys.argv[1] if len(sys.argv) > 1 else "eich"
    if befehl == "eich":
        secs, info = bauen("build/r22/v0")
        ok = True
        for cut in (4, 5):
            a = open(os.path.join(MASKS, "ROOM10F0_PRI%02d.TIM" % cut), "rb").read()
            b = open("build/r22/v0/ROOM10F0_PRI%02d.TIM" % cut, "rb").read()
            alt = raum._container_lesen(os.path.join(MASKS, "ROOM10F0.MSK"))[cut]
            gl = (a == b) and (alt == secs[cut])
            ok &= gl
            print("EICH cut %d: Rechtecke %d, Soll %d px, Sektion+TIM bitgleich=%s"
                  % (cut, info[cut]["rects"], info[cut]["soll"], gl))
        print("EICHUNG %s" % ("BESTANDEN" if ok else "⛔ FEHLGESCHLAGEN"))
        sys.exit(0 if ok else 1)
