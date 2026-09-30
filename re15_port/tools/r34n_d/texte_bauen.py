#!/usr/bin/env python3
"""texte_bauen.py - Spur D (Runde 34 Nacht): die vier neuen Nachrichten als .msg-Rohbytes, jedes Wort
mit Fundstelle im Auslieferungsstand (wie engine/src/tuer1120_1130.c), plus Zeilenbreiten.

Glyphentabelle = die des Ports (scd_walk_lib.glyph == engine/src/msg_common.c re15_msg_glyph), umgekehrt.
Satzform (Kopf/Farbe/Ende) je Nachricht nach einem ausgelieferten Vorbild:
  Dialogzeile  `04 00 05 cc` Name `16 05 00 00` Text `04 01 01 63`   (ROOM1090 msg 0 @0x275C / msg 1 @0x279C)
  Tuer-Text    `04 02` Text `01 00`                                   (ROOM1130 msg 1 @0x0B46, tuer1120_1130.c)
Breite: include/font_width.h (DEBUG.BIN[0x4416+code]); Umbruch 0x08.

Aufruf: texte_bauen.py [--c]    (--c: C-Initialisierer ausgeben)
"""
import glob, os, sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as W                      # noqa: E402

REPO = os.path.dirname(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))
PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")
FW = os.path.join(REPO, "re15_port", "include", "font_width.h")

# Umkehrung der Port-Glyphentabelle. '.' als Satzende = 0x57 (Dialogzeilen enden so: ROOM1050 msg 7
# @0x1037 "...medical room." -> `4e 4b 4b 49 57`); 0x3C ist das Abkuerzungs-'.' (R.P.D.).
ENC = {" ": 0x00, ":": 0x16, ",": 0x18, '"': 0x19, "!": 0x1A, "?": 0x1B, "'": 0x3A, "-": 0x3B,
       ".": 0x57}
for i in range(10):
    ENC[chr(ord("0") + i)] = 0x0C + i
for i in range(26):
    ENC[chr(ord("A") + i)] = 0x1D + i
    ENC[chr(ord("a") + i)] = 0x3D + i


def enc(s):
    return bytes(ENC[c] for c in s)


def breite():
    t = open(FW, encoding="latin-1").read()
    body = t[t.index("{") + 1:t.index("}")]
    return [int(x) for x in body.replace("\n", " ").split(",") if x.strip()]


def korpus():
    """Alle ausgelieferten Nachrichtenbloecke: [(raum, msg_id, datei_off, bytes)]."""
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
                out.append((os.path.basename(f)[:8], i, s, d[s:e]))
    return out


def fundstelle(korp, stueck):
    b = enc(stueck)
    for raum, mid, s, blob in korp:
        k = blob.find(b)
        if k >= 0:
            return "%s msg %d @0x%04X" % (raum, mid, s + k)
    return None


NACHRICHTEN = [
    # (id, form, sprecher/farbe, [zeilen], [woerter fuer Fundstellen])
    (22, "dialog", ("Woman", 0x02), ["Hello? Anyone? Please,", "get me out of here!"],
     ["Woman:", "Hello?", "Anyone?", "Anyone", "Please,", "Please", "get me out of here!"]),
    (23, "dialog", ("Leon", 0x01), ["Another civilian survivor."],
     ["Leon:", "Another", "Another ", "civilian", "survivor", "survivor.", "r."]),
    (24, "dialog", ("Leon", 0x01), ["I have to help her!"],
     ["Leon:", "I have to help", "I have to help her", "help her!", "her!"]),
    (25, "tuer", None, ["I have to help", "the Survivor first!"],
     ["I have to help", "the", " the ", "Survivor", "survivor", "S", "first!", " first"]),
]


def bauen(nid, form, spr, zeilen):
    if form == "dialog":
        name, farbe = spr
        kopf = bytes([0x04, 0x00, 0x05, farbe]) + enc(name) + bytes([0x16, 0x05, 0x00, 0x00])
        ende = bytes([0x04, 0x01, 0x01, 0x63])
    else:
        kopf = bytes([0x04, 0x02])
        ende = bytes([0x01, 0x00])
    koerper = b"\x08".join(enc(z) for z in zeilen)
    return kopf + koerper + ende


def fragmente(korp, text):
    """Gierig: je Position das laengste Stueck, das irgendwo ausgeliefert steht (mit Fundstelle)."""
    out, i = [], 0
    while i < len(text):
        best = None
        for j in range(len(text), i, -1):
            f = fundstelle(korp, text[i:j])
            if f:
                best = (text[i:j], f); break
        if not best:
            out.append((text[i], "KEIN BELEG")); i += 1; continue
        out.append(best); i += len(best[0])
    return out


def main():
    korp = korpus()
    fw = breite()
    print("Korpus: %d ausgelieferte Nachrichten" % len(korp))
    for nid, form, spr, zeilen, woerter in NACHRICHTEN:
        b = bauen(nid, form, spr, zeilen)
        print("\n=== msg %d (%s) %d Bytes" % (nid, form, len(b)))
        print("   " + " ".join("%02x" % x for x in b))
        for z in zeilen:
            px = sum(fw[c] for c in enc(z))
            print("   Zeile %-40r %3d px" % (z, px))
        if form == "dialog":
            name = spr[0] + ": "
            px = sum(fw[c] for c in enc(name + zeilen[0]))
            print("   Zeile 1 mit Sprecher %-28r %3d px" % (name + zeilen[0], px))
        for w in woerter:
            print("   %-22r -> %s" % (w, fundstelle(korp, w)))
        for z in zeilen:
            print("   Fragmente %r:" % z)
            for st, f in fragmente(korp, z):
                print("      %-24r %s" % (st, f))
        if "--c" in sys.argv:
            print("   C: {" + ",".join("0x%02x" % x for x in b) + "}")


if __name__ == "__main__":
    main()
