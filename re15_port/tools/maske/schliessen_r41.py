"""Runde 41 Spur M (masken36): die Generator-Masken der Stages 3..6 SCHLIESSEN.

Auftrag (Nutzer 2026-10-23): "Die von dir erstellten Masken fuer STAGE3-6 sind relativ unbrauchbar, weil die ueberall
Loecher aufweisen" -> "danach kannst du deine Masken schliessen in der naechsten Version."
Dossier: analysis/befunde_runde41/M_masken36.md.

BEFUND (Dossier 1.1): die Loecher der Uebersichtsbilder pri/STAGE3-6/*_port.png kamen aus einem Fehler im Python-
Entpacker (original.py, behoben). Im Spiel sind die Rechtecke des Generators (analysis/esp_masken_2026-09-03/
gen_masken.py, b8db7b2b/02c62294) VOLL deckend; was dort fehlt, ist der Rand: der Generator hat seine eigene, verifizierte
Region (find_blocks NCC >= 0.97 + region_grow, Praezision 99,39 %) nur mit den groessten VOLLEN Rechtecken ueberdeckt
(rects_from_mask, "Kleinkram < 16" faellt weg) - jede schraege Objektkante wird zur Treppe, Kerben und kleine Inseln
bleiben offen, und die Figur dahinter scheint dort durch.

SCHLIESSEN = die verifizierte Region des Generators VOLLSTAENDIG zeichnen:
  1. Region je Verschiebung exakt wie der Generator (gen_region.find_blocks block 16 / stride 4 / NCC 0.97,
     region_grow tol 0.55) - Probe: jedes der 3239 vorhandenen Rechtecke liegt vollstaendig in der Region seiner
     Verschiebung, keine Verschiebung fehlt (Dossier 1.3).
  2. Tiefe exakt nach der Regel des Generators (gen_masken.depth_map_for_cut: je Bildspalte Bodenkontakt der
     Komponente auf y = 0, DEPTH_SAFETY 0.90; Rechteck = int(Median der Fenstertiefen)) - Probe: 3239 von 3239
     vorhandenen Rechtecktiefen bitgleich nachgerechnet.
  3. Saum = Regionspunkte, die unter KEINEM vorhandenen Rechteck liegen. Die vorhandenen Rechtecke bleiben
     unveraendert (Lage, Groesse, Tiefe, Reihenfolge -> der Ausschluss masken36_r37.c gilt fuer sie weiter);
     der Saum kommt in NEUE Rechtecke hinten an der Sektion.
  4. Ein Saum-Rechteck deckt nur SEINE Punkte (eigener Atlas mit Palettenindex 0 ringsum, wie die Freistellungen;
     Texel = Hintergrund an der Zielstelle, erste freie Atlasstelle). Seine Tiefe ist der Median der Spaltentiefen
     seiner Punkte; wie fein zerlegt wird, entscheidet die FEINSTE Tiefenstufe, die die Engine-Grenzen haelt
     (105 Rechtecke je Cut, re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT; 256x256-Atlas) - dieselbe Regel wie bau_p2.bau_cut.
  5. Echte Zwischenraeume bleiben offen: ein Punkt, der nicht zur verifizierten Region gehoert (Kuenstler-Atlas
     Index 0 = Gelaender-/Gitterluecke, oder Farbe passt nicht zur Verschiebung), wird nie gedeckt.
Die vorhandenen Rechtecke behalten Stelle, Gruppe und Bytes (Probe 3: alter Teil der Sektion bytegleich); ihre Texel
liegen im eigenen Atlas an derselben Stelle wie im BSS-Atlas (Inhalt = Hintergrund an der Zielstelle). Tiefe aus der
Erreichbarkeit fuer Rechtecke, die eine Figur VOR dem Objekt verdecken wuerden: Laufzeit-Tabelle (engine/src/
masken36_r41.c), erzeugt von test_r37_masken36 "erzeuge_tiefe" - nicht in dieser Datei.

Aufruf:
  python re15_port/tools/maske/schliessen_r41.py --ppm <probe_bg_dump-Ordner> --out <Ordner> [--cache <Ordner>]
         [ROOMxxxx ...]
Schreibt je Raumpaar ROOMxxx0/xxx1: ROOMxxxx.MSK + ROOMxxxx_PRIcc.TIM (bytegleich fuer beide Varianten, wie bisher die
MSK-Dateien) und einen Bericht <out>/schliessen_r41.json.
"""
import argparse
import json
import os
import struct
import sys

