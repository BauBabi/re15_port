#!/usr/bin/env python3
"""re2_items.py - RE2-Retail (Leon, SLUS-00748) Item-Tabellen byte-true auslesen.

Alle Adressen sind aus der RE2-PSX.EXE disassembliert (siehe
analysis/befunde_2026-09-21/re2-sicherung-item.md §1):

  FUN_800693d0 @0x800693d0   Item-Namens-Zeichner (a3 = item-id)
    800693ec: sll v0,s4,3            ; id*8
    80069404: lbu v0,-25059(at)      ; at=0x800b0000 -> 0x800a9e1d = PROP[id]+5
    8006943c: addiu a3,s1,129        ; Variante -> Namens-Index 129+nibble
    80069450: jal 0x80031070         ; -> Text-Renderer
  FUN_80030b9c @0x80030b9c   Namens-Zeiger aufloesen
    80030bd0: lbu s0,-25059(at)      ; 0x800a9e1d = PROP[id]+5 (Varianten-Nibble)
    80030c24: lhu v1,-5204(at)       ; 0x8009ebac = NAME_OFFTBL_EN[id]
    80030c2c: addiu v0,v0,-6832      ; 0x8009e550 = NAME_BASE_EN
    80030c00: lhu v1,-4946(at)       ; 0x8009ecae = NAME_OFFTBL_EN[129+nibble]
    80030c74: lhu v1,-7112(at)       ; 0x8009e438 = NAME_OFFTBL_JP[id]
    80030c7c: addiu v0,v0,-8388      ; 0x8009df3c = NAME_BASE_JP

  Kombinations-Pruefung @0x800695F8..0x80069638 (legt die Recordlage fest):
    80069600: lbu a0,-25057(at)      ; 0x800A9E1F + id*8 = n_mix
    80069618: lw  v1,-25056(at)      ; 0x800A9E20 + id*8 = mix_ptr
    80069620: lbu v0,0(v1)           ; Rezept[+0] = Partner-Item
    80069638: addiu v1,v1,4          ; Rezept-Schrittweite 4
    80069644: lbu v0,2(v1)           ; Rezept[+2] = Ergebnis-Item
  Daraus folgt die 8-Byte-Recordlage mit Basis 0x800A9E1C (4-Byte-ausgerichtet,
  sonst wuerde das `lw` auf mix_ptr unaligned trappen):
    +0 max  +1 variant  +2 class  +3 n_mix  +4 mix_ptr(u32)
  Die 12 Zugriffe auf 0x800A9E1C (Feld +0) und die 3 auf 0x800A9E1D (Feld +1)
  bestaetigen das (tools/re2_sicherung/xref_scan.py range 0x800a9e18 0x800a9e21).
"""
import struct, os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE  = os.path.join(REPO, "info", "re2leon", "PSX.EXE")

D = open(EXE, "rb").read()
TADDR = struct.unpack_from("<I", D, 0x18)[0]          # 0x80010000
TSIZE = struct.unpack_from("<I", D, 0x1c)[0]


def fo(a):
    """RAM-Adresse -> Datei-Offset in PSX.EXE (Text ab Datei 0x800)."""
    return 0x800 + (a - TADDR)


# --- Tabellen-Adressen (alle aus dem Disasm oben) ---------------------------
NAME_OFFTBL_EN = 0x8009EBAC   # u16[140], relativ zu NAME_BASE_EN
NAME_BASE_EN   = 0x8009E550
NAME_OFFTBL_JP = 0x8009E438   # u16[140], relativ zu NAME_BASE_JP
NAME_BASE_JP   = 0x8009DF3C
PROP_TBL       = 0x800A9E1C   # 8 Byte je Item, ids 0x00..0x64
MIX_LIST_BASE  = 0x800A9CE8   # Ziel der mix_ptr (Kombinations-Rezepte, 4 Byte/Record)
N_NAMES        = 140          # Index 0..139 = 0x00..0x8B (139 ist letzter monoton
                              # steigender Offset 0x0651 < 0x065C = Blockende)
N_ITEMS        = 101          # 0x800AA144 - 0x800A9E1C = 0x328 -> /8 = 101.
                              # Die Obergrenze ist belegt: 0x800AA144/0x800AA146 werden
                              # von 5 lhu-Instruktionen als EIGENE Tabelle gelesen
                              # (0x8006D0B0, 0x80072520, 0x800727C8, 0x8007603C, 0x80076224)

