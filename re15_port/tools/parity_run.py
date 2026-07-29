#!/usr/bin/env python
"""parity_run.py — PAIRED INPUT REPLAY: original vs port, same input, field-by-field verdict.

The idea (user, 2026-07-29): drive the ORIGINAL in DuckStation with an exact input string, drive the
PORT with the SAME string, and compare the resulting GAME STATE. Identical => that area is done;
then walk through the door and repeat for the next room. Room by room, until everything matches.

This works because both sides already speak the same input language: re15-room-capture's
`re15_quickload.py --path "R0.5,U2,R0.3,U6"` and the port's `RE15_INPUT_SCRIPT` take the IDENTICAL
string (input_pc.c:41-51 — "the port MUST produce the same result as the PSX original; any
divergence is a port bug").

Compared quantities are RAM/state values, never pixels:
    player : x, z, rot, hp
    enemies: type, +0x4 state, +0x5 sub-state, +0x9 grid byte, distance to player

USAGE
    # compare against an ALREADY captured original savestate (fast, reproducible)
    parity_run.py --room 1140 --path "R0.5,U2,R0.3,U6" \
                  --psx stage_saves/mzd_stage1_engage_live.sav \
                  --psx-start stage_saves/mzd_stage1_briefing.sav

    # (re-)capture the original first — needs DuckStation + vgamepad, see the re15-room-capture skill
    #   python .claude/skills/re15-room-capture/scripts/re15_quickload.py \
    #          --left 16 --postload 10 --path "R0.5,U2,R0.3,U6" --out <sav>

WHY THE START STATE MATTERS
The port is driven with RE15_START_ROOM, the original with the debug JUMP. If those two do not
place the player identically the comparison measures the SETUP, not the behaviour. --psx-start
takes the clean pre-movement capture of the same room and the tool reports the start delta
separately, so a setup mismatch can never be mistaken for a behaviour bug.
"""
import argparse, os, re, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))

PLAYER_BASE = 0x800ACA54           # player entity struct base
PLAYER_YAW  = 0x800ACABE           # PLAYER_BASE + 0x6a = rot_y (4096 = 360), compares 1:1 to
                                   # the port's actor.rot_y (re15_actor.h:57)
PLAYER_POS  = 0x800ACA88           # = MATRIX.t[0..2] of the entity MATRIX at +0x20 (NOT a
                                   # separate "position cache"): +0x20 is a PsyQ MATRIX, so
                                   # 0x800ACA74 is m[0][0] = COS(yaw) and must never be read as
                                   # an angle. Reading it as one produced a phantom "spawn-yaw
                                   # divergence" (4051 vs -96) that does not exist.
ENEMY_BASE, ENEMY_STRIDE = 0x800ACC2C, 0x1F4


def s32(v):
    return v - (1 << 32) if v >= (1 << 31) else v


def read_psx(path):
    import re15_ss
    r = re15_ss.Ram(path)
    yaw = r.u16(PLAYER_YAW); yaw = yaw - (1 << 16) if yaw >= (1 << 15) else yaw
    out = {"player": {"x": s32(r.u32(PLAYER_POS)), "z": s32(r.u32(PLAYER_POS + 8)),
                      "rot": yaw},
           "enemies": []}
    for i in range(8):
        b = ENEMY_BASE + i * ENEMY_STRIDE
        flags = r.u8(b + 0x00)
        if not (flags & 1):
            continue
        out["enemies"].append({
            "slot": i,
            "type": r.u8(b + 0x08),      # +0x8 = the type byte (same offset re15_enemy_state.py reads)
            "st":   r.u8(b + 0x04),
            "ss1":  r.u8(b + 0x05),
            "grid": r.u8(b + 0x09),
            "dist": r.u32(b + 0x1D0),
        })
    return out


def run_port(room, path, lead_in, exe, log):
    env = dict(os.environ)
    env.update({"RE15_NOAUDIO": "1", "RE15_SOFTWARE_RENDER": "1",
                "RE15_START_ROOM": str(room), "RE15_INPUT_SCRIPT": path,
                "RE15_INPUT_SCRIPT_START": str(lead_in), "RE15_STATE_LOG": log})
    # headless does not self-terminate; the state log is written incrementally, so run it just past
    # the end of the input timeline and then kill it.
    secs = max(12, int(path_frames(path) / 30.0) + int(lead_in / 30.0) + 6)
    try:
        subprocess.run([exe, "--headless"], env=env, cwd=os.path.dirname(exe),
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=secs)
    except subprocess.TimeoutExpired:
        pass
    return os.path.join(os.path.dirname(exe), log)


def path_frames(path, fps=30):
    n = 0.0
    for tok in path.split(","):
        m = re.match(r"^[A-Za-z]+([0-9.]+)$", tok.strip())
        if m:
            n += float(m.group(1))
    return int(round(n * fps))


