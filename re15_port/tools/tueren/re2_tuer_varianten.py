#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_tuer_varianten.py - je RE2-Tuerarchiv und je Variante: wo stehen Angel und Griff im Bild,
geht das Blatt vom Betrachter weg oder zu ihm hin, welche Meshes sind Blatt/Griff/Anbauteil.

Runde 31, Teil T2 (analysis/befunde_runde31/tueren_02_re2.md, Abschnitt 1 und 3).
Liest NUR info/re2leon/PSX.EXE und info/re2leon/COMMON/DOOR/DOORxx.DO2 (ueber tools/tor/tuerkatalog.py).
Schreibt NUR build/r31_tueren/t2/ (varianten.json, bilder/DOORxx_vN.png, Kontaktboegen).

Grundlage (alle Belege in tuerkatalog.py und analysis/tor_1170/03_tuersequenz.md / 04_tuerkatalog.md):
  - Skriptmaschine = tuerkatalog.VM (Handler je Opcode mit PC-Vorschub-Adresse).
  - Variante = var 12 = Payload+13 & 0x7f (@0x80013e5c lbu v0,13(a0) / @0x80013e6c andi v0,v0,0xff7f);
    die Werte kommen aus den Case-Saetzen des Switch in Skript 0.
  - Kamera Auge (10000,0,0) -> Ziel (0,0,0) (@0x80010830/@0x8001083c, @0x80013e38..3c jal 0x80076cb0):
    Bild-x = 160 + 290*z/(10000-x), Bild-y = 120 + 290*y/(10000-x); H = 290 (@0x80013e34 addiu a0,zero,290).
    Bildmitte (160,120): SetGeomOffset der Spiel-Aufrufer @0x80049cac/b0, @0x80068e80/88 (03 3.2).
  - Matrizenkette FUN_80014234 (Eltern * lokal), RotMatrix @0x8008e1f4 M = Rx*Ry*Rz.
  - Rueckseitenwurf: NCLIP (@0x800148d0), verworfen bei MAC0 < 0 (@0x800148f8 bgez).
  - Nur Dreiecke (@0x80014710/18, Schleife bis @0x80014b04).
  - Textur: 128x256 8 bpp, CLUT 0; Texel mit Farbwert 0x0000 wird nicht gezeichnet (PSX-Regel).
Das Bild ist eine Uebersicht (affine Textur, Z-Puffer statt OT, ohne Licht); Angel/Griff-Zahlen
kommen aus der Geometrie, nicht aus dem Bild.

Aufruf:
  python re15_port/tools/tueren/re2_tuer_varianten.py            # alle 55, JSON + Bilder + Boegen
  python re15_port/tools/tueren/re2_tuer_varianten.py --nur 00,2E --tabelle
