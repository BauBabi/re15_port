#!/usr/bin/env python3
"""Runde 38 Spur R — WER klemmt die Schachtspinne beim Herauskriechen? (Original RE1.5, DuckStation-GDB)

Befund orig_2060_schacht_gift.txt: Bild 67 z 5600 -> 5525 (statt 5400), Bild 68 x 5800 -> 5975 und z 5325.
Weder ROOM2060-SCA (kein Band-2-Feld) noch FUN_800245d8 (reiner Vorschub) erklaeren das. Diese Messung
klammert jeden Aufruf der Spinnen-Wurzel 0x801109e4 (STAGE2.BIN) mit Haltepunkten hinter jedem jal und
liest dort x/z der Spinne e0 (0x800acc2c):
  0x801109e4  Eintritt
  0x80110a54  nach jalr Zustand (Schacht-Choreografie)
  0x80110a64  nach jal 0x8002b498
  0x80110a74  nach jal 0x8002aec4 (Spieler <-> Spinne)
  0x80110a84  nach jal 0x8002b544 (Gegner <-> Spinne)
  0x80110aa4  nach jal 0x8003b0a4 (SCA)
  0x80110b58  nach jal 0x8001b064
  0x8001a50c  Gegner-Schleife (Bildgrenze)
Muster/Ablauf wie gdb_spinne.py (Sprung ueber den Debug-Menue-Tick, Spieler bei Bild --tp nach --pl).
settings.ini: EnableGDBServer nur fuer den Lauf true.
"""
import socket, subprocess, time, struct, argparse, os, shutil
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI  = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
ap = argparse.ArgumentParser()
ap.add_argument("--state", default=r"C:\workspace\git\reAi_v2\stage_saves\mzd_debugmenu.sav")
ap.add_argument("--stage", type=int, default=1); ap.add_argument("--idx", type=int, default=6)
ap.add_argument("--pl", default="6000,3000,1024"); ap.add_argument("--tp", type=int, default=60)
ap.add_argument("--von", type=int, default=61); ap.add_argument("--bis", type=int, default=76)
ap.add_argument("--out", required=True)
ap.add_argument("--watch", default=None, help="Z2-Schreib-Wachpunkt <adr_hex> ab Bild --von (statt der Klammern)")
ap.add_argument("--treffer", type=int, default=12)
a = ap.parse_args()
PL, ENT0, CUR = 0x800aca54, 0x800acc2c, 0x800ac784
KLAMMERN = [0x801109e4, 0x80110a54, 0x80110a64, 0x80110a74, 0x80110a84, 0x80110aa4, 0x80110b58]
NAMEN = {0x801109e4: "eintritt", 0x80110a54: "n.zustand", 0x80110a64: "n.b498", 0x80110a74: "n.aec4(spieler)",
         0x80110a84: "n.b544(gegner)", 0x80110aa4: "n.b0a4(sca)", 0x80110b58: "n.b064", 0x8001a50c: "schleife"}

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
    ini_zurueck()
    s.settimeout(60); buf = b""
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
    def pc():
        regs = cmd("g")
        return struct.unpack("<I", bytes.fromhex(regs[37*8:38*8]))[0]
    out = open(a.out, "w")
    print("?", cmd("?"))
    print("Z0 menu", cmd("Z0,8001443c,4"))
    send("c"); recv_pkt()
    wr(0x800bbe5e, bytes([a.stage])); wr(0x800bbe5f + a.stage, bytes([a.idx]))
    wr(0x800ac9a8, b"\x00\x00\x00\x00"); wr(0x800b5359, b"\x01"); wr(0x800bbe5c, b"\x00")
    print("z0 menu", cmd("z0,8001443c,4"))
    print("Z0 loop", cmd("Z0,8001a50c,4"))
    n = 0; geladen = False; bild = 0; klammer = False
    while True:
        send("c"); recv_pkt()
        p = pc()
        if p == 0x8001a50c:
            stage = rd(0x800b0fe0, 1)[0]; room = struct.unpack("<H", rd(0x800b0fe2, 2))[0]
            if not geladen:
                if stage == a.stage and room == a.idx:
                    out.write("# geladen stage %d room %d prolog 801109e4=%s\n" % (stage, room, rd(0x801109e4, 4).hex()))
                    geladen = True
                else:
                    bild += 1
                    if bild > 2000: raise SystemExit("Raum nicht geladen")
                    continue
            n += 1
            if n == a.tp:
                x, z, yaw = [int(v) for v in a.pl.split(",")]
                wr(PL + 0x34, struct.pack("<i", x)); wr(PL + 0x3c, struct.pack("<i", z))
                wr(PL + 0x6a, struct.pack("<H", yaw & 0xfff))
                out.write("# Teleport Spieler -> %s bei Bild %d\n" % (a.pl, n))
            if n == a.von and not klammer:
                if a.watch:
                    r = cmd("Z2,%s,4" % a.watch)
                    out.write("# Z2 %s -> %r\n" % (a.watch, r))
                    if r != "OK":
                        break
                    cmd("z0,8001a50c,4")
                    for k in range(a.treffer):
                        send("c"); stop = recv_pkt()
                        regs = cmd("g")
                        def reg(i): return struct.unpack("<I", bytes.fromhex(regs[i*8:(i+1)*8]))[0]
                        e = rd(ENT0, 0x1f4)
                        x, y, zz = struct.unpack_from("<3i", e, 0x34)
                        out.write("WATCH %d stop=%s pc=%08x ra=%08x a0=%08x a1=%08x s0=%08x s1=%08x e0 @%d,%d,%d c%d/%d\n" % (
                            k, stop, reg(37), reg(31), reg(4), reg(5), reg(16), reg(17), x, y, zz, e[0x94], e[0x95]))
                        out.flush()
                    cmd("z2,%s,4" % a.watch)
                    break
                for k in KLAMMERN: cmd("Z0,%08x,4" % k)
                klammer = True
            if n > a.bis:
                break
        if not klammer:
            continue
        cur = struct.unpack("<I", rd(CUR, 4))[0]
        e = rd(ENT0, 0x1f4)
        x, y, zz = struct.unpack_from("<3i", e, 0x34)
        pl = rd(PL, 0x40)
        px, py, pz = struct.unpack_from("<3i", pl, 0x34)
        out.write("n%d %-18s cur=%08x e0 @%d,%d,%d s%d/%d/%d/%d g%02x f%d c%d/%d | pl %d,%d,%d\n" % (
            n, NAMEN.get(p, "%08x" % p), cur, x, y, zz, e[4], e[5], e[6], e[7], e[9], e[0x82], e[0x94], e[0x95],
            px, py, pz))
        out.flush()
    # Tabelle der aktiven Entities (Typ, Lage, Kasten +0x78) einmal am Ende
    ents = rd(ENT0, 20 * 0x1f4)
    for i in range(20):
        e = ents[i*0x1f4:(i+1)*0x1f4]
        if not (struct.unpack_from("<I", e, 0)[0] & 1): continue
        bx = struct.unpack_from("<I", e, 0x78)[0]
        box = rd(bx, 12) if 0x80000000 <= bx < 0x80200000 else b""
        out.write("# Entity %d typ %02x @%d,%d,%d floor %d Kasten@%08x %s\n" % (
            i, e[8], *struct.unpack_from("<3i", e, 0x34), e[0x82], bx,
            str(struct.unpack("<6h", box)) if len(box) == 12 else "-"))
    out.close()
    try:
        for k in KLAMMERN: cmd("z0,%08x,4" % k)
        cmd("z0,8001a50c,4"); send("c")
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
    ini_zurueck()
    print("DONE", "settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace").read() == ini_alt)
