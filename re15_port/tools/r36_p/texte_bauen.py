#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""texte_bauen.py - Runde 36 Spur P "story": die neuen/umgeschriebenen Nachrichten als .msg-Rohbytes.

Erzeugt re15_port/engine/src/gen/story_r36_texte.inc (Tabelle fuer engine/src/story_r36.c).

FORM (belegt, nicht geraten):
  * Glyphen = Umkehrung der Port-Tabelle engine/src/msg_common.c re15_msg_glyph (dieselbe Tabelle wie
    tools/r35_l/texte_bauen.py): ' '=00, ':'=16, ','=18, '!'=1A, '?'=1B, A-Z=1D.., a-z=3D.., "'"=3A,
    '-'=3B, '.'=57 (die Originale schreiben den Punkt als 0x57: "RPD." ROOM30C1 @0x00F07 `2e 2c 20 57`,
    "..." = 57 57 57 ROOM1211 msg 10 @0x0259D+0x37).
  * UMSCHREIBEN (P4): Kopf und Schwanz werden WOERTLICH aus der Original-Nachricht DESSELBEN Raums
    und DERSELBEN Id uebernommen — Kopf = alles bis einschliesslich `16 05 00 00` (`04 00 05 <Farbe>
    <Sprecher> : <Farbe 00> <Leerzeichen>`), Schwanz = `04 01 01 63`. Nur der Satz dazwischen ist neu.
    Das Werkzeug bricht ab, wenn eine Vorlage diese Form nicht hat.
  * NEU (P2/P3, Id 20 in Raeumen ohne Nachrichtenblock): Untersuchungstext `04 02 <Seite 1> 02 00
    <Seite 2> 01 00` = Form von ROOM1211 msg 10 @0x0259D (`04 02 ... 57 02 00 25 50 ... 57 57 57 01 00`).
  * Zeilenumbruch 0x08 (FUN_80028868); Zeilenbreite aus include/font_width.h (DEBUG.BIN[0x4416+code]);
    jede Zeile <= 271 px (99 % der 2286 ausgelieferten Zeilen, tuer1120_1130.c). Zeile 1 einer
    Dialognachricht zaehlt den Sprecher-Kopf mit.
  * Laenge <= 128 B (PSX MSG_RAW_LEN, msg_common.c).
  * UNTERSUCHEN (Runde 37 Spur F, ROOM50B1 msg 0): einen Untersuchungstext DERSELBEN Id umschreiben.
    Kopf `04 02`, Seitentrenner `02 00` und Schwanz `01 00` WOERTLICH aus der Original-Nachricht;
    Seiten- und Zeilenzahl muessen der Vorlage gleichen ("Zeilenumbrueche wie das Original"), sonst Abbruch.

WORTLAUT = PORT-WAHL (Story, Nachtrag AUFTRAG.md 14:00, P4: "Die duerfen sich niemals begegnet sein");
alt -> neu steht im Dossier analysis/befunde_runde36/P_story.md zur Freigabe.

Aufruf: python re15_port/tools/r36_p/texte_bauen.py          (schreibt die .inc)
        python re15_port/tools/r36_p/texte_bauen.py --check  (prueft nur, schreibt nichts)
