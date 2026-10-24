#!/usr/bin/env python3
"""Runde 41 Spur K: Clip-Folge je Aktor ORIGINAL (k2_orig_3091.py-Protokoll) gegen PORT (RE15_ANIM_TRACE, anim_folge.py).

Original-Zeile: "B<n> | vs.. | R309 | cut.. | S:t.. st4 sub0 ph1 clip13 f37 f8f0 fl0024 w168=.. | E0:... | E1:... | E2:..."
  S = Spieler (0x800aca54), E0/E1 = Gegner 0/1 (0x800acc2c + i*0x1F4).
  Abschnitt = gleiches (Kanal-Sub, Clip, Flag 0x80 rueckwaerts); Plc_motion setzt +0x94/+0x5/+0x1c4 neu
  (@0x80041ba8/@0x80041bc4/@0x80041bc8) -> ein neuer Plc_motion mit gleichem Clip+Flag ist am Bild-Ruecksprung erkennbar.
Port-Zeile (anim_select_common.c): bild aktor typ motion clip fc cur slot cliplen loco rev legacy; Aktor 0 = Spieler,
  1 = Gegner 0, 2 = Gegner 1 (Port-Slot = 1 + Gegner-Index).
Ausgabe: je Aktor die beiden Folgen (Clip[r], Bildspanne) nebeneinander + Gleichheit der reinen Clip-Folge.
    python k2_vergleich.py <orig.log> <port_anim.txt> [port_abschnitt=1]
"""
import sys, re

def orig_folgen(pfad):
    akt = {"S": [], "E0": [], "E1": []}
    for ln in open(pfad, encoding="utf-8", errors="replace"):
        if not ln.startswith("B"): continue
        teile = ln.strip().split(" | ")
        b = int(teile[0][1:])
        for t in teile[4:]:
            m = re.match(r"(S|E0|E1|E2):t(\w+) st(\d+) sub(\d+) ph(\d+) clip(\d+) f(\d+) f8f(\d+) fl(\w+)", t)
            if not m or m.group(1) not in akt: continue
            sub, ph, clip, f, fl = int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)), int(m.group(9), 16)
            rev = 1 if fl & 0x80 else 0
            st = akt[m.group(1)]
            key = (sub, clip, rev)
            neu = (not st) or st[-1]["key"] != key
            # gleicher Clip neu gestartet: Bild faellt auf 0 zurueck, waehrend vorher gespielt wurde (Phase 1)
            if not neu and f < st[-1]["f_letzt"] and ph == 1 and st[-1]["ph_letzt"] == 1 and f <= 1 and st[-1]["f_letzt"] > 2 and not (fl & 4):
                neu = True
            if neu:
                st.append({"key": key, "b0": b, "b1": b, "f_min": f, "f_max": f, "f_letzt": f, "ph_letzt": ph, "fl": fl})
            else:
                e = st[-1]; e["b1"] = b; e["f_min"] = min(e["f_min"], f); e["f_max"] = max(e["f_max"], f)
                e["f_letzt"] = f; e["ph_letzt"] = ph; e["fl"] |= fl
    return akt

def port_folgen(pfad, abschnitt):
    rows = [l.split() for l in open(pfad) if l.strip()]
    absch, cur, last = [], [], -1
    for r in rows:
        f = int(r[0])
        if f < last - 5: absch.append(cur); cur = []
        last = f; cur.append(r)
    absch.append(cur)
    akt = {}
    for r in absch[abschnitt]:
        a = r[1]
        if a not in ("1", "2"): a = "0"   # Spieler-Index im Trace ist kein g_actors-Index
        key = (int(r[4]), int(r[10]))
        st = akt.setdefault(a, [])
        if not st or st[-1]["key"] != key:
            st.append({"key": key, "b0": int(r[0]), "b1": int(r[0]), "fc": int(r[5])})
        else:
            st[-1]["b1"] = int(r[0])
    return akt

o = orig_folgen(sys.argv[1])
p = port_folgen(sys.argv[2], int(sys.argv[3]) if len(sys.argv) > 3 else 1)
for oname, pname, titel in (("S", "0", "Spieler (Sherry)"), ("E0", "1", "Gegner 0 (John 0x49)"), ("E1", "2", "Gegner 1 (Elza-NPC 0x42)")):
    of = [e for e in o[oname] if e["key"][0] == 0]          # nur Kanal-Sub 0 (Raum-RBJ, Plc_motion(0,n))
    pf = p.get(pname, [])
    print("== %s: Original %d Abschnitte (alle Subs %d), Port %d Abschnitte" % (titel, len(of), len(o[oname]), len(pf)))
    for e in o[oname]:
        print("   ORIG B%5d-%5d sub%d clip%2d%s Bild %d..%d fl %04x" % (e["b0"], e["b1"], e["key"][0], e["key"][1],
              " rev" if e["key"][2] else "    ", e["f_min"], e["f_max"], e["fl"]))
    for e in pf:
        print("   PORT F%5d-%5d clip%2d%s fc %d" % (e["b0"], e["b1"], e["key"][0], " rev" if e["key"][1] else "    ", e["fc"]))
    oc = [(e["key"][1], e["key"][2]) for e in of]
    print("   Original Sub-0-Clipfolge:", oc)