# Zeichensatz des RE2-Textsystems.
#   Buchstaben: verifiziert an "Knife" (id 1), "Hand Gun" (id 2), "Magnum" (id 5),
#               "Shotgun" (id 7) — Grossbuchstaben ab 0x1D, Kleinbuchstaben ab 0x3D.
#   Ziffern:    verifiziert an id 37 "Chemical AC-W24" — dort stehen {0E}{10} fuer
#               "24", also '0' = 0x0C. (Mein erster Ansatz '0' = 0x13 war FALSCH;
#               er machte aus "AC-W24" ein "AC-W{0E}{10}" und aus
#               "Operation report 1/2" ein "report {0D}/{0E}".)
#   0x3B '-':   id 37 "AC-W24", id 82 "G-virus", id 126 "P-epsilon report"
#   0x3A '\'':  id 104 "CHRIS's diary", id 111/112 "Secretary's diary A/B"
#   0x01 '.':   id 4 "C. Hand Gun", id 9 "G. Launcher", id 79 "Vaccine Cart."
TERM = 0xF7


def dec_char(b):
    if b == 0x00: return " "
    if b == 0x01: return "."
    if 0x0C <= b <= 0x15: return chr(ord("0") + b - 0x0C)
    if 0x1D <= b <= 0x36: return chr(ord("A") + b - 0x1D)
    if 0x3A == b:         return "'"
    if 0x3B == b:         return "-"
    if 0x3D <= b <= 0x56: return chr(ord("a") + b - 0x3D)
    return "{%02X}" % b


def name_raw(i, offtbl=NAME_OFFTBL_EN, base=NAME_BASE_EN):
    """(Adresse, Rohbytes ohne 0xF7) des Namens mit Index i."""
    off = struct.unpack_from("<H", D, fo(offtbl + i * 2))[0]
    p = fo(base + off)
    raw = bytearray()
    while D[p] != TERM and len(raw) < 48:
        raw.append(D[p]); p += 1
    return base + off, bytes(raw)


def name(i, offtbl=NAME_OFFTBL_EN, base=NAME_BASE_EN):
    a, raw = name_raw(i, offtbl, base)
    return a, "".join(dec_char(b) for b in raw), raw


def prop(i):
    """(Adresse, 8 Rohbytes) des Item-Eigenschafts-Records."""
    a = PROP_TBL + i * 8
    return a, D[fo(a):fo(a) + 8]


def prop_fields(i):
    a, r = prop(i)
    d = {
        "addr":     a,
        "raw":      r,
        "max":      r[0],                               # +0 Stapel-/Magazin-Obergrenze
        "variant":  r[1],                               # +1 Varianten-Nibble (Namens-Swap)
        "cls":      r[2],                               # +2 Klassen-Byte
        "n_mix":    r[3],                               # +3 Anzahl Kombinations-Rezepte
        "mix_ptr":  struct.unpack_from("<I", r, 4)[0],  # +4 Zeiger auf die Rezeptliste
    }
    d["mix"] = mix_recipes(d["mix_ptr"], d["n_mix"])
    return d


def mix_recipes(ptr, n):
    """Die n Kombinations-Rezepte ab ptr. Record = 4 Byte:
       +0 Partner-Item (0x80069620), +2 Ergebnis-Item (0x80069644)."""
    out = []
    if n == 0 or not (0x80010000 <= ptr < 0x80010000 + TSIZE):
        return out
    for k in range(n):
        r = D[fo(ptr + k * 4):fo(ptr + k * 4) + 4]
        out.append({"addr": ptr + k * 4, "partner": r[0], "b1": r[1],
                    "result": r[2], "b3": r[3], "raw": r})
    return out


def all_items():
    for i in range(N_ITEMS):
        a, en, raw = name(i)
        f = prop_fields(i)
        f["id"] = i
        f["name_addr"] = a
        f["name_en"] = en
        f["name_raw"] = raw
        yield f


if __name__ == "__main__":
    print("PSX.EXE t_addr=0x%08X t_size=0x%X" % (TADDR, TSIZE))
    print("%-4s %-10s %-22s %-10s %s" % ("id", "addr", "name(EN)", "prop@", "raw"))
    for f in all_items():
        mix = ", ".join("+%d=>%d" % (m["partner"], m["result"]) for m in f["mix"])
        print("%3d  0x%08X %-22s 0x%08X %s  max=%-3d var=%-2d cls=0x%02X nmix=%d %s" % (
            f["id"], f["name_addr"], f["name_en"], f["addr"],
            " ".join("%02x" % b for b in f["raw"]),
            f["max"], f["variant"], f["cls"], f["n_mix"], mix))
    print()
    print("--- Namens-Indizes 101..139 (keine Item-Slots) ---")
    for i in range(N_ITEMS, N_NAMES):
        a, en, raw = name(i)
        print("%3d  0x%08X %s" % (i, a, en))
