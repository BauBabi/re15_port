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
  platzierungen.csv  jede Item_aot_set-Platzierung mit md1 < 0x20
  ohne_modell.csv    Platzierungen mit md1 >= 0x20 (kein Weltobjekt)
  lageplaene.csv     die 7 Lageplan-Aufnahmen (Raum-Objekt, Platzierung, Ja-Zweig) — Nachbesserung 1
  kontaktbogen.png   alle Meshes auf einem Bogen; karten.png nur die "Karten" (Name mit Card/Map)
  uebersicht.html    Bildergalerie mit Namen (lokal im Browser oeffnen)
  _bericht.txt       Zahlen + Gegenprobe

Aufruf:
    C:/Python310/python.exe tools/re2_sicherung/re2_item_worldmodels.py [ziel]
"""
import os, sys, csv, hashlib, collections, html, glob, re, struct

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import re2_doc_worldmodels as W          # render_clut() — dieselbe Belegkette
import re2_items, rdt_props, md1_view, re2_scd_walk, msg_decode
from PIL import Image, ImageDraw

REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
NO_MODEL = W.NO_MODEL
RDTDIR = W.RDTDIR


# --- Nachbesserung 1 (Runde 35 Spur F, Mangel M1) ------------------------------------------------
# 1) Bloecke DIREKT aus dem RDT (re2_scd_walk.rdt_blocks, RDT+0x48/+0x4C @0x800535d4/@0x800535f4):
#    die Dateien room*/scd/*.scd sind an 268 von 2568 Bloecken abgeschnitten (room20B0 sub00:
#    14 statt 310 Byte) -> Item_aot_set und Lageplan-Objekte fehlten.
# 2) Lageplaene ("map") sind in RE2 KEIN Item_aot_set, sondern ein Raum-Objekt (Obj_model_set,
#    Objekt-Index = Modell-Slot: @0x80055290 lbu t1,1(s2) / @0x80055424 lw v1,48(a0) /
#    @0x80055430 lw a1,4(v0)) plus Ereignisblock: Message_on(Frage) -> Ck(0x0B,0x1F,0) (Ja) ->
#    Work_set(4,n) -> Pos_set/Member_set (Objekt weg) -> Set(Merkbit) -> Message_on(Bestaetigung).
def scan_rdt():
    """Alle Item-AOTs aller RDTs, Bloecke aus den RDT-Tabellen, RDT-Byte-Offset exakt."""
    rows, desync = [], []
    stat = dict(bytes_total=0, bytes_walked=0, blocks=0, cut_blocks=0, cut_bytes=0)
    for rdtp in sorted(glob.glob(os.path.join(RDTDIR, "ROOM*.RDT"))):
        room = "room" + os.path.basename(rdtp)[4:8]
        raw = open(rdtp, "rb").read()
        bl = re2_scd_walk.rdt_blocks(raw)
        for k, (ent, s, e) in enumerate(bl):
            buf = raw[s:e]
            recs, st = re2_scd_walk.walk(buf)
            if st != "ok":
                # Der LETZTE Block der Tabelle hat kein Folge-Offset: sein Ende ist das letzte
                # Evt_end (0x01) vor dem Desync, dahinter liegen andere RDT-Daten (z.B. ROOM1020
                # sub19: Evt_end @+0x60, danach ein MD1-Kopf). Bei inneren Bloecken ist ein Rest
                # <= 3 Byte hinter dem letzten Evt_end Ausrichtung. Sonst: echter Desync.
                last = max((i for i, rr in enumerate(recs) if rr[1] == 0x01), default=None)
                if last is not None:
                    end = recs[last][0] + len(recs[last][2])
                    if k == len(bl) - 1 or len(buf) - end <= 3:
                        stat["cut_blocks"] += 1
                        stat["cut_bytes"] += len(buf) - end
                        buf = buf[:end]
                        recs, st = re2_scd_walk.walk(buf)
            items, st = re2_scd_walk.item_records(buf)
            stat["blocks"] += 1
            stat["bytes_total"] += len(buf)
            walked = (recs[-1][0] + len(recs[-1][2])) if recs else 0
            stat["bytes_walked"] += walked
            if st != "ok":
                desync.append((room, ent, st))
            for it in items:
                it.update(room=room, ent=ent, rdt=rdtp, rdt_off=s + it["off"], uniq=True,
                          scd_status=st)
                rows.append(it)
    return rows, desync, stat


def md1_trim(m):
    """MD1 auf seine eigene Laenge kuerzen. rdt_props schneidet bis zum naechsten bekannten
    Offset — beim letzten MD1 vor den TIMs sind das z.B. 14188 statt 236 Byte (room5060 Slot 2),
    die md5 waere dann falsch. Ende = groesstes Blockende laut Mesh-Kopf (Format re15_md1.h /
    md1_view.parse: Kopf 12 + n/2*56, Vertex/Normal 8 B, Tri/TriUV 12 B, Quad/QuadUV 16 B, Offsets
    ab +12). Gegenprobe: gleich der Groesse der obj/modelNN.md1 in 596 von 597 Faellen."""
    h = md1_view.parse(m)
    e = 12 + (h["nobj"] // 2) * 56
    for me in h["meshes"]:
        for part, st in (("t", 12), ("q", 16)):
            x = me[part]
            e = max(e, x["vtx_off"] + 8 * x["vtx_cnt"], x["nrm_off"] + 8 * x["nrm_cnt"],
                    x["face_off"] + st * x["face_cnt"], x["uv_off"] + st * x["face_cnt"])
    return m[:e] if 0 < e <= len(m) else m


def _msg(room, mid):
    p = os.path.join(RDTDIR, room, "msg", "sub%02d.msg" % mid)
    if not os.path.exists(p):
        return ""
    t = msg_decode.text(p)
    t = re.sub(r"\s+", " ", re.sub(r"<[A-Z0-9]+>|\{[0-9A-F]+\}", " ", t))
    t = re.sub(r"\s\.(?=\S)", " ", t)          # <F9>-Hervorhebung: " .police" -> " police"
    return re.sub(r"\s+", " ", re.sub(r"\s\.(?=\s|$)", "", t)).strip(" ,")


def lageplaene():
    """Lageplan-Aufnahmen aller RDTs: Frage-Nachricht mit 'map', Ja-Zweig, Objekt, Platzierung."""
    out = []
    for rdtp in sorted(glob.glob(os.path.join(RDTDIR, "ROOM*.RDT"))):
        room = "room" + os.path.basename(rdtp)[4:8]
        raw = open(rdtp, "rb").read()
        blocks = []
        for ent, s, e in re2_scd_walk.rdt_blocks(raw):
            recs, _ = re2_scd_walk.walk(raw[s:e])
            blocks.append((ent, s, recs))
        for ent, s, recs in blocks:
            for i, (off, op, r) in enumerate(recs):
                if op != 0x2B:
                    continue
                frage = _msg(room, r[2])
                if not re.search(r"\bmap\b", frage, re.I) or not re.search(r"take|file", frage, re.I):
                    continue
                ja = None
                for j in range(i + 1, min(len(recs), i + 8)):
                    o2, op2, r2 = recs[j]
                    if op2 == 0x21 and r2[1] == 0x0B and r2[2] == 0x1F and r2[3] == 0:
                        ja = j
                        break
                if ja is None:
                    continue
                obj = weg = None
                merk, best = [], None
                for j in range(ja + 1, len(recs)):
                    o2, op2, r2 = recs[j]
                    if op2 == 0x2B:
                        best = (s + o2, r2[2], _msg(room, r2[2]))
                        break
                    if op2 == 0x2E and r2[1] == 4 and obj is None:
                        obj = (r2[2], s + o2)
                    elif op2 in (0x32, 0x34) and obj is not None and weg is None:
                        weg = (s + o2, r2.hex(" "))
                    elif op2 == 0x22 and r2[3] == 1:
                        merk.append((r2[1], r2[2], s + o2))
                plaetze = []
                if obj is not None:
                    for ent2, s2, recs2 in blocks:
                        for o3, op3, r3 in recs2:
                            if op3 == 0x2D and r3[1] == obj[0]:
                                x, y, z = struct.unpack_from("<hhh", r3, 14)
                                plaetze.append((ent2, s2 + o3, x, y, z))
                out.append(dict(room=room, rdt=rdtp, ent=ent, frage_off=s + off, frage_id=r[2],
                                frage=frage, obj=obj, weg=weg, merk=merk, best=best,
                                plaetze=plaetze, name=_planname(frage)))
    return out


def _planname(frage):
    """Planname aus der Frage: letzter Ausdruck 'a/the <Woerter> map' (room6120: 'laboratory map')."""
    m = re.findall(r"\b(?:a|the)\s+([A-Za-z0-9 ]+?\bmap)\b", frage, re.I)
    return m[-1].strip() if m else "map"


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


def schneiden(v, pre, kopf, fehler, label):
    """MD1/TIM/OBJ + zwei CLUT-richtige Ansichten (re2_doc_worldmodels.render_clut)."""
    open(pre + ".md1", "wb").write(v["md1"])
    open(pre + ".tim", "wb").write(v["tim"])
    try:
        hh, tris, quads = md1_view.geometry(v["md1"])
        with open(pre + ".obj", "w") as f:
            f.write("# %s\n" % kopf)
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
        fehler.append("%s: %s" % (label, e))
        v.update(tris=0, quads=0, bb=None)
    v["file"] = os.path.basename(pre)


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "extracted_re2_items")
    mdir = os.path.join(out, "modelle")
    os.makedirs(mdir, exist_ok=True)
    rows, desync, stat = scan_rdt()
    for r in rows:
        if "x" not in r:          # Op 0x69 (4-Punkt-Zone, 30 Byte): erster Punkt als Lage
            r["x"] = struct.unpack_from("<h", r["raw"], 6)[0]
            r["z"] = struct.unpack_from("<h", r["raw"], 8)[0]
    # Weltmodell nur bei md1 < 0x20 (`sltiu v0,s2,0x20` @0x80054D98); 255 und jeder andere Wert
    # >= 0x20 (room2130 sub00: 0xE1) faellt durch = kein Weltmodell (Nachbesserung 1).
    withm = [r for r in rows if r["md1"] < 0x20]
    without = [r for r in rows if r["md1"] >= 0x20]

    seen = collections.OrderedDict()
    fehler = []
    for r in sorted(withm, key=lambda r: (r["i_item"], r["room"], r["ent"], r["off"])):
        d, n, tbl, props = rdt_props.parse(r["rdt"])
        if r["md1"] >= n:
            fehler.append("%s %s Id %d: md1=%d >= nOmodel=%d" % (r["room"], r["ent"], r["i_item"], r["md1"], n))
            continue
        p = props[r["md1"]]
        md1 = md1_trim(d[p["md1_off"]:p["md1_off"] + p["md1_size"]])
        tim = d[p["tim_off"]:p["tim_off"] + p["tim_size"]]
        h = hashlib.md5(md1).hexdigest()
        r.update(nomodel=n, md1_off=p["md1_off"], md1_size=len(md1),
                 tim_off=p["tim_off"], tim_size=p["tim_size"], md5=h)
        if h not in seen:
            seen[h] = dict(md1=md1, tim=tim, uses=[])
        seen[h]["uses"].append(r)

    order = list(seen.items())
    idx = {}
    for i, (h, v) in enumerate(order):
        pre = os.path.join(mdir, "mesh%03d_%s" % (i, h[:8]))
        names = sorted(set(item_name(u["i_item"]) for u in v["uses"]))
        schneiden(v, pre, "RE2-Welt-Item-Modell mesh%03d md5=%s (%s)" % (i, h, ", ".join(names)),
                  fehler, "mesh%03d" % i)
        idx[h] = i
        v["label"] = "mesh%03d" % i
        v["names"] = names
        v["ids"] = sorted(set(u["i_item"] for u in v["uses"]))

    # --- Lageplaene (Nachbesserung 1, M1): Raum-Objekt des Aufnahme-Ereignisses -------------------
    plaene = lageplaene()
    pseen = collections.OrderedDict()
    for pl in plaene:
        if pl["obj"] is None:
            continue
        d, n, tbl, props = rdt_props.parse(pl["rdt"])
        slot = pl["obj"][0]
        if slot >= n:
            fehler.append("%s Lageplan obj %d >= nOmodel %d" % (pl["room"], slot, n))
            continue
        pr = props[slot]
        md1 = md1_trim(d[pr["md1_off"]:pr["md1_off"] + pr["md1_size"]])
        tim = d[pr["tim_off"]:pr["tim_off"] + pr["tim_size"]]
        h = hashlib.md5(md1).hexdigest()
        pl.update(md5=h, md1_off=pr["md1_off"], md1_size=len(md1), tim_off=pr["tim_off"],
                  tim_size=pr["tim_size"], nomodel=n)
        pseen.setdefault(h, dict(md1=md1, tim=tim, uses=[]))["uses"].append(pl)
    plan_order = list(pseen.items())
    for k, (h, v) in enumerate(plan_order):
        pre = os.path.join(mdir, "karte%02d_%s" % (k, h[:8]))
        names = sorted(set(u["name"] for u in v["uses"]))
        schneiden(v, pre, "RE2-Lageplan-Weltmodell karte%02d md5=%s (%s)" % (k, h, ", ".join(names)),
                  fehler, "karte%02d" % k)
        v["label"] = "karte%02d" % k
        v["names"] = [nm + " (Lageplan)" for nm in names]
        v["ids"] = []
        v["in_items"] = h in seen          # gleiches Mesh auch als Item?

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
        # Lageplaene: kein Item (keine Id) — Raum-Objekt des Aufnahme-Ereignisses (M1)
        for h, v in plan_order:
            w.writerow(["", "", " / ".join(v["names"]), "ja", v["label"], "modelle/%s_a.png" % v["file"],
                        " ".join("%s/obj%d@%s Aufnahme %s@0x%05X" % (
                            u["room"], u["obj"][0],
                            "+".join("%s@0x%05X" % (pp[0], pp[1]) for pp in u["plaetze"]),
                            u["ent"], u["frage_off"]) for u in v["uses"])])
    with open(os.path.join(out, "lageplaene.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f, delimiter=";")
        w.writerow(["raum", "plan", "aufnahme_block", "frage_rdt_offset", "frage", "objekt",
                    "work_set_offset", "weg_offset", "weg_record", "merkbits", "obj_model_set",
                    "lage_xyz", "mesh", "md1_rdt_offset", "tim_rdt_offset", "md5"])
        for pl in plaene:
            w.writerow([pl["room"], pl["name"], pl["ent"], "0x%05X" % pl["frage_off"], pl["frage"],
                        "" if pl["obj"] is None else pl["obj"][0],
                        "" if pl["obj"] is None else "0x%05X" % pl["obj"][1],
                        "" if pl["weg"] is None else "0x%05X" % pl["weg"][0],
                        "" if pl["weg"] is None else pl["weg"][1],
                        " ".join("(%d,0x%02X)@0x%05X" % m for m in pl["merk"]),
                        " ".join("%s@0x%05X" % (pp[0], pp[1]) for pp in pl["plaetze"]),
                        " ".join("(%d,%d,%d)" % pp[2:] for pp in pl["plaetze"]),
                        pseen[pl["md5"]]["label"] if "md5" in pl else "KEIN Weltobjekt",
                        "0x%06X" % pl["md1_off"] if "md5" in pl else "",
                        "0x%06X" % pl["tim_off"] if "md5" in pl else "",
                        pl.get("md5", "")[:8]])

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
            dr.text((cc * S + 4, rr * (S + 40) + 3), "%s  %dT/%dQ" % (v["label"], v.get("tris", 0),
                    v.get("quads", 0)), fill=(250, 250, 250))
            dr.text((cc * S + 4, rr * (S + 40) + 16), (", ".join(v["names"]))[:38], fill=(190, 210, 255))
        img.save(pfad)
        return img.size

    gr_all = bogen(order, os.path.join(out, "kontaktbogen.png"))
    karten = [(h, v) for h, v in order if any(ist_karte(n) for n in v["names"])] + plan_order
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
        f.write("<p>Karten = Card Keys (Item_aot_set) und Lageplaene (Raum-Objekt des Aufnahme-Ereignisses, "
                "kein Item; Liste lageplaene.csv). Der Laborplan (room6120) hat kein Weltobjekt.</p>\n")
        for titel, auswahl in (("Karten (Card Keys + Lageplaene)", karten), ("Alle Item-Meshes", order)):
            f.write("<h2>%s (%d)</h2><div class='g'>\n" % (html.escape(titel), len(auswahl)))
            for h, v in auswahl:
                f.write("<div class='k'><img src='modelle/%s_a.png'><img src='modelle/%s_b.png'><br>"
                        "<b>%s</b> %s<br>%s<br><small>%s</small></div>\n" % (
                            v["file"], v["file"], v["label"], html.escape(", ".join(v["names"])),
                            " ".join("0x%02X" % i for i in v["ids"]),
                            html.escape(" ".join("%s/%s" % (u["room"], u["ent"]) for u in v["uses"][:6]))))
            f.write("</div>\n")

    bericht = [
        "# RE2-Welt-Item-Modelle (Runde 35 Spur F, Punkt 6)",
        "Item_aot_set-Platzierungen gesamt: %d" % len(rows),
        "  mit Weltmodell (md1 < 0x20):     %d" % len(withm),
        "  ohne (md1 >= 0x20, meist 255):   %d" % len(without),
        "verschiedene Meshes (md5 MD1):     %d" % len(order),
        "  davon Karten (Card Keys):        %d  (%s)" % (len(karten) - len(plan_order),
            "; ".join("%s %s" % (v["label"], "/".join(v["names"])) for h, v in karten[:len(karten) - len(plan_order)])),
        "Lageplan-Aufnahmen (Frage mit 'map' + Ja-Zweig): %d, mit Weltobjekt %d, verschiedene Meshes %d" % (
            len(plaene), sum(1 for pl in plaene if "md5" in pl), len(plan_order)),
    ] + ["  %s %-22s obj %-4s Aufnahme %s@0x%05X  Platzierung %s  %s" % (
            pl["room"], pl["name"], "-" if pl["obj"] is None else pl["obj"][0], pl["ent"], pl["frage_off"],
            " ".join("%s@0x%05X(%d,%d,%d)" % pp for pp in pl["plaetze"]) or "-",
            (pseen[pl["md5"]]["label"] + " md5 " + pl["md5"][:8]) if "md5" in pl else "KEIN Weltobjekt")
         for pl in plaene] + [
        "Item-Ids mit Weltmodell:           %d" % len(per_id),
        "Bloecke aus den RDT-Tabellen (RDT+0x48/+0x4C @0x800535d4/@0x800535f4), nicht aus scd/*.scd",
        "  am letzten Evt_end beendet: %d Bloecke, %d Byte dahinter (letzter Block: andere RDT-Daten;"
        " sonst <= 3 Byte Ausrichtung)" % (stat["cut_blocks"], stat["cut_bytes"]),
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
