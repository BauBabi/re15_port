"""Zensus Masken-Zwillinge: welche Elza-Raeume (ungerade Raum-Id) duerfen die Port-Masken der Leon-Variante nutzen?

Runde 37 Spur A, Nachbesserung 1 (VERTRAG.md §1.7, Abnahme A#0 M1; Dossier analysis/befunde_runde37/A_pri1100.md §8).

WARUM: Der Port laedt nachgezeichnete Masken (R15M, NUTZER-VORGABE) als MASKS/ROOM%04X.MSK mit der VOLLEN Raum-Id.
In STAGE1 gibt es sie nur fuer die Leon-Variante (Ausnahme ROOM1221) — Elza sah z.B. in ROOM1101 (RDT bytegleich zu
ROOM1100) den Zombie 1 ueber der Westwand.

KRITERIUM (aus dem Original abgeleitet, nicht gesetzt):
  - Masken des Originals = f(RDT-Kamerasatz): FUN_800392d4 @0x80039310 lw v1,36(a0) (RDT+0x24 Kameratabelle),
    @0x80039314 sll v0,v0,5 (Cut*0x20), @0x8003931c lw t5,28(v0) (Satz+0x1C = sprite.pri), @0x8003932c NULL-Test.
  - Projektion = f(RDT-Kamerasatz): Cut-Wechsel @0x80021e5c..e70 lhu a0,2(Satz) >> 7 -> 0x80066c30 (ctc2 H),
    @0x80021e84..e90 Satz -> FUN_80053ca4 (a0 = Satz, @0x80053cac addu s3,a0,zero; liest pos +4/+8/+12 und Ziel
    +16/+20/+24: @0x80053d1c lw v0,16(s3), @0x80053d20 lw v1,4(s3), @0x80053d30 lw v0,20(s3), @0x80053d34 lw v1,8(s3),
    @0x80053d44 lw v1,12(s3), @0x80053d48 lw v0,24(s3)). Verglichen wird Satz+0x00..+0x1B (flag +0 eingeschlossen = strenger).
  - Hintergrund = f(Stage, Raum OHNE Varianten-Ziffer, Cut): @0x80021c88 lh a3 Cut (0x800b0fe4), @0x80021c90 lh a1
    Raum (0x800b0fe2), @0x80021cc8 Stage-Tabelle 0x8007438c -> u16[Raum] (STAGE1 @0x8007429c, 40 Raeume),
    @0x80021cd8 sll a3,a3,5 (+Cut*32 Sektoren = Cut*0x10000). Die Spieler-Variante geht nicht ein.
  => Zwei Raeume mit gleichen Kamerasaetzen (Satz+0x00..+0x1B je Cut, gleiche Cut-Zahl RDT[1]) und gleichem
     Hintergrund je Cut (so, wie der Port ihn laedt: BSS/ROOM%04X/BG%02d.BSS, sonst STAGE%u/ROOM%03X.BSS-Scheibe)
     sehen dieselben Pixel unter derselben Projektion — dieselbe Maske gilt. Nur dann Rueckfall, und nur, wenn die
     ungerade Variante KEINE eigene MSK hat (eigene Datei gewinnt).

AUSGABE: engine/src/gen/pri_masken_zwilling_r37.inc (Tabelle {ungerade, gerade}) und der Zensus je Raum auf stdout
(Markdown). Der Riegel unit_r37_pri1100_zwilling rechnet denselben Zensus in C aus den Dateien nach.

Aufruf (Repo-Wurzel): python re15_port/tools/maske/zwilling_zensus.py [--nur-pruefen]
"""
import argparse
import os
import struct
import sys

CD = "re15_port/shared_assets/PSX"
INC = "re15_port/engine/src/gen/pri_masken_zwilling_r37.inc"
SATZ = 0x20          # Kamerasatz-Stride (@0x80039314 / @0x80021e60 sll ...,5)
SATZ_VERGL = 0x1C    # verglichen: flag/fov/pos/target (Satz+0x00..+0x1B); +0x1C = sprite.pri-Zeiger (Dateilage)
SCHEIBE = 0x10000    # @0x80021cd8 sll a3,a3,5 = Cut*32 Sektoren * 2048