import numpy as np
from scipy import ndimage

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HIER)
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "esp_masken_2026-09-03"))

import atlas as atlasmod          # noqa: E402
import geom                       # noqa: E402
import original                   # noqa: E402
from gen_region import find_blocks, region_grow   # noqa: E402  (unveraendert der Generator vom 2026-09-03)

PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
MASKS = os.path.join(PSX, "MASKS")

# Parameter des Generators (gen_masken.py build_cut / Kopf) - NICHT neu gewaehlt, nur uebernommen:
NCC_MIN = 0.97        # gen_masken.py NCC_MIN
BLOCK = 16            # find_blocks(..., block=16, stride=4, ...)
STRIDE = 4
DEPTH_SAFETY = 0.90   # gen_masken.py DEPTH_SAFETY (END-ZU-ENDE kalibriert)
MAX_MASKEN = 105      # re15_pri.h RE15_PRI_MAX_MASKS_PER_CUT (= Original-Maximum RDT-Header Byte[7], gen_masken.py)
# Tiefenstufen wie bau_p2.bau_cut (1, 2, 3, 4, 6, 8, 12, 16), davor 0 = jede Spaltentiefe genau.
STUFEN = (0, 1, 2, 3, 4, 6, 8, 12, 16)


def lum(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def clut_rgb(clut):
    c = clut.astype(np.int32)
    return np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], 1)


def sektionen(blob):
    """R15M-Container -> {cut: {"groups": [(n, base, gx, gy)], "masks": [(sx, sy, dxu8, dyu8, dep, size, w, h, gi)],
    "decl": deklarierte Zahl}} (Leser wie maskenbild.masken / pri_common.c re15_pri_parse_section)."""
    assert blob[:4] == b"R15M"
    _, nc = struct.unpack_from("<II", blob, 4)
    offs = struct.unpack_from("<%dI" % nc, blob, 12)
    out = {}
    for cut in range(nc):
        o = offs[cut]
        if not o:
            continue
        gc, mc = struct.unpack_from("<HH", blob, o)
        p = o + 4
        groups = []
        for _ in range(gc):
            groups.append(struct.unpack_from("<HHhh", blob, p))
            p += 8
        masks, gi, used = [], 0, 0
        for _ in range(sum(g[0] for g in groups)):
            sx, sy, dx, dy = blob[p], blob[p + 1], blob[p + 2], blob[p + 3]
            dep, size = struct.unpack_from("<HH", blob, p + 4)
            p += 8
            if (size & 0xF000) == 0:
                w, h = struct.unpack_from("<HH", blob, p)
                p += 4
            else:
                w = h = (size >> 12) * 8
            while gi < gc and used >= groups[gi][0]:
                gi += 1
                used = 0
            used += 1
            masks.append((sx, sy, dx, dy, dep, size, w, h, gi))
        out[cut] = {"groups": groups, "masks": masks, "decl": mc}
    return out, nc


def bildlage(sek, m):
    g = sek["groups"][m[8]]
    X = ((m[2] + g[2] + 0x8000) & 0xFFFF) - 0x8000
    Y = ((m[3] + g[3] + 0x8000) & 0xFFFF) - 0x8000
    return X, Y


def bg_lesen(ppm_dir, rid, cut):
    from PIL import Image
    p = os.path.join(ppm_dir, "ROOM%03X%02d.ppm" % (rid >> 4, cut))
    if not os.path.exists(p):
        return None
    return np.asarray(Image.open(p).convert("RGB"), np.uint8)


