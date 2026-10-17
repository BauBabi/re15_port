"""vergleich_orig.py - Runde 38 Spur D "zopf", Dossier D_zopf.md §4.4: Zopf an der EXE (RE15_ZOPF_LOG eines Laufs von
mess_laufen.sh origfolge/origfolge29) gegen den Zopf im ORIGINAL (orig_zopf_frames.txt, DuckStation-GDB @0x80024c30 /
@0x80024e10, gleiche Stelle ROOM1121 (150,9550), gleiche Eingabe).

Ausgerichtet wird an zwei Ereignissen, nicht an Bildnummern (Vorspann/Leerlauf-Animation sind verschieden lang):
  Drehbeginn  erstes Bild mit Blick != Startblick   (Original Bild 16)
  Gehbeginn   erstes Bild mit Lage != Lage davor     (Original Bild 135)
Je Fenster und Glied 17..20: mittlere und groesste Abweichung von xang (rot.vx) und zang (rot.vz) in 4096tel, dazu die
Eingaenge (Blick, Teil-0/Teil-8-Winkel der Animation, Nacken-Gier Teil 8 +0x94, Schrittweite der Lage).
Das Gehfenster endet am ersten Bild, in dem die EINGAENGE auseinanderlaufen: Nacken-Gier verschieden (der Port-Nacken
dreht in ROOM1121 nahe (1297,7071) auf -512, das Original bleibt 0) oder ein Lauf steht an der Wand (Schritt < 50).

Aufruf: python -I vergleich_orig.py <zopf.log> <orig_zopf_frames.txt>"""
import math
import re
import sys

ZPAT = re.compile(r"^Z(\d+) s0=(-?\d+)(.*?) kopf=.*? ein=\(ent (-?\d+),(-?\d+),(-?\d+) t0vy (-?\d+) t0vz (-?\d+) "
                  r"t8vy (-?\d+) ng (-?\d+) nf ([0-9a-f]+)\) pos=(-?\d+),(-?\d+),(-?\d+)")
GPAT = re.compile(r"g(\d)=\((-?\d+),(-?\d+),(-?\d+) v94=(-?\d+) v96=(-?\d+) t=(-?\d+),(-?\d+),(-?\d+)\)")


def port_lesen(p):
    rows = []
    for line in open(p):
        if line.startswith("R kommando0"):
            rows = []
            continue
        m = ZPAT.match(line)
        if not m:
            continue
        g = [list(map(int, x[1:])) for x in GPAT.findall(m.group(3))]
        rows.append(dict(yaw=int(m.group(5)), a=[int(m.group(7)), int(m.group(8)), int(m.group(9)), int(m.group(10))],
                         ng=int(m.group(10)),
                         pos=(int(m.group(12)), int(m.group(13)), int(m.group(14))),
                         rot=[x[0:3] for x in g], v=[x[3:5] for x in g]))
    return rows


def orig_lesen(p):
    rows = []
    for line in open(p):
        t = line.split()
        if not t or t[0] != "F":
            continue
        k = t.index("ent")
        e = list(map(int, t[k + 1:k + 7]))
        k = t.index("a")
        a = list(map(int, t[k + 1:k + 6]))
        k = t.index("nach")
        v = list(map(int, t[k + 1:k + 1 + 4 * 17]))
        nach = [v[j * 17:(j + 1) * 17] for j in range(4)]       # m[9] w[3] rot[3] v94 v96
        rows.append(dict(i=int(t[1]), phase=t[2], yaw=e[1], pos=(e[3], e[4], e[5]), a=a[0:4], ng=a[3],
                         rot=[g[12:15] for g in nach], v=[g[15:17] for g in nach]))
    return rows


def w12(d):
    """Winkeldifferenz modulo 4096 nach [-2048, 2047]."""
    return (d + 2048) % 4096 - 2048


def beginn(rows, art):
    for i in range(1, len(rows)):
        if art == "dreh" and rows[i]["yaw"] != rows[0]["yaw"]:
            return i
        if art == "geh" and rows[i]["pos"] != rows[i - 1]["pos"]:
            return i
    return None


