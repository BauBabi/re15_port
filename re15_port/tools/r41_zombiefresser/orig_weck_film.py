#!/usr/bin/env python3
"""Runde 41 Spur I: ORIGINAL-Filmstreifen des Naehe-Weckers (DuckStation, Auslieferungs-EXE der MZD-Disc).

1. Savestate mit RAM-Patch (re15_ss_patch-Verfahren): Zombie-Slot N (Entity @0x800acc2c + N*0x1f4) an
   eine Position mit 2D-Abstand < 4000 zum Spieler setzen (+0x34/+0x3c, 32 Bit vorzeichenbehaftet).
2. DuckStation -batch -statefile <gepatcht> starten (virtueller Pad vorher, SDL-0).
3. Alle --step Sekunden LB (Hotkey SaveSelectedSaveState = SDL-0/LeftShoulder) und den Slot-Save kopieren.
4. Beenden NUR ueber die eigene PID (taskkill /PID ohne /F -> SaveStateOnExit).
Auswertung: fuer jede Probe +0x4/+0x5/+0x6/+0x9/+0x93/+0x94/+0x95/+0x9c/+0x1d0 der Zombie-Slots.

Aufruf: python orig_weck_film.py --state <in.sav> --slot 3 --x 1350 --z -20500 --n 16 --step 0.5 --out <dir>
"""
import argparse, os, shutil, struct, subprocess, sys, time
HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
from re15_ss import Ram  # noqa: E402
import zstandard  # noqa: E402

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
SSDIR = os.path.join(os.environ.get("LOCALAPPDATA", ""), "DuckStation", "savestates")
SLOT1 = os.path.join(SSDIR, "HASH-957757946319438E_1.sav")
ENEMY, STRIDE = 0x800acc2c, 0x1f4
PSX_EXE = os.path.join(REPO, "info", "Re1.5", "PSX.EXE")

def patch(inp, outp, patches):
    data = open(inp, "rb").read()
    f1_off = struct.unpack_from("<I", data, 0xd4)[0]
    f1_csz = struct.unpack_from("<I", data, 0xcc)[0]
    assert f1_off + f1_csz == len(data)
    blob = bytearray(zstandard.ZstdDecompressor().stream_reader(data[f1_off:]).read())
    sig = open(PSX_EXE, "rb").read()[0x800:0x800 + 48]
    base = blob.find(sig) - 0x10000
    for addr, val in patches:
        struct.pack_into("<i", blob, base + (addr - 0x80000000), val)
    f1_new = zstandard.ZstdCompressor(level=12).compress(bytes(blob))
    out = bytearray(data[:f1_off]) + f1_new
    struct.pack_into("<I", out, 0xcc, len(f1_new))
    open(outp, "wb").write(out)

def zeile(sav, slots):
    r = Ram(sav)
    t = []
    for s in slots:
        a = ENEMY + s * STRIDE
        if (r.u32(a) & 1) == 0:
            t.append(f"[{s} leer]"); continue
        t.append(f"[{s} t={r.u8(a+8):02x} w={r.u8(a+4)}/{r.u8(a+5)}/{r.u8(a+6)}/{r.u8(a+7)} g={r.u8(a+9):02x} "
                 f"mo={r.u8(a+0x94):02x} af={r.u8(a+0x95)} f8f={r.u8(a+0x8f)} h93={r.u8(a+0x93):02x} "
                 f"t9c={r.s16(a+0x9c)} d={r.u32(a+0x1d0)} pos=({r.s32(a+0x34) if hasattr(r,'s32') else r.s16(a+0x34)},"
                 f"{r.s16(a+0x3c)})]")
    return " ".join(t)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--state", required=True)
    ap.add_argument("--slot", type=int, default=3)
    ap.add_argument("--x", type=int, required=True)
    ap.add_argument("--z", type=int, required=True)
    ap.add_argument("--n", type=int, default=16)
    ap.add_argument("--step", type=float, default=0.5)
    ap.add_argument("--lead", type=float, default=4.0)
    ap.add_argument("--out", required=True)
    ap.add_argument("--resume", action="store_true",
                    help="EINE Probe: nach --lead Sekunden grazioes schliessen (SaveStateOnExit -> _resume.sav)")
    a = ap.parse_args()
    import vgamepad as vg
    a.out = os.path.normpath(os.path.abspath(a.out))     # DuckStation braucht Backslash-Pfade
    os.makedirs(a.out, exist_ok=True)
    gep = os.path.join(a.out, "gepatcht.sav")
    ea = ENEMY + a.slot * STRIDE
    patch(a.state, gep, [(ea + 0x34, a.x), (ea + 0x3c, a.z)])
    print("vorher :", zeile(a.state, (2, 3)), flush=True)
    print("patch  :", zeile(gep, (2, 3)), flush=True)
    gp = vg.VX360Gamepad(); time.sleep(3.0)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", gep, CUE])
    t0 = time.time(); time.sleep(a.lead)
    if a.resume:
        res = os.path.join(SSDIR, "HASH-957757946319438E_resume.sav")
        m0 = os.path.getmtime(res) if os.path.exists(res) else 0
        subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
        for _ in range(80):
            time.sleep(0.25)
            if os.path.exists(res) and os.path.getmtime(res) != m0: break
        time.sleep(1.0)
        try: proc.wait(timeout=30)
        except Exception: pass
        dst = os.path.join(a.out, "resume_lead%04.1f.sav" % a.lead)
        shutil.copy(res, dst)
        print("lead=%.1f neu=%s: %s" % (a.lead, os.path.getmtime(res) != m0, zeile(dst, (2, 3))), flush=True)
        return
    for i in range(a.n):
        m0 = os.path.getmtime(SLOT1) if os.path.exists(SLOT1) else 0
        gp.press_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update(); time.sleep(0.08)
        gp.release_button(button=vg.XUSB_BUTTON.XUSB_GAMEPAD_LEFT_SHOULDER); gp.update()
        for _ in range(40):
            time.sleep(0.05)
            if os.path.exists(SLOT1) and os.path.getmtime(SLOT1) != m0: break
        time.sleep(0.15)
        dst = os.path.join(a.out, "probe_%02d.sav" % i)
        shutil.copy(SLOT1, dst)
        print("t=%5.2f probe_%02d: %s" % (time.time() - t0, i, zeile(dst, (2, 3))), flush=True)
        time.sleep(max(0.0, a.step - 0.3))
    subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
    try: proc.wait(timeout=30)
    except Exception: pass
    print("fertig", flush=True)

if __name__ == "__main__":
    main()
