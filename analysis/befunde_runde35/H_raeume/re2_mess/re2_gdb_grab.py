"""re2_gdb_grab.py — RE2-Retail-Leon (SLUS-00748, re2leon.cue; EXE = info/re2leon/PSX.EXE, Code-Abgleich
re2_ss.py) in DuckStation ueber dessen GDB-Server messen: Spielstand ROOM2050 laden, Leon neben die Gitterarme
stellen (RAM-Schreiben wie FUN_80026b7c @0x80026c80ff), Griff abwarten und an JEDEM Halte-Bild (Haltepunkt auf
B4 P1 des Arms, EM2D-Overlay @0x80100CB4 — laeuft genau einmal je Bild, solange der Arm haelt) die RAM lesen:
PL-Entity + Leons Parts + haltender Arm + dessen Parts. Danach Haltepunkt weg, weiterlaufen, DuckStation
geordnet schliessen (SaveStateOnExit -> Bild der Halte-Phase aus der VRAM).

Aufruf: python re2_gdb_grab.py <spielstand.sav> <ausgabeordner> <x> <z> <yaw> [halte_bilder] [schliessen_nach]
Runde 35 Spur H, Nachbesserung 3. settings.ini wird gesichert und am Ende zurueckgeschrieben."""
import os, shutil, socket, struct, subprocess, sys, time

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue"
INI = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\settings.ini")
SAVES = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates")
PORT = 2345
PL, HOLDER, ETAB = 0x800CFBF8, 0x800CFDAC, 0x800CFE18
BP_HALTEN = 0x80100CB4          # EM2D B4 P1 (Halten), Tabelle @0x8010146C[4] -> 0x80100B68 -> P1
PART_N, PART_SZ, ENT_SZ = 16, 0xAC, 0x248


