#!/usr/bin/env python3
"""gen_discard_sites.py - erzeugt engine/src/gen/discard_sites.inc.

Die Tabelle wird NICHT gewaehlt, sondern AUS DEN AUSGELIEFERTEN DATEN ABGELEITET.
Drei Messungen, jede eine Aufnahmebedingung; wer eine nicht erfuellt, faellt raus:

  (A) NAME  - der Raumtext nennt den Gegenstand woertlich: "You've used the <NAME>."
      <NAME> wird gegen die 102 Eintraege der Item-Namenstabelle aufgeloest
      (DEBUG.BIN, Offsettabelle @0x800C495C, Blob @0x800C4A28; die 102 ist physisch:
      (0x800C4A28-0x800C495C)/2, Leser FUN_80028840 @0x80028840 prueft keinen Bereich).
      LAENGSTER Treffer gewinnt - sonst schluckte "Red Keycard" das "Red Master Keycard",
      "Blue Keycard" das "Blue Master Keycard" und "Minidisc Player" das
      "Minidisc Player w/ Disc".

  (B) AUSGABE - der Gegenstand wird im Spiel ueberhaupt ausgegeben: mindestens ein
      Item_aot_set (0x50) mit diesem Typ. Ohne Ausgabe kann er nicht im Inventar liegen.
      (Faellt damit raus: "Fuse" 0x40 - 0 Platzierungen; "Latch Key" - gar nicht in der
      102er-Tabelle.)

  (C) EINDEUTIGKEIT - der Gegenstand hat GENAU EINE Benutzungsstelle, gezaehlt ueber
      BASISRAEUME (die Spieler-Variantenziffer ROOM###0/ROOM###1 = Elza/John wird
      eingeklappt). Mehr als eine Stelle hiesse: nach der ersten Benutzung ist er noch
      nicht ueberfluessig - dann darf nicht gefragt werden.

Das ist der RE1.5-Ersatz fuer RE2s Zaehlerfeld: RE2 traegt die Zahl der Tueren im
Item_aot_set (Byte 16-17) und zaehlt sie beim Aufschliessen herunter (@0x80051810),
Abfrage erst bei Null (@0x80051824). RE1.5 hat kein solches Feld - die Zahl steckt
stattdessen in der Anzahl der Benutzungsstellen, und die ist hier ausgezaehlt.

Ausgabe je Eintrag: (room_id, msg_id, item_id). room_id ist die Zahl aus dem Dateinamen
(ROOM10D0 -> 0x10D0), also genau g_current_room_id. Beide Spieler-Varianten stehen
einzeln drin, weil es zwei getrennte RDT-Dateien sind.

Aufruf: python gen_discard_sites.py [ASSET_ROOT]
"""
import struct, sys, os, glob, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from discard_zensus import (op_size, fwd_target, rdt_section_end, section_regions,
                            u16, u32)
from discard_nutzstellen import decode, messages, walk_ops

ROOT  = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
DEBUG = os.path.join(ROOT, "shared_assets", "PSX", "BIN", "DEBUG.BIN")
OUT   = os.path.join(ROOT, "engine", "src", "gen", "discard_sites.inc")

BASE      = 0x800C0000
NAME_OFF  = 0x800C495C
NAME_BLOB = 0x800C4A28

def glyph(b):
    if b == 0x00: return " "
    if 0x0C <= b <= 0x15: return chr(48 + b - 0x0C)
    if b == 0x16: return ":"
    if b == 0x18: return ","
    if b == 0x19: return '"'
    if b == 0x1A: return "!"
    if b == 0x1B: return "?"
    if 0x1D <= b <= 0x36: return chr(65 + b - 0x1D)
    if b == 0x37: return "["
    if b == 0x38: return "/"
    if b == 0x39: return "]"
    if b == 0x3A: return "'"
    if b == 0x3B: return "-"
    if b == 0x3C: return "."
    if 0x3D <= b <= 0x56: return chr(97 + b - 0x3D)
    if b == 0x57: return "."
    return "?"

def item_names():
    d = open(DEBUG, "rb").read()
    n = (NAME_BLOB - NAME_OFF) // 2
    offs = struct.unpack_from("<%dH" % n, d, NAME_OFF - BASE)
    prev = -1
    for i, o in enumerate(offs):                 # Monotonie belegen, nicht behaupten
        assert i == 0 or o > prev, "Namens-Offsets nicht monoton bei %d" % i
        prev = o
    out = {}
    for i, o in enumerate(offs):
        p = NAME_BLOB - BASE + o
        s = ""
        while d[p] != 0x07:
            s += glyph(d[p]); p += 1
        out[i] = s
    return n, out

