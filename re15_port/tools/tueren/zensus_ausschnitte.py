#!/usr/bin/env python3
"""zensus_ausschnitte.py - Ausschnitte + entzerrte Blaetter je Tuerseite (R31 T1, Schritt 2).

Fuer RE1.5 (alle begehbaren Tuerseiten aus zensus_paare) und RE2 (alle Door_aot_set der
Leon-Raeume ROOM1xx0..ROOM7xx0 mit Hintergrund). Verfahren: zensus_bild.py (Dossier §4.1).

Schreibt build/r31_tueren/t1/{re15,re2}_seiten/<kennung>_cNN_{aus,entz,voll}.png und gibt die
Messwerte als dict zurueck (zensus_alles.py sammelt sie in zensus.json).
"""
import os
import sys

import numpy as np

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
import zensus_lib as L     # noqa: E402
import zensus_bild as B    # noqa: E402

MAX_CUTS = 3


def cuts_pruefen(raum, kante, band, mitte, bereiche, gr, groesse):
    """Alle Cuts fuer eine Kandidatenkante -> (welt, cuts, ok sortiert)."""
    welt = B.blatt_welt(kante, band)
    cuts = []
    for c in range(raum.ncut):
        if not raum.hat_bild(c):
            cuts.append(dict(cut=c, ok=False, grund="kein Hintergrund", aktiv=False))
            continue
        sv = B.sichtbarkeit(raum.kamera(c), welt, kante)
        sv["cut"] = c
        q = bereiche.get(c)
        sv["aktiv"] = bool(q is not None and L.im_viereck(q, mitte[0], mitte[1]))
        cuts.append(sv)
    # Bereich der Tuer: Gruppen (Umschaltgraph) der Cuts, deren Bildbereich die Mitte enthaelt;
    # ohne solchen Cut die Gruppe des naechstgelegenen Bildbereichs.
    aktiv = [c["cut"] for c in cuts if c.get("aktiv")]
    if not aktiv and bereiche:
        naechst = min(bereiche, key=lambda k: L.abstand_viereck(bereiche[k], mitte[0], mitte[1]))
        aktiv = [naechst]
    eigene = {gr.get(x) for x in aktiv}
    for c in cuts:
        c["bereich"] = gr.get(c["cut"]) in eigene
        # Cut ohne jede Umschaltzone: nur per Skript erreichbar (Zwischensequenz o.ae.)
        c["einzeln"] = groesse.get(gr.get(c["cut"]), 0) <= 1
    ok = [c for c in cuts if c.get("ok")]
    ok.sort(key=lambda c: (not c["bereich"], c["einzeln"], not c["aktiv"], -c["px_h"] * c["im_bild"]))
    if any(c["bereich"] for c in ok):
        ok = [c for c in ok if c["bereich"]]
    return welt, cuts, ok


def kante_waehlen(raum, pts, band, ankunft, mitte=None):
    """Kantenwahl (Dossier §4.1): Kandidaten = lange Kanten. Nur Kandidaten, deren Blatt in
    mindestens einem Cut des Tuerbereichs von VORN zu sehen ist, bleiben (die Kameras stehen
    auf der Raumseite der Tuerwand). Unter mehreren: liegt die Ankunft der Gegenseite
    ausserhalb des Rechtecks, die von ihr abgewandte Kante; sonst die Kante mit dem
    staerksten Bildbeleg (Kantenwert / Median nach der Verfeinerung).
    -> (kante, welt, cuts, ok)"""
    bereiche = raum.bereiche()
    if mitte is None:
        mitte = np.mean(np.array(pts, float), axis=0)
    gr = raum.gruppen()
    groesse = {}
    for c in range(raum.ncut):
        groesse[gr.get(c)] = groesse.get(gr.get(c), 0) + 1
    kandidaten = B.kandidatenkanten(pts)
    auswertung = []
    for k in kandidaten:
        welt, cuts, ok = cuts_pruefen(raum, k, band, mitte, bereiche, gr, groesse)
        auswertung.append(dict(kante=k, welt=welt, cuts=cuts, ok=ok))
    mit = [e for e in auswertung if e["ok"]]
    ank_aussen = [p for p in (ankunft or []) if not L.im_viereck(pts, p[0], p[1])]
    if len(mit) == 1:
        wahl, grund = mit[0], "einzige Kante mit Frontansicht"
    elif len(mit) > 1 and ank_aussen:
        wahl = max(mit, key=lambda e: min(float(e["kante"]["n"].dot((e["kante"]["a"] + e["kante"]["b"]) / 2 - np.array(p, float)))
                                          for p in ank_aussen))
        grund = "Frontansicht bei %d Kanten; von der Ankunft abgewandt" % len(mit)
    elif len(mit) > 1 and any(c["aktiv"] for e in mit for c in e["ok"]):
        # Die Cuts, die beim Stehen im Rechteck aktiv sind, zeigen die Tuer: die Kante, deren
        # Blatt dort im Mittel am frontalsten erscheint (Breite/Hoehe im Bild gegen 1950/3549,
        # hoechstens 1), dann die groesste Flaeche.
        def fr(e):
            v = [min(1.0, c["px_w"] / max(1.0, c["px_h"]) / (L.BLATT_B / L.BLATT_H)) for c in e["ok"] if c["aktiv"]]
            return (round(float(np.mean(v)), 2) if v else 0.0,
                    max([c["px_h"] * c["px_w"] * c["im_bild"] for c in e["ok"] if c["aktiv"]] or [0.0]))
        wahl = max(mit, key=fr)
        grund = "Frontansicht bei %d Kanten, Ankunft im Rechteck; frontalstes Blatt in den aktiven Cuts (%.2f, %.0f px2)" % (
            (len(mit),) + fr(wahl))
    elif len(mit) > 1:
        bestw = None
        for e in mit:
            c = e["ok"][0]
            cam = raum.kamera(c["cut"])
            v = B.verfeinern(cam, B.helligkeit(raum.hintergrund(c["cut"])), e["kante"], band)
            kontrast = v["wert"] / max(1e-6, v["median"])
            e["kontrast"] = kontrast
            if bestw is None or kontrast > bestw:
                bestw, wahl = kontrast, e
        grund = "Frontansicht bei %d Kanten, Ankunft im Rechteck; Bildbeleg %.2f" % (len(mit), bestw)
    else:
        k = B.tuerkante(pts, ankunft, bereiche)
        wahl = [e for e in auswertung if np.allclose(e["kante"]["a"], k["a"]) and np.allclose(e["kante"]["b"], k["b"])][0]
        grund = "keine Frontansicht; " + k["grund"]
    kante, welt, cuts, ok = wahl["kante"], wahl["welt"], wahl["cuts"], wahl["ok"]
    kante["grund"] = grund
    return kante, welt, cuts, ok


