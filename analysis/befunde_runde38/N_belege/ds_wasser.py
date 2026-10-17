"""Runde 38 Spur N — DuckStation-Gegenprobe ROOM1260 (RE1.5 MZD-Disc, Original-EXE).

Ablauf: Debug-Menue-Spielstand (stage_saves/mzd_debugmenu.sav) -> JUMP 0x126 SEWER PASSAGE -> per GDB Leon auf
(x, z, Blick) stellen -> Haltepunkte im Schritt FUN_80045630 (Eintritt @0x80045630, Spawn @0x80045710, Tonindex
@0x80045798) -> Medienmitschnitt (Bild + Ton, Hotkey ToggleMediaCapture an SDL-0/LeftShoulder) -> UP halten ->
Mitschnitt aus -> geordnet schliessen. settings.ini wird gesichert und zurueckgeschrieben.

Aufruf: python ds_wasser.py <ausgabe> <x> <z> <blick> <geh_sekunden> [lauf=0|1]
"""
import os, shutil, socket, struct, subprocess, sys, time
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\settings.ini")
VIDS = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\videos")
STATE = r"C:\workspace\git\reAi_v2\.claude\worktrees\r38_wasser\stage_saves\mzd_debugmenu.sav"
PORT = 2345
PL = 0x800ACA54
B = vg.XUSB_BUTTON
BP_EIN, BP_SPAWN, BP_TON = 0x80045630, 0x80045710, 0x80045798


