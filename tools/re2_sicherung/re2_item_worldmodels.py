#!/usr/bin/env python3
"""re2_item_worldmodels.py - ALLE RE2-Welt-Item-Modelle (World Items) schneiden, entdoppeln,
rendern und als Katalog ablegen. Runde 35 Spur F, Punkt 6 (AUFTRAG.md Z. 39, woertlich):
  "Ich will das du mir die Karten Modelle in der Welt zum Einsammeln, also die World items, aus
   Resident Evil 2 extrahierst und irgendwo ablegst wo ich es sehen kann"

Verallgemeinerung von re2_doc_worldmodels.py (dort nur Dokumente, Id >= 0x68). DIESELBE
Belegkette (info/re2leon/PSX.EXE, selbst disassembliert; Kopf von re2_doc_worldmodels.py):
  Op 0x4E Item_aot_set, Tabelleneintrag @0x800A7600 -> LAB_80054CD4; Record 22 Byte:
    +14 i_item (`lhu` ...), +16 n_item, +18 flag (`lhu a1,0x12(s0)` @0x80054CF4),
    +20 md1 (`lbu s2,0x14(s0)` @0x80054CF8), +21 action (`lbu s1,0x15(s0)` @0x80054DD4).
  md1 < 32 (`sltiu v0,s2,0x20` @0x80054D98) = Slot der Raum-Modelltabelle RDT+0x30 (Lader
  FUN_80052D14: `lbu s2,0x2(v0)` @0x80052D70 nOmodel, `lw s4,0x30(v0)` @0x80052D74 Tabelle,
  `addiu s4,s4,0x8` @0x80052DF4 Schrittweite 8 = (TIM, MD1)). md1 == 255 = KEIN Weltmodell.
  Das Modell haengt also an der PLATZIERUNG, nicht an der Item-Id -> je Platzierung schneiden,
  ueber md5 der MD1-Bytes entdoppeln.

Ausgabe (<ziel>, Standard extracted_re2_items/):
  modelle/meshNNN_<md5>.{md1,tim,obj,_a.png,_b.png}   je VERSCHIEDENES Mesh, zwei Ansichten
  katalog.csv        je Item-Id: Id, Name, Meshes, Platzierungen (Raum/Block/RDT-Offset)
  platzierungen.csv  jede Item_aot_set-Platzierung mit md1 != 255
  ohne_modell.csv    Platzierungen mit md1 == 255 (kein Weltobjekt)
  kontaktbogen.png   alle Meshes auf einem Bogen; karten.png nur die "Karten" (Name mit Card/Map)
  uebersicht.html    Bildergalerie mit Namen (lokal im Browser oeffnen)
  _bericht.txt       Zahlen + Gegenprobe

Aufruf:
    C:/Python310/python.exe tools/re2_sicherung/re2_item_worldmodels.py [ziel]
"""
import os, sys, csv, hashlib, collections, html

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import re2_doc_worldmodels as W          # scan(), render_clut() — dieselbe Belegkette
import re2_items, rdt_props, md1_view
from PIL import Image, ImageDraw

REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
NO_MODEL = W.NO_MODEL


def item_name(i):
    if 0 <= i < re2_items.N_NAMES:
        try:
            return re2_items.name(i)[1]
        except Exception:
            return "??"
    return "??"