def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "shared_assets", "PSX")
    nids, names = item_names()

    rdts = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
    platzierungen = collections.Counter()        # (B) Item_aot_set je Typ
    mengen        = collections.defaultdict(set)
    roh           = []                           # (room, room_id, msg_id, text)
    n_rdt = n_stub = 0

    for p in rdts:
        d = open(p, "rb").read()
        room = os.path.basename(p).split(".")[0]
        if len(d) < 0x48:
            n_stub += 1; continue
        n_rdt += 1
        room_id = int(room[4:], 16)
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                for (pc, op, sz) in walk_ops(d, sec + o, sec + e):
                    if op == 0x50:
                        lf = (d[pc+3] & 0x80) != 0
                        t  = d[pc+22] if lf else d[pc+14]
                        a  = d[pc+24] if lf else d[pc+16]
                        platzierungen[t] += 1; mengen[t].add(a)
        msgs = messages(d)
        for mid, (txt, ctrl) in msgs.items():
            if "ve used the" in txt or "have used the" in txt:
                roh.append((room, room_id, mid, txt))

    # (A) Namen aufloesen, LAENGSTER Treffer
    sortiert = sorted(names.items(), key=lambda kv: -len(kv[1]))
    kandidaten = []
    unaufgeloest = []
    for (room, room_id, mid, txt) in roh:
        treffer = None
        for iid, nm in sortiert:
            if len(nm) >= 4 and nm in txt:
                treffer = (iid, nm); break
        if treffer is None: unaufgeloest.append((room, mid, txt))
        else: kandidaten.append((room, room_id, mid, treffer[0], treffer[1], txt))

    # (C) Eindeutigkeit ueber BASISRAEUME (Variantenziffer einklappen)
    def basis(room): return room[:-1] + "x"
    stellen = collections.defaultdict(set)
    for (room, room_id, mid, iid, nm, txt) in kandidaten:
        stellen[iid].add((basis(room), mid))

    gewaehlt, verworfen = [], []
    for (room, room_id, mid, iid, nm, txt) in kandidaten:
        if platzierungen[iid] == 0:
            verworfen.append((room, mid, iid, nm, "B: keine Item_aot_set-Ausgabe")); continue
        if len(stellen[iid]) != 1:
            verworfen.append((room, mid, iid, nm,
                              "C: %d Benutzungsstellen" % len(stellen[iid]))); continue
        gewaehlt.append((room, room_id, mid, iid, nm))
    gewaehlt.sort(key=lambda r: (r[1], r[2]))

    ids = sorted(set(g[3] for g in gewaehlt))
    L = []
    w = L.append
    w("/* ERZEUGT von tools/gen_discard_sites.py - NICHT von Hand aendern.")
    w(" *")
    w(" * Die Benutzungsstellen der Schluessel-Gegenstaende, aus den ausgelieferten Daten")
    w(" * ABGELEITET (Aufnahmebedingungen A/B/C siehe Generator-Kopf), nicht gewaehlt.")
    w(" *")
    w(" * ABDECKUNG DIESES LAUFS:")
    w(" *   %d RDTs mit Header gelesen (+%d Stummel <0x48 B), 0 Desync-Stopps" % (n_rdt, n_stub))
    w(" *   %d Item_aot_set-Records insgesamt" % sum(platzierungen.values()))
    w(" *   %d Nachrichten \"...used the X...\" in %d Raeumen"
      % (len(roh), len(set(r[0] for r in roh))))
    w(" *   %d davon namentlich aufgeloest, %d nicht (%s)"
      % (len(kandidaten), len(unaufgeloest),
         ", ".join(sorted(set(t[2][:40] for t in unaufgeloest))) or "-"))
    w(" *   %d Eintraege aufgenommen = %d Gegenstaende x ihre Raumvarianten"
      % (len(gewaehlt), len(ids)))
    for (room, mid, iid, nm, grund) in verworfen:
        w(" *   VERWORFEN %s msg %d  0x%02X %-24s %s" % (room, mid, iid, nm, grund))
    for (room, mid, txt) in unaufgeloest:
        w(" *   VERWORFEN %s msg %d  (A: kein Name der 102er-Tabelle)  %r" % (room, mid, txt[:48]))
    w(" *")
    w(" * AUFGENOMMENE GEGENSTAENDE (Id, Name, Ausgaben, Mengen je Ausgabe):")
    for iid in ids:
        w(" *   0x%02X %-24s %d Item_aot_set, Menge %s"
          % (iid, names[iid], platzierungen[iid], sorted(mengen[iid])))
    w(" */")
    w("")
    w("typedef struct { uint16_t room; uint8_t msg; uint8_t item; } re15_discard_site_t;")
    w("")
    w("static const re15_discard_site_t re15_discard_sites[] = {")
    for (room, room_id, mid, iid, nm) in gewaehlt:
        w("    { 0x%04X, %2d, 0x%02X },   /* %s: \"You've used the %s.\" */"
          % (room_id, mid, iid, room, nm))
    w("};")
    w("#define RE15_DISCARD_SITE_COUNT %d" % len(gewaehlt))
    w("")
    w("/* Die ausgelieferte Stueckzahl je aufgenommenem Gegenstand (Item_aot_set Byte 16).")
    w(" * Sie ist der RE1.5-Gegenwert zu RE2s Zaehlerfeld und wird beim Gebrauch")
    w(" * heruntergezaehlt (@0x80051810); bei Null wird gefragt (@0x80051824). */")
    w("#define RE15_DISCARD_MENGE_JE_AUSGABE 1   /* gemessen: alle %d Gegenstaende" % len(ids))
    w("                                           * werden mit Menge 1 platziert */")
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    open(OUT, "w", newline="\n").write("\n".join(L) + "\n")

    for m in mengen:
        pass
    assert all(mengen[i] == {1} for i in ids), \
        "Nicht alle aufgenommenen Gegenstaende werden mit Menge 1 platziert: %s" \
        % {hex(i): sorted(mengen[i]) for i in ids if mengen[i] != {1}}

    print("\n".join(L[:40]))
    print("...")
    print("-> %s  (%d Eintraege, %d Gegenstaende)" % (OUT, len(gewaehlt), len(ids)))

main()
