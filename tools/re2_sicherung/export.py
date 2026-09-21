#!/usr/bin/env python3
"""export.py - baut extracted_re2_sicherung/ vollstaendig und wiederholbar auf.

Alles hier stammt aus den Werkzeugen daneben; jede Zahl ist in
analysis/befunde_2026-09-21/re2-sicherung-item.md mit Adresse belegt.

Aufruf: export.py <zielordner>
"""
import sys, os, json, struct, shutil

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, HERE)
import re2_items as R
import itemall_extract as IX
import rdt_props, md1_view, re15_items, re2_scd_walk

OUT = sys.argv[1]
ROOM60D0 = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM60D0.RDT")

# Item-Id -> (Ordnername, Prop-Slot in ROOM60D0, Quelle der Platzierung)
TARGETS = {
    76: ("item_076_main_fuse", 3),
    77: ("item_077_fuse_case", 1),
}


def mk(*p):
    d = os.path.join(OUT, *p)
    os.makedirs(d, exist_ok=True)
    return d


def wr(path, data):
    mode = "wb" if isinstance(data, (bytes, bytearray)) else "w"
    with open(path, mode, **({} if mode == "wb" else {"encoding": "utf-8"})) as f:
        f.write(data)


# ---------------------------------------------------------------- Tabellen
tab = mk("tabellen")
items = list(R.all_items())
rows = []
for f in items:
    rows.append({
        "id": f["id"], "id_hex": "0x%02X" % f["id"],
        "name_en": f["name_en"], "name_addr": "0x%08X" % f["name_addr"],
        "name_bytes_hex": f["name_raw"].hex(),
        "prop_addr": "0x%08X" % f["addr"], "prop_raw_hex": f["raw"].hex(),
        "max": f["max"], "variant": f["variant"], "cls": f["cls"],
        "n_mix": f["n_mix"], "mix_ptr": "0x%08X" % f["mix_ptr"],
        "mix": [{"partner": m["partner"], "result": m["result"],
                 "addr": "0x%08X" % m["addr"], "raw_hex": m["raw"].hex()} for m in f["mix"]],
    })
wr(os.path.join(tab, "re2_item_properties.json"), json.dumps(rows, indent=1, ensure_ascii=False))

lines = ["# RE2-Leon (SLUS-00748) — Namenstabelle, Index 0..139",
         "# Offsets: u16[140] @0x8009EBAC, relativ zu 0x8009E550 (englisch)",
         "#          u16[140] @0x8009E438, relativ zu 0x8009DF3C (japanisch)",
         "#",
         "idx\tid_hex\tadresse_en\tname_en\tbytes_en_hex\tadresse_jp\tbytes_jp_hex"]
for i in range(R.N_NAMES):
    ae, en, rawe = R.name(i)
    aj, _, rawj = R.name(i, R.NAME_OFFTBL_JP, R.NAME_BASE_JP)
    lines.append("%d\t0x%02X\t0x%08X\t%s\t%s\t0x%08X\t%s"
                 % (i, i, ae, en, rawe.hex(), aj, rawj.hex()))
wr(os.path.join(tab, "re2_item_names_140.tsv"), "\n".join(lines) + "\n")

# Alle 106 Symbole als PNG + der CLUT-Block
ico = mk("tabellen", "alle_106_symbole")
pal = IX.clut()
for i in range(IX.N_TILES):
    nm = R.name(i)[1].strip().replace("/", "-") if i < R.N_NAMES else ""
    IX.tile_image(i, pal).save(os.path.join(ico, "icon_%03d_%s.png" % (i, nm or "unbenannt")))
wr(os.path.join(tab, "ITEMALL_clut_256_bgr555.bin"), IX.clut_raw())
IX.sheet(os.path.join(tab, "re2_alle_106_symbole.png"))

# RE1.5-Vergleich
v = mk("re15_vergleich")
nl = ["# RE1.5 — Item-Namen aus info/Re1.5/PSX/BIN/DEBUG.BIN",
      "# Block ab Datei-0x4A28, Eintraege '0x07 <Text>'.",
      "# id = Platz + 1 (zweifach verankert: id 0x15 = 'H. Gun Bullets' und",
      "# id 0x31 = 'Fire Extinguisher', beide in re15_port/engine/src/scd_vm.c:3829-3831",
      "# als gemessene RDT-Werte zitiert).",
      "#", "id_hex\tid\tdatei_offset\tname"]
for i, off, t in re15_items.names():
    nl.append("0x%02X\t%d\t0x%04X\t%s" % (i, i, off, t))
wr(os.path.join(v, "re15_item_names.tsv"), "\n".join(nl) + "\n")

# ---------------------------------------------------------------- die zwei Gegenstaende
d60, n60, tbl60, props60 = rdt_props.parse(ROOM60D0)
place = {}
scd = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room60D0", "scd", "sub02.scd")
buf = open(scd, "rb").read()
for rec in re2_scd_walk.item_records(buf)[0]:
    place[rec["i_item"]] = rec

