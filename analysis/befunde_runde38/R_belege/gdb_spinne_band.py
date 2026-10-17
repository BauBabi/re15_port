#!/usr/bin/env python3
"""Runde 38 Integration (versteckte Schachtspinne) — Kopie von R_belege/gdb_spinne.py mit zwei Zusaetzen:
  * je Spinne w0 = Entity-Wort 0 (Band = w0 >> 29: 4 UP / 2 LEVEL / 1 DOWN), gegen INIT @0x80110d38-48,
    Zustand 4 ohne word0-Schreiber, ACTIVE-Stempel @0x80110f94-fd4;
  * --r1 N: ab 20 Bilder nach dem Teleport das virtuelle R1 (0x800ac768 |= 0x0500, Preset @0x80073dbc [8]/[10];
    Kante 0x800ac76c am Zyklusbeginn) N Bilder halten / N Bilder loslassen, je Bild 0x800acaf3 (Rueckgabe der
    Zielhilfe FUN_8003703c, `sb v0,-13581(at)` @0x80032fbc bzw. Messer-Zweig) und den Zielzeiger *(0x800acbfc)
    mitschreiben. (Ein direkter Aufruf per Register-Umleitung geht nicht: der DuckStation-GDB-Server beantwortet
    `P` (Register schreiben) leer.)
Original-Kopf: Runde 38 Spur R — Original-Messung RE1.5 (MZD-Disc, DuckStation-GDB): Stage-2-Spinnen.
Muster: analysis/befunde_runde36/F_belege/nb4/gdb_affen.py (GDB-Server Port 2345, settings.ini nur
fuer den Lauf auf EnableGDBServer = true).

Ablauf:
  1. stage_saves/mzd_debugmenu.sav laden (Debug-Menue offen, ROOM1240).
  2. Haltepunkt FUN_8001443c (Debug-Menue-Tick): Stage-Var 0x800bbe5e := stage, Raum-Index
     0x800bbe5f[stage] := idx, 0x800ac9a8 := 0, Sprung-Anforderung 0x800b5359 := 1, Menue zu
     (0x800bbe5c := 0) = genau die Schreibsatz-Folge von Quadrat in der JUMP-Zeile (@0x80014a44-58).
  3. Haltepunkt FUN_8001a50c (Gegner-Schleife, einmal je Bild): warten bis Raum geladen, dann je Bild
     eine Zeile: VSync, angezeigter Cut (work[0x0A] = 0x800b0fe4), Spieler, jede aktive Entity vom Typ
     0x25/0x26 (Zustand, grid, floor, Lage, Drehung, Clip, +0x1ba), Effekt-Plaetze mit Routine A in
     {10,24,25} (die Gift-Zeilen der ESP 0x0d).
  --pl x,z,yaw  ab Bild --tp Spieler dorthin setzen (einmal).
  --hp N        Spieler-HP je Bild auf mindestens N halten.
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI  = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
ap = argparse.ArgumentParser()
ap.add_argument("--state", default=r"C:\workspace\git\reAi_v2\stage_saves\mzd_debugmenu.sav")
ap.add_argument("--stage", type=int, default=1); ap.add_argument("--idx", type=int, default=6)
ap.add_argument("--frames", type=int, default=600); ap.add_argument("--out", required=True)
ap.add_argument("--pl", default=None); ap.add_argument("--tp", type=int, default=30)
ap.add_argument("--hp", type=int, default=0)
ap.add_argument("--cut", type=int, default=-1)
ap.add_argument("--rngprobe", type=int, default=0)
ap.add_argument("--dump", default=None, help="Ordner: Rohabbild der Spinnen-Entities je Bild (0x1f4 B)")
ap.add_argument("--r1", type=int, default=0, help="ab dem Teleport: virtuelles R1 (0x800ac768 |= 0x0500, Preset @0x80073dbc [8]/[10]) N Bilder halten, N Bilder los, im Wechsel")
a = ap.parse_args()
PL, ENT0, FX0 = 0x800aca54, 0x800acc2c, 0x800a73b8

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
    out = open(a.out, "w")
    print("?", cmd("?"))
    # --- Phase A: Sprung ueber den Debug-Menue-Tick ---
    print("Z0 menu", cmd("Z0,8001443c,4"))
    send("c"); recv_pkt()
    wr(0x800bbe5e, bytes([a.stage]))
    wr(0x800bbe5f + a.stage, bytes([a.idx]))
    wr(0x800ac9a8, b"\x00\x00\x00\x00")
    wr(0x800b5359, b"\x01")
    wr(0x800bbe5c, b"\x00")
    out.write("# Sprung: stage %d idx %d bei VSync %d\n" % (a.stage, a.idx, struct.unpack("<I", rd(0x800787dc, 4))[0]))
    print("z0 menu", cmd("z0,8001443c,4"))
    # --- Phase B/C: Gegner-Schleife ---
    print("Z0 loop", cmd("Z0,8001a50c,4"))
    n = 0; geladen = False; tp_done = False; t0 = time.time(); bild = 0
    while True:
        send("c"); recv_pkt()
        vs = struct.unpack("<I", rd(0x800787dc, 4))[0]
        stage = rd(0x800b0fe0, 1)[0]; room = struct.unpack("<H", rd(0x800b0fe2, 2))[0]
        if not geladen:
            if stage == a.stage and room == a.idx:
                # Overlay-Kontrolle: Spinnen-Wurzel-Prolog @0x801109e4 = addiu sp,sp,-24
                pro = rd(0x801109e4, 4).hex()
                out.write("# geladen VSync %d stage %d room %d prolog 801109e4=%s\n" % (vs, stage, room, pro))
                geladen = True
            else:
                bild += 1
                if bild > 2000: raise SystemExit("Raum nicht geladen")
                continue
        n += 1
        if a.pl and not tp_done and n >= a.tp:
            x, z, yaw = [int(v) for v in a.pl.split(",")]
            wr(PL + 0x34, struct.pack("<i", x)); wr(PL + 0x3c, struct.pack("<i", z))
            wr(PL + 0x6a, struct.pack("<H", yaw & 0xfff))
            if a.cut >= 0:
                wr(0x800afbb5, bytes([a.cut])); wr(0x800b0fe4, struct.pack("<h", a.cut))
            tp_done = True; out.write("# Teleport Spieler -> %s bei Bild %d (cut %d)\n" % (a.pl, n, a.cut))
        if a.hp:
            hp = struct.unpack("<h", rd(PL + 0x9a, 2))[0]
            if hp < a.hp:
                wr(PL + 0x9a, struct.pack("<h", 100)); out.write("# HP-Reset %d -> 100 bei Bild %d\n" % (hp, n))

        if a.rngprobe and tp_done:
            print("z0 loop", cmd("z0,8001a50c,4"))
            print("Z0 rng", cmd("Z0,80012e78,4"))
            hits = 0
            while hits < a.rngprobe:
                send("c"); pkt = recv_pkt()
                regs = cmd("g")
                def reg(i): return struct.unpack("<I", bytes.fromhex(regs[i*8:(i+1)*8]))[0]
                a0 = reg(4)
                h0 = (a0 >> 7) & 0xff; r1 = (a0 + h0) & 0xff
                a01 = r1 | (h0 << 8); h1 = (a01 >> 7) & 0xff; r2 = (r1 + h1) & 0xff
                pc = reg(37)
                st = struct.unpack("<H", rd(0x800acaec, 2))[0]
                out.write("# RNG-PROBE pc=%08x a0=%08x r1=%d r2=%d gift=%d st=%04x\n" % (pc, a0, r1, r2, (r1 & 1) and (r2 & 1), st))
                out.flush(); hits += 1
                hp = struct.unpack("<h", rd(PL + 0x9a, 2))[0]
                if hp < 30: wr(PL + 0x9a, struct.pack("<h", 100))
            print("z0 rng", cmd("z0,80012e78,4"))
            break
        r1_an = 0
        if a.r1 and tp_done and n >= a.tp + 20:
            ph = ((n - a.tp - 20) // a.r1) % 2
            if ph == 0:
                held = struct.unpack("<H", rd(0x800ac768, 2))[0]
                wr(0x800ac768, struct.pack("<H", held | 0x0500)); r1_an = 1
                if (n - a.tp - 20) % a.r1 == 0:
                    edge = struct.unpack("<H", rd(0x800ac76c, 2))[0]
                    wr(0x800ac76c, struct.pack("<H", edge | 0x0500))
        p = rd(PL, 0xa0)
        cut = struct.unpack("<h", rd(0x800b0fe4, 2))[0]
        st = struct.unpack("<H", rd(0x800acaec, 2))[0]
        zeile = "%d n%d cut%d | pl %d %d %d r%d hp %d s%d/%d/%d/%d st%04x" % (
            vs, n, cut, *struct.unpack_from("<3i", p, 0x34), struct.unpack_from("<H", p, 0x6a)[0],
            struct.unpack_from("<h", p, 0x9a)[0], p[4], p[5], p[6], p[7], st)
        zeile += " r1=%d wpn=%d af3=%d ziel=%08x" % (r1_an, rd(0x800aca5d, 1)[0], rd(0x800acaf3, 1)[0],
                                                   struct.unpack("<I", rd(0x800acbfc, 4))[0])
        ents = rd(ENT0, 20 * 0x1f4)
        for i in range(20):
            e = ents[i*0x1f4:(i+1)*0x1f4]
            if not (struct.unpack_from("<I", e, 0)[0] & 1): continue
            t = e[8]
            if t not in (0x25, 0x26): continue
            x, y, z = struct.unpack_from("<3i", e, 0x34)
            rx, ry, rz = struct.unpack_from("<3h", e, 0x68)
            zeile += " | e%d t%02x w0=%08x band%d s%d/%d/%d/%d g%02x f%d @%d,%d,%d r%d,%d,%d c%d/%d gy%d hp%d sp%d h93=%02x" % (
                i, t, struct.unpack_from("<I", e, 0)[0], struct.unpack_from("<I", e, 0)[0] >> 29, e[4], e[5], e[6], e[7], e[9], e[0x82], x, y, z, rx, ry, rz, e[0x94], e[0x95],
                struct.unpack_from("<h", e, 0x1ba)[0], struct.unpack_from("<h", e, 0x9a)[0],
                struct.unpack_from("<h", e, 0x8c)[0], e[0x93])
            if a.dump:
                os.makedirs(a.dump, exist_ok=True)
                open(os.path.join(a.dump, "e%d_%05d.bin" % (i, n)), "wb").write(e)
        fx = rd(FX0, 96 * 0x84)
        for k in range(96):
            f = fx[k*0x84:(k+1)*0x84]
            fl = f[0x6c]
            if not (fl & 1): continue
            ra = struct.unpack_from("<H", f, 0)[0]
            if ra in (10, 24, 25) or struct.unpack_from("<H", f, 2)[0] in (10, 24, 25):
                zeile += " | fx%d A%d B%d fl%02x w%d p%d,%d,%d an%d" % (
                    k, ra, struct.unpack_from("<H", f, 2)[0], fl, struct.unpack_from("<H", f, 4)[0],
                    *struct.unpack_from("<3h", f, 0x28), f[0x6e])
        out.write(zeile + "\n")
        if n % 100 == 0: out.flush(); print("Bild", n, "%.0f s" % (time.time() - t0), flush=True)
        if n >= a.frames: break
    out.close()
    try: cmd("z0,8001a50c,4"); send("c")
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
