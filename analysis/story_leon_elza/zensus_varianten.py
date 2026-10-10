#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""zensus_varianten.py - VARIANTEN-ZENSUS Leon (ROOMxxx0) <-> Elza (ROOMxxx1), alle 6 Stages.

NUR ANALYSE. Liest die Original-RDTs aus re15_port/shared_assets/PSX/STAGE1..6 und vergleicht je
Raumpaar opcode-exakt (Walker = re15_port/tools/scd_walk_lib.py, EINE Laengentabelle):

  EM    0x44 Sce_em_set     pc1 Slot, pc2 Typ, pc3 grid/Verhalten, pc7 Kill-Flag (Bank 7 St.1-3 /
                            Bank 8 St.4-6, Gate @0x80042120-38), pc8/10/12 x/y/z, pc16 Richtung
  DOOR  0x3B Door_aot_set   pc1 Slot, pc2 sce (0 = inert/zu, 2 = Tuer), Nutzlast pc+14 (lang pc+22):
                            +8 Stage(0-basiert) +9 Raum +10 Cut   (Warp FUN_8001d600)
  ITEM  0x50 Item_aot_set   pc14 Item, pc16 Anzahl, pc18 Taken-Bit (Bank 9), pc20 Prop (lang +8)
  AOT   0x2C Aot_set        pc2 sce: 1 Text(pc14) 3 Ereignis(sub=pc17) 4 Flag(pc14/16/18) 5 Marke ...
  AOTR  0x46 Aot_reset      Retype: pc2 sce, Nutzlast pc4..9
  OBJ   0x2D Obj_model_set  pc1 obj_id, pc2 Render-Typ, pc4 Band, pc10/12/14 Lage
  MSGON 0x2B Message_on     pc1 Text-Id
  CK    0x21 Ck  (Bank, Bit, Soll)    SET 0x22 Set (Bank, Bit, Op 0/1/7)
  ESPR  0x3A Sce_espr_on    (Effekt-Sprites, z.B. Feuer) roh
  XA    0x59 Xa_on          (Sprachausgabe) roh
  EVT   0x04 Evt_exec       sub = pc3
  Texte RDT+0x3C            dekodiert mit der Port-Glyphentabelle (scd_walk_lib.messages)

Variante: Raumlader FUN_800396fc @0x800397e4 `srl a0,a0,31` / @0x800397ec `addu a0,a0,v0`
(Dateiindex = Basis + Bit 31 von DAT_800aca3c), Basis 681 = ROOM1000.RDT, 682 = ROOM1001.RDT
(Dateitabelle @0x8006f43c, Groessen 0x2b388/0x2b370 == Dateigroessen). Bit 31 gesetzt = Elza
(FUN_8001d22c @0x8001d2a4 bltz -> Raum 0x03 LOBBY, sonst 0x17 HELIPORT).
Szenario-Abfrage INNERHALB einer RDT: Ck(1,0,x) = DAT_800aca3c Bit 31 (Banktabelle @0x80074664[1]
= 0x800aca3c; Ck-Handler @0x8003fcf4: Bit = pc2&0x1f MSB-first, Wort = pc2>>5).

