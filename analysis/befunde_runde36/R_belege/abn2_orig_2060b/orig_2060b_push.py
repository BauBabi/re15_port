"""orig_2060b_push.py (Lauf b: Tritt-Suche tolerant, Original hat ihn beim Raumstart auf x -3143 ausgeschoben) - Abnahme 2 Spur R (N5): die Kiste obj 1 in ROOM3050 am ORIGINAL (DuckStation, MZD-Disc =
Auslieferungs-EXE @0x80026e4c 0800e003) mit dem virtuellen Pad denselben Weg fahren wie der Port (nb1_3050fall_d:
Osten der Kiste -> Schub WEST bis zur Westwand r*[31], dann Norden der Kiste -> Schub SUED in die Absprungzelle r*[46])
und ueber den GDB-Server je Schritt messen (Haltepunkte wie orig_2050_push.py: @0x8002bfc0 SCHUB-OK, @0x8002c0b0
SCHUB-GESPERRT, @0x8002c13c FALL-AUSLOESER, @0x8003da78 TREFFER-ZELLE). ROOM3050 hat keine Sce_em_set im main00 ->
keine Gegner-Neutralisierung. Pool-Index K der Kiste wird ueber die Record-Lage (-18000,-7200,-20800) gesucht.
Aufruf: python orig_3050_push.py <ausgabeordner> <savestate>
"""
import os, shutil, struct, subprocess, sys, time, math

BELEGE = r"C:\workspace\git\reAi_v2\.claude\worktrees\r36_dachkiste\analysis\befunde_runde36\R_belege"
sys.path.insert(0, BELEGE)
from orig_1090_push import Gdb as Gdb0, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL, POOL, OBJ_SZ, BP_OK, BP_BLOCK, BP_FALL, BP_HIT

