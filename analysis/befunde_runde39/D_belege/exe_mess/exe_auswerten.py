"""exe_auswerten.py <laufordner> - state.log (RE15_STATE_LOG) + treffer.log (RE15_TREFFER_LOG) eines Messlaufs auswerten:
je Gegner HP-Verlauf (Treffer = HP faellt), Zustand/Bild dabei; Zielwahl-Zeilen; Sichtlinien-Sperren/Freigaben."""
import re, sys, os, collections
rx_en = re.compile(r"\[(\d+) t=([0-9a-f]{2}) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]{2}) mo=(-?\d+) af=(\d+) stun=(-?\d+) d=(\d+) @\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)")
rx_f = re.compile(r"^F(\d+) pad=([0-9a-f]{4}) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")

def main(d):
    st = os.path.join(d, "state.log"); tl = os.path.join(d, "treffer.log")
    last = {}; treffer = []; frames = 0; pl_last = None; rooms = set()
    for line in open(st, encoding="utf-8", errors="replace"):
        m = rx_f.match(line)
        if not m: continue
        frames += 1
        f = int(m.group(1)); pad = int(m.group(2), 16); pl_last = (f, pad, int(m.group(3)), int(m.group(4)), int(m.group(5)))
        for e in rx_en.finditer(line):
            slot = int(e.group(1)); hp = int(e.group(15))
            key = slot
            if key in last and hp < last[key][0]:
                treffer.append((f, slot, e.group(2), int(e.group(3)), int(e.group(4)), int(e.group(5)), e.group(7), int(e.group(8)), int(e.group(9)), last[key][0], hp, int(e.group(11)), pad))
            last[key] = (hp, f)
    print("%s: %d Bilder, letztes PL F%d pad=%04x (%d,%d) rot=%d" % (os.path.basename(d), frames, pl_last[0], pl_last[1], pl_last[2], pl_last[3], pl_last[4]) if pl_last else "keine Bilder")
    print("Treffer (HP faellt): %d" % len(treffer))
    for t in treffer[:60]:
        print("  F%d slot %d typ %s st=%d/%d/%d g=%s mo=%d af=%d hp %d->%d d=%d pad=%04x" % t)
    if os.path.exists(tl):
        c = collections.Counter(); ziele = []
        for line in open(tl, encoding="utf-8", errors="replace"):
            parts = line.split()
            if len(parts) < 3: continue
            art = parts[2]
            c[art] += 1
            if art == "ziel": ziele.append(line.strip())
        print("treffer.log:", dict(c))
        for z in ziele[:12]: print("  " + z)
        sperren = [l.strip() for l in open(tl, encoding="utf-8", errors="replace") if " sperre " in l or " frei " in l or "zielhilfe_verdeckt" in l]
        for s in sperren[:8]: print("  " + s)
        if len(sperren) > 8: print("  ... %d weitere" % (len(sperren) - 8))

if __name__ == "__main__":
    main(sys.argv[1])
