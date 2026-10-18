"""re2_stops_auswerten.py - Runde 39 Spur D: stops.txt eines re2_gdb_aufsteher.py-Laufs auswerten.
Je SCHUSS-Stopp (Applier FUN_800410CC Eintritt): Zombie-Zustandswort w4 (EXEC = Byte 1, Phase = Byte 2), Clip-Bild (+0x14D),
Teile-Maske (w0>>26&7), HP davor; HP danach = naechste Probe (Rueckkehr-Stopp oder Herzschlag). Treffer = HP faellt.
Tabelle: Zustand x Bild<55 / >=55 x Maske -> Schuesse / Treffer. Aufruf: python re2_stops_auswerten.py <stops.txt>"""
import re, sys, collections

rx = re.compile(r"^\s*(\d+)\s+(\S+)\s+t=\s*([\d.]+) Z w0=([0-9a-f]{8}) maske=(\d) w4=([0-9a-f]{8}) clip=([0-9a-f]{2}) bild=\s*(\d+) rate=([0-9a-f]{2}) hp=\s*(-?\d+) d223=\s*(-?\d+) 21A=([0-9a-f]{4}) 1D3=([0-9a-f]{2}) 10E=([0-9a-f]{4}) y=(-?\d+) dist=(\d+) \| L aim=([0-9a-f]{4})")

def main(p):
    rows = []
    for line in open(p, encoding="utf-8", errors="replace"):
        m = rx.match(line)
        if not m: continue
        n, tag, t, w0, maske, w4, clip, bild, rate, hp, d223, f21a, d1d3, f10e, y, dist, aim = m.groups()
        rows.append(dict(n=int(n), tag=tag, t=float(t), w0=int(w0, 16), maske=int(maske), w4=int(w4, 16), clip=int(clip, 16),
                         bild=int(bild), hp=int(hp), d223=int(d223), f21a=int(f21a, 16), d1d3=int(d1d3, 16), f10e=int(f10e, 16),
                         y=int(y), dist=int(dist), aim=int(aim, 16)))
    print("%d Proben, %d Schuss-Stopps" % (len(rows), sum(1 for r in rows if r["tag"] == "SCHUSS")))
    stat = collections.OrderedDict()
    schuesse = []
    for i, r in enumerate(rows):
        if r["tag"] != "SCHUSS": continue
        nach = next((q for q in rows[i + 1:] if q["tag"] != "SCHUSS"), None)
        hp_nach = nach["hp"] if nach else r["hp"]
        treffer = hp_nach < r["hp"]
        exec_, phase = (r["w4"] >> 8) & 0xff, (r["w4"] >> 16) & 0xff
        zustand = "EXEC[%d]P%d" % (exec_, phase) if (r["w4"] & 0xff) == 1 else "w4=%08x" % r["w4"]
        bk = "Bild<55" if r["bild"] < 55 else "Bild>=55"
        key = (zustand, r["clip"], bk, r["maske"], "aim%04x" % (r["aim"] & 0xe000))
        s = stat.setdefault(key, [0, 0]); s[0] += 1; s[1] += int(treffer)
        schuesse.append((r["t"], zustand, r["clip"], r["bild"], r["maske"], r["hp"], hp_nach, treffer, r["dist"], r["aim"]))
    print("\nje Schuss: t  Zustand  clip bild maske  hp->hp'  Treffer  dist aim")
    for t, z, c, b, mk, hp, hpn, tr, d, aim in schuesse:
        print("  %6.2f %-12s %02x %3d  %d   %4d->%4d  %s  %5d %04x" % (t, z, c, b, mk, hp, hpn, "TREFFER" if tr else "-      ", d, aim))
    print("\nZustand x Clip x Bild x Maske x Zielhoehe -> Schuesse/Treffer")
    for k, (n, tr) in stat.items():
        print("  %-12s clip %02x %-8s maske %d %s : %3d / %3d" % (k[0], k[1], k[2], k[3], k[4], n, tr))
    # Phasen-Zeitleiste (Herzschlag): wann EXEC[5] P0..P8
    print("\nZeitleiste EXEC-Phasen (erste Probe je Wechsel):")
    last = None
    for r in rows:
        exec_, phase = (r["w4"] >> 8) & 0xff, (r["w4"] >> 16) & 0xff
        z = (r["w4"] & 0xff, exec_, phase)
        if z != last:
            print("  t=%6.2f w4=%08x clip=%02x bild=%3d maske=%d hp=%d 21A=%04x 1D3=%02x 10E=%04x" % (
                r["t"], r["w4"], r["clip"], r["bild"], r["maske"], r["hp"], r["f21a"], r["d1d3"], r["f10e"]))
            last = z

if __name__ == "__main__":
    main(sys.argv[1])
