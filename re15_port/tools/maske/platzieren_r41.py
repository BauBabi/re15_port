"""Runde 41 Spur M, Nachbesserung 4 (Abnahme 3, Mangel 2): UNPLATZIERTE Kuenstler-Komponenten der Stages 3..6 platzieren.

Dossier: analysis/befunde_runde41/M_masken36.md, Abschnitt "Nachbesserung 4".

BEFUND: Der Generator (analysis/esp_masken_2026-09-03/gen_masken.py build_cut) saet nur aus VOLLSTAENDIG deckenden 16x16-Bloecken
des Kuenstler-Atlas (gen_region.find_blocks: `if cnt != n: continue`, NCC >= 0.97) und waechst von dort (region_grow) nur innerhalb
schon belegter Verschiebungen. Duenne Teile - Gelaender, Rohre, Kabel, Lampen - haben nie einen solchen Block; liegen sie im
Atlas als EIGENE Komponente (8-zusammenhaengend, Index != 0), wurden sie nie platziert und fehlen im Spiel ganz. Gemessen an den 216
Generator-Sektionen: 800 unplatzierte Komponenten (>= 16 Texel, 410 127 Texel), z.B. ROOM30E0 Cut 7: das Gelaender des Stegs (alle
Pfosten und der Handlauf) - die Restliste der Abnahme ("Gelaender nicht in der Generator-Region").

REGEL (je Cut, je unplatzierter Komponente K des Original-Atlas = SLD des BSS-Chunks, Entpacker wie FUN_800c47e8 / original.atlas):
  1. Lage = Verschiebung s mit der kleinsten mittleren quadratischen Farbabweichung (maskierte SSD ueber ALLE ganzzahligen s per
     FFT, nur Texel von K, nur Texel im Bild; mindestens 90 % von K im Bild). Grund: der Atlasinhalt ist der Hintergrund an der
     Zielstelle (gen_region Kopf, Q1) - an der richtigen Lage bleibt nur die Paletten-Rundung.
  2. Annahme nur, wenn (a) RMS <= RMS_MAX und (b) die beste NICHT benachbarte Lage (|ds| > 2) mindestens EINDEUTIG-mal schlechter ist.
     Kalibriert an den 422 vom Generator platzierten (verifizierten) Teilen >= 16 Texel derselben Sektionen (Dossier NB4): die SSD
     findet 410 an der Generator-Lage; RMS dort Median 9,66, 99 % 18,91, Maximum 20,24 -> RMS_MAX = 18,91 (99 %-Punkt); Verhaeltnis
     zweitbeste/beste bei den 12 abweichenden Faellen hoechstens 1,47 -> EINDEUTIG = 1,5 (darueber lag keiner).
  3. Texel von K, deren Farbe an der Lage um mehr als 48 abweicht (|dR|+|dG|+|dB|, Grenze der R37-Blit-Pruefung), bleiben weg
     (wie das Wachstum des Generators je Texel prueft).
  4. Bildpunkte, die schon ein vorhandenes Rechteck des Cuts deckend zeichnet (gezeichnet ODER ausgeschlossen), bleiben weg -
     die neuen Rechtecke kommen nur dazu (alter Teil jeder Sektion bytegleich, Probe unten).
  5. Tiefe = Regel des Generators je Bildspalte (gen_masken.depth_map_for_cut = schliessen_r41.tiefenkarte: Bodenkontakt der
     Komponente in dieser Spalte auf y = 0, DEPTH_SAFETY 0,90), aber JE KOMPONENTE (eigene Bildpunkte), damit sich zwei Gegenstaende
     in einer Spalte nicht einen Kontakt teilen. Punkte ohne Bodenkontakt (Strahl ueber dem Horizont) bekommen keine Tiefe und bleiben
     weg. Rechtecke wie schliessen_r41 (Grundstuecke gleicher Spaltentiefe, gierig zusammengelegt, Tiefe = Median), Platz hoechstens
     105 Rechtecke je Cut (re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT); reicht er nicht, kommen die groessten Komponenten zuerst.
  6. Atlas: das eigene Blatt des Cuts (_PRIcc.TIM) behaelt alle vorhandenen Fenster; neue Texel kommen an die erste freie Stelle,
     Farbe = Hintergrund an der Zielstelle, Index = naechste Farbe der VORHANDENEN Palette (CLUT bleibt bytegleich -> vorhandene Texel
     unveraendert). Index 0 = durchsichtig (Gelaender-Luecken bleiben offen).
  7. Danach rechnet test_r37_masken36 "erzeuge" den Ausschluss der Spur P (R37) neu: ein neues Rechteck, das an einem erreichbaren
     Stehplatz einen Figurpunkt ohne Gegenstand davor verdecken wuerde, faellt weg.
Leon- und Elza-Datei bleiben bytegleich.

Aufruf:
  python re15_port/tools/maske/platzieren_r41.py --ppm <probe_bg_dump-Ordner> --cache <Regionen-Cache> --out <Ordner> [ROOMxxxx ...]
"""
import argparse
import json
import os
import struct
import sys

