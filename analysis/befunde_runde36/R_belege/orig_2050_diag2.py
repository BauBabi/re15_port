"""Diagnose 2: Ursache der fehlenden Pad-Eingabe im Messskript isolieren. mode=z0: Haltepunkte wie im Messskript,
keine Entity-Schreibzugriffe; mode=ent: Entity-Schreibzugriffe (Spinnen aus), keine Haltepunkte; mode=beide."""
import os, shutil, struct, subprocess, sys, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL, BP_OK, BP_BLOCK, BP_FALL, BP_HIT
from orig_2050_push import Gdb2, ENT, ENT_SZ
STATE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "room2050_orig.sav")
out = os.path.abspath(sys.argv[1]); mode = sys.argv[2]; os.makedirs(out, exist_ok=True)
log = open(os.path.join(out, "log.txt"), "w")
def L(*a):
    s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
import vgamepad as vg
B = vg.XUSB_BUTTON
gp = vg.VX360Gamepad(); time.sleep(2.0)
bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(INI, bak)
eigene = set()
try:
    ini_set(INI, "Debug", "EnableGDBServer", "true"); ini_set(INI, "Main", "ConfirmPowerOff", "false"); pad_bind_alle(INI)
    vor = pids()
    subprocess.Popen([DUCK, "-batch", "-statefile", STATE, CUE], cwd=out)
    for _ in range(20):
        time.sleep(1); eigene = pids() - vor
        if eigene: break
    L("DuckStation gestartet", eigene, "mode", mode)
    g = Gdb2(PORT); L("GDB verbunden"); time.sleep(8)
    L("Stopp:", g.stop())
    if mode in ("ent", "beide", "ent2"):
        for s in range(2):
            a = ENT + ENT_SZ * s
            w0 = struct.unpack("<H", g.mem(a, 2))[0]
            g.memw(a, struct.pack("<H", w0 & 0xFFFE))
            L("Entity %d: w0 %04x -> %04x" % (s, w0, struct.unpack("<H", g.mem(a, 2))[0]))
    if mode == "ent2":
        # g_active_count @0x800aca4e (FUN_8001a50c: lbu s1,-13746(s1) @0x8001a528, Schleife bis s0 == s1 @0x8001a5b0-b8)
        n = g.mem(0x800ACA4E, 1)[0]
        g.memw(0x800ACA4E, bytes([max(0, n - 2)]))
        L("g_active_count @0x800aca4e: %d -> %d" % (n, g.mem(0x800ACA4E, 1)[0]))
    if mode in ("z0", "beide"):
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
    g.cont()
    def padwort(): return struct.unpack("<H", g.mem(0x800AC768, 2))[0]
    def blick(): return struct.unpack("<h", g.mem(PL + 0x6A, 2))[0]
    gp.press_button(button=B.XUSB_GAMEPAD_DPAD_RIGHT); gp.update()
    for i in range(5):
        time.sleep(0.5)
        r = g.stop(); L("Stopp:", r, "RIGHT %.1fs: Padwort 0x%04x Blick %d" % (0.5 * (i + 1), padwort(), blick())); g.cont()
    gp.release_button(button=B.XUSB_GAMEPAD_DPAD_RIGHT); gp.update(); time.sleep(0.3)
    g.stop()
    if mode in ("z0", "beide"):
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT): g.cmd("z0,%x,4" % a)
    g.cont(); time.sleep(0.3)
    for p in eigene: subprocess.run(["taskkill", "/PID", p], capture_output=True)
finally:
    t0 = time.time()
    while time.time() - t0 < 20 and (pids() & eigene): time.sleep(1)
    for p in pids() & eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
    shutil.copy2(bak, INI)
    L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" % ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))
