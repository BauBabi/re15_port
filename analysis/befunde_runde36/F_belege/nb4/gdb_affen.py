#!/usr/bin/env python3
"""Runde 36 Spur F NB4 (Abnahme 3, Maengel 1/2): Gorilla-Stau / Nahjagd im ORIGINAL messen.
Muster: Spur J jnb6/gdbm2.py (DuckStation-GDB-Server Port 2345, r3-Savestate s033 direkt geladen, Haltepunkt
Gorilla-Wurzel 0x80116db8, Patch beim ersten Halt mit cur == e2 (0x800ad014) ab VSync --arm).
Patch (optional je Aktor): Lage +0x34/+0x38/+0x3c (int32) + Vorlage +0x40/+0x42/+0x44 (int16), Yaw +0x6a;
Gorilla zusaetzlich +0x4..+0x7 := 1/sub/0/sub3, +0x82 := 0, +0x1dc := 0.
Je e2-Halt (= einmal je Bild, e1 schon getickt, e2 noch nicht) eine Zeile: VSync, e1, e2, Leon.
--hp N: Leons HP nach jedem Bild auf N halten (wie der Port-Riegel, damit der Kampf nicht endet).
settings.ini [Debug] EnableGDBServer wird nur fuer den Lauf auf true gesetzt und danach zurueckgeschrieben."""
import socket, subprocess, time, sys, struct, argparse, os, shutil
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI  = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
ap = argparse.ArgumentParser()
ap.add_argument("--state", required=True); ap.add_argument("--arm", type=int, default=10029)
ap.add_argument("--frames", type=int, default=600); ap.add_argument("--out", required=True)
ap.add_argument("--pl", default=None, help="x,z,yaw")
ap.add_argument("--e1", default=None, help="x,z,yaw,sub,sub3")
ap.add_argument("--e2", default=None, help="x,z,yaw,sub,sub3")
ap.add_argument("--hp", type=int, default=0)
a = ap.parse_args()
E1, E2, PL = 0x800ace20, 0x800ad014, 0x800aca54

ini_alt = open(INI, "r", encoding="utf-8", errors="replace").read()
assert "EnableGDBServer = false" in ini_alt, "settings.ini: EnableGDBServer nicht false (laeuft eine andere Messung?)"
shutil.copyfile(INI, a.out + ".settings.ini.vorher")
open(INI, "w", encoding="utf-8").write(ini_alt.replace("EnableGDBServer = false", "EnableGDBServer = true", 1))
proc = None
def ini_zurueck():
    open(INI, "w", encoding="utf-8").write(ini_alt)
try:
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    s = None
    for _ in range(80):
        try: s = socket.create_connection(("127.0.0.1", 2345), timeout=2); break
        except OSError: time.sleep(0.5)
    if s is None: raise SystemExit("keine Verbindung")
    ini_zurueck()   # der Server laeuft; die Datei sofort zurueck (DuckStation liest sie nur beim Start)
    s.settimeout(30); buf = b""
    def send(cmd):
        c = sum(cmd.encode()) & 0xff
        s.sendall(b"$" + cmd.encode() + b"#%02x" % c)
    def recv_pkt():
        global buf
        while True:
            i = buf.find(b"$")
            if i >= 0:
                j = buf.find(b"#", i)
                if j >= 0 and len(buf) >= j + 3:
                    pkt = buf[i+1:j]; buf = buf[j+3:]; s.sendall(b"+"); return pkt.decode(errors="replace")
            d = s.recv(65536)
            if not d: raise EOFError
            buf += d
    def cmd(c): send(c); return recv_pkt()
    def rd(addr, n): return bytes.fromhex(cmd("m%08x,%x" % (addr, n)))
    def wr(addr, data): r = cmd("M%08x,%x:%s" % (addr, len(data), data.hex())); assert r == "OK", r
    print("?", cmd("?")); print("Z0", cmd("Z0,80116db8,4"))
    out = open(a.out, "w"); patched = False; n = 0; t0 = time.time()
    def lage(base, x, z, yaw):
        wr(base + 0x34, struct.pack("<3i", x, 0, z)); wr(base + 0x40, struct.pack("<3h", x, 0, z)); wr(base + 0x6a, struct.pack("<H", yaw & 0xfff))
    def gor(base, spec):
        x, z, yaw, sub, sub3 = [int(v) for v in spec.split(",")]
        lage(base, x, z, yaw); wr(base + 0x04, bytes([1, sub, 0, sub3])); wr(base + 0x82, b"\x00"); wr(base + 0x1dc, b"\x00\x00")
    def ezeile(e):
        x, y, z = struct.unpack_from("<3i", e, 0x34); yaw = struct.unpack_from("<H", e, 0x6a)[0]
        return "%d %d r%d s%d/%d/%d/%d c%d/%d d%d k%d h%d L%d B%d b%d" % (
            x, z, yaw, e[4], e[5], e[6], e[7], e[0x94], e[0x95], struct.unpack_from("<H", e, 0x1d4)[0],
            struct.unpack_from("<h", e, 0x1d6)[0], struct.unpack_from("<H", e, 0x1de)[0], struct.unpack_from("<H", e, 0x1d0)[0],
            struct.unpack_from("<h", e, 0x1dc)[0], e[0x82])
    while True:
        send("c"); recv_pkt()
        vs = struct.unpack("<I", rd(0x800787dc, 4))[0]
        cur = struct.unpack("<I", rd(0x800ac784, 4))[0]
        if cur != E2: continue
        if not patched:
            if vs < a.arm: continue
            if a.pl:
                x, z, yaw = [int(v) for v in a.pl.split(",")]; lage(PL, x, z, yaw)
            if a.e1: gor(E1, a.e1)
            if a.e2: gor(E2, a.e2)
            patched = True
            out.write("# Patch bei VSync %d: pl=%s e1=%s e2=%s hp=%d\n" % (vs, a.pl, a.e1, a.e2, a.hp))
        if a.hp:
            hp = struct.unpack("<h", rd(PL + 0x9a, 2))[0]
            if hp < a.hp: wr(PL + 0x9a, struct.pack("<h", a.hp))
        e1 = rd(E1, 0x1f4); e2 = rd(E2, 0x1f4); p = rd(PL, 0xa0)
        px, pz = struct.unpack_from("<i", p, 0x34)[0], struct.unpack_from("<i", p, 0x3c)[0]
        out.write("%d | e1 %s | e2 %s | pl %d %d r%d hp %d s%d/%d/%d/%d\n" % (vs, ezeile(e1), ezeile(e2), px, pz,
                  struct.unpack_from("<H", p, 0x6a)[0], struct.unpack_from("<h", p, 0x9a)[0], p[4], p[5], p[6], p[7]))
        n += 1
        if n % 200 == 0: out.flush(); print("Bild", n, "%.0f s" % (time.time() - t0), flush=True)
        if n >= a.frames: break
    out.close()
    try: cmd("z0,80116db8,4"); send("c")
    except Exception: pass
    s.close()
finally:
    if proc is not None:
        subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
        for _ in range(40):
            time.sleep(0.5)
            if proc.poll() is not None: break
        if proc.poll() is None: subprocess.run(["taskkill", "/F", "/PID", str(proc.pid)], capture_output=True)
        time.sleep(1.0)
    ini_zurueck()   # NACH dem Prozessende (DuckStation kann beim Beenden schreiben)
    print("DONE", "settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace").read() == ini_alt)
