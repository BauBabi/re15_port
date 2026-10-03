#!/usr/bin/env python3
"""texte_bauen.py - Spur K (Runde 35): die 17 neuen Dialogzeilen der ROOM10F0-Szene als .msg-Rohbytes.

Abgeleitet von tools/r34n_d/texte_bauen.py (Spur D, Runde 34 Nacht). Glyphentabelle = die des Ports
(scd_walk_lib.glyph == engine/src/msg_common.c re15_msg_glyph), umgekehrt. Satzform je Zeile nach dem
ausgelieferten Vorbild:
  Dialogzeile  `04 00 05 cc` Name `16 05 00 00` Text `04 01 01 63`
     Leon   cc=01  ROOM1090 msg 1 @0x279C / ROOM11C0 msg 0 @0x1C98
     Woman  cc=02  ROOM1090 msg 0 @0x275C   (Nutzer-Konvention vor der Vorstellung)
     Ada    cc=02  ROOM11C0 msg 1 @0x1CD0
     Marvin cc=07  ROOM10D0 msg 14 @0x211E (`04 00 05 07 29 3d 4e 52 45 4a 16 05 00 00`)
  Auslassung "..." = `57 57 57` (ROOM11C0 msg 8 @0x1E14: `... 00 00 57 57 57 30 44 ...`)
Breite: include/font_width.h (DEBUG.BIN[0x4416+code]); Umbruch 0x08. Das Budget je Zeile ist das
Maximum der AUSGELIEFERTEN Dialogzeilen (wird unten aus dem Korpus gemessen, nicht gesetzt).

Aufruf: texte_bauen.py [--c]    (--c: C-Initialisierer ausgeben)
"""
import glob, os, sys
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, ".."))
import scd_walk_lib as W                      # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(HIER)))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
FW = os.path.join(REPO, "re15_port", "include", "font_width.h")

ENC = {" ": 0x00, ":": 0x16, ",": 0x18, '"': 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B, ".": 0x57}
for i in range(10):
    ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i

FARBE = {"Leon": 0x01, "Woman": 0x02, "Ada": 0x02, "Marvin": 0x07}

# (id, Sprecher, [Zeilen])  -- NUTZER-VORGABE (AUFTRAG.md Z.44-63), Wortlaut unveraendert; Umbrueche = Port-Satz
NACHRICHTEN = [
    (6,  "Leon",   ["Hey - how did you came in here?"]),
    (7,  "Woman",  ["Did you really think there was", "only one staff card for the", "Communication Room?"]),
    (8,  "Woman",  ["Anyway... the communication", "system is completely destroyed."]),
    (9,  "Woman",  ["We won't reach anyone with it", "anymore..."]),
    (10, "Marvin", ["Leon! You already made it!"]),
    (11, "Leon",   ["Hey Marvin, glad you made it!"]),
    (12, "Leon",   ["Allow me to introduce you. This is..."]),
    (13, "Ada",    ["... Ada, Ada Wong"]),
    (14, "Leon",   ["Ada Wong."]),
    (15, "Marvin", ["Hello, glad to meet another", "Survivor! I'm Marvin."]),
    (16, "Leon",   ["Anyway... looks like we can't", "contact anyone with this thing", "anymore."]),
    (17, "Marvin", ["Ohh... what do we do then?..."]),
    (18, "Leon",   ["..."]),
    (19, "Leon",   ["I know! The patrol car! We can", "use it to get out of here!"]),
    (20, "Marvin", ["Yeah, you're right! That could", "be our way out!"]),
    (21, "Leon",   ["Okay, Marvin, you go with Ada to", "the parking lot and wait there."]),
    (22, "Leon",   ["I'm going to get Chief Irons,", "and I'll be right behind you!"]),
    (23, "Marvin", ["Alright! Sounds like a plan.", "Take care Leon!"]),
]


def enc(s):
    return bytes(ENC[c] for c in s)


