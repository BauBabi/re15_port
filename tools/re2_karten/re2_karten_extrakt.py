#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_karten_extrakt.py - die KARTEN von Resident Evil 2 (Retail, Leon) extrahieren:
Weltmodelle, Kartenblaetter, Namen, Fundorte, Auswahlbogen.

Baugleich zur Dokument-Extraktion (tools/re2_sicherung/re2_doc_worldmodels.py +
re15_port/tools/re2_dokumente_extrakt.py) - dieselben Bausteine, dieselbe Ausgabe-Form.

⛔ BELEGKETTE (alles aus info/re2leon/PSX.EXE, RAM-Adressen; Ghidra-Dump ghidra_re2_Leon.txt;
   Datei-Offset = RAM - 0x8000F800, t_addr @0x18 = 0x80010000)

1) RE2 hat KEIN Karten-ITEM. Namensbank EN: Offsettabelle 0x8009EBAC, Basis 0x8009E550
   (`lhu v1,-0x1454(at)` @0x80030C24 / `addiu v0,v0,-0x1AB0` @0x80030C2C). Alle 140
   Indizes dekodiert -> kein Eintrag "Map". Ids 0..100 Gegenstaende, 104..128 Dokumente.

2) Karten sind FLAG-BANK 33. Bank-Zeigertabelle 0x800A78C8
   (`lw v1,30920(at)` @0x80054384 im Ck-Handler, @0x800543E8 im Set-Handler).
   Eintrag 33 = 0x800D4924. Der Kartenbildschirm testet genau diese Bank:
   FUN_8006dcc0 -> FUN_80077360(&DAT_800d4924, etage) mit
   `bits[id>>5] & (0x80000000 >> (id&31))` (FUN_80077360 @0x80077360, MSB zuerst).

3) SCD-Opcodes (Sprungtabelle 0x800A74C8):
   op 0x21 Ck  -> 0x80054354, 4 B {op, bank, bit, sollwert}
   op 0x22 Set -> 0x800543B4, 4 B {op, bank, bit, wert}
       `lbu v1,1(v0)` @0x800543BC, `lbu a1,2(v0)` @0x800543C0,
       `lbu a2,3(v0)` @0x800543C4, `addiu v0,v0,4` @0x800543C8

4) Das Weltmodell haengt am OBJEKT-POOL - demselben, den `Item_aot_set.md1` indiziert.
   `Work_set` op 0x2E @0x80055904, 3 B {op, typ, index}; Sprungtabelle 0x800111F0,
   Fall typ=4 @0x80055994:  idx*64 - idx = idx*63, <<3 = idx*0x1F8, Basis 0x800D0324.
   Genau die Rechnung steht auch im Item_aot_set-Handler @0x80054D9C-DB4.
   Pool-Eintrag k <-> Paar k in RDT+0x30, Anzahl RDT+0x02 (Lader FUN_80052D14:
   `lbu s2,0x2(v0)` @0x80052D70, `lw s4,0x30(v0)` @0x80052D74, `addiu s4,s4,0x8` @0x80052DF4).

5) Verstecken nach dem Fund:
   `Pos_set` op 0x32 @0x80055BA0: `sw a1,56(v1)/sw a2,60(v1)/sw a3,64(v1)` = x/y/z
   `Member_set` op 0x34 @0x80055C00 -> FUN_80055CB0; Member 0x0C schreibt
   *(u32*)(obj+0x3C) = 60 = DASSELBE Feld wie Pos_set-Y.

