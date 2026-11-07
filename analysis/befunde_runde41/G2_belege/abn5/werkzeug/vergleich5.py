"""vergleich5.py <lauf_alt> <lauf_neu>: Bild-fuer-Bild-Vergleich zweier exe-Laeufe gleicher Eingabe (Abnahme 5, Punkt 3c).
Je Besuch (state.log-Folge, F faellt zurueck; climb.log T-Zeile i = state.log F-Zeile i) je Bild die Figur (x, z, rot, hp, mo) und den Gator ([35 ...]-Block);
im letzten Besuch dazu gator_trag.log (TRAG je Bild). Meldet die erste Abweichung und die Lage der Figur davor: OBEN (y < -900 auf Rampe/Plattform),
BAND (bis 1800 vor einer Blockkante, nicht im Block), NISCHE (x > 7200 im Rampenband), IM-BLOCK oder frei. "bitgleich ausserhalb der beiden Orte" heisst:
die erste Abweichung liegt in einem Bild, in dem die Figur oben, im Band oder in der Nische steht (bzw. vom Koerper-Stoss dorthin geschoben wird)."""
import re, sys, os

RP = (1850, 7200, -18100, -14500)
PP = (-1700, 1850, -20000, -12450)


def im(r, x, z):
    return r[0] <= x <= r[1] and r[2] <= z <= r[3]


def ort(x, y, z, m=1800):
    if y is not None and y < -900 and (im(RP, x, z) or im(PP, x, z)):
        return "OBEN"
    if x > RP[1] and RP[2] <= z <= RP[3]:
        return "NISCHE"
    if im(RP, x, z) or im(PP, x, z):
        return "IM-BLOCK"
    if (PP[0] - m <= x <= PP[1] + m and PP[2] - m <= z <= PP[3] + m) or (RP[0] - m <= x <= RP[1] + m and RP[2] - m <= z <= RP[3] + m):
        return "BAND"
    return "frei"


def besuche(run):
    sl = [l.rstrip("\n") for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
    cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")] if os.path.exists(run + "/climb.log") else []
    b = []
    a = 0
    prev = None
    for i, l in enumerate(sl):
        f = int(re.match(r"F(\d+)", l).group(1))
        if prev is not None and f < prev:
            b.append((a, i - 1))
            a = i
        prev = f
    b.append((a, len(sl) - 1))
    out = []
    for (a, e) in b:
        mr = re.search(r"room=(\w+)", cl[a]) if a < len(cl) else None
        out.append((a, e, mr.group(1) if mr else "?"))
    return sl, cl, out


def lies(run, besuch=-1):
    sl, cl, bs = besuche(run)
    start, ende, _r = bs[besuch]
    st = {}
    for i in range(start, ende + 1):
        l = sl[i]
        m = re.match(r"F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(\d+),hp=(-?\d+)\).*? mo=(\d+)", l)
        if not m:
            continue
        g = re.search(r"\[35 t=23 .*?\] hp=(-?\d+)", l)
        y = None
        if i < len(cl):
            c = re.search(r"pos=\((-?\d+),(-?\d+),(-?\d+)\)", cl[i])
            if c:
                y = int(c.group(2))
        mc = re.search(r"cam=(\d+)", l)
        st[int(m.group(1))] = dict(pl=(int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7))), g=g.group(0) if g else "",
                                  px=int(m.group(3)), pz=int(m.group(4)), y=y, cam=mc.group(1) if mc else "?")
    trag = {}
    if besuch == -1 and os.path.exists(run + "/gator_trag.log"):
        for l in open(run + "/gator_trag.log", errors="replace"):
            m = re.match(r"TRAG F(\d+) (.*)", l.rstrip("\n"))
            if m:
                trag[int(m.group(1))] = m.group(2)
    return st, trag


def main(a, b):
    _, _, ba = besuche(a)
    _, _, bb = besuche(b)
    na, nb = a.replace("\\", "/").split("/")[-1], b.replace("\\", "/").split("/")[-1]
    print("== %s | %s - Besuche alt: %s | neu: %s" % (na, nb, ", ".join("%s:%d" % (r, e - s + 1) for s, e, r in ba), ", ".join("%s:%d" % (r, e - s + 1) for s, e, r in bb)))
    for k in range(min(len(ba), len(bb)) - 1):
        sa0, _ = lies(a, k)
        sb0, _ = lies(b, k)
        Fs0 = sorted(set(sa0) & set(sb0))
        d0 = [F for F in Fs0 if sa0[F]["pl"] != sb0[F]["pl"] or sa0[F]["g"] != sb0[F]["g"]]
        print("   Besuch %d (%s): %d Bilder gemeinsam, abweichend %d%s" % (k + 1, ba[k][2], len(Fs0), len(d0), (" (erste F%d)" % d0[0]) if d0 else " - BITGLEICH"))
    sa, ta = lies(a)
    sb, tb = lies(b)
    Fs = sorted(set(sa) & set(sb))
    d_st = [F for F in Fs if sa[F]["pl"] != sb[F]["pl"] or sa[F]["g"] != sb[F]["g"]]
    Ft = sorted(set(ta) & set(tb))
    d_tr = [F for F in Ft if ta[F] != tb[F]]
    print("   letzter Besuch (%s): state %d/%d gemeinsam %d, abweichend %d (erste F%s); TRAG %d/%d gemeinsam %d, abweichend %d (erste F%s)" % (
        ba[-1][2], len(sa), len(sb), len(Fs), len(d_st), d_st[0] if d_st else "-", len(ta), len(tb), len(Ft), len(d_tr), d_tr[0] if d_tr else "-"))
    if not d_st and not d_tr:
        print("   BITGLEICH")
        return
    F0 = min(d_st[:1] + d_tr[:1])
    orte = {}
    for F in range(max(Fs[0], F0 - 60), F0 + 1):
        if F in sa:
            o = ort(sa[F]["px"], sa[F]["y"], sa[F]["pz"])
            orte[o] = orte.get(o, 0) + 1
    s = sa.get(F0)
    print("   erste Abweichung F%d: Figur (%d,%s,%d) cam %s Ort %s; Ort der Figur in F%d..F%d: %s" % (
        F0, s["px"], s["y"], s["pz"], s["cam"], ort(s["px"], s["y"], s["pz"]), max(Fs[0], F0 - 60), F0, ", ".join("%s %d" % kv for kv in sorted(orte.items()))))
    erst_ort = next((F for F in Fs if F <= F0 and ort(sa[F]["px"], sa[F]["y"], sa[F]["pz"]) != "frei"), None)
    print("   erstes gemeinsames Bild mit Figur nicht frei: %s" % (("F%d (%s)" % (erst_ort, ort(sa[erst_ort]["px"], sa[erst_ort]["y"], sa[erst_ort]["pz"]))) if erst_ort is not None else "keins"))
    for F in range(F0 - 2, F0 + 2):
        if F in sa and F in sb:
            print("   F%d alt: PL%s %s | TRAG %s" % (F, sa[F]["pl"], sa[F]["g"][:92], ta.get(F, "-")[:110]))
            print("   F%d neu: PL%s %s | TRAG %s" % (F, sb[F]["pl"], sb[F]["g"][:92], tb.get(F, "-")[:110]))


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
