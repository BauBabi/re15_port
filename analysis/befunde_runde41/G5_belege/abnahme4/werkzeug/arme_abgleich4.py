# Abnahme 4: Port-Arme (Messschiene RE15_G5A_SPUR des Ablauftests, Engine-Diagnosen) gegen eigenen RE2-RAM-Leser, ALLE Savestates
import json, sys, re
def w12(v):
    v &= 0xfff
    return v - 4096 if v >= 2048 else v
def spur(p):
    out = {}
    for z in open(p):
        k = z.split("||")
        a = k[0].split("|")[0].split()
        b = int(a[0])
        d = {"b": b, "cam": int(a[1]), "plx": int(a[2]), "plz": int(a[3]), "ex": int(a[4]), "ez": int(a[5]), "clip": int(a[6]), "af": int(a[7]),
             "gs": int(a[8]), "gp": int(a[9]), "sc": [int(a[15 + 4 * i]) for i in range(4)]}
        arme = re.findall(r"a(\d) s(\d+) p(\d+)", k[0])
        d["sub"] = {int(i): int(s) for i, s, p in arme}; d["ph"] = {int(i): int(p) for i, s, p in arme}
        m = re.search(r"g5t (-?\d+)", k[1]); d["gt"] = int(m.group(1))
        d["m"] = {int(i): (int(t), w12(int(x)), w12(int(y)), w12(int(zz))) for i, t, x, y, zz in re.findall(r"m(\d) (-?\d+) (-?\d+) (-?\d+) (-?\d+)", k[1])}
        out[b] = d
    return out
re2 = json.load(open(sys.argv[1]))
for raum in ("5090", "5091"):
    P = spur(sys.argv[2] + "_%s.txt" % raum)
    gesamt = gleich = 0; felder_n = felder_ok = 0
    print("== Raum", raum)
    for o in re2:
        g = o["g5"]
        if g["ph"] == 0: print("s%02d G5 Ph 0 (Vorlauf, vor Bild 0) - nicht zugeordnet" % o["s"]); continue
        clipph = g["ph"] in (2, 6, 8, 12)
        kand = [b for b, d in sorted(P.items()) if d["gs"] == g["sub"] and d["gp"] == g["ph"] and (d["af"] == g["bild"] if clipph else d["gt"] == g["t"])]
        if not kand: print("s%02d keine Zuordnung (G5 ph %d t %d bild %d)" % (o["s"], g["ph"], g["t"], g["bild"])); continue
        b = kand[0]; d = P[b]
        zeile = []; alle_ok = True
        for i in range(4):
            a = o["arme"][str(i)]
            t, rx, ry, rz = d["m"][i]
            port = (d["sub"][i], d["ph"][i], d["sc"][i], t, rx, ry, rz)
            soll = (a["sub"], a["ph"], a["sc"], a["t"], a["rx"], a["ry"], a["rz"])
            okf = [p == r for p, r in zip(port, soll)]
            felder_n += 7; felder_ok += sum(okf)
            gesamt += 1
            ok_ohne_t = all(okf[:3] + okf[4:])
            if all(okf): gleich += 1; zeile.append("A%d =" % i)
            else:
                alle_ok = False
                zeile.append("A%d Port %s RE2 %s%s" % (i, port, soll, " (nur t)" if ok_ohne_t else ""))
        print("s%02d -> Bild %d (Kandidaten %s) G5 ph %d %s %d x Port %d RE2 %d | %s" % (o["s"], b, kand[:4], g["ph"], "bild" if clipph else "t", g["bild"] if clipph else g["t"],
              d["ex"], g["x"], "ALLE GLEICH" if alle_ok else "; ".join(zeile)))
    print("Raum %s: Arme gleich %d / %d, Felder gleich %d / %d" % (raum, gleich, gesamt, felder_ok, felder_n))
