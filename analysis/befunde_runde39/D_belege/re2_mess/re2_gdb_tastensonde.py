"""re2_gdb_tastensonde.py - Runde 39 Spur D, Hilfssonde: welche vgamepad-Taste loest in RE2 (SLUS-00748, Spielstand _4,
Leon mit SMG id 15) den Feuer-Zweig des Halte-Handlers B aus, und ueber welchen Feuer-Handler laeuft die Waffe?
R1 wird gehalten, die Tasten werden je 1,2 s durchgewechselt. Haltepunkte: B Square-Zweig @0x80043828 (`andi s1,0x40`
erfuellt), B Feuer-Uebergang @0x80043838 (`addiu v0,zero,2` vor `sh v0,6(s0)`), Feuer-Handler der Tabellen
0x800a702c/48/68/80/98/b0: 0x80042c64 / 0x80043908 / 0x80047c6c / 0x80048314 / 0x800487d8 / 0x80048b8c, Applier
FUN_800410CC. Zusaetzlich: Overlay-Bytes @0x80103568/@0x80103628 live gegen EMOVL10_S0.BIN (Haltepunkt-Adressen P6/P7).
Aufruf: python re2_gdb_tastensonde.py <spielstand.sav> <ausgabeordner> [sekunden=25]"""
import os, struct, subprocess, sys, time, threading, shutil
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2_gdb_aufsteher import Gdb, ini_set, pids, DUCK, CUE, INI, PORT, PL

BPS = {0x80043828: "B:square-zweig", 0x80043838: "B:feuer->sub2", 0x80042c64: "feuer_T1", 0x80043908: "feuer_Tgun",
       0x80047c6c: "feuer_T3", 0x80048314: "feuer_T4", 0x800487d8: "feuer_T5", 0x80048b8c: "feuer_T6",
       0x800410CC: "APPLIER"}
REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", ".."))
OVL = os.path.join(REPO, "info", "re2leon", "COMMON", "BIN", "EMOVL10_S0.BIN")


class Pad(threading.Thread):
    def __init__(s):
        super().__init__(daemon=True)
        import vgamepad as vg
        s.B = vg.XUSB_BUTTON; s.pad = vg.VX360Gamepad(); s.stop = False; s.on = False; s.aktuell = "-"
        s.folge = [("X", s.B.XUSB_GAMEPAD_X), ("A", s.B.XUSB_GAMEPAD_A), ("B", s.B.XUSB_GAMEPAD_B), ("Y", s.B.XUSB_GAMEPAD_Y),
                   ("LB", s.B.XUSB_GAMEPAD_LEFT_SHOULDER), ("START", s.B.XUSB_GAMEPAD_START), ("BACK", s.B.XUSB_GAMEPAD_BACK),
                   ("LTHUMB", s.B.XUSB_GAMEPAD_LEFT_THUMB), ("RTHUMB", s.B.XUSB_GAMEPAD_RIGHT_THUMB),
                   ("DL", s.B.XUSB_GAMEPAD_DPAD_LEFT), ("DR", s.B.XUSB_GAMEPAD_DPAD_RIGHT), ("DU", s.B.XUSB_GAMEPAD_DPAD_UP),
                   ("DD", s.B.XUSB_GAMEPAD_DPAD_DOWN)]
    def run(s):
        while not s.stop:
            if not s.on: time.sleep(0.05); continue
            for name, b in s.folge:
                if s.stop or not s.on: break
                s.aktuell = name
                s.pad.press_button(s.B.XUSB_GAMEPAD_RIGHT_SHOULDER); s.pad.press_button(b); s.pad.update()
                time.sleep(0.9)
                s.pad.release_button(b); s.pad.update(); time.sleep(0.3)
            # Trigger (R2/L2) als Achsen
            for name, fn in (("RT", lambda v: s.pad.right_trigger(value=v)), ("LT", lambda v: s.pad.left_trigger(value=v))):
                if s.stop or not s.on: break
                s.aktuell = name; fn(255); s.pad.update(); time.sleep(0.9); fn(0); s.pad.update(); time.sleep(0.3)
        s.pad.reset(); s.pad.update()


def main():
    state, out = os.path.normpath(sys.argv[1]), os.path.abspath(sys.argv[2])
    secs = int(sys.argv[3]) if len(sys.argv) > 3 else 25
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    if pids(): L("DuckStation laeuft bereits - Abbruch"); return 2
    bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(INI, bak)
    pad = Pad(); pad.start(); time.sleep(2.0)
    eigene = set()
    try:
        if "EnableGDBServer = true" not in open(INI, encoding="utf-8").read():
            ini_set(INI, "Debug", "EnableGDBServer", "true")
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE], cwd=out)
        for _ in range(15):
            time.sleep(1); eigene = pids() - vor
            if eigene: break
        g = Gdb(PORT); time.sleep(6)
        g.sk.sendall(b"\x03"); time.sleep(0.5); L("Stopp:", g.cmd("?"))
        ovl = open(OVL, "rb").read()
        for a in (0x80103568, 0x80103628, 0x80102BE8):
            live = g.mem(a, 16); disk = ovl[a - 0x80100000:a - 0x80100000 + 16]
            L("overlay @%08x live=%s disk=%s %s" % (a, live.hex(), disk.hex(), "GLEICH" if live == disk else "VERSCHIEDEN"))
        for bp in BPS: L("Z0 %08x %s:" % (bp, BPS[bp]), g.cmd("Z0,%x,4" % bp))
        pad.on = True
        t0 = time.time(); n = 0
        while time.time() - t0 < secs:
            g.sk.sendall(b"$c#63")
            try:
                g.recv(1.0)
            except TimeoutError:
                g.sk.sendall(b"\x03"); time.sleep(0.2)
                try: g.recv(1)
                except TimeoutError: pass
                g.cmd("?")
                L("  herz t=%5.1f taste=%-6s L w4=%08x 14c=%08x" % (time.time() - t0, pad.aktuell, g.u32(PL + 4), g.u32(PL + 0x14C)))
                continue
            pc = g.pc(); n += 1
            L("STOPP t=%5.1f pc=%08x %-14s taste=%-6s L w4=%08x 14c=%08x" % (time.time() - t0, pc, BPS.get(pc, "?"), pad.aktuell,
              g.u32(PL + 4), g.u32(PL + 0x14C)))
            if BPS.get(pc) == "APPLIER":
                g.cmd("z0,%x,4" % pc)      # nur einmal
        pad.on = False
        for bp in BPS: g.cmd("z0,%x,4" % bp)
        g.sk.sendall(b"$c#63"); time.sleep(0.2)
        L("Stopps:", n)
        for p in eigene: subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        pad.stop = True
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene): time.sleep(1)
        for p in pids() & eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI); L("settings.ini zurueckgeschrieben")


if __name__ == "__main__":
    sys.exit(main())
