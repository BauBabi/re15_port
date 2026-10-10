#!/usr/bin/env python3
"""re2_raum_se_zensus.py — Runde 36 Spur L: Zensus eines Raumbank-Satzes (Se_on Bank 2) ueber alle
RE2-Leon-RDTs (info/re2leon/PL0/RDT). Je Satz-Id: welche Welle (sha1, Laenge) liegt in welchen Raeumen.

Satzformat (re15_port/tools/re2_door_se_cut.py, selbst nachgelesen):
  RDT-Kopf +0x08/+0x0C/+0x10 = EDT / VH ("pBAV") / VB der Raumbank (Bank 2, FUN_80059e54 @0x80059f44).
  EDT[satz] 4 B: Byte1&0x7F Programm, Byte2>>4 Ton; Ton @VH+0x820+prog*0x200+ton*0x20, VAG-Index u16 @+0x16;
  VAG-Groessentabelle @VH+0x820+nprog*0x200 (u16 * 8); VAG k beginnt bei VB + Summe(Groesse[0..k-1]).
Aufruf: python re2_raum_se_zensus.py 0x25 [0x26 ...]   (Satz-Ids hex)
"""
import os, sys, struct, hashlib, glob, collections
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
RDT = os.path.join(REPO, "info", "re2leon", "PL0", "RDT")


def satz_welle(d, sid):
    edt, vh, vb = struct.unpack_from("<III", d, 8)
    if not edt or not vh or vh + 4 > len(d) or d[vh:vh + 4] != b"pBAV":
        return None
    if edt + sid * 4 + 4 > vh:
        return None
    raw = d[edt + sid * 4: edt + sid * 4 + 4]
    if raw == b"\0\0\0\0":
        return None
    prog, tone = raw[1] & 0x7F, raw[2] >> 4
    nprog = struct.unpack_from("<H", d, vh + 0x12)[0]
    toff = vh + 0x820 + prog * 0x200 + tone * 0x20
    if toff + 0x20 > len(d) or vh + 0x820 + nprog * 0x200 + 0x200 > len(d):
        return None
    vagi = struct.unpack_from("<H", d, toff + 0x16)[0]
    st = vh + 0x820 + nprog * 0x200
    if vagi > 255 or st + 2 * vagi + 2 > len(d):
        return None
    pos = vb
    for k in range(vagi):
        pos += struct.unpack_from("<H", d, st + 2 * k)[0] << 3
    ln = struct.unpack_from("<H", d, st + 2 * vagi)[0] << 3
    w = d[pos:pos + ln]
    return dict(edt_off=edt + sid * 4, raw=raw, tone_off=toff, vag_off=pos, vag_len=ln,
                sha=hashlib.sha1(w).hexdigest(), lagen=raw[3] >> 5, tone=d[toff:toff + 32])


def main():
    for a in sys.argv[1:]:
        sid = int(a, 16)
        fam = collections.defaultdict(list)
        for p in sorted(glob.glob(os.path.join(RDT, "ROOM*.RDT"))):
            d = open(p, "rb").read()
            if len(d) < 0x60:
                continue
            w = satz_welle(d, sid)
            if w:
                fam[(w["sha"], w["vag_len"])].append((os.path.basename(p)[4:8], w))
        print("== Satz 0x%02X: %d Wellen-Familien" % (sid, len(fam)))
        for (sha, ln), rs in sorted(fam.items(), key=lambda x: -len(x[1])):
            r0 = rs[0][1]
            print("  sha1 %s %6d B (%.3f s @ 22050? nur Mass)  n=%2d  EDT %s  erste: ROOM%s EDT@0x%05X VAG@0x%05X  Raeume: %s"
                  % (sha[:12], ln, ln / 16 * 28 / 22050.0, len(rs), r0["raw"].hex(" "), rs[0][0], r0["edt_off"],
                     r0["vag_off"], " ".join(r for r, _ in rs)))


if __name__ == "__main__":
    main()
