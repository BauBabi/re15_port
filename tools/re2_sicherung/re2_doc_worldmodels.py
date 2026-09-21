#!/usr/bin/env python3
"""re2_doc_worldmodels.py - die WELTMODELLE der RE2-Dokumente schneiden, entdoppeln, rendern.

Frage, die dieses Werkzeug beantwortet: liegt ein RE2-Dokument sichtbar als 3D-Objekt
im Raum, und wenn ja, welches Mesh ist das?

BELEGKETTE (alles aus info/re2leon/PSX.EXE, RAM-Adressen; Ghidra-Dump ghidra_re2_Leon.txt)

1) Op 0x4E = Item_aot_set. Dispatch-Tabelle 0x800A74C8; Eintrag 0x4E liegt @0x800A7600
   (0x800A7600-0x800A74C8 = 0x138 = 78*4) und zeigt auf LAB_80054CD4.

2) Feldlage des 22-Byte-Records, direkt am Handler abgelesen:
     80054CEC  lw   s0,0x1c(a0)   ; s0 = Task-PC = &record
     80054CF4  lhu  a1,0x12(s0)   ; +18 flag
     80054CF8  lbu  s2,0x14(s0)   ; +20 md1
     80054CFC  addiu v0,s0,0x16   ; Recordlaenge = 22
     80054D04  lbu  s1,0x1(s0)    ; +1  aot
     80054DD4  lbu  s1,0x15(s0)   ; +21 action
   Daraus (mit den Koordinaten dazwischen):
     +0 op +1 aot +2 sce +3 sat +4 nFloor +5 super +6 x +8 z +10 w +12 d
     +14 i_item +16 n_item +18 flag +20 md1 +21 action

3) md1 ist der Index in den OBJEKT-POOL des Raums, Basis 0x800D0324, Schrittweite 0x1F8:
     80054D98  sltiu v0,s2,0x20   ; Schranke md1 < 32
     80054D9C/DA0/DA4/DA8  v0 = md1*64 - md1 = md1*63, dann <<3  => md1*504 (0x1F8)
     80054DAC  addu v0,s3,v0      ; s3 = 0x800CC1E8
     80054DB4  sw   0x80000000,0x413c(v0)   ; 0x800CC1E8+0x413C = 0x800D0324 + md1*0x1F8
     80054DBC  sb   zero,0x4281(v0)         ; 0x800D0469 + md1*0x1F8  (= Pool[md1]+0x145)

4) Dass dieser Pool genau die Modelltabelle des RAUMS ist, steht im Lader FUN_80052D14:
     80052D68  lw   v0,0x213c(s3) ; v0 = *0x800CE324 = RDT-Basiszeiger
     80052D70  lbu  s2,0x2(v0)    ; Schleifenzahl = RDT+0x02 = nOmodel
     80052D74  lw   s4,0x30(v0)   ; s4 = RDT+0x30 = Offset der Modelltabelle
     80052D8C  lw   a0,0x0(s4)    ; erster u32 des Tabelleneintrags
     80052DF4  addiu s4,s4,0x8    ; Schrittweite 8 = zwei u32 (TIM, MD1)
     80052D30  li   s2,0x20 / 80052D54..64: Pool mit 0x1F8 Schritt geloescht (32 Eintraege)
   => md1 = Slot in RDT+0x30, Anzahl = RDT+0x02. Genau das liest rdt_props.py.

5) md1 == 255 (0xFF) faellt durch `sltiu v0,s2,0x20` (@0x80054D98 / @0x80054DC0) -> kein
   Pool-Zugriff. 255 heisst also: kein Weltmodell.

Aufruf:
    re2_doc_worldmodels.py <zielordner>
"""
import os, sys, glob, struct, hashlib, collections, math

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import re2_scd_walk, re2_items, rdt_props, md1_view
from PIL import Image, ImageDraw