def schneide(spiel, raum, pts, band, ankunft, kennung, ausdir, mitte=None, max_cuts=MAX_CUTS):
    """Eine Tuerseite: Tuerkante waehlen, alle Cuts pruefen, die besten max_cuts ausschneiden.
    raum = L.Raum. -> dict(kante, cuts=[...], gewaehlt=[...])."""
    from PIL import Image
    kante, welt, cuts, ok = kante_waehlen(raum, pts, band, ankunft, mitte)
    gewaehlt = []
    os.makedirs(ausdir, exist_ok=True)
    for c in ok[:max_cuts]:
        cam = raum.kamera(c["cut"])
        rgb = raum.hintergrund(c["cut"])
        lum = B.helligkeit(rgb)
        v = B.verfeinern(cam, lum, kante, band)
        quad = v["quad"]
        im, box = B.ausschnitt(rgb, quad, f=3)
        if im is None:
            continue
        B.umriss(im, v["quad_start"], box, 3, (255, 220, 0))
        B.umriss(im, quad, box, 3, (255, 40, 40))
        stamm = os.path.join(ausdir, "%s_c%02d" % (kennung, c["cut"]))
        im.save(stamm + "_aus.png")
        ent, anteil = B.entzerren(rgb, quad)
        Image.fromarray(ent).save(stamm + "_entz.png")
        # Vollbild 1-fach mit Umriss (fuer den Kontaktbogen)
        voll = Image.fromarray(rgb.copy())
        B.umriss(voll, quad, (0, 0, 0, 0), 1, (255, 40, 40))
        voll.save(stamm + "_voll.png")
        gewaehlt.append(dict(cut=c["cut"], aktiv=c["aktiv"], bereich=c["bereich"], einzeln=c["einzeln"],
                             px_h=round(c["px_h"], 1),
                             im_bild=round(c["im_bild"], 3), dn=v["dn"], dt=v["dt"], W=v["W"],
                             kantenwert=round(v["wert"], 2), kantenwert_start=round(v["wert_start"], 2),
                             median=round(v["median"], 2), verschiebung_px=round(v["verschiebung_px"], 1),
                             quad=[[round(a, 2) for a in p] for p in quad],
                             quad_daten=[[round(a, 2) for a in p] for p in v["quad_start"]],
                             entz_im_bild=round(anteil, 3),
                             ausschnitt=L.rel(stamm + "_aus.png"), entzerrt=L.rel(stamm + "_entz.png"),
                             vollbild=L.rel(stamm + "_voll.png"), hintergrund=L.rel(raum.bg % c["cut"])))
    return dict(kante=dict(a=kante["a"].tolist(), b=kante["b"].tolist(), n=kante["n"].tolist(),
                           grund=kante["grund"]),
                blatt_welt=welt.round(1).tolist(),
                cuts=[{k: (round(v, 3) if isinstance(v, float) else v) for k, v in c.items() if k != "quad"}
                      for c in cuts],
                gewaehlt=gewaehlt, sichtbar=bool(gewaehlt))


