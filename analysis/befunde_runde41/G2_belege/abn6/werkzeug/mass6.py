"""mass6.py <laufordner> [...]: Mass der Abnahme 6 = mass5.py der Abnahme 5 (unabhaengig von obenlog/haenglog des Bauers) + Basis "ab Kampfbeginn"
(Ende des RE2-Intros, gator_boss.log "KAMERA INTRO frei ... F<n>") fuer die Spawn-Laeufe und RAUMHALT-Zeilen.
Letzter Besuch von 2090/2091 (state.log F faellt auf 0 zurueck; climb.log T-Zeile i = state.log F-Zeile i, room= wird mitgefuehrt).
A (oben): Figur oben = y < -900 im Grundriss Rampe (1850..7200, -18100..-14500) oder Plattform (-1700..1850, -20000..-12450); Treffer = hp-Flanke < 100;
   Art: RAMMEN-Zeile (gator_boss.log) > TRAG-Phase 7 = CROSS-Biss > Gator-Clip 4 = Hochbiss > TRAG-Phase 3 = Biss; je Oben-Abschnitt erster Treffer und
   laengste Pause ab dem ERSTEN Oben-Bild (Mass des Messblatts A2) und ab "KANISTER angehaengt" (Lesart obenlog).
B (Haengen): Haenge-Fenster wie gator_rw_r41.c (b) (150 Bilder Phase 2 ohne Biss/Treffer, Kasten < 2500, Kopf 3236 vor der Wurzel > 4500, netto < 600/60),
   ohne Fortschritt zur Figur (mass_v4.py), laengste Pause ohne Treffer im ganzen Besuch und waehrend die Figur im Wasser steht (Messblatt B1b), Rueckfall-Zeilen.
Kanister: "Kanister-Sitz F...: angehaengt N Bilder, Abweichung max M" (debug.log)."""
import re, sys, math, os

RP = (1850, 7200, -18100, -14500)
PP = (-1700, 1850, -20000, -12450)


def im(r, x, z):
    return r[0] < x < r[1] and r[2] < z < r[3]


def im_band(x, z, m=1800):
    if im(RP, x, z) or im(PP, x, z):
        return False
    return (PP[0] - m <= x <= PP[1] + m and PP[2] - m <= z <= PP[3] + m) or (RP[0] - m <= x <= RP[1] + m and RP[2] - m <= z <= RP[3] + m)