def gewachsen(idx, clut, bgimg, cache=None):
    """Region je Verschiebung wie gen_masken.build_cut (Atlas = Engine-Entpacker, Hintergrund = probe_bg_dump)."""
    if cache and os.path.exists(cache):
        z = np.load(cache)
        return {(int(k.split("_")[0]), int(k.split("_")[1])): z[k] for k in z.files}
    at = clut_rgb(clut)[idx].astype(np.int16)
    B = bgimg.astype(np.int16)
    opaque = idx != 0
    blocks = find_blocks(lum(at), opaque, lum(B), block=BLOCK, stride=STRIDE, min_ncc=NCC_MIN)
    g = region_grow(lum(at), opaque, lum(B), blocks, block=BLOCK) if blocks else {}
    if cache:
        os.makedirs(os.path.dirname(cache), exist_ok=True)
        np.savez_compressed(cache, **{"%d_%d" % k: v for k, v in g.items()})
    return g


def region_im_bild(grown):
    """-> {shift: bool 240x320} und die Vereinigung."""
    per, alle = {}, np.zeros((240, 320), bool)
    for (dx, dy), m in grown.items():
        ys, xs = np.nonzero(m)
        by, bx = ys + dy, xs + dx
        ok = (by >= 0) & (by < 240) & (bx >= 0) & (bx < 320)
        f = np.zeros((240, 320), bool)
        f[by[ok], bx[ok]] = True
        per[(dx, dy)] = f
        alle |= f
    return per, alle


def tiefenkarte(rdt, cut, screen):
    """gen_masken.depth_map_for_cut, Zeile fuer Zeile (geom.build_view/vz_at_floor == q3b_tiefe, gleiche Formeln)."""
    cam = struct.unpack_from("<I", rdt, 0x24)[0]
    rec = cam + cut * 32
    fov = struct.unpack_from("<H", rdt, rec + 2)[0]
    P = struct.unpack_from("<iii", rdt, rec + 4)
    T = struct.unpack_from("<iii", rdt, rec + 0x10)
    v = geom.build_view(fov, *P, *T)
    if not v:
        return None
    R, t, H = v
    if H <= 0:
        return None
    lab, n = ndimage.label(screen, structure=np.ones((3, 3)))
    dep = np.zeros((240, 320), np.int32)
    for k in range(1, n + 1):
        m = lab == k
        for x in np.where(m.any(0))[0]:
            rows = np.where(m[:, x])[0]
            yb = int(rows.max())
            yq = float(yb) if yb < 239 else 239.0
            zz = geom.vz_at_floor(R, t, H, x + 0.5, yq, 0)
            if not zz:
                continue
            dep[rows, x] = max(1, min(1023, int(zz * DEPTH_SAFETY / 64.0)))
    return dep


def stuecke_bilden(saum_per_g, dep):
    """Saum je Verschiebung -> Grundstuecke: 8-zusammenhaengende Punkte GLEICHER Spaltentiefe (Stufe 0)."""
    st = []
    for sh, f in saum_per_g.items():
        if not f.any():
            continue
        for d in np.unique(dep[f]):
            lab, n = ndimage.label(f & (dep == d), structure=np.ones((3, 3)))
            for k in range(1, n + 1):
                ys, xs = np.nonzero(lab == k)
                st.append({"sh": sh, "ys": ys, "xs": xs, "dmin": int(d), "dmax": int(d),
                           "box": (int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1)})
    return st


def _flaeche(b):
    return (b[2] - b[0]) * (b[3] - b[1])


def _vereint(a, b):
    return (min(a[0], b[0]), min(a[1], b[1]), max(a[2], b[2]), max(a[3], b[3]))


def zusammenlegen(stuecke, stufe, budget):
    """Gierig zusammenlegen: gleiche Verschiebung, Tiefenspanne <= stufe; zuerst, was die Kastenflaeche nicht
    vergroessert, dann (solange ueber dem Budget) das Paar mit dem kleinsten Flaechenzuwachs."""
    st = [dict(s) for s in stuecke]
    while True:
        best = None
        for i in range(len(st)):
            a = st[i]
            for j in range(i + 1, len(st)):
                b = st[j]
                if a["sh"] != b["sh"]:
                    continue
                if max(a["dmax"], b["dmax"]) - min(a["dmin"], b["dmin"]) > stufe:
                    continue
                u = _vereint(a["box"], b["box"])
                kosten = _flaeche(u) - _flaeche(a["box"]) - _flaeche(b["box"])
                if best is None or kosten < best[0]:
                    best = (kosten, i, j, u)
        if best is None:
            break
        if best[0] > 0 and len(st) <= budget:
            break
        k, i, j, u = best
        a, b = st[i], st[j]
        a["ys"] = np.concatenate([a["ys"], b["ys"]])
        a["xs"] = np.concatenate([a["xs"], b["xs"]])
        a["dmin"] = min(a["dmin"], b["dmin"])
        a["dmax"] = max(a["dmax"], b["dmax"])
        a["box"] = u
        del st[j]
    return st


