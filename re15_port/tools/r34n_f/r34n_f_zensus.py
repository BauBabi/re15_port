#!/usr/bin/env python3
"""r34n_f_zensus.py — Runde 34 Nacht, Spur F (Leichen ROOM1110 / ROOM1230).

Ein Werkzeug, alle Zaehlungen, die der Bauplan braucht. Jede Zahl im Dossier
analysis/befunde_runde34_nacht/F_leichen.md stammt aus einem dieser Teile:

  texte    Nachrichten der vier Raeume (1110/1111/1230/1231) roh + Satzzeichen-Zensus
           ueber ALLE RDTs (Satzende vor 01, Punktfolgen 57x3 / 57x4, "..." vor 02)
  glyphen  Belegstellen je Wort des neuen Textes (erster Fundort im Auslieferungsstand)
  breite   Zeilenbreiten des neuen Textes (Vorschubtabelle include/font_width.h =
           DEBUG.BIN[0x4416+code], Leser FUN_80028868) gegen den Bestand
  msgref   wer im Raum (und seiner Variante) die Leichen-Nachricht oeffnet
           (Message_on pc[1], Aot_set/Aot_reset sce 1 Nutzlast) - nur das Ereignis?
  slots    AOT-Slots + Nachrichtenanzahl je Raum; Leichen-Saetze bytegleich in der Variante?
  munition RE1.5 Item_aot_set Typ 0x15 (Menge) + RE2 Item_aot_set Id 0x14 (Menge, action)
  bank9    Zone-9-Bits (Item_aot_set +18, Ck/Set/0x59 mit Bank 9) ueber alle RDTs

Aufruf (Repo-Wurzel des Baums):  C:/Python310/python.exe re15_port/tools/r34n_f/r34n_f_zensus.py [teil ...]
Ohne Argument laufen alle Teile.
"""
import glob, os, struct, sys, collections

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS = os.path.abspath(os.path.join(HERE, ".."))
REPO = os.path.abspath(os.path.join(TOOLS, "..", ".."))
sys.path.insert(0, TOOLS)
import scd_walk_lib as W   # EINE Laengentabelle (scd_dump_room.py / scd_vm.c s_opcode_sizes)

PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
RAEUME = ("1110", "1111", "1230", "1231")


def rdt(room):
    return open(os.path.join(PSX, "STAGE%s" % room[0], "ROOM%s.RDT" % room), "rb").read()


def alle_rdts():
    """Alle ausgelieferten RDTs mit RDT-Kopf (Platzhalter-Dateien < 0x48 Byte zaehlen nicht mit;
    der Zaehler steht in der Ausgabe von `texte`)."""
    fs = sorted(glob.glob(os.path.join(PSX, "STAGE*", "ROOM*.RDT")))
    return [f for f in fs if os.path.getsize(f) >= 0x48]


def msg_block(d):
    """[(id, start, ende)] der Nachrichten (RDT+0x3C), Ende = naechster Start bzw. Blockende."""
    ms = W.u32(d, 0x3C)
    if ms == 0 or ms + 2 > len(d):
        return []
    first = W.u16(d, ms)
    if first < 2 or first % 2 or ms + first > len(d):
        return []
    n = first // 2
    tbl = [W.u16(d, ms + 2 * i) for i in range(n)]
    out = []
    for i, o in enumerate(tbl):
        st = ms + o
        en = ms + tbl[i + 1] if i + 1 < n and tbl[i + 1] > o else len(d)
        out.append((i, st, min(en, st + 600)))
    return out


def msg_bytes(d, st, en):
    """Nachrichtenbytes bis einschliesslich 01 xx (Steuercodes mit Argument uebersprungen)."""
    i = st
    while i < en:
        b = d[i]
        if b == 0x01:
            return d[st:i + 2]
        if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            i += 2; continue
        i += 1
    return d[st:en]


def hexs(b):
    return " ".join("%02x" % x for x in b)


