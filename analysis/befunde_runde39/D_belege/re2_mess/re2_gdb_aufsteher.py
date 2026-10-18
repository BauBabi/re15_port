"""re2_gdb_aufsteher.py - Runde 39 Spur D: RE2-Retail-Leon (SLUS-00748, re2leon.cue) in DuckStation ueber GDB messen:
Trifft der Spieler-Schuss den Zombie WAEHREND des Boden-Aufstehers (EXEC[5] P6/P7, Clip 8/9) bei Zielhoehe EBEN bzw. TIEF?

Spielstand SLUS-00748_4.sav (Stage 0 Raum 20, EIN Zombie id 0x10 in 7485 Abstand). Zombie-HP wird auf `hp` gesetzt,
damit er nicht stirbt, sondern unter Dauerfeuer stuerzt (0x501) und wieder aufsteht. Eingabe ueber vgamepad (R1 halten,
Viereck tippen; TIEF = D-Pad unten halten). Haltepunkte: Schuss-Applier FUN_800410CC Eintritt @0x800410CC und Rueckkehr
@0x80043B04 (nach `jal 0x800410CC` @0x80043AFC + Delay-Slot), EXEC[5] P6 @0x80103568 und P7 @0x80103628 (je Bild).
Je Stopp: pc, Zombie w0 (Maske = bits 26-28), w4 (Zustand), +0x14C..F (Clip/Bild/Rate), HP +0x156, +0x223, +0x21A, +0x1D3,
Leon +0x154 (Zielhoehe), +0x4. Ausgabe <out>/stops.txt, log.txt. settings.ini: EnableGDBServer an, am Ende zurueck.

Aufruf: python re2_gdb_aufsteher.py <spielstand.sav> <ausgabeordner> <eben|tief> [hp=1000] [max_stops=900] [max_s=240]
Muster: analysis/befunde_runde35/H_raeume/re2_mess/re2_gdb_grab.py (Runde 35 Spur H)."""
import os, shutil, socket, struct, subprocess, sys, time, threading

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue"
INI = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\settings.ini")
PORT = 2345
PL, ETAB = 0x800CFBF8, 0x800CFE18
BP_APPLIER, BP_RET, BP_P6, BP_P7 = 0x800410CC, 0x80043B04, 0x80103568, 0x80103628
BP_ZIEL = 0x80042144          # Zielwahl FUN_80042144 (R1-Druck: `jal` @0x800434e4 / @0x8004332c) - Diagnose, ob R1 ankommt
HOLD_MS = float(os.getenv("R2_HOLD_MS", "120"))   # Viereck-Haltezeit je Schuss (SMG id 15 = Dauerfeuer: laenger halten)


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
    def _read(s, timeout):
        s.sk.settimeout(timeout); d = s.sk.recv(65536)
        if not d: raise EOFError("GDB-Verbindung zu")
        s.buf += d
    def recv(s, timeout=10.0):
        t0 = time.time()
        while True:
            i = s.buf.find(b"$")
            if i >= 0:
                j = s.buf.find(b"#", i)
                if j >= 0 and len(s.buf) >= j + 3:
                    data = s.buf[i + 1:j]; s.buf = s.buf[j + 3:]; s.sk.sendall(b"+"); return data
            left = timeout - (time.time() - t0)
            if left <= 0: raise TimeoutError("keine GDB-Antwort")
            try: s._read(min(left, 1.0))
            except socket.timeout: pass
    def cmd(s, data, timeout=10.0):
        if isinstance(data, str): data = data.encode()
        s.sk.sendall(b"$" + data + b"#%02x" % (sum(data) & 0xFF))
        r = s.recv(timeout)
        # nachlaufende Stopp-Pakete (S02/T05 nach ^C) gehoeren nicht zur Antwort auf m/M/g/Z/z
        while data[:1] not in (b"?", b"c") and (r.startswith(b"S") or r.startswith(b"T")) and len(r) <= 3:
            r = s.recv(timeout)
        return r
    def mem(s, addr, n):
        out = b""
        while n > 0:
            k = min(n, 0x400); r = s.cmd("m%x,%x" % (addr, k))
            if r.startswith(b"E") or len(r) != 2 * k: raise IOError("m %08x,%x -> %r" % (addr, k, r[:20]))
            out += bytes.fromhex(r.decode()); addr += k; n -= k
        return out
    def wmem(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK": raise IOError("M %08x -> %r" % (addr, r))
    def u32(s, a): return struct.unpack("<I", s.mem(a, 4))[0]
    def s32(s, a): return struct.unpack("<i", s.mem(a, 4))[0]
    def s16(s, a): return struct.unpack("<h", s.mem(a, 2))[0]
    def u16(s, a): return struct.unpack("<H", s.mem(a, 2))[0]
    def pc(s):
        r = s.cmd("g")
        if len(r) < 38 * 8: return -1
        return struct.unpack("<I", bytes.fromhex(r[37 * 8:38 * 8].decode()))[0]


def ini_set(path, section, key, value):
    lines = open(path, encoding="utf-8").read().splitlines(); sec, done = None, False
    for i, l in enumerate(lines):
        if l.startswith("["): sec = l.strip()
        elif sec == "[%s]" % section and l.split("=")[0].strip() == key: lines[i] = "%s = %s" % (key, value); done = True
    if not done: raise KeyError(key)
    open(path, "w", encoding="utf-8").write("\n".join(lines) + "\n")


def pids():
    out = subprocess.run(["tasklist", "/NH", "/FI", "IMAGENAME eq duckstation-qt-x64-ReleaseLTCG.exe"],
                         capture_output=True, text=True, encoding="cp850", errors="replace").stdout
    return {l.split()[1] for l in out.splitlines() if l.split() and l.split()[0].lower().startswith("duck")}


class Pad(threading.Thread):
    """R1 dauerhaft halten, Viereck im Takt tippen (an 120 ms / aus 380 ms), TIEF = D-Pad unten halten."""
    def __init__(s, tief):
        super().__init__(daemon=True)
        import vgamepad as vg
        s.vg, s.B = vg, vg.XUSB_BUTTON; s.pad = vg.VX360Gamepad(); s.tief = tief; s.on = False; s.stop = False; s.shots = 0
    def run(s):
        while not s.stop:
            if s.on:
                s.pad.press_button(s.B.XUSB_GAMEPAD_RIGHT_SHOULDER)
                if s.tief: s.pad.press_button(s.B.XUSB_GAMEPAD_DPAD_DOWN)
                s.pad.press_button(s.B.XUSB_GAMEPAD_X); s.pad.update(); time.sleep(HOLD_MS / 1000.0)
                s.pad.release_button(s.B.XUSB_GAMEPAD_X); s.pad.update(); s.shots += 1; time.sleep(0.38)
            else:
                time.sleep(0.05)
    def frei(s):
        s.pad.reset(); s.pad.update()


def main():
    state, out, art = os.path.normpath(sys.argv[1]), os.path.abspath(sys.argv[2]), sys.argv[3]
    hp = int(sys.argv[4]) if len(sys.argv) > 4 else 1000
    max_stops = int(sys.argv[5]) if len(sys.argv) > 5 else 900
    max_s = int(sys.argv[6]) if len(sys.argv) > 6 else 240
    tief = (art == "tief")
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    if pids(): L("DuckStation laeuft bereits - Abbruch"); return 2
    bak = os.path.join(out, "settings.ini.bak"); shutil.copy2(INI, bak)
    pad = Pad(tief); pad.start(); time.sleep(1.5)          # Pad vor DuckStation anmelden (SDL-Enumeration)
    eigene = set()
    try:
        if "EnableGDBServer = true" not in open(INI, encoding="utf-8").read():
            ini_set(INI, "Debug", "EnableGDBServer", "true")
        vor = pids()
        proc = subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE], cwd=out)
        for _ in range(15):
            time.sleep(1); eigene = pids() - vor
            if eigene: break
        L("DuckStation gestartet", eigene)
        g = Gdb(PORT); L("GDB verbunden"); time.sleep(6)
        g.sk.sendall(b"\x03"); time.sleep(0.5); L("Stopp:", g.cmd("?"))
        st, rm = g.s16(0x800D481C), g.s16(0x800D481E)
        L("stage", st, "room", rm, "PL", g.s32(PL + 0x38), g.s32(PL + 0x40), "yaw", g.s16(PL + 0x76), "aim %04x" % g.u16(PL + 0x154))
        # den Zombie finden (id 0x10) und sein HP setzen
        end = g.u32(0x800CE334); z = None
        a = ETAB
        while a < end and a < ETAB + 4 * 64:
            p = g.u32(a); a += 4
            if 0x80000000 <= p < 0x80200000 and p != 0x800D424C and g.mem(p + 8, 1)[0] in (0x10, 0x11, 0x12, 0x13, 0x16, 0x18):
                z = p; break
        if z is None: L("kein Zombie - Abbruch"); return 3
        g.wmem(z + 0x156, struct.pack("<h", hp))
        L("Zombie @%08x id %02x w0=%08x w4=%08x pos (%d,%d,%d) hp=%d 223=%d" % (z, g.mem(z + 8, 1)[0], g.u32(z), g.u32(z + 4),
          g.s32(z + 0x38), g.s32(z + 0x3C), g.s32(z + 0x40), g.s16(z + 0x156), struct.unpack("<b", g.mem(z + 0x223, 1))[0]))
        for bp in (BP_APPLIER, BP_RET, BP_P6, BP_P7, BP_ZIEL):
            L("Z0 %08x:" % bp, g.cmd("Z0,%x,4" % bp))
        txt = open(os.path.join(out, "stops.txt"), "w")
        pad.on = True
        n = 0; t0 = time.time(); hp_prev = hp
        while n < max_stops and time.time() - t0 < max_s:
            g.sk.sendall(b"$c#63")
            try:
                r = g.recv(1.5)
            except TimeoutError:
                g.sk.sendall(b""); time.sleep(0.3)
                try: g.recv(2)                                          # Stopp-Paket des ^C (falls eines kommt)
                except TimeoutError: pass
                g.cmd("?")                                              # Herzschlag: anhalten, Leon lesen, weiter
                L("  herz t=%6.2f L w4=%08x aim=%04x 14c=%08x 10E=%04x 16B=%d yaw=%d pos=(%d,%d) | Z w4=%08x hp=%d pos=(%d,%d) pad-schuesse=%d" % (
                    time.time() - t0, g.u32(PL + 4), g.u16(PL + 0x154), g.u32(PL + 0x14C), g.u16(PL + 0x10E), g.mem(PL + 0x16B, 1)[0],
                    g.s16(PL + 0x76), g.s32(PL + 0x38), g.s32(PL + 0x40), g.u32(z + 4), g.s16(z + 0x156), g.s32(z + 0x38), g.s32(z + 0x40), pad.shots))
                continue
            pc = g.pc()
            e = g.mem(z, 0x230)
            w0, w4 = struct.unpack_from("<II", e, 0)
            c14c = struct.unpack_from("<I", e, 0x14C)[0]
            hpz = struct.unpack_from("<h", e, 0x156)[0]
            r223 = struct.unpack_from("<b", e, 0x223)[0]
            f21a = struct.unpack_from("<H", e, 0x21A)[0]
            d1d3 = e[0x1D3]; f10e = struct.unpack_from("<H", e, 0x10E)[0]
            zx, zy, zz = struct.unpack_from("<iii", e, 0x38)
            aim = g.u16(PL + 0x154); pw4 = g.u32(PL + 4); p14c = g.u32(PL + 0x14C)
            px, pz = g.s32(PL + 0x38), g.s32(PL + 0x40)
            tag = {BP_APPLIER: "SCHUSS", BP_RET: "nachher", BP_P6: "P6", BP_P7: "P7", BP_ZIEL: "ZIEL"}.get(pc, "?%08x" % pc)
            dist = int(((zx - px) ** 2 + (zz - pz) ** 2) ** 0.5)
            treffer = "  <<< TREFFER %d" % (hp_prev - hpz) if hpz < hp_prev else ""
            line = "%4d %-7s t=%6.2f Z w0=%08x maske=%u w4=%08x clip=%02x bild=%3u rate=%02x hp=%4d d223=%3d 21A=%04x 1D3=%02x 10E=%04x y=%d dist=%d | L aim=%04x w4=%08x 14c=%08x%s" % (
                n, tag, time.time() - t0, w0, (w0 >> 26) & 7, w4, c14c & 0xff, (c14c >> 8) & 0xff, (c14c >> 16) & 0xff,
                hpz, r223, f21a, d1d3, f10e, zy, dist, aim, pw4, p14c, treffer)
            txt.write(line + "\n"); txt.flush()
            if tag in ("SCHUSS", "nachher", "ZIEL") or hpz < hp_prev or n % 25 == 0: L(line)
            hp_prev = hpz
            n += 1
            if hpz < 0: L("Zombie tot"); break
        pad.on = False; pad.frei()
        for bp in (BP_APPLIER, BP_RET, BP_P6, BP_P7, BP_ZIEL): g.cmd("z0,%x,4" % bp)
        g.sk.sendall(b"$c#63"); time.sleep(0.2)
        L("Stopps:", n, "Pad-Schuesse:", pad.shots)
        for p in eigene: subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        pad.stop = True; pad.frei()
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene): time.sleep(1)
        for p in pids() & eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI); L("settings.ini zurueckgeschrieben (EnableGDBServer wie vorher)")


if __name__ == "__main__":
    sys.exit(main())
