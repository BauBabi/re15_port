#!/usr/bin/env python3
"""Runde 41 Spur L - Original-VIDEO der Schachtspinne ROOM2060 (MZD-Disc, DuckStation-GDB + Media Capture).
GDB nur fuer Sprung (Debug-Menue-Tick, Muster R38 gdb_spinne.py) und Teleport des Spielers bei Bild --tp; danach
laeuft das Spiel FREI (kein Haltepunkt), die Media Capture (FFmpeg/mp4, Nachtrag 1) nimmt auf, was der Spieler sieht.
Hotkey ToggleMediaCapture temporaer auf SDL-0/LeftShoulder (vgamepad). settings.ini byte-genau wiederhergestellt.
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil, glob
import vgamepad as vg

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
VIDEOS = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "videos")

ap = argparse.ArgumentParser()
ap.add_argument("--state", default=r"C:\workspace\git\reAi_v2\.claude\worktrees\r41_spinnenanim\stage_saves\mzd_debugmenu.sav")
ap.add_argument("--stage", type=int, default=1)
ap.add_argument("--idx", type=int, default=6)
ap.add_argument("--out", required=True)
ap.add_argument("--pl", default="6000,3000,1024")
ap.add_argument("--tp", type=int, default=60)
ap.add_argument("--rec_an", type=int, default=40, help="Bild, bei dem die Aufnahme startet")
ap.add_argument("--frei_ab", type=int, default=64, help="ab diesem Bild ohne Haltepunkt")
ap.add_argument("--frei_s", type=float, default=9.0)
ap.add_argument("--stop_bei", type=int, default=0, help=">0: Aufnahme bei Bild N im Haltepunkt-Betrieb beenden")
a = ap.parse_args()

PL = 0x800aca54
os.makedirs(a.out, exist_ok=True)
B = vg.XUSB_BUTTON
ini_alt = open(INI, "r", encoding="utf-8", errors="replace", newline="").read()
assert "EnableGDBServer = false" in ini_alt, "settings.ini: EnableGDBServer nicht false (laeuft eine andere Messung?)"
shutil.copyfile(INI, os.path.join(a.out, "settings.ini.vorher"))
neu = ini_alt.replace("EnableGDBServer = false", "EnableGDBServer = true", 1)
neu = neu.replace("SaveSelectedSaveState = SDL-0/LeftShoulder\n", "")
neu = neu.replace("ToggleMediaCapture = Keyboard/F12", "ToggleMediaCapture = SDL-0/LeftShoulder", 1)
assert "ToggleMediaCapture = SDL-0/LeftShoulder" in neu


def ini_zurueck():
    open(INI, "w", encoding="utf-8", newline="").write(ini_alt)


proc = None
log = open(os.path.join(a.out, "orig_video.log"), "w")
buf = b""
vids_vorher = set(glob.glob(os.path.join(VIDEOS, "*")))
try:
    gp = vg.VX360Gamepad()
    time.sleep(3.0)

    def tap(btn, hold=0.15):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update()

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

    print("?", cmd("?"))
    cmd("Z0,8001443c,4")
    send("c"); recv_pkt()
    wr(0x800bbe5e, bytes([a.stage])); wr(0x800bbe5f + a.stage, bytes([a.idx]))
    wr(0x800ac9a8, b"\x00\x00\x00\x00"); wr(0x800b5359, b"\x01"); wr(0x800bbe5c, b"\x00")
    cmd("z0,8001443c,4")
    cmd("Z0,8001a50c,4")
    n = 0; geladen = False; bild = 0
    while True:
        send("c"); recv_pkt()
        vs = struct.unpack("<I", rd(0x800787dc, 4))[0]
        stage = rd(0x800b0fe0, 1)[0]
        room = struct.unpack("<H", rd(0x800b0fe2, 2))[0]
        if not geladen:
            if stage == a.stage and room == a.idx:
                geladen = True
                log.write("# geladen VSync %d\n" % vs)
            else:
                bild += 1
                if bild > 3000:
                    raise SystemExit("Raum nicht geladen")
                continue
        n += 1
        cut = struct.unpack("<h", rd(0x800b0fe4, 2))[0]
        e = rd(0x800acc2c, 0xa0)
        log.write("%d n%d cut%d e0 s%d/%d/%d/%d c%d/%d f8f=%d @%d,%d,%d r%d,%d,%d\n" % (vs, n, cut, e[4], e[5], e[6], e[7], e[0x94], e[0x95], e[0x8f], *struct.unpack_from("<3i", e, 0x34), *struct.unpack_from("<3h", e, 0x68)))
        if n == a.rec_an:
            tap(B.XUSB_GAMEPAD_LEFT_SHOULDER)
            log.write("# LB (Aufnahme an) bei Bild %d VSync %d\n" % (n, vs))
        if n == a.tp:
            x, z, yaw = [int(v) for v in a.pl.split(",")]
            wr(PL + 0x34, struct.pack("<i", x)); wr(PL + 0x3c, struct.pack("<i", z)); wr(PL + 0x6a, struct.pack("<H", yaw & 0xfff))
            log.write("# Teleport %s bei Bild %d VSync %d\n" % (a.pl, n, vs))
        if a.stop_bei and n == a.stop_bei:
            tap(B.XUSB_GAMEPAD_LEFT_SHOULDER)
            log.write("# LB (Aufnahme aus) bei Bild %d VSync %d\n" % (n, vs))
        if a.stop_bei and n >= a.stop_bei + 6:
            break
        if not a.stop_bei and n >= a.frei_ab:
            break
    log.flush()
    cmd("z0,8001a50c,4")
    send("c")
    log.write("# frei ab Bild %d\n" % n); log.flush()
    if not a.stop_bei:
        time.sleep(a.frei_s)
        tap(B.XUSB_GAMEPAD_LEFT_SHOULDER)
        log.write("# LB (Aufnahme aus) nach %.1f s frei\n" % a.frei_s); log.flush()
    time.sleep(3.0)
    try:
        s.close()
    except Exception:
        pass
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
    neu_v = sorted(set(glob.glob(os.path.join(VIDEOS, "*"))) - vids_vorher, key=os.path.getmtime)
    for v in neu_v:
        ziel = os.path.join(a.out, "orig_2060_" + os.path.basename(v).replace(" ", "_"))
        shutil.move(v, ziel)
        print("VIDEO", ziel, os.path.getsize(ziel))
    log.close()
    print("DONE", "settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace", newline="").read() == ini_alt)
