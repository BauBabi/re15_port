"""Abnahme 1 Spur G2 (Runde 34 Nacht) — Zensus: kann die neue Masken-Sichtbarkeit
(masken_gruppen.c, re15_mg_sichtbar) NACHGEZEICHNETE R15M-Masken verstecken?

Port-Regel (main.c pri-Block): R15M-Masken werden geladen, sobald der Original-Parser
re15_pri_parse_section 0 liefert — das ist NICHT nur die NULL-Sektion (u32 == 0xFFFFFFFF),
sondern auch group_count == 0, mask_count_decl == 0, > 256, zu kurz (pri_common.c Z.48-53).
masken_gruppen.c setzt dagegen Zahl = (kopf >> 16) & 0xFF fuer JEDE Nicht-NULL-Sektion und
loescht Byte0 der RDT[7] Records. re15_mg_sichtbar(i) liefert fuer i < Zahl dann Byte0 & 1.

Dieser Zensus listet je RDT/Cut: Kopf, Parser-Ergebnis, Zahl (mg), R15M-Abschnitt ja/nein —
und markiert Faelle, in denen R15M-Masken unter Zahl > 0 fallen wuerden (Regression).

    C:/Python310/python.exe re15_port/tools/r34n_g/abn1_r15m_zensus.py [--cd re15_port/shared_assets/PSX]
"""
import argparse
import glob
import os
import struct


def u16(d, o):
    return struct.unpack_from("<H", d, o)[0]


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def parser_n(d, off):
    """Nachbau re15_pri_parse_section: Rueckgabe out_count (0 = keine Original-Masken)."""
    if off + 4 > len(d):
        return 0, "kurz"
    gc, decl = u16(d, off), u16(d, off + 2)
    if gc == 0xFFFF and decl == 0xFFFF:
        return 0, "NULL"
    if gc == 0 or decl == 0:
        return 0, "leer(gc=%d,decl=%d)" % (gc, decl)
    if gc > 256 or decl > 256:
        return 0, "gross(gc=%d,decl=%d)" % (gc, decl)
    if len(d) - off < 4 + gc * 8:
        return 0, "kurz"
    return sum(u16(d, off + 4 + g * 8) for g in range(gc)), "ok"


def mg_zahl(d, off):
    """Nachbau re15_mg_aufbauen: Zahl."""
    if off > len(d) - 4:
        return 0
    k = u32(d, off)
    if k == 0xFFFFFFFF:
        return 0
    return (k >> 16) & 0xFF


def msk_off(blob, cut):
    if not blob or len(blob) < 12 or blob[:4] != b"R15M":
        return 0
    if u32(blob, 4) != 1:
        return 0
    cuts = u32(blob, 8)
    if cut >= cuts or 12 + cuts * 4 > len(blob):
        return 0
    o = u32(blob, 12 + cut * 4)
    if o == 0 or o + 4 > len(blob):
        return 0
    return o


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cd", default="re15_port/shared_assets/PSX")
    a = ap.parse_args()
    n_cuts = n_r15m = n_konflikt = 0
    faelle = {}
    for p in sorted(glob.glob(os.path.join(a.cd, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        name = os.path.basename(p)[:-4]
        if len(d) < 0x60:
            continue
        mskp = os.path.join(a.cd, "MASKS", name + ".MSK")
        blob = open(mskp, "rb").read() if os.path.exists(mskp) else None
        ncut, cam = d[1], u32(d, 0x24)
        for c in range(ncut):
            e = cam + c * 0x20
            if e + 0x20 > len(d):
                break
            n_cuts += 1
            po = u32(d, e + 0x1C)
            pn, art = parser_n(d, po)
            z = mg_zahl(d, po)
            faelle[art.split("(")[0]] = faelle.get(art.split("(")[0], 0) + 1
            mo = msk_off(blob, c) if pn == 0 else 0
            if mo:
                n_r15m += 1
                r15m_n, _ = parser_n(blob, mo)
                flag = ""
                if z > 0:
                    n_konflikt += 1
                    flag = "  <== KONFLIKT: R15M-Masken 0..%d unter Zahl %d (Byte0 geloescht)" % (
                        min(r15m_n, z) - 1, z)
                print("%s Cut %d: Original %s, Zahl(mg) %d, R15M %d Masken%s" % (
                    name, c, art, z, r15m_n, flag))
            elif pn == 0 and art != "NULL":
                print("%s Cut %d: Original %s, Zahl(mg) %d, kein R15M" % (name, c, art, z))
    print("Cuts gesamt %d, Arten %s, Cuts mit R15M %d, KONFLIKTE %d" % (
        n_cuts, faelle, n_r15m, n_konflikt))


if __name__ == "__main__":
    main()
