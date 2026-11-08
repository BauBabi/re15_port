"""oben3.py <laufordner>: Mass "Figur oben auf der Rampe" eines exe-Laufs (Messblatt NB3, Vorgabe A).
Eingaben: state.log (F, PL x/z, hp, cam, Gator @(x,z,r) mo), climb.log (Figur y je Bild, Zeile fuer Zeile wie state.log), gator_trag.log (TRAG F ph= ... je Bild),
gator_boss.log (RAMMEN F..., CROSS start, UEBERQUERUNG). Oben = Figur y < -900 und im Grundriss von Rampe (1850..7200, -18100..-14500) oder Plattform.
Ausgabe: Bilder oben (erstes/letztes), Treffer (hp < 100, Flanke) mit Phase (7 = Querung/Rammen, 5 = Belagerung/Hochbiss, 3 = Biss) und Gator-Lage, Zeit vom
ersten Oben-Bild bis zum ersten Treffer oben, laengste Pause ohne Treffer waehrend oben, Phasenanteile oben, Kamera, Figur-Lage je Treffer."""
import re, sys, math
run = sys.argv[1]
sl = [l for l in open(run + "/state.log", errors="replace") if l.startswith("F")]
cl = [l for l in open(run + "/climb.log", errors="replace") if l.startswith("T")]
start = 0
for i, l in enumerate(sl):
    m = re.match(r"F(\d+) ", l)
    if m and int(m.group(1)) == 0: start = i
st = {}
for i in range(start, len(sl)):
    l = sl[i]
    m = re.match(r"F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(\d+),hp=(-?\d+)\).*mo=(\d+).*cam=(\d+)", l)
    if not m: continue
    F = int(m.group(1))
    g = re.search(r"\[35 t=23 .*mo=(\d+) af=(\d+) stun=(\d+) .*@\((-?\d+),(-?\d+),r(\d+)\)", l)
    y = None
    if i < len(cl):
        c = re.search(r"pos=\((-?\d+),(-?\d+),(-?\d+)\)", cl[i]); y = int(c.group(2)) if c else None
    st[F] = dict(px=int(m.group(3)), pz=int(m.group(4)), hp=int(m.group(6)), pmo=int(m.group(7)), cam=int(m.group(8)), y=y,
                 gmo=int(g.group(1)) if g else -1, gaf=int(g.group(2)) if g else -1, stun=int(g.group(3)) if g else -1,
                 gx=int(g.group(4)) if g else 0, gz=int(g.group(5)) if g else 0, gr=int(g.group(6)) if g else 0)
ph = {}
for l in open(run + "/gator_trag.log", errors="replace"):
    m = re.match(r"TRAG F(\d+) ph=(\d+)", l)
    if m: ph[int(m.group(1))] = int(m.group(2))
ramm = set(int(m.group(1)) for m in re.finditer(r"RAMMEN F(\d+)", open(run + "/gator_boss.log", errors="replace").read()))
def oben(s): return s["y"] is not None and s["y"] < -900 and ((1850 < s["px"] < 7200 and -18100 < s["pz"] < -14500) or (-1700 < s["px"] < 1850 and -20000 < s["pz"] < -12450))
Fs = sorted(st)
ob = [F for F in Fs if oben(st[F])]
treffer = [F for F in Fs if st[F]["hp"] < 100 and (F - 1 not in st or st[F - 1]["hp"] >= 100)]
name = run.replace("\\", "/").split("/")[-1]
print("%s: %d Bilder (F%d..F%d); Figur oben %d Bilder (erstes F%s, letztes F%s); Treffer gesamt %d" % (name, len(Fs), Fs[0], Fs[-1], len(ob), ob[0] if ob else "-", ob[-1] if ob else "-", len(treffer)))
def art(F):
    p = ph.get(F, -1)
    if F in ramm: return "Rammen"
    if p == 7: return "CROSS-Biss"
    if p == 5: return "Hochbiss"
    if p == 3: return "Biss"
    return "ph%d" % p
for F in treffer:
    s = st[F]
    th = s["gr"] * 2 * math.pi / 4096
    kx, kz = s["gx"] + math.cos(th) * 3236, s["gz"] - math.sin(th) * 3236
    print("    Treffer F%-5d %-10s Figur (%d,%s,%d) %s | Gator Wurzel (%d,%d) Gier %d Clip %d Bild %d, Kopf (%d,%d) | cam %d" % (F, art(F), s["px"], s["y"], s["pz"], "OBEN" if oben(s) else "unten",
          s["gx"], s["gz"], s["gr"], s["gmo"], s["gaf"], kx, kz, s["cam"]))
if ob:
    # zusammenhaengende Oben-Abschnitte
    abschn = []; a = ob[0]; prev = ob[0]
    for F in ob[1:]:
        if F != prev + 1: abschn.append((a, prev)); a = F
        prev = F
    abschn.append((a, prev))
    for (a, b) in abschn:
        tr = [F for F in treffer if a <= F <= b]
        pausen = []; letzt = a
        for F in tr: pausen.append(F - letzt); letzt = F
        pausen.append(b - letzt)
        phs = {}
        for F in range(a, b + 1): p = ph.get(F, -1); phs[p] = phs.get(p, 0) + 1
        cams = {}
        for F in range(a, b + 1):
            if F in st: cams[st[F]["cam"]] = cams.get(st[F]["cam"], 0) + 1
        print("  oben F%d..F%d (%d Bilder = %.1f s): Treffer %d (erster nach %s Bildern = %s s), laengste Pause ohne Treffer %d Bilder = %.1f s; Phasen %s; cam %s" %
              (a, b, b - a + 1, (b - a + 1) / 30.0, len(tr), (tr[0] - a) if tr else "-", ("%.1f" % ((tr[0] - a) / 30.0)) if tr else "-", max(pausen), max(pausen) / 30.0,
               ", ".join("%d:%d" % kv for kv in sorted(phs.items())), ", ".join("%d:%d" % kv for kv in sorted(cams.items()))))
