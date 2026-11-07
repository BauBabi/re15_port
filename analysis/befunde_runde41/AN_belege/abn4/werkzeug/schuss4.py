#!/usr/bin/env python3
"""schuss4.py - Abnahme 4 Spur AN: Schuesse und Treffer aus einem state.log.
Je Bild: Magazin mg, Spieler-Lage, alle Aktoren [slot t=.. st=.. ... @(x,z,r..)] hp=..
Ausgabe: Schusszahl (mg-Abfall), je Aktor-Slot/Typ die hp-Wechsel (Bild, alt->neu), Zustandswechsel der NPCs (t>=0x40),
und fuer die Bilder mit Schuss die Lage von Spieler, NPC und Gegnern.
  python schuss4.py <state.log> [raum-abschnitt: 'letzter' | Zahl]"""
import sys, re
lines = [l.rstrip("\n") for l in open(sys.argv[1], errors="replace") if l.startswith("F")]
# Abschnitte (Bildnummer faellt = neuer Raum)
segs, cur, last = [], [], -1
for l in lines:
    f = int(l.split(" ", 1)[0][1:])
    if f < last: segs.append(cur); cur = []
    cur.append(l); last = f
segs.append(cur)
wahl = sys.argv[2] if len(sys.argv) > 2 else "letzter"
seg = segs[-1] if wahl == "letzter" else segs[int(wahl)]
print("Abschnitte: %d, gewaehlt %s (%d Bilder, %s..%s)" % (len(segs), wahl, len(seg), seg[0].split()[0], seg[-1].split()[0]))
mgp = re.compile(r" mg=(-?\d+) "); plp = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")
ap = re.compile(r"\[(\d+) t=(\w+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=(\w+) mo=(\d+) af=(\d+) stun=(-?\d+) d=(-?\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
prev_mg = None; schuesse = []; prev = {}; hpw = []; stw = []
for l in seg:
    f = l.split(" ", 1)[0]
    m = mgp.search(l); mg = int(m.group(1)) if m else None
    acts = {int(a.group(1)): a.groups() for a in ap.finditer(l)}
    if prev_mg is not None and mg is not None and mg < prev_mg:
        p = plp.search(l)
        schuesse.append((f, prev_mg, mg, p.groups() if p else None,
                         {k: (v[1], v[2], v[11], v[12], v[14]) for k, v in acts.items()}))
    for k, v in acts.items():
        if k in prev:
            o = prev[k]
            if o[1] == v[1] and o[14] != v[14]: hpw.append((f, k, v[1], o[14], v[14], v[2]))
            if o[1] == v[1] and int(v[1], 16) >= 0x40 and o[2] != v[2]: stw.append((f, k, v[1], o[2], v[2]))
    prev = acts; prev_mg = mg if mg is not None else prev_mg
print("Schuesse (mg-Abfall): %d" % len(schuesse))
for s in schuesse[:40]:
    print("  %s mg %d->%d PL %s | %s" % (s[0], s[1], s[2], s[3], " ".join("[%d t=%s st=%s @(%s,%s) hp=%s]" % ((k,) + v) for k, v in sorted(s[4].items()))))
print("hp-Wechsel: %d" % len(hpw))
for h in hpw[:80]: print("  %s Slot %d t=%s hp %s -> %s (st %s)" % h)
print("NPC-Zustandswechsel (t>=0x40): %d" % len(stw))
for h in stw[:40]: print("  %s Slot %d t=%s st %s -> %s" % h)