"""
import argparse
import json
import math
import os
import sys

import numpy as np
from PIL import Image, ImageDraw

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools", "tor"))
import tuerkatalog as tk  # noqa: E402

OUT = os.path.join(REPO, "build", "r31_tueren", "t2")
W, H = 320, 240
BLATT_HOEHE = 3000       # Mesh-Hoehe ab der ein Teil "Blatt" heisst (04 2: alle Blaetter 5639..7163 hoch)


# ---------------------------------------------------------------------------------------
# Varianten aus Skript 0 (wie tuerkatalog.door_record)
# ---------------------------------------------------------------------------------------
def varianten(door):
    ins0, _ = tk.disasm_script(door, 0)
    cases = []
    for idx, i in enumerate(ins0):
        if i["op"] == 0x14:
            tgt = None
            for j in ins0[idx + 1:]:
                if j["op"] == 0x18:
                    tgt = j["f"]["script"]
                    break
                if j["op"] == 0x14:
                    continue
                if j["op"] in (0x16, 0x1A):
                    break
            cases.append((i["f"]["val"], tgt))
    if not cases:
        cases = [(0, None)]
    return cases


# ---------------------------------------------------------------------------------------
# Textur + Rasterer
# ---------------------------------------------------------------------------------------
def textur(door):
    d = door.data
    w, h = door.tex_w, door.tex_h
    idx = np.frombuffer(d, dtype=np.uint8, count=w * h, offset=door.tim_pix_off).reshape(h, w)
    pal = np.array(door.cluts[0], dtype=np.uint16)
    c = pal[idx]
    rgb = np.stack([(c & 31) << 3, ((c >> 5) & 31) << 3, ((c >> 10) & 31) << 3], axis=-1).astype(np.uint8)
    transparent = (c == 0)
    return rgb, transparent


def weltdreiecke(door, state):
    tr = tk.world_transforms(state)
    out = []
    for s in state:
        if s["mesh"] >= len(door.meshes):
            continue
        m = door.meshes[s["mesh"]]
        R, t = tr[s["obj"]]
        vw = [[a + b for a, b in zip(tk.mat_vec(R, v), t)] for v in m["verts"]]
        for tri in m["tris"]:
            if max(tri["v"]) < len(vw):
                out.append((s["obj"], s["mesh"], [vw[i] for i in tri["v"]], tri["uv"]))
    return out


def rendern(door, state, tex, markierung=None):
    rgb, transp = tex
    img = np.full((H, W, 3), 24, dtype=np.uint8)
    zb = np.full((H, W), np.inf)
    th, tw = transp.shape
    for obj, mesh, pw, uv in weltdreiecke(door, state):
        ps = [tk.project(p) for p in pw]
        if any(p is None for p in ps):
            continue
        (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = ps
        mac0 = x0 * y1 + x1 * y2 + x2 * y0 - x0 * y2 - x1 * y0 - x2 * y1
        if mac0 < 0:            # @0x800148f8 bgez: nur MAC0 >= 0 wird gezeichnet
            continue
        if mac0 == 0:
            continue
        xmin = max(int(math.floor(min(x0, x1, x2))), 0)
        xmax = min(int(math.ceil(max(x0, x1, x2))), W - 1)
        ymin = max(int(math.floor(min(y0, y1, y2))), 0)
        ymax = min(int(math.ceil(max(y0, y1, y2))), H - 1)
        if xmin > xmax or ymin > ymax:
            continue
        gx, gy = np.meshgrid(np.arange(xmin, xmax + 1) + 0.5, np.arange(ymin, ymax + 1) + 0.5)
        den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
        if den == 0:
            continue
        l0 = ((y1 - y2) * (gx - x2) + (x2 - x1) * (gy - y2)) / den
        l1 = ((y2 - y0) * (gx - x2) + (x0 - x2) * (gy - y2)) / den
        l2 = 1 - l0 - l1
        inside = (l0 >= 0) & (l1 >= 0) & (l2 >= 0)
        if not inside.any():
            continue
        z = l0 * z0 + l1 * z1 + l2 * z2
        u = l0 * uv[0][0] + l1 * uv[1][0] + l2 * uv[2][0]
        v = l0 * uv[0][1] + l1 * uv[1][1] + l2 * uv[2][1]
        ui = np.clip(u.astype(int), 0, tw - 1)
        vi = np.clip(v.astype(int), 0, th - 1)
        sub = zb[ymin:ymax + 1, xmin:xmax + 1]
        ok = inside & (z < sub) & (~transp[vi, ui])
        if not ok.any():
            continue
        sub[ok] = z[ok]
        tgt = img[ymin:ymax + 1, xmin:xmax + 1]
        tgt[ok] = rgb[vi[ok], ui[ok]]
    im = Image.fromarray(img)
    if markierung:
        dr = ImageDraw.Draw(im)
        for (x, y, farbe, text) in markierung:
            if x is None:
                continue
            dr.line([(x, 0), (x, H)], fill=farbe)
            dr.text((min(max(x + 2, 0), W - 40), 2 + (10 if farbe == (80, 220, 255) else 0)), text, fill=farbe)
    return im


# ---------------------------------------------------------------------------------------
# Rollen, Angel, Griff, Richtung
# ---------------------------------------------------------------------------------------
def welt(door, state, obj, lokal):
    tr = tk.world_transforms(state)
    R, t = tr[obj]
    return [a + b for a, b in zip(tk.mat_vec(R, lokal), t)]


def bild_x(p):
    q = tk.project(p)
    return None if q is None else q[0]


def seite(x):
    if x is None:
        return "-"
    if x < 150:
        return "links"
    if x > 170:
        return "rechts"
    return "mitte"


def analysiere(door, variante):
    vm, summ = tk.analyse_variant(door, variante)
    fr = vm.frames
    if not fr or not max(len(s) for s in fr):
        return {"variante": variante, "bilder": len(fr), "objekte": [], "leer": True,
                "toene": vm.sounds, "notizen": sorted(vm.notes)}, vm, None, None
    t0, t1 = tk.key_frames(door, vm, summ)
    st0 = fr[t0]
    meshes = door.meshes
    rollen = {}
    blatt = []
    for s in summ:
        m = meshes[s["mesh"]] if s["mesh"] < len(meshes) else None
        hoch = m["size"][1] if m else 0
        rx, ry, rz = s["rot_extreme"]
        _, ay, az = s["pos_ohne_fahrt_extrem"]
        bewegt = abs(ry) >= 64 or abs(ay) >= 200 or abs(az) >= 200
        if hoch >= BLATT_HOEHE and bewegt:
            rollen[s["obj"]] = "Blatt"
            blatt.append(s)
    for s in summ:
        if s["obj"] in rollen:
            continue
        m = meshes[s["mesh"]] if s["mesh"] < len(meshes) else None
        groesse = m["size"] if m else [0, 0, 0]
        if s["parent"] in rollen and rollen[s["parent"]] == "Blatt" and max(groesse) < 1200:
            rollen[s["obj"]] = "Griff"
        elif max(groesse) >= BLATT_HOEHE:
            rollen[s["obj"]] = "Blatt (fest)" if s["first_move"] is None or (
                abs(s["rot_extreme"][1]) < 64 and abs(s["pos_ohne_fahrt_extrem"][1]) < 200
                and abs(s["pos_ohne_fahrt_extrem"][2]) < 200) else "Blatt"
        else:
            rollen[s["obj"]] = "Anbauteil"
    # Hauptblatt = das bewegte Blatt mit der kleinsten Objektnummer
    haupt = min(blatt, key=lambda s: s["obj"]) if blatt else None
    ergebnis = {"variante": variante, "bilder": len(fr), "t0": t0, "t_offen": t1,
                "objekte": [], "toene": vm.sounds, "notizen": sorted(vm.notes),
                "schliesston": vm.global248,
                "blende": vm.fades}
    for s in summ:
        o = {"obj": s["obj"], "mesh": s["mesh"], "eltern": s["parent"], "flags": "0x%04x" % s["flags"],
             "pos0": s["pos0"], "rot0": s["rot0"], "rolle": rollen.get(s["obj"], "?"),
             "rot_weg": s["rot_extreme"], "hub_schub": s["pos_ohne_fahrt_extrem"],
             "bild_erste_bewegung": s["first_move"], "fahrt_von": s["fahrt_x_von"]}
        if s["obj"] in [x["obj"] for x in st0]:
            o["bild_x_t0"] = round(bild_x(welt(door, st0, s["obj"], [0, 0, 0])) or -1, 1)
        ergebnis["objekte"].append(o)
    mitte_t = t0
    if haupt is not None:
        m = meshes[haupt["mesh"]]
        # freie Kante: Mesh-Ecke mit dem groessten |z| (Ursprung = Angel, 03 9.2) auf halber Hoehe
        zs = [v[2] for v in m["verts"]]
        zfrei = min(zs) if abs(min(zs)) >= abs(max(zs)) else max(zs)
        yhalb = (m["bbox_min"][1] + m["bbox_max"][1]) / 2.0
        angel_w0 = welt(door, st0, haupt["obj"], [0, yhalb, 0])
        frei_w0 = welt(door, st0, haupt["obj"], [0, yhalb, zfrei])
        ry = haupt["rot_extreme"][1]
        _, ay, az = haupt["pos_ohne_fahrt_extrem"]
        art = "dreht" if abs(ry) >= 64 else ("hebt" if abs(ay) >= abs(az) else "schiebt")
        # Offen-Zustand ohne Kamerafahrt (Wurzel-x auf t0)
        offen = tk.undolly(fr[t1], st0)
        frei_w1 = welt(door, offen, haupt["obj"], [0, yhalb, zfrei])
        angel_w1 = welt(door, offen, haupt["obj"], [0, yhalb, 0])
        if art == "dreht":
            richtung = "weg" if frei_w1[0] < angel_w1[0] - 50 else ("hin" if frei_w1[0] > angel_w1[0] + 50 else "?")
            a_von, a_bis = haupt["first_rot"], haupt["last_rot"]
        else:
            if art == "hebt":
                richtung = "hoch" if ay < 0 else "runter"
            else:
                sx0, sx1 = bild_x(frei_w0), bild_x(welt(door, offen, haupt["obj"], [0, yhalb, zfrei]))
                richtung = "nach rechts" if az > 0 else "nach links"
            a_von, a_bis = haupt["first_move"], (haupt["fahrt_x_von"] or haupt["last_move"])
        if a_von is not None and a_bis is not None:
            mitte_t = min((a_von + a_bis) // 2, len(fr) - 1)
        griffe = []
        for o in ergebnis["objekte"]:
            if o["rolle"] == "Griff":
                gw0 = welt(door, st0, o["obj"], [0, 0, 0])
                griffe.append({"obj": o["obj"], "mesh": o["mesh"], "bild_x": round(bild_x(gw0), 1),
                               "seite": seite(bild_x(gw0)), "lage_im_blatt": o["pos0"],
                               "dreht": o["rot_weg"]})
        ergebnis.update({
            "blatt_obj": haupt["obj"], "blatt_mesh": haupt["mesh"], "bewegung": art,
            "winkel": ry, "grad": tk.deg(ry),
            "angel_bild_x": round(bild_x(angel_w0), 1), "angel_seite": seite(bild_x(angel_w0)),
            "freie_kante_bild_x": round(bild_x(frei_w0), 1), "freie_kante_seite": seite(bild_x(frei_w0)),
            "richtung": richtung, "griffe": griffe,
            "freie_kante_welt_t0": [round(c) for c in frei_w0], "freie_kante_welt_offen": [round(c) for c in frei_w1],
            "bewegung_bilder": [a_von, a_bis], "t_mitte": mitte_t,
        })
    ergebnis["erstes_se_on"] = vm.sounds[0]["tick"] if vm.sounds else None
    return ergebnis, vm, t0, mitte_t


def zeichne_paar(door, vm, erg, t0, tm, tex):
    fr = vm.frames
    mk = []
    if erg.get("angel_bild_x") is not None:
        mk.append((erg["angel_bild_x"], 0, (255, 220, 60), "Angel"))
    for g in erg.get("griffe", []):
        mk.append((g["bild_x"], 0, (80, 220, 255), "Griff"))
    a = rendern(door, fr[t0], tex, mk)
    b = rendern(door, fr[tm], tex)
    im = Image.new("RGB", (W * 2 + 6, H + 16), (50, 50, 60))
    im.paste(a, (0, 16))
    im.paste(b, (W + 6, 16))
    dr = ImageDraw.Draw(im)
    txt = "%s V%d  Bild %d | Bild %d  %s %s" % (door.name, erg["variante"], t0, tm,
                                                 erg.get("bewegung", "-"), erg.get("richtung", ""))
    dr.text((4, 2), txt, fill=(255, 255, 255))
    return im


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--nur", default="")
    ap.add_argument("--tabelle", action="store_true")
    ap.add_argument("--keine-bilder", action="store_true")
    a = ap.parse_args()
    nur = [int(x, 16) for x in a.nur.split(",") if x] or list(range(tk.N_DOORS))
    os.makedirs(os.path.join(OUT, "bilder"), exist_ok=True)
    alle = {}
    boegen = []
    for idx in nur:
        door = tk.Door(idx)
        tex = textur(door)
        rec = {"archiv": door.name, "meshes": [{"mesh": m["index"], "dreiecke": m["n_tris"],
                                                "groesse": m["size"], "min": m["bbox_min"], "max": m["bbox_max"]}
                                               for m in door.meshes], "varianten": []}
        for v, tgt in varianten(door):
            erg, vm, t0, tm = analysiere(door, v)
            erg["aufbau_skript"] = tgt
            rec["varianten"].append(erg)
            if not a.keine_bilder and t0 is not None:
                im = zeichne_paar(door, vm, erg, t0, tm, tex)
                p = os.path.join(OUT, "bilder", "%s_v%d.png" % (door.name, v))
                im.save(p)
                boegen.append(im)
            if a.tabelle:
                print("%s V%d skr=%s bilder=%d %s %s  Angel %s (%.0f)  frei %s  Griffe %s  Se_on@%s  zu-Ton=%d" % (
                    door.name, v, tgt, erg["bilder"], erg.get("bewegung", "-"), erg.get("richtung", "-"),
                    erg.get("angel_seite", "-"), erg.get("angel_bild_x", -1) or -1, erg.get("freie_kante_seite", "-"),
                    [(g["mesh"], g["seite"]) for g in erg.get("griffe", [])], erg.get("erstes_se_on"),
                    erg.get("schliesston", 0)))
        alle[door.name] = rec
    with open(os.path.join(OUT, "varianten.json"), "w") as f:
        json.dump(alle, f, indent=1)
    if boegen and not a.nur:
        je = 12
        for k in range(0, len(boegen), je):
            teil = boegen[k:k + je]
            cols = 3
            rows = (len(teil) + cols - 1) // cols
            bw, bh = teil[0].size
            sh = Image.new("RGB", (cols * bw, rows * bh), (30, 30, 36))
            for i, im in enumerate(teil):
                sh.paste(im, ((i % cols) * bw, (i // cols) * bh))
            sh = sh.resize((sh.width // 2, sh.height // 2), Image.BILINEAR)
            sh.save(os.path.join(OUT, "bogen_varianten_%02d.png" % (k // je)))


if __name__ == "__main__":
    main()
