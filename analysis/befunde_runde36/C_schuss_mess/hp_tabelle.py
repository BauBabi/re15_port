"""HP-Verlauf je Platz aus den state.log-Dateien der run_p1_alle.sh-Laeufe: je Waffe und Lauf
(tisch/fresser) der HP-Hoechstwert nach dem Spawn, der Endwert und das Bild des ersten HP-Verlusts.
Aufruf: python -I hp_tabelle.py <mess_r36c/alle>"""
import os, re, sys

FRAME = re.compile(r"^F(\d+)\s")
SLOT = re.compile(r"\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+)[^\]]*\] hp=(-?\d+)")
NAMEN = {3: "Browning HP", 4: "SIG P228", 5: "Beretta M93R", 6: "Glock 18", 7: "Super Redhawk",
         8: "Remington M870", 12: "Ingram M10", 13: "SPAS-12", 14: "Flammenwerfer", 15: "GL Explosiv",
         16: "GL Saeure", 17: "GL Brand", 18: "Raketenwerfer", 19: "H&K MC51", 20: "Colt Python"}

def lauf(path):
    hp = {}      # slot -> [max, last, verlust_frame, st, ss1]
    try:
        for ln in open(path, encoding="utf-8", errors="replace"):
            fm = FRAME.match(ln)
            if not fm: continue
            f = int(fm[1])
            for m in SLOT.finditer(ln):
                slot, typ, st, ss1, h = int(m[1]), m[2], m[3], m[4], int(m[5])
                if typ not in ("10", "11", "12", "16", "18"): continue
                e = hp.setdefault(slot, [h, h, None, st, ss1])
                if h > e[0]: e[0] = h
                if h < e[1] and e[2] is None: e[2] = f
                e[1] = h; e[3] = st; e[4] = ss1
    except FileNotFoundError:
        return None
    return hp

def main():
    base = sys.argv[1]
    print("| Waffe | Lauf | Platz 1 | Platz 2 | Platz 3 | Platz 4 | Platz 5 | Urteil |")
    print("|---|---|---|---|---|---|---|---|")
    for w in sorted(int(d[1:]) for d in os.listdir(base) if d.startswith("w")):
        for name in ("tisch", "fresser"):
            hp = lauf(os.path.join(base, f"w{w}", name, "state.log"))
            if hp is None: print(f"| {w} {NAMEN.get(w,'')} | {name} | (kein state.log) |"); continue
            zellen = []; verlust = 0
            for slot in sorted(hp):
                mx, last, vf, st, ss1 = hp[slot]
                z = f"{mx}->{last}"
                if vf is not None: z += f" (F{vf})"; verlust += 1
                zellen.append(z)
            urteil = "kein HP-Verlust" if verlust == 0 else f"{verlust} Platz/Plaetze verlieren HP"
            print(f"| {w} {NAMEN.get(w,'')} | {name} | " + " | ".join(zellen) + f" | {urteil} |")

main()
