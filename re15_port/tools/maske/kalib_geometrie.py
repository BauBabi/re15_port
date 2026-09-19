"""Kalibrierung der Tiefenregel (geometrie.py) an den KUENSTLER-Masken von STAGE1.

Phase 2, Punkt 6d: BEVOR ein Nutzer-Cut gebaut wird, muss die Regel an den 114
Original-Cuts (3583 Kuenstler-Rechtecke) gemessen sein. Silhouette = die opaken Texel des
SLD-Atlas an ihrer Zielstelle (original.atlas + artist_rects), Objekte = ihre
Zusammenhangskomponenten, Bodenebene = das begehbare Band des Raums (bzw. das Blickziel,
wenn eindeutig). Je Kuenstler-Rechteck: Modelltiefe minus eingetragene Tiefe, und zwar
fuer DREI Hypothesen, was Capcom je Rechteck eintraegt (MAX = fernster Punkt, MEDIAN,
MIN = naechster Punkt), und fuer mehrere Tiefenfaktoren (0,90..1,10) — Faktor und
Statistik werden hier GEMESSEN, nicht gesetzt (Gegenpruefung Punkte 6 und 7).

Ausgegeben werden ausserdem die Streuung der Modelltiefe INNERHALB eines Kuenstler-
Rechtecks (= die Tiefenstufe, mit der die Kuenstler selbst arbeiten) und die Rechteckzahl,
die die Zerlegung bei verschiedenen Stufen braucht.

Aufruf: python re15_port/tools/maske/kalib_geometrie.py [--out datei] [--ohne-zelle]
"""
import argparse, glob, os, struct, sys, time
import numpy as np
from scipy import ndimage

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geom, geometrie, original as ORIG
from geom import load_rdt, cut_view

CD = "re15_port/shared_assets/PSX"
FAKTOREN = (0.90, 0.95, 1.00, 1.05, 1.10)
ATLAS_DUMP = "build/p2/atlas"     # probe_p2_atlas_dump: ROOM####_ORIG##.TIM ueber den Engine-Dekoder


def atlas_des_raums(room, n_cut):
    """Original-Atlanten je Cut: ZUERST der Engine-Dump (byte-true, alle 359 Cuts), sonst
    der Python-Nachbau in original.py (scheitert an ROOM1020/1030/1040/1070)."""
    import maskenbild as MB
    out = {}
    for cut in range(n_cut):
        p = os.path.join(ATLAS_DUMP, "%s_ORIG%02d.TIM" % (room, cut))
        if os.path.exists(p):
            t = MB.lies_tim(p)
            if t:
                out[cut] = t
    if out:
        return out
    return ORIG.atlas(room)


def silhouette_und_felder(rdt, cam, cut, at):
    """-> (Silhouette, Felder, Gruppen). Felder = (x0, y0, x1, y1, tiefe, opak, gruppe);
    Gruppen = {gruppenindex: Silhouette der Gruppe}. Die Kuenstler-GRUPPE (gc je Sektion,
    eigener Anker) ist die Objektaufteilung der Kuenstler: STAGE1 median 3 Gruppen je Cut,
    5 Rechtecke und 4 verschiedene Tiefen je Gruppe (gemessen 2026-09-19; vgl. RE2 Retail,
    Memory reai-v2-re2-pri-vorbild: ein Gegenstand = eine Gruppe)."""
    ms = ORIG.artist_rects(rdt, cam, cut)
    if not ms or cut not in at:
        return None, None, None
    idx = at[cut][0]
    reg = np.zeros((240, 320), bool)
    felder = []
    gruppen = {}
    for (sx, sy, X, Y, w, h, dep, gi) in ms:
        x0, x1 = max(0, X), min(320, X + w); y0, y1 = max(0, Y), min(240, Y + h)
        if x1 <= x0 or y1 <= y0:
            continue
        sub = idx[sy + (y0 - Y):sy + (y1 - Y), sx + (x0 - X):sx + (x1 - X)]
        if sub.shape != (y1 - y0, x1 - x0):
            continue
        op = np.zeros((240, 320), bool)
        op[y0:y1, x0:x1] = sub != 0
        reg |= op
        gruppen.setdefault(gi, np.zeros((240, 320), bool))
        gruppen[gi] |= op
        felder.append((x0, y0, x1, y1, dep, op, gi))
    return reg, felder, gruppen


