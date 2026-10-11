"""Diagnose: kommt der virtuelle Pad im room2050_orig.sav-Lauf an? Padwort 0x800ac768 + Blick bei gehaltenen Tasten,
frei laufend (ohne GDB-Stopp dazwischen) und nach Stopp/Cont; DuckStation-Log (LogToFile) fuer die Controller-Enumeration."""
import os, shutil, struct, subprocess, sys, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL
STATE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "room2050_orig.sav")
out = os.path.abspath(sys.argv[1]); os.makedirs(out, exist_ok=True)
log = open(os.path.join(out, "log.txt"), "w")
def L(*a):
    s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
import vgamepad as vg
B = vg.XUSB_BUTTON
gp = vg.VX360Gamepad(); time.sleep(2.0)
bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(INI, bak)
eigene = set()
try:
    ini_set(INI, "Debug", "EnableGDBServer", "true")
    ini_set(INI, "Main", "ConfirmPowerOff", "false")
    try:
        ini_set(INI, "Logging", "LogToFile", "true")
    except KeyError as e:
        L("LogToFile nicht in der ini:", e)
    pad_bind_alle(INI)
    vor = pids()
    subprocess.Popen([DUCK, "-batch", "-statefile", STATE, CUE], cwd=out)
    for _ in range(20):
        time.sleep(1); eigene = pids() - vor
        if eigene: break
    L("DuckStation gestartet", eigene)
    g = Gdb(PORT); L("GDB verbunden"); time.sleep(8)
    def padwort():
        return struct.unpack("<H", g.mem(0x800AC768, 2))[0]
    def blick():
        return struct.unpack("<h", g.mem(PL + 0x6A, 2))[0]
    # A: frei laufend, Taste halten, waehrenddessen nur kurz anhalten zum Lesen
    for name, btn in (("RIGHT", B.XUSB_GAMEPAD_DPAD_RIGHT), ("UP", B.XUSB_GAMEPAD_DPAD_UP), ("X/Square", B.XUSB_GAMEPAD_X)):
        gp.press_button(button=btn); gp.update()
        for i in range(4):
            time.sleep(0.5)
            L("Stopp:", g.stop()); L("  %s gehalten %.1fs: Padwort 0x%04x Blick %d pos %s" % (name, 0.5 * (i + 1), padwort(), blick(),
                                           struct.unpack("<iii", g.mem(PL + 0x34, 12)))); g.cont()
        gp.release_button(button=btn); gp.update(); time.sleep(0.5)
    L("Stopp:", g.stop()); L("losgelassen: Padwort 0x%04x" % padwort()); g.cont()
    # B: ganz ohne GDB-Stopp 3 s RIGHT halten, dann einmal lesen
    gp.press_button(button=B.XUSB_GAMEPAD_DPAD_RIGHT); gp.update(); time.sleep(3.0)
    L("Stopp:", g.stop()); L("RIGHT 3 s frei: Padwort 0x%04x Blick %d" % (padwort(), blick())); g.cont()
    gp.release_button(button=B.XUSB_GAMEPAD_DPAD_RIGHT); gp.update()
    time.sleep(0.5); g.stop()
    g.cont(); time.sleep(0.3)
    for p in eigene: subprocess.run(["taskkill", "/PID", p], capture_output=True)
finally:
    t0 = time.time()
    while time.time() - t0 < 20 and (pids() & eigene): time.sleep(1)
    for p in pids() & eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
    shutil.copy2(bak, INI)
    L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" % ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))
    dl = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\duckstation.log")
    if os.path.exists(dl):
        for l in open(dl, encoding="utf-8", errors="replace").read().splitlines()[-400:]:
            if "ontroller" in l or "SDL" in l and ("insert" in l.lower() or "open" in l.lower() or "bind" in l.lower()):
                L("DUCKLOG:", l.strip())