# --- CLUT-richtiges Rendern -------------------------------------------------
# ⛔ md1_view.render nimmt IMMER Palettenzeile 0. Die RDT-TIMs der Raeume tragen
# aber 1..4 Paletten (TIM-CLUT-Kopf: cw=256, ch=1..4), und JEDE MD1-Flaeche nennt
# ihre Palette im clut-Feld des UV-Records (u16 bei +2): Bits 0..5 = VRAM-x/16,
# Bits 6..14 = VRAM-y. Palettenzeile = (clut>>6) - clut_y_des_TIM.
# Ohne das sieht z.B. mesh08 nach rohem Fleisch aus statt nach dem, was es ist.
def tim_pages(path_or_bytes):
    d = open(path_or_bytes, "rb").read() if isinstance(path_or_bytes, str) else path_or_bytes
    assert struct.unpack_from("<I", d, 0)[0] == 0x10, "kein TIM"
    flags = struct.unpack_from("<I", d, 4)[0]
    bpp = {0: 4, 1: 8, 2: 16, 3: 24}[flags & 3]
    p, pal, cy, cw, ch = 8, None, 0, 0, 0
    if flags & 8:
        csz, cx, cy, cw, ch = struct.unpack_from("<IHHHH", d, p)
        pal = d[p + 12: p + csz]
        p += csz
    isz, ix, iy, iw, ih = struct.unpack_from("<IHHHH", d, p)
    body = d[p + 12: p + isz]
    w = iw * (16 // bpp) if bpp in (4, 8) else iw
    return dict(bpp=bpp, pal=pal, cy=cy, cw=cw, ch=ch, body=body, w=w, h=ih)


def palette_image(t, row):
    """Ein RGB-Bild der Textur mit Palettenzeile `row`."""
    key = row
    if key in t.setdefault("_cache", {}):
        return t["_cache"][key]
    w, h, body, pal, cw = t["w"], t["h"], t["body"], t["pal"], t["cw"]
    base = max(0, min(t["ch"] - 1, row)) * cw * 2
    im = Image.new("RGB", (w, h))
    px = im.load()

    def col(i):
        o = base + i * 2
        if o + 2 > len(pal):
            return (0, 0, 0)
        v = pal[o] | (pal[o + 1] << 8)
        return ((v & 0x1F) << 3, ((v >> 5) & 0x1F) << 3, ((v >> 10) & 0x1F) << 3)
    lut = [col(i) for i in range(256 if t["bpp"] == 8 else 16)]
    for y in range(h):
        ro = y * (w if t["bpp"] == 8 else w // 2)
        for x in range(w):
            if t["bpp"] == 8:
                px[x, y] = lut[body[ro + x]] if ro + x < len(body) else (0, 0, 0)
            else:
                b = body[ro + x // 2] if ro + x // 2 < len(body) else 0
                px[x, y] = lut[b & 0xF if x % 2 == 0 else b >> 4]
    t["_cache"][key] = im
    return im


def render_clut(md1, timbytes, out, size=320, yaw=0.7, pitch=0.5, bg=(16, 16, 20)):
    h, tris, quads = md1_view.geometry(md1)
    t = tim_pages(timbytes)
    allv = [v for f, _ in tris + quads for v in f]
    if not allv:
        return None
    xs = [v[0] for v in allv]; ys = [v[1] for v in allv]; zs = [v[2] for v in allv]
    cx, cy, cz = (min(xs) + max(xs)) / 2, (min(ys) + max(ys)) / 2, (min(zs) + max(zs)) / 2
    ext = max(max(xs) - min(xs), max(ys) - min(ys), max(zs) - min(zs)) or 1
    sc = size * 0.78 / ext
    cyw, syw, cp, sp = math.cos(yaw), math.sin(yaw), math.cos(pitch), math.sin(pitch)

    def proj(v):
        x, y, z = v[0] - cx, v[1] - cy, v[2] - cz
        x, z = x * cyw + z * syw, -x * syw + z * cyw
        y, z = y * cp - z * sp, y * sp + z * cp
        return (size / 2 + x * sc, size / 2 + y * sc, z)

    img = Image.new("RGB", (size, size), bg)
    zb = [[1e9] * size for _ in range(size)]
    px = img.load()

    def tri(p, uv, tex, tw, th):
        (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = p
        minx, maxx = int(max(0, min(x0, x1, x2))), int(min(size - 1, max(x0, x1, x2)))
        miny, maxy = int(max(0, min(y0, y1, y2))), int(min(size - 1, max(y0, y1, y2)))
        den = (y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2)
        if abs(den) < 1e-9:
            return
        tp = tex.load()
        for yy in range(miny, maxy + 1):
            for xx in range(minx, maxx + 1):
                a = ((y1 - y2) * (xx - x2) + (x2 - x1) * (yy - y2)) / den
                b = ((y2 - y0) * (xx - x2) + (x0 - x2) * (yy - y2)) / den
                c = 1 - a - b
                if a < -0.002 or b < -0.002 or c < -0.002:
                    continue
                z = a * z0 + b * z1 + c * z2
                if z >= zb[yy][xx]:
                    continue
                zb[yy][xx] = z
                u = a * uv[0][0] + b * uv[1][0] + c * uv[2][0]
                v = a * uv[0][1] + b * uv[1][1] + c * uv[2][1]
                px[xx, yy] = tp[min(tw - 1, max(0, int(u))), min(th - 1, max(0, int(v)))]

    for f, u in tris:
        tex = palette_image(t, (u["clut"] >> 6) - t["cy"])
        tri([proj(v) for v in f], u["pts"][:3], tex, t["w"], t["h"])
    for f, u in quads:
        tex = palette_image(t, (u["clut"] >> 6) - t["cy"])
        p = [proj(v) for v in f]
        tri([p[0], p[1], p[2]], [u["pts"][0], u["pts"][1], u["pts"][2]], tex, t["w"], t["h"])
        tri([p[1], p[3], p[2]], [u["pts"][1], u["pts"][3], u["pts"][2]], tex, t["w"], t["h"])
    img.save(out)
    return len(tris), len(quads)

REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
RDTDIR = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")

DOC_ID_MIN = 104          # 0x68, Schranke `sltiu v0,a3,0x68` @0x80071BBC
NO_MODEL = 255


def brute(buf):
    """Unabhaengige Gegenprobe: JEDE Byte-Position als Item_aot_set(0x4E) lesen und
    nur die annehmen, die alle Feldschranken des Handlers erfuellen. Damit ist die
    Dunkelziffer der desynchronen Bloecke beziffert statt behauptet."""
    out = []
    for p in range(0, len(buf) - 22 + 1):
        if buf[p] != 0x4E or buf[p + 2] != 0x02:
            continue                      # op, sce=2 (der Item-/Untersuchen-Handler)
        iid = struct.unpack_from("<H", buf, p + 14)[0]
        n = struct.unpack_from("<H", buf, p + 16)[0]
        md1, act = buf[p + 20], buf[p + 21]
        if not (DOC_ID_MIN <= iid <= 126):
            continue
        if n == 0 or n > 16:
            continue
        if not (md1 < 0x20 or md1 == NO_MODEL):
            continue
        if act > 8:
            continue
        out.append(p)
    return out


def scan():
    """Alle Item-AOTs aller Raeume, mit RDT-Byte-Offset des Records."""
    rows, desync = [], []
    stat = dict(bytes_total=0, bytes_walked=0, blocks=0, brute_hits=0)
    brute_extra = []
    for d in sorted(glob.glob(os.path.join(RDTDIR, "room*"))):
        if not os.path.isdir(d):
            continue
        room = os.path.basename(d)
        rdtp = os.path.join(RDTDIR, "ROOM%s.RDT" % room[4:].upper())
        raw = open(rdtp, "rb").read() if os.path.exists(rdtp) else None
        for f in sorted(glob.glob(os.path.join(d, "scd", "*.scd"))):
            ent = os.path.splitext(os.path.basename(f))[0]
            buf = open(f, "rb").read()
            items, st = re2_scd_walk.item_records(buf)
            recs, _ = re2_scd_walk.walk(buf)
            stat["blocks"] += 1
            stat["bytes_total"] += len(buf)
            stat["bytes_walked"] += (recs[-1][0] + len(recs[-1][2])) if recs else 0
            found = set(it["off"] for it in items)
            for p in brute(buf):
                stat["brute_hits"] += 1
                if p not in found:
                    brute_extra.append((room, ent, p, struct.unpack_from("<H", buf, p + 14)[0]))
            if st != "ok":
                desync.append((room, ent, st))
            for it in items:
                # Byte-Offset des Records im RDT: Block im RDT suchen (Gegenprobe:
                # der Block muss GENAU EINMAL vorkommen, sonst ist der Offset unsicher).
                rdt_off, uniq = None, None
                if raw is not None and buf:
                    first = raw.find(buf)
                    if first >= 0:
                        uniq = (raw.find(buf, first + 1) < 0)
                        rdt_off = first + it["off"]
                it.update(room=room, ent=ent, rdt=rdtp, rdt_off=rdt_off, uniq=uniq,
                          scd_status=st)
                rows.append(it)
    return rows, desync, stat, brute_extra


def main():
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    rows, desync, stat, brute_extra = scan()
    docs = [r for r in rows if r["i_item"] >= DOC_ID_MIN]
    withm = [r for r in docs if r["md1"] != NO_MODEL]
    without = [r for r in docs if r["md1"] == NO_MODEL]

    print("Item-AOT-Records gesamt:            %3d" % len(rows))
    print("davon Dokumente (Id >= %d):        %3d" % (DOC_ID_MIN, len(docs)))
    print("davon MIT Weltmodell (md1 != 255):  %3d" % len(withm))
    print("davon ohne (md1 == 255):            %3d" % len(without))
    print("SCD-Bloecke: %d, davon desynchron %d; Bytes %d gesamt / %d gewalkt (%.2f%%)"
          % (stat["blocks"], len(desync), stat["bytes_total"], stat["bytes_walked"],
             100.0 * stat["bytes_walked"] / max(1, stat["bytes_total"])))
    print("Brute-Force-Gegenprobe (jede Byte-Position): %d Treffer, davon NEU: %d %s"
          % (stat["brute_hits"], len(brute_extra), brute_extra[:8]))
    print()

    # --- Meshes schneiden, ueber md5 DER GEOMETRIE entdoppeln --------------------
    seen = collections.OrderedDict()      # md5(md1-bytes) -> dict
    for r in sorted(withm, key=lambda r: (r["i_item"], r["room"])):
        d, n, tbl, props = rdt_props.parse(r["rdt"])
        if r["md1"] >= n:
            print("!! %s Id %d: md1=%d >= nOmodel=%d" % (r["room"], r["i_item"], r["md1"], n))
            continue
        p = props[r["md1"]]
        md1 = d[p["md1_off"]:p["md1_off"] + p["md1_size"]]
        tim = d[p["tim_off"]:p["tim_off"] + p["tim_size"]]
        h = hashlib.md5(md1).hexdigest()
        r.update(nomodel=n, tbl=tbl, md1_off=p["md1_off"], md1_size=p["md1_size"],
                 tim_off=p["tim_off"], tim_size=p["tim_size"], md5=h)
        if h not in seen:
            seen[h] = dict(md1=md1, tim=tim, uses=[])
        seen[h]["uses"].append(r)

    print("VERSCHIEDENE Meshes (md5 der MD1-Bytes): %d" % len(seen))
    print()

    order = list(seen.items())
    idx = {}
    for i, (h, v) in enumerate(order):
        pre = os.path.join(out, "mesh%02d_%s" % (i, h[:8]))
        open(pre + ".md1", "wb").write(v["md1"])
        open(pre + ".tim", "wb").write(v["tim"])
        try:
            hh, tris, quads = md1_view.geometry(v["md1"])
            with open(pre + ".obj", "w") as f:
                f.write("# RE2-Dokument-Weltmodell, mesh%02d  md5=%s\n" % (i, h))
                nvt = 1
                for fa, _ in tris:
                    for vv in fa:
                        f.write("v %d %d %d\n" % vv)
                    f.write("f %d %d %d\n" % (nvt, nvt + 1, nvt + 2)); nvt += 3
                for fa, _ in quads:
                    for vv in fa:
                        f.write("v %d %d %d\n" % vv)
                    f.write("f %d %d %d %d\n" % (nvt, nvt + 1, nvt + 3, nvt + 2)); nvt += 4
            allv = [vv for fa, _ in tris + quads for vv in fa]
            bb = (min(x[0] for x in allv), max(x[0] for x in allv),
                  min(x[1] for x in allv), max(x[1] for x in allv),
                  min(x[2] for x in allv), max(x[2] for x in allv))
            v.update(tris=len(tris), quads=len(quads), bb=bb, nobj=hh["nobj"])
            for ang, tag in ((0.7, "a"), (2.3, "b")):
                render_clut(v["md1"], v["tim"], pre + "_%s.png" % tag, 320, ang, 0.55)
            print("mesh%02d  %5d B  %3d Tri %3d Quad  bbox=%s" % (i, len(v["md1"]),
                                                                 len(tris), len(quads), bb))
        except Exception as e:
            print("!! mesh%02d: %s" % (i, e))
            v.update(tris=0, quads=0, bb=None, nobj=0)
        idx[h] = i
        v["file"] = os.path.basename(pre)

    # --- Index --------------------------------------------------------------
    with open(os.path.join(out, "_index.txt"), "w") as f:
        f.write("# RE2-Dokument-Weltmodelle\n")
        f.write("# %d Item-AOTs gesamt, %d Dokument-Platzierungen, %d mit Modell, "
                "%d ohne, %d VERSCHIEDENE Meshes\n"
                % (len(rows), len(docs), len(withm), len(without), len(seen)))
        f.write("# SCD: %d Bloecke, %d desynchron; %d Bytes gesamt / %d gewalkt = %.2f%%\n"
                % (stat["blocks"], len(desync), stat["bytes_total"], stat["bytes_walked"],
                   100.0 * stat["bytes_walked"] / max(1, stat["bytes_total"])))
        f.write("# Brute-Force-Gegenprobe ueber JEDE Byte-Position: %d Treffer, davon neu %d\n\n"
                % (stat["brute_hits"], len(brute_extra)))
        f.write("## alle Dokument-Platzierungen\n")
        f.write("%-9s %-7s %-8s %-4s %-22s %6s %6s %5s %4s %4s %5s %s\n" % (
            "Raum", "Block", "RDT-Off", "Id", "Name", "x", "z", "flag", "md1", "act", "mesh", "md5"))
        for r in sorted(docs, key=lambda r: (r["i_item"], r["room"])):
            nm = re2_items.name(r["i_item"])[1] if r["i_item"] < re2_items.N_NAMES else "??"
            mesh = ("mesh%02d" % idx[r["md5"]]) if r.get("md5") in idx else "-"
            f.write("%-9s %-7s 0x%05X %-4d %-22s %6d %6d %5d %4d %4d %5s %s\n" % (
                r["room"], r["ent"], r["rdt_off"] or 0, r["i_item"], nm,
                r["x"], r["z"], r["flag"], r["md1"], r["action"], mesh,
                r.get("md5", "-")[:8]))
        f.write("\n## die verschiedenen Meshes\n")
        for h, v in order:
            i = idx[h]
            f.write("mesh%02d md5=%s %5d B  nObj=%d  %d Tri %d Quad  bbox=%s\n" % (
                i, h, len(v["md1"]), v.get("nobj", 0), v.get("tris", 0), v.get("quads", 0),
                v.get("bb")))
            for r in v["uses"]:
                nm = re2_items.name(r["i_item"])[1] if r["i_item"] < re2_items.N_NAMES else "??"
                f.write("        %s Slot %d (von %d)  Id %d %s  MD1 @0x%06X %d B  TIM @0x%06X %d B\n"
                        % (r["room"], r["md1"], r["nomodel"], r["i_item"], nm,
                           r["md1_off"], r["md1_size"], r["tim_off"], r["tim_size"]))
        f.write("\n## Dokument-Platzierungen OHNE Weltmodell (md1 == 255)\n")
        for r in sorted(without, key=lambda r: (r["i_item"], r["room"])):
            nm = re2_items.name(r["i_item"])[1] if r["i_item"] < re2_items.N_NAMES else "??"
            f.write("%-9s %-7s 0x%05X Id %-4d %-22s x=%-6d z=%-6d flag=%-4d act=%d\n" % (
                r["room"], r["ent"], r["rdt_off"] or 0, r["i_item"], nm,
                r["x"], r["z"], r["flag"], r["action"]))
        f.write("\n## desynchrone SCD-Bloecke (nicht gewalkt, Dunkelziffer)\n")
        for d_ in desync:
            f.write("  %s %s %s\n" % d_)

    # --- Kontaktbogen -------------------------------------------------------
    S, COLS = 320, 4
    rowsn = (len(order) + COLS - 1) // COLS
    img = Image.new("RGB", (S * COLS, (S + 46) * rowsn), (14, 14, 18))
    dr = ImageDraw.Draw(img)
    for j, (h, v) in enumerate(order):
        rr, cc = divmod(j, COLS)
        pre = os.path.join(out, v["file"] + "_a.png")
        if os.path.exists(pre):
            img.paste(Image.open(pre), (cc * S, rr * (S + 46) + 46))
        names = sorted(set(re2_items.name(u["i_item"])[1] for u in v["uses"]))
        dr.text((cc * S + 4, rr * (S + 46) + 3), "mesh%02d  %dB  %dT/%dQ" % (
            j, len(v["md1"]), v.get("tris", 0), v.get("quads", 0)), fill=(250, 250, 250))
        dr.text((cc * S + 4, rr * (S + 46) + 15), (", ".join(names))[:52], fill=(190, 210, 255))
        dr.text((cc * S + 4, rr * (S + 46) + 27),
                (" ".join("%s#%d" % (u["room"], u["md1"]) for u in v["uses"]))[:52],
                fill=(170, 170, 170))
    p = os.path.join(out, "_kontaktbogen.png")
    img.save(p)
    print("Kontaktbogen:", p, img.size)
    print("Index:", os.path.join(out, "_index.txt"))


if __name__ == "__main__":
    main()
