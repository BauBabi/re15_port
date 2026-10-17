"""ppm_vergleich.py <ordner_a> <ordner_b> — vergleicht gleichnamige f*.ppm zweier Laeufe (mess_vorher_nachher.sh):
je Bild Zahl der verschiedenen Pixel (Kanalsumme > 0, also BITGENAU) und ihre Huelle. Exit 0 immer; die Zeilen sind
das Messergebnis (Dossier D_zopf.md §4)."""
import os, sys


def ppm(p):
    b = open(p, "rb").read()
    assert b[:2] == b"P6", p
    f, i = [], 2
    while len(f) < 3:
        while b[i:i + 1].isspace():
            i += 1
        if b[i:i + 1] == b"#":
            while b[i:i + 1] != b"\n":
                i += 1
            continue
        j = i
        while b[j:j + 1].isdigit():
            j += 1
        f.append(int(b[i:j]))
        i = j
    i += 1
    w, h = f[0], f[1]
    return w, h, b[i:i + w * h * 3]


def main():
    a, b = sys.argv[1], sys.argv[2]
    namen = sorted(n for n in os.listdir(a) if n.startswith("f") and n.endswith(".ppm"))
    if not namen:
        print("keine Bilder in", a)
        return 0
    for n in namen:
        pb = os.path.join(b, n)
        if not os.path.exists(pb):
            print("%s: fehlt in %s" % (n, b))
            continue
        w1, h1, d1 = ppm(os.path.join(a, n))
        w2, h2, d2 = ppm(pb)
        if (w1, h1) != (w2, h2):
            print("%s: Groesse %dx%d gegen %dx%d" % (n, w1, h1, w2, h2))
            continue
        diff, x0, y0, x1, y1 = 0, w1, h1, -1, -1
        for k in range(w1 * h1):
            o = k * 3
            if d1[o] != d2[o] or d1[o + 1] != d2[o + 1] or d1[o + 2] != d2[o + 2]:
                diff += 1
                x, y = k % w1, k // w1
                x0, y0, x1, y1 = min(x0, x), min(y0, y), max(x1, x), max(y1, y)
        if diff:
            print("%s: %d Pixel verschieden, Huelle (%d,%d)-(%d,%d)" % (n, diff, x0, y0, x1, y1))
        else:
            print("%s: bitgleich (%dx%d)" % (n, w1, h1))
    return 0


if __name__ == "__main__":
    sys.exit(main())
