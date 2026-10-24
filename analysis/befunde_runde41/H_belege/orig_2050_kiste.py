"""orig_2050_kiste.py - Runde 41 Spur H: Kiste gegen Kiste am ORIGINAL (DuckStation, MZD-Disc = Auslieferungs-EXE,
@0x80026e4c = 0800e003; Spielstand analysis/befunde_runde36/R_belege/room2050_orig.sav: Leon an der Tuer-Ankunft
(9100,-5400,4100) Band 3, per Debug-Menue JUMP 205 SEPTIC POOL geladen).

Frage: blockt das Original eine geschobene Kiste an einer anderen (Objekt-gegen-Objekt-Pass FUN_8002bd44
@0x8002c03c-c0a8, FUN_8002cdf4 @0x8002c078)?

Aufbau (RAM ueber den GDB-Server, wie die Spinnen-Abschaltung der Spur R):
  * die beiden Adult Spider (0x25) aus: Aktiv-Bit 0 ihres Entity-Worts + g_active_count @0x800aca4e - 2
  * Raetsel-Kiste obj 1 (Pool 0x800b3f98 + 0x94*1, Band 0 (2052,0,-5552)) SUEDLICH vor die Band-3-Kiste obj 6 setzen:
    +0x34/+0x38/+0x3C = (-1600,-5400,-6948), +0x82 = 3, Spiegel +0x40/+0x42/+0x44 = dieselbe Lage (s16).
    obj 6 steht bei (-1600,-5400,-9348); Abstand dz 2400 > 1800 = hz+hz -> noch keine Ueberlappung.
  * Leon wie orig_2050_push.py (Spur R) hinter obj 6 (Nordseite) und nach SUEDEN schieben (Blick 3072).
Erwartung nach dem Code: sobald die Ziel-Lage von obj 6 z >= -8748 erreicht (|dZ| <= 1800), trifft FUN_8002cdf4 ->
Haltepunkt @0x8002cfb8 (`ori v0,zero,0x1`, Ueberlappung gefunden) und danach @0x8002c0b0 (GESPERRT, Ruecksetzung).
Haltepunkte: @0x8002bfc0 SCHUB-OK, @0x8002c0b0 GESPERRT, @0x8002cfb8 OBJEKT-TREFFER (a3 = geschobenes, t2 = anderes),
@0x8003da78 TREFFER-ZELLE (Zellen-Sperre zur Unterscheidung).
Aufruf: python orig_2050_kiste.py <ausgabeordner>. settings.ini wird gesichert und zurueckgeschrieben."""
import os, shutil, struct, subprocess, sys, time, math

R36 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "befunde_runde36", "R_belege")
sys.path.insert(0, os.path.abspath(R36))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL, POOL, OBJ_SZ, BP_OK, BP_BLOCK, BP_HIT

STATE = os.path.join(os.path.abspath(R36), "room2050_orig.sav")
ENT, ENT_SZ = 0x800ACC2C, 0x1F4
K = 6          # obj 6 = die geschobene Kiste (Band 3)
K_ANDERE = 1   # obj 1 = Raetsel-Kiste, wird vor obj 6 gesetzt
BP_OBJ = 0x8002CFB8
ZIEL1 = (-1600, -5400, -6948)