6) Kartenblaetter: CD-Datei-Id 171 = MAPS.PIX (`addiu a0,zero,171` @0x8006D6C0,
   Blockname "MAP FILE" @0x80011C88). Slot-Tabelle 0x800A9414, 20 x 8 B
   {u32 groesse; u16 sektor_lo; u8 sektor_hi; u8}: `lw v0,-27628(at)` @0x8006D718,
   `lhu t0,-27624(at)` @0x8006D754, `lbu v0,-27622(at)` @0x8006D748.
   Jede Groesse 0x8000, Sektoren 0x000..0x130 im Schritt 0x10 -> 20 x 32768 = 655360 B
   = exakt die Dateigroesse. VRAM-Ziel (448,256), 64 x 256 Worte
   (`addiu v0,zero,448` @0x8006D77C, `addiu v1,zero,256` @0x8006D780,
    `addiu v0,zero,64` @0x8006D788) => 4bpp, 256 x 256 Pixel.
   Karten-Kopftabelle 0x800AAA38, 20 x 8 B {u32 ptr; u8 n; u8 map_id; u16 pix_slot}
   (`lhu t0,-21954(at)` @0x8006D6DC liest +6).

⛔ WAS NICHT BELEGT IST: die Palette. MAPS.PIX enthaelt NUR den 4bpp-Koerper (20 x 0x8000,
   nachgerechnet). Das Spiel waehlt die CLUT je Raumzustand: y = 490 (`addiu s1,zero,490`
   @0x8006E3F0), 506 (@0x8006E4E4), 510 (@0x8006E500), 492 (`GetClut(0x100,0x1ec)` in
   FUN_8006dea0), x = 256 (`addiu a0,zero,256` @0x8006E74C vor `jal GetClut` @0x8006E750).
   Die hier benutzte Palette stammt aus dem CLUT-Block der bereits im Repo liegenden
   MAPS_0NN.TIM (cx=0, cy=480, cw=256, ch=3), 16-Farben-Block 0 der Zeile 0. Das ist eine
   ANSICHTSHILFE, kein Beleg fuer die Spielfarbe.