def ebene_des_cuts(rdt, cam, cut, rid):
    baender = sorted(geom.begehbare_baender(rid))
    if len(baender) == 1:
        return -baender[0] * geom.BAND_HOEHE, baender[0], "band"
    r = geom.ebene_aus_kamera(rdt, cam, cut, rid)
    if r is None or r[1] >= geom.GUETE_MAX:
        return None, None, "uneindeutig"
    return r[0], int(round(-r[0] / float(geom.BAND_HOEHE))), "kamera %.2f" % r[1]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--stage", type=int, default=1)
    ap.add_argument("--out", default="build/p2/kalib_geometrie.txt")
    ap.add_argument("--ohne-zelle", action="store_true", help="Regel (i) abschalten (Gegenprobe)")
    ap.add_argument("--stufen", default="1,2,3,4,6,8,12,16")
    ap.add_argument("--partition", choices=("gruppe", "komponente"), default="gruppe",
                    help="Objekt = Kuenstler-Gruppe (Standard) oder Zusammenhangskomponente")
    a = ap.parse_args()
    stufen = [int(v) for v in a.stufen.split(",")]
    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    out = open(a.out, "w", encoding="utf-8")

    def W(s=""):
        out.write(s + "\n"); out.flush(); print(s, flush=True)

    rooms = sorted(os.path.basename(p)[:-4] for p in glob.glob(os.path.join(CD, "STAGE%d" % a.stage, "ROOM*.RDT")))
    fehler = {(f, h): [] for f in FAKTOREN for h in ("max", "med", "min", "objmin")}
    nach_quelle = {}      # (quelle, hypothese) -> [fehler bei Faktor 1,00]
    streu = []
    quellen = {}
    n_cuts = 0; n_felder = 0; n_komp = 0
    bedarf = {s: [] for s in stufen}
    ueberlauf = {s: 0 for s in stufen}
    uebersprungen = []
    t_start = time.time()
    W("# Kalibrierung geometrie.py an den Kuenstler-Masken STAGE%d (%s, Objekt = %s)" % (a.stage, "OHNE Regel (i)" if a.ohne_zelle else "mit Regel (i)", a.partition))
    W("%-9s %-3s %6s %5s %6s %-8s | %7s %7s %7s | %s" % ("Raum", "Cut", "ebene", "Komp", "Felder", "Quelle", "medMax", "medMed", "medMin", "Rechtecke je Stufe " + "/".join(str(s) for s in stufen) + " (X = Ueberlauf)"))
    for room in rooms:
        rid = int(room[4:], 16)
        if rid & 1:
            continue
        rdt, _ = load_rdt(CD, room)
        if not rdt or len(rdt) < 64:          # ROOM1270/1271.RDT sind 4-Byte-Stubs
            continue
        cam = struct.unpack_from("<I", rdt, 0x24)[0]
        try:
            at = atlas_des_raums(room, rdt[1])
        except Exception as e:
            W("  %s: Atlas nicht lesbar (%s)" % (room, e)); continue
        for cut in range(rdt[1]):
            reg, felder, gruppen = silhouette_und_felder(rdt, cam, cut, at)
            if reg is None or not reg.any():
                continue
            v = cut_view(rdt, cam, cut)
            if not v or v[2] <= 0:
                continue
            R, t, H = v
            y0, band, wie = ebene_des_cuts(rdt, cam, cut, rid)
            if y0 is None:
                uebersprungen.append("%s C%d (%s)" % (room, cut, wie)); continue
            n_cuts += 1
            if a.partition == "gruppe":
                teile = [gruppen[g] for g in sorted(gruppen)]
            else:
                lab, n = ndimage.label(reg, structure=np.ones((3, 3)))
                teile = [lab == k for k in range(1, n + 1)]
            n = len(teile)
            vz_all = np.zeros((240, 320), np.float64)
            quelle_px = np.full((240, 320), "", dtype=object)
            objmin_px = np.zeros((240, 320), np.float64)     # je Punkt: kleinste vz SEINES Objekts
            objekte = []
            zellen = geom.sca_sperrzellen(rdt, band) or []
            q_cut = []
            for k, comp in enumerate(teile, 1):
                if comp.sum() < 4:
                    continue
                n_komp += 1
                vzm, info = geometrie.tiefe_geometrie(rdt, R, t, H, comp, y0, band, None, zellen,
                                                      zelle_erlaubt=not a.ohne_zelle)
                q = info.get("quelle") or "keine"
                quellen[q] = quellen.get(q, 0) + 1
                q_cut.append(q[0])
                if vzm is None:
                    continue
                vz_all = np.where(comp, vzm, vz_all)
                quelle_px[comp] = q
                objmin_px[comp] = float(vzm[comp & (vzm > 0)].min()) if (comp & (vzm > 0)).any() else 0.0
                objekte.append(("K%d" % k, comp, geometrie.rastern(vzm) * comp))
            dep_f = {f: geometrie.rastern(vz_all * f) * (vz_all > 0) for f in FAKTOREN}
            objmin_f = {f: geometrie.rastern(objmin_px * f) * (objmin_px > 0) for f in FAKTOREN}
            med = {"max": [], "med": [], "min": []}
            for (x0, y0_, x1, y1, dep, op, gi) in felder:
                if dep == 0:
                    continue
                w1 = dep_f[1.00][op]
                w1 = w1[w1 > 0]
                if len(w1) == 0:
                    continue
                n_felder += 1
                streu.append(int(w1.max()) - int(w1.min()))
                qs = [s for s in quelle_px[op] if s]
                q_feld = max(set(qs), key=qs.count) if qs else "keine"
                for f in FAKTOREN:
                    w = dep_f[f][op]; w = w[w > 0]
                    om = objmin_f[f][op]; om = om[om > 0]
                    fehler[(f, "max")].append(int(w.max()) - dep)
                    fehler[(f, "med")].append(int(np.median(w)) - dep)
                    fehler[(f, "min")].append(int(w.min()) - dep)
                    fehler[(f, "objmin")].append((int(om.min()) if len(om) else int(w.min())) - dep)
                    if f == 1.00:
                        med["max"].append(int(w.max()) - dep); med["med"].append(int(np.median(w)) - dep); med["min"].append(int(w.min()) - dep)
                        for h, e_ in (("max", int(w.max()) - dep), ("med", int(np.median(w)) - dep), ("min", int(w.min()) - dep),
                                      ("objmin", (int(om.min()) if len(om) else int(w.min())) - dep)):
                            nach_quelle.setdefault((q_feld, h), []).append(e_)
            zahlen = []
            for s in stufen:
                try:
                    rects, wahl = geometrie.zerlegung(objekte, s)
                    bedarf[s].append(len(rects)); zahlen.append(str(len(rects)))
                except RuntimeError:
                    ueberlauf[s] += 1; zahlen.append("X")
            W("%-9s %-3d %6d %5d %6d %-8s | %7s %7s %7s | %s" % (
                room, cut, y0, n, len(felder), "".join(q_cut)[:8],
                ("%+d" % np.median(med["max"])) if med["max"] else "-",
                ("%+d" % np.median(med["med"])) if med["med"] else "-",
                ("%+d" % np.median(med["min"])) if med["min"] else "-", "/".join(zahlen)))
    W()
    W("# Zusammenfassung: %d Cuts, %d Komponenten, %d Kuenstler-Rechtecke, %.0f s" % (n_cuts, n_komp, n_felder, time.time() - t_start))
    W("# Tiefenquelle je Komponente: %s" % ", ".join("%s=%d" % kv for kv in sorted(quellen.items())))
    if uebersprungen:
        W("# uebersprungen (Bodenebene uneindeutig): %d: %s" % (len(uebersprungen), "; ".join(uebersprungen)))
    W("# Fehler Modell minus Kuenstler je Hypothese und Faktor (Einheit: OT-Bucket = 64,0625 vz):")
    W("# %-6s %-6s %7s %7s %7s %7s %8s %8s %8s %8s" % ("Faktor", "Stat", "Median", "Mittel", "|e|<=2", "|e|<8", "e<-8", "e>8", "e<0", "e>0"))
    for f in FAKTOREN:
        for h in ("max", "med", "min", "objmin"):
            e = np.array(fehler[(f, h)])
            if not len(e):
                continue
            W("# %-6.2f %-6s %+7.1f %+7.1f %6.1f%% %6.1f%% %7.1f%% %7.1f%% %7.1f%% %7.1f%%"
              % (f, h.upper(), np.median(e), e.mean(), 100 * (np.abs(e) <= 2).mean(), 100 * (np.abs(e) < 8).mean(),
                 100 * (e < -8).mean(), 100 * (e > 8).mean(), 100 * (e < 0).mean(), 100 * (e > 0).mean()))
    W("# Fehler bei Faktor 1,00 NACH TIEFENQUELLE des Rechtecks (zelle = Regel (i), profil = Regel (ii)+(iii)):")
    W("# %-8s %-6s %6s %7s %7s %7s %7s %7s %7s" % ("Quelle", "Stat", "n", "Median", "Mittel", "|e|<8", "e<-8", "e>8", "|e|<=2"))
    for q in sorted(set(k[0] for k in nach_quelle)):
        for h in ("max", "med", "min", "objmin"):
            e = np.array(nach_quelle.get((q, h), []))
            if not len(e):
                continue
            W("# %-8s %-6s %6d %+7.1f %+7.1f %6.1f%% %6.1f%% %6.1f%% %6.1f%%"
              % (q, h.upper(), len(e), np.median(e), e.mean(), 100 * (np.abs(e) < 8).mean(),
                 100 * (e < -8).mean(), 100 * (e > 8).mean(), 100 * (np.abs(e) <= 2).mean()))
    s = np.array(streu)
    if len(s):
        W("# Streuung der Modelltiefe (Faktor 1,00) INNERHALB eines Kuenstler-Rechtecks (max-min): Median %.0f  p75 %.0f  p90 %.0f  p95 %.0f  Anteil <=1: %.1f %%  <=2: %.1f %%  <=4: %.1f %%  <=8: %.1f %%"
          % (np.median(s), np.percentile(s, 75), np.percentile(s, 90), np.percentile(s, 95),
             100 * (s <= 1).mean(), 100 * (s <= 2).mean(), 100 * (s <= 4).mean(), 100 * (s <= 8).mean()))
    for st in stufen:
        b = np.array(bedarf[st])
        W("# Stufe %2d: Zerlegung haelt in %d von %d Cuts; Rechtecke Median %s p90 %s max %s; Ueberlauf %d"
          % (st, len(b), n_cuts, ("%.0f" % np.median(b)) if len(b) else "-", ("%.0f" % np.percentile(b, 90)) if len(b) else "-",
             ("%d" % b.max()) if len(b) else "-", ueberlauf[st]))
    out.close()


if __name__ == "__main__":
    main()