def fenster(name, port, orig, p0, o0, n):
    n = min(n, len(port) - p0, len(orig) - o0)
    print("== %s: Port Schritte %d..%d gegen Original Bilder %d..%d (%d Bilder)" %
          (name, p0, p0 + n - 1, orig[o0]["i"], orig[o0 + n - 1]["i"], n))
    dy = [abs(port[p0 + j]["yaw"] - orig[o0 + j]["yaw"]) for j in range(n)]
    da = [max(abs(w12(port[p0 + j]["a"][q] - orig[o0 + j]["a"][q])) for q in range(3)) for j in range(n)]
    print("   Eingang: Blick-Abweichung max %d; Animationswinkel Teil0 vy/vz + Teil8 vy max %d (Mittel %.1f)" %
          (max(dy), max(da), sum(da) / n))
    sp = [math.dist(port[p0 + j]["pos"], port[p0 + j - 1]["pos"]) for j in range(1, n)]
    so = [math.dist(orig[o0 + j]["pos"], orig[o0 + j - 1]["pos"]) for j in range(1, n)]
    if max(sp) > 0 or max(so) > 0:
        bew_p = [x for x in sp if x > 0]
        bew_o = [x for x in so if x > 0]
        print("   Schrittweite der Lage: Port %.0f..%.0f (Mittel %.1f), Original %.0f..%.0f (Mittel %.1f)" %
              (min(bew_p), max(bew_p), sum(bew_p) / len(bew_p), min(bew_o), max(bew_o), sum(bew_o) / len(bew_o)))
    for k in range(4):
        for ax, nm in ((0, "xang"), (2, "zang")):
            d = [abs(w12(port[p0 + j]["rot"][k][ax] - orig[o0 + j]["rot"][k][ax])) for j in range(n)]
            spanne = max(orig[o0 + j]["rot"][k][ax] for j in range(n)) - min(orig[o0 + j]["rot"][k][ax] for j in range(n))
            print("   Glied %d %s: Abweichung Mittel %6.1f max %4d (= %.1f Grad) | Original-Spanne im Fenster %d" %
                  (17 + k, nm, sum(d) / n, max(d), max(d) * 360.0 / 4096, spanne))


def main():
    port = port_lesen(sys.argv[1])
    orig = orig_lesen(sys.argv[2])
    pd, od = beginn(port, "dreh"), beginn(orig, "dreh")
    pg, og = beginn(port, "geh"), beginn(orig, "geh")
    print("Port: %d Schritte, Drehbeginn %s, Gehbeginn %s | Original: %d Bilder, Drehbeginn Bild %s, Gehbeginn Bild %s" %
          (len(port), pd, pg, len(orig), od, og))
    fenster("Drehung + Nachlauf", port, orig, pd, od, og - od)
    fenster("Gehen + Stehen (alle Bilder)", port, orig, pg, og, len(orig) - og)
    n = 0
    while pg + n < len(port) and og + n < len(orig):
        pp, oo = port[pg + n], orig[og + n]
        sp = math.dist(pp["pos"], port[pg + n - 1]["pos"])
        so = math.dist(oo["pos"], orig[og + n - 1]["pos"])
        if pp["ng"] != oo["ng"] or sp < 50 or so < 50:
            print("   Gehfenster endet bei Port-Schritt %d / Original-Bild %d: Nacken Port %d / Original %d, Schritt %.0f / %.0f"
                  % (pg + n, oo["i"], pp["ng"], oo["ng"], sp, so))
            break
        n += 1
    fenster("Gehen bis die Eingaenge auseinanderlaufen", port, orig, pg, og, n)
    print("   Endblick Port %d / Original %d, Endlage Port %s / Original %s" %
          (port[-1]["yaw"], orig[-1]["yaw"], port[pg + len(orig) - og - 1]["pos"] if pg + len(orig) - og - 1 < len(port)
           else port[-1]["pos"], orig[-1]["pos"]))


if __name__ == "__main__":
    main()