def frei_platz(belegt, w, h):
    """Erste freie Stelle (Zeile vor Spalte) fuer einen w x h-Kasten im Atlas; None, wenn keine."""
    H, W = belegt.shape
    if w > W or h > H:
        return None
    S = np.zeros((H + 1, W + 1), np.int32)
    S[1:, 1:] = np.cumsum(np.cumsum(belegt.astype(np.int32), 0), 1)
    summe = S[h:, w:] - S[:-h, w:] - S[h:, :-w] + S[:-h, :-w]
    frei = np.argwhere(summe == 0)
    if not len(frei):
        return None
    ay, ax = int(frei[0][0]), int(frei[0][1])
    return ax, ay


def palette_und_index(rgb, opak):
    """255 Farben (Median-Schnitt) ueber die deckenden Texel, Index 0 = durchsichtig - wie atlas.build_rechtecke."""
    from PIL import Image
    idx = np.zeros(opak.shape, np.uint8)
    pix = rgb[opak]
    pal_img = Image.fromarray(pix.reshape(-1, 1, 3)).quantize(colors=255, method=Image.MEDIANCUT)
    roh = pal_img.getpalette() or []
    n = min(255, len(roh) // 3)
    pal = np.array(roh[:n * 3], np.uint8).reshape(n, 3)
    flat = pix.astype(np.int32)
    d = ((flat[:, None, :] - pal[None, :, :].astype(np.int32)) ** 2).sum(2)
    idx[opak] = (np.argmin(d, 1) + 1).astype(np.uint8)
    return idx, pal


def schliesse_cut(rdt, cut, sek, idx, clut, bgimg, cache=None):
    """-> (section bytes, tim bytes, bericht) oder (None, None, bericht).

    Atlas-Aufbau: die VORHANDENEN Rechtecke behalten ihre Stelle (src) und ihre Gruppen - ihre Texel liegen im eigenen
    Atlas dort, wo sie im BSS-Atlas lagen (Inhalt = Hintergrund an der Zielstelle, voll deckend); ihr Teil der Sektion
    bleibt bytegleich. Die Saum-Rechtecke kommen in die freien Stellen (erste freie Stelle, groesste zuerst), je eine
    eigene Gruppe mit Anker = Ziel - Quelle, deckend NUR an ihren Saum-Punkten."""
    bericht = {"cut": cut}
    grown = gewachsen(idx, clut, bgimg, cache)
    per_g, screen = region_im_bild(grown)
    dep = tiefenkarte(rdt, cut, screen)
    alt = []
    unter = np.zeros((240, 320), bool)
    for i, m in enumerate(sek["masks"]):
        X, Y = bildlage(sek, m)
        sx, sy, w, h, d0 = m[0], m[1], m[6], m[7], m[4]
        sh = (X - sx, Y - sy)
        # Probe 1: Rechteck liegt vollstaendig in der Region seiner Verschiebung (sonst ist die Rekonstruktion falsch)
        if sh not in grown or not grown[sh][sy:sy + h, sx:sx + w].all():
            raise RuntimeError("Cut %d Rechteck %d: nicht in der Generator-Region (%s)" % (cut, i, sh))
        win = dep[max(0, Y):Y + h, max(0, X):X + w]
        if int(np.median(win[win > 0])) != d0:
            raise RuntimeError("Cut %d Rechteck %d: Tiefe %d != Generator-Regel %d" % (cut, i, d0, int(np.median(win[win > 0]))))
        unter[max(0, Y):Y + h, max(0, X):X + w] = True
        alt.append((sx, sy, X, Y, w, h, d0))
    # Atlas: vorhandene Rechtecke an ihrer alten Stelle
    a_rgb = np.zeros((256, 256, 3), np.uint8)
    a_op = np.zeros((256, 256), bool)
    for (sx, sy, X, Y, w, h, d0) in alt:
        a_rgb[sy:sy + h, sx:sx + w] = bgimg[Y:Y + h, X:X + w]
        a_op[sy:sy + h, sx:sx + w] = True
    belegt0 = a_op.copy()
    saum = {sh: f & ~unter for sh, f in per_g.items()}
    saum_px = int(sum(int(f.sum()) for f in saum.values()))
    bericht.update(rechtecke_alt=len(alt), saum_px=saum_px, region_px=int(screen.sum()), deck_alt_px=int(unter.sum()))
    budget = MAX_MASKEN - len(alt)
    stuecke = stuecke_bilden(saum, dep)
    wahl = None
    for stufe in STUFEN:
        neu = zusammenlegen(stuecke, stufe, budget)
        if len(neu) > budget:
            continue
        belegt = belegt0.copy()
        rgb, op = a_rgb.copy(), a_op.copy()
        plaetze, ok = [], True
        for s in sorted(neu, key=lambda s: -_flaeche(s["box"])):
            x0, y0, x1, y1 = s["box"]
            w, h = x1 - x0, y1 - y0
            pl = frei_platz(belegt, w, h)
            if pl is None:
                ok = False
                break
            ax, ay = pl
            belegt[ay:ay + h, ax:ax + w] = True
            ys, xs = s["ys"], s["xs"]
            rgb[ys - y0 + ay, xs - x0 + ax] = bgimg[ys, xs]
            op[ys - y0 + ay, xs - x0 + ax] = True
            plaetze.append((s, ax, ay))
        if not ok:
            bericht.setdefault("atlas_fehler", []).append("Stufe %d: kein Platz" % stufe)
            continue
        wahl = (stufe, plaetze, rgb, op, neu)
        break
    if wahl is None:
        bericht["fehler"] = "keine Stufe haelt Budget %d / Atlas (Stuecke %d)" % (budget, len(stuecke))
        return None, None, bericht
    stufe, plaetze, rgb, op, neu = wahl
    a_idx, pal = palette_und_index(rgb, op)
    tim = atlasmod.tim_aus(a_idx, pal)
    groups = [(g[0], g[2], g[3]) for g in sek["groups"]]
    masks = [(sx, sy, X, Y, w, h, d0) for (sx, sy, X, Y, w, h, d0) in alt]
    neue = []
    for (s, ax, ay) in sorted(plaetze, key=lambda t: (t[0]["box"][1], t[0]["box"][0])):
        x0, y0, x1, y1 = s["box"]
        d = int(np.median(dep[s["ys"], s["xs"]]))
        neue.append((ax, ay, x0, y0, x1 - x0, y1 - y0, d))
    for m in neue:
        groups.append((1, m[2] - m[0], m[3] - m[1]))
    sec = geom.pack_section(groups, masks + neue)
    fehler_tiefe = [abs(int(np.median(dep[s["ys"], s["xs"]])) - int(v)) for s in neu for v in dep[s["ys"], s["xs"]]]
    bericht.update(stufe=stufe, rechtecke_neu=len(neue), rechtecke=len(masks) + len(neue),
                   tiefe_abw_max=int(max(fehler_tiefe)) if fehler_tiefe else 0,
                   atlas_px=int((belegt0.sum())), atlas_belegt=int(op.sum()))
    return sec, tim, bericht


def deckung(sec_bytes, tim_bytes, n_cut, cut):
    """Deckung + Tiefe ueber denselben Leser wie die Sonde (maskenbild.masken, abnahme.deckung_und_tiefe)."""
    import abnahme
    import maskenbild
    blob = geom.pack_container({cut: sec_bytes}, n_cut)
    ms = maskenbild.masken(blob, cut) or []
    idx = maskenbild.lies_tim_bytes(tim_bytes)[0]
    return abnahme.deckung_und_tiefe(ms, idx)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("raeume", nargs="*")
    ap.add_argument("--ppm", required=True)
    ap.add_argument("--out", required=True)
    ap.add_argument("--cache", default=None)
    a = ap.parse_args()
    os.makedirs(a.out, exist_ok=True)
    dateien = sorted(f for f in os.listdir(MASKS) if len(f) == 12 and f.endswith(".MSK") and f[4] in "3456")
    bericht = {}
    for f in dateien:
        room = f[:8]
        rid = int(room[4:], 16)
        if rid & 1:
            continue
        if a.raeume and room not in a.raeume:
            continue
        blob = open(os.path.join(MASKS, f), "rb").read()
        zwilling = os.path.join(MASKS, "ROOM%04X.MSK" % (rid | 1))
        if not os.path.exists(zwilling) or open(zwilling, "rb").read() != blob:
            raise SystemExit("%s: Elza-Datei fehlt oder ist nicht bytegleich" % room)
        secs, n_cut = sektionen(blob)
        rdt = open(os.path.join(PSX, "STAGE%d" % (rid >> 12), "%s.RDT" % room), "rb").read()
        at = original.atlas(room)
        neu_secs, tims, b_raum = {}, {}, {}
        for cut, sek in sorted(secs.items()):
            if sek["decl"] & 0xFF != len(sek["masks"]):
                raise SystemExit("%s Cut %d: deklariert %d != gebaut %d" % (room, cut, sek["decl"], len(sek["masks"])))
            bgimg = bg_lesen(a.ppm, rid, cut)
            idx, clut = at[cut]
            cache = os.path.join(a.cache, "%s_%02d.npz" % (room, cut)) if a.cache else None
            sec, tim, b = schliesse_cut(rdt, cut, sek, idx, clut, bgimg, cache)
            if sec is None:
                raise SystemExit("%s Cut %d: %s" % (room, cut, b.get("fehler")))
            # Probe 2: die neue Deckung ist bitgenau alte Rechtecke + Saum, nichts sonst
            deck, tief = deckung(sec, tim, n_cut, cut)
            grown = gewachsen(idx, clut, bgimg, cache)
            per_g, screen = region_im_bild(grown)
            alt_unter = np.zeros((240, 320), bool)
            for m in sek["masks"]:
                X, Y = bildlage(sek, m)
                alt_unter[max(0, Y):Y + m[7], max(0, X):X + m[6]] = True
            soll = alt_unter | screen
            b["deck_px"] = int(deck.sum())
            b["fehlt"] = int((soll & ~deck).sum())
            b["zuviel"] = int((deck & ~soll).sum())
            if b["fehlt"] or b["zuviel"]:
                raise SystemExit("%s Cut %d: Deckung != Soll (fehlt %d, zuviel %d)" % (room, cut, b["fehlt"], b["zuviel"]))
            alt_bytes = blob[struct.unpack_from("<I", blob, 12 + 4 * cut)[0]:][:4 + 8 * len(sek["groups"])]
            if sec[4:4 + 8 * len(sek["groups"])] != alt_bytes[4:]:
                raise SystemExit("%s Cut %d: alte Gruppen nicht bytegleich" % (room, cut))
            o_alt = struct.unpack_from("<I", blob, 12 + 4 * cut)[0] + 4 + 8 * len(sek["groups"])
            n_alt = 12 * len(sek["masks"])
            o_neu = 4 + 8 * (len(sek["groups"]) + b["rechtecke_neu"])
            if sec[o_neu:o_neu + n_alt] != blob[o_alt:o_alt + n_alt]:
                raise SystemExit("%s Cut %d: alte Rechtecke nicht bytegleich" % (room, cut))
            neu_secs[cut], tims[cut] = sec, tim
            b_raum[cut] = b
            print("%s C%-2d alt %3d +%3d Rechtecke (Stufe %d), Saum %5d px, Atlas %5d px" % (
                room, cut, b["rechtecke_alt"], b["rechtecke_neu"], b["stufe"], b["saum_px"], b["atlas_px"]), flush=True)
        out = geom.pack_container(neu_secs, n_cut)
        for r in (rid, rid | 1):
            open(os.path.join(a.out, "ROOM%04X.MSK" % r), "wb").write(out)
            for cut, tim in tims.items():
                open(os.path.join(a.out, "ROOM%04X_PRI%02d.TIM" % (r, cut)), "wb").write(tim)
        bericht[room] = b_raum
    json.dump(bericht, open(os.path.join(a.out, "schliessen_r41.json"), "w"), indent=1)


if __name__ == "__main__":
    main()
