"""re2_biss_gdb.py — Runde 37 Spur J "bissarm": den RE2-ZOMBIE-GRIFF im RE2-Retail-Original messen.

RE2 (SLUS-00748, re2leon.cue; EXE = info/re2leon/PSX.EXE, Code-Abgleich ../../../befunde_runde35/H_raeume/re2_mess/
re2_ss.py) in DuckStation ueber dessen GDB-Server: Spielstand laden (Standard SLUS-00748_4.sav = Stage 0 Raum 20,
EIN Zombie Typ 0x10), Leon vor/hinter den Zombie stellen (RAM-Schreiben wie der Tuerwechsel FUN_80026b7c:
+0x38/+0x40 s32, +0x44/+0x48, +0x118/+0x11A s16, +0x76 Blick), Haltepunkt auf den BISS-LOOP P3 des Zombie-Griffs
(EMOVL10_S0.BIN @0x80102838 = EXEC[3] P3, im Spielstand RELOZIERT geladen: Basis = Fundstelle der Datei-Bytes),
an jedem Halt: PL-Entity (0x248) + Leons 16 Parts (0xAC, PL+0x198) + Zombie-Entity + 17 Zombie-Parts.
Nach `bilder` Halts: Haltepunkt weg, weiterlaufen, DuckStation geordnet schliessen (SaveStateOnExit) -> das
Bild dieses Moments steht in der VRAM des Resume-Spielstands (Kopie nach <ausgabe>/halt_resume.sav).

Sicherung: settings.ini UND der Resume-Spielstand des Nutzers werden vorher kopiert und am Ende zurueckgeschrieben.

Aufruf: python re2_biss_gdb.py <ausgabe> <vorn|hinten> <abstand> <bilder> [spielstand]
Musterwerkzeug: analysis/befunde_runde35/H_raeume/re2_mess/re2_gdb_grab.py (Runde 35 Spur H)."""
import math, os, shutil, socket, struct, subprocess, sys, time

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue"
INI = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\settings.ini")
SAVES = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates")
HERE = os.path.dirname(os.path.abspath(__file__))
OVL = os.path.join(HERE, "..", "..", "..", "..", "info", "re2leon", "COMMON", "BIN", "EMOVL10_S0.BIN")
PORT = 2345
PL, ETAB = 0x800CFBF8, 0x800CFE18
P3_DATEI = 0x2838            # EXEC[3] P3 Biss-Loop (EMOVL10_S0.BIN raw @0x80100000 -> +0x2838)
ENT_SZ, PART_SZ = 0x248, 0xAC