"""
import os, sys, struct, glob, hashlib, json, csv, html

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "tools", "re2_sicherung"))
import re2_scd_walk as W
import rdt_props, md1_view, msg_decode
import re2_doc_worldmodels as DWM          # render_clut / tim_pages
from PIL import Image, ImageDraw

RDTDIR = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
PIXP = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "MAPS.PIX")
TIMP = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "MAPS", "MAPS_001.TIM")
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")

MAP_BANK = 33                  # 0x800A78C8[33] = 0x800D4924
SLOT_TBL = 0x800A9414          # 20 x 8 B
HEAD_TBL = 0x800AAA38          # 20 x 8 B
N_MAPS = 20

D = open(EXE, "rb").read()
TADDR = struct.unpack_from("<I", D, 0x18)[0]
def fo(a): return 0x800 + (a - TADDR)


# ---------------------------------------------------------------- Zensus ----
def scan_bank33():
    """Jedes Set/Ck auf Bank 33 in allen 2553 SCD-Bloecken der 250 RDTs."""
    sets, cks, stat = [], [], dict(blocks=0, total=0, walked=0, desync=0)
    for d in sorted(glob.glob(os.path.join(RDTDIR, "room*"))):
        if not os.path.isdir(d):
            continue
        room = os.path.basename(d)
        for f in sorted(glob.glob(os.path.join(d, "scd", "*.scd"))):
            ent = os.path.splitext(os.path.basename(f))[0]
            buf = open(f, "rb").read()
            recs, st = W.walk(buf)
            stat["blocks"] += 1
            stat["total"] += len(buf)
            stat["walked"] += (recs[-1][0] + len(recs[-1][2])) if recs else 0
            if st != "ok":
                stat["desync"] += 1
            for off, op, r in recs:
                if op == 0x22 and r[1] == MAP_BANK:
                    sets.append(dict(room=room, ent=ent, off=off, bit=r[2], val=r[3]))
                elif op == 0x21 and r[1] == MAP_BANK:
                    cks.append(dict(room=room, ent=ent, off=off, bit=r[2], val=r[3]))
    # unabhaengige Gegenprobe: jede Byte-Position als Set(bank=33) lesen
    brute = 0
    for f in sorted(glob.glob(os.path.join(RDTDIR, "room*", "scd", "*.scd"))):
        b = open(f, "rb").read()
        for i in range(len(b) - 4):
            if b[i] == 0x22 and b[i + 1] == MAP_BANK and b[i + 2] < 0x20 and b[i + 3] in (0, 1, 7):
                brute += 1
    stat["brute"] = brute
    return sets, cks, stat


def find_trigger(room, subno):
    """Wer startet sub<subno>? Aot_set(0x2C, sub bei +17), Aot_set_4p(0x67, sub bei +28),
       Gosub(0x18), Evt_exec(0x04: {op, cond, ex_op, event})."""
    out = []
    for f in sorted(glob.glob(os.path.join(RDTDIR, room, "scd", "*.scd"))):
        ent = os.path.splitext(os.path.basename(f))[0]
        buf = open(f, "rb").read()
        recs, st = W.walk(buf)
        for off, op, r in recs:
            if op == 0x2C and len(r) > 17 and r[2] == 5 and r[17] == subno:
                out.append(dict(kind="Aot_set", ent=ent, off=off, aot=r[1],
                                x=struct.unpack_from("<h", r, 6)[0],
                                z=struct.unpack_from("<h", r, 8)[0],
                                w=struct.unpack_from("<h", r, 10)[0],
                                d=struct.unpack_from("<h", r, 12)[0],
                                raw=r.hex()))
            elif op == 0x67 and len(r) > 28 and r[2] == 5 and r[28] == subno:
                pts = [struct.unpack_from("<hh", r, 6 + k * 4) for k in range(4)]
                out.append(dict(kind="Aot_set_4p", ent=ent, off=off, aot=r[1],
                                x=sum(p[0] for p in pts) // 4, z=sum(p[1] for p in pts) // 4,
                                w=0, d=0, pts=pts, raw=r.hex()))
            elif op == 0x18 and r[1] == subno:
                out.append(dict(kind="Gosub", ent=ent, off=off))
            elif op == 0x04 and r[3] == subno:
                out.append(dict(kind="Evt_exec", ent=ent, off=off, cond=r[1], exop=r[2]))
    return out


def obj_model_sets(room):
    """Alle Obj_model_set (op 0x2D, 38 B) des Raums: Slot -> Position."""
    out = []
    for f in sorted(glob.glob(os.path.join(RDTDIR, room, "scd", "*.scd"))):
        ent = os.path.splitext(os.path.basename(f))[0]
        buf = open(f, "rb").read()
        recs, st = W.walk(buf)
        for off, op, r in recs:
            if op == 0x2D:
                out.append(dict(ent=ent, off=off, slot=r[1],
                                x=struct.unpack_from("<h", r, 14)[0],
                                y=struct.unpack_from("<h", r, 16)[0],
                                z=struct.unpack_from("<h", r, 18)[0]))
    return out


def hide_ops(room, ent):
    """Work_set/Pos_set/Member_set im Fund-Block: wie das Modell verschwindet."""
    buf = open(os.path.join(RDTDIR, room, "scd", ent + ".scd"), "rb").read()
    recs, st = W.walk(buf)
    cur, out = None, []
    for off, op, r in recs:
        if op == 0x2E:
            cur = (r[1], struct.unpack_from("<b", r, 2)[0])
        elif op == 0x32 and cur and cur[0] == 4:
            out.append(dict(off=off, slot=cur[1], kind="Pos_set",
                            x=struct.unpack_from("<h", r, 2)[0],
                            y=struct.unpack_from("<h", r, 4)[0],
                            z=struct.unpack_from("<h", r, 6)[0]))
        elif op == 0x34 and cur and cur[0] == 4:
            out.append(dict(off=off, slot=cur[1], kind="Member_set",
                            member=r[1], value=struct.unpack_from("<h", r, 2)[0]))
    return out


def msgs(room):
    out = {}
    for m in sorted(glob.glob(os.path.join(RDTDIR, room, "msg", "sub*.msg"))):
        k = int(os.path.basename(m)[3:5])
        out[k] = msg_decode.text(m).strip()
    return out


# ------------------------------------------------------ Kartenblaetter ------
def sheets(out):
    """Die 20 Kartenbilder aus MAPS.PIX, 256x256 4bpp."""
    os.makedirs(out, exist_ok=True)
    pix = open(PIXP, "rb").read()
    t = open(TIMP, "rb").read()
    csz = struct.unpack_from("<I", t, 8)[0]
    pal = t[8 + 12: 8 + csz]
    # Slot-Tabelle aus der EXE gegenpruefen
    rows = []
    for k in range(N_MAPS):
        sz, lo, hi, x = struct.unpack_from("<IHBB", D, fo(SLOT_TBL + k * 8))
        rows.append((sz, (hi << 16) | lo, x))
    head = []
    for k in range(N_MAPS):
        ptr, n, mid, slot = struct.unpack_from("<IBBH", D, fo(HEAD_TBL + k * 8))
        head.append(dict(ptr=ptr, n=n, map_id=mid, pix_slot=slot))
    assert sum(r[0] for r in rows) == len(pix), (sum(r[0] for r in rows), len(pix))

    def render(body, row, blk):
        im = Image.new("RGB", (256, 256))
        px = im.load()
        base = row * 512 + blk * 32
        lut = []
        for i in range(16):
            o = base + i * 2
            v = pal[o] | (pal[o + 1] << 8)
            lut.append(((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3))
        for y in range(256):
            ro = y * 128
            for x in range(256):
                b = body[ro + x // 2]
                px[x, y] = lut[b & 0xF if x % 2 == 0 else b >> 4]
        return im

    made = []
    for k in range(N_MAPS):
        off = rows[k][1] * 0x800
        body = pix[off: off + rows[k][0]]
        open(os.path.join(out, "blatt%02d.bin" % k), "wb").write(body)
        im = render(body, 0, 0)
        im.save(os.path.join(out, "blatt%02d.png" % k))
        made.append(dict(slot=k, off=off, size=rows[k][0],
                         md5=hashlib.md5(body).hexdigest()))
    # Kontaktbogen
    S, C = 256, 5
    R = (N_MAPS + C - 1) // C
    sheet = Image.new("RGB", (S * C, (S + 18) * R), (12, 12, 16))
    dr = ImageDraw.Draw(sheet)
    for k in range(N_MAPS):
        r, c = divmod(k, C)
        sheet.paste(Image.open(os.path.join(out, "blatt%02d.png" % k)),
                    (c * S, r * (S + 18) + 18))
        dr.text((c * S + 4, r * (S + 18) + 4), "PIX-Slot %d" % k, fill=(240, 240, 240))
    sheet.save(os.path.join(out, "_kontaktbogen_blaetter.png"))
    return made, head, rows


# ------------------------------------------------------------- Modelle ------
def cut_model(room, slot, pre):
    rdt = os.path.join(RDTDIR, "ROOM%s.RDT" % room[4:].upper())
    d, n, tbl, props = rdt_props.parse(rdt)
    if slot >= n:
        return None
    p = props[slot]
    md1 = d[p["md1_off"]:p["md1_off"] + p["md1_size"]]
    tim = d[p["tim_off"]:p["tim_off"] + p["tim_size"]]
    open(pre + ".md1", "wb").write(md1)
    open(pre + ".tim", "wb").write(tim)
    info = dict(nomodel=n, md1_off=p["md1_off"], md1_size=len(md1),
                tim_off=p["tim_off"], tim_size=len(tim),
                md5=hashlib.md5(md1).hexdigest())
    try:
        h, tris, quads = md1_view.geometry(md1)
        with open(pre + ".obj", "w") as f:
            f.write("# RE2 Karten-Weltmodell %s Slot %d  md5=%s\n" % (room, slot, info["md5"]))
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
        info.update(tris=len(tris), quads=len(quads), nobj=h["nobj"],
                    bb=(min(v[0] for v in allv), max(v[0] for v in allv),
                        min(v[1] for v in allv), max(v[1] for v in allv),
                        min(v[2] for v in allv), max(v[2] for v in allv)))
        for ang, tag in ((0.7, "a"), (2.3, "b")):
            DWM.render_clut(md1, tim, pre + "_%s.png" % tag, 320, ang, 0.55)
    except Exception as e:
        info["fehler"] = str(e)
    return info


# ---------------------------------------------------------------- main ------
KARTEN = [
    # name,               raum,      fundblock, slot, bits
    ("police station map", "room20B0", "sub10", 8,  [2, 3, 4]),
    ("police B1 map",      "room2130", "sub05", 0,  [5]),
    ("sewage disposal map","room3060", "sub22", 7,  [6]),
    ("sewer map",          "room4040", "sub06", 2,  [7, 8]),
    ("factory map",        "room5040", "sub13", 1,  [9, 10, 11]),
    ("factory map",        "room5060", "sub04", 2,  [9, 10, 11]),
    ("laboratory map",     "room6120", "sub03", None, [13, 14, 15, 16, 17]),
]


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "build", "extracted", "re2_karten")
    os.makedirs(out, exist_ok=True)
    wm = os.path.join(out, "weltmodelle"); os.makedirs(wm, exist_ok=True)
    bl = os.path.join(out, "blaetter")

    sets, cks, stat = scan_bank33()
    print("SCD: %d Bloecke, %d desynchron, %d/%d Bytes = %.2f%% gewalkt"
          % (stat["blocks"], stat["desync"], stat["walked"], stat["total"],
             100.0 * stat["walked"] / stat["total"]))
    print("Set(bank=33): %d gewalkt, %d in der Brute-Force-Gegenprobe" % (len(sets), stat["brute"]))
    print("Ck (bank=33): %d" % len(cks))
    bits = sorted(set(s["bit"] for s in sets))
    print("gesetzte Bits: %s  -> nie gesetzt: %s"
          % (bits, [b for b in range(N_MAPS) if b not in bits]))

    made, head, rows = sheets(bl)
    print("Kartenblaetter: %d x 256x256 4bpp aus MAPS.PIX (%d B)"
          % (len(made), sum(r[0] for r in rows)))

    rec = []
    for name, room, ent, slot, bb in KARTEN:
        m = msgs(room)
        trig = find_trigger(room, int(ent[3:]))
        oms = [o for o in obj_model_sets(room) if slot is not None and o["slot"] == slot]
        hid = hide_ops(room, ent)
        info = None
        if slot is not None:
            info = cut_model(room, slot, os.path.join(wm, "%s_slot%02d" % (room, slot)))
        rec.append(dict(name=name, room=room, block=ent, slot=slot, bits=bb,
                        trigger=trig, obj_model_set=oms, hide=hid, model=info,
                        msgs=m))
        print("%-21s %-9s Slot=%-4s Bits=%-14s Obj_model_set=%d  Modell=%s"
              % (name, room, slot, bb, len(oms),
                 ("%d B md5=%s" % (info["md1_size"], info["md5"][:8])) if info else "-"))

    json.dump(rec, open(os.path.join(out, "karten.json"), "w"), indent=1, default=str)
    with open(os.path.join(out, "karten.csv"), "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["name", "raum", "block", "slot", "bits", "obj_pos", "modell_md5",
                    "md1_byte_offset", "md1_bytes"])
        for r in rec:
            pos = ("%d,%d,%d" % (r["obj_model_set"][0]["x"], r["obj_model_set"][0]["y"],
                                 r["obj_model_set"][0]["z"])) if r["obj_model_set"] else ""
            mo = r["model"] or {}
            w.writerow([r["name"], r["room"], r["block"], r["slot"],
                        " ".join(map(str, r["bits"])), pos, mo.get("md5", ""),
                        hex(mo.get("md1_off", 0)), mo.get("md1_size", "")])
    json.dump(dict(sets=sets, cks=cks, stat=stat, head=head,
                   slots=[dict(size=r[0], sector=r[1]) for r in rows], sheets=made),
              open(os.path.join(out, "zensus.json"), "w"), indent=1)
    print("geschrieben nach", out)
    return rec


if __name__ == "__main__":
    main()
