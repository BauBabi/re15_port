"""orig_1090_push.py - Runde 36 Spur R: die Dach-Kiste obj 1 in ROOM1090 am ORIGINAL (DuckStation, MZD-Disc =
Auslieferungs-EXE, Spielstand stage_saves/room1090_orig.sav: Leon auf dem Dach (-5282,-9000,-1514) Band 5) mit dem
virtuellen Pad (vgamepad, SDL-0) schieben und ueber den GDB-Server (127.0.0.1:2345) je Schritt messen:
  Haltepunkte  @0x8002bfc0 Schub ANGENOMMEN (FUN_8003b558 == 0, sb 8 -> obj+0x8C)
               @0x8002c0b0 Schub GESPERRT   (FUN_8003b558 != 0, sb s7(=10) -> obj+0x8C, Ruecksetzung @0x8002c0b8-d4)
               @0x8002c13c FALL-AUSLOESER   (Objekt-Pass FUN_8003b7f0(.,0x1c2,Band) & 2 -> FUN_8002d3d8)
  RAM          Spieler 0x800aca54 (+0x34/+0x3C x/z s32, +0x6A Blick, +0x82 Band, +0x05 Substate),
               Objekt-Pool 0x800b3f98 + 0x94*k (+0x34/+0x38/+0x3C, +0x04 faellt, +0x82 Band, +0x8C Zaehler).
Navigation geschlossen geregelt: Blick/Lage aus der RAM lesen (^C, '?'), Pad drehen/laufen, weiter ('c').
Aufruf: python orig_1090_push.py <ausgabeordner> [plan]   plan = nord | west | westseite | sued | alles (Default alles)
settings.ini wird gesichert und am Ende zurueckgeschrieben (EnableGDBServer = false)."""
import os, shutil, socket, struct, subprocess, sys, time, math

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
STATE = r"C:\workspace\git\reAi_v2\stage_saves\room1090_orig.sav"
INI = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\settings.ini")
PORT = 2345
PL, POOL, OBJ_SZ = 0x800ACA54, 0x800B3F98, 0x94
BP_OK, BP_BLOCK, BP_FALL, BP_HIT = 0x8002BFC0, 0x8002C0B0, 0x8002C13C, 0x8003DA78


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
        s.stops = []

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
        """Befehl senden; unaufgeforderte Stopp-Antworten (S02 nach ^C, T05 Haltepunkt) werden
        weggelesen und in s.stops gesammelt — DuckStation antwortet auf ^C sehr wohl mit 'S02'."""
        if isinstance(data, str):
            data = data.encode()
        s.sk.sendall(b"$" + data + b"#%02x" % (sum(data) & 0xFF))
        while True:
            r = s.recv(timeout)
            if data not in (b"?",) and len(r) >= 3 and r[:1] in (b"S", b"T") and r[1:3].isalnum() and (data[:1] not in b"g"):
                s.stops.append(r)
                continue
            return r

    def mem(s, addr, n):
        out = b""
        while n > 0:
            k = min(n, 0x400)
            r = s.cmd("m%x,%x" % (addr, k))
            if r.startswith(b"E") or len(r) != 2 * k:
                raise IOError("m %08x,%x -> %r" % (addr, k, r[:20]))
            out += bytes.fromhex(r.decode())
            addr += k
            n -= k
        return out

    def stop(s):
        """Anhalten: ^C -> DuckStation antwortet 'S02' (weglesen), dann '?' bestaetigt den Stopp."""
        s.sk.sendall(b"\x03")
        try:
            r = s.recv(2.0)
            s.stops.append(r)
        except TimeoutError:
            pass
        r = s.cmd("?")
        s.stops.clear()
        return r

    def cont(s):
        s.sk.sendall(b"$c#63")

    def wait_stop(s, timeout):
        """Auf einen Haltepunkt-Stopp warten; None bei Zeitablauf (laeuft dann weiter)."""
        try:
            return s.recv(timeout)
        except TimeoutError:
            return None

    def regs(s):
        raw = bytes.fromhex(s.cmd("g").decode())
        n = len(raw) // 4
        return list(struct.unpack("<%dI" % n, raw[:n * 4]))

    def pc(s):
        r = s.regs()
        # DuckStation-GDB: 32 GPR + sr, lo, hi, bad, cause, pc (Index 37)
        return r[37] if len(r) >= 38 else 0


