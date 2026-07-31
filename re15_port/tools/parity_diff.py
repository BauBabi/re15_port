#!/usr/bin/env python
"""parity_diff.py — vergleicht den PCSX-Trace (Original) mit dem Port-State-Log, Frame fuer Frame.

Beide Seiten schreiben dasselbe Zeilenformat; dieses Werkzeug legt sie auf eine GEMEINSAME ZEITBASIS
und meldet die erste echte Abweichung je Groesse.

ZWEI FALLEN, die beim ersten Vergleich sofort zugeschlagen haben und hier fest verdrahtet sind:

1. YAW MASKIEREN. Das Original schreibt rot_y ungemaskt (gemessen: -5952), der Port maskiert auf
   12 Bit. Ohne Maskierung ist jeder Vergleich sinnlos. ACHTUNG: -5952 & 0xFFF = 2240, NICHT 4000 —
   ich hatte zunaechst behauptet, es sei derselbe Winkel wie die -96 des Ports. Ist es nicht. Nach
   der Maskierung bleibt eine ECHTE Yaw-Differenz (PSX 2240 vs Port 4000) stehen, und sie hat einen
   plausiblen Grund: dieser Raum wurde per Debug-JUMP aus einem anderen Raum betreten, der Port
   startet ihn per RE15_START_ROOM. Die Eintritts-Orientierung ist also nicht dieselbe Situation.

2. ZEITBASIS AB RAUMEINTRITT. Der PCSX-Trace zaehlt ab "# ... ROOM LIVE", der Port-Log ab
   Programmstart. Frame N der einen Seite ist nicht Frame N der anderen. Verglichen wird deshalb
   Offset zu Offset relativ zum jeweiligen Raumeintritt.

Aufruf:
    parity_diff.py <pcsx_trace.log> <port_state.log> [--port-live N] [--frames N]
      --port-live  Frame, ab dem der Port als "im Raum" gilt (Default 0: RE15_START_ROOM ist sofort da)
"""
import argparse, re, sys

RE_PL = re.compile(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)")
RE_EM = re.compile(r"\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+)")


def yaw12(v):
    """Beide Seiten auf 12 Bit bringen; das Original schreibt ungemaskt."""
    return int(v) & 0xFFF


def parse(path, live_from=None, slot_base=0):
    """-> {offset: (px, pz, yaw, hp, [(slot,typ,st,ss1,ss2,ss3,grid), ...])}"""
    out, live = {}, live_from
    for ln in open(path, encoding="utf-8", errors="replace"):
        if live is None and ln.startswith("#") and "ROOM LIVE" in ln:
            m = re.search(r"f(\d+)", ln)
            if m:
                live = int(m.group(1))
            continue
        m = re.match(r"F(\d+) ", ln)
        if not m or live is None:
            continue
        off = int(m.group(1)) - live
        if off < 0:
            continue
        p = RE_PL.search(ln)
        if not p:
            continue
        # SLOT-VERSATZ: der Port fuehrt den SPIELER als Slot 0, das Original nicht — Port-Slot N
        # entspricht PSX-Slot N-1. Ohne das meldet der Vergleich bei JEDEM Gegner eine Abweichung,
        # die reine Nummerierung ist.
        ems = [(int(a) - slot_base, b, int(c), int(d), int(e), int(f))
               for a, b, c, d, e, f, _g in RE_EM.findall(ln)]
        out[off] = (int(p.group(1)), int(p.group(2)), yaw12(p.group(3)), int(p.group(4)), ems)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("pcsx")
    ap.add_argument("port")
    ap.add_argument("--port-live", type=int, default=0)
    ap.add_argument("--frames", type=int, default=400)
    ap.add_argument("--tol", type=int, default=8, help="Positions-Toleranz in Welteinheiten")
    a = ap.parse_args()

    A = parse(a.pcsx)
    B = parse(a.port, live_from=a.port_live, slot_base=1)
    if not A:
        print("FAIL: im PCSX-Trace kein 'ROOM LIVE' gefunden"); return 2
    if not B:
        print("FAIL: im Port-Log keine Frames gefunden"); return 2

    common = sorted(set(A) & set(B))
    common = [o for o in common if o <= a.frames]
    print("Gemeinsame Frames ab Raumeintritt: %d (0..%d)" % (len(common), common[-1] if common else -1))

    first = {}
    for off in common:
        pa, pb = A[off], B[off]
        for name, va, vb, tol in (("x", pa[0], pb[0], a.tol), ("z", pa[1], pb[1], a.tol),
                                  ("yaw", pa[2], pb[2], 0), ("hp", pa[3], pb[3], 0)):
            if abs(va - vb) > tol and name not in first:
                first[name] = (off, va, vb)
        # Gegner: Anzahl und je Slot Typ/State/Substates
        if len(pa[4]) != len(pb[4]) and "roster" not in first:
            first["roster"] = (off, len(pa[4]), len(pb[4]))
        else:
            for ea, eb in zip(pa[4], pb[4]):
                if ea != eb and "enemy" not in first:
                    first["enemy"] = (off, ea, eb)

    if not first:
        print("PASS — kein Unterschied in %d Frames" % len(common))
        return 0
    print("Erste Abweichung je Groesse (Offset ab Raumeintritt):")
    for k in ("roster", "enemy", "x", "z", "yaw", "hp"):
        if k in first:
            o, va, vb = first[k]
            print("  %-7s @+%-4d  PSX=%s  Port=%s" % (k, o, va, vb))
    return 1


if __name__ == "__main__":
    sys.exit(main())
