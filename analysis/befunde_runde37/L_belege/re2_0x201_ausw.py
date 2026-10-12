"""Auswertung re2_gdb_0x201.py (Runde 37 Spur L): jeder Wechsel eines RE2-Zombies in den Arme-hoch-Gang +0x5 == 2
(Zustandswort 0x201), mit Zuordnung zum Produzenten nach dem Original-Wortlaut (EMZ0.BIN, EXEC[1] @0x80101A40):
  (1) TIMER      @0x80101D68-DBC  +0x15A lief im Takt ab (vorher 1, danach (rand>>3)+100 = 100..131), dist < 0x1388
  (2) NAHBEREICH @0x80101EC4-F10  dist < 0x7D0, Spieler im +-512-Kegel des Zombies, 0x800CFDCB & 0x80 == 0
  (3) RENNEN     @0x80101F14-F60  0x800CFBF6 & 4, dist < 0x9C4, Spieler im +-1324-Kegel
Dazu: was danach kommt (Griff 0x301 / zurueck 0x101 / ...), Dauer im Gang, Weg je Takt in 0x101 und 0x201.
Kegeltest wie 0x80015614: ((Peilung - Gier + h) & 0xfff) < 2h. Peilung = -atan2(dz,dx) in 4096-Teilung — empirisch aus
denselben Takten bestimmt (Gier gehender Zombies vs. Richtung zum Steuerziel +0x1C4/+0x1C6: 3508 von 4188 Takten innerhalb
+-128; die Bewegung 0x800152C8 ist x += cos(Gier), z -= sin(Gier)). Nahe der Kegelkante unscharf, deshalb "kante" mit aus.
Aufruf: python re2_0x201_ausw.py <takte.txt|takte.txt.gz>"""
import gzip, math, sys
from collections import defaultdict


def peilung(x0, z0, x1, z1):
    return int(round(-math.atan2(z1 - z0, x1 - x0) * 2048.0 / math.pi)) & 0xfff


def kegel(yaw, peil, h):
    return ((peil - yaw + h) & 0xfff) < 2 * h


def main():
    reihen = defaultdict(list)
    quelle = gzip.open(sys.argv[1], "rt") if sys.argv[1].endswith(".gz") else open(sys.argv[1])
    for z in quelle:
        if z.startswith("#"):
            continue
        a, b = z.split("|")
        f = a.split(); g = b.split()
        r = dict(n=int(f[0]), ent=f[1], w4=int(f[3], 16), x=int(f[4]), z=int(f[5]), yaw=int(f[6]), d=int(f[7]),
                 los=int(f[8], 16), t158=int(f[9]), t15a=int(f[10]), b16a=int(f[11]), sx=int(f[12]), sz=int(f[13]),
                 plx=int(g[0]), plz=int(g[1]), bf6=int(g[3], 16), dcb=int(g[4], 16))
        reihen[r["ent"]].append(r)
    gesamt = defaultdict(int)
    weg = defaultdict(lambda: [0, 0])
    for ent, rs in reihen.items():
        rs.sort(key=lambda r: r["n"])
        for i in range(1, len(rs)):
            a, b = rs[i - 1], rs[i]
            s1a, s1b = (a["w4"] >> 8) & 0xff, (b["w4"] >> 8) & 0xff
            sta, stb = a["w4"] & 0xff, b["w4"] & 0xff
            if sta == 1 and stb == 1 and s1a == s1b and s1a in (1, 2) and ((a["w4"] >> 16) & 0xff) and ((b["w4"] >> 16) & 0xff):
                weg[s1a][0] += 1
                weg[s1a][1] += abs(b["x"] - a["x"]) + abs(b["z"] - a["z"])
            if not (stb == 1 and s1b == 2 and not (sta == 1 and s1a == 2)):
                continue
            # Der Haltepunkt steht VOR `sw v0,496(s0)`: +0x1F0 in Probe n ist der Abstand, den die KI im Takt n-1 (dem
            # Wechsel-Takt zwischen Probe n-1 und n) benutzt hat -> Abstand aus b, Lage/Gier/Spieler aus b (nach dem Takt).
            dd = b["d"]
            p = peilung(b["x"], b["z"], b["plx"], b["plz"])
            k512, k1324 = kegel(b["yaw"], p, 512), kegel(b["yaw"], p, 1324)
            rand_kante = min((p - b["yaw"] + 512) & 0xfff, abs(((p - b["yaw"] + 512) & 0xfff) - 1024))
            prod = []
            if a["t15a"] == 1 and 100 <= b["t15a"] <= 131 and dd < 0x1388:
                prod.append("(1)")
            if dd < 0x7D0 and k512 and not (a["dcb"] & 0x80):
                prod.append("(2)")
            if (a["bf6"] & 4) and dd < 0x9C4 and k1324:
                prod.append("(3)")
            # was danach: der naechste Zustand mit s1 != 2
            nach, dauer = None, 0
            for j in range(i, len(rs)):
                if (rs[j]["w4"] & 0xff) == 1 and ((rs[j]["w4"] >> 8) & 0xff) == 2:
                    dauer += 1
                    continue
                nach = "%08x" % rs[j]["w4"]; break
            gesamt["wechsel"] += 1
            for q in prod or ["?"]:
                gesamt[q] += 1
            print("%s n=%d von %08x d(Takt)=%d peil=%d gier=%d k512=%d k1324=%d (kante %d) bf6=%04x dcb=%02x t15a %d->%d "
                  "=> %s | %d Takte in 0x201, danach %s (d=%s)" % (
                      ent, b["n"], a["w4"], dd, p, b["yaw"], k512, k1324, rand_kante, a["bf6"], a["dcb"],
                      a["t15a"], b["t15a"], "+".join(prod) or "?", dauer, nach,
                      rs[min(i + dauer, len(rs) - 1)]["d"]))
    print("SUMME", dict(gesamt))
    for s1, (n, w) in sorted(weg.items()):
        print("Weg je Takt s1=%d: %.1f (%d Takte)" % (s1, w / n if n else 0.0, n))


if __name__ == "__main__":
    main()