class Gdb(Gdb0):
    def memw(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK":
            raise IOError("M %08x -> %r" % (addr, r))


REC = (-3125, 0, 2075)   # ROOM2060 obj 2 Tritt @0x00EA6 Band 0 (Box c(50,-900,0) h(675,900,600))


def main():
    out = os.path.abspath(sys.argv[1])
    state = os.path.abspath(sys.argv[2])
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
        subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE], cwd=out)
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
        K = -1
        for k in range(16):
            o = obj(k)
            bp = struct.unpack("<I", g.mem(POOL + OBJ_SZ * k + 0x78, 4))[0]
            box = list(struct.unpack("<6h", g.mem(bp, 12))) if 0x80000000 <= bp < 0x80200000 else "-"
            w0 = struct.unpack("<I", g.mem(POOL + OBJ_SZ * k, 4))[0]
            L("  Pool %2d: w0 %08x pos (%d,%d,%d) faellt=%d z=%d band=%d box %s" % (k, w0, o[0], o[1], o[2], o[3], o[4], o[5], box))
            if abs(o[0] - REC[0]) <= 60 and o[1] == REC[1] and abs(o[2] - REC[2]) <= 60 and K < 0:
                K = k
        L("Spieler:", pl(), "K =", K, "HP:", struct.unpack("<h", g.mem(0x800ACAEE, 2))[0])
        if K < 0:
            L("Tritt obj 2 (-3125,0,2075) nicht im Pool - falscher Raum/Spielstand, Abbruch")
            return 3
        ENT, ENT_SZ = 0x800ACC2C, 0x1F4
        n_akt = g.mem(0x800ACA4E, 1)[0]
        weg = 0
        for sidx in range(max(n_akt, 1)):
            a = ENT + ENT_SZ * sidx
            w0 = struct.unpack("<H", g.mem(a, 2))[0]
            e8 = g.mem(a + 8, 1)[0]
            pos = struct.unpack("<iii", g.mem(a + 0x34, 12))
            if (w0 & 1) and e8 != 0:
                g.memw(a, struct.pack("<H", w0 & 0xFFFE))
                weg += 1
                L("Entity %d @%08x: w0 %04x typ 0x%02x pos %s -> Aktiv-Bit geloescht" % (sidx, a, w0, e8, pos))
            else:
                L("Entity %d @%08x: w0 %04x typ 0x%02x pos %s (bleibt)" % (sidx, a, w0, e8, pos))
        g.memw(0x800ACA4E, bytes([max(0, n_akt - weg)]))
        L("g_active_count @0x800aca4e: %d -> %d" % (n_akt, g.mem(0x800ACA4E, 1)[0]))
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
        ereignisse = []
        fall = [0]

        def lauf(sekunden, *btns, bed=None, stall=None):
            pad(*btns)
            g.cont()
            t0 = time.time()
            t_ok = t0
            grund = "Zeit"
            gesperrt_folge = 0
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
                        o = obj(K)
                        if bed(o):
                            grund = "Bedingung"
                            break
                        g.cont()
                    continue
                rg = g.regs()
                pc = rg[37] if len(rg) >= 38 else 0
                tag = {BP_OK: "SCHUB-OK", BP_BLOCK: "SCHUB-GESPERRT", BP_FALL: "FALL-AUSLOESER", BP_HIT: "TREFFER-ZELLE"}.get(pc, "stop@%08x" % pc)
                o = obj(K)
                p = pl()
                if pc == BP_HIT:
                    a0, a1 = rg[4], rg[5]
                    zelle = g.mem(a0, 12)
                    w, d, x, z, t, u0, u1, fl = struct.unpack("<HHhhBBBB", zelle)
                    L("  TREFFER-ZELLE a0=%08x obj=%08x: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x | obj=%s" % (
                        a0, a1, x, x + w, z, z + d, t, u0, u1, fl, o))
                else:
                    s4, s5 = rg[20], rg[21]
                    ereignisse.append((tag, o, p))
                    if tag == "SCHUB-OK" and len(ereignisse) % 25 != 1 and pc == BP_OK:
                        pass
                    else:
                        L("  %s obj=%s spieler=%s s4/s5=(%d,%d)" % (tag, o, p, (s4 & 0xffff) - (0x10000 if s4 & 0x8000 else 0),
                                                                     (s5 & 0xffff) - (0x10000 if s5 & 0x8000 else 0)))
                    if tag == "SCHUB-GESPERRT":
                        gesperrt_folge += 1
                    elif tag == "SCHUB-OK":
                        gesperrt_folge = 0
                        t_ok = time.time()
                    if tag == "FALL-AUSLOESER":
                        fall[0] += 1
                        grund = "Fall-Ausloeser"
                        break
                    if gesperrt_folge >= 3 and bed is not None:
                        grund = "dreimal gesperrt"
                        break
                if bed is not None and bed(o):
                    grund = "Bedingung"
                    break
                g.cont()
            r = g.stop()
            pad()
            return grund

        def zustand(tag=""):
            p = pl()
            o = obj(K)
            L("%s spieler=(%d,%d,%d) blick=%d band=%d sub=%d | obj=(%d,%d,%d) faellt=%d z=%d band=%d n=%d" % (
                tag, p[0], p[1], p[2], p[3], p[4], p[5], o[0], o[1], o[2], o[3], o[4], o[5], o[6]))
            return p, o

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
            n0 = len(ereignisse)
            while time.time() - t0 < maxs:
                p = pl()
                d = math.hypot(x - p[0], z - p[2])
                if d <= tol:
                    L("  GEHE (%d,%d): angekommen bei (%d,%d), Schub-Ereignisse unterwegs %d" % (x, z, p[0], p[2], len(ereignisse) - n0))
                    return True
                drehe_auf(ziel_blick(x, z))
                lauf(min(1.0, max(0.12, (d - 300) / 1800.0)), B.XUSB_GAMEPAD_DPAD_UP)
            p = pl()
            L("  GEHE (%d,%d): NICHT angekommen, Rest %d bei (%d,%d)" % (x, z, math.hypot(x - p[0], z - p[2]), p[0], p[2]))
            return False

        def schiebe_bis(blick, achse, op, wert, maxs, tag, station, runden=12):
            idx = 2 if achse == "z" else 0
            bed = (lambda o: o[idx] <= wert) if op == "<=" else (lambda o: o[idx] >= wert)
            n0 = len(ereignisse)
            grund = "keine Runde"
            for r in range(runden):
                o = obj(K)
                if bed(o):
                    grund = "Bedingung"
                    break
                sx, sz = station(o)
                gehe_zu(sx, sz, 110)
                drehe_auf(blick, 40)
                zustand("%s Runde %d VOR" % (tag, r))
                grund = lauf(maxs, B.XUSB_GAMEPAD_DPAD_UP, bed=bed, stall=2.0)
                zustand("%s Runde %d NACH (%s)" % (tag, r, grund))
                if grund in ("Bedingung", "Fall-Ausloeser", "dreimal gesperrt"):
                    break
            ok = sum(1 for e in ereignisse[n0:] if e[0] == "SCHUB-OK")
            bl = sum(1 for e in ereignisse[n0:] if e[0] == "SCHUB-GESPERRT")
            fa = sum(1 for e in ereignisse[n0:] if e[0] == "FALL-AUSLOESER")
            L("%s: Schub angenommen %d x, gesperrt %d x, Fall-Ausloeser %d x, Ende: %s" % (tag, ok, bl, fa, grund))
            return grund

        zustand("START")
        L("== Weg wie der Port-Lauf 2060echt2: Ostseite nach Norden, Nordgasse nach Westen, Westgasse nach Sueden (am Tritt westlich vorbei), vor die Suedseite des Tritts ==")
        gehe_zu(5000, 3950, 200)
        gehe_zu(5000, -6900, 200)
        gehe_zu(-4700, -6900, 200)
        gehe_zu(-4700, 3250, 200)
        gehe_zu(-3100, 3300, 120)
        L("== Schub: nach Norden (Blick 1024) bis Tritt-Mitte z <= 1900 oder gesperrt/Stillstand (Port: jeder Schub gesperrt, TREFFER r0[9] x -2400..3150 z 2150..5350) ==")
        grund = schiebe_bis(1024, "z", "<=", 1900, 45, "NORD", lambda o: (o[0] + 25, o[2] + 1225), runden=3)
        L("== Gegenprobe: Schub nach Osten aus der Westseite (Blick 0) ==")
        grund2 = schiebe_bis(0, "x", ">=", -2900, 30, "OST", lambda o: (o[0] - 1500, o[2]), runden=2)
        pad()
        g.cont()
        for i in range(12):
            time.sleep(0.5)
            g.stop()
            zustand("NACH-FALL %d" % i)
            g.cont()
        g.stop()
        zustand("ENDE")
        L("Ereignisse gesamt:", len(ereignisse), "Fall-Ausloeser:", fall[0],
          "HP:", struct.unpack("<h", g.mem(0x800ACAEE, 2))[0])
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
