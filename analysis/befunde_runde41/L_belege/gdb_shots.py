#!/usr/bin/env python3
"""Runde 41 Spur L - Original-Messung RE1.5 MIT BILDERN (MZD-Disc, DuckStation-GDB + Screenshot-Hotkey).
Muster: analysis/befunde_runde38/R_belege/gdb_spinne.py (Sprung per Debug-Menue-Tick, Teleport, Bildzeile je Takt).
Neu: je Takt im Fenster [--shot_von, --shot_bis] ein DuckStation-Screenshot (Hotkey Screenshot temporaer auf
SDL-0/LeftShoulder, vgamepad), ScreenshotMode = InternalResolution; je Takt die Figur-Matrix +0x20 (RotMatrix(+0x68)
@0x8001e8f4) und die Weltmatrix des Wurzelteils (Teil 0 +0x40, FUN_80022da0 @0x8001ea24), Clip +0x94, Bild +0x95,
Ueberblendung +0x8f.
settings.ini wird nur fuer den Lauf geaendert und im finally wiederhergestellt (auch nach dem Beenden).
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil, glob
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
SHOTS = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "screenshots")

ap = argparse.ArgumentParser()
ap.add_argument("--state", default=r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_spinnenanim\stage_saves\mzd_debugmenu.sav")
ap.add_argument("--stage", type=int, default=1)
ap.add_argument("--idx", type=int, default=6)
ap.add_argument("--frames", type=int, default=160)
ap.add_argument("--out", required=True)
ap.add_argument("--pl", default="6000,3000,1024")
ap.add_argument("--tp", type=int, default=60)
ap.add_argument("--shot_von", type=int, default=56)
ap.add_argument("--shot_bis", type=int, default=150)
ap.add_argument("--slot", type=int, default=0, help="Figur-Platz der Spinne")
ap.add_argument("--shotmode", default="ScreenResolution")
a = ap.parse_args()

PL, ENT0 = 0x800aca54, 0x800acc2c
os.makedirs(a.out, exist_ok=True)
B = vg.XUSB_BUTTON

ini_alt = open(INI, "r", encoding="utf-8", errors="replace", newline="").read()
assert "EnableGDBServer = false" in ini_alt, "settings.ini: EnableGDBServer nicht false (laeuft eine andere Messung?)"
shutil.copyfile(INI, os.path.join(a.out, "settings.ini.vorher"))
neu = ini_alt.replace("EnableGDBServer = false", "EnableGDBServer = true", 1)
neu = neu.replace("SaveSelectedSaveState = SDL-0/LeftShoulder\n", "")
neu = neu.replace("Screenshot = Keyboard/F10", "Screenshot = SDL-0/LeftShoulder", 1)
neu = neu.replace("ScreenshotMode = ScreenResolution", "ScreenshotMode = " + a.shotmode, 1)
assert "Screenshot = SDL-0/LeftShoulder" in neu


def ini_zurueck():
    open(INI, "w", encoding="utf-8", newline="").write(ini_alt)


proc = None
log = open(os.path.join(a.out, "orig.log"), "w")
buf = b""
try:
    gp = vg.VX360Gamepad()
    time.sleep(3.0)
    open(INI, "w", encoding="utf-8", newline="").write(neu)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    s = None
    for _ in range(80):
        try:
            s = socket.create_connection(("127.0.0.1", 2345), timeout=2)
            break
        except OSError:
            time.sleep(0.5)
    if s is None:
        raise SystemExit("keine Verbindung")
    s.settimeout(60)

    def send(c):
        k = sum(c.encode()) & 0xff
        s.sendall(b"$" + c.encode() + b"#%02x" % k)

    def recv_pkt():
        global buf
        while True:
            i = buf.find(b"$")
            if i >= 0:
                j = buf.find(b"#", i)
                if j >= 0 and len(buf) >= j + 3:
                    pkt = buf[i + 1:j]
                    buf = buf[j + 3:]
                    s.sendall(b"+")
                    return pkt.decode(errors="replace")
            d = s.recv(65536)
            if not d:
                raise EOFError
            buf += d

    def cmd(c):
        send(c)
        return recv_pkt()

    def rd(addr, n):
        return bytes.fromhex(cmd("m%08x,%x" % (addr, n)))

    def wr(addr, data):
        r = cmd("M%08x,%x:%s" % (addr, len(data), data.hex()))
        assert r == "OK", r

    def schuss(name):
        vor = set(glob.glob(os.path.join(SHOTS, "*.png")))
        gp.press_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER)
        gp.update()
        time.sleep(0.12)
        gp.release_button(button=B.XUSB_GAMEPAD_LEFT_SHOULDER)
        gp.update()
        t0 = time.time()
        while time.time() - t0 < 6.0:
            neu_ = set(glob.glob(os.path.join(SHOTS, "*.png"))) - vor
            if neu_:
                f = sorted(neu_, key=os.path.getmtime)[-1]
                g1 = -1
                for _ in range(20):
                    time.sleep(0.05)
                    g2 = os.path.getsize(f)
                    if g2 == g1 and g2 > 0:
                        break
                    g1 = g2
                ziel = os.path.join(a.out, name)
                shutil.move(f, ziel)
                return ziel
            time.sleep(0.03)
        return None

    print("?", cmd("?"))
    print("Z0 menu", cmd("Z0,8001443c,4"))
    send("c")
    recv_pkt()
    wr(0x800bbe5e, bytes([a.stage]))
    wr(0x800bbe5f + a.stage, bytes([a.idx]))
    wr(0x800ac9a8, b"\x00\x00\x00\x00")
    wr(0x800b5359, b"\x01")
    wr(0x800bbe5c, b"\x00")
    log.write("# Sprung: stage %d idx %d bei VSync %d\n" % (a.stage, a.idx, struct.unpack("<I", rd(0x800787dc, 4))[0]))
    print("z0 menu", cmd("z0,8001443c,4"))
    print("Z0 loop", cmd("Z0,8001a50c,4"))
    n = 0
    geladen = False
    tp_done = False
    bild = 0
    t0 = time.time()
    fehl = 0
    while True:
        send("c")
        recv_pkt()
        vs = struct.unpack("<I", rd(0x800787dc, 4))[0]
        stage = rd(0x800b0fe0, 1)[0]
        room = struct.unpack("<H", rd(0x800b0fe2, 2))[0]
        if not geladen:
            if stage == a.stage and room == a.idx:
                log.write("# geladen VSync %d stage %d room %d prolog 801109e4=%s\n" % (vs, stage, room, rd(0x801109e4, 4).hex()))
                geladen = True
            else:
                bild += 1
                if bild > 3000:
                    raise SystemExit("Raum nicht geladen")
                continue
        n += 1
        if a.pl and not tp_done and n >= a.tp:
            x, z, yaw = [int(v) for v in a.pl.split(",")]
            wr(PL + 0x34, struct.pack("<i", x))
            wr(PL + 0x3c, struct.pack("<i", z))
            wr(PL + 0x6a, struct.pack("<H", yaw & 0xfff))
            tp_done = True
            log.write("# Teleport Spieler -> %s bei Bild %d\n" % (a.pl, n))
        p = rd(PL, 0xa0)
        cut = struct.unpack("<h", rd(0x800b0fe4, 2))[0]
        zeile = "%d n%d cut%d | pl %d %d %d r%d hp %d" % (
            vs, n, cut, *struct.unpack_from("<3i", p, 0x34),
            struct.unpack_from("<H", p, 0x6a)[0], struct.unpack_from("<h", p, 0x9a)[0])
        e = rd(ENT0 + a.slot * 0x1f4, 0x1f4)
        if struct.unpack_from("<I", e, 0)[0] & 1:
            x, y, z = struct.unpack_from("<3i", e, 0x34)
            rx, ry, rz = struct.unpack_from("<3h", e, 0x68)
            m = struct.unpack_from("<9h", e, 0x20)
            parts = struct.unpack_from("<I", e, 0x188)[0]
            zeile += " w0=%08x sh[%s]" % (struct.unpack_from("<I", e, 0)[0], ",".join(str(v) for v in struct.unpack_from("<8h", e, 0xb0)))
            zeile += " | e%d t%02x s%d/%d/%d/%d g%02x @%d,%d,%d r%d,%d,%d c%d/%d f8f=%d gy%d M[%s]" % (
                a.slot, e[8], e[4], e[5], e[6], e[7], e[9], x, y, z, rx, ry, rz, e[0x94], e[0x95], e[0x8f],
                struct.unpack_from("<h", e, 0x1ba)[0], ",".join(str(v) for v in m))
            if 0x80000000 <= parts < 0x80200000:
                p0 = rd(parts, 172)
                wm = struct.unpack_from("<9h", p0, 0x40)
                wt = struct.unpack_from("<3i", p0, 0x54)
                rt = struct.unpack_from("<3h", p0, 0x38)
                zeile += " P0W[%s] P0T(%d,%d,%d) P0root(%d,%d,%d)" % (",".join(str(v) for v in wm), *wt, *rt)
        if a.shot_von <= n <= a.shot_bis:
            f = schuss("o%03d.png" % n)
            zeile += " shot=%s" % (os.path.basename(f) if f else "FEHLT")
            if not f:
                fehl += 1
        log.write(zeile + "\n")
        log.flush()
        if n % 20 == 0:
            print("Bild", n, "%.0f s" % (time.time() - t0), "fehl", fehl, flush=True)
        if n >= a.frames:
            break
        if fehl >= 5:
            print("ABBRUCH: 5 Screenshots fehlen")
            break
    log.close()
    try:
        cmd("z0,8001a50c,4")
        send("c")
    except Exception:
        pass
    s.close()
finally:
    if proc is not None:
        subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
        for _ in range(40):
            time.sleep(0.5)
            if proc.poll() is not None:
                break
        if proc.poll() is None:
            subprocess.run(["taskkill", "/F", "/PID", str(proc.pid)], capture_output=True)
        time.sleep(1.0)
    ini_zurueck()
    print("DONE", "settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace", newline="").read() == ini_alt)
