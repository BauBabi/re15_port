#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht): Satzschluss der RE1.5-UNTERSUCHUNGSTEXTE (Kopf `04 02`).
Zaehlt ueber alle RDT-Nachrichten die letzte Glyphe vor dem Ende 0x01 (Steuercodes mit
Parameter 0x02/0x04/0x05/0x06/0x09/0x0A/0x0B werden uebersprungen, 0x03/0x07 = Auswahl).
Beleg fuer die FORM eines neuen Untersuchungstexts ("Nothing happened.")."""
import glob, struct, os, collections
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
cnt = collections.Counter(); bsp = {}
for p in sorted(glob.glob(os.path.join(REPO, "re15_port/shared_assets/PSX/STAGE*/ROOM*.RDT"))):
    d = open(p, "rb").read()
    if len(d) < 0x60: continue
    ms = struct.unpack_from("<I", d, 0x3C)[0]
    if ms == 0 or ms >= len(d): continue
    first = d[ms] | (d[ms+1] << 8); n = first // 2
    if n == 0 or n > 64: continue
    tbl = [d[ms+2*i] | (d[ms+2*i+1] << 8) for i in range(n)]
    for idx, o in enumerate(tbl):
        st = ms + o
        if d[st:st+2] != b"\x04\x02": continue
        i = st + 2; last = None
        while i < len(d) and i < st + 800:
            b = d[i]
            if b == 0x01: break
            if b in (0x02, 0x04, 0x05, 0x06, 0x09, 0x0A, 0x0B): i += 2; continue
            if b in (0x03, 0x07): last = "AUSWAHL(%02x)" % b; i += 1; continue
            if b == 0x08: i += 1; continue
            last = b; i += 1
        key = last if isinstance(last, str) else ("%02x" % last if last is not None else "leer")
        cnt[key] += 1
        bsp.setdefault(key, "%s msg %d @0x%05X" % (os.path.basename(p), idx, st))
NAME = {"57": "'.'", "1a": "'!'", "19": "'\"'", "1b": "'?'", "38": "'/'", "00": "' '"}
tot = sum(cnt.values())
for k, v in cnt.most_common():
    print("%-12s %-5s %4d  (%.1f %%)  z.B. %s" % (k, NAME.get(k, ""), v, 100.0 * v / tot, bsp[k]))
print("# %d Untersuchungstexte (Kopf 04 02) in allen RDTs" % tot)