def lies(p):
    try:
        with open(p, "rb") as f:
            return f.read()
    except OSError:
        return None


def kamera(rdt):
    n = rdt[1]
    off = struct.unpack_from("<I", rdt, 0x24)[0]
    saetze = [rdt[off + c * SATZ: off + c * SATZ + SATZ] for c in range(n)]
    return n, saetze


def pri_null(rdt, satz):
    po = struct.unpack_from("<I", satz, 0x1C)[0]
    if po + 4 > len(rdt):
        return None
    return struct.unpack_from("<I", rdt, po)[0] == 0xFFFFFFFF


def hintergrund(stage, rid, cut):
    """Der 64-KB-Block, den der Port fuer (Raum, Cut) laedt (bg_pc.c re15_bg_load_room_cut)."""
    b = lies(os.path.join(CD, "BSS", "ROOM%04X" % rid, "BG%02d.BSS" % cut))
    if b is not None:
        return b, "BSS/ROOM%04X" % rid
    s = lies(os.path.join(CD, "STAGE%d" % stage, "ROOM%03X.BSS" % (rid >> 4)))
    if s is not None and (cut + 1) * SCHEIBE <= len(s):
        return s[cut * SCHEIBE:(cut + 1) * SCHEIBE], "STAGE%d/ROOM%03X.BSS" % (stage, rid >> 4)
    return None, "fehlt"


