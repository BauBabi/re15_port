"""Pixelvergleich zweier PPM-Reihen aus elza_run.sh.

Aufruf:  python elza_cmp.py <marke_a> <marke_b>
Gibt je Bildnummer die Zahl abweichender Pixel und die Gesamtzahl aus.
"""
import sys, os, glob

MESS = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                    "..", "..", "re15_port", "build", "platform", "pc", "mess")


def ppm(path):
    d = open(path, "rb").read()
    # P6\n<w> <h>\n<max>\n
    parts = []
    i = 0
    while len(parts) < 4:
        while i < len(d) and d[i:i + 1].isspace():
            i += 1
        if d[i:i + 1] == b"#":
            while i < len(d) and d[i:i + 1] != b"\n":
                i += 1
            continue
        j = i
        while j < len(d) and not d[j:j + 1].isspace():
            j += 1
        parts.append(d[i:j])
        i = j
    i += 1
    w, h = int(parts[1]), int(parts[2])
    return w, h, d[i:i + w * h * 3]


def main():
    a, b = sys.argv[1], sys.argv[2]
    fa = sorted(glob.glob(os.path.join(MESS, a + "_*.ppm")))
    total_diff = 0
    for pa in fa:
        nr = os.path.basename(pa).rsplit("_", 1)[1]
        pb = os.path.join(MESS, b + "_" + nr)
        if not os.path.exists(pb):
            print("%s fehlt bei %s" % (nr, b))
            continue
        wa, ha, da = ppm(pa)
        wb, hb, db = ppm(pb)
        if (wa, ha) != (wb, hb):
            print("%s GROESSE %dx%d vs %dx%d" % (nr, wa, ha, wb, hb))
            continue
        n = sum(1 for k in range(0, len(da), 3) if da[k:k + 3] != db[k:k + 3])
        total_diff += n
        print("%s  abweichende Pixel %d von %d" % (nr.replace(".ppm", ""), n, wa * ha))
    print("SUMME abweichende Pixel:", total_diff)


main()