import numpy as np
from scipy import ndimage
from scipy.signal import fftconvolve

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)

import geom                      # noqa: E402
import maskenbild                # noqa: E402
import original                  # noqa: E402
import schliessen_r41 as SR      # noqa: E402

PSX = SR.PSX
MASKS = SR.MASKS
MAX_MASKEN = 105                 # re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT. Fuer die 37 vollen Cuts braeuchte es 128: das Original hat KEINE
                                 # feste Grenze (FUN_80039270 @0x8003928c lbu v0,7(a0): Zahl = RDT-Kopfbyte 7, u8), die Port-Tabelle des
                                 # Ausschlusses (masken36_r37.c bits[4]) traegt 128 - Vorschlag, nicht eingebaut (Entscheid M-E1, Dossier NB4)
MIN_TEXEL = 16                   # kleinste Komponente (wie die Kalibrierung; kleiner ist die Lage nicht belegbar)
RMS_MAX = 18.91                  # 99 %-Punkt der RMS an der Generator-Lage (Kalibrierung, Kopf)
EINDEUTIG = 1.5                  # zweitbeste/beste > 1,47 (groesster Wert der abweichenden Kalibrierfaelle)
IM_BILD = 0.9                    # Anteil der Texel im Bild
FARBE_MAX = 48                   # R37 §2.1 Blit-Pruefung: |dR|+|dG|+|dB| > 48 zaehlt als Abweichung


def _corr(img, kern):
    return fftconvolve(img, kern[::-1, ::-1], mode="full")


def lage_suchen(maske, farbe, bild):
    """maske (h,w) bool, farbe (h,w,3) Atlasfarben, bild (240,320,3). -> [(dx, dy, mittel_d2, n_im_bild)] je beste Lage zuerst;
    dx/dy = Lage der linken oberen Ecke des Ausschnitts im Bild."""
    h, w = maske.shape
    m = maske.astype(np.float64)
    B = bild.astype(np.float64)
    O = np.ones(bild.shape[:2])
    n_on = _corr(O, m)
    t1 = _corr(O, m * (farbe.astype(np.float64) ** 2).sum(2))
    t2 = np.zeros_like(n_on)
    t3 = np.zeros_like(n_on)
    for ch in range(3):
        t2 += _corr(B[..., ch], m * farbe[..., ch])
        t3 += _corr(B[..., ch] ** 2, m)
    ssd = np.maximum(t1 - 2 * t2 + t3, 0.0)          # FFT-Rundung kann knapp negativ werden
    ok = n_on >= IM_BILD * m.sum() - 0.5
    md = np.where(ok, ssd / np.maximum(n_on, 1.0), np.inf)
    out = []
    for k in np.argsort(md, axis=None)[:60]:
        iy, ix = np.unravel_index(int(k), md.shape)
        if not np.isfinite(md[iy, ix]):
            break
        out.append((int(ix - (w - 1)), int(iy - (h - 1)), float(md[iy, ix]), int(round(n_on[iy, ix]))))
    return out