def parse_port(log, frame):
    want = None
    for ln in open(log):
        m = re.match(r"F(\d+) ", ln)
        if not m:
            continue
        if int(m.group(1)) == frame:
            want = ln
            break
        if int(m.group(1)) < frame:
            want = ln                      # keep the last one before, in case the run ended early
    if want is None:
        return None
    p = re.search(r"PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\)", want)
    out = {"player": {"x": int(p.group(1)), "z": int(p.group(2)),
                      "rot": int(p.group(3)), "hp": int(p.group(4))}, "enemies": []}
    for e in re.finditer(r"\[(\d+) t=([0-9a-f]+) st=(\d+) ss1=(\d+) ss2=\d+ ss3=\d+ "
                         r"g=([0-9a-f]+) mo=\d+ af=\d+ stun=-?\d+ d=(\d+)", want):
        out["enemies"].append({"slot": int(e.group(1)) - 1,      # port slot 0 = the player
                               "type": int(e.group(2), 16), "st": int(e.group(3)),
                               "ss1": int(e.group(4)), "grid": int(e.group(5), 16),
                               "dist": int(e.group(6))})
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--room", required=True)
    ap.add_argument("--path", required=True)
    ap.add_argument("--psx", required=True, help="original savestate AFTER the path")
    ap.add_argument("--psx-start", help="original savestate BEFORE the path (clean room load)")
    ap.add_argument("--lead-in", type=int, default=90)
    ap.add_argument("--exe", default=os.path.join(REPO, "re15_port", "build", "platform", "pc", "re15_pc.exe"))
    ap.add_argument("--tol", type=int, default=64, help="position tolerance in world units")
    a = ap.parse_args()

    psx = read_psx(a.psx)
    log = run_port(a.room, a.path, a.lead_in, a.exe, "parity_port.txt")
    end = a.lead_in + path_frames(a.path)
    port = parse_port(log, end)
    if port is None:
        print("FAIL: the port produced no state at frame %d" % end)
        return 2

    print("ROOM %s   PATH %r   (Port-Frame %d)" % (a.room, a.path, end))
    fails = 0

    if a.psx_start:
        st = read_psx(a.psx_start)
        p0 = parse_port(log, 0)
        dx = st["player"]["x"] - p0["player"]["x"]
        dz = st["player"]["z"] - p0["player"]["z"]
        tag = "OK  " if abs(dx) <= a.tol and abs(dz) <= a.tol else "FAIL"
        if tag == "FAIL":
            fails += 1
        print("%s  START gleich: PSX (%d,%d) vs Port (%d,%d)   Delta (%d,%d)"
              % (tag, st["player"]["x"], st["player"]["z"], p0["player"]["x"], p0["player"]["z"], dx, dz))
        print("      PSX-Weg  Delta (%d,%d)   Port-Weg  Delta (%d,%d)"
              % (psx["player"]["x"] - st["player"]["x"], psx["player"]["z"] - st["player"]["z"],
                 port["player"]["x"] - p0["player"]["x"], port["player"]["z"] - p0["player"]["z"]))

    dx = psx["player"]["x"] - port["player"]["x"]
    dz = psx["player"]["z"] - port["player"]["z"]
    tag = "OK  " if abs(dx) <= a.tol and abs(dz) <= a.tol else "FAIL"
    if tag == "FAIL":
        fails += 1
    print("%s  SPIELER-ENDE: PSX (%d,%d) vs Port (%d,%d)   Delta (%d,%d)"
          % (tag, psx["player"]["x"], psx["player"]["z"], port["player"]["x"], port["player"]["z"], dx, dz))

    print("      slot  typ        +0x4     +0x5      grid      dist")
    pe = {e["slot"]: e for e in port["enemies"]}
    for e in psx["enemies"]:
        q = pe.get(e["slot"])
        if q is None:
            print("FAIL  %-4d  fehlt im Port" % e["slot"]); fails += 1; continue
        row, bad = [], False
        for k, fmt in (("type", "0x%02x"), ("st", "%d"), ("ss1", "0x%x"), ("grid", "0x%02x")):
            same = e[k] == q[k]
            bad |= not same
            row.append((fmt % e[k]) + ("==" if same else "!=") + (fmt % q[k]))
        dd = abs(e["dist"] - q["dist"])
        bad |= dd > 256
        row.append("%d/%d" % (e["dist"], q["dist"]))
        if bad:
            fails += 1
        print("%s  %-4d  %s" % ("OK  " if not bad else "FAIL", e["slot"], "  ".join(row)))

    print("VERDICT:", "PASS" if fails == 0 else "FAIL (%d)" % fails)
    return 0 if fails == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
