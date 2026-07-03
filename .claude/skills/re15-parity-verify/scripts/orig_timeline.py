#!/usr/bin/env python3
"""orig_timeline.py — DETERMINISTIC original-behavior TIMELINE capture (re15-parity-verify §4).

Direct-loads a gameplay savestate (NO debug-menu JUMP nav -> deterministic), walks a multi-token
tank-path, then SAMPLES savestates at fixed intervals during the settle via the LB save-hotkey
([Hotkeys] SaveSelectedSaveState = SDL-0/LeftShoulder -> slot 1 file). One run = one consistent
timeline of the SAME playthrough: decode each sample with re15_enemy_state.py / ppos.py and diff
against the port's RE15_STATE_LOG at the matching tick offsets.

  python orig_timeline.py --state stage_saves/mzd_stage1_briefing.sav \
      --path "R0.3,U2.5" --samples 12 --interval 1.5 --outdir /tmp/tl
"""
import argparse, os, shutil, subprocess, time
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SLOT1 = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_1.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--load", type=float, default=16.0)
    ap.add_argument("--path", default="")            # tank tokens: U/D fwd/back, L/R turn, X cross
    ap.add_argument("--samples", type=int, default=12)
    ap.add_argument("--interval", type=float, default=1.5)
    ap.add_argument("--mash", type=float, default=0.0)   # after the path: rapidly tap CROSS for N s
                                                          # (the grab mash-escape; ~8 taps/s)
    ap.add_argument("--outdir", required=True)
    a = ap.parse_args()
    os.makedirs(a.outdir, exist_ok=True)

    B = vg.XUSB_BUTTON
    PMAP = {"U": B.XUSB_GAMEPAD_DPAD_UP, "D": B.XUSB_GAMEPAD_DPAD_DOWN,
            "L": B.XUSB_GAMEPAD_DPAD_LEFT, "R": B.XUSB_GAMEPAD_DPAD_RIGHT,
            "X": B.XUSB_GAMEPAD_A}
    gp = vg.VX360Gamepad(); time.sleep(3.0)          # pad BEFORE DuckStation (SDL-0 enumeration)

    def hold(btn, secs):
        gp.press_button(button=btn); gp.update(); time.sleep(secs)
        gp.release_button(button=btn); gp.update(); time.sleep(0.15)

    print("[tl] launch -statefile %s" % a.state, flush=True)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    time.sleep(a.load)

    t_path0 = time.monotonic()
    for tok in (a.path.split(",") if a.path else []):
        tok = tok.strip()
        if not tok: continue
        btn = PMAP.get(tok[0].upper()); secs = float(tok[1:] or "0.5")
        if btn is None: continue
        print("[tl] PATH %s %.2fs" % (tok[0].upper(), secs), flush=True)
        hold(btn, secs)
    t_path_end = time.monotonic()
    print("[tl] path done (%.2fs), sampling %d x %.1fs" % (t_path_end - t_path0, a.samples, a.interval), flush=True)

    def mash_for(secs):
        # rapid CROSS taps (~8/s): any D-pad/face press EDGE (FUN_80037024 mask 0xf0f0) drains the
        # grab's escape window by 5 extra per tap frame -> break free alive.
        t_end = time.monotonic() + secs
        while time.monotonic() < t_end:
            gp.press_button(button=B.XUSB_GAMEPAD_A); gp.update(); time.sleep(0.06)
            gp.release_button(button=B.XUSB_GAMEPAD_A); gp.update(); time.sleep(0.06)

    # SAMPLE LOOP: LB (save slot 1) -> wait for the write -> copy out. Timestamps are relative to
    # path END (the port aligns on its own path-end tick). One run = one consistent timeline.
    # With --mash the inter-sample gap is filled with CROSS taps (mash-escape probe, samples DURING).
    for i in range(a.samples):
        if a.mash > 0 and (time.monotonic() - t_path_end) < a.mash:
            mash_for(a.interval)
        else:
            time.sleep(a.interval)
        m0 = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        gp.press_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update(); time.sleep(0.12)
        gp.release_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update()
        t_rel = time.monotonic() - t_path_end
        for _ in range(20):                              # wait for the async save write
            time.sleep(0.1)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > m0: break
        time.sleep(0.25)
        dst = os.path.join(a.outdir, "tl_%02d_t%04.1f.sav" % (i, t_rel))
        try:
            shutil.copy2(SLOT1, dst)
            print("[tl] sample %d @ +%.1fs -> %s" % (i, t_rel, os.path.basename(dst)), flush=True)
        except Exception as e:
            print("[tl] sample %d FAILED: %s" % (i, e), flush=True)

    print("[tl] graceful close", flush=True)
    subprocess.run(["taskkill", "/IM", IMG], capture_output=True)
    time.sleep(3.0)
    print("[tl] DONE (%d samples in %s)" % (a.samples, a.outdir), flush=True)

if __name__ == "__main__":
    main()