# ------------------------------------------------------------------------------------------
def teil_texte():
    print("== texte: Nachrichten der vier Raeume ==")
    for r in RAEUME:
        d = rdt(r)
        blk = msg_block(d)
        print("ROOM%s: %d Nachrichten (IDs 0..%d), msg-Sektion @0x%X" % (r, len(blk), len(blk) - 1, W.u32(d, 0x3C)))
        for i, st, en in blk:
            b = msg_bytes(d, st, en)
            txt, ctrl = W.decode(d, st, st + len(b))
            print("   id %2d @0x%05X  %-70s %s" % (i, st, repr(txt)[:70], ctrl))
    # 1111 == 1110 / 1231 == 1230 im Nachrichtenblock?
    for a, b in (("1110", "1111"), ("1230", "1231")):
        da, db = rdt(a), rdt(b)
        ma, mb = msg_block(da), msg_block(db)
        gleich = [msg_bytes(da, s, e) == msg_bytes(db, s2, e2) for (i, s, e), (j, s2, e2) in zip(ma, mb)]
        print("ROOM%s vs ROOM%s: %d/%d Nachrichten bytegleich" % (a, b, sum(gleich), len(gleich)))

    print()
    print("== texte: Satzzeichen-Zensus ueber alle ausgelieferten RDTs ==")
    ende = collections.Counter()
    ende_beispiel = {}
    n_msg = 0
    punkt4 = []
    punkt3 = 0
    punkt3_seite = []
    punkt3_punkt = []
    for f in alle_rdts():
        d = open(f, "rb").read()
        room = os.path.basename(f)[4:8]
        for i, st, en in msg_block(d):
            b = msg_bytes(d, st, en)
            if not b or b[-2:-1] != b"\x01":
                continue
            n_msg += 1
            # letztes druckbares Byte vor 01 (Steuercodes mit Argument ueberspringen)
            k, letzt = 0, None
            body = b[:-2]
            while k < len(body):
                c = body[k]
                if c in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
                    k += 2; continue
                if c in (0x03, 0x07, 0x08):
                    letzt = ("ctrl%02x" % c); k += 1; continue
                letzt = c; k += 1
            key = letzt if isinstance(letzt, str) else ("%02x" % letzt if letzt is not None else "leer")
            ende[key] += 1
            ende_beispiel.setdefault(key, "ROOM%s id %d" % (room, i))
            # Punktfolgen
            s = hexs(body)
            if "57 57 57 57" in s:
                punkt4.append("ROOM%s id %d" % (room, i))
            k = 0
            while True:
                j = s.find("57 57 57", k)
                if j < 0:
                    break
                punkt3 += 1
                nach = s[j + 8:j + 14].strip()
                if nach.startswith("02"):
                    punkt3_seite.append("ROOM%s id %d" % (room, i))
                k = j + 9
    alle = sorted(glob.glob(os.path.join(PSX, "STAGE*", "ROOM*.RDT")))
    print("RDT-Dateien: %d, davon mit Kopf (>= 0x48 B): %d" % (len(alle), len(alle_rdts())))
    print("ausgewertete Nachrichten (mit 01-Ende): %d" % n_msg)
    for key, n in ende.most_common():
        g = W.glyph(int(key, 16)) if len(key) == 2 else None
        print("   letztes Zeichen %-7s %-4s x%-4d  z.B. %s" % (key, repr(g) if g else "", n, ende_beispiel[key]))
    print("Folgen 57 57 57 (\"...\"): %d, davon direkt vor Seitenumbruch 02: %d" % (punkt3, len(punkt3_seite)))
    print("   vor 02:", ", ".join(punkt3_seite[:12]), "..." if len(punkt3_seite) > 12 else "")
    print("Folgen 57 57 57 57 (vier Punkte): %d  %s" % (len(punkt4), punkt4[:12]))


# ------------------------------------------------------------------------------------------
NEU = {
    # Name: Glyphenfolge (ohne Kopf/Ende)
    "police_dead": bytes.fromhex("25 50 3a 4f 00 3d 00 4c 4b 48 45 3f 41 00 4b 42 42 45 3f 41 4e 18 00 44 41 3a 4f 00 40 41 3d 40 57"),
    "he_is_holding": bytes.fromhex("24 41 00 45 4f 00 44 4b 48 40 45 4a 43"),
    " something": bytes.fromhex("00 4f 4b 49 41 50 44 45 4a 43"),
    "miserable": bytes.fromhex("1d 00 49 45 4f 41 4e 3d 3e 48 41 00 40 41 3d 50 44 57 57 57"),
}