class Gdb:
    def __init__(s, port):
        t0 = time.time()
        while True:
            try:
                s.sk = socket.create_connection(("127.0.0.1", port), timeout=2)
                break
            except OSError:
                if time.time() - t0 > 90:
                    raise
                time.sleep(1)
        s.buf = b""

    def _read(s, timeout):
        s.sk.settimeout(timeout)
        d = s.sk.recv(65536)
        if not d:
            raise EOFError("GDB-Verbindung zu")
        s.buf += d

    def recv(s, timeout=10.0):
        t0 = time.time()
        while True:
            i = s.buf.find(b"$")
            if i >= 0:
                j = s.buf.find(b"#", i)
                if j >= 0 and len(s.buf) >= j + 3:
                    data = s.buf[i + 1:j]
                    s.buf = s.buf[j + 3:]
                    s.sk.sendall(b"+")
                    return data
            left = timeout - (time.time() - t0)
            if left <= 0:
                raise TimeoutError("keine GDB-Antwort")
            try:
                s._read(min(left, 1.0))
            except socket.timeout:
                pass

    def cmd(s, data, timeout=10.0):
        if isinstance(data, str):
            data = data.encode()
        s.sk.sendall(b"$" + data + b"#%02x" % (sum(data) & 0xFF))
        return s.recv(timeout)

    def mem(s, addr, n):
        out = b""
        while n > 0:
            k = min(n, 0x400)
            r = s.cmd("m%x,%x" % (addr, k))
            if r.startswith(b"E") or len(r) != 2 * k:
                raise IOError("m %08x,%x -> %r" % (addr, k, r[:20]))
            out += bytes.fromhex(r.decode())
            addr += k; n -= k
        return out

    def wmem(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK":
            raise IOError("M %08x -> %r" % (addr, r))

    def u32(s, a): return struct.unpack("<I", s.mem(a, 4))[0]
    def s32(s, a): return struct.unpack("<i", s.mem(a, 4))[0]
    def s16(s, a): return struct.unpack("<h", s.mem(a, 2))[0]


def ini_set(path, section, key, value):
    lines = open(path, encoding="utf-8").read().splitlines()
    sec, done = None, False
    for i, l in enumerate(lines):
        if l.startswith("["):
            sec = l.strip()
        elif sec == "[%s]" % section and l.split("=")[0].strip() == key:
            lines[i] = "%s = %s" % (key, value); done = True
    if not done:
        raise KeyError(key)
    open(path, "w", encoding="utf-8").write("\n".join(lines) + "\n")


def pids():
    out = subprocess.run(["tasklist", "/NH", "/FI", "IMAGENAME eq duckstation-qt-x64-ReleaseLTCG.exe"],
                         capture_output=True, text=True, encoding="cp850", errors="replace").stdout
    return {l.split()[1] for l in out.splitlines() if l.split() and l.split()[0].lower().startswith("duck")}


def main():
    state, out = os.path.normpath(sys.argv[1]), os.path.abspath(sys.argv[2])
    lx, lz, lyaw = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
    nhold = int(sys.argv[6]) if len(sys.argv) > 6 else 150
    close_after = int(sys.argv[7]) if len(sys.argv) > 7 else 40
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    if pids():
        L("DuckStation laeuft bereits — Abbruch"); return 2
    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    resume = os.path.join(SAVES, "SLUS-00748_resume.sav")
    t_resume = os.path.getmtime(resume) if os.path.exists(resume) else 0
    eigene = set()
    try:
        if "EnableGDBServer = true" not in open(INI, encoding="utf-8").read():
            ini_set(INI, "Debug", "EnableGDBServer", "true")
        vor = pids()
        proc = subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE], cwd=out)
        for _ in range(15):
            time.sleep(1)
            eigene = pids() - vor
            if eigene: break
        L("DuckStation gestartet", eigene)
        g = Gdb(PORT)
        L("GDB verbunden")
        time.sleep(6)                       # Spielstand laden lassen
        g.sk.sendall(b"\x03")
        time.sleep(0.5)                     # DuckStation antwortet auf ^C nicht; '?' liefert den Stopp
        L("Stopp:", g.cmd("?"))
        st, rm = g.s16(0x800D481C), g.s16(0x800D481E)
        L("stage", st, "room", rm, "PL", g.s32(PL + 0x38), g.s32(PL + 0x40), "yaw", g.s16(PL + 0x76))
        if (st, rm) != (1, 5):
            L("nicht ROOM2050 — Abbruch"); return 3
        # Leon stellen: genau die Felder, die der Tuerwechsel FUN_80026b7c setzt (+0x38/+0x40 s32,
        # +0x44/+0x48 s16, +0x118/+0x11A s16, +0x76 Blick).
        g.wmem(PL + 0x38, struct.pack("<i", lx)); g.wmem(PL + 0x40, struct.pack("<i", lz))
        g.wmem(PL + 0x44, struct.pack("<h", lx)); g.wmem(PL + 0x48, struct.pack("<h", lz))
        g.wmem(PL + 0x118, struct.pack("<h", lx)); g.wmem(PL + 0x11A, struct.pack("<h", lz))
        g.wmem(PL + 0x76, struct.pack("<h", lyaw))
        L("Leon gestellt", g.s32(PL + 0x38), g.s32(PL + 0x40), "yaw", g.s16(PL + 0x76))
        only = os.getenv("R2_ONLY")          # Satz-Index des EINEN Arms, der greifen darf
        if only is not None:
            # Alle anderen Arme in Sub 7 = ENDE (A7/B7 `jr ra` @0x80100F40/48, kein Setzer fuehrt heraus,
            # Dossier arme-1210-re2.md 2.4) — der Ziel-Arm laeuft unveraendert im Original-Code.
            for k in range(1, 33):
                p = g.u32(ETAB + 4 * k)
                if 0x80000000 <= p < 0x80200000 and p != 0x800D424C and g.mem(p + 8, 1)[0] == 0x2D:
                    rec = g.mem(p + 0xC, 1)[0] - 2  # +0xC = Satz-Index + 2 (gemessen: Saetze 0..9 -> 2..11)
                    if rec != int(only):
                        g.wmem(p + 4, struct.pack("<I", 0x00000701))
                    L("Arm k%d Satz %d y=%d -> %s" % (k, rec, g.s32(p + 0x3C), "Ziel" if rec == int(only) else "ENDE"))
        L("Z0:", g.cmd("Z0,%x,4" % BP_HALTEN))
        binf = open(os.path.join(out, "frames.bin"), "wb")
        txt = open(os.path.join(out, "frames.txt"), "w")
        n = 0
        while n < nhold:
            g.sk.sendall(b"$c#63")
            r = g.recv(120)
            h = g.u32(HOLDER)
            ent = g.mem(PL, ENT_SZ)
            lp = struct.unpack_from("<I", ent, 0x198)[0]
            lparts = g.mem(lp, PART_N * PART_SZ)
            hent = g.mem(h, ENT_SZ)
            hp = struct.unpack_from("<I", hent, 0x198)[0]
            hparts = g.mem(hp, PART_N * PART_SZ)
            binf.write(b"R2G1" + struct.pack("<II", n, h) + ent + lparts + hent + hparts)
            px, py, pz = struct.unpack_from("<iii", ent, 0x38)
            hx, hy, hz = struct.unpack_from("<iii", hent, 0x38)
            txt.write("%d stop=%s PL(%d,%d,%d) yaw=%d w4=%08x cw=%08x | H %08x (%d,%d,%d) yaw=%d w4=%08x cw=%08x f10e=%04x\n" % (
                n, r.decode(errors="replace"), px, py, pz, struct.unpack_from("<h", ent, 0x76)[0],
                struct.unpack_from("<I", ent, 4)[0], struct.unpack_from("<I", ent, 0x14C)[0], h, hx, hy, hz,
                struct.unpack_from("<h", hent, 0x76)[0], struct.unpack_from("<I", hent, 4)[0],
                struct.unpack_from("<I", hent, 0x14C)[0], struct.unpack_from("<H", hent, 0x10E)[0]))
            txt.flush()
            if n == 0:
                L("erster Halte-Stopp:", r, "Halter %08x" % h)
            n += 1
            if n == close_after:
                break
        binf.close(); txt.close()
        L("Halte-Stopps:", n)
        g.cmd("z0,%x,4" % BP_HALTEN)
        g.sk.sendall(b"$c#63")
        time.sleep(0.2)
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)   # geordnet -> SaveStateOnExit
        t0 = time.time()
        while time.time() - t0 < 30 and (os.path.getmtime(resume) if os.path.exists(resume) else 0) <= t_resume:
            time.sleep(1)
        time.sleep(2)
        if os.path.exists(resume) and os.path.getmtime(resume) > t_resume:
            shutil.copy2(resume, os.path.join(out, "halten_resume.sav")); L("Bild-Spielstand kopiert")
        return 0
    finally:
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        if os.getenv("R2_INI_RESTORE", "0") == "1":
            shutil.copy2(bak, INI); L("settings.ini zurueckgeschrieben")


if __name__ == "__main__":
    sys.exit(main())
