#!/usr/bin/env python
"""check_anim_trace.py — pass/fail verdict on a POSE-STREAM trace.

Why this exists: animation defects are invisible in the state log. Two of them cost a full round
each in this project — the aim-release "land" (a ONE-frame wrong pose) and, most recently, a frame
clock that wrapped against the wrong bank's clip length, which read as "the zombies loop oddly".
Neither shows up in state/motion logs; both are obvious in the resolved POSE stream.

Produce a trace with:
    RE15_ANIM_TRACE=<path> ./re15_pc.exe --headless ...
Each line is one resolved pose, written from re15_compute_actor_kf (the only place a pose is
resolved in the whole port):

    frame actor type motion clip fc_render cur slot fc_clock loco reverse

  fc_render = the frame count of the clip the RENDERER is indexing
  slot      = the keyframe slot actually posed  (this is what the eye sees)
  fc_clock  = the frame count re15_actor_clip_len() reports, i.e. what the AI's clip clock uses

CHECK 1 — BANK CONSISTENCY (the bug this run is about)
    fc_render must equal fc_clock on every line. If they differ, the state machine is timing its
    phases against a different clip than the one on screen.

CHECK 2 — CLEAN RAMPS (the regression guard)
    While an actor stays on one clip, `slot` must walk a contiguous ramp and may only wrap from
    fc-1 back to 0 (looping clips) or stop at fc-1 (hold-last clips). A jump to an arbitrary slot
    mid-cycle IS the "loops oddly" symptom and fails here automatically.

Exit code 0 = both checks pass.
"""
import sys
from collections import defaultdict


def load(path):
    rows = []
    for ln in open(path):
        p = ln.split()
        if len(p) != 11:
            continue
        try:
            rows.append(tuple(int(x) for x in p))
        except ValueError:
            continue
    return rows


def check_bank_consistency(rows):
    """Only ENEMIES have a clip clock (re15_actor_clip_len goes through the enemy bank registry),
    so the player (type 0) is not part of this invariant and is excluded — an unfiltered check
    reported 690 'violations' that were just the player having no enemy bank."""
    bad = defaultdict(int)
    for fr, act, typ, mo, clip, fcr, cur, slot, fcc, loco, rev in rows:
        if typ == 0:
            continue
        if fcr != fcc:
            bad[(act, typ, mo, clip, fcr, fcc, loco)] += 1
    return bad


def check_ramps(rows, tol_actors=None):
    """A ramp break = slot changed by something other than +1 (or a wrap fc-1 -> 0) while the
    actor stayed on the same clip and the clip did not restart from 0."""
    last = {}
    breaks = defaultdict(list)
    for fr, act, typ, mo, clip, fcr, cur, slot, fcc, loco, rev in rows:
        key = act
        prev = last.get(key)
        last[key] = (fr, slot, fcr, mo, clip, loco, rev)
        if prev is None:
            continue
        pfr, pslot, pfc, pmo, pclip, ploco, prev_rev = prev
        # Only judge CONTINUOUS playback of the SAME clip out of the SAME bank in the SAME
        # direction. A clip change, a bank switch or a direction flip legitimately jumps.
        if fr != pfr + 1 or clip != pclip or loco != ploco or rev != prev_rev or fcr != pfc:
            continue
        d = slot - pslot
        if rev:                                   # reverse playback walks DOWN
            ok = (d == 0) or (d == -1) or (pslot == 0 and slot == fcr - 1)
        else:
            ok = (d == 0) or (d == 1) or (pslot == fcr - 1 and slot == 0)
        if not ok:
            breaks[(act, clip, fcr, rev)].append((fr, pslot, slot))
    return breaks


def main():
    if len(sys.argv) < 2:
        print("usage: check_anim_trace.py <trace> [--quiet]")
        return 2
    rows = load(sys.argv[1])
    if not rows:
        print("FAIL: trace is empty — was RE15_ANIM_TRACE set and did anything render?")
        return 2
    actors = sorted({r[1] for r in rows})
    print("Posen: %d   Actors: %s   Frames: %d..%d"
          % (len(rows), actors, rows[0][0], rows[-1][0]))

    fails = 0
    bad = check_bank_consistency(rows)
    if bad:
        fails += 1
        total = sum(bad.values())
        print("FAIL  Bank-Konsistenz: %d Posen, bei denen die Uhr gegen eine ANDERE Clip-Laenge "
              "vergleicht als der Renderer indiziert" % total)
        for (act, typ, mo, clip, fcr, fcc, loco), n in sorted(bad.items(), key=lambda kv: -kv[1])[:8]:
            print("      actor %d typ 0x%02x motion %-3d clip %-3d  %s  Renderer fc=%-3d  Uhr fc=%-3d  (%dx)"
                  % (act, typ, mo, clip, "LOCO" if loco else "ACT ", fcr, fcc, n))
    else:
        print("OK    Bank-Konsistenz: fc_render == fc_clock auf allen %d Posen" % len(rows))

    breaks = check_ramps(rows)
    if breaks:
        fails += 1
        total = sum(len(v) for v in breaks.values())
        print("FAIL  Clip-Rampen: %d Spruenge mitten im Zyklus (= 'loopt komisch')" % total)
        for (act, clip, fc, rev), v in sorted(breaks.items(), key=lambda kv: -len(kv[1]))[:8]:
            ex = ", ".join("f%d %d->%d" % t for t in v[:4])
            print("      actor %d clip %-3d fc=%-3d %s  %dx   z.B. %s"
                  % (act, clip, fc, "REV" if rev else "FWD", len(v), ex))
    else:
        print("OK    Clip-Rampen: keine Spruenge mitten im Zyklus")

    print("VERDICT:", "PASS" if fails == 0 else "FAIL")
    return 0 if fails == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