def teil_glyphen():
    print("== glyphen: erste Fundstellen je Baustein im Auslieferungsstand (Nachrichtenbloecke) ==")
    for name, g in NEU.items():
        funde = []
        for f in alle_rdts():
            d = open(f, "rb").read()
            room = os.path.basename(f)[4:8]
            for i, st, en in msg_block(d):
                b = msg_bytes(d, st, en)
                k = b.find(g)
                if k >= 0:
                    funde.append("ROOM%s @0x%05X (msg %d)" % (room, st + k, i))
        txt = "".join(W.glyph(c) or "?" for c in g)
        print("  %-14s %r  [%s]" % (name, txt, hexs(g)))
        print("      %d Fundstellen: %s" % (len(funde), "; ".join(funde[:6])))


# ------------------------------------------------------------------------------------------
def fontbreite():
    h = open(os.path.join(REPO, "re15_port", "include", "font_width.h"), encoding="latin-1").read()
    body = h[h.index("{") + 1:h.index("}")]
    return [int(x) for x in body.replace("\n", " ").split(",") if x.strip()]


def zeilen(b, fw):
    """Pixelbreite je Zeile (Umbruch 0x08 und Seitenwechsel 0x02 beginnen neu)."""
    out, cur, i = [], 0, 0
    while i < len(b):
        c = b[i]
        if c == 0x01:
            break
        if c == 0x02:
            out.append(cur); cur = 0; i += 2; continue
        if c in (0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            i += 2; continue
        if c in (0x03, 0x07):
            i += 1; continue
        if c == 0x08:
            out.append(cur); cur = 0; i += 1; continue
        cur += fw[c]; i += 1
    out.append(cur)
    return out


def teil_breite():
    fw = fontbreite()
    print("== breite: Zeilen des neuen Textes (Vorschub DEBUG.BIN[0x4416+code]) ==")
    kopf, ende, seite = b"\x04\x02", b"\x01\x00", b"\x02\x00"
    neu = {
        "1110 lang": kopf + NEU["police_dead"] + seite + NEU["he_is_holding"] + NEU[" something"] + b"\x57" + ende,
        "1110 kurz": kopf + NEU["police_dead"] + ende,
        "1230 lang": kopf + NEU["miserable"] + seite + NEU["he_is_holding"] + NEU[" something"] + b"\x57" + ende,
        "1230 kurz": kopf + NEU["miserable"] + ende,
    }
    for k, b in neu.items():
        print("  %-10s %s  Zeilen px %s" % (k, hexs(b), zeilen(b[2:], fw)))
    # Bestand
    alle = []
    for f in alle_rdts():
        d = open(f, "rb").read()
        for i, st, en in msg_block(d):
            b = msg_bytes(d, st, en)
            if b[:1] == b"\x04":
                b = b[2:]
            alle += zeilen(b, fw)
    alle = [x for x in alle if x > 0]
    alle.sort()
    n = len(alle)
    print("  Bestand: %d Zeilen, Median %d px, 99%%-Quantil %d px, max %d px" % (
        n, alle[n // 2], alle[int(n * 0.99)], alle[-1]))


# ------------------------------------------------------------------------------------------
def teil_msgref():
    print("== msgref: wer oeffnet die Leichen-Nachricht? ==")
    ziel = {"1110": 0, "1111": 0, "1230": 10, "1231": 10}
    for r in RAEUME:
        d = rdt(r)
        m = ziel[r]
        treffer = []
        for (tag, idx), ops in sorted(W.regionen(d).items()):
            for pc, op, sz in ops:
                if op == 0x2B and d[pc + 1] == m:
                    treffer.append("%s%02d @0x%05X Message_on %s" % (tag, idx, pc, hexs(d[pc:pc + 4])))
                if op == 0x2C and d[pc + 2] == 1:
                    pay = pc + (22 if d[pc + 3] & 0x80 else 14)
                    if d[pay] == m:
                        treffer.append("%s%02d @0x%05X Aot_set sce1 msg %d" % (tag, idx, pc, m))
                if op == 0x46 and d[pc + 2] == 1 and d[pc + 4] == m:
                    treffer.append("%s%02d @0x%05X Aot_reset sce1 msg %d" % (tag, idx, pc, m))
        print("  ROOM%s msg %d: %d Stelle(n): %s" % (r, m, len(treffer), "; ".join(treffer)))


# ------------------------------------------------------------------------------------------
def teil_slots():
    print("== slots: AOT-Slots (Aot_set/Door_aot_set/Item_aot_set/Aot_reset) je Raum ==")
    for r in RAEUME:
        d = rdt(r)
        belegt = collections.defaultdict(list)
        for (tag, idx), ops in sorted(W.regionen(d).items()):
            for pc, op, sz in ops:
                if op in (0x2C, 0x3B, 0x50, 0x46):
                    belegt[d[pc + 1]].append("%s%02d:%s" % (tag, idx, W.NAMES.get(op, "%02x" % op)))
        print("  ROOM%s: Slots %s" % (r, sorted(belegt)))
        frei = [s for s in range(48) if s not in belegt]
        print("           frei unter 48: %s" % frei)
    # Leichen-Satz + Ereignis bytegleich in der Variante?
    for a, b, sat, sub in (("1110", "1111", 0x0AEE, "sub02"), ("1230", "1231", 0x0D52, "sub21")):
        da, db = rdt(a), rdt(b)
        ra, rb = W.regionen(da), W.regionen(db)
        ka = ("sub", int(sub[3:]))
        ea = b"".join(da[pc:pc + sz] for pc, op, sz in ra.get(ka, []))
        eb = b"".join(db[pc:pc + sz] for pc, op, sz in rb.get(ka, []))
        print("  %s vs %s: %s bytegleich=%s (%d B)" % (a, b, sub, ea == eb and len(ea) > 0, len(ea)))
        # Aot_set der Leiche in der Variante an derselben Stelle?
        print("     Leichen-Aot_set @0x%05X: %s | %s" % (sat, hexs(da[sat:sat + 20]), hexs(db[sat:sat + 20])))


# ------------------------------------------------------------------------------------------
def teil_munition():
    print("== munition: RE1.5 Item_aot_set Typ 0x15 (H. Gun Bullets) ==")
    mengen = collections.Counter()
    liste = []
    alle_typ = collections.Counter()
    for f in alle_rdts():
        d = open(f, "rb").read()
        room = os.path.basename(f)[4:8]
        for (tag, idx), ops in sorted(W.regionen(d).items()):
            for pc, op, sz in ops:
                if op != 0x50:
                    continue
                p = pc + (22 if d[pc + 3] & 0x80 else 14)
                typ, menge, bit = W.u16(d, p), W.u16(d, p + 2), W.u16(d, p + 4)
                alle_typ[typ] += 1
                if typ == 0x15:
                    mengen[menge] += 1
                    liste.append("ROOM%s %s%02d @0x%05X slot %d x%d bit %d" % (room, tag, idx, pc, d[pc + 1], menge, bit))
    print("  %d Saetze Typ 0x15; Mengen: %s" % (sum(mengen.values()), dict(sorted(mengen.items()))))
    for s in liste:
        print("    " + s)
    # RE2
    print("== munition: RE2 Item_aot_set Id 0x14 (H. Gun Bullets) ==")
    sys.path.insert(0, os.path.join(REPO, "tools", "re2_sicherung"))
    cwd = os.getcwd(); os.chdir(REPO)
    import re2_scd_walk as R
    os.chdir(cwd)
    m2 = collections.Counter(); akt = collections.Counter(); l2 = []
    files = sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "room*", "scd", "*.scd")))
    desync = 0
    for f in files:
        room = os.path.basename(os.path.dirname(os.path.dirname(f)))
        ent = os.path.splitext(os.path.basename(f))[0]
        its, st = R.item_records(open(f, "rb").read())
        if st != "ok":
            desync += 1
        for it in its:
            if it["i_item"] == 0x14:
                m2[it["n_item"]] += 1
                akt[it["action"]] += 1
                l2.append("%s %s +0x%04X aot %d x%d flag %d md1 %d action %d" % (
                    room, ent, it["off"], it["aot"], it["n_item"], it["flag"], it["md1"], it["action"]))
    print("  %d SCD-Bloecke (%d desynchron); %d Saetze Id 0x14; Mengen: %s; action: %s" % (
        len(files), desync, sum(m2.values()), dict(sorted(m2.items())), dict(sorted(akt.items()))))
    for s in l2:
        print("    " + s)


# ------------------------------------------------------------------------------------------
def teil_bank9():
    print("== bank9: Zone-9-Bits im Auslieferungsstand ==")
    bits = collections.defaultdict(list)
    op59 = []
    for f in alle_rdts():
        d = open(f, "rb").read()
        room = os.path.basename(f)[4:8]
        for (tag, idx), ops in sorted(W.regionen(d).items()):
            for pc, op, sz in ops:
                if op == 0x50:
                    p = pc + (22 if d[pc + 3] & 0x80 else 14)
                    bits[W.u16(d, p + 4)].append("ROOM%s Item_aot_set" % room)
                elif op in (0x21, 0x22) and d[pc + 1] == 9:
                    bits[d[pc + 2]].append("ROOM%s %s" % (room, "Ck" if op == 0x21 else "Set"))
                elif op == 0x59 and d[pc + 1] == 9:
                    op59.append("ROOM%s %s%02d @0x%05X" % (room, tag, idx, pc))
    belegt = sorted(b for b in bits if b < 256)
    print("  belegt: %d Bits; 0x59 mit Bank 9: %d %s" % (len(belegt), len(op59), op59[:5]))
    frei = [b for b in range(256) if b not in bits]
    bloecke, start = [], None
    for b in range(257):
        if b < 256 and b in frei:
            if start is None: start = b
        else:
            if start is not None: bloecke.append((start, b - 1)); start = None
    print("  freie Bloecke (>=4): %s" % [x for x in bloecke if x[1] - x[0] >= 3])
    for b in (53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64):
        print("  Bit %d: %s" % (b, bits.get(b, "frei im Auslieferungsstand")))


def teil_re2leiche():
    """RE2: liegt ein Item-AOT auf demselben Rechteck wie ein Text-AOT (sce 4), dessen ENG-Text
    eine Leiche beschreibt? Dann ist das RE2s Vorbild "Leiche haelt ein Item".
    Text-AOT-Nutzlast (+14) = Nachrichten-Nr. (RE2 ROOM4050 @0x00F82 `2c 06 04 31 ... 08 00 00 00 ff ff`
    -> msg 8 = room4050/msg/sub08.msg "He's holding something. I don't need this right now.")."""
    sys.path.insert(0, os.path.join(REPO, "tools", "re2_sicherung"))
    cwd = os.getcwd(); os.chdir(REPO)
    import re2_scd_walk as R
    import msg_decode as M
    import re2_items as I
    os.chdir(cwd)
    base = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")
    worte = ("corpse", "body", "dead", "holding", "officer", "cop")
    print("== re2leiche: Item-AOT und Leichen-Text auf demselben Rechteck (RE2 Leon, alle Raeume) ==")
    for rdir in sorted(glob.glob(os.path.join(base, "room*"))):
        room = os.path.basename(rdir)
        items, texte = [], []
        for f in sorted(glob.glob(os.path.join(rdir, "scd", "*.scd"))):
            ent = os.path.splitext(os.path.basename(f))[0]
            buf = open(f, "rb").read()
            recs, st = R.walk(buf)
            for off, op, r in recs:
                if op == 0x4E:
                    rect = struct.unpack_from("<4h", r, 6)
                    items.append((ent, off, r[1], rect, struct.unpack_from("<H", r, 14)[0],
                                  struct.unpack_from("<H", r, 16)[0], r[20], r[21]))
                elif op == 0x2C and r[2] == 4 and not (r[3] & 0x80):
                    rect = struct.unpack_from("<4h", r, 6)
                    texte.append((ent, off, r[1], rect, r[14]))
        for (e1, o1, a1, rc1, iid, n, md1, act) in items:
            for (e2, o2, a2, rc2, mid) in texte:
                if rc1 != rc2:
                    continue
                mf = os.path.join(rdir, "msg", "sub%02d.msg" % mid)
                t = M.text(mf) if os.path.exists(mf) else "?"
                if not any(w in t.lower() for w in worte):
                    continue
                print("  %s: Item %s(0x%02X) x%d md1 %d action %d [%s +0x%04X aot %d]  ==  Text msg %d [%s +0x%04X aot %d]"
                      % (room, I.name(iid)[1] if iid < I.N_NAMES else "?", iid, n, md1, act, e1, o1, a1,
                         mid, e2, o2, a2))
                print("        Rechteck %s  Text: %s" % (rc1, t.replace("<FC>", " ").replace("<FD>", " | ")[:110]))


TEILE = {"re2leiche": teil_re2leiche, "texte": teil_texte, "glyphen": teil_glyphen, "breite": teil_breite, "msgref": teil_msgref,
         "slots": teil_slots, "munition": teil_munition, "bank9": teil_bank9}

if __name__ == "__main__":
    wahl = sys.argv[1:] or list(TEILE)
    for w in wahl:
        TEILE[w]()
        print()
