"""orig_20a0_rolladen.py - Runde 38 Spur W: der Rolladen (obj 1) in ROOM20A0 am ORIGINAL (DuckStation, MZD-Disc =
Auslieferungs-EXE, Pruefung @0x80026e4c == 0800e003) mit dem virtuellen Pad (vgamepad) und dem GDB-Server
(127.0.0.1:2345) messen — Gegenstueck zum Port-Lauf v2_leiter (Cut 11, Rolladen offen).

Ablauf:
  1. Debug-Menue-Spielstand stage_saves/mzd_debugmenu.sav laden; Menue-Eingabe mit Hoch/Hoch/Runter aktivieren;
     per RAM die JUMP-Auswahl setzen (Zeile 0x800BBE5D = 1, Stage 0x800BBE5E = 1, Raumindex 0x800BBE5F+1 = 0x0A,
     Adressen debug_menu_common.c @0x80014444ff) und mit Quadrat laden (JUMP 20A).
  2. Gegner aus (Aktiv-Bit der Entities 0x800ACC2C + 500*k, g_active_count 0x800ACA4E = 0, wie orig_2060b_push.py).
  3. Spieler per RAM an den Schalter (15348,-7200,-19456) Blick 1024 Band 4 (Port-Lage im Lauf v2_leiter), Quadrat
     dreimal (Frage oeffnen / Text / Ja) -> sub05 -> sub07 @0x01E40 faehrt den Rolladen hoch; Pool 1 (0x800B3F98 +
     0x94) y wird gelesen, bis -10800.
  4. Spieler per RAM in die Zone "12 -> 11" (12500,-7200,-15700) -> Cut 11 (DAT_800B0FE4).
  5. Haltepunkte im Flach-Viereck-Zeichner FUN_80023098: @0x80023098 (Eintritt, a0 = Modellkopf, Anzahl +20),
     @0x80023190 (bgez nach NCLIP: v0 = OPZ, a1 = Prim; SXY0..2 = Prim+8/+20/+32), @0x80023228 (beq nach srl 6:
     v1 = OTZ, SXY3 = Prim+44), @0x8002325c (sw = in die OT eingehaengt). Ein voller Aufruf mit 48 Vierecken.
  6. Haltepunkte weg, 3 s laufen lassen, DuckStation geordnet schliessen (SaveStateOnExit) -> resume.sav kopieren
     (Bild aus der VRAM: re15_ss.py).
Aufruf: python orig_20a0_rolladen.py <ausgabeordner>
settings.ini wird gesichert und am Ende zurueckgeschrieben (EnableGDBServer = false)."""
import os, shutil, socket, struct, subprocess, sys, time

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
STATE = r"C:\workspace\git\reAi_v2\stage_saves\mzd_debugmenu.sav"
INI = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\settings.ini")
SAVES = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates")
RESUME = os.path.join(SAVES, "HASH-957757946319438E_resume.sav")
PORT = 2345
PL, POOL, OBJ_SZ = 0x800ACA54, 0x800B3F98, 0x94
ENT0, ENT_SZ, ACTIVE_CNT = 0x800ACC2C, 500, 0x800ACA4E
DBG_ROW, DBG_STAGE, DBG_ROOM0 = 0x800BBE5D, 0x800BBE5E, 0x800BBE5F
CUT_SHOWN, ROOM_IDX = 0x800B0FE4, 0x800B0FE2
BP_ENTRY, BP_NCLIP, BP_OTZ, BP_LINK = 0x80023098, 0x80023190, 0x80023228, 0x8002325C


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
        while True:
            r = s.recv(timeout)
            if data != b"?" and len(r) >= 3 and r[:1] in (b"S", b"T") and r[1:3].isalnum() and data[:1] != b"g":
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

    def memw(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK":
            raise IOError("M %08x -> %r" % (addr, r))

    def stop(s):
        s.sk.sendall(b"\x03")
        try:
            s.recv(2.0)
        except TimeoutError:
            pass
        return s.cmd("?")

    def cont(s):
        s.sk.sendall(b"$c#63")

    def wait_stop(s, timeout):
        try:
            return s.recv(timeout)
        except TimeoutError:
            return None

    def regs(s):
        raw = bytes.fromhex(s.cmd("g").decode())
        n = len(raw) // 4
        return list(struct.unpack("<%dI" % n, raw[:n * 4]))


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
    """[Pad1]-Bindungen SDL-0/<x> zusaetzlich an SDL-1..SDL-(n-1) (Runde 36 R: der vgamepad wurde SDL-1)."""
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
        for p in [p for p in parts if p.startswith("SDL-0/")]:
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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def sxy(word):
    return (s16(word), s16(word >> 16))


def main():
    out = os.path.abspath(sys.argv[1])
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

    def tap(btn, hold=0.12, gap=0.5):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)

    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    eigene = set()
    resume_mtime0 = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        ini_set(INI, "Main", "SaveStateOnExit", "true")
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
        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        dm = g.mem(DBG_ROW, 8)
        L("Debug-Menue vorher: Zeile %d Stage %d Raumidx %s" % (dm[0], dm[1], list(dm[2:8])))
        # Menue-Eingabe aktivieren (nach -statefile wird ein blankes Rechts ignoriert, re15_quickload.py)
        g.cont()
        tap(B.XUSB_GAMEPAD_DPAD_UP, 0.25, 0.8)       # erster Druck kommt verzoegert an (R36: 0,5-1 s)
        tap(B.XUSB_GAMEPAD_DPAD_UP, 0.06, 0.5)
        tap(B.XUSB_GAMEPAD_DPAD_DOWN, 0.06, 0.6)
        g.stop()
        dm = g.mem(DBG_ROW, 8)
        L("Debug-Menue nach Hoch/Hoch/Runter: Zeile %d Stage %d Raumidx %s" % (dm[0], dm[1], list(dm[2:8])))
        g.memw(DBG_ROW, bytes([1]))
        g.memw(DBG_STAGE, bytes([1]))
        g.memw(DBG_ROOM0 + 1, bytes([0x0A]))
        dm = g.mem(DBG_ROW, 8)
        L("Debug-Menue gesetzt: Zeile %d Stage %d Raumidx %s (JUMP 20A)" % (dm[0], dm[1], list(dm[2:8])))
        g.cont()
        time.sleep(0.5)
        tap(B.XUSB_GAMEPAD_X, 0.1, 0.3)
        L("Quadrat (Laden) gedrueckt, warte 25 s")
        time.sleep(25)
        g.stop()

        def pl():
            e = g.mem(PL, 0x90)
            return (struct.unpack_from("<i", e, 0x34)[0], struct.unpack_from("<i", e, 0x38)[0],
                    struct.unpack_from("<i", e, 0x3C)[0], struct.unpack_from("<h", e, 0x6A)[0], e[0x82])

        def obj(k):
            o = g.mem(POOL + OBJ_SZ * k, OBJ_SZ)
            return (struct.unpack_from("<I", o, 0)[0], struct.unpack_from("<i", o, 0x34)[0],
                    struct.unpack_from("<i", o, 0x38)[0], struct.unpack_from("<i", o, 0x3C)[0],
                    struct.unpack_from("<h", o, 0x6A)[0], o[0x08], struct.unpack_from("<I", o, 0x0C)[0])

        def cut():
            return struct.unpack("<H", g.mem(CUT_SHOWN, 2))[0]

        L("Raumindex @0x800b0fe2:", struct.unpack("<H", g.mem(ROOM_IDX, 2))[0], "Cut:", cut(), "Spieler:", pl())
        for k in range(8):
            L("  Pool %d: flags/pos/ry/typ/attr" % k, obj(k))
        o1 = obj(1)
        if (o1[1], o1[3]) != (7942, -20510):
            L("Pool 1 ist nicht der Rolladen (7942,?,-20510) - ROOM20A0 nicht geladen? Abbruch")
            return 3
        # Gegner aus (orig_2060b_push.py): Aktiv-Bit (w0 & 1) loeschen, g_active_count = 0
        for k in range(16):
            a = ENT0 + ENT_SZ * k
            w0 = struct.unpack("<I", g.mem(a, 4))[0]
            typ = g.mem(a + 0x04, 1)[0]
            if w0 & 1:
                g.memw(a, struct.pack("<I", w0 & ~1))
                L("Entity %d @%08x: w0 %04x typ 0x%02x -> Aktiv-Bit geloescht" % (k, a, w0, typ))
        g.memw(ACTIVE_CNT, struct.pack("<H", 0))

        def setze_spieler(x, y, z, rot, band):
            g.memw(PL + 0x34, struct.pack("<iii", x, y, z))
            g.memw(PL + 0x6A, struct.pack("<h", rot))
            g.memw(PL + 0x82, bytes([band]))

        # Leiter wie im Port-Lauf v2_leiter: Door_aot_set slot 1 @0x01766 (Band 2) -> ROOM20A0 Cut 12 (16500,-7200,-20000)
        setze_spieler(16500, -3600, -21050, 3072, 2)
        g.cont(); time.sleep(1.0)
        tap(B.XUSB_GAMEPAD_X, 0.25, 5.0)
        g.stop()
        L("nach der Leiter: Spieler", pl(), "Cut", cut())
        if cut() != 12:
            g.cont(); tap(B.XUSB_GAMEPAD_X, 0.25, 5.0); g.stop()
            L("nach der Leiter (2. Versuch): Spieler", pl(), "Cut", cut())
        setze_spieler(15348, -7200, -19456, 1024, 4)
        g.cont(); time.sleep(2.0); g.stop()
        L("am Schalter: Spieler", pl(), "Cut", cut(), "Pool1", obj(1))
        g.cont()
        tap(B.XUSB_GAMEPAD_X, 0.25, 1.5)
        for n in range(6):
            g.stop()
            o1 = obj(1)
            L("  nach Quadrat %d: Pool1 y=%d Spieler %s Cut %d" % (n + 1, o1[2], pl(), cut()))
            if o1[2] != -7200:
                break
            g.cont()
            tap(B.XUSB_GAMEPAD_X, 0.12, 1.5)
        g.cont()
        for _ in range(12):
            time.sleep(1.0)
            g.stop()
            o1 = obj(1)
            g.cont()
            if o1[2] == -10800:
                break
        g.stop()
        L("Rolladen:", obj(1), "Spieler", pl(), "Cut", cut())
        setze_spieler(12500, -7200, -15700, 3072, 4)
        g.cont(); time.sleep(2.5); g.stop()
        L("in der Zone 12->11: Spieler", pl(), "Cut", cut(), "Pool1", obj(1))

        # Haltepunkte im Zeichner
        for a in (BP_ENTRY, BP_NCLIP, BP_OTZ, BP_LINK):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
        quads, aufzeichnen, fertig, n_eintritt = [], False, False, 0
        t0 = time.time()
        g.cont()
        while not fertig and time.time() - t0 < 60:
            r = g.wait_stop(5.0)
            if r is None:
                L("kein Haltepunkt in 5 s")
                g.stop()
                break
            rg = g.regs()
            pc = rg[37] if len(rg) >= 38 else 0
            if pc == BP_ENTRY:
                a0 = rg[4]
                hdr = g.mem(a0, 24)
                anzahl = struct.unpack_from("<I", hdr, 20)[0]
                n_eintritt += 1
                if aufzeichnen and quads:
                    fertig = True
                elif anzahl == 48:
                    aufzeichnen = True
                    L("FUN_80023098 Eintritt a0=%08x Anzahl %d (Rolladen)" % (a0, anzahl))
                else:
                    L("FUN_80023098 Eintritt a0=%08x Anzahl %d (anderes Objekt)" % (a0, anzahl))
            elif pc == BP_NCLIP and aufzeichnen:
                v0, a1 = rg[2], rg[5]
                prim = g.mem(a1, 48)
                s = [sxy(struct.unpack_from("<I", prim, o)[0]) for o in (8, 20, 32)]
                quads.append({"opz": struct.unpack("<i", struct.pack("<I", v0))[0], "sxy": s, "prim": a1})
            elif pc == BP_OTZ and aufzeichnen and quads:
                v1, a1 = rg[3], rg[5]
                quads[-1]["otz"] = v1
                quads[-1]["sxy3"] = sxy(struct.unpack("<I", g.mem(a1 + 44, 4))[0])
            elif pc == BP_LINK and aufzeichnen and quads:
                quads[-1]["eingehaengt"] = 1
            if not fertig:
                g.cont()
        for a in (BP_ENTRY, BP_NCLIP, BP_OTZ, BP_LINK):
            g.cmd("z0,%x,4" % a)
        L("Vierecke aufgezeichnet:", len(quads))
        gez = 0
        for i, q in enumerate(quads):
            pts = q["sxy"] + ([q["sxy3"]] if "sxy3" in q else [])
            gpu = ""
            if len(pts) == 4:
                for t, tri in enumerate(((0, 1, 2), (1, 2, 3))):
                    xs = [pts[k][0] for k in tri]; ys = [pts[k][1] for k in tri]
                    gross = (max(xs) - min(xs)) >= 1024 or (max(ys) - min(ys)) >= 512
                    auf = not (max(xs) < 0 or min(xs) > 319 or max(ys) < 0 or min(ys) > 239)
                    gpu += " T%d:%s%s" % (t, "ZU-GROSS" if gross else "ok", "" if auf else "/aus")
                    if q.get("eingehaengt") and not gross and auf:
                        gez += 1
            L("Q%02d opz=%d otz=%s sxy=%s eingehaengt=%d%s" % (i, q["opz"], q.get("otz", "-"), pts,
                                                             q.get("eingehaengt", 0), gpu))
        L("Dreiecke eingehaengt, GPU-gross ok und auf dem Schirm:", gez)
        g.cont()
        time.sleep(3.0)
        g.stop()
        L("Ende: Spieler", pl(), "Cut", cut(), "Pool1", obj(1))
        g.cont()
        time.sleep(0.5)
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        try:
            gp.reset(); gp.update()
        except Exception:
            pass
        t0 = time.time()
        while time.time() - t0 < 25 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        ok = "EnableGDBServer = false" in open(INI, encoding="utf-8").read()
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" % ok)
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mtime0:
            shutil.copy2(RESUME, os.path.join(out, "orig_20a0_cut11_offen.sav"))
            L("resume.sav kopiert")
        else:
            L("resume.sav NICHT neu geschrieben")


if __name__ == "__main__":
    sys.exit(main())
