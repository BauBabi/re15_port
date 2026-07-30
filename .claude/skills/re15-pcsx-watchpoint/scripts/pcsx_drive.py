"""
re15-pcsx-watchpoint : launch PCSX-Redux with a Lua memory-write watchpoint and drive the
MZD intro (New Game) via a virtual X360 pad, so an address's writes get logged while the game runs.

Why PCSX-Redux (not DuckStation): only PCSX-Redux has a scriptable Lua memory-breakpoint API
(PCSX.addBreakpoint(addr,'Write',...)) — the one thing needed to catch an INDIRECT/DMA write that a
static disasm scan misses. DuckStation's debugger is GUI-only.

Config via env (all optional, defaults target the fade-direction flag word 0x800aca3c):
  PCSX_WATCH_ADDR   hex address to watch          (default 0x800aca3c)
  PCSX_WATCH_LOG    log path                      (default shots\pcsx_watch.log)
  PCSX_WATCH_BITMASK hex bit to flag "<== SETS"   (default 0)
  PCSX_MAX_SECS     run budget                    (default 600)

Gotchas learned:
  * PCSX-Redux is LuaJIT (5.1): the watch .lua must use bit.band/bit.rshift, not & >> operators.
  * -iso path must have NO spaces/parentheses -> hardlink the disc to a simple path first
    (New-Item -ItemType HardLink; same drive, no 215MB copy).
  * breakpoints need -interpreter -debugger (dynarec has no accurate breakpoints; interpreter is ~5x slower).
  * the vgamepad must be created BEFORE PCSX-Redux launches (GLFW enumerates it at/after startup).
  * boot-to-title on the interpreter ~110s; the fade/room writes only start after New Game.
"""
import vgamepad as vg
import subprocess, time, os

B = vg.XUSB_BUTTON
EXE = os.getenv("PCSX_EXE", r"C:\Users\mjoedicke\AppData\Local\Microsoft\WinGet\Packages\GrumpyCoders.PCSX-Redux_Microsoft.Winget.Source_8wekyb3d8bbwe\pcsx-redux.exe")
BIOS = os.getenv("PCSX_BIOS", r"C:\tmp\scph1001.bin")           # copy of SCPH1001.BIN (DuckStation bios dir)
ISO  = os.getenv("PCSX_ISO",  r"C:\tmp\mzd.cue")                # hardlink/cue to the MZD disc (simple path!)
LUA  = os.getenv("PCSX_LUA",  os.path.join(os.path.dirname(__file__), "watch_addr.lua"))
LOG  = os.getenv("PCSX_WATCH_LOG", r"C:\workspace\git\reAi_v2\shots\pcsx_watch.log")
MAXS = int(os.getenv("PCSX_MAX_SECS", "600"))

def tap(gp, btn, hold=0.12, gap=0.35):
    gp.press_button(button=btn); gp.update(); time.sleep(hold)
    gp.release_button(button=btn); gp.update(); time.sleep(gap)

def setter_found():
    try:
        with open(LOG) as f:
            return any(">>> SETTER" in ln for ln in f)
    except Exception:
        return False

gp = vg.VX360Gamepad(); gp.update(); time.sleep(1.0)
print("vgamepad created", flush=True)
proc = subprocess.Popen([EXE, "-bios", BIOS, "-iso", ISO, "-interpreter", "-debugger",
                         "-run", "-dofile", LUA, "-stdout", "-lua_stdout"],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
print("pcsx-redux launched pid", proc.pid, flush=True)

t0 = time.time(); next_ng = 110
while time.time() - t0 < MAXS:
    time.sleep(5)
    el = time.time() - t0
    if setter_found():
        print("SETTER FOUND at %.0fs" % el, flush=True); break
    if el >= next_ng:  # New Game: Up x3 + Cross, then Leon/confirm + a few Cross to advance movies/dialog
        print("[%.0fs] driving New Game + advance" % el, flush=True)
        for _ in range(3): tap(gp, B.XUSB_GAMEPAD_DPAD_UP, gap=0.45)
        for _ in range(3): tap(gp, B.XUSB_GAMEPAD_A, gap=0.8)
        for _ in range(4): tap(gp, B.XUSB_GAMEPAD_A, gap=0.5)
        next_ng = el + 18
print("done; log:", LOG, flush=True)