"""
import os, re, struct, sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(os.path.dirname(HERE))                       # re15_port
PSX = os.path.join(PORT, "shared_assets", "PSX")
FW = os.path.join(PORT, "include", "font_width.h")
OUT = os.path.join(PORT, "engine", "src", "gen", "story_r36_texte.inc")

ENC = {" ": 0x00, ":": 0x16, ",": 0x18, '"': 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10):
    ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i
DEC = {v: k for k, v in ENC.items()}
DEC[0x3C] = "."

MAX_PX = 271
MAX_LEN = 128


def widths():
    src = open(FW, encoding="utf-8", errors="replace").read()
    body = src.split("{", 1)[1].split("}", 1)[0]
    nums = [int(x, 0) for x in re.findall(r"\b(0x[0-9a-fA-F]+|\d+)\b", body)]
    assert len(nums) == 256, len(nums)
    return nums


WID = widths()


def enc(text):
    try:
        return bytes(ENC[c] for c in text)
    except KeyError as e:
        raise SystemExit("Zeichen ohne Glyphe: %r in %r" % (e.args[0], text))


def px(bs):
    return sum(WID[b] for b in bs)


def rdt(room):
    st = int(room[0], 16)
    return open(os.path.join(PSX, "STAGE%X" % st, "ROOM%s.RDT" % room), "rb").read()


def msg_raw(room, mid):
    """Original-Nachricht (Offset, Bytes bis einschliesslich `01 xx`) — RDT+0x3C wie
    re15_msg_load_room_block (msg_common.c): off[0]/2 Eintraege, jeder ein Nachrichtenanfang."""
    d = rdt(room)
    ms = struct.unpack_from("<I", d, 0x3C)[0]
    assert ms, "ROOM%s hat keinen Nachrichtenblock" % room
    n = (d[ms] | d[ms + 1] << 8) // 2
    assert mid < n, "ROOM%s hat nur %d Nachrichten" % (room, n)
    s = ms + (d[ms + 2 * mid] | d[ms + 2 * mid + 1] << 8)
    j = s
    while True:
        b = d[j]
        if b == 0x01:
            j += 2
            break
        j += 2 if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B) else 1
    return s, d[s:j]


def decode(bs):
    """Wie msg_common.c re15_msg_decode_text: 0x08 -> ' ', Codes mit Argument ueberspringen."""
    out, i = "", 0
    while i < len(bs):
        b = bs[i]
        if b == 0x01:
            break
        if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            i += 2
            continue
        if b == 0x03:
            break
        if b == 0x07:
            i += 1
            continue
        if b == 0x08:
            out += " "
            i += 1
            continue
        out += DEC.get(b, "")
        i += 1
    return out.rstrip(" ")


def split_dialog(orig):
    """Kopf (bis inkl. `16 05 00 00`) und Schwanz (`04 01 01 63`) der Original-Dialogzeile."""
    k = orig.find(bytes([0x16, 0x05, 0x00, 0x00]))
    assert orig[:3] == bytes([0x04, 0x00, 0x05]) and k > 4, orig.hex(" ")
    assert orig[-4:] == bytes([0x04, 0x01, 0x01, 0x63]), orig.hex(" ")
    return orig[:k + 4], orig[-4:]


def kopf_px(kopf):
    """Sichtbare Breite des Kopfs: Sprecherglyphen + ':' + Leerzeichen (05 xx zeichnet nicht)."""
    sicht, i = [], 0
    while i < len(kopf):
        b = kopf[i]
        if b in (0x04, 0x05):
            i += 2
            continue
        sicht.append(b)
        i += 1
    return px(sicht)


# (Raum, Id, C-Name, neue Zeilen, Grund) — P4 UMSCHREIBEN (Kopf/Schwanz = Original derselben Id)
UMSCHREIBEN = [
    ("30C1", 3, "k_s36_30c1_m3", ["Leon? I don't know him.", "I never ran into him at the RPD."],
     "Elza fragte nach Leon und sprach von einer Trennung im RPD = Bekanntschaft"),
    ("30C1", 4, "k_s36_30c1_m4", ["He's a rookie, and he's helping", "me and Ada escape the city."],
     "Antwort auf 'How is he?' setzte Elzas Sorge um Leon voraus"),
    ("30C1", 7, "k_s36_30c1_m7", ["Will you tell them that me and John", "and Sherry are here and we're all right?"],
     "'tell him ... we're all right' = persoenliche Nachricht an einen Bekannten"),
    ("6030", 1, "k_s36_6030_m1", ["Is anyone else with you?"],
     "Leon fragte nach Elza beim Namen"),
    ("6030", 2, "k_s36_6030_m2", ["Elza is out there fighting", "the monsters!"],
     "Sherry nennt Elza jetzt als Fremde beim Namen (Antwort auf die neue Frage)"),
    ("6030", 12, "k_s36_6030_m12", ["I've never met her.", "Can you start this thing?"],
     "Annette (sub08, Zug vorne, Eintritt Cut 0) kann VOR der Wagen-Szene (sub02) kommen: Spawn aus 6000 "
     "(-17970,-20380) liegt oestlich der Marke Slot 5 x[-19730..-18130], die Tuer nach vorne Slot 1 bei x -9300 "
     "- dann wusste Leon ohne Sherry, was Elza tut = Bekanntschaft"),
    ("6031", 0, "k_s36_6031_m0", ["Sherry! Is anyone else here?"],
     "Elza fragte nach Leon"),
    ("6031", 1, "k_s36_6031_m1", ["A policeman! He's fighting off", "monsters. He wants us to start the train."],
     "'He wants you to start the train' = Leon kennt Elza"),
    ("50B0", 1, "k_s36_50b0_m1", ["Some other survivors are", "down there with her too."],
     "'Your other friends' = Leon kennt die Ueberlebenden am Zug (Elza/John/Sherry)"),
    ("6000", 0, "k_s36_6000_m0", ["You must be that cop.", "Your friend is already here."],
     "'Your friends' = alle am Zug als Leons Freunde (Freund = Marvin, 50B0 msg 0)"),
    ("6000", 3, "k_s36_6000_m3", ["Come on. Let's join the others", "and get out of here."],
     "'Let's join your friends' wie msg 0"),
]

# (Raum, Id, C-Name, Seiten (je Liste von Zeilen), Grund) — P2/P3 NEU, Untersuchungstext
NEU = [
    ("1210", 20, "k_s36_1210_m20",
     [["The cell door is open..."], ["Whoever was locked up here", "is gone."]],
     "P2 leere Zelle (Wortlaut-Anfang = Zitat des Nutzers)"),
    ("1091", 20, "k_s36_1091_m20",
     [["It's a burnt-out shed."], ["Someone must have put out", "the fire."]],
     "P3 'burnt-out shed' (Wortlaut = Stichwort des Nutzers)"),
]


# (Raum, Id, C-Name, Seiten (je Liste von Zeilen), Grund) — Runde 37 Spur F: Untersuchungstext UMSCHREIBEN
# (Form `04 02 <Seite 1> 02 00 <Seite 2> 01 00` wie die Vorlage DERSELBEN Id; Kopf/Trenner/Schwanz = Original-Bytes).
# Wortlaut = PORT-WAHL auf NUTZER-VORGABE (analysis/befunde_runde37/AUFTRAG.md #5 "Bezug zu ada bei elza kann raus"):
# nur der Name weicht einer Beschreibung — RE2-Form fuer Tote, die der Spieler nicht kennt (RE2 Leon, englischer
# Block RDT+0x40: ROOM60A0 msg 1 @0x01982 "A researcher's corpse. / It looks like he poured / oil all over the
# place...", ROOM2020 msg 3 @0x02FA4 "It's a horrible corpse.", ROOM4090 msg 34 @0x04139 "The corpse of a sewer /
# maintenance worker."); "She's dead." bleibt der RE1.5-Satz (auch ROOM50B0 msg 18 @0x00FA4).
UNTERSUCHEN = [
    ("50B1", 0, "k_s37_50b1_m0",
     [["It's a woman. She's dead."], ["It looks like one of those", "gorillas killed her."]],
     "Runde 37 #5 + Nachtrag 8 (NUTZER-VORGABE): Elza findet nur die Leiche - Ada kennt sie nur dem Namen nach "
     "(Marvin, 30C1 msg 4/6), erkennen kann sie sie nicht: 'Dann ist es halt nur - a woman.'"),
]


def bauen():
    eintraege = []
    fehler = []
    for room, mid, name, zeilen, grund in UMSCHREIBEN:
        off, orig = msg_raw(room, mid)
        kopf, schwanz = split_dialog(orig)
        body = b"\x08".join(enc(z) for z in zeilen)
        neu = kopf + body + schwanz
        breiten = [kopf_px(kopf) + px(enc(zeilen[0]))] + [px(enc(z)) for z in zeilen[1:]]
        # Ausnahme: eine Zeile, die BYTEGLEICH als Zeile der Original-Nachricht ausgeliefert wird
        # (das Original zeichnet sie so), zaehlt nicht gegen die Grenze.
        orig_zeilen = orig[len(kopf):-4].split(b"")
        for i, z in enumerate(zeilen):
            if breiten[i] > MAX_PX and not (i > 0 and enc(z) in orig_zeilen):
                fehler.append("%s msg %d: Zeile %d = %d px > %d" % (room, mid, i + 1, breiten[i], MAX_PX))
        if len(neu) > MAX_LEN:
            fehler.append("%s msg %d: %d B > %d" % (room, mid, len(neu), MAX_LEN))
        eintraege.append(dict(room=room, mid=mid, name=name, bytes=neu, orig=orig, off=off,
                              alt=decode(orig), neu=decode(neu), breiten=breiten, grund=grund,
                              kopf=len(kopf), art="umschreiben"))
    for room, mid, name, seiten, grund in NEU:
        body = b"\x02\x00".join(b"\x08".join(enc(z) for z in s) for s in seiten)
        neu = bytes([0x04, 0x02]) + body + bytes([0x01, 0x00])
        breiten = [px(enc(z)) for s in seiten for z in s]
        if max(breiten) > MAX_PX:
            fehler.append("%s msg %d: Zeile %d px > %d" % (room, mid, max(breiten), MAX_PX))
        if len(neu) > MAX_LEN:
            fehler.append("%s msg %d: %d B > %d" % (room, mid, len(neu), MAX_LEN))
        eintraege.append(dict(room=room, mid=mid, name=name, bytes=neu, orig=b"", off=0,
                              alt="", neu=decode(neu), breiten=breiten, grund=grund, kopf=2, art="neu"))
    for room, mid, name, seiten, grund in UNTERSUCHEN:
        off, orig = msg_raw(room, mid)
        if orig[:2] != bytes([0x04, 0x02]) or orig[-2:] != bytes([0x01, 0x00]):
            raise SystemExit("%s msg %d: Vorlage hat nicht die Form 04 02 .. 01 00: %s" % (room, mid, orig.hex(" ")))
        o_seiten = [sb.split(bytes([0x08])) for sb in orig[2:-2].split(bytes([0x02, 0x00]))]
        for sb in o_seiten:
            for z in sb:
                if any(b in (0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0A, 0x0B) for b in z):
                    raise SystemExit("%s msg %d: Vorlage mit Steuercode in einer Zeile: %s" % (room, mid, z.hex(" ")))
        if [len(sb) for sb in o_seiten] != [len(sn) for sn in seiten]:
            raise SystemExit("%s msg %d: Seiten/Zeilen %s != Vorlage %s" % (room, mid, [len(sn) for sn in seiten],
                             [len(sb) for sb in o_seiten]))
        body = bytes([0x02, 0x00]).join(bytes([0x08]).join(enc(z) for z in sn) for sn in seiten)
        neu = orig[:2] + body + orig[-2:]
        breiten = [px(enc(z)) for sn in seiten for z in sn]
        if max(breiten) > MAX_PX:
            fehler.append("%s msg %d: Zeile %d px > %d" % (room, mid, max(breiten), MAX_PX))
        if len(neu) > MAX_LEN:
            fehler.append("%s msg %d: %d B > %d" % (room, mid, len(neu), MAX_LEN))
        eintraege.append(dict(room=room, mid=mid, name=name, bytes=neu, orig=orig, off=off,
                              alt=decode(orig), neu=decode(neu), breiten=breiten, grund=grund,
                              kopf=2, art="untersuchen"))
    return eintraege, fehler


def c_str(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def schreiben(eintraege):
    L = []
    L.append("/* AUTO-GENERIERT von re15_port/tools/r36_p/texte_bauen.py — NICHT von Hand aendern.")
    L.append(" * Runde 36 Spur P (story). Form/Belege: Kopf des Skripts + analysis/befunde_runde36/P_story.md.")
    L.append(" * Wortlaut = PORT-WAHL (Nachtrag AUFTRAG.md 14:00). */")
    L.append("")
    for e in eintraege:
        L.append("/* ROOM%s msg %d (%s) — %s" % (e["room"], e["mid"], e["art"], e["grund"]))
        if e["art"] in ("umschreiben", "untersuchen"):
            L.append(" *   alt @0x%05X (%d B): \"%s\"" % (e["off"], len(e["orig"]), e["alt"]))
            L.append(" *   Kopf %d B + Schwanz %d B = Original-Bytes derselben Nachricht" % (e["kopf"], 4 if e["art"] == "umschreiben" else 2))
        L.append(" *   neu (%d B): \"%s\"" % (len(e["bytes"]), e["neu"]))
        L.append(" *   Zeilenbreiten px: %s (Grenze %d) */" % (", ".join(str(b) for b in e["breiten"]), MAX_PX))
        hexs = ["0x%02x" % b for b in e["bytes"]]
        L.append("static const uint8_t %s[] = {" % e["name"])
        for i in range(0, len(hexs), 16):
            L.append("    " + ",".join(hexs[i:i + 16]) + ",")
        L.append("};")
        L.append("")
    L.append("static const re15_story_r36_text_t k_s36_texte[] = {")
    for e in eintraege:
        L.append("    { 0x%s, %2d, %s, (uint8_t)sizeof %s, 0x%05X, %s,"
                 % (e["room"], e["mid"], e["name"], e["name"], e["off"], c_str(e["alt"])))
        L.append("      %s }," % c_str(e["neu"]))
    L.append("};")
    L.append("")
    data = "\n".join(L)
    with open(OUT, "w", encoding="utf-8", newline="\n") as f:
        f.write(data)


def main():
    eintraege, fehler = bauen()
    for e in eintraege:
        print("ROOM%s msg %2d %-11s %3d B  px %-12s alt: %s" % (e["room"], e["mid"], e["art"], len(e["bytes"]),
              ",".join(str(b) for b in e["breiten"]), e["alt"]))
        print("%40s neu: %s" % ("", e["neu"]))
    if fehler:
        print("FEHLER:\n  " + "\n  ".join(fehler))
        return 1
    if "--check" not in sys.argv:
        schreiben(eintraege)
        print("geschrieben:", os.path.relpath(OUT, os.path.dirname(PORT)))
    return 0


if __name__ == "__main__":
    sys.exit(main())
