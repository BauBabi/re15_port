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

  (D) AUSGEGEBEN - der Raum fuehrt die Nachricht ueberhaupt aus: sein SCD enthaelt
      mindestens ein `Message_on <mid>` (Opcode 0x2B, pc[1] == mid; Handler @0x800404f4,
      `ori a1,zero,0x300` @0x80040500). Der TEXT allein genuegt NICHT - er steht im
      Nachrichtenblock, auch wenn kein Opcode ihn je aufmacht, und der Port haengt die
      Abfrage an op_message_on. Gemessen mit demselben Laengen-Vorschub wie (B).
      (Faellt damit raus: ROOM4001 msg 2 "You've used the Blue Master Keycard." - der
      Text ist da, der Raum sagt aber nur die Nachrichten 3..12; das ausgeloeste
      Message_on 2 gibt es nur in ROOM4000 sub02. Die frueher dafuer genannte
      Begruendung "liegt im mainScd" war falsch: im mainScd steht es auch nicht.)

Das ist der RE1.5-Ersatz fuer RE2s Zaehlerfeld: RE2 traegt die Zahl der Tueren im
Item_aot_set (Byte 16-17) und zaehlt sie beim Aufschliessen herunter (@0x80051810),
Abfrage erst bei Null (@0x80051824). RE1.5 hat kein solches Feld - die Zahl steckt
stattdessen in der Anzahl der Benutzungsstellen, und die ist hier ausgezaehlt.

  (E) CODE-PANEL-GATE - NUTZER-BEFUND 2026-09-22: "das Problem das ich Bei dir sah, war
      das du die discard Abfrage auch bei Toren mit Raetsel panels machst, also wo man
      einen Code eingeben muss. Da ist das natuerlich erst dann korrekt, wenn man den
      zugriffscode den man braucht einmalig richtig eingegeben hat."

      An den sechs Kartenlesern faellt die "used the"-Nachricht, BEVOR das Tastenfeld
      ueberhaupt aufgeht - sie heisst dort nicht "erledigt", sondern "die Karte steckt
      jetzt im Schlitz". ROOM10D0 sub20, unverzweigt hintereinander:
          @0x019C4  2b 09 ff ff   Message_on 9   "You've used the Blue Keycard."
          @0x019CA  04 ff 18 11   Evt_exec sub17 <- HIER erst geht das Tastenfeld auf
      Zwischen der Zeile und der ersten Ziffern-Aufforderung liegen 47 Bilder (gemessen,
      analysis/befunde_2026-09-22/discard-codepanel-was-markiert-den-code.md §0).

      DESHALB wird fuer solche Raeume die Stelle UMGEHAENGT - nicht auf die Einsteck-,
      sondern auf die Erfolgs-Nachricht des Panels - und zusaetzlich an das Erfolgs-Bit
      gebunden. Beides wird hier GEMESSEN, nicht je Raumnummer eingetragen:

      E1 PANEL-KENNUNG = das Rohbyte-Muster der Vier-Ziffern-Schranke
         `21 05 0d 01 21 05 0e 01 21 05 0f 01 21 05 10 01`
         = Ck(5,13..16,1), die vier Ziffern-Riegel. Gemessen ueber alle 240 Dateien:
         GENAU 6 Raeume, je 1x (10D0 @0x01516, 10D1 @0x01500, 11E0/11E1 @0x01A76,
         1230/1231 @0x00FDE). Ein siebter Raum mit diesem Muster wird automatisch
         mitgenommen - hier steht keine Raumnummer.
      E2 GATE-BIT = das `Ck` UNMITTELBAR VOR dem Muster: `21 03 32 00` @0x01512 =
         Ck(3,50,0) "Schloss noch zu". Bank 3 = DAT_800b0ff8 (Zeigertabelle
         PTR_DAT_80074664[3]) = bestaendiger Spielstand. Gegenprobe im selben Zug: das
         `Set` direkt hinter dem `Evt_exec` muss dasselbe Bit auf 1 setzen
         (@0x0152A `22 03 32 01`) - sonst ist das Muster nicht die Erfolgs-Schranke.
      E3 AUSLOESER-MSG = das erste `Message_on` im Sub des folgenden `Evt_exec`
         (@0x01526 `04 ff 18 13` -> sub19, erstes Message_on @0x0199E = msg 5
         "You've opened the lock."). `Evt_exec` startet nur einen Thread und gibt die
         Kontrolle NICHT ab (op_evt_exec, byte-true @0x8003f2b8) - `Set(3,50,1)` laeuft
         also vor dem ersten Opcode von sub19, das Bit steht bei msg 5 bereits.
      E4 DIE UMZUHAENGENDE STELLE = die (A)-(D)-Zeile, deren `Message_on` im LESER-Sub
         liegt. Leser-Sub = der Sub, der per `Evt_exec` den Sub startet, welcher das
         Panel scharf macht (`Set(5,2,1)`; Bank 5 = Panel-Arbeitsspeicher). Damit bleibt
         die Pliers-Stelle von ROOM11E0/11E1 (msg 12, eigener Sub) unberuehrt.
      E5 KOLLISIONS-RIEGEL ueber ALLE RDT, sonst wird die Zeile VERWORFEN (fail-closed):
         genau EIN Basisraum mit `Set(zone,bit,1)` und das muss der Panel-Raum selbst
         sein; NULL loeschende Schreiber `Set(zone,bit,!=1)`; NULL indizierte Schreiber
         (0x59, LAB_8003fe90 - Bit kommt aus work_vars, statisch nicht aufloesbar) in
         derselben Bank. Gemessen: flag(3,50) 1 Basisraum/0/0 OK, flag(3,137)
         1 Basisraum/0/0 OK, flag(3,139) **2 Basisraeume** (ROOM11Ex + ROOM124x
         @0x0055A, der Neuspiel-Startraum) -> ROOM11E0/11E1 msg 9 FLIEGEN RAUS.
         0x59 kommt im ganzen Bestand nur mit Bank 5 vor (ROOM1030/1031).

      Zu E5/ROOM1240: `Set(3,139,1)` @0x0055A steht ab dem ersten Bild eines neuen
      Spiels. Ein Gate `flag(3,139)==1` waere dort das SCHLIMMSTE - die Abfrage kaeme
      sofort. Und der Leser ist dann ohnehin tot: ROOM11E0 main00 @0x01576 nimmt den
      Else-Zweig und installiert @0x016C0 eine fertige Tuer statt des Kartenlesers.
      Die Yellow Keycard hat im ausgelieferten Stand also KEINE erreichbare
      Benutzungsstelle - eine Eigenschaft der Daten, nicht der Wegwerf-Abfrage.

