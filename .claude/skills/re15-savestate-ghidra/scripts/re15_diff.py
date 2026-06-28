#!/usr/bin/env python3
"""re15_diff.py - differential RAM-global finder across DuckStation RE1.5
savestates. Feed it 2+ labeled savestates and constraints; it prints the RAM
addresses whose values satisfy them. This is the "find an unknown global" half
of the workflow -- the hits are CANDIDATES; confirm/name them with re15_ghidra.py.

Usage:
  re15_diff.py LABEL=path [LABEL=path ...] [options]
    --eq A,B        require ram[A] == ram[B]   (repeatable)
    --neq A,B       require ram[A] != ram[B]   (repeatable)
    --alldiffer     require all labels pairwise distinct
    --width N       read width 1|2|4 (default 1)
    --min V --max V value bounds (dec or 0x..)  applied to every label
    --region LO HI  hex addr range (default 0x80090000 0x800c0000)
    --limit N       max hits printed (default 60)

Example (stage discriminator -- res shares stage2's overlay, differs from s1):
  re15_diff.py s1=stage1.sav s2=stage2.sav res=resume.sav --eq res,s2 --neq s1,s2 --max 8
"""
import sys, os, argparse
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re15_ss import Ram

def _int(s): return int(s, 16) if s.lower().startswith("0x") else int(s)

def main():
    labels, rest = {}, []
    for a in sys.argv[1:]:
        if "=" in a and not a.startswith("--"):
            k, v = a.split("=", 1); labels[k] = v
        else:
            rest.append(a)
    if len(labels) < 2:
        print(__doc__); sys.exit(1)
    ap = argparse.ArgumentParser()
    ap.add_argument("--eq", action="append", default=[])
    ap.add_argument("--neq", action="append", default=[])
    ap.add_argument("--alldiffer", action="store_true")
    ap.add_argument("--width", type=int, default=1)
    ap.add_argument("--min", type=_int, default=None)
    ap.add_argument("--max", type=_int, default=None)
    ap.add_argument("--region", nargs=2, default=["0x80090000", "0x800c0000"])
    ap.add_argument("--limit", type=int, default=60)
    args = ap.parse_args(rest)

    rams = {k: Ram(v) for k, v in labels.items()}
    rd = {1: "u8", 2: "u16", 4: "u32"}[args.width]
    eqs  = [tuple(p.split(",")) for p in args.eq]
    neqs = [tuple(p.split(",")) for p in args.neq]
    lo, hi = _int(args.region[0]), _int(args.region[1])

    print("labels: %s | width=%d | eq=%s neq=%s alldiffer=%s" %
          (",".join(labels), args.width, args.eq, args.neq, args.alldiffer))
    hits = 0
    for addr in range(lo, hi, args.width):
        vals = {k: getattr(r, rd)(addr) for k, r in rams.items()}
        if args.min is not None and any(v < args.min for v in vals.values()): continue
        if args.max is not None and any(v > args.max for v in vals.values()): continue
        if any(vals[a] != vals[b] for a, b in eqs):  continue
        if any(vals[a] == vals[b] for a, b in neqs): continue
        if args.alldiffer:
            vv = list(vals.values())
            if len(set(vv)) != len(vv): continue
        if not (eqs or neqs or args.alldiffer): continue   # need at least one constraint
        print("  0x%08x  " % addr + " ".join("%s=%s" % (k, hex(vals[k])) for k in labels))
        hits += 1
        if hits >= args.limit:
            print("  ... (--limit %d reached)" % args.limit); break
    print("%d hit(s)." % hits)

if __name__ == "__main__":
    main()