def ini_set(path, section, key, value):
    lines = open(path, encoding="utf-8").read().splitlines()
    sec, done = None, False
    for i, l in enumerate(lines):
        if l.startswith("["):
            sec = l.strip()
        elif sec == "[%s]" % section and l.split("=")[0].strip() == key:
            lines[i] = "%s = %s" % (key, value)
            done = True
    if not done:
        raise KeyError(key)
    open(path, "w", encoding="utf-8").write("\n".join(lines) + "\n")


def pad_bind_alle(path, n=3):
    """[Pad1]-Bindungen, die SDL-0/<x> nennen, zusaetzlich an SDL-1..SDL-(n-1) binden (gemessen 2026-10-11:
    zwei XInput-Geraete am Bus, der eigene vgamepad-Pad wurde SDL-1 und kam nie im Spiel an)."""
    lines = open(path, encoding="utf-8").read().splitlines()
    sec = None
    for i, l in enumerate(lines):
        if l.startswith("["):
            sec = l.strip()
            continue
        if sec != "[Pad1]" or "SDL-0/" not in l or "=" not in l:
            continue
        key, val = l.split("=", 1)
        parts = [p.strip() for p in val.split("&")]
        sdl = [p for p in parts if p.startswith("SDL-0/")]
        for p in sdl:
            for k in range(1, n):
                q = p.replace("SDL-0/", "SDL-%d/" % k)
                if q not in parts:
                    parts.append(q)
        lines[i] = "%s= %s" % (key, " & ".join(parts))
    open(path, "w", encoding="utf-8").write(chr(10).join(lines) + chr(10))


def pids():
    out = subprocess.run(["tasklist", "/NH", "/FI", "IMAGENAME eq duckstation-qt-x64-ReleaseLTCG.exe"],
                         capture_output=True, text=True, encoding="cp850", errors="replace").stdout
    return {l.split()[1] for l in out.splitlines() if l.split() and l.split()[0].lower().startswith("duck")}