class Gdb:
    def __init__(s, port):
        t0 = time.time()
        while True:
            try:
                s.sk = socket.create_connection(("127.0.0.1", port), timeout=2); break
            except OSError:
                if time.time() - t0 > 60: raise
                time.sleep(1)
        s.buf = b""

    def recv(s, timeout=10.0):
        t0 = time.time()
        while True:
            i = s.buf.find(b"$")
            if i >= 0:
                j = s.buf.find(b"#", i)
                if j >= 0 and len(s.buf) >= j + 3:
                    d = s.buf[i + 1:j]; s.buf = s.buf[j + 3:]; s.sk.sendall(b"+"); return d
            left = timeout - (time.time() - t0)
            if left <= 0: raise TimeoutError("keine GDB-Antwort")
            s.sk.settimeout(min(left, 1.0))
            try:
                d = s.sk.recv(65536)
                if not d: raise EOFError("GDB zu")
                s.buf += d
            except socket.timeout:
                pass

    def cmd(s, data, timeout=10.0):
        if isinstance(data, str): data = data.encode()
        s.sk.sendall(b"$" + data + b"#%02x" % (sum(data) & 0xFF))
        return s.recv(timeout)

    def mem(s, a, n):
        out = b""
        while n > 0:
            k = min(n, 0x400); r = s.cmd("m%x,%x" % (a, k))
            if r.startswith(b"E") or len(r) != 2 * k: raise IOError("m %08x -> %r" % (a, r[:20]))
            out += bytes.fromhex(r.decode()); a += k; n -= k
        return out

    def wmem(s, a, data):
        r = s.cmd("M%x,%x:%s" % (a, len(data), data.hex()))
        if r != b"OK": raise IOError("M %08x -> %r" % (a, r))

    def regs(s):
        r = s.cmd("g").decode()
        return [struct.unpack("<I", bytes.fromhex(r[i * 8:i * 8 + 8]))[0] for i in range(min(38, len(r) // 8))]


def ini_set(path, section, key, value):
    lines = open(path, encoding="utf-8").read().splitlines()
    sec, done = None, False
    for i, l in enumerate(lines):
        if l.startswith("["): sec = l.strip()
        elif sec == "[%s]" % section and l.split("=")[0].strip() == key:
            lines[i] = "%s = %s" % (key, value); done = True
    if not done: raise KeyError(key)
    open(path, "w", encoding="utf-8").write("\n".join(lines) + "\n")


def pids():
    out = subprocess.run(["tasklist", "/NH", "/FI", "IMAGENAME eq duckstation-qt-x64-ReleaseLTCG.exe"],
                         capture_output=True, text=True, encoding="cp850", errors="replace").stdout
    return {l.split()[1] for l in out.splitlines() if l.split() and l.split()[0].lower().startswith("duck")}


R = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
     "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]


def main():
    out = os.path.abspath(sys.argv[1]); stellen = sys.argv[2] != "spawn"
    x, z, blick = (int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4])) if stellen else (0, 0, 0)
    geh = float(sys.argv[5]); lauf = len(sys.argv) > 6 and sys.argv[6] == "1"
    tri = int(os.getenv("DS_TRI", "0")); rechts = int(os.getenv("DS_RECHTS", "2"))
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    T0 = time.time()
    def L(*a):
        s_ = "[%6.1f] " % (time.time() - T0) + " ".join(str(v) for v in a); print(s_, flush=True); log.write(s_ + "\n"); log.flush()
    if pids():
        L("DuckStation laeuft bereits — Abbruch"); return 2
    bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(INI, bak)
    vids_vor = set(os.listdir(VIDS)) if os.path.isdir(VIDS) else set()
    eigene = set(); proc = None
    try:
        import ctypes
        xi = ctypes.windll.xinput1_4
        class XS(ctypes.Structure):
            _fields_ = [("pkt", ctypes.c_uint32), ("b", ctypes.c_uint16), ("lt", ctypes.c_ubyte), ("rt", ctypes.c_ubyte),
                        ("lx", ctypes.c_short), ("ly", ctypes.c_short), ("rx", ctypes.c_short), ("ry", ctypes.c_short)]
        def slots():
            r = []
            for i in range(4):
                st_ = XS()
                if xi.XInputGetState(i, ctypes.byref(st_)) == 0: r.append(i)
            return r
        sl_vor = slots()
        gp = vg.VX360Gamepad(); time.sleep(3.0)
        sl_nach = slots(); mein = sorted(set(sl_nach) - set(sl_vor))
        L("vgamepad bereit, XInput vorher", sl_vor, "nachher", sl_nach, "mein", mein)
        sdl = mein[0] if mein else 0
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Hotkeys", "ToggleMediaCapture", "SDL-%d/LeftShoulder" % sdl)
        ini_set(INI, "Hotkeys", "SaveSelectedSaveState", "Keyboard/F2")
        ini_set(INI, "Logging", "LogToFile", "true")
        ini_set(INI, "MediaCapture", "Container", "mp4")
        mt_ini = os.path.getmtime(INI)
        def tap(btn, hold=0.13, gap=0.6):
            gp.press_button(button=btn); gp.update(); time.sleep(hold)
            gp.release_button(button=btn); gp.update(); time.sleep(gap)
        vor = pids()
        proc = subprocess.Popen([DUCK, "-batch", "-statefile", STATE, CUE], cwd=out)
        for _ in range(15):
            time.sleep(1); eigene = pids() - vor
            if eigene: break
        L("DuckStation gestartet", eigene)
        time.sleep(16)
        for _ in range(2): tap(B.XUSB_GAMEPAD_DPAD_UP, 0.06, 0.30)
        tap(B.XUSB_GAMEPAD_DPAD_DOWN, 0.06, 0.50)
        for i in range(tri): L("Dreieck", i + 1); tap(B.XUSB_GAMEPAD_Y, 0.06, 0.55)
        for i in range(rechts): L("Rechts", i + 1); tap(B.XUSB_GAMEPAD_DPAD_RIGHT, 0.08, 0.45)
        L("Quadrat = laden"); tap(B.XUSB_GAMEPAD_X, 0.13, 0.6)
        time.sleep(10 + (8 if tri else 0))
        g = Gdb(PORT); L("GDB verbunden")
        g.sk.sendall(b"\x03"); time.sleep(0.5); L("Stopp:", g.cmd("?"))
        rdt = struct.unpack("<I", g.mem(0x800AC778, 4))[0]
        flr = struct.unpack("<I", g.mem(rdt + 0x34, 4))[0]
        f0 = g.mem(flr, 2 + 12)
        L("RDT %08x FLR %08x n=%d e0 x=%d z=%d w=%d d=%d typ=%#x" % ((rdt, flr, struct.unpack_from("<H", f0, 0)[0]) +
          struct.unpack_from("<4h", f0, 2) + (f0[10],)))
        px, py, pz = struct.unpack("<3i", g.mem(PL + 0x34, 12)); yaw = struct.unpack("<h", g.mem(PL + 0x6A, 2))[0]
        L("Leon vorher (%d,%d,%d) Blick %d Band %d" % (px, py, pz, yaw, g.mem(PL + 0x82, 1)[0]))
        if stellen:
            g.wmem(PL + 0x34, struct.pack("<i", x)); g.wmem(PL + 0x3C, struct.pack("<i", z))
            g.wmem(PL + 0x40, struct.pack("<h", x)); g.wmem(PL + 0x44, struct.pack("<h", z))
            g.wmem(PL + 0x6A, struct.pack("<h", blick))
        L("Stempel +0x88 =", struct.unpack("<h", g.mem(PL + 0x88, 2))[0])
        px, py, pz = struct.unpack("<3i", g.mem(PL + 0x34, 12))
        L("Leon gestellt (%d,%d,%d) Blick %d" % (px, py, pz, struct.unpack("<h", g.mem(PL + 0x6A, 2))[0]))
        for bp in (BP_EIN, BP_SPAWN, BP_TON):
            L("Z0 %08x:" % bp, g.cmd("Z0,%x,4" % bp))
        g.sk.sendall(b"$c#63")
        time.sleep(1.0)
        L("Mitschnitt AN"); tap(B.XUSB_GAMEPAD_LEFT_SHOULDER, 0.15, 0.6)
        time.sleep(1.0)
        knoepfe = [B.XUSB_GAMEPAD_DPAD_UP] + ([B.XUSB_GAMEPAD_A] if lauf else [])
        for b in knoepfe: gp.press_button(button=b)
        gp.update(); L("UP halten %.1fs%s" % (geh, " + X (Lauf)" if lauf else ""))
        t_end = time.time() + geh
        n = 0
        while time.time() < t_end:
            try:
                r = g.recv(timeout=max(0.05, t_end - time.time()))
            except TimeoutError:
                break
            rg = g.regs(); pc = rg[37] if len(rg) > 37 else 0
            pos = struct.unpack("<3i", g.mem(PL + 0x34, 12)); st = struct.unpack("<h", g.mem(PL + 0x88, 2))[0]
            gier = struct.unpack("<h", g.mem(PL + 0x6A, 2))[0]
            if pc == BP_EIN:
                ent = g.mem(PL, 0x1A0)
                fw_ptr = struct.unpack_from("<I", ent, 0x168)[0]
                fw = struct.unpack("<I", g.mem(fw_ptr, 4))[0] if 0x80000000 <= fw_ptr < 0x80200000 else 0
                L("EIN  a0=%d a1=%d Leon=(%d,%d,%d) Blick=%d +0x88=%d clip(+0x94)=%d bild(+0x95)=%d +0x168=%08x wort=%08x" % (
                    rg[4], rg[5], pos[0], pos[1], pos[2], gier, st, ent[0x94], ent[0x95], fw_ptr, fw))
            elif pc == BP_SPAWN:
                a2, a3 = rg[6], rg[7]
                m = g.mem(a2, 32); off = struct.unpack("<3i", g.mem(a3, 12))
                Rm = struct.unpack_from("<9h", m, 0); T = struct.unpack_from("<3i", m, 20)
                ank = [T[i] + ((Rm[i * 3 + 0] * off[0] + Rm[i * 3 + 1] * off[1] + Rm[i * 3 + 2] * off[2]) >> 12) for i in range(3)]
                L("SPAWN a0=%08x a1=%#x a2=%08x versatz=%s s0(boden)=%#x anker=(%d,%d,%d) R=%s T=%s Leon=(%d,%d,%d)" % (
                    rg[4], rg[5], a2, off, rg[16], ank[0], ank[1], ank[2], Rm, T, pos[0], pos[1], pos[2]))
            elif pc == BP_TON:
                L("TON  index(v0)=%#x s0=%#x Leon=(%d,%d,%d)" % (rg[2], rg[16], pos[0], pos[1], pos[2]))
            else:
                L("Stopp pc=%08x r=%r" % (pc, r[:20]))
            n += 1
            g.sk.sendall(b"$c#63")
        for b in knoepfe: gp.release_button(button=b)
        gp.update(); L("losgelassen, Stopps:", n)
        time.sleep(1.5)
        # Pool lesen (Effekt 6 aktiv?)
        g.sk.sendall(b"\x03"); time.sleep(0.3)
        try:
            L("Stopp2:", g.recv(3))
        except TimeoutError:
            L("Stopp2: keine Antwort")
        pool = g.mem(0x800A73B8, 96 * 132)
        for i in range(96):
            e = pool[i * 132:(i + 1) * 132]
            if e[108] & 1:
                L("POOL %2d fl=%02x id=%d sub=%d skala=%#x wpos=(%d,%d,%d)" % (i, e[108], e[112], e[113],
                  struct.unpack_from("<H", e, 114)[0]) + struct.unpack_from("<3h", e, 40))
        for bp in (BP_EIN, BP_SPAWN, BP_TON):
            g.cmd("z0,%x,4" % bp)
        g.sk.sendall(b"$c#63"); time.sleep(1.0)
        L("Mitschnitt AUS"); tap(B.XUSB_GAMEPAD_LEFT_SHOULDER, 0.15, 1.5)
        time.sleep(2.0)
        return 0
    finally:
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        try:
            if os.path.getmtime(INI) >= mt_ini - 1:
                shutil.copy2(bak, INI); L("settings.ini zurueckgeschrieben")
        except Exception as ex:
            L("INI-Ruecksicherung:", ex)
        try:
            shutil.copy2(os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\duckstation.log"), os.path.join(out, "duckstation.log"))
        except Exception as ex:
            L("log:", ex)
        neu = sorted(set(os.listdir(VIDS)) - vids_vor) if os.path.isdir(VIDS) else []
        for v in neu:
            shutil.copy2(os.path.join(VIDS, v), os.path.join(out, v)); L("Video:", v)


if __name__ == "__main__":
    sys.exit(main())