# ----------------------------------------------------------------------------
# RE1.5
# ----------------------------------------------------------------------------
def re15_alle(Zs, nur=None):
    seiten = Zs["seiten"]
    by = {s["id"]: s for s in seiten}
    ausdir = os.path.join(L.AUS, "re15_seiten")
    raeume = {}
    aus = {}
    for s in seiten:
        if s["nullflaeche"]:
            continue
        if nur and s["basis"] not in nur:
            continue
        var = "0" if "0" in s["varianten"] else s["varianten"][0]
        nm = s["basis"] + var
        if nm not in raeume:
            raeume[nm] = L.Raum("re15", nm)
        ankunft = []
        g = s.get("gegenseite")
        if g:
            ankunft = [(z[0][0], z[0][2]) for z in by[g]["ziele"]]
        kennung = "%s_ROOM%s" % (s["id"], nm)
        r = schneide("re15", raeume[nm], s["pts"], s["band"], ankunft, kennung, ausdir)
        r["raumdatei"] = nm
        aus[s["id"]] = r
        print("%s ROOM%s b%d -> %s  Cuts ok %d/%d  gewaehlt %s  (%s)" % (
            s["id"], nm, s["band"], s["ziel_basis"], sum(1 for c in r["cuts"] if c.get("ok")), len(r["cuts"]),
            [g["cut"] for g in r["gewaehlt"]], r["kante"]["grund"]))
    return aus


# ----------------------------------------------------------------------------
# RE2: alle Tuersaetze der Leon-Raeume mit Hintergrund; Seite = (Raum, Band, Flaeche, Archiv)
# ----------------------------------------------------------------------------
def re2_seiten():
    import glob
    alle = []
    for p in sorted(glob.glob(os.path.join(L.REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT"))):
        nm = os.path.basename(p)[4:8].upper()
        if nm[0] not in "1234567" or nm[3] != "0":
            continue
        d = open(p, "rb").read()
        try:
            ts = L.re2_tueren(d)
        except Exception:
            continue
        for t in ts:
            t["raum"] = nm
            alle.append(t)
    seiten = {}
    for t in alle:
        k = (t["raum"], t["band"], tuple(t["pts"]), t["archiv"])
        if k not in seiten:
            seiten[k] = dict(raum=t["raum"], band=t["band"], pts=[list(p) for p in t["pts"]], archiv=t["archiv"],
                             varianten=set(), ziel_basis=L.raumname(t["ziel_stage"], t["ziel_raum"]),
                             saetze=[], form=t["form"])
        seiten[k]["varianten"].add(t["variante"])
        seiten[k]["saetze"].append("0x%05X" % t["pc"])
    aus = []
    for i, (k, v) in enumerate(sorted(seiten.items(), key=lambda kv: (kv[0][0], kv[0][3], kv[0][2]))):
        v["id"] = "R%03d" % i
        v["varianten"] = sorted(v["varianten"])
        xs = [p[0] for p in v["pts"]]
        zs = [p[1] for p in v["pts"]]
        v["nullflaeche"] = (max(xs) - min(xs) == 0 and max(zs) - min(zs) == 0)
        # Ankunft der Rueckrichtung: Saetze anderer Raeume mit Ziel = dieser Raum, Ankunft <= 2500
        ank = []
        for t in alle:
            if t["raum"][:3] == v["raum"][:3]:
                continue
            if L.raumname(t["ziel_stage"], t["ziel_raum"]) != v["raum"][:3]:
                continue
            if L.abstand_viereck(v["pts"], t["ziel"][0], t["ziel"][2]) <= 2500:
                ank.append((t["ziel"][0], t["ziel"][2]))
        v["ankunft"] = sorted(set(ank))
        aus.append(v)
    return aus


def re2_alle(nur_archive=None):
    seiten = re2_seiten()
    ausdir = os.path.join(L.AUS, "re2_seiten")
    raeume = {}
    erg = []
    for v in seiten:
        if v["nullflaeche"]:
            continue
        if nur_archive is not None and v["archiv"] not in nur_archive:
            continue
        nm = v["raum"]
        if nm not in raeume:
            raeume[nm] = L.Raum("re2", nm)
        r = raeume[nm]
        if not r.hat_bild(0):
            v["sichtbar"] = False
            v["grund"] = "kein Hintergrund"
            erg.append(v)
            continue
        kennung = "%s_ROOM%s_D%02X" % (v["id"], nm, v["archiv"])
        res = schneide("re2", r, v["pts"], v["band"], v["ankunft"], kennung, ausdir)
        v.update(res)
        erg.append(v)
        print("%s ROOM%s D%02X b%d  ok %d/%d gewaehlt %s" % (v["id"], nm, v["archiv"], v["band"],
              sum(1 for c in res["cuts"] if c.get("ok")), len(res["cuts"]), [g["cut"] for g in res["gewaehlt"]]))
    return erg


if __name__ == "__main__":
    import zensus_paare as P
    if len(sys.argv) > 1 and sys.argv[1] == "re2":
        re2_alle()
    else:
        re15_alle(P.bauen(), nur=set(sys.argv[1:]) or None)
