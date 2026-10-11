"""orig_diag.py - Diagnose fuer orig_1090_push.py: laeuft die Emulation nach 'c' (Haltepunkt am Objekt-Pass
FUN_8002bd44 @0x8002bd44, einmal je Bild) und kommt der virtuelle Pad im Spiel an (Padwort 0x800ac768, das die
Schubkette @0x8002bf24 liest; 0x800ac76c/0x800ac770 daneben)? Aufruf: python orig_diag.py <ordner>"""
import os, sys, time, struct, subprocess, shutil
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import orig_1090_push as O


def main():
    out = os.path.abspath(sys.argv[1]); os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    if O.pids(): L("DuckStation laeuft bereits"); return 2
    import vgamepad as vg
    B = vg.XUSB_BUTTON
    gp = vg.VX360Gamepad(); time.sleep(3.0)
    bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(O.INI, bak)
    eigene = set()
    try:
        O.ini_set(O.INI, "Debug", "EnableGDBServer", "true")
        O.ini_set(O.INI, "Main", "ConfirmPowerOff", "false")
        O.ini_set(O.INI, "Logging", "LogToFile", "true")
        O.pad_bind_alle(O.INI)
        vor = O.pids()
        subprocess.Popen([O.DUCK, "-batch", "-statefile", O.STATE, O.CUE], cwd=out)
        for _ in range(20):
            time.sleep(1); eigene = O.pids() - vor
            if eigene: break
        L("DuckStation", eigene)
        g = O.Gdb(O.PORT); L("GDB verbunden"); time.sleep(8)
        try:
            import win32gui, win32con
            hw = []
            def cb(h, _):
                t = win32gui.GetWindowText(h)
                if win32gui.IsWindowVisible(h) and "DuckStation" in t: hw.append((h, t))
            win32gui.EnumWindows(cb, None)
            L("Fenster:", hw)
            for h, t in hw:
                try:
                    win32gui.ShowWindow(h, win32con.SW_RESTORE); win32gui.SetForegroundWindow(h); L("Fokus ->", t)
                except Exception as ex: L("Fokus-Fehler", ex)
        except Exception as ex:
            L("win32 nicht verfuegbar", ex)
        L("stop:", g.stop())
        def pl():
            e = g.mem(O.PL, 0x90)
            return (struct.unpack_from("<i", e, 0x34)[0], struct.unpack_from("<i", e, 0x3C)[0], struct.unpack_from("<h", e, 0x6A)[0])
        def padwort():
            return g.mem(0x800AC768, 12).hex()
        L("PL", pl(), "pad", padwort())
        # 1) laeuft die Emulation? Haltepunkt je Bild
        L("Z0 bd44:", g.cmd("Z0,8002bd44,4"))
        g.cont(); r = g.wait_stop(5.0); L("Stopp nach c (<=5 s):", r)
        if r is not None:
            g.cont(); r2 = g.wait_stop(5.0); L("zweiter Stopp:", r2)
        L("z0:", g.cmd("z0,8002bd44,4"))
        L("stop:", g.stop()); L("PL", pl())
        # 2) Pad: UP halten, laufen lassen, anhalten, Padwort lesen (mehrfach)
        for i in range(3):
            gp.press_button(button=B.XUSB_GAMEPAD_DPAD_UP); gp.update()
            g.cont(); time.sleep(1.0)
            L("UP gehalten %d: stop %s pad %s PL %s" % (i, g.stop(), padwort(), pl()))
        gp.release_button(button=B.XUSB_GAMEPAD_DPAD_UP); gp.update()
        g.cont(); time.sleep(0.5); L("losgelassen: stop %s pad %s PL %s" % (g.stop(), padwort(), pl()))
        # 3) Cross/Start tippen (Menue-Reaktion?) und Blick
        gp.press_button(button=B.XUSB_GAMEPAD_DPAD_RIGHT); gp.update(); g.cont(); time.sleep(1.0)
        L("RIGHT gehalten: stop %s pad %s PL %s" % (g.stop(), padwort(), pl()))
        gp.release_button(button=B.XUSB_GAMEPAD_DPAD_RIGHT); gp.update()
        g.cont(); time.sleep(0.3)
        for p in eigene: subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        t0 = time.time()
        while time.time() - t0 < 20 and (O.pids() & eigene): time.sleep(1)
        for p in O.pids() & eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, O.INI)
        L("settings.ini zurueck:", "EnableGDBServer = false" in open(O.INI, encoding="utf-8").read())
        try:
            dl = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\duckstation.log")
            for l in open(dl, encoding="utf-8", errors="replace").read().splitlines()[-400:]:
                if any(k in l for k in ("SDL", "ontroller", "GDB", "Pad", "Input", "ViGEm", "XInput", "state")):
                    L("  DSLOG:", l[:160])
        except Exception as ex:
            L("log?", ex)


if __name__ == "__main__":
    sys.exit(main())
