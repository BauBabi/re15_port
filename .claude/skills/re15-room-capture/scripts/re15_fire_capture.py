#!/usr/bin/env python3
"""re15_fire_capture.py - load a gun-equipped ROOM1140 state, walk to firing range,
aim+fire at a zombie, and capture the transient hit effect (effect-0 blood) LIVE by
interleaving fire with the controller-bound save-state hotkey.

KEY MECHANISM (works, verified 2026-07-01): DuckStation hotkey SaveSelectedSaveState
is bound in settings.ini to SDL-0/LeftShoulder (a SINGLE binding -- '&' is a CHORD,
not OR, so a multi-binding silently never fires). Pressing LB on the vgamepad writes
HASH-<serial>_1.sav LIVE mid-game (no window focus / keyboard needed). We fire Square,
then immediately LB-save and copy slot1 out -> the ~0.5s blood effect lands in the snap.
Repeat to span the effect window. re15_ss.py then scans each snap for an active
effect-0 in the pool (DAT_800a73b8, stride 0x108, +0x6c state / +0x70 effid==0).

Aim R1 is also SDL-bound (settings.ini R1 = ... & SDL-0/RightShoulder); base state must
be gun-equipped (weapon DAT_800aca5d != 1). Build it with the equip sequence from a SAFE
spawn (briefing_live), since engage_live dies on resume (player adjacent to zombies).
"""
import time, sys, subprocess, os, shutil, argparse
import vgamepad as vg
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SLOT1 = r"C:\Users\mjoedicke\AppData\Local\DuckStation\savestates\HASH-957757946319438E_1.sav"
IMG = "duckstation-qt-x64-ReleaseLTCG.exe"
B = vg.XUSB_BUTTON

def log(m): print("[%6.1f] %s" % (time.monotonic()-T0, m), flush=True)

def nearest_dist(path):
    """nearest live-zombie distance (entity dist field @ enemy+0x?); reuse enemy array
    DAT_800acc2c stride 0x1f4. dist is precomputed by the game at enemy+0xd8? -- instead
    use the enemy_state script's value via a light re-read: scan active enemies, dist at
    the field the script prints (we read it the same way)."""
    r = Ram(path); base = 0x800acc2c; best = 10**9
    for i in range(8):
        e = base + i*0x1f4
        act = r.u8(e+0x0)
        # distance the script reports lives at enemy+0xdc (player-enemy dist, set per frame)
        d = r.u32(e+0xdc)
        if 0 < d < best: best = d
    return best

def main():
    global T0
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", default="stage_saves/equip_test.sav")
    ap.add_argument("--load", type=float, default=16.0)
    ap.add_argument("--path", default="R0.5,U2,R0.3,U5")  # approach (tank: U fwd, R/L turn)
    ap.add_argument("--aim", type=float, default=1.2)
    ap.add_argument("--shots", type=int, default=10)       # interleaved fire+save iterations
    ap.add_argument("--outdir", default="stage_saves/fire_caps")
    args = ap.parse_args()
    T0 = time.monotonic()
    os.makedirs(args.outdir, exist_ok=True)
    for f in os.listdir(args.outdir):
        if f.startswith("cap_"): os.remove(os.path.join(args.outdir, f))

    gp = vg.VX360Gamepad(); time.sleep(3.0); log("vgamepad up")
    def hold_btn(btn, secs):
        gp.press_button(button=btn); gp.update(); time.sleep(secs)
        gp.release_button(button=btn); gp.update(); time.sleep(0.2)
    def lbsave(dst):
        mt = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        gp.press_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update(); time.sleep(0.16)
        gp.release_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update()
        for _ in range(25):
            time.sleep(0.06)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) > mt: break
        time.sleep(0.08); shutil.copy2(SLOT1, dst)

    proc = subprocess.Popen([DUCK, "-batch", "-statefile", args.state, CUE])
    log("loading %.0fs..." % args.load); time.sleep(args.load)

    PMAP = {"U":B.XUSB_GAMEPAD_DPAD_UP,"D":B.XUSB_GAMEPAD_DPAD_DOWN,
            "L":B.XUSB_GAMEPAD_DPAD_LEFT,"R":B.XUSB_GAMEPAD_DPAD_RIGHT}
    for tok in args.path.split(","):
        tok=tok.strip()
        if not tok: continue
        log("approach %s"%tok); hold_btn(PMAP[tok[0].upper()], float(tok[1:] or "0.5"))

    log("aim R1 %.1fs"%args.aim)
    gp.press_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update(); time.sleep(args.aim)
    # interleave: fire Square, then LB-save+copy -> catch the just-spawned effect
    for i in range(args.shots):
        gp.press_button(button=B.XUSB_GAMEPAD_X); gp.update(); time.sleep(0.08)
        gp.release_button(button=B.XUSB_GAMEPAD_X); gp.update(); time.sleep(0.05)
        lbsave(os.path.join(args.outdir, "cap_%02d.sav"%i))
        log("shot+snap %d"%i)
    gp.release_button(button=B.XUSB_GAMEPAD_RIGHT_SHOULDER); gp.update()

    log("close")
    subprocess.run(["taskkill","/IM",IMG], capture_output=True)
    for _ in range(40):
        time.sleep(0.5)
        if proc.poll() is not None: break

    # scan captures for an active effect-0
    log("=== scanning %d captures for effect-0 ===" % args.shots)
    POOL=0x800a73b8; STRIDE=0x108
    hits=[]
    for i in range(args.shots):
        p=os.path.join(args.outdir,"cap_%02d.sav"%i)
        if not os.path.exists(p): continue
        r=Ram(p); act=[]
        for s in range(0x60):
            b=POOL+s*STRIDE
            if r.u8(b+0x6c)!=0 and s<16:
                act.append((s, r.u8(b+0x70), r.u16(b+0x30), r.u16(b+0x32)))
        eff0=[a for a in act if a[1]==0]
        print("  cap_%02d: low-pool=%s  effect-0=%s"%(i, act if act else "none",
              [(hex(a[2]),hex(a[3])) for a in eff0] if eff0 else "NONE"))
        if eff0: hits.append((p,eff0))
    print("EFFECT-0 CAPTURED in %d frames"%len(hits) if hits else "NO effect-0 captured")

if __name__=="__main__":
    main()