def lies(run):
    sl = [l for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
    cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")] if os.path.exists(run + "/climb.log") else []
    start = 0
    prevF = None
    for i, l in enumerate(sl):   # letzter Besuch = ab dem letzten Ruecksprung der F-Zahl (Folgebesuche beginnen mit F1)
        mf = re.match(r"F(\d+) ", l)
        if not mf:
            continue
        f = int(mf.group(1))
        if prevF is not None and f < prevF:
            start = i
        prevF = f
    st = []
    for i in range(start, len(sl)):
        m = re.match(r"F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? mo=(\d+).*?cam=(\d+)", sl[i])
        if not m:
            continue
        g = re.search(r"\[35 t=23 st=(\d+) ss1=(\d+).*?mo=(\d+) af=(\d+) stun=(\d+) .*?@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)", sl[i])
        y = None
        room = None
        if i < len(cl):
            c = re.search(r"room=(\w+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\)", cl[i])
            if c:
                room = c.group(1)
                y = int(c.group(3))
        st.append(dict(F=int(m.group(1)), px=int(m.group(3)), pz=int(m.group(4)), rot=int(m.group(5)), hp=int(m.group(6)), pmo=int(m.group(7)),
                       cam=int(m.group(8)), y=y, room=room, gss1=int(g.group(2)) if g else -1, gmo=int(g.group(3)) if g else -1,
                       gaf=int(g.group(4)) if g else -1, gx=int(g.group(6)) if g else None, gz=int(g.group(7)) if g else None,
                       gr=int(g.group(8)) if g else 0, ghp=int(g.group(9)) if g else None))
    return st


def main(run, zeilen=40):
    st = lies(run)
    name = run.replace("\\", "/").rstrip("/").split("/")[-1]
    rooms = sorted(set(s["room"] for s in st if s["room"]))
    trag = {}
    if os.path.exists(run + "/gator_trag.log"):
        for l in open(run + "/gator_trag.log", errors="replace"):
            m = re.match(r"TRAG F(\d+) ph=(\d+) mo=(\d+) af=(\d+) gier=(\d+) pos=\((-?\d+),(-?\d+)\).*spieler=\((-?\d+),(-?\d+)\)", l)
            if m:
                trag[int(m.group(1))] = tuple(int(v) for v in m.groups())
    boss = open(run + "/gator_boss.log", errors="replace").read() if os.path.exists(run + "/gator_boss.log") else ""
    ramm = set(int(x) for x in re.findall(r"RAMMEN F(\d+)", boss))
    auf = re.findall(r"KANISTER angehaengt.*?F(\d+)", boss)
    auf = int(auf[0]) if auf else None
    kb = re.findall(r"KAMERA INTRO frei.*? F(\d+)", boss)
    kampf = int(kb[0]) if kb else None
    raumhalt = len(re.findall(r"RAUMHALT F", boss))
    haengt = len(re.findall(r"G2RE2 HAENGT", boss))
    rw = len(re.findall(r"RUECKWAERTS an", boss))
    ta = len(re.findall(r"TAUCHEN an", boss))
    dbg = open(run + "/debug.log", errors="replace").read()
    sitz = re.findall(r"Kanister-Sitz F(\d+): angehaengt (\d+) Bilder, Abweichung max (\d+)", dbg)
    fehler = len(re.findall(r"(?i)\b(assert|abort|segfault|crash)\b", dbg))
    exitat = "EXIT_AT" in dbg
    pid = open(run + "/pid.txt").read().replace("\n", " ") if os.path.exists(run + "/pid.txt") else ""
    print("== %s: %s exe %s; Besuch %d Bilder F%d..F%d, Raeume %s, Kampfbeginn (Intro-Ende) %s, RAUMHALT %d, Kanister angehaengt %s, Kanister-Sitz %s, Fehlerzeilen %d, EXIT_AT %s" % (
        name, pid.strip(), (open(run + "/exe_md5.txt").read()[:8] if os.path.exists(run + "/exe_md5.txt") else "ctest"), len(st), st[0]["F"] if st else -1, st[-1]["F"] if st else -1, rooms,
        ("F%d" % kampf) if kampf else "-", raumhalt, ("F%d" % auf) if auf else "nie",
        ("%s Bilder, Abw. max %s (letzte Zeile F%s)" % (sitz[-1][1], max(int(s[2]) for s in sitz), sitz[-1][0])) if sitz else "-", fehler, exitat))

    def art(s):
        F = s["F"]
        t = trag.get(F)
        if F in ramm:
            return "Rammen"
        if t:
            if t[1] == 7:
                return "CROSS-Biss"
            if t[1] == 5 and s["gmo"] == 4:
                return "Hochbiss"
            if t[1] == 3:
                return "Biss"
            return "ph%d/mo%d" % (t[1], s["gmo"])
        if s["gmo"] == 4:   # ohne TRAG-Log (kein Kanister im Lauf): Clip 4 = Biss-Clip; oben = Hochbiss, im Wasser = Biss
            return "Hochbiss" if (s["y"] is not None and s["y"] < -900) else "Biss"
        return "ph?/mo%d" % s["gmo"]

    def oben(s):
        return s["y"] is not None and s["y"] < -900 and (im(RP, s["px"], s["pz"]) or im(PP, s["px"], s["pz"]))

    tr = [i for i, s in enumerate(st) if s["hp"] < 100 and (i == 0 or st[i - 1]["hp"] >= 100)]
    trs = set(tr)
    arten = {}
    for i in tr:
        a = art(st[i])
        arten[a] = arten.get(a, 0) + 1
    print("   Treffer gesamt %d: %s" % (len(tr), ", ".join("%s %d" % kv for kv in sorted(arten.items()))))
    # A: Oben-Abschnitte
    abschn = []
    a = None
    for i, s in enumerate(st):
        o = oben(s)
        if o and a is None:
            a = i
        if not o and a is not None:
            abschn.append((a, i - 1))
            a = None
    if a is not None:
        abschn.append((a, len(st) - 1))
    oben_n = sum(b - a + 1 for a, b in abschn)
    print("   A oben: %d Bilder in %d Abschnitten" % (oben_n, len(abschn)))
    for (a, b) in abschn:
        if b - a + 1 < 30:
            continue
        tt = [i for i in tr if a <= i <= b]
        for basis in ("ab erstem Oben-Bild", "ab Kampfbeginn", "ab Anhaengen"):
            ab = a
            if basis == "ab Kampfbeginn":
                if kampf is None:
                    continue
                ab = next((i for i in range(a, b + 1) if st[i]["F"] >= kampf), None)
                if ab is None or ab == a:
                    continue
            if basis == "ab Anhaengen":
                if auf is None:
                    continue
                ab = next((i for i in range(a, b + 1) if st[i]["F"] >= auf), None)
                if ab is None or ab == a:
                    continue
            t2 = [i for i in tt if i >= ab]
            pausen = []
            letzt = ab
            for i in t2:
                pausen.append((i - letzt, st[letzt]["F"], st[i]["F"]))
                letzt = i
            ende_rest = (b - letzt, st[letzt]["F"], st[b]["F"])
            pm = max(pausen + [ende_rest])
            print("     oben F%d..F%d (%d B. = %.1f s) %s F%d: Treffer %d, erster nach %s B., laengste Pause %d B. = %.1f s (F%d..F%d), Rest am Ende %d B.%s" % (
                st[a]["F"], st[b]["F"], b - a + 1, (b - a + 1) / 30.0, basis, st[ab]["F"], len(t2), (t2[0] - ab) if t2 else "KEIN", pm[0], pm[0] / 30.0,
                pm[1], pm[2], ende_rest[0], " (Laufende)" if b == len(st) - 1 else " (verlassen)"))
        print("       Treffer: " + ", ".join("F%d %s -%d (%d,%d)" % (st[i]["F"], art(st[i]), 100 - st[i]["hp"], st[i]["px"], st[i]["pz"]) for i in tt[:zeilen]) + (" ..." if len(tt) > zeilen else ""))
        sch = {}
        for i in tt:
            sch.setdefault(art(st[i]), []).append(100 - st[i]["hp"])
        print("       Schaden je Art (oben): " + "; ".join("%s %d x, je %s" % (k, len(v), "/".join(str(x) for x in sorted(set(v)))) for k, v in sorted(sch.items())))
    # B: Haengen (aus TRAG wenn vorhanden, sonst aus state.log)
    rows = []
    for s in st:
        t = trag.get(s["F"])
        if t:
            rows.append(dict(F=s["F"], ph=t[1], gier=t[4], gx=t[5], gz=t[6], px=t[7], pz=t[8], hp=s["hp"], y=s["y"]))
        elif not trag and s["gx"] is not None:
            rows.append(dict(F=s["F"], ph=None, gier=s["gr"], gx=s["gx"], gz=s["gz"], px=s["px"], pz=s["pz"], hp=s["hp"], y=s["y"]))

    def kd(r):
        w = r["gier"] * 2 * math.pi / 4096
        return math.hypot(r["gx"] + math.cos(w) * 3236 - r["px"], r["gz"] - math.sin(w) * 3236 - r["pz"])

    best = None
    lauf = 0
    ohne = (0, -1)
    p0 = None
    for r in rows:
        k = kd(r)
        if p0 is None or k <= 4500 or math.hypot(r["px"] - p0[0], r["pz"] - p0[1]) > 1500 or k < best - 300:
            lauf = 0
            best = k
            p0 = (r["px"], r["pz"])
        else:
            lauf += 1
            if lauf > ohne[0]:
                ohne = (lauf, r["F"])
    fenster = []
    if trag:
        seit = 1 << 20
        hist = []
        for r in rows:
            if r["ph"] in (3, 8) or r["hp"] < 100:
                seit = 0
            else:
                seit += 1
            if r["ph"] != 2:
                hist = []
                continue
            hist.append(r)
            if len(hist) >= 150 and seit >= 150:
                w = hist[-150:]
                xs = [h["gx"] for h in w]
                zs = [h["gz"] for h in w]
                nx, nz = r["gx"] - w[-60]["gx"], r["gz"] - w[-60]["gz"]
                if max(xs) - min(xs) < 2500 and max(zs) - min(zs) < 2500 and kd(r) > 4500 and math.hypot(nx, nz) < 600:
                    fenster.append((w[0]["F"], r["F"], min(xs), max(xs), min(zs), max(zs), r["px"], r["pz"], int(kd(r))))
                    hist = []

    # laengste Pause ohne Treffer innerhalb zusammenhaengender Index-Abschnitte (Beginn eines Abschnitts zaehlt als Start)
    def pmax(idx):
        best = (0, None, None)
        letzt = None
        prev = None
        for i in idx:
            if prev is None or i != prev + 1:
                letzt = i
            g = i - letzt
            if g > best[0]:
                best = (g, st[letzt]["F"], st[i]["F"])
            if i in trs:
                letzt = i
            prev = i
        return best

    alle = list(range(len(st)))
    wasser = [i for i in alle if not oben(st[i])]
    pw = pmax(wasser)
    pa = pmax(alle)
    band_n = sum(1 for s in st if not oben(s) and im_band(s["px"], s["pz"]))
    print("   B: ohne Fortschritt max %d B. = %.1f s (bis F%d); Haenge-Fenster %d%s; laengste Pause ohne Treffer: ganzer Besuch %d B. (F%s..F%s), "
          "Figur im Wasser %d B. (F%s..F%s); Figur im Band %d Bilder; Zeilen HAENGT %d, RUECKWAERTS an %d, TAUCHEN an %d" % (
              ohne[0], ohne[0] / 30.0, ohne[1], len(fenster),
              (": " + "; ".join("F%d-F%d Kasten (%d..%d, %d..%d) Figur (%d,%d) Kopf %d" % f for f in fenster)) if fenster else "",
              pa[0], pa[1], pa[2], pw[0], pw[1], pw[2], band_n, haengt, rw, ta))
    if trag:
        phn = {}
        for r in rows:
            phn[r["ph"]] = phn.get(r["ph"], 0) + 1
        print("   Phasen (TRAG): " + ", ".join("%d:%d" % kv for kv in sorted(phn.items())))


if __name__ == "__main__":
    for r in sys.argv[1:]:
        main(r)