def platzieren(idx, clut, bild, grown):
    """-> Liste dict je angenommener Komponente: komp, n, shift (Atlas -> Bild), rms, rms2, maske (Atlas, verifizierte Texel)."""
    pal = SR.clut_rgb(clut).astype(np.int32)
    A = pal[idx]
    lab, n = ndimage.label(idx != 0, structure=np.ones((3, 3)))
    belegt = set()
    for m in grown.values():
        belegt |= set(int(c) for c in np.unique(lab[m]) if c)
    erg = []
    for c in range(1, n + 1):
        if c in belegt:
            continue
        ys, xs = np.nonzero(lab == c)
        if len(ys) < MIN_TEXEL:
            continue
        y0, y1, x0, x1 = ys.min(), ys.max() + 1, xs.min(), xs.max() + 1
        best = lage_suchen(lab[y0:y1, x0:x1] == c, A[y0:y1, x0:x1], bild)
        if not best:
            continue
        b0 = best[0]
        b1 = next((b for b in best[1:] if abs(b[0] - b0[0]) > 2 or abs(b[1] - b0[1]) > 2), None)
        rms = float(np.sqrt(b0[2]))
        rms2 = float(np.sqrt(b1[2])) if b1 else None
        e = dict(komp=int(c), n=int(len(ys)), box=(int(x0), int(y0), int(x1), int(y1)), shift=(int(b0[0] - x0), int(b0[1] - y0)), rms=round(rms, 2),
                 rms2=round(rms2, 2) if rms2 is not None else None)
        e["angenommen"] = bool(rms <= RMS_MAX and (rms2 is None or rms2 >= EINDEUTIG * max(rms, 1e-6)))
        if e["angenommen"]:
            dx, dy = e["shift"]
            X, Y = xs + dx, ys + dy
            im = (X >= 0) & (X < 320) & (Y >= 0) & (Y < 240)
            d = np.full(len(ys), 999)
            d[im] = np.abs(A[ys[im], xs[im]] - bild[Y[im], X[im]].astype(np.int32)).sum(1)
            ok = d <= FARBE_MAX
            mk = np.zeros(idx.shape, bool)
            mk[ys[ok], xs[ok]] = True
            e["maske"] = mk
            e["verifiziert"] = int(ok.sum())
        erg.append(e)
    return erg


def tiefe_je_komponente(rdt, cut, schirm):
    """Generator-Regel (schliessen_r41.tiefenkarte) fuer EINE Komponente: je Bildspalte Bodenkontakt auf y = 0, x 0,90."""
    return SR.tiefenkarte(rdt, cut, schirm)


def naechster_index(rgb, pal):
    """rgb (N,3) -> Index 1..255 der naechsten Palettenfarbe (Index 0 = durchsichtig bleibt frei)."""
    p = pal[1:].astype(np.int32)
    d = ((rgb[:, None, :].astype(np.int32) - p[None, :, :]) ** 2).sum(2)
    return (np.argmin(d, 1) + 1).astype(np.uint8)