class Gdb2(Gdb):
    def memw(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK":
            raise IOError("M %08x -> %r" % (addr, r))


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
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        pad_bind_alle(INI)
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", STATE, CUE], cwd=out)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        L("DuckStation gestartet", eigene)
        g = Gdb2(PORT)
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
                    struct.unpack_from("<i", o, 0x3C)[0], o[4], o[5], o[0x82], o[0x8C],
                    struct.unpack_from("<I", o, 0)[0])

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        L("Spieler:", pl(), "obj6:", obj(K), "obj1:", obj(K_ANDERE))
        for s in range(2):
            a = ENT + ENT_SZ * s
            w0 = struct.unpack("<H", g.mem(a, 2))[0]
            e8 = g.mem(a + 8, 1)[0]
            g.memw(a, struct.pack("<H", w0 & 0xFFFE))
            L("Entity %d @%08x: w0 %04x typ 0x%02x -> w0 %04x" % (s, a, w0, e8, struct.unpack("<H", g.mem(a, 2))[0]))
        n_akt = g.mem(0x800ACA4E, 1)[0]
        g.memw(0x800ACA4E, bytes([max(0, n_akt - 2)]))
        L("g_active_count @0x800aca4e: %d -> %d" % (n_akt, g.mem(0x800ACA4E, 1)[0]))
        # obj 1 vor obj 6 setzen (Lage + Band + Spiegel)
        a1 = POOL + OBJ_SZ * K_ANDERE
        g.memw(a1 + 0x34, struct.pack("<iii", *ZIEL1))
        g.memw(a1 + 0x40, struct.pack("<hhh", *ZIEL1))
        g.memw(a1 + 0x82, bytes([3]))
        L("obj1 gesetzt:", obj(K_ANDERE), "Kasten-Zeiger +0x78 = %08x" % struct.unpack("<I", g.mem(a1 + 0x78, 4))[0])
        b78 = struct.unpack("<I", g.mem(a1 + 0x78, 4))[0]
        L("obj1 Kasten:", struct.unpack("<6h", g.mem(b78, 12)))
        b78_6 = struct.unpack("<I", g.mem(POOL + OBJ_SZ * K + 0x78, 4))[0]
        L("obj6 Kasten:", struct.unpack("<6h", g.mem(b78_6, 12)))
        for a in (BP_OK, BP_BLOCK, BP_OBJ, BP_HIT):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
        ereignisse = []

        def lauf(sekunden, *btns, bed=None, stall=None):
            pad(*btns)
            g.cont()
            t0 = time.time()
            t_ok = t0
            grund = "Zeit"
            while True:
                left = sekunden - (time.time() - t0)
                if left <= 0:
                    break
                if stall is not None and time.time() - t0 > 3.0 and time.time() - t_ok > stall:
                    grund = "Stillstand"
                    break
                r = g.wait_stop(min(left, 0.5))
                if r is None:
                    if bed is not None:
                        g.stop()
                        if bed(obj(K)):
                            grund = "Bedingung"
                            break
                        g.cont()
                    continue
                rg = g.regs()
                pc = rg[37] if len(rg) >= 38 else 0
                tag = {BP_OK: "SCHUB-OK", BP_BLOCK: "SCHUB-GESPERRT", BP_OBJ: "OBJEKT-TREFFER",
                       BP_HIT: "TREFFER-ZELLE"}.get(pc, "stop@%08x" % pc)
                o = obj(K)
                p = pl()
                if pc == BP_OBJ:
                    t2, a3 = rg[10], rg[7]
                    L("  OBJEKT-TREFFER @0x8002cfb8 t2(anderes)=%08x a3(geschoben)=%08x | obj6=%s obj1=%s spieler=%s" % (
                        t2, a3, o[:3], obj(K_ANDERE)[:3], p[:3]))
                    ereignisse.append((tag, o, p))
                elif pc == BP_HIT:
                    zelle = g.mem(rg[4], 12)
                    w, d, x, z, t, u0, u1, fl = struct.unpack("<HHhhBBBB", zelle)
                    L("  TREFFER-ZELLE x %d..%d z %d..%d u0 0x%02x fl 0x%02x | obj6=%s" % (x, x + w, z, z + d, u0, fl, o[:3]))
                    ereignisse.append((tag, o, p))
                else:
                    ereignisse.append((tag, o, p))
                    n_ok = sum(1 for e in ereignisse if e[0] == "SCHUB-OK")
                    if tag != "SCHUB-OK" or n_ok % 25 == 1:
                        L("  %s obj6=%s spieler=%s (OK bisher %d)" % (tag, o[:3], p[:3], n_ok))
                    if tag == "SCHUB-OK":
                        t_ok = time.time()
                if bed is not None and bed(o):
                    grund = "Bedingung"
                    break
                g.cont()
            g.stop()
            pad()
            return grund

        r_sign = 0
        lauf(0.8, B.XUSB_GAMEPAD_DPAD_RIGHT)
        for _ in range(3):
            p0 = pl()
            lauf(0.4, B.XUSB_GAMEPAD_DPAD_RIGHT)
            p1 = pl()
            dr = ((p1[3] - p0[3] + 2048) % 4096) - 2048
            L("Drehsinn: R 0.4 s -> dBlick = %d" % dr)
            if abs(dr) > 100:
                r_sign = 1 if dr > 0 else -1
                break
        if r_sign == 0:
            L("Pad kommt nicht an (kein Drehen) - Abbruch")
            return 4

        def drehe_auf(ziel, tol=100):
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

        def gehe_zu(x, z, tol=200, maxs=60):
            t0 = time.time()
            while time.time() - t0 < maxs:
                p = pl()
                d = math.hypot(x - p[0], z - p[2])
                if d <= tol:
                    L("  GEHE (%d,%d): angekommen bei (%d,%d)" % (x, z, p[0], p[2]))
                    return True
                drehe_auf(ziel_blick(x, z))
                lauf(min(1.0, max(0.12, (d - 300) / 1800.0)), B.XUSB_GAMEPAD_DPAD_UP)
            p = pl()
            L("  GEHE (%d,%d): NICHT angekommen bei (%d,%d)" % (x, z, p[0], p[2]))
            return False

        def zustand(tag=""):
            p = pl()
            o = obj(K)
            o1 = obj(K_ANDERE)
            L("%s spieler=(%d,%d,%d) blick=%d band=%d sub=%d | obj6=(%d,%d,%d) band=%d n=%d | obj1=(%d,%d,%d) band=%d" % (
                tag, p[0], p[1], p[2], p[3], p[4], p[5], o[0], o[1], o[2], o[5], o[6], o1[0], o1[1], o1[2], o1[5]))

        zustand("START")
        gehe_zu(9100, -6300)
        gehe_zu(-100, -6300)
        gehe_zu(-100, -8400)
        gehe_zu(-100, -11200)
        for r in range(3):
            o = obj(K)
            gehe_zu(o[0] + 600, o[2] - 1450, 110)
            drehe_auf(3072, 40)
            zustand("SUED Runde %d VOR" % r)
            grund = lauf(40, B.XUSB_GAMEPAD_DPAD_UP, bed=lambda o: o[2] >= -7050, stall=2.0)
            zustand("SUED Runde %d NACH (%s)" % (r, grund))
            lauf(0.6)
        ok = sum(1 for e in ereignisse if e[0] == "SCHUB-OK")
        bl = sum(1 for e in ereignisse if e[0] == "SCHUB-GESPERRT")
        ob = sum(1 for e in ereignisse if e[0] == "OBJEKT-TREFFER")
        ze = sum(1 for e in ereignisse if e[0] == "TREFFER-ZELLE")
        L("SUMME: Schub angenommen %d, gesperrt %d, davon Objekt-Treffer %d, Zellen-Treffer %d" % (ok, bl, ob, ze))
        zustand("ENDE")
        for a in (BP_OK, BP_BLOCK, BP_OBJ, BP_HIT):
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