class Gdb:
    def __init__(s, port):
        t0 = time.time()
        while True:
            try:
                s.sk = socket.create_connection(("127.0.0.1", port), timeout=2); break
            except OSError:
                if time.time() - t0 > 90: raise
                time.sleep(1)
        s.buf = b""

    def recv(s, timeout=10.0):
        t0 = time.time()
        while True:
            i = s.buf.find(b"$")
            if i >= 0:
                j = s.buf.find(b"#", i)
                if j >= 0 and len(s.buf) >= j + 3:
                    data = s.buf[i + 1:j]; s.buf = s.buf[j + 3:]; s.sk.sendall(b"+")
                    return data
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

    def mem(s, addr, n):
        out = b""
        while n > 0:
            k = min(n, 0x400)
            r = s.cmd("m%x,%x" % (addr, k))
            if r.startswith(b"E") or len(r) != 2 * k: raise IOError("m %08x -> %r" % (addr, r[:20]))
            out += bytes.fromhex(r.decode()); addr += k; n -= k
        return out

    def wmem(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK": raise IOError("M %08x -> %r" % (addr, r))

    def u32(s, a): return struct.unpack("<I", s.mem(a, 4))[0]
    def s32(s, a): return struct.unpack("<i", s.mem(a, 4))[0]
    def s16(s, a): return struct.unpack("<h", s.mem(a, 2))[0]


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


def main():
    out = os.path.abspath(sys.argv[1]); seite = sys.argv[2]; abst = int(sys.argv[3]); nbild = int(sys.argv[4])
    state = sys.argv[5] if len(sys.argv) > 5 else os.path.join(SAVES, "SLUS-00748_4.sav")
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    if pids():
        L("DuckStation laeuft bereits — Abbruch"); return 2
    ovl = open(OVL, "rb").read()
    ini_bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(INI, ini_bak)
    resume = os.path.join(SAVES, "SLUS-00748_resume.sav")
    res_bak = os.path.join(out, "resume_vorher.sav")
    if os.path.exists(resume): shutil.copy2(resume, res_bak)
    t_resume = os.path.getmtime(resume) if os.path.exists(resume) else 0
    eigene = set()
    try:
        if "EnableGDBServer = true" not in open(INI, encoding="utf-8").read():
            ini_set(INI, "Debug", "EnableGDBServer", "true")
        # Ohne Rueckfrage schliessen, sonst haengt das geordnete Schliessen am Dialog und SaveStateOnExit
        # schreibt kein Bild (gemessen 2026-10-12: Lauf vorn_a ohne Bild-Spielstand). Am Ende zurueck.
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE], cwd=out)
        for _ in range(20):
            time.sleep(1); eigene = pids() - vor
            if eigene: break
        L("DuckStation gestartet", eigene)
        g = Gdb(PORT); L("GDB verbunden")
        time.sleep(6)
        g.sk.sendall(b"\x03"); time.sleep(0.5)
        L("Stopp:", g.cmd("?"))
        L("stage", g.s16(0x800D481C), "room", g.s16(0x800D481E), "PL", g.s32(PL + 0x38), g.s32(PL + 0x40),
          "yaw", g.s16(PL + 0x76))
        # Overlay-Basis: die Datei-Bytes des P3 im RAM suchen (Spielstand 4: relozierte Ladung @0x8010D000).
        basis = None
        for b in range(0x80100000, 0x80140000, 0x1000):
            if g.mem(b + P3_DATEI, 16) == ovl[P3_DATEI:P3_DATEI + 16]:
                basis = b; break
        if basis is None:
            L("EMOVL10_S0-P3 nicht im RAM — Abbruch"); return 3
        bp = basis + P3_DATEI
        L("Overlay-Basis %08x, Haltepunkt P3 %08x" % (basis, bp))
        z = None
        for k in range(1, 33):
            p = g.u32(ETAB + 4 * k)
            if 0x80000000 <= p < 0x80200000 and p != 0x800D424C and 0x10 <= g.mem(p + 8, 1)[0] <= 0x1F:
                z = p; break
        if z is None:
            L("kein Zombie — Abbruch"); return 4
        zx, zz, zyaw = g.s32(z + 0x38), g.s32(z + 0x40), g.s16(z + 0x76)
        L("Zombie %08x Typ %02x (%d,%d) yaw %d w4 %08x" % (z, g.mem(z + 8, 1)[0], zx, zz, zyaw, g.u32(z + 4)))
        # Vorwaerts eines RE2-Aktors = (cos yaw, -sin yaw) (Wurzelversatz 0x80015CB8; Port re15_clip_root_motion_abs).
        a = (zyaw & 0xFFF) * 2 * math.pi / 4096.0
        fx, fz = math.cos(a), -math.sin(a)
        lx, lz = int(zx + fx * abst), int(zz + fz * abst)
        lyaw = (zyaw + 2048) & 0xFFF if seite == "vorn" else zyaw & 0xFFF
        for adr, fmt, v in ((PL + 0x38, "<i", lx), (PL + 0x40, "<i", lz), (PL + 0x44, "<h", lx), (PL + 0x48, "<h", lz),
                            (PL + 0x118, "<h", lx), (PL + 0x11A, "<h", lz), (PL + 0x76, "<h", lyaw)):
            g.wmem(adr, struct.pack(fmt, v))
        L("Leon gestellt (%d,%d) yaw %d (%s, Abstand %d)" % (lx, lz, lyaw, seite, abst))
        L("Z0:", g.cmd("Z0,%x,4" % bp))
        binf = open(os.path.join(out, "frames.bin"), "wb")
        txt = open(os.path.join(out, "frames.txt"), "w")
        n = 0
        while n < nbild:
            g.sk.sendall(b"$c#63")
            r = g.recv(180)
            ent = g.mem(PL, ENT_SZ); lp = struct.unpack_from("<I", ent, 0x198)[0]
            lparts = g.mem(lp, 16 * PART_SZ)
            zent = g.mem(z, ENT_SZ); zp = struct.unpack_from("<I", zent, 0x198)[0]
            zparts = g.mem(zp, 17 * PART_SZ)
            binf.write(b"R2B1" + struct.pack("<II", n, z) + ent + lparts + zent + zparts)
            px, py, pz = struct.unpack_from("<iii", ent, 0x38)
            qx, qy, qz = struct.unpack_from("<iii", zent, 0x38)
            fl = [struct.unpack_from("<I", zparts, k * PART_SZ)[0] for k in range(17)]
            txt.write("%d PL(%d,%d,%d) yaw=%d w4=%08x cw=%08x | Z (%d,%d,%d) yaw=%d w4=%08x cw=%08x 14d=%d | Zfl %s\n" % (
                n, px, py, pz, struct.unpack_from("<h", ent, 0x76)[0], struct.unpack_from("<I", ent, 4)[0],
                struct.unpack_from("<I", ent, 0x14C)[0], qx, qy, qz, struct.unpack_from("<h", zent, 0x76)[0],
                struct.unpack_from("<I", zent, 4)[0], struct.unpack_from("<I", zent, 0x14C)[0], zent[0x14D],
                " ".join("%x" % f for f in fl)))
            txt.flush()
            if n == 0: L("erster P3-Halt:", r)
            n += 1
        binf.close(); txt.close()
        L("Halts:", n)
        g.cmd("z0,%x,4" % bp)
        g.sk.sendall(b"$c#63"); time.sleep(0.2)
        for p in eigene: subprocess.run(["taskkill", "/PID", p], capture_output=True)
        t0 = time.time()
        while time.time() - t0 < 30 and (os.path.getmtime(resume) if os.path.exists(resume) else 0) <= t_resume:
            time.sleep(1)
        time.sleep(2)
        if os.path.exists(resume) and os.path.getmtime(resume) > t_resume:
            shutil.copy2(resume, os.path.join(out, "halt_resume.sav")); L("Bild-Spielstand kopiert")
        return 0
    finally:
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene): time.sleep(1)
        for p in pids() & eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(ini_bak, INI)
        if os.path.exists(res_bak): shutil.copy2(res_bak, resume)
        L("settings.ini + Resume-Spielstand zurueckgeschrieben")


if __name__ == "__main__":
    sys.exit(main())