def cut_bearbeiten(rdt, cut, sek, idx_eigen, clut_eigen, idx_orig, clut_orig, bild, grown, reserve=0):
    bericht = {"cut": cut}
    alt = []
    deck_alt = np.zeros((240, 320), bool)
    belegt = np.zeros(idx_eigen.shape, bool)
    for m in sek["masks"]:
        X, Y = SR.bildlage(sek, m)
        sx, sy, w, h, d0 = m[0], m[1], m[6], m[7], m[4]
        alt.append((sx, sy, X, Y, w, h, d0))
        belegt[sy:sy + h, sx:sx + w] = True
        op = idx_eigen[sy:sy + h, sx:sx + w] != 0
        ys, xs = np.nonzero(op)
        Xs, Ys = xs + X, ys + Y
        ok = (Xs >= 0) & (Xs < 320) & (Ys >= 0) & (Ys < 240)
        deck_alt[Ys[ok], Xs[ok]] = True
    budget = MAX_MASKEN - len(alt) - reserve
    komps = platzieren(idx_orig, clut_orig, bild, grown)
    bericht["komponenten"] = komps                      # Liste wird am Ende ohne Masken abgelegt (neu_px erst unten bekannt)
    ang = sorted([e for e in komps if e["angenommen"]], key=lambda e: -e["verifiziert"])
    bericht["budget"] = budget
    if budget <= 0 or not ang:
        bericht["neu"] = 0
        return None, None, bericht
    stuecke_alle = []
    dep_cut = np.zeros((240, 320), np.int32)              # Tiefe je Bildpunkt aus der Regel SEINER Komponente
    for e in ang:
        dx, dy = e["shift"]
        ys, xs = np.nonzero(e["maske"])
        X, Y = xs + dx, ys + dy
        ok = (X >= 0) & (X < 320) & (Y >= 0) & (Y < 240)
        schirm = np.zeros((240, 320), bool)
        schirm[Y[ok], X[ok]] = True
        schirm &= ~deck_alt
        if not schirm.any():
            e["neu_px"] = 0
            continue
        dep = tiefe_je_komponente(rdt, cut, schirm)
        if dep is None:
            continue
        schirm &= (dep > 0) & (dep_cut == 0)            # ohne Bodenkontakt keine Tiefe -> nicht zeichnen; je Punkt eine Komponente
        e["neu_px"] = int(schirm.sum())
        if not schirm.any():
            continue
        dep_cut[schirm] = dep[schirm]
        for s in SR.stuecke_bilden({(dx, dy): schirm}, dep):
            s["komp"] = e["komp"]
            stuecke_alle.append(s)
    if not stuecke_alle:
        bericht["neu"] = 0
        return None, None, bericht
    wahl = None
    for stufe in SR.STUFEN + (24, 32, 48, 64, 1024):
        neu = SR.zusammenlegen(stuecke_alle, stufe, budget)
        if len(neu) <= budget:
            wahl = (stufe, neu)
            break
    if wahl is None:
        stufe, neu = 1024, sorted(SR.zusammenlegen(stuecke_alle, 1024, budget), key=lambda s: -len(s["ys"]))[:budget]
    else:
        stufe, neu = wahl
    pal = SR.clut_rgb(clut_eigen).astype(np.int32)
    idx_neu = idx_eigen.copy()
    rgb_at = pal[idx_eigen].astype(np.uint8)          # alte Texel: ihre bisherige Farbe
    op_at = idx_eigen != 0
    plaetze, verloren = [], 0
    for s in sorted(neu, key=lambda s: -SR._flaeche(s["box"])):
        x0, y0, x1, y1 = s["box"]
        w, h = x1 - x0, y1 - y0
        pl = SR.frei_platz(belegt, w, h)
        if pl is None:
            verloren += len(s["ys"])
            continue
        ax, ay = pl
        belegt[ay:ay + h, ax:ax + w] = True
        ys, xs = s["ys"], s["xs"]
        rgb_at[ys - y0 + ay, xs - x0 + ax] = bild[ys, xs]   # neue Texel: Hintergrund an der Zielstelle
        op_at[ys - y0 + ay, xs - x0 + ax] = True
        plaetze.append((s, ax, ay))
    if not plaetze:
        bericht["neu"] = 0
        bericht["atlas_verloren"] = verloren
        return None, None, bericht
    # Palette fuer das ganze Blatt neu (255 Farben Median-Schnitt, Index 0 = durchsichtig; schliessen_r41.palette_und_index):
    # die vorhandene Palette kennt die Farben der neuen Teile nicht (gemessen ROOM30E0 C9: naechste Farbe im Mittel 51,6 daneben).
    idx_neu, pal_neu = SR.palette_und_index(rgb_at, op_at)
    pal_neu_full = np.zeros((256, 3), np.int32)
    pal_neu_full[1:1 + len(pal_neu)] = pal_neu
    alt_abw = np.abs(pal_neu_full[idx_neu[idx_eigen != 0]] - pal[idx_eigen[idx_eigen != 0]]).sum(1)
    groups = [(g[0], g[2], g[3]) for g in sek["groups"]]
    masks = list(alt)
    neue = []
    for (s, ax, ay) in sorted(plaetze, key=lambda t: (t[0]["box"][1], t[0]["box"][0])):
        x0, y0, x1, y1 = s["box"]
        d = int(np.median(dep_cut[s["ys"], s["xs"]]))        # wie schliessen_r41: Median der Spaltentiefen des Stuecks
        neue.append((ax, ay, x0, y0, x1 - x0, y1 - y0, d))
    for m in neue:
        groups.append((1, m[2] - m[0], m[3] - m[1]))
    sec = geom.pack_section(groups, masks + neue)
    # Farbe der neuen Texel gegen den Hintergrund (eigene Palette)
    abw = []
    for (s, ax, ay) in plaetze:
        x0, y0 = s["box"][0], s["box"][1]
        ii = idx_neu[s["ys"] - y0 + ay, s["xs"] - x0 + ax]
        abw.append(np.abs(pal_neu_full[ii] - bild[s["ys"], s["xs"]].astype(np.int32)).sum(1))
    abw = np.concatenate(abw) if abw else np.zeros(0)
    bericht.update(neu=len(neue), stufe=stufe, rechtecke=len(masks) + len(neue), neu_px=int(sum(len(s["ys"]) for s, _, _ in plaetze)),
                   atlas_verloren=verloren, farbe_mittel=round(float(abw.mean()), 2) if len(abw) else 0.0,
                   farbe_ueber48=round(float((abw > FARBE_MAX).mean()), 4) if len(abw) else 0.0,
                   neue_rechtecke=[list(m) for m in neue], erstes_neues=len(masks),
                   alte_texel_abw_mittel=round(float(alt_abw.mean()), 2) if len(alt_abw) else 0.0,
                   alte_texel_abw_max=int(alt_abw.max()) if len(alt_abw) else 0)
    return sec, (idx_neu, pal_neu), bericht


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--ppm", required=True, help="probe_bg_dump-Ordner (ROOMrrrcc.ppm)")
    ap.add_argument("--cache", required=True, help="Regionen-Cache des Generators (ROOMxxxx_cc.npz, schliessen_r41.gewachsen)")
    ap.add_argument("--out", required=True)
    ap.add_argument("--quelle", default=MASKS)
    ap.add_argument("--inc", default=None, help="C-Tabelle fuer den Riegel (tests/unit/gen_r41_masken36_platziert.inc)")
    ap.add_argument("--reserve", action="append", default=[], help="RAUM:CUT:N - N Plaetze des Cuts frei lassen (z.B. fuer teilen_r41.py)")
    ap.add_argument("--bericht-dazu", default=None, help="platzieren_r41.json eines frueheren Laufs: Raeume, die hier nicht laufen, von dort")
    a = ap.parse_args()
    reserve = {}
    for r in a.reserve:
        rr, cc, nn = r.split(":")
        reserve[(int(rr, 16), int(cc))] = int(nn)
    os.makedirs(a.out, exist_ok=True)
    bericht = {}
    for f in sorted(os.listdir(a.quelle)):
        if not (len(f) == 12 and f.endswith(".MSK") and f[4] in "3456"):
            continue
        room = f[:8]
        rid = int(room[4:], 16)
        if rid & 1 or (a.raeume and room not in a.raeume):
            continue
        blob = open(os.path.join(a.quelle, f), "rb").read()
        if open(os.path.join(a.quelle, "ROOM%04X.MSK" % (rid | 1)), "rb").read() != blob:
            raise SystemExit("%s: Elza-Datei nicht bytegleich" % room)
        secs, n_cut = SR.sektionen(blob)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "%s.RDT" % room), "rb").read()
        at = original.atlas(room)
        offs = struct.unpack_from("<%dI" % n_cut, blob, 12)
        alle = {}
        for cut in range(n_cut):
            if offs[cut]:
                ende = min([o for o in offs if o > offs[cut]] + [len(blob)])
                alle[cut] = blob[offs[cut]:ende]
        b_raum, tims = {}, {}
        for cut, sek in sorted(secs.items()):
            tp = os.path.join(a.quelle, "%s_PRI%02d.TIM" % (room, cut))
            gp = os.path.join(a.cache, "%s_%02d.npz" % (room, cut))
            if cut not in at or not os.path.exists(tp) or not os.path.exists(gp):
                continue
            bild = SR.bg_lesen(a.ppm, rid, cut)
            if bild is None:
                continue
            idx_e, clut_e = maskenbild.lies_tim(tp)
            z = np.load(gp)
            grown = {(int(k.split("_")[0]), int(k.split("_")[1])): z[k] for k in z.files}
            idx_o, clut_o = at[cut]
            sec, idx_neu, b = cut_bearbeiten(rdt, cut, sek, idx_e, clut_e, idx_o, clut_o, bild, grown, reserve.get((rid, cut), 0))
            b["komponenten"] = [{k: v for k, v in e.items() if k != "maske"} for e in b.get("komponenten", [])]
            b_raum[cut] = b
            if sec is None:
                continue
            # Probe: alter Teil der Sektion bytegleich (Gruppen und Rechtecke)
            o = offs[cut]
            ng, nm = len(sek["groups"]), len(sek["masks"])
            if sec[4:4 + 8 * ng] != blob[o + 4:o + 4 + 8 * ng]:
                raise SystemExit("%s Cut %d: alte Gruppen nicht bytegleich" % (room, cut))
            o_neu = 4 + 8 * (ng + b["neu"])
            if sec[o_neu:o_neu + 12 * nm] != blob[o + 4 + 8 * ng:o + 4 + 8 * ng + 12 * nm]:
                raise SystemExit("%s Cut %d: alte Rechtecke nicht bytegleich" % (room, cut))
            alle[cut] = sec
            tims[cut] = SR.atlasmod.tim_aus(idx_neu[0], idx_neu[1])
            print("%s C%-2d +%3d Rechtecke (%3d -> %3d, Stufe %d), %5d neue Punkte, Farbe Mittel %.1f, alte Texel %.2f (max %d), "
                  "Atlas verloren %d" % (room, cut, b["neu"], nm, b["rechtecke"], b["stufe"], b["neu_px"], b["farbe_mittel"],
                                         b["alte_texel_abw_mittel"], b["alte_texel_abw_max"], b["atlas_verloren"]), flush=True)
        if tims:
            out = geom.pack_container(alle, n_cut)
            for r in (rid, rid | 1):
                open(os.path.join(a.out, "ROOM%04X.MSK" % r), "wb").write(out)
                for cut, tb in tims.items():
                    open(os.path.join(a.out, "ROOM%04X_PRI%02d.TIM" % (r, cut)), "wb").write(tb)
        bericht[room] = b_raum
    if a.bericht_dazu:
        for room, cuts in json.load(open(a.bericht_dazu)).items():
            bericht.setdefault(room, cuts)
    json.dump(bericht, open(os.path.join(a.out, "platzieren_r41.json"), "w"), indent=1, default=str)
    if a.inc:
        inc_schreiben(a.inc, bericht)