def ist_karte(name):
    n = name.lower()
    return ("card" in n) or ("map" in n)


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "extracted_re2_items")
    mdir = os.path.join(out, "modelle")
    os.makedirs(mdir, exist_ok=True)
    rows, desync, stat, _brute_extra = W.scan()
    import struct
    for r in rows:
        if "x" not in r:          # Op 0x69 (4-Punkt-Zone, 30 Byte): erster Punkt als Lage
            r["x"] = struct.unpack_from("<h", r["raw"], 6)[0]
            r["z"] = struct.unpack_from("<h", r["raw"], 8)[0]
    withm = [r for r in rows if r["md1"] != NO_MODEL]
    without = [r for r in rows if r["md1"] == NO_MODEL]

    seen = collections.OrderedDict()
    fehler = []
    for r in sorted(withm, key=lambda r: (r["i_item"], r["room"], r["ent"], r["off"])):
        d, n, tbl, props = rdt_props.parse(r["rdt"])
        if r["md1"] >= n:
            fehler.append("%s %s Id %d: md1=%d >= nOmodel=%d" % (r["room"], r["ent"], r["i_item"], r["md1"], n))
            continue
        p = props[r["md1"]]
        md1 = d[p["md1_off"]:p["md1_off"] + p["md1_size"]]
        tim = d[p["tim_off"]:p["tim_off"] + p["tim_size"]]
        h = hashlib.md5(md1).hexdigest()
        r.update(nomodel=n, md1_off=p["md1_off"], md1_size=p["md1_size"],
                 tim_off=p["tim_off"], tim_size=p["tim_size"], md5=h)
        if h not in seen:
            seen[h] = dict(md1=md1, tim=tim, uses=[])
        seen[h]["uses"].append(r)

    order = list(seen.items())
    idx = {}
    for i, (h, v) in enumerate(order):
        pre = os.path.join(mdir, "mesh%03d_%s" % (i, h[:8]))
        open(pre + ".md1", "wb").write(v["md1"])
        open(pre + ".tim", "wb").write(v["tim"])
        try:
            hh, tris, quads = md1_view.geometry(v["md1"])
            with open(pre + ".obj", "w") as f:
                f.write("# RE2-Welt-Item-Modell mesh%03d md5=%s (%s)\n"
                        % (i, h, ", ".join(sorted(set(item_name(u["i_item"]) for u in v["uses"])))))
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
            bb = (min(x[0] for x in allv), max(x[0] for x in allv), min(x[1] for x in allv),
                  max(x[1] for x in allv), min(x[2] for x in allv), max(x[2] for x in allv))
            v.update(tris=len(tris), quads=len(quads), bb=bb)
            for ang, tag in ((0.7, "a"), (2.3, "b")):
                W.render_clut(v["md1"], v["tim"], pre + "_%s.png" % tag, 320, ang, 0.55)
        except Exception as e:                       # noqa: BLE001 — im Bericht gezaehlt
            fehler.append("mesh%03d: %s" % (i, e))
            v.update(tris=0, quads=0, bb=None)
        idx[h] = i
        v["file"] = os.path.basename(pre)
        v["names"] = sorted(set(item_name(u["i_item"]) for u in v["uses"]))
        v["ids"] = sorted(set(u["i_item"] for u in v["uses"]))

    # --- Tabellen ----------------------------------------------------------------
    with open(os.path.join(out, "platzierungen.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, delimiter=";")
        w.writerow(["raum", "block", "rdt_offset", "item_id", "item_hex", "name", "menge", "x", "z",
                    "flag", "md1_slot", "action", "mesh", "md1_rdt_offset", "tim_rdt_offset", "md5"])
        for r in sorted(withm, key=lambda r: (r["i_item"], r["room"])):
            if "md5" not in r:
                continue
            w.writerow([r["room"], r["ent"], "0x%05X" % (r["rdt_off"] or 0), r["i_item"],
                        "0x%02X" % r["i_item"], item_name(r["i_item"]), r["n_item"], r["x"], r["z"],
                        r["flag"], r["md1"], r["action"], "mesh%03d" % idx[r["md5"]],
                        "0x%06X" % r["md1_off"], "0x%06X" % r["tim_off"], r["md5"][:8]])
    with open(os.path.join(out, "ohne_modell.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, delimiter=";")
        w.writerow(["raum", "block", "rdt_offset", "item_id", "item_hex", "name", "menge", "x", "z", "flag"])
        for r in sorted(without, key=lambda r: (r["i_item"], r["room"])):
            w.writerow([r["room"], r["ent"], "0x%05X" % (r["rdt_off"] or 0), r["i_item"],
                        "0x%02X" % r["i_item"], item_name(r["i_item"]), r["n_item"], r["x"], r["z"], r["flag"]])
    per_id = collections.OrderedDict()
    for r in sorted(withm, key=lambda r: r["i_item"]):
        if "md5" in r:
            per_id.setdefault(r["i_item"], []).append(r)
    with open(os.path.join(out, "katalog.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, delimiter=";")
        w.writerow(["item_id", "item_hex", "name", "karte", "meshes", "dateien", "platzierungen"])
        for iid, rs in per_id.items():
            ms = sorted(set(idx[r["md5"]] for r in rs))
            w.writerow([iid, "0x%02X" % iid, item_name(iid), "ja" if ist_karte(item_name(iid)) else "",
                        " ".join("mesh%03d" % m for m in ms),
                        " ".join("modelle/%s_a.png" % order[m][1]["file"] for m in ms),
                        " ".join("%s/%s@0x%05X" % (r["room"], r["ent"], r["rdt_off"] or 0) for r in rs)])

    # --- Bilder ------------------------------------------------------------------
    def bogen(auswahl, pfad):
        S, COLS = 240, 6
        if not auswahl:
            return None
        rowsn = (len(auswahl) + COLS - 1) // COLS
        img = Image.new("RGB", (S * COLS, (S + 40) * rowsn), (14, 14, 18))
        dr = ImageDraw.Draw(img)
        for j, (h, v) in enumerate(auswahl):
            rr, cc = divmod(j, COLS)
            p = os.path.join(mdir, v["file"] + "_a.png")
            if os.path.exists(p):
                img.paste(Image.open(p).resize((S, S)), (cc * S, rr * (S + 40) + 40))
            dr.text((cc * S + 4, rr * (S + 40) + 3), "mesh%03d  %dT/%dQ" % (idx[h], v.get("tris", 0),
                    v.get("quads", 0)), fill=(250, 250, 250))
            dr.text((cc * S + 4, rr * (S + 40) + 16), (", ".join(v["names"]))[:38], fill=(190, 210, 255))
        img.save(pfad)
        return img.size

    gr_all = bogen(order, os.path.join(out, "kontaktbogen.png"))
    karten = [(h, v) for h, v in order if any(ist_karte(n) for n in v["names"])]
    gr_k = bogen(karten, os.path.join(out, "karten.png"))

    with open(os.path.join(out, "uebersicht.html"), "w", encoding="utf-8") as f:
        f.write("<!doctype html><meta charset='utf-8'><title>RE2 World Items</title>\n"
                "<style>body{background:#111;color:#ddd;font:14px sans-serif;margin:16px}"
                ".g{display:flex;flex-wrap:wrap;gap:10px}.k{background:#1c1c22;padding:6px;width:330px}"
                "img{width:160px;height:160px}h2{margin-top:28px}</style>\n")
        f.write("<h1>Resident Evil 2 (Leon) &mdash; Welt-Item-Modelle</h1>\n<p>%d Item_aot_set-Platzierungen, "
                "%d mit Weltmodell, %d ohne, %d verschiedene Meshes. Quelle info/re2leon/PL0/RDT, "
                "Werkzeug tools/re2_sicherung/re2_item_worldmodels.py.</p>\n"
                % (len(rows), len(withm), len(without), len(order)))
        for titel, auswahl in (("Karten (Name mit Card / Map)", karten), ("Alle Meshes", order)):
            f.write("<h2>%s (%d)</h2><div class='g'>\n" % (html.escape(titel), len(auswahl)))
            for h, v in auswahl:
                f.write("<div class='k'><img src='modelle/%s_a.png'><img src='modelle/%s_b.png'><br>"
                        "<b>mesh%03d</b> %s<br>%s<br><small>%s</small></div>\n" % (
                            v["file"], v["file"], idx[h], html.escape(", ".join(v["names"])),
                            " ".join("0x%02X" % i for i in v["ids"]),
                            html.escape(" ".join("%s/%s" % (u["room"], u["ent"]) for u in v["uses"][:6]))))
            f.write("</div>\n")

    bericht = [
        "# RE2-Welt-Item-Modelle (Runde 35 Spur F, Punkt 6)",
        "Item_aot_set-Platzierungen gesamt: %d" % len(rows),
        "  mit Weltmodell (md1 != 255):     %d" % len(withm),
        "  ohne (md1 == 255):               %d" % len(without),
        "verschiedene Meshes (md5 MD1):     %d" % len(order),
        "  davon Karten (Name Card/Map):    %d  (%s)" % (len(karten),
            "; ".join("mesh%03d %s" % (idx[h], "/".join(v["names"])) for h, v in karten)),
        "Item-Ids mit Weltmodell:           %d" % len(per_id),
        "SCD-Bloecke %d, desynchron %d; Bytes %d / gewalkt %d (%.2f%%)" % (
            stat["blocks"], len(desync), stat["bytes_total"], stat["bytes_walked"],
            100.0 * stat["bytes_walked"] / max(1, stat["bytes_total"])),
        "Kontaktbogen %s, Karten-Bogen %s" % (gr_all, gr_k),
        "Fehler: %d" % len(fehler),
    ] + ["  " + e for e in fehler]
    open(os.path.join(out, "_bericht.txt"), "w", encoding="utf-8").write("\n".join(bericht) + "\n")
    print("\n".join(bericht))


if __name__ == "__main__":
    main()