def main():
    out = os.path.abspath(sys.argv[1])
    plan = sys.argv[2] if len(sys.argv) > 2 else "alles"
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")

    def L(*a):
        s = " ".join(str(x) for x in a)
        print(s, flush=True)
        log.write(s + "\n")
        log.flush()

    if pids():
        L("DuckStation laeuft bereits - Abbruch")
        return 2
    import vgamepad as vg
    B = vg.XUSB_BUTTON
    gp = vg.VX360Gamepad()
    time.sleep(2.0)
    held = set()

    def pad(*btns):
        want = set(btns)
        for b in held - want:
            gp.release_button(button=b)
        for b in want - held:
            gp.press_button(button=b)
        held.clear()
        held.update(want)
        gp.update()

    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    eigene = set()
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")   # sonst haengt das geordnete Schliessen im Dialog
        pad_bind_alle(INI)
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", STATE, CUE], cwd=out)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        L("DuckStation gestartet", eigene)
        g = Gdb(PORT)
        L("GDB verbunden")
        time.sleep(8)
        L("Stopp:", g.stop())

        def pl():
            e = g.mem(PL, 0x90)
            return (struct.unpack_from("<i", e, 0x34)[0], struct.unpack_from("<i", e, 0x38)[0],
                    struct.unpack_from("<i", e, 0x3C)[0], struct.unpack_from("<h", e, 0x6A)[0], e[0x82], e[0x05])

        def obj(k):
            o = g.mem(POOL + OBJ_SZ * k, OBJ_SZ)
            return (struct.unpack_from("<i", o, 0x34)[0], struct.unpack_from("<i", o, 0x38)[0],
                    struct.unpack_from("<i", o, 0x3C)[0], o[4], o[5], o[0x82], o[0x8C])

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        L("Spieler:", pl(), "obj0:", obj(0), "obj1:", obj(1))
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
        bx = struct.unpack("<I", g.mem(PL + 0x7C, 4))[0]
        L("Spieler-Box +0x78:", struct.unpack("<6h", g.mem(struct.unpack("<I", g.mem(PL + 0x78, 4))[0], 12)),
          "+0x7C @%08x:" % bx, struct.unpack("<6h", g.mem(bx, 12)) if 0x80000000 <= bx < 0x80200000 else bx)
        o1b = struct.unpack("<I", g.mem(POOL + OBJ_SZ + 0x7C, 4))[0]
        L("obj1 +0x7C @%08x:" % o1b, struct.unpack("<6h", g.mem(o1b, 12)) if 0x80000000 <= o1b < 0x80200000 else o1b)
        ereignisse = []

        def lauf(sekunden, *btns):
            """Pad halten und laufen lassen; Haltepunkte waehrenddessen protokollieren. Ende: angehalten."""
            pad(*btns)
            g.cont()
            t0 = time.time()
            while True:
                left = sekunden - (time.time() - t0)
                if left <= 0:
                    break
                r = g.wait_stop(min(left, 0.5))
                if r is None:
                    continue
                rg = g.regs()
                pc = rg[37] if len(rg) >= 38 else 0
                tag = {BP_OK: "SCHUB-OK", BP_BLOCK: "SCHUB-GESPERRT", BP_FALL: "FALL-AUSLOESER", BP_HIT: "TREFFER-ZELLE"}.get(pc, "stop@%08x" % pc)
                o1 = obj(1)
                p = pl()
                if pc == BP_HIT:
                    a0, a1 = rg[4], rg[5]
                    zelle = g.mem(a0, 12)
                    w, d, x, z, t, u0, u1, fl = struct.unpack("<HHhhBBBB", zelle)
                    L("  TREFFER-ZELLE a0=%08x obj=%08x: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x | obj1=%s" % (
                        a0, a1, x, x + w, z, z + d, t, u0, u1, fl, o1))
                else:
                    s4, s5 = rg[20], rg[21]
                    ereignisse.append((tag, o1, p))
                    L("  %s obj1=%s spieler=%s s4/s5=(%d,%d)" % (tag, o1, p, (s4 & 0xffff) - (0x10000 if s4 & 0x8000 else 0),
                                                                  (s5 & 0xffff) - (0x10000 if s5 & 0x8000 else 0)))
                g.cont()
            r = g.stop()
            pad()
            return r

        def zustand(tag=""):
            p = pl()
            o1 = obj(1)
            L("%s spieler=(%d,%d,%d) blick=%d band=%d sub=%d | obj1=(%d,%d,%d) faellt=%d z=%d band=%d n=%d" % (
                tag, p[0], p[1], p[2], p[3], p[4], p[5], o1[0], o1[1], o1[2], o1[3], o1[4], o1[5], o1[6]))
            return p, o1

        # Drehsinn messen: 0.6 s R (nach einem Stopp braucht die Eingabe einige Bilder, 0.2 s ergab 0).
        r_sign = 0
        for _ in range(3):
            p0 = pl()
            lauf(0.6, B.XUSB_GAMEPAD_DPAD_RIGHT)
            p1 = pl()
            dr = ((p1[3] - p0[3] + 2048) % 4096) - 2048
            L("Drehsinn: R 0.6 s -> dBlick = %d" % dr)
            if abs(dr) > 100:
                r_sign = 1 if dr > 0 else -1
                break
        if r_sign == 0:
            L("Pad kommt nicht an (kein Drehen) - Abbruch")
            return 4

        def drehe_auf(ziel, tol=120):
            """Drehen ~96 Einheiten je Bild (gemessen: R 1 s = +2784); Haltezeit proportional, kurze Tipper
            fuer den Rest. Drehsinn nur bei eindeutig falscher Richtung umkehren."""
            nonlocal r_sign
            for _ in range(30):
                p = pl()
                err = ((ziel - p[3] + 2048) % 4096) - 2048
                if abs(err) <= tol:
                    return p
                btn = B.XUSB_GAMEPAD_DPAD_RIGHT if (err * r_sign) > 0 else B.XUSB_GAMEPAD_DPAD_LEFT
                dauer = min(0.8, max(0.035, (abs(err) - 60) / 2880.0))
                lauf(dauer, btn)
                p2 = pl()
                err2 = ((ziel - p2[3] + 2048) % 4096) - 2048
                if dauer >= 0.2 and abs(err2) > abs(err) + 300:
                    r_sign = -r_sign
                    L("  Drehsinn umgekehrt (err %d -> %d)" % (err, err2))
            return pl()

        def ziel_blick(x, z):
            p = pl()
            dx, dz = x - p[0], z - p[2]
            return int(round(math.atan2(-dz, dx) / (2 * math.pi) * 4096)) % 4096

        def gehe_zu(x, z, tol=250, maxs=40):
            t0 = time.time()
            while time.time() - t0 < maxs:
                p = pl()
                d = math.hypot(x - p[0], z - p[2])
                if d <= tol:
                    return True
                drehe_auf(ziel_blick(x, z))
                lauf(min(1.0, max(0.15, d / 1500.0)), B.XUSB_GAMEPAD_DPAD_UP)
            return False

        def schiebe(blick, sekunden, tag):
            """Blick einstellen, UP halten (Schub), Ereignisse zaehlen."""
            drehe_auf(blick, 100)
            n0 = len(ereignisse)
            zustand("%s VOR" % tag)
            lauf(sekunden, B.XUSB_GAMEPAD_DPAD_UP)
            zustand("%s NACH" % tag)
            ok = sum(1 for e in ereignisse[n0:] if e[0] == "SCHUB-OK")
            bl = sum(1 for e in ereignisse[n0:] if e[0] == "SCHUB-GESPERRT")
            fa = sum(1 for e in ereignisse[n0:] if e[0] == "FALL-AUSLOESER")
            L("%s: Schub angenommen %d x, gesperrt %d x, Fall-Ausloeser %d x" % (tag, ok, bl, fa))

        zustand("START")
        if plan in ("nord", "alles"):
            L("== Plan NORD: von Sueden gegen die Kiste schieben (Bruestung r2[0] z -16403) ==")
            gehe_zu(-5900, -8000)
            gehe_zu(-5900, -13500, 200)
            schiebe(1024, 4.0, "NORD")
            lauf(0.6, B.XUSB_GAMEPAD_DPAD_DOWN)
            schiebe(1024, 3.0, "NORD-2")
        if plan in ("west", "alles"):
            L("== Plan WEST: von Osten (Tasche x -4610..-3850) gegen die Kiste schieben (Wand r3[2] x <= -6878) ==")
            lauf(0.8, B.XUSB_GAMEPAD_DPAD_DOWN)
            gehe_zu(-4300, -13600, 200)
            gehe_zu(-4250, -15300, 150)
            schiebe(2048, 4.0, "WEST")
            lauf(0.6, B.XUSB_GAMEPAD_DPAD_DOWN)
            schiebe(2048, 3.0, "WEST-2")
        if plan in ("westseite", "alles"):
            L("== Plan WESTSEITE: zwischen Wand und Kiste kommen (x -6428, z -15482) und nach Osten schieben? ==")
            lauf(0.8, B.XUSB_GAMEPAD_DPAD_DOWN)
            gehe_zu(-4300, -13500, 200)
            gehe_zu(-6300, -13900, 150)
            drehe_auf(1024, 100)
            zustand("WESTSEITE VOR")
            lauf(3.0, B.XUSB_GAMEPAD_DPAD_UP)
            zustand("WESTSEITE NACH (UP 3 s nach Norden bei x~-6400)")
            schiebe(0, 3.0, "OST-VERSUCH")
        if plan in ("sued", "alles"):
            L("== Plan SUED: Nordseite der Kiste erreichen? (Spieler z < -16832 noetig; Bruestung erlaubt z >= -15953) ==")
            lauf(0.8, B.XUSB_GAMEPAD_DPAD_DOWN)
            gehe_zu(-4250, -14000, 200)
            gehe_zu(-4250, -15900, 120)
            drehe_auf(1024, 100)
            zustand("NORDTASCHE VOR")
            lauf(2.0, B.XUSB_GAMEPAD_DPAD_UP)
            zustand("NORDTASCHE NACH (UP 2 s nach Norden)")
            schiebe(2048 + 512, 3.0, "SUED-DIAGONAL")
        zustand("ENDE")
        L("Ereignisse gesamt:", len(ereignisse))
        for e in ereignisse:
            L("  ", e)
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT):
            g.cmd("z0,%x,4" % a)
        g.cont()
        time.sleep(0.3)
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        pad()
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        ok = "EnableGDBServer = false" in open(INI, encoding="utf-8").read()
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" % ok)


if __name__ == "__main__":
    sys.exit(main())
