"""aussen6.py <laufordner> [...]: eigenes Mass der Abnahme 6 (unabhaengig vom aussenlog des Bauers), ALLE Besuche von ROOM2090/2091.
Grundlage: ROOM2090.RDT = ROOM2091.RDT (md5 a48d11a4), SCA-Kopf @0x6dc, selbst geparst (werkzeug/sca6.py): Band 0 Aussenwaende [0] x <= -8900 (0x6f4),
[1] z <= -27000 (0x700), [2] z >= -5400 (0x70c), [3] x >= 7200 (0x718); Plattform [4] x -1700..1850 z -20000..-12450 (0x724); Rampe [5] x 1850..7200
z -18100..-14500 (0x730). Band 1: nur [6] Ostwand, [7] Plattform, Leisten [8] z -14500..-13500 (0x754), [9] z -19100..-18100 (0x760).
Je Bild (state.log F-Zeile i = climb.log T-Zeile i; Raum aus climb.log room=):
  Klettern  = climb.log sub != 0 (Steigen zwischen Deck und Wasser) - getrennt gezaehlt
  oben      = y < -900: "Deck verlassen" = Mittelpunkt ausserhalb des Rampen-Grundrisses [5] (Band 1 hat sonst keine begehbare Flaeche)
  unten     = sonst: "aussen" = ausserhalb der Aussenwaende, "Nische" = x > 7200 im Rampenband, "im Block" = Mittelpunkt in [4] oder [5]
Dazu: RAUMHALT-Zeilen in gator_boss.log (Engine-Diagnose des Baus), Absturz-/Ausnahme-Zeilen in debug.log, exit aus pid.txt."""
import re, sys, os

RP = (1850, 7200, -18100, -14500)
PP = (-1700, 1850, -20000, -12450)


def lies(run):
    sl = [l for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
    cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")] if os.path.exists(run + "/climb.log") else []
    out = []
    for i, l in enumerate(sl):
        m = re.match(r"F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)", l)
        if not m or i >= len(cl):
            continue
        c = re.search(r"room=(\w+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\).*? sub=(\d+)", cl[i])
        if not c:
            continue
        out.append(dict(F=int(m.group(1)), x=int(m.group(2)), z=int(m.group(3)), hp=int(m.group(5)), room=c.group(1), y=int(c.group(3)),
                        cx=int(c.group(2)), cz=int(c.group(4)), sub=int(c.group(5))))
    return out, len(sl), len(cl)


def ort(s):
    x, z = s["x"], s["z"]
    if s["sub"] != 0:
        return "klettern"
    if s["y"] < -900:
        return "oben" if (RP[0] <= x <= RP[1] and RP[2] <= z <= RP[3]) else "deck_verlassen"
    if x > 7200 and RP[2] <= z <= RP[3]:
        return "nische"
    if x < -8900 or x > 7200 or z < -27000 or z > -5400:
        return "aussen"
    if (PP[0] < x < PP[1] and PP[2] < z < PP[3]) or (RP[0] < x < RP[1] and RP[2] < z < RP[3]):
        return "im_block"
    return "wasser"


for run in sys.argv[1:]:
    name = run.replace("\\", "/").rstrip("/").split("/")[-1]
    st, nsl, ncl = lies(run)
    st = [s for s in st if s["room"] in ("2090", "2091")]
    n = {}
    erst = {}
    for s in st:
        o = ort(s)
        n[o] = n.get(o, 0) + 1
        if o not in erst:
            erst[o] = s
    boss = open(run + "/gator_boss.log", errors="replace").read() if os.path.exists(run + "/gator_boss.log") else ""
    rh = re.findall(r"RAUMHALT F(\d+) Band (\d) \((-?\d+),(-?\d+)\) -> \((-?\d+),(-?\d+)\)", boss)
    rh1 = sum(1 for r in rh if r[1] == "1")
    dbg = open(run + "/debug.log", errors="replace").read() if os.path.exists(run + "/debug.log") else ""
    aus = len(re.findall(r"AUSNAHME|Access violation|0xC0000005", dbg))
    pid = open(run + "/pid.txt").read() if os.path.exists(run + "/pid.txt") else ""
    ex = re.findall(r"exit=(\d+)", pid)
    schlecht = sum(n.get(k, 0) for k in ("deck_verlassen", "aussen", "nische", "im_block"))
    teile = ", ".join("%s %d" % (k, n.get(k, 0)) for k in ("wasser", "oben", "klettern", "deck_verlassen", "aussen", "nische", "im_block"))
    erste = "; ".join("erstes %s F%d bei (%d,%d,%d)" % (k, erst[k]["F"], erst[k]["x"], erst[k]["y"], erst[k]["z"]) for k in ("deck_verlassen", "aussen", "nische", "im_block") if k in erst)
    print("%-44s exit %s, %5d Bilder in 2090/2091 (state %d, climb %d): %s -> %s%s; RAUMHALT %d (Band 1: %d, Band 0: %d)%s; Ausnahme-Zeilen %d" % (
        name, ",".join(ex) or "?", len(st), nsl, ncl, teile, "NIE ausserhalb" if schlecht == 0 else "AUSSERHALB %d" % schlecht, ("; " + erste) if erste else "",
        len(rh), rh1, len(rh) - rh1, (" (erste F%s)" % rh[0][0]) if rh else "", aus))