def breite():
    t = open(FW, encoding="latin-1").read()
    body = t[t.index("{") + 1:t.index("}")]
    return [int(x) for x in body.replace("\n", " ").split(",") if x.strip()]


def nachrichten_laenge(b):
    i = 0
    while i < len(b):
        c = b[i]
        if c == 0x01:
            return min(len(b), i + 2)
        if c in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            i += 2; continue
        i += 1
    return len(b)


def korpus():
    out = []
    for f in sorted(glob.glob(os.path.join(PSX, "STAGE*", "ROOM*.RDT"))):
        d = open(f, "rb").read()
        if len(d) < 0x60:
            continue
        ms = W.u32(d, 0x3C)
        if ms == 0 or ms + 2 > len(d):
            continue
        first = W.u16(d, ms)
        if first < 2 or first % 2 or ms + first > len(d):
            continue
        n = first // 2
        tbl = [W.u16(d, ms + 2 * i) for i in range(n)]
        for i, o in enumerate(tbl):
            s = ms + o
            e = ms + tbl[i + 1] if i + 1 < n else min(len(d), s + 400)
            if s < e <= len(d):
                e = s + nachrichten_laenge(d[s:e])
                out.append((os.path.basename(f)[:8], i, s, d[s:e]))
    return out


def zeilen_px(blob, fw):
    """Pixelbreite jeder Zeile einer ausgelieferten Nachricht (Steuercodes ueberspringen)."""
    px, out, i = 0, [], 0
    while i < len(blob):
        c = blob[i]
        if c == 0x01:
            break
        if c in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B):
            i += 2; continue
        if c in (0x03, 0x07):
            i += 1; continue
        if c == 0x08:
            out.append(px); px = 0; i += 1; continue
        if c < len(fw):
            px += fw[c]
        i += 1
    out.append(px)
    return out


def bauen(spr, zeilen):
    kopf = bytes([0x04, 0x00, 0x05, FARBE[spr]]) + enc(spr) + bytes([0x16, 0x05, 0x00, 0x00])
    ende = bytes([0x04, 0x01, 0x01, 0x63])
    return kopf + b"\x08".join(enc(z) for z in zeilen) + ende


def main():
    fw = breite()
    korp = korpus()
    # Budget: breiteste ausgelieferte Zeile einer DIALOG-Nachricht (Kopf 04 00 05 cc) ueber alle 240 RDTs
    maxpx, maxwo = 0, ""
    for raum, mid, s, blob in korp:
        if blob[:3] != b"\x04\x00\x05":
            continue
        for k, px in enumerate(zeilen_px(blob, fw)):
            if px > maxpx:
                maxpx, maxwo = px, "%s msg %d Zeile %d @0x%04X" % (raum, mid, k, s)
    print("Korpus: %d Nachrichten; breiteste Dialogzeile %d px (%s)" % (len(korp), maxpx, maxwo))
    c_out = []
    for nid, spr, zeilen in NACHRICHTEN:
        b = bauen(spr, zeilen)
        pxs = []
        pxs.append(sum(fw[c] for c in enc(spr + ": " + zeilen[0])))
        for z in zeilen[1:]:
            pxs.append(sum(fw[c] for c in enc(z)))
        warn = " <-- UEBER BUDGET" if max(pxs) > maxpx else ""
        print("msg %2d %-6s %3d B  Zeilen-px %s%s" % (nid, spr, len(b), pxs, warn))
        for z in zeilen:
            print("      %r" % z)
        if "--c" in sys.argv:
            txt = " / ".join(zeilen)
            c_out.append("static const uint8_t k_msg%02d[] = {   /* %s: %s */" % (nid, spr, txt))
            hexs = ["0x%02x" % x for x in b]
            for i in range(0, len(hexs), 16):
                c_out.append("    " + ", ".join(hexs[i:i + 16]) + ",")
            c_out.append("};")
    if c_out:
        print("\n".join(c_out))


if __name__ == "__main__":
    main()
