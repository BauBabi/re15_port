# luft_auswertung.py <npckante.log> [slot ...]
# Wertet den Messhaken RE15_NPCKANTE_LOG (npckante_r38.c re15_npckante_spur) aus: je NPC-Slot
#  - Bilder in den Geh-/Lauf-/Stand-Subs (State 4, Sub 4/5/6/9), in denen y NICHT auf dem Boden unter dem NPC
#    liegt (boden = room_coll(x,z,0,Band,0x100) = Oberkante des hoechsten Blocks unter dem eigenen Band, 0 = Grund)
#    -> "in der Luft" (y < boden) bzw. "im Boden" (y > boden)
#  - Kanten-Sub (0x12/0x16) und Sprung-Sub (0x17/0x18): Bilder, Clips, Bandwechsel mit Bildnummer und Lage
#  - Ende: Lage, Band, Sub
import re, sys
from collections import defaultdict

pat = re.compile(r"F(\d+) R(\w+) s(\d+) t(\w+) x=(-?\d+) y=(-?\d+) z=(-?\d+) b=(\d+) fy=(-?\d+) st=(\d+) "
                 r"s1=(\d+) s2=(\d+) mo=(-?\d+) af=(\d+) k=(\d+) sonde=(\d+) boden=(-?\d+) r=(\d+)(?: fall=(\d) f1c0=(\w+))?")
rows = defaultdict(list)
for line in open(sys.argv[1], encoding="utf-8", errors="replace"):
    m = pat.match(line)
    if not m:
        continue
    g = m.groups()
    rows[(g[1], int(g[2]))].append(dict(F=int(g[0]), raum=g[1], typ=g[3], x=int(g[4]), y=int(g[5]), z=int(g[6]),
                                       b=int(g[7]), fy=int(g[8]), st=int(g[9]), s1=int(g[10]), s2=int(g[11]),
                                       mo=int(g[12]), af=int(g[13]), k=int(g[14]), sonde=int(g[15]),
                                       boden=int(g[16]), fall=int(g[18]) if g[18] else -1,
                                       f1c0=int(g[19], 16) if g[19] else 0))
nur = set(int(a) for a in sys.argv[2:])
gesamt_luft = 0
for (raum, slot), rs in sorted(rows.items()):
    if nur and slot not in nur:
        continue
    # "in der Luft" (Geh-/Lauf-/Stand-Subs 4/5/6/9): y nicht auf der Bandhoehe -(Band*1800) ODER das Kriterium
    # der Engine-Schwerkraft FUN_8001bd60 greift (fall=1: Mittelpunkt >= Radius tief in einer Absprungzelle des
    # eigenen Bandes) ODER (alte Logs ohne fall=) y ueber dem room_coll-Boden.
    def in_luft(r):
        if r["y"] != -r["b"] * 1800:
            return True
        if r["fall"] >= 0:
            return r["fall"] == 1
        return r["y"] < r["boden"]
    # geparkte Aktoren (Pos_set (-30000,*,-30000), z.B. ROOM10B1 main00 @0x01726) zaehlen nicht
    walk = [r for r in rs if r["st"] == 4 and r["s1"] in (4, 5, 6, 9) and not (r["x"] == -30000 and r["z"] == -30000)]
    luft = [r for r in walk if in_luft(r) and r["y"] < r["boden"]] if rs[0]["fall"] < 0 else [r for r in walk if in_luft(r)]
    tief = [r for r in walk if r["y"] > max(r["boden"], -r["b"] * 1800)]
    schwer = [r for r in rs if r["f1c0"] & 0x8000]
    kante = [r for r in rs if r["st"] == 4 and r["s1"] in (0x12, 0x16)]
    sprung = [r for r in rs if r["st"] == 4 and r["s1"] in (0x17, 0x18)]
    clips_k = sorted(set(r["mo"] for r in kante))
    clips_s = sorted(set(r["mo"] for r in sprung))
    gesamt_luft += len(luft)
    print("ROOM%s Slot %d Typ %s: %d Bilder (F%d..F%d)" % (raum, slot, rs[0]["typ"], len(rs), rs[0]["F"], rs[-1]["F"]))
    print("  Geh/Lauf/Stand in der Luft (y < Boden): %d%s" % (len(luft), (" erstes F%d (%d,%d,%d) Boden %d" % (
        luft[0]["F"], luft[0]["x"], luft[0]["y"], luft[0]["z"], luft[0]["boden"])) if luft else ""))
    print("  Geh/Lauf/Stand unter dem Boden (y > Boden): %d%s" % (len(tief), (" erstes F%d (%d,%d,%d) Boden %d" % (
        tief[0]["F"], tief[0]["x"], tief[0]["y"], tief[0]["z"], tief[0]["boden"])) if tief else ""))
    print("  Schwerkraft-Fall (+0x1c0 & 0x8000): %d Bilder%s" % (len(schwer), (" erstes F%d (%d,%d,%d)" % (
        schwer[0]["F"], schwer[0]["x"], schwer[0]["y"], schwer[0]["z"])) if schwer else ""))
    print("  Kanten-Sub 0x12/0x16: %d Bilder, Clips %s; Sprung-Sub 0x17/0x18: %d Bilder, Clips %s" % (
        len(kante), clips_k, len(sprung), clips_s))
    vor = None
    for r in rs:
        if vor is not None and r["b"] != vor["b"]:
            print("  F%d Band %d -> %d, y %d -> %d, Lage (%d,%d), Sub 0x%02X Phase %d, Clip %d" % (
                r["F"], vor["b"], r["b"], vor["y"], r["y"], r["x"], r["z"], r["s1"], r["s2"], r["mo"]))
        if vor is not None and r["s1"] != vor["s1"] and (r["s1"] in (0x12, 0x16, 0x17, 0x18)):
            print("  F%d Sub %d -> 0x%02X bei (%d,%d,%d) Band %d (Sonde %d)" % (
                r["F"], vor["s1"], r["s1"], r["x"], r["y"], r["z"], r["b"], vor["sonde"]))
        vor = r
    e = rs[-1]
    print("  Ende F%d: (%d,%d,%d) Band %d Boden %d, State %d Sub %d Phase %d Clip %d" % (
        e["F"], e["x"], e["y"], e["z"], e["b"], e["boden"], e["st"], e["s1"], e["s2"], e["mo"]))
print("SUMME Luft-Bilder: %d" % gesamt_luft)