def sektion(rdt, idx):
    """Bytes der RDT-Sektion idx (Kopf-Offset 0x20 + 4*idx) bis zur naechsthoeheren Sektion."""
    offs = [struct.unpack_from("<I", rdt, 0x08 + 4 * i)[0] for i in range(22)]
    o = offs[(0x20 - 0x08) // 4 + idx]
    hoeher = [x for x in offs if x > o] + [len(rdt)]
    return rdt[o:min(hoeher)]


def paar(stage, raum):
    ge, un = (stage << 12) | (raum << 4), (stage << 12) | (raum << 4) | 1
    rg = lies(os.path.join(CD, "STAGE%d" % stage, "ROOM%04X.RDT" % ge))
    ru = lies(os.path.join(CD, "STAGE%d" % stage, "ROOM%04X.RDT" % un))
    if rg is None or ru is None or len(rg) < 0x60 or len(ru) < 0x60:
        return None          # kein Paar bzw. 4-Byte-Platzhalter (z.B. STAGE1/ROOM1270.RDT, unbenutzter Raum-Slot)
    z = {"gerade": ge, "ungerade": un,
         "msk_g": os.path.exists(os.path.join(CD, "MASKS", "ROOM%04X.MSK" % ge)),
         "msk_u": os.path.exists(os.path.join(CD, "MASKS", "ROOM%04X.MSK" % un)),
         "rdt_gleich": rg == ru}
    ng, sg = kamera(rg)
    nu, su = kamera(ru)
    z["cuts"] = (ng, nu)
    z["kamera_gleich"] = ng == nu and all(a[:SATZ_VERGL] == b[:SATZ_VERGL] for a, b in zip(sg, su))
    z["kamera_abw"] = [c for c in range(min(ng, nu)) if sg[c][:SATZ_VERGL] != su[c][:SATZ_VERGL]]
    z["pri_null_gleich"] = ng == nu and all(pri_null(rg, a) == pri_null(ru, b) for a, b in zip(sg, su))
    bg_abw, quellen = [], set()
    for c in range(max(ng, nu)):
        bgg, qg = hintergrund(stage, ge, c)
        bgu, qu = hintergrund(stage, un, c)
        quellen.add(qg.split("/")[0]); quellen.add(qu.split("/")[0])
        if bgg != bgu:
            bg_abw.append(c)
    z["bg_gleich"] = not bg_abw
    z["bg_abw"] = bg_abw
    z["bg_quellen"] = sorted(quellen)
    z["sca_gleich"] = sektion(rg, 0) == sektion(ru, 0)          # 0x20 collisionStart (nur Auskunft)
    z["rueckfall"] = z["msk_g"] and not z["msk_u"] and z["kamera_gleich"] and z["bg_gleich"]
    return z


def grund(z):
    if z["msk_u"]:
        return "eigene MSK der Elza-Variante"
    if not z["msk_g"]:
        return "keine Leon-Masken (nichts nachzureichen)"
    if not z["kamera_gleich"]:
        return "Kamera verschieden (Cuts %d/%d, Saetze %s)" % (z["cuts"][0], z["cuts"][1], z["kamera_abw"])
    if not z["bg_gleich"]:
        return "Hintergrund verschieden (Cuts %s)" % z["bg_abw"]
    return "RUECKFALL"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--nur-pruefen", action="store_true", help="nichts schreiben, nur vergleichen")
    a = ap.parse_args()
    alle = []
    for stage in range(1, 7):
        for raum in range(0x40):
            z = paar(stage, raum)
            if z:
                alle.append(z)
    print("| Paar | Leon-MSK | Elza-MSK | RDT bytegleich | Kamera gleich | Hintergrund gleich | SCA gleich | "
          "sprite.pri-NULL gleich | Ergebnis |")
    print("|---|---|---|---|---|---|---|---|---|")
    ja = lambda v: "ja" if v else "nein"
    for z in alle:
        stage = z["gerade"] >> 12
        if stage != 1 and not (z["msk_g"] and not z["msk_u"]) and not z["msk_u"]:
            continue          # spaetere Stages: nur Paare mit Leon- oder Elza-Masken
        print("| %04X/%04X | %s | %s | %s | %s | %s | %s | %s | %s |"
              % (z["gerade"], z["ungerade"], ja(z["msk_g"]), ja(z["msk_u"]), ja(z["rdt_gleich"]),
                 ja(z["kamera_gleich"]), ja(z["bg_gleich"]), ja(z["sca_gleich"]), ja(z["pri_null_gleich"]),
                 grund(z)))
    tab = [z for z in alle if z["rueckfall"]]
    zeilen = ["/* ERZEUGT von re15_port/tools/maske/zwilling_zensus.py - NICHT von Hand aendern.",
              " * Runde 37 Spur A, Nachbesserung 1 (VERTRAG.md 1.7): Elza-Raum (ungerade Id) -> Leon-Variante, deren",
              " * Port-Masken (MASKS/ROOM%04X.MSK + _PRI%02d.TIM) gelten, weil Kamerasaetze (RDT+0x24, Satz+0x00..+0x1B je",
              " * Cut; fov @0x80021e68, pos/Ziel FUN_80053ca4 @0x80053d1c..d48) und Hintergrund je Cut (@0x80021c88..cd8, Raum",
              " * ohne Varianten-Ziffer)",
              " * nachweislich gleich sind und die Elza-Variante keine eigene MSK hat. Riegel: unit_r37_pri1100_zwilling. */"]
    for z in tab:
        zeilen.append("{ 0x%04X, 0x%04X },   /* RDT %s, SCA %s */"
                      % (z["ungerade"], z["gerade"], "bytegleich" if z["rdt_gleich"] else "verschieden",
                         "gleich" if z["sca_gleich"] else "verschieden"))
    text = "\n".join(zeilen) + "\n"
    alt = (lies(INC) or b"").replace(b"\r\n", b"\n")   # Arbeitskopie unter Windows ggf. CRLF (git autocrlf)
    print("\nRueckfall-Tabelle: %d Raeume: %s" % (len(tab), ", ".join("%04X->%04X" % (z["ungerade"], z["gerade"])
                                                                   for z in tab)))
    if a.nur_pruefen:
        print("Tabelle %s: %s" % (INC, "aktuell" if alt == text.encode() else "VERALTET"))
        return 0 if alt == text.encode() else 1
    with open(INC, "w", newline="\n", encoding="ascii") as f:   # reines ASCII (C-Quelle)
        f.write(text)
    print("geschrieben: %s" % INC)
    return 0


if __name__ == "__main__":
    sys.exit(main())