def inc_schreiben(pfad, bericht):
    """Tabelle fuer tests/unit/test_r41_masken36.c (platziert): je Sektion der erste neue Rechteck-Index und die Zahl, je
    angenommener Komponente ihr Atlas-Kasten, die Verschiebung und die Zahl der verifizierten Texel."""
    sek, komp = [], []
    for room, cuts in sorted(bericht.items()):
        rid = int(room[4:], 16)
        for cut, b in sorted(cuts.items(), key=lambda kv: int(kv[0])):
            if not b.get("neu"):
                continue
            sek.append((rid, int(cut), b["erstes_neues"], b["neu"], b["neu_px"]))
            for e in b["komponenten"]:
                if e.get("angenommen") and e.get("neu_px", 0) > 0:
                    komp.append((rid, int(cut), e["komp"], e["box"][0], e["box"][1], e["box"][2], e["box"][3], e["shift"][0],
                                 e["shift"][1], e["verifiziert"]))
    NL = chr(10)
    kopf = ["/* Erzeugt von re15_port/tools/maske/platzieren_r41.py (Runde 41 Spur M, Nachbesserung 4) - NICHT von Hand aendern.",
            " * Unplatzierte Kuenstler-Komponenten des SLD-Atlas der Stages 3..6, per Lage-Suche platziert (Regel im Kopf des Werkzeugs).",
            " * r41_platz_sek: Raum (Leon-Datei; Elza xxx1 bytegleich), Cut, erster neuer Rechteck-Index, Zahl neuer Rechtecke, neue Punkte.",
            " * r41_platz_komp: Raum, Cut, Komponente, Atlas-Kasten x0 y0 x1 y1 (exklusiv), Verschiebung Atlas->Bild dx dy, verifizierte Texel. */"]
    zeilen = kopf + ["static const r41_platz_sek_t r41_platz_sek[] = {"]
    zeilen += ["    { 0x%04X, %2d, %3d, %3d, %5d }," % s for s in sek]
    zeilen += ["    { 0x0000, 0xFF, 0, 0, 0 }   /* Ende */", "};", "static const r41_platz_komp_t r41_platz_komp[] = {"]
    zeilen += ["    { 0x%04X, %2d, %3d, %3d, %3d, %3d, %3d, %4d, %4d, %5d }," % k for k in komp]
    zeilen += ["    { 0x0000, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0 }   /* Ende */", "};"]
    with open(pfad, "w", newline=NL) as f:
        f.write(NL.join(zeilen) + NL)
    print("Tabelle: %d Sektionen, %d Komponenten -> %s" % (len(sek), len(komp), pfad))


def SR_tim_mit_index(tb, idx):
    """8-bpp-TIM: CLUT-Block bytegleich lassen, Indexbild ersetzen (wie kacheln_r41.tim_mit_index)."""
    magic, flag = struct.unpack_from("<II", tb, 0)
    if magic != 0x10:
        raise SystemExit("kein TIM")
    o = 8
    if flag & 8:
        o += struct.unpack_from("<I", tb, o)[0]
    blen, px, py, pw, ph = struct.unpack_from("<IHHHH", tb, o)
    if idx.shape != (ph, pw * 2):
        raise SystemExit("TIM-Groesse %s != %s" % (idx.shape, (ph, pw * 2)))
    return tb[:o + 12] + idx.astype(np.uint8).tobytes() + tb[o + 12 + pw * 2 * ph:]


if __name__ == "__main__":
    main()