for iid, (folder, slot) in TARGETS.items():
    dd = mk(folder)
    a, en, rawe = R.name(iid)
    aj, _, rawj = R.name(iid, R.NAME_OFFTBL_JP, R.NAME_BASE_JP)
    pf = R.prop_fields(iid)
    rec = place.get(iid)
    p = props60[slot]

    # Symbol
    IX.tile_image(iid, pal).save(os.path.join(dd, "symbol_%d.png" % iid))
    IX.tile_image(iid, pal).resize((IX.W * 8, IX.H * 8)).save(
        os.path.join(dd, "symbol_%d_x8.png" % iid))
    wr(os.path.join(dd, "symbol_%d_roh_40x30_8bpp.bin" % iid), IX.tile_bytes(iid))
    wr(os.path.join(dd, "symbol_clut_256_bgr555.bin"), IX.clut_raw())

    # Weltmodell
    md1 = d60[p["md1_off"]:p["md1_off"] + p["md1_size"]]
    tim = d60[p["tim_off"]:p["tim_off"] + p["tim_size"]]
    wr(os.path.join(dd, "weltmodell.md1"), md1)
    wr(os.path.join(dd, "weltmodell.tim"), tim)
    h, tris, quads = md1_view.geometry(md1)
    with open(os.path.join(dd, "weltmodell.obj"), "w") as f:
        f.write("# ROOM60D0.RDT Prop-Slot %d -> %s\n" % (slot, en))
        k = 1
        for fa, _ in tris:
            for vv in fa: f.write("v %d %d %d\n" % vv)
            f.write("f %d %d %d\n" % (k, k + 1, k + 2)); k += 3
        for fa, _ in quads:
            for vv in fa: f.write("v %d %d %d\n" % vv)
            f.write("f %d %d %d %d\n" % (k, k + 1, k + 3, k + 2)); k += 4
    md1_view.render(md1, os.path.join(dd, "weltmodell.tim"),
                    os.path.join(dd, "weltmodell_ansicht.png"), 384, 0.7, 0.45)

    allv = [vv for fa, _ in tris + quads for vv in fa]
    meta = {
        "spiel": "Resident Evil 2 (Retail), Leon-Disc SLUS-00748",
        "item_id": iid, "item_id_hex": "0x%02X" % iid,
        "name_en": en,
        "name_en_adresse": "0x%08X" % a, "name_en_bytes_hex": rawe.hex(),
        "name_jp_adresse": "0x%08X" % aj, "name_jp_bytes_hex": rawj.hex(),
        "eigenschaften": {
            "adresse": "0x%08X" % pf["addr"], "roh_hex": pf["raw"].hex(),
            "max_stapel": pf["max"], "varianten_nibble": pf["variant"],
            "klassenbyte": pf["cls"], "anzahl_rezepte": pf["n_mix"],
            "rezeptzeiger": "0x%08X" % pf["mix_ptr"],
        },
        "symbol": {"quelle": "COMMON/DATA/ITEMALL.PIX", "kachel": iid,
                   "datei_offset": "0x%X" % (iid * IX.TILE),
                   "format": "40x30, 8 bpp, CLUT 256 x u16 BGR555 (VRAM y=496)"},
        "platzierung": None,
        "weltmodell": {
            "quelle": "PL0/RDT/ROOM60D0.RDT", "prop_slot": slot,
            "md1_offset": "0x%06X" % p["md1_off"], "md1_bytes": p["md1_size"],
            "tim_offset": "0x%06X" % p["tim_off"], "tim_bytes": p["tim_size"],
            "md1_header": {"length": h["length"], "unknown": h["unknown"],
                           "object_count": h["nobj"], "meshes": len(h["meshes"])},
            "tris": len(tris), "quads": len(quads),
            "bbox": {"x": [min(x[0] for x in allv), max(x[0] for x in allv)],
                     "y": [min(x[1] for x in allv), max(x[1] for x in allv)],
                     "z": [min(x[2] for x in allv), max(x[2] for x in allv)]},
        },
    }
    if rec:
        meta["platzierung"] = {
            "raum": "ROOM60D0", "block": "sub02",
            "offset_im_block": "0x%04X" % rec["off"],
            "offset_in_der_RDT": "0x%06X" % (0x0008E6 if iid == 76 else 0x0008FC),
            "opcode": "0x%02X" % rec["op"],
            "opcode_name": "Item_aot_set" if rec["op"] == 0x4E else "Item_aot_set_4p",
            "roh_hex": rec["raw"].hex(),
            "aot": rec["aot"], "sce": rec["sce"], "sat": rec["sat"],
            "anzahl": rec["n_item"], "flag": rec["flag"],
            "md1_slot": rec["md1"], "action": rec["action"],
        }
        if rec["op"] == 0x4E:
            meta["platzierung"].update(x=rec["x"], z=rec["z"], w=rec["w"], d=rec["d"])
    wr(os.path.join(dd, "beleg.json"), json.dumps(meta, indent=1, ensure_ascii=False))
    print("fertig:", folder)

# Die drei Skriptbloecke, die die beiden Gegenstaende steuern
sc = mk("room60D0_skript")
for b in ("sub02", "sub05", "main00"):
    src = os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room60D0", "scd", b + ".scd")
    shutil.copyfile(src, os.path.join(sc, b + ".scd"))
    bb = open(src, "rb").read()
    recs, st = re2_scd_walk.walk(bb)
    txt = ["# %s.scd — %d Byte, %d Records, Walk: %s" % (b, len(bb), len(recs), st)]
    for off, op, r in recs:
        txt.append("+0x%04X  0x%02X %-18s %s" % (
            off, op, re2_scd_walk.NAMES.get(op, "?"), " ".join("%02x" % x for x in r)))
    wr(os.path.join(sc, b + ".txt"), "\n".join(txt) + "\n")

print("Ziel:", OUT)
