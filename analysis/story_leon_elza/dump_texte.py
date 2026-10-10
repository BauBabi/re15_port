#!/usr/bin/env python3
"""dump_texte.py - alle RDT-Nachrichten beider Varianten (Leon = xxx0, Elza = xxx1).

Dekoder = re15_port/tools/scd_walk_lib.py (Glyphentabelle des Ports,
engine/src/msg_common.c:175-199), keine zweite Tabelle.
Variante: der Raumlader addiert das Elza-Bit auf den CD-Dateiindex
(PSX.EXE @0x800397e4 `srl a0,a0,31`, @0x800397ec `addu a0,a0,v0`) -> Elza-RDT = Leon-RDT + 1.

Ausgabe: texte_leon.md, texte_elza.md, texte_alle.json (im Ordner dieses Skripts).
"""
import os, sys, json, glob, hashlib
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools"))
import scd_walk_lib as L  # noqa: E402

PSX = os.path.join(REPO, "re15_port", "shared_assets", "PSX")


def msgs_with_offsets(d):
    if len(d) < 0x60:
        return 0, []
    ms = L.u32(d, 0x3C)
    if ms == 0 or ms >= len(d) or ms + 2 > len(d):
        return ms, []
    first = L.u16(d, ms)
    if first < 2 or first % 2 or ms + first > len(d):
        return ms, []
    n = first // 2
    tbl = [L.u16(d, ms + 2 * i) for i in range(n)]
    out = []
    for idx, o in enumerate(tbl):
        st = ms + o
        en = ms + tbl[idx + 1] if idx + 1 < n else len(d)
        if st >= len(d):
            continue
        txt, ctrl = L.decode(d, st, min(en, st + 400))
        color = None
        for c in ctrl:
            if c.startswith("05:") and c != "05:00":
                color = c[3:]
                break
        mode = None
        for c in ctrl:
            if c.startswith("04:"):
                mode = c[3:]
                break
        speaker = ""
        if ":" in txt[:20]:
            cand = txt.split(":", 1)[0].strip()
            if cand and all(ch.isalpha() or ch in " .'" for ch in cand):
                speaker = cand
        out.append(dict(id=idx, off=st, text=txt, ctrl=ctrl, color=color, mode=mode, speaker=speaker))
    return ms, out


def main():
    rooms = {}
    for st in range(1, 8):
        for p in sorted(glob.glob(os.path.join(PSX, "STAGE%d" % st, "ROOM*.RDT"))):
            name = os.path.basename(p)[4:8]
            d = open(p, "rb").read()
            ms, ml = msgs_with_offsets(d)
            rooms[name] = dict(stage=st, path=os.path.relpath(p, REPO).replace("\\", "/"),
                               size=len(d), md5=hashlib.md5(d).hexdigest(), msg_sec=ms, msgs=ml)
    json.dump(rooms, open(os.path.join(HERE, "texte_alle.json"), "w", encoding="utf-8"),
              ensure_ascii=False, indent=1)

    for var, fname, who in ((0, "texte_leon.md", "Leon"), (1, "texte_elza.md", "Elza")):
        lines = []
        lines.append("# RE1.5 - alle RDT-Nachrichten, %s-Variante (ROOMxxx%d)\n" % (who, var))
        lines.append("Erzeugt von `analysis/story_leon_elza/dump_texte.py` (Dekoder = "
                     "`re15_port/tools/scd_walk_lib.py`, Glyphen = `engine/src/msg_common.c:175-199`).")
        lines.append("Variante: Elza-RDT = Leon-RDT + 1 (Raumlader PSX.EXE @0x800397e4 `srl a0,a0,31` + "
                     "@0x800397ec `addu a0,a0,v0`; Startraum Leon 0x17=ROOM1170 @0x8001d2a8, Elza 0x03=ROOM1031 @0x8001d324).")
        lines.append("Spalten: ID = Nachrichten-Index (Message_on-Argument), Off = Datei-Byte-Offset des Textanfangs, "
                     "Sp. = Sprecher aus dem Textpraefix \"Name:\", Farbe = Steuercode 05:xx (Sprecherfarbe), "
                     "Modus = Steuercode 04:xx (00 = Dialog, 02 = Untersuchungstext). \" / \" = Zeilenumbruch (0x08).\n")
        for name in sorted(rooms):
            if int(name[-1], 16) != var:
                continue
            r = rooms[name]
            other = name[:-1] + ("1" if var == 0 else "0")
            same = (other in rooms and rooms[other]["md5"] == r["md5"])
            lines.append("## ROOM%s (STAGE%d, %d B, msg_sec=0x%X%s)\n" %
                         (name, r["stage"], r["size"], r["msg_sec"],
                          ", BYTEGLEICH mit ROOM%s" % other if same else ""))
            if not r["msgs"]:
                lines.append("_kein Nachrichtenblock_\n")
                continue
            lines.append("| ID | Off | Sp. | Farbe | Modus | Text |")
            lines.append("|---:|---:|---|---|---|---|")
            for m in r["msgs"]:
                t = m["text"].replace("|", "\\|")
                lines.append("| %d | 0x%04X | %s | %s | %s | %s |" %
                             (m["id"], m["off"], m["speaker"], m["color"] or "", m["mode"] or "", t))
            lines.append("")
        open(os.path.join(HERE, fname), "w", encoding="utf-8").write("\n".join(lines) + "\n")
    print("Raeume:", len(rooms))


if __name__ == "__main__":
    main()