Aufruf:  python zensus_varianten.py            -> zensus.csv, zensus.md, zensus.json (neben dem Skript)
"""
import csv, json, os, re, sys
from collections import Counter, defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L  # noqa: E402

ASSETS = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
DEBUG_BIN = os.path.join(REPO, "info", "Re1.5", "PSX", "BIN", "DEBUG.BIN")

# Typ-Namen: NUR als Lesehilfe. Quelle = RE15Editor (Fremdwerkzeug,
# info/.../RE15Editor/.../ScdUtilities.java:215-242) + Port-Belege (0x42 Ada: analysis/befunde_2026-09-21).
# Die Zuordnung Typ -> Person ist fuer NPCs NICHT byte-belegt (0x47 traegt dort "Irons (Leon), Roy (Elza)").
TYPE_NAMES = {
    0x10: "Polizei-Zombie", 0x11: "Polizei-Zombie dick", 0x12: "Alligator", 0x13: "Zombie Frau",
    0x16: "Zivil-Zombie", 0x18: "Labor-Zombie", 0x1A: "Zombie Oberkoerper", 0x20: "Hund",
    0x21: "Kraehe", 0x24: "Wasser", 0x25: "Spinne", 0x26: "0x26 (Feuer?)", 0x27: "Affe/Gorilla",
    0x29: "Affe", 0x2A: "0x2A", 0x2B: "Spinnenmann", 0x2D: "Sicherungsgeraet", 0x30: "Birkin",
    0x36: "Birkin Oberkoerper", 0x40: "NPC 0x40 (Marvin?)", 0x42: "NPC 0x42 (Ada)",
    0x45: "NPC 0x45 (Irons?)", 0x47: "NPC 0x47 (Elliot/Roy?)", 0x49: "NPC 0x49 (Kendo?)",
    0x4B: "NPC 0x4B (Sherry?)", 0x4D: "NPC 0x4D (Annette?)",
}
NPC_TYPES = set(range(0x40, 0x50))

SCE_NAMES = {0: "inert", 1: "Text", 2: "Tuer", 3: "Ereignis", 4: "Flag", 5: "Marke",
             7: "Rampe", 8: "Wasser", 9: "Item", 12: "TreppeX", 13: "TreppeZ"}

STORY_WORDS = re.compile(
    r"\b(leon|elza|ada|marvin|roy|kendo|irons|chief|sherry|annette|birkin|ben|brad|claire|"
    r"dead|died|die|body|corpse|blood|fire|burn\w*|smoke|power|electric\w*|generator|locked|lock|"
    r"cell|prison|jail|escape|help|zombie\w*|monster\w*|survivor\w*|woman|man|girl|officer|"
    r"police|someone|somebody|who|already|broken|destroyed|door|window|key|card)\b", re.I)


def room_names():
    """DEBUG.BIN @0x263A + (637*stage + 13*raum)*2, Name bei +8 (gen_debug_jump_table.py)."""
    out = {}
    try:
        d = open(DEBUG_BIN, "rb").read()
    except OSError:
        return out
    for st in range(6):
        for i in range(49):
            o = 0x263A + (st * 637 + i * 13) * 2
            nm = d[o + 8:o + 22].split(b"\x00")[0].decode("ascii", "replace").strip()
            out[(st + 1, i)] = nm
    return out


README = os.path.join(REPO, "info", "Re1.5", "README.TXT")


def readme_mentions():
    """MZD-Mod-README (info/Re1.5/README.TXT): je Raum-Basis (z.B. ROOM109) die Zeilennummern, die
    'room 109' / 'room109' nennen. Der Auslieferungsstand ist laut README Z.3/7 der 'Magic Zombie Door'-
    Umbau eines Team-IGAS-Builds - so laesst sich je Unterschied pruefen, ob ihn der Mod nennt."""
    out = defaultdict(list)
    try:
        lines = open(README, encoding="latin-1").read().splitlines()
    except OSError:
        return out
    for n, ln in enumerate(lines, 1):
        for m in re.finditer(r"\broom\s*([1-6][0-9A-Fa-f]{2})\b", ln, re.I):
            out["ROOM" + m.group(1).upper()].append(n)
    return out


def dest_name(stage0, room):
    return "ROOM%d%02X" % (stage0 + 1, room)


def walk_file(path):
    d = open(path, "rb").read()
    R = dict(size=len(d), hdr=list(d[0:8]), regions=[], recs=defaultdict(list), msgs={},
             cover_ok=0, cover_all=0, desync=[])
    if len(d) < 0x60:          # 4-Byte-Platzhalter (z.B. ROOM1270/1271): kein Raum
        return d, R
    R["msgs"] = {k: v[0] for k, v in L.messages(d).items()}
    for sec_off, tag in ((0x40, "main"), (0x44, "sub")):
        sec = L.u32(d, sec_off)
        if sec == 0 or sec >= len(d):
            continue
        se = L.rdt_section_end(d, sec)
        for (o, e, idx) in L.section_regions(d, sec, se):
            lbl = "%s%02d" % (tag, idx)
            ops = L.walk_ops(d, sec + o, sec + e)
            walked = (ops[-1][0] + ops[-1][2] - (sec + o)) if ops else 0
            R["regions"].append((lbl, sec + o, e - o, walked))
            R["cover_all"] += (e - o)
            R["cover_ok"] += walked
            # Ein Walk, der vor dem Regionsende stoppt, ohne auf Evt_end zu enden = Abdeckungsluecke
            if ops and ops[-1][1] != 0x01 and walked < (e - o):
                R["desync"].append((lbl, ops[-1][0] + ops[-1][2]))
            for (pc, op, sz) in ops:
                collect(d, pc, op, lbl, R["recs"])
    return d, R


def s16(d, o):
    return L.s16(d, o)


def collect(d, pc, op, lbl, recs):
    ev = dict(s=lbl, off=pc)
    if op == 0x44:
        recs["EM"].append(dict(ev, slot=d[pc + 1], typ=d[pc + 2], grid=d[pc + 3],
                               b456=d[pc + 4:pc + 7].hex(), kill=d[pc + 7],
                               x=s16(d, pc + 8), y=s16(d, pc + 10), z=s16(d, pc + 12),
                               w14=L.u16(d, pc + 14), dir=s16(d, pc + 16), w18=L.u16(d, pc + 18)))
    elif op == 0x3B:
        lf = (d[pc + 3] & 0x80) != 0
        nl = pc + (22 if lf else 14)
        recs["DOOR"].append(dict(ev, slot=d[pc + 1], sce=d[pc + 2], flags=d[pc + 3], band=d[pc + 4],
                                 dest=dest_name(d[nl + 8], d[nl + 9]), cut=d[nl + 10],
                                 nx=s16(d, nl), ny=s16(d, nl + 2), nz=s16(d, nl + 4), ndir=s16(d, nl + 6),
                                 tail=d[nl + 11:nl + 18].hex()))
    elif op == 0x50:
        lf = (d[pc + 3] & 0x80) != 0
        b = 8 if lf else 0
        recs["ITEM"].append(dict(ev, slot=d[pc + 1], sce=d[pc + 2], flags=d[pc + 3],
                                 item=d[pc + 14 + b], amount=d[pc + 16 + b],
                                 tk=d[pc + 18 + b], prop=d[pc + 20 + b]))
    elif op == 0x2C:
        lf = (d[pc + 3] & 0x80) != 0
        pb = d[pc + (22 if lf else 14):pc + (28 if lf else 20)]
        recs["AOT"].append(dict(ev, slot=d[pc + 1], sce=d[pc + 2], flags=d[pc + 3], band=d[pc + 4],
                                pay=pb.hex(), what=aot_what(d[pc + 2], pb)))
    elif op == 0x46:
        pb = d[pc + 4:pc + 10]
        recs["AOTR"].append(dict(ev, slot=d[pc + 1], sce=d[pc + 2], flags=d[pc + 3], pay=pb.hex(),
                                 what=aot_what(d[pc + 2], pb)))
    elif op == 0x2D:
        recs["OBJ"].append(dict(ev, obj=d[pc + 1], typ=d[pc + 2], b3=d[pc + 3], band=d[pc + 4],
                                hdr=d[pc + 5:pc + 10].hex(),
                                x=s16(d, pc + 10), y=s16(d, pc + 12), z=s16(d, pc + 14),
                                ry=s16(d, pc + 18)))
    elif op == 0x2B:
        recs["MSGON"].append(dict(ev, msg=d[pc + 1], mask=L.u16(d, pc + 2)))
    elif op == 0x21:
        recs["CK"].append(dict(ev, bank=d[pc + 1], bit=d[pc + 2], val=d[pc + 3]))
    elif op == 0x22:
        recs["SET"].append(dict(ev, bank=d[pc + 1], bit=d[pc + 2], val=d[pc + 3]))
    elif op == 0x3A:
        recs["ESPR"].append(dict(ev, raw=d[pc + 1:pc + 16].hex()))
    elif op == 0x59:
        recs["XA"].append(dict(ev, raw=d[pc + 1:pc + 4].hex()))
    elif op == 0x04:
        recs["EVT"].append(dict(ev, cond=d[pc + 1], typ=d[pc + 2], sub=d[pc + 3]))
    elif op in (0x54, 0x57):
        recs["BGM"].append(dict(ev, op="%02x" % op, raw=d[pc + 1:pc + (6 if op == 0x54 else 4)].hex()))


def aot_what(sce, pb):
    if sce == 1:
        return "Text %d" % pb[0]
    if sce == 3:
        return "sub%02d" % pb[3]
    if sce == 4:
        return "Flag(%d,%d)=%d" % (pb[0], pb[2], 1 if pb[4] else 0)
    return SCE_NAMES.get(sce, "sce%d" % sce)


# ---- Vergleichs-Schluessel je Kategorie (ohne Offset/Skript) ----
KEYS = {
    "EM":    lambda r: (r["typ"], r["grid"], r["kill"], r["x"], r["y"], r["z"], r["dir"]),
    "DOOR":  lambda r: (r["sce"], r["dest"], r["cut"]),
    "ITEM":  lambda r: (r["item"], r["amount"], r["tk"], r["sce"]),
    "AOT":   lambda r: (r["sce"], r["flags"], r["what"]),
    "AOTR":  lambda r: (r["slot"], r["sce"], r["what"]),
    "OBJ":   lambda r: (r["typ"], r["band"], r["x"], r["y"], r["z"], r["ry"]),
    "MSGON": lambda r: (r["msg"],),
    "CK":    lambda r: (r["bank"], r["bit"], r["val"]),
    "SET":   lambda r: (r["bank"], r["bit"], r["val"]),
    "ESPR":  lambda r: (r["raw"],),
    "XA":    lambda r: (r["raw"],),
    "EVT":   lambda r: (r["sub"],),
    "BGM":   lambda r: (r["op"], r["raw"]),
}
CATS = list(KEYS)


def fmt(cat, r):
    o = "%s@0x%05X" % (r["s"], r["off"])
    if cat == "EM":
        return "%s Typ 0x%02X %s grid 0x%02X kill %s (%d,%d,%d) dir %d" % (
            o, r["typ"], TYPE_NAMES.get(r["typ"], ""), r["grid"],
            "-" if r["kill"] == 0xFF else r["kill"], r["x"], r["y"], r["z"], r["dir"])
    if cat == "DOOR":
        return "%s Slot %d sce %d(%s) -> %s Cut %d" % (o, r["slot"], r["sce"], SCE_NAMES.get(r["sce"], "?"),
                                                    r["dest"], r["cut"])
    if cat == "ITEM":
        return "%s Slot %d Item 0x%02X x%d Taken-Bit (9,%d) Prop %d sce %d" % (
            o, r["slot"], r["item"], r["amount"], r["tk"], r["prop"], r["sce"])
    if cat == "AOT":
        return "%s Slot %d sce %d flags 0x%02X %s" % (o, r["slot"], r["sce"], r["flags"], r["what"])
    if cat == "AOTR":
        return "%s Aot_reset Slot %d -> sce %d %s" % (o, r["slot"], r["sce"], r["what"])
    if cat == "OBJ":
        return "%s obj %d typ %d band %d (%d,%d,%d) ry %d" % (o, r["obj"], r["typ"], r["band"], r["x"], r["y"],
                                                             r["z"], r["ry"])
    if cat == "MSGON":
        return "%s Message_on %d" % (o, r["msg"])
    if cat in ("CK", "SET"):
        return "%s %s(%d,%d,%d)" % (o, "Ck" if cat == "CK" else "Set", r["bank"], r["bit"], r["val"])
    if cat == "EVT":
        return "%s Evt_exec sub%02d" % (o, r["sub"])
    if cat == "BGM":
        return "%s op%s %s" % (o, r["op"], r["raw"])
    return "%s %s" % (o, r.get("raw", ""))


def diff_cat(cat, A, B):
    """Multimengen-Differenz: (nur_A, nur_B) als Listen der Records."""
    ka = Counter(KEYS[cat](r) for r in A)
    kb = Counter(KEYS[cat](r) for r in B)
    oa, ob = ka - kb, kb - ka
    ra, rb = [], []
    for r in A:
        k = KEYS[cat](r)
        if oa.get(k, 0) > 0:
            ra.append(r); oa[k] -= 1
    for r in B:
        k = KEYS[cat](r)
        if ob.get(k, 0) > 0:
            rb.append(r); ob[k] -= 1
    return ra, rb


def diff_msgs(ma, mb):
    ta = Counter(ma.values()); tb = Counter(mb.values())
    oa, ob = ta - tb, tb - ta
    ra = [(i, t) for i, t in sorted(ma.items()) if oa.get(t, 0) > 0]
    rb = [(i, t) for i, t in sorted(mb.items()) if ob.get(t, 0) > 0]
    return ra, rb


def used_msgs(R):
    """Text-Ids, die ein Skript anzeigt: Message_on pc[1] (Handler @0x800404f4 `lbu a2,1(v0)`)
    + sce-1-AOTs (Aot_set/Aot_reset, Nutzlast u16@0 = Text-Id, Handler @0x80043084)."""
    u = set(r["msg"] for r in R["recs"].get("MSGON", []))
    for r in R["recs"].get("AOT", []) + R["recs"].get("AOTR", []):
        if r["sce"] == 1:
            u.add(int(r["pay"][0:2], 16))
    return u


def story_tags(dd, ma_only, mb_only, RA, RB):
    tags = []
    if any(r["typ"] in NPC_TYPES for r in dd["EM"][0] + dd["EM"][1]):
        tags.append("NPC")
    ea = [r for r in RA["recs"]["EM"] if r["typ"] not in NPC_TYPES]
    eb = [r for r in RB["recs"]["EM"] if r["typ"] not in NPC_TYPES]
    if len(ea) != len(eb) or Counter(r["typ"] for r in ea) != Counter(r["typ"] for r in eb):
        tags.append("Gegner")
    if dd["ITEM"][0] or dd["ITEM"][1]:
        tags.append("Items")
    if dd["DOOR"][0] or dd["DOOR"][1]:
        tags.append("Tueren")
    if dd["OBJ"][0] or dd["OBJ"][1]:
        tags.append("Objekte")
    if dd["ESPR"][0] or dd["ESPR"][1]:
        tags.append("Effekte")
    if ma_only or mb_only:
        if any(STORY_WORDS.search(t) for _, t in ma_only + mb_only):
            tags.append("Texte*")
        else:
            tags.append("Texte")
    if any(r["bank"] == 1 and r["bit"] == 0 for r in RA["recs"]["CK"] + RB["recs"]["CK"]):
        tags.append("CHAR-CK")
    return tags


def main():
    names = room_names()
    rmz = readme_mentions()
    rows, md, js = [], [], {}
    pairs = []
    for st in range(1, 7):
        sd = os.path.join(ASSETS, "STAGE%d" % st)
        bases = sorted(set(f[:-5] for f in os.listdir(sd) if re.match(r"ROOM\d[0-9A-F]{2}[01]\.RDT$", f)))
        for b in bases:
            pairs.append((st, b))
    char_ck_sites = []
    for st, b in pairs:
        sd = os.path.join(ASSETS, "STAGE%d" % st)
        pa, pb = os.path.join(sd, b + "0.RDT"), os.path.join(sd, b + "1.RDT")
        ha, hb = os.path.exists(pa), os.path.exists(pb)
        ridx = int(b[5:7], 16)
        nm = names.get((st, ridx), "")
        if not (ha and hb):
            rows.append(dict(stage=st, raum=b, name=nm, leon=int(ha), elza=int(hb)))
            md.append("### %s  (%s)\n\nNUR %s vorhanden.\n" % (b, nm, "Leon" if ha else "Elza"))
            continue
        da, RA = walk_file(pa)
        db, RB = walk_file(pb)
        same = (da == db)
        dd = {c: diff_cat(c, RA["recs"][c], RB["recs"][c]) for c in CATS}
        ma_only, mb_only = diff_msgs(RA["msgs"], RB["msgs"])
        ua0, ub0 = used_msgs(RA), used_msgs(RB)
        tags = story_tags(dd, [x for x in ma_only if x[0] in ua0], [x for x in mb_only if x[0] in ub0], RA, RB)
        for R, v in ((RA, "0"), (RB, "1")):
            for cat in ("CK", "SET"):
                for r in R["recs"][cat]:
                    if r["bank"] == 1 and r["bit"] == 0:
                        char_ck_sites.append((b + v, cat, r))
        row = dict(stage=st, raum=b, name=nm, leon=1, elza=1, byte_gleich=int(same),
                   size_L=RA["size"], size_E=RB["size"],
                   hdr_L=" ".join(map(str, RA["hdr"][:7])), hdr_E=" ".join(map(str, RB["hdr"][:7])),
                   subs_L=len(RA["regions"]), subs_E=len(RB["regions"]),
                   msgs_L=len(RA["msgs"]), msgs_E=len(RB["msgs"]),
                   abdeckung_L="%d/%d" % (RA["cover_ok"], RA["cover_all"]),
                   abdeckung_E="%d/%d" % (RB["cover_ok"], RB["cover_all"]),
                   story=";".join(tags),
                   mzd_readme_zeilen=" ".join(map(str, rmz.get(b, []))))
        for c in CATS:
            row["%s_L" % c] = len(RA["recs"][c]); row["%s_E" % c] = len(RB["recs"][c])
            row["%s_diff" % c] = len(dd[c][0]) + len(dd[c][1])
        row["txt_diff"] = len(ma_only) + len(mb_only)
        rows.append(row)
        js[b] = dict(name=nm, same=same, tags=tags,
                     L=dict(size=RA["size"], hdr=RA["hdr"], regions=RA["regions"], desync=RA["desync"],
                            recs=RA["recs"], msgs=RA["msgs"]),
                     E=dict(size=RB["size"], hdr=RB["hdr"], regions=RB["regions"], desync=RB["desync"],
                            recs=RB["recs"], msgs=RB["msgs"]))
        # ---- Markdown je Raum ----
        head = "### %s  \"%s\"  %s" % (b, nm, ("[" + ", ".join(tags) + "]") if tags else "")
        md.append(head + "\n")
        if rmz.get(b):
            md.append("MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) %s.\n" %
                      ", ".join(map(str, rmz[b])))
        if same:
            md.append("Byte-identisch (%d B).\n" % RA["size"])
            continue
        md.append("Groesse L %d / E %d B; Kopf L [%s] / E [%s]; Skripte L %d / E %d; Texte L %d / E %d; "
                  "Abdeckung L %s / E %s%s\n" % (
                      RA["size"], RB["size"], row["hdr_L"], row["hdr_E"], row["subs_L"], row["subs_E"],
                      row["msgs_L"], row["msgs_E"], row["abdeckung_L"], row["abdeckung_E"],
                      ("; Walk-Luecken L %s E %s" % (RA["desync"], RB["desync"])) if (RA["desync"] or RB["desync"]) else ""))
        anydiff = False
        for c in ["EM", "ITEM", "DOOR", "OBJ", "AOT", "AOTR", "ESPR", "XA", "BGM", "MSGON", "EVT", "CK", "SET"]:
            a, bb = dd[c]
            if not a and not bb:
                continue
            anydiff = True
            if c in ("CK", "SET", "MSGON", "EVT") and len(a) + len(bb) > 12:
                ca = Counter(KEYS[c](r) for r in a); cb = Counter(KEYS[c](r) for r in bb)
                md.append("- **%s** nur Leon: %s" % (c, ", ".join("%s x%d" % (k, n) for k, n in sorted(ca.items()))))
                md.append("- **%s** nur Elza: %s" % (c, ", ".join("%s x%d" % (k, n) for k, n in sorted(cb.items()))))
                continue
            for r in a:
                md.append("- **%s** nur Leon: %s" % (c, fmt(c, r)))
            for r in bb:
                md.append("- **%s** nur Elza: %s" % (c, fmt(c, r)))
        ua, ub = used_msgs(RA), used_msgs(RB)
        for i, t in ma_only:
            md.append("- **TEXT** nur Leon id %d%s: \"%s\"" % (i, "" if i in ua else " (UNBENUTZT)", t)); anydiff = True
        for i, t in mb_only:
            md.append("- **TEXT** nur Elza id %d%s: \"%s\"" % (i, "" if i in ub else " (UNBENUTZT)", t)); anydiff = True
        if not anydiff:
            md.append("- (keine Unterschiede in den gezaehlten Kategorien; Byte-Unterschied liegt in anderen "
                      "Sektionen, z.B. Kamera/Kollision/Modelle/Offsets)")
        md.append("")
    # ---- CSV ----
    keys = []
    for r in rows:
        for k in r:
            if k not in keys:
                keys.append(k)
    with open(os.path.join(HERE, "zensus.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=keys)
        w.writeheader()
        for r in rows:
            w.writerow(r)
    with open(os.path.join(HERE, "zensus.json"), "w", encoding="utf-8") as f:
        json.dump(js, f, ensure_ascii=False, indent=0, default=str)
    # ---- MD ----
    n_same = sum(1 for r in rows if r.get("byte_gleich") == 1)
    story = [r for r in rows if r.get("story")]
    out = ["# Varianten-Zensus Leon (xxx0) <-> Elza (xxx1)", "",
           "Erzeugt von `zensus_varianten.py` aus `re15_port/shared_assets/PSX/STAGE1..6/ROOM*.RDT`. "
           "Alle Offsets = RDT-Datei-Offset; `mainNN`/`subNN` = SCD-Region.", "",
           "Raumpaare: %d; byte-identisch: %d; mit Story-relevantem Unterschied: %d." % (len(rows), n_same, len(story)),
           "", "Kennzeichen: NPC = NPC-Spawns (Typ 0x40-0x4F) unterscheiden sich; Gegner = Anzahl/Typen der Gegner; "
           "Items; Tueren (Ziel/sce); Objekte (Obj_model_set); Effekte (Sce_espr_on); Texte* = abweichende Texte mit "
           "Story-Stichwort; CHAR-CK = Raum fragt Ck(1,0) = Elza-Bit ab.", "",
           "## Uebersicht (nur Raeume mit Kennzeichen)", "",
           "| Raum | Name | Kennzeichen | EM L/E | Items L/E | Tueren L/E | Texte L/E | MZD-README Z. |",
           "|---|---|---|---|---|---|---|---|"]
    for r in story:
        out.append("| %s | %s | %s | %d/%d | %d/%d | %d/%d | %d/%d | %s |" % (
            r["raum"], r["name"], r["story"].replace(";", ", "), r["EM_L"], r["EM_E"], r["ITEM_L"], r["ITEM_E"],
            r["DOOR_L"], r["DOOR_E"], r["msgs_L"], r["msgs_E"], r.get("mzd_readme_zeilen", "")))
    out += ["", "Deutung der Story-Spuren, Zeitleisten-Bedingungen und Gegenpruefung: `A_zensus.md` (gleicher Ordner).",
            "Achtung Herkunft: Der Auslieferungsstand ist laut `info/Re1.5/README.TXT` Z.3/7 der fan-gemachte "
            "\"Magic Zombie Door\"-Umbau (MartinBiohazard) eines Team-IGAS-Builds; die Spalte MZD-README nennt die "
            "Zeilen, in denen der Mod diesen Raum erwaehnt."]
    out += ["", "## Szenario-Abfragen innerhalb der RDTs: Ck/Set auf Bank 1 Bit 0 (Elza-Bit)", ""]
    n_ck = sum(1 for _, c, _ in char_ck_sites if c == "CK")
    out.append("Ck(1,0,x) (= Abfrage): %d Stellen. Set(1,0,x) (= Umschalten): %d Stellen." % (
        n_ck, len(char_ck_sites) - n_ck))
    for f, c, r in char_ck_sites:
        out.append("- %s %s" % (f, fmt(c, r)))
    out += ["", "## Je Raum", ""] + md
    with open(os.path.join(HERE, "zensus.md"), "w", encoding="utf-8") as f:
        f.write("\n".join(out) + "\n")
    print("Paare %d, byte-gleich %d, Story-Kennzeichen %d, Elza-Bit-Ck/Set %d" % (len(rows), n_same, len(story),
                                                                         len(char_ck_sites)))
    cov = [(b, v["L"]["desync"], v["E"]["desync"]) for b, v in js.items() if v["L"]["desync"] or v["E"]["desync"]]
    print("Walk-Luecken (Region nicht bis Evt_end gelaufen):", len(cov))
    for c in cov[:40]:
        print("  ", c)


if __name__ == "__main__":
    main()
