"""Spur G2 (Runde 34 Nacht) — Zensus: wo steht SCD-Opcode 0x45 (Col_chg_set) und welche
sprite.pri-Gruppen trifft er?

Opcode 0x45 (Handler @0x800428d4, Tabelle @0x800744a8) ruft FUN_800396a8(op1+1, op2):
fuer jeden der RDT[0] Masken-Records (4 Byte, Tabelle DAT_800b2584) mit Byte1 == op1+1
wird Byte0 := op2. FUN_800392d4 schreibt beim Cut-Wechsel Byte1 = Gruppenindex+1 und
Byte0 |= 1; FUN_80039590 zeichnet je Bild nur Records mit Byte0 & 1. Der Opcode schaltet
also ganze Maskengruppen des AKTUELLEN Cuts sichtbar/unsichtbar.

Ausgabe je Raum: jede 0x45-Stelle (Region, Datei-Offset, Operanden) und je Cut, ob der Cut
eine Gruppe op1+1 hat (Zahl der Records in dieser Gruppe).

    python re15_port/tools/r34n_g/col_chg_zensus.py [--cd re15_port/shared_assets/PSX]
"""
import argparse
import glob
import os
import struct
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
import scd_walk_lib as L  # noqa: E402


def gruppen_je_cut(d):
    """{cut: [n_records je Gruppe]} aus Kamera +0x1C (Parser-Regel FUN_800392d4)."""
    out = {}
    ncut = d[1]
    cam = L.u32(d, 0x24)
    for c in range(ncut):
        e = cam + c * 0x20
        if e + 0x20 > len(d):
            break
        po = L.u32(d, e + 0x1C)
        if po + 4 > len(d):
            continue
        w = L.u32(d, po)
        if w == 0xFFFFFFFF:
            continue
        ng = w & 0xFFFF
        if ng == 0 or ng > 256:
            continue
        out[c] = [L.u16(d, po + 4 + g * 8) for g in range(ng)]
    return out


def regionen_alle(d):
    reg = dict(L.regionen(d))
    xs = L.u32(d, 0x48)
    if xs and xs < len(d):
        try:
            se = L.rdt_section_end(d, xs)
            for (o, e, idx) in L.section_regions(d, xs, se):
                reg[("extra", idx)] = L.walk_ops(d, xs + o, xs + e)
        except Exception as ex:  # extra-SCD nicht zerlegbar -> melden, nicht raten
            reg[("extra", "FEHLER %s" % ex)] = []
    return reg


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cd", default="re15_port/shared_assets/PSX")
    a = ap.parse_args()
    rdts = sorted(glob.glob(os.path.join(a.cd, "STAGE*", "ROOM*.RDT")))
    n_raum = n_stellen = 0
    treffer = []
    for p in rdts:
        d = open(p, "rb").read()
        name = os.path.basename(p)[:-4]
        if len(d) < 0x60:
            print("%s  nur %d Byte (Platzhalter-RDT), uebersprungen" % (name, len(d)))
            continue
        reg = regionen_alle(d)
        stellen = []
        for (tag, idx), ops in sorted(reg.items(), key=lambda kv: (str(kv[0][0]), str(kv[0][1]))):
            for (pc, op, sz) in ops:
                if op == 0x45:
                    stellen.append((tag, idx, pc, d[pc + 1], d[pc + 2]))
        if not stellen:
            continue
        n_raum += 1
        n_stellen += len(stellen)
        gj = gruppen_je_cut(d)
        print("%s  (RDT+0x4C=0x%X, %d Cuts, Stellen %d)" % (name, L.u32(d, 0x4C), d[1], len(stellen)))
        ids = sorted(set(s[3] + 1 for s in stellen))
        for (tag, idx, pc, o1, o2) in stellen:
            print("   %-5s %-3s @0x%05X  45 %02x %02x  -> Gruppe %d := %d" % (tag, idx, pc, o1, o2, o1 + 1, o2))
        for gid in ids:
            cuts = ["Cut %d (%d Rec)" % (c, gj[c][gid - 1]) for c in sorted(gj) if len(gj[c]) >= gid]
            print("   Gruppe %d existiert in: %s" % (gid, ", ".join(cuts) if cuts else "KEINEM Cut"))
        treffer.append(name)
    print("\nRDTs gesamt %d, mit Opcode 0x45: %d Raeume, %d Stellen: %s"
          % (len(rdts), n_raum, n_stellen, " ".join(treffer)))


if __name__ == "__main__":
    main()
