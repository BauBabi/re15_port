#!/usr/bin/env python3
"""r36_re2_piep_zensus.py - Runde 36 Spur G (Punkt 3): wo spielt RE2 den Kartenleser-Piep, und was folgt ihm?

Nur LESEND. Je RE2-Raum (info/re2leon/PL0/RDT) jeden SCD-Block laufen (Walker r30_re2_scd), jedes Se_on
(Opcode 0x36, @0x80056428: Bank pc[1], Satz pc[2]) der Raumbank 2 auf seine Welle aufloesen
(r30_re2_roombank.resolve = EDT/VH wie Se_on 0x8005ba28) und die Bloecke ausgeben, die eine
TON-Welle (stehender Ton, r36_kartenton_analyse.segmente: Flachheit < 0.08) enthalten, mit der Folge aller
Se_on/Sleep/Message_on/Aot_reset/Set in diesem Block. Messwerkzeug, kein Spielcode.
"""
import os, sys, struct
HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "analysis", "befunde_runde30", "tools"))
sys.path.insert(0, HIER)
import r30_re2_scd as S                                   # noqa: E402
from r30_re2_roombank import load_bank, resolve            # noqa: E402
import r36_kartenton_analyse as A                         # noqa: E402
import numpy as np                                         # noqa: E402
import glob

_cache = {}
def art(raum, satz):
    k = (raum, satz)
    if k in _cache: return _cache[k]
    w = A.welle(raum, satz)
    if w is None:
        _cache[k] = None; return None
    seg = A.segmente(A.profil(w["pcm"], w["rate"]))
    ton = [s for s in seg if s["flach"] < 0.08 and s["f_anteil"] >= 0.6 and s["f"] > 300 and (s["t1"] - s["t0"]) >= 0.1]
    _cache[k] = dict(sha=w["L"]["sha1"][:8], dauer=len(w["pcm"]) / w["rate"], ton=ton)
    return _cache[k]

def main():
    nur = sys.argv[1:] 
    for f in sorted(glob.glob(os.path.join(REPO, "info", "re2leon", "PL0", "RDT", "ROOM*.RDT"))):
        raum = os.path.basename(f)[4:8]
        if nur and raum not in nur: continue
        d = open(f, "rb").read()
        for which, nm in ((16, "main"), (17, "sub")):
            for i, s, e in S.blocks(d, which):
                recs, st = S.walk(d, s, e)
                folge, hat_ton = [], False
                for off, op, r in recs:
                    n = S.NAMES.get(op, "op%02X" % op)
                    if op == 0x36:
                        bank, satz = r[1], r[2]
                        a = art(raum, satz) if bank == 2 else None
                        t = ""
                        if a:
                            t = " %s %.2fs" % (a["sha"], a["dauer"])
                            if a["ton"]:
                                hat_ton = True
                                t += " TON@%.0fHz %.2f..%.2fs" % (a["ton"][0]["f"], a["ton"][0]["t0"], a["ton"][0]["t1"])
                        folge.append("@0x%05X Se_on(%d,0x%02x)%s" % (off, bank, satz, t))
                    elif op in (0x09, 0x0A):
                        folge.append("@0x%05X %s %s" % (off, n, r.hex(" ")))
                    elif op in (0x2B, 0x46, 0x22, 0x62, 0x29, 0x3B, 0x2C, 0x04):
                        folge.append("@0x%05X %s %s" % (off, n, r.hex(" ")))
                if hat_ton:
                    print("ROOM%s %s%02d (%s)" % (raum, nm, i, st))
                    for z in folge: print("    " + z)

if __name__ == "__main__":
    main()