Ausgabe je Eintrag: (room_id, msg_id, item_id, gate_zone, gate_bit). room_id ist die Zahl
aus dem Dateinamen (ROOM10D0 -> 0x10D0), also genau g_current_room_id. Beide
Spieler-Varianten stehen einzeln drin, weil es zwei getrennte RDT-Dateien sind.
gate_zone == 0 heisst: kein Gate (die 10 Stellen ohne Zifferncode).

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

# ---------------------------------------------------------------------------
# (E) Das Zifferncode-Panel eines Raums messen. Siehe E1..E4 im Kopf.
# ---------------------------------------------------------------------------
ZIFFERN_MUSTER = bytes.fromhex("21050d0121050e0121050f0121051001")   # Ck(5,13..16,1)
PANEL_SCHARF   = (5, 2, 1)     # Set(5,2,1) = Tastenfeld offen (ROOM10D0 sub17 @0x018F4)

def panel_messen(d, regs):
    """Liefert None oder das gemessene Panel eines Raums:
       {muster, gate_zone, gate_bit, gate_ck, erfolg_set, evt_exec, erfolg_sub,
        ausloeser_msg, ausloeser_pc, leser_sub, vorzeitig, taken}
       Alle Werte kommen aus den Bytes dieses Raums - keine Raumnummer, keine Tabelle."""
    # --- E1: die Vier-Ziffern-Schranke, opcode-exakt (nicht nur roh gesucht) ---
    fund = None
    for (tag, idx), ops in sorted(regs.items()):
        for k in range(len(ops) - 3):
            seq = ops[k:k + 4]
            if all(o == 0x21 and d[pc + 1] == 5 and d[pc + 2] == 13 + j and d[pc + 3] == 1
                   for j, (pc, o, sz) in enumerate(seq)):
                assert fund is None, "mehr als eine Vier-Ziffern-Schranke im Raum"
                fund = ((tag, idx), k, ops)
    if fund is None:
        return None
    (tag, idx), k, ops = fund
    assert d.startswith(ZIFFERN_MUSTER, ops[k][0]), \
        "opcode-exakter Fund deckt sich nicht mit dem Rohbyte-Muster @0x%05X" % ops[k][0]

    # --- E2: das Ck unmittelbar davor traegt das Gate-Bit ---
    assert k > 0 and ops[k - 1][1] == 0x21, "vor der Schranke steht kein Ck"
    gck = ops[k - 1][0]
    gate_zone, gate_bit, gate_val = d[gck + 1], d[gck + 2], d[gck + 3]
    assert gate_val == 0, "das Gate-Ck prueft nicht auf 0 (Schloss noch zu), sondern %d" % gate_val

    # --- E3: das folgende Evt_exec, sein Sub, dessen erstes Message_on ---
    ex = None
    for (pc, o, sz) in ops[k + 4:]:
        if o == 0x04:
            ex = pc; break
        if o not in (0x00, 0x21):            # geradliniger Block, sonst kein Fund
            break
    assert ex is not None and d[ex + 2] == 0x18, "kein `Evt_exec <Gosub sub>` hinter der Schranke"
    erfolg_sub = d[ex + 3]
    # Gegenprobe E2: direkt hinter dem Evt_exec setzt der Raum dasselbe Bit auf 1.
    nach = [o for o in ops if o[0] > ex]
    assert nach and nach[0][1] == 0x22 and d[nach[0][0] + 1] == gate_zone \
        and d[nach[0][0] + 2] == gate_bit and d[nach[0][0] + 3] == 1, \
        "hinter dem Evt_exec steht kein Set(%d,%d,1)" % (gate_zone, gate_bit)
    erfolg_set = nach[0][0]
    ziel = regs.get(("sub", erfolg_sub))
    assert ziel, "Erfolgs-Sub %d fehlt" % erfolg_sub
    ausl = [(pc, d[pc + 1]) for (pc, o, sz) in ziel if o == 0x2B]
    assert ausl, "der Erfolgs-Sub gibt keine Nachricht aus"
    ausloeser_pc, ausloeser_msg = ausl[0]

    # --- E4: der Leser-Sub = wer den Sub startet, der das Tastenfeld scharf macht ---
    scharf = set()
    for (t2, i2), o2 in regs.items():
        if t2 != "sub":
            continue
        for (pc, o, sz) in o2:
            if o == 0x22 and (d[pc + 1], d[pc + 2], d[pc + 3]) == PANEL_SCHARF:
                scharf.add(i2)
    leser = None
    for (t2, i2), o2 in sorted(regs.items()):
        for (pc, o, sz) in o2:
            if o == 0x04 and d[pc + 2] == 0x18 and d[pc + 3] in scharf:
                assert leser is None or leser == (t2, i2), "mehr als ein Leser-Sub"
                leser = (t2, i2)
    assert leser is not None, "kein Sub startet das Tastenfeld"
    vorzeitig = [(pc, d[pc + 1]) for (pc, o, sz) in regs[leser] if o == 0x2B]
    # Das Taken-Bit, das der Leser unmittelbar vor der Einsteck-Zeile prueft (nur Ausweis;
    # der Port benutzt statt dessen die echte Inventarsuche, @0x80051628).
    taken = {}
    for j, (pc, o, sz) in enumerate(regs[leser]):
        if o == 0x2B and j and regs[leser][j - 1][1] == 0x21:
            q = regs[leser][j - 1][0]
            if d[q + 1] == 9 and d[q + 3] == 1:
                taken[d[pc + 1]] = (d[q + 2], q)
    return dict(muster=ops[k][0], gate_zone=gate_zone, gate_bit=gate_bit, gate_ck=gck,
                erfolg_set=erfolg_set, evt_exec=ex, erfolg_sub=erfolg_sub,
                ausloeser_msg=ausloeser_msg, ausloeser_pc=ausloeser_pc,
                leser_sub=leser[1], vorzeitig=vorzeitig, taken=taken)


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "shared_assets", "PSX")
    nids, names = item_names()

    rdts = sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT")))
    platzierungen = collections.Counter()        # (B) Item_aot_set je Typ
    mengen        = collections.defaultdict(set)
    ausgegeben    = collections.defaultdict(set)  # (D) Message_on-Ids je Raum
    roh           = []                           # (room, room_id, msg_id, text)
    n_rdt = n_stub = 0
    panels        = {}                           # (E1-E4) room -> gemessenes Panel
    setz1         = collections.defaultdict(set)  # (E5) (zone,bit) -> {Basisraum}
    setz1_stellen = collections.defaultdict(list)
    setz_loesch   = collections.defaultdict(list) # (E5) Set(zone,bit,!=1)
    idx59         = collections.defaultdict(list) # (E5) 0x59-Schreiber je Bank
    n_roh_muster  = 0

    def basisraum(r): return r[:-1] + "x"

    for p in rdts:
        d = open(p, "rb").read()
        room = os.path.basename(p).split(".")[0]
        n_roh_muster += d.count(ZIFFERN_MUSTER)
        if len(d) < 0x48:
            n_stub += 1; continue
        n_rdt += 1
        room_id = int(room[4:], 16)
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        regs = {}
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d): continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                ops = walk_ops(d, sec + o, sec + e)
                regs[(tag, idx)] = ops
                for (pc, op, sz) in ops:
                    if op == 0x50:
                        lf = (d[pc+3] & 0x80) != 0
                        t  = d[pc+22] if lf else d[pc+14]
                        a  = d[pc+24] if lf else d[pc+16]
                        platzierungen[t] += 1; mengen[t].add(a)
                    elif op == 0x2B:                 # (D) Message_on <id>
                        ausgegeben[room].add(d[pc+1])
                    elif op == 0x22:                 # (E5) Set(zone,bit,val)
                        key = (d[pc+1], d[pc+2])
                        if d[pc+3] == 1:
                            setz1[key].add(basisraum(room))
                            setz1_stellen[key].append((room, pc))
                        else:
                            setz_loesch[key].append((room, pc, d[pc+3]))
                    elif op == 0x59:                 # (E5) indizierter Schreiber
                        idx59[d[pc+1]].append((room, pc))
        pa = panel_messen(d, regs)
        if pa is not None:
            pa["room"] = room; pa["room_id"] = room_id
            panels[room] = pa
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
    #
    # ⛔ VOLLSUCHE UEBER DEN NAMEN, nicht ueber die Zuordnung aus (A).
    # Frueher zaehlte (C) nur die Nachrichten, die (A) dem Gegenstand ZUGESCHLAGEN hatte.
    # (A) ordnet aber nach LAENGSTEM Treffer zu: eine Nachricht mit "Red Master Keycard"
    # geht an den Master, und "Red Keycard" sieht sie nie — obwohl ihr Name woertlich darin
    # steht. Haette ein Gegenstand zwei Benutzungsstellen, von denen eine so verschluckt
    # wird, zaehlte (C) eine und wuerde faelschlich eine Wegwerf-Abfrage erlauben, obwohl
    # der Gegenstand noch gebraucht wird. Deshalb wird jetzt fuer JEDEN Kandidaten der Name
    # in ALLEN "used the"-Nachrichten gesucht, unabhaengig davon, wem (A) sie zugeschlagen
    # hat. Die Regel kann dadurch nur noch STRENGER werden, nie lockerer.
    def basis(room): return room[:-1] + "x"
    stellen = collections.defaultdict(set)
    kandidaten_ids = sorted(set(k[3] for k in kandidaten))
    for iid in kandidaten_ids:
        nm = names[iid]
        for (room, room_id, mid, txt) in roh:
            if len(nm) >= 4 and nm in txt:
                stellen[iid].add((basis(room), mid))

    gewaehlt, verworfen = [], []
    for (room, room_id, mid, iid, nm, txt) in kandidaten:
        if platzierungen[iid] == 0:
            verworfen.append((room, mid, iid, nm, "B: keine Item_aot_set-Ausgabe")); continue
        if len(stellen[iid]) != 1:
            verworfen.append((room, mid, iid, nm,
                              "C: %d Benutzungsstellen" % len(stellen[iid]))); continue
        if mid not in ausgegeben[room]:
            verworfen.append((room, mid, iid, nm,
                              "D: kein Message_on %d im SCD (nur %s)"
                              % (mid, sorted(ausgegeben[room])))); continue
        gewaehlt.append((room, room_id, mid, iid, nm))
    gewaehlt.sort(key=lambda r: (r[1], r[2]))

    # ---- (E) CODE-PANEL-GATE: umhaengen oder verwerfen -------------------------------
    assert n_roh_muster == len(panels), \
        "Rohbyte-Muster %dx, opcode-exakt aber %d Raeume" % (n_roh_muster, len(panels))
    panel_ok, panel_tot = {}, {}
    for room, pa in sorted(panels.items()):
        z, b = pa["gate_zone"], pa["gate_bit"]
        eigen, fremd = basisraum(room), sorted(setz1[(z, b)] - {basisraum(room)})
        grund = None
        if fremd:
            grund = ("E5: flag(%d,%d) wird auch ausserhalb gesetzt (%s) - kein Marker "
                     "dieses Panels" % (z, b, ", ".join("%s @0x%05X" % (r, o)
                     for r, o in setz1_stellen[(z, b)] if basisraum(r) != eigen)))
        elif eigen not in setz1[(z, b)]:
            grund = "E5: flag(%d,%d) wird nirgends auf 1 gesetzt" % (z, b)
        elif setz_loesch[(z, b)]:
            grund = ("E5: flag(%d,%d) wird geloescht (%s) - nicht einmalig" % (z, b,
                     ", ".join("%s @0x%05X val=%d" % t for t in setz_loesch[(z, b)])))
        elif idx59[z]:
            grund = ("E5: Bank %d hat indizierte Schreiber 0x59 (%s) - statisch nicht "
                     "aufloesbar" % (z, ", ".join("%s @0x%05X" % t for t in idx59[z])))
        (panel_tot if grund else panel_ok)[room] = dict(pa, grund=grund)

    ad_zeilen = list(gewaehlt)          # die (A)-(D)-Zeilen VOR dem Umhaengen
    umgehaengt, gewaehlt2 = [], []
    for (room, room_id, mid, iid, nm) in gewaehlt:
        pa = panels.get(room)
        if pa is None or mid not in [m for _, m in pa["vorzeitig"]]:
            gewaehlt2.append((room, room_id, mid, iid, nm, 0, 0))   # kein Gate
            continue
        if room in panel_tot:
            verworfen.append((room, mid, iid, nm, panel_tot[room]["grund"]))
            continue
        neu = pa["ausloeser_msg"]
        assert neu in ausgegeben[room], "Ausloeser-msg %d wird im Raum nicht ausgegeben" % neu
        gewaehlt2.append((room, room_id, neu, iid, nm, pa["gate_zone"], pa["gate_bit"]))
        umgehaengt.append((room, mid, neu, iid, nm, pa))
    gewaehlt = sorted(gewaehlt2, key=lambda r: (r[1], r[2]))
    doppelt = [k for k, c in collections.Counter((g[1], g[2]) for g in gewaehlt).items() if c > 1]
    assert not doppelt, "doppelte (Raum, Nachricht) nach dem Umhaengen: %s" % doppelt

    ids = sorted(set(g[3] for g in gewaehlt))
    L = []
    w = L.append
    w("/* ERZEUGT von tools/gen_discard_sites.py - NICHT von Hand aendern.")
    w(" *")
    w(" * Die Benutzungsstellen der Schluessel-Gegenstaende, aus den ausgelieferten Daten")
    w(" * ABGELEITET (Aufnahmebedingungen A/B/C/D/E siehe Generator-Kopf), nicht gewaehlt.")
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
    w(" *")
    w(" * (E) ZIFFERNCODE-PANELS: %d Raeume tragen das Muster `21 05 0d 01 ... 21 05 10 01`"
      % len(panels))
    w(" *     (Ck(5,13..16,1), die vier Ziffern-Riegel). Bei ihnen faellt die")
    w(" *     \"used the\"-Zeile, BEVOR das Tastenfeld aufgeht - deshalb umgehaengt:")
    for (room, alt, neu, iid, nm, pa) in umgehaengt:
        w(" *     %s msg %d -> msg %d  (Ausloeser @0x%05X, Erfolgs-Sub %d), Gate"
          " flag(%d,%d)==1 (`Set` @0x%05X)"
          % (room, alt, neu, pa["ausloeser_pc"], pa["erfolg_sub"],
             pa["gate_zone"], pa["gate_bit"], pa["erfolg_set"]))
    for room, pa in sorted(panel_tot.items()):
        w(" *     %s GESTRICHEN: %s" % (room, pa["grund"]))
    w(" */")
    w("")
    w("typedef struct { uint16_t room; uint8_t msg; uint8_t item;")
    w("                 uint8_t gate_zone; uint8_t gate_bit; } re15_discard_site_t;")
    w("")
    w("static const re15_discard_site_t re15_discard_sites[] = {")
    for (room, room_id, mid, iid, nm, gz, gb) in gewaehlt:
        pa = panel_ok.get(room)
        wie = ("\"You've opened the lock.\" nach flag(%d,%d) @0x%05X" % (gz, gb, pa["erfolg_set"])
               if gz else "\"You've used the %s.\"" % nm)
        w("    { 0x%04X, %2d, 0x%02X, %d, %3d },   /* %s: %s */"
          % (room_id, mid, iid, gz, gb, room, wie))
    w("};")
    w("#define RE15_DISCARD_SITE_COUNT %d" % len(gewaehlt))
    w("")
    w("/* NUR FUER DEN RIEGEL (tests/unit/r21_discard_wegwerfen.c TEIL R): die gemessenen")
    w(" * Zifferncode-Panels mitsamt der VORZEITIGEN Zeile, die frueher die Abfrage")
    w(" * ausloeste, dem Leser-Sub und dem Taken-Bit, das der Leser prueft. Der Motor")
    w(" * benutzt diese Tabelle NICHT - sie erlaubt dem Riegel, die alte Fehlerstelle")
    w(" * anzufahren, ohne eine Raumnummer in den Test zu schreiben.")
    w(" * `tot` = 1: Panel vom Kollisions-Riegel E5 verworfen, KEINE Wegwerf-Stelle -")
    w(" * gate_zone/gate_bit stehen dort trotzdem (das GEMESSENE Erfolgs-Bit), damit der")
    w(" * Riegel den ausgelieferten Startzustand nachstellen kann (flag(3,139) steht ab")
    w(" * Bild 0, ROOM1240 @0x0055A). */")
    w("typedef struct { uint16_t room; uint8_t leser_sub; uint8_t vorzeitig_msg;")
    w("                 uint8_t erfolg_sub; uint8_t ausloeser_msg; uint8_t item;")
    w("                 uint8_t gate_zone; uint8_t gate_bit;")
    w("                 uint8_t taken_bit; uint8_t tot; } re15_discard_panel_t;")
    w("")
    w("static const re15_discard_panel_t re15_discard_panels[] = {")
    npanel = 0
    for room, pa in sorted(panels.items()):
        # Die vorzeitige Zeile: die (A)-(D)-Nachricht dieses Raums, die im Leser-Sub liegt.
        vm = [m for (_, m) in pa["vorzeitig"]
              if any(z[0] == room and z[2] == m for z in ad_zeilen)]
        if not vm:
            continue
        item = [z[3] for z in ad_zeilen if z[0] == room and z[2] == vm[0]][0]
        tot = 1 if room in panel_tot else 0
        w("    { 0x%04X, %2d, %2d, %2d, %2d, 0x%02X, %d, %3d, %3d, %d },   /* %s%s */"
          % (pa["room_id"], pa["leser_sub"], vm[0], pa["erfolg_sub"], pa["ausloeser_msg"],
             item, pa["gate_zone"], pa["gate_bit"],
             pa["taken"].get(vm[0], (0, 0))[0], tot, room,
             " - GESTRICHEN (E5)" if tot else ""))
        npanel += 1
    w("};")
    w("#define RE15_DISCARD_PANEL_COUNT %d" % npanel)
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
