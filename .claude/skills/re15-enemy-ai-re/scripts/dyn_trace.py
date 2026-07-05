#!/usr/bin/env python3
"""dyn_trace.py - DYNAMIC DIFFERENTIAL TRACE driver (task DYN).

Like orig_timeline.py (re15-parity-verify) but samples CONCURRENTLY with the
tank-path drive: a sampler thread presses LB (SaveSelectedSaveState, slot 1)
every ~interval seconds from BEFORE path start until --total seconds after
path start, while the main thread drives the path. One run = one consistent
timeline of the same playthrough (film strip).
"""
import argparse, os, shutil, subprocess, time, threading
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SLOT1 = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_1.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--load", type=float, default=16.0)
    ap.add_argument("--path", default="")           # tank tokens U/D/L/R/X
    ap.add_argument("--total", type=float, default=25.0)  # sampling window from path start
    ap.add_argument("--interval", type=float, default=1.0)
    ap.add_argument("--mash", type=float, default=0.0)    # after the path: rapid CROSS taps for N s
    ap.add_argument("--outdir", required=True)
    a = ap.parse_args()
    os.makedirs(a.outdir, exist_ok=True)

    B = vg.XUSB_BUTTON
    PMAP = {"U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
            "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT,
            "X": B.XUSB_GAMEPAD_A}
    gp = vg.VX360Gamepad(); time.sleep(3.0)   # pad BEFORE DuckStation (SDL-0)

    lock = threading.Lock()
    def press(btn, secs):
        with lock:
            gp.press_button(button=btn); gp.update()
        time.sleep(secs)
        with lock:
            gp.release_button(button=btn); gp.update()

    def hold(btn, secs):
        press(btn, secs); time.sleep(0.15)

    print("[dyn] launch -statefile %s" % a.state, flush=True)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    time.sleep(a.load)

    state = {"t0": None}
    stop = threading.Event()
    def take_sample(idx, tag):
        m0 = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        press(B.XUSB_GAMEPAD_LEFT_SHOULDER, 0.12)
        t_rel = (time.monotonic() - state["t0"]) if state["t0"] is not None else -1.0
        ok = False
        for _ in range(25):
            time.sleep(0.1)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > m0:
                ok = True; break
        time.sleep(0.3)
        dst = os.path.join(a.outdir, "s%02d_t%06.2f.sav" % (idx, t_rel))
        try:
            shutil.copy2(SLOT1, dst)
            print("[dyn] sample %02d (%s) @ %+06.2fs write=%s" % (idx, tag, t_rel, ok), flush=True)
        except Exception as e:
            print("[dyn] sample %02d FAILED: %s" % (idx, e), flush=True)

    def sampler():
        i = 1
        while not stop.is_set():
            tgt = state["t0"] + i * a.interval
            now = time.monotonic()
            if now < tgt:
                time.sleep(min(tgt - now, 0.05)); continue
            take_sample(i, "auto")
            i += 1
            if (time.monotonic() - state["t0"]) >= a.total:
                break
        print("[dyn] sampler done (%d samples)" % (i - 1), flush=True)

    # baseline sample before any input
    take_sample(0, "baseline")

    state["t0"] = time.monotonic()
    th = threading.Thread(target=sampler, daemon=True); th.start()

    for tok in (a.path.split(",") if a.path else []):
        tok = tok.strip()
        if not tok: continue
        btn = PMAP.get(tok[0].upper()); secs = float(tok[1:] or "0.5")
        if btn is None: continue
        print("[dyn] PATH %s %.2fs (t=%.2f)" % (tok[0].upper(), secs,
              time.monotonic() - state["t0"]), flush=True)
        hold(btn, secs)
    print("[dyn] path done at t=%.2f, standing" % (time.monotonic() - state["t0"]), flush=True)

    if a.mash > 0:
        t_end = time.monotonic() + a.mash
        print("[dyn] MASH cross for %.1fs" % a.mash, flush=True)
        while time.monotonic() < t_end:
            press(B.XUSB_GAMEPAD_A, 0.06); time.sleep(0.06)
        print("[dyn] mash done at t=%.2f" % (time.monotonic() - state["t0"]), flush=True)

    th.join(timeout=a.total + 30)
    stop.set()
    print("[dyn] graceful close", flush=True)
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    time.sleep(3.0)
    print("[dyn] DONE", flush=True)

if __name__ == "__main__":
    main()
