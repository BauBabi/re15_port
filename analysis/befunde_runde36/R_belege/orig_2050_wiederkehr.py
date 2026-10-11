"""orig_2050_wiederkehr.py - Runde 36 Spur R, Nachbesserung 3 (Abschluss): steht die Kiste obj 6 von ROOM2050 nach
ihrem FALL beim Wiederbetreten des Raums wieder auf ihrem Record? Gegenstueck zum Port-Lauf nb3_2050wiederkehr
(Wegplan wegplan_2050_wiederkehr.txt). Original = DuckStation-GDB, MZD-Disc mit Auslieferungs-EXE (@0x80026e4c 0800e003),
Spielstand R_belege/room2050_orig.sav (JUMP 205, Leon an der Tuer-Ankunft (9100,-5400,4100) Band 3).

Ablauf (Weg und Schuebe wie orig_2050_fall.py / orig_2050_push.py, Haltepunkte nur @0x8002bfc0 SCHUB-OK,
@0x8002c0b0 SCHUB-GESPERRT, @0x8002c13c FALL-AUSLOESER; KEIN Haltepunkt in FUN_8003da78, damit der Fall frei laeuft):
  1 Sued-/Ost-/Nord-Schub bis zum Fall-Ausloeser, 3 s ohne Haltepunkte -> Ruhelage obj 6 (erwartet Band 2).
  2 zurueck (7600,-6300) -> (9100,-6300) -> (9100,4300), Blick 3072 (+Z), SQUARE (XInput X) = Tuer slot 0 @0x01C4A
    (floor 3, x 8100..10100 z 4700..6700) -> ROOM2000-Ankunft (-26450,-1800,10250).
  3 in ROOM2000 Blick 2048 (-X), SQUARE = Tuer slot 4 @0x0169E (x -27600..-26600 z 9800..10800) -> ROOM2050.
  4 Pool-Dump: obj 6 (Pool-Index 6) wieder auf dem Record main00 @0x01D0E (-1600,-5400,-9348) Band 3?
Mechanismus, der hier gemessen wird: der Raumlader FUN_8003ea7c nullt bei JEDEM Laden pool[i]+0 (@0x8003eab0-cc, 32
Eintraege, Schritt 148, Aufruf FUN_800396fc @0x800399a0), main00 setzt obj 6 unbedingt neu (Obj_model_set @0x01D0E ->
Installer LAB_80040914, obj.x/y/z @0x800409bc/@0x800409c8/@0x800409d4).
Aufruf: python orig_2050_wiederkehr.py <ausgabeordner>
settings.ini wird gesichert und am Ende zurueckgeschrieben."""
import os, shutil, struct, subprocess, sys, time, math

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL, POOL, OBJ_SZ, BP_OK, BP_BLOCK, BP_FALL

STATE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "room2050_orig.sav")
ENT, ENT_SZ = 0x800ACC2C, 0x1F4
RDT_PTR = 0x800AC778
K = 6   # obj 6 = Pool-Index 6 (room2050_orig.sav: (-1600,-5400,-9348) Band 3)
REC6 = (-1600, -5400, -9348)
ANK_2000 = (-26450, -1800, 10250)   # ROOM2050 slot 0 @0x01C4A -> ROOM2000
ANK_2050 = (9100, -5400, 4100)      # ROOM2000 slot 4 @0x0169E -> ROOM2050


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
                    struct.unpack_from("<i", o, 0x3C)[0], o[4], o[5], o[0x82], o[0x8C])

        def rdt_kopf():
            r = struct.unpack("<I", g.mem(RDT_PTR, 4))[0]
            if not (0x80000000 <= r < 0x80200000):
                return r, "ungueltig"
            try:
                return r, g.mem(r, 16).hex()
            except IOError:
                return r, "unlesbar"

        def pool_dump(tag):
            for k in range(10):
                o = obj(k)
                w0 = struct.unpack("<I", g.mem(POOL + OBJ_SZ * k, 4))[0]
                sp = struct.unpack("<hh", g.mem(POOL + OBJ_SZ * k + 0x40, 2) + g.mem(POOL + OBJ_SZ * k + 0x44, 2))
                L("  %s Pool %d: w0 %08x pos (%d,%d,%d) spiegel (%d,%d) faellt=%d z=%d band=%d" % (
                    tag, k, w0, o[0], o[1], o[2], sp[0], sp[1], o[3], o[4], o[5]))

        def gegner_weg(tag):
            n_akt = g.mem(0x800ACA4E, 1)[0]
            weg = 0
            for sidx in range(8):
                a = ENT + ENT_SZ * sidx
                w0 = struct.unpack("<H", g.mem(a, 2))[0]
                e8 = g.mem(a + 8, 1)[0]
                if (w0 & 1) and e8 != 0:
                    pos = struct.unpack("<iii", g.mem(a + 0x34, 12))
                    g.memw(a, struct.pack("<H", w0 & 0xFFFE))
                    weg += 1
                    L("  %s Entity %d @%08x: w0 %04x typ 0x%02x pos %s -> Aktiv-Bit 0 geloescht" % (tag, sidx, a, w0, e8, pos))
            g.memw(0x800ACA4E, bytes([max(0, n_akt - weg)]))
            L("  %s g_active_count @0x800aca4e: %d -> %d" % (tag, n_akt, g.mem(0x800ACA4E, 1)[0]))

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        kopf2050 = rdt_kopf()
        L("Spieler:", pl(), "obj6:", obj(K), "HP:", struct.unpack("<h", g.mem(0x800ACAEE, 2))[0], "RDT:", kopf2050)
        pool_dump("START")
        o6 = obj(K)
        if (o6[0], o6[1], o6[2]) != REC6:
            L("obj 6 nicht auf dem Record %s - falscher Spielstand, Abbruch" % (REC6,))
            return 3
        gegner_weg("2050")
        for a in (BP_OK, BP_BLOCK, BP_FALL):
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
                tag = {BP_OK: "SCHUB-OK", BP_BLOCK: "SCHUB-GESPERRT", BP_FALL: "FALL-AUSLOESER"}.get(pc, "stop@%08x" % pc)
                o = obj(K)
                p = pl()
                ereignisse.append((tag, o, p))
                if not (tag == "SCHUB-OK" and len(ereignisse) % 25 != 1):
                    L("  %s obj6=%s spieler=%s" % (tag, o, p))
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
            g.stop()
            pad()
            return grund

        def zustand(tag=""):
            p = pl()
            o = obj(K)
            L("%s spieler=(%d,%d,%d) blick=%d band=%d sub=%d | obj6=(%d,%d,%d) faellt=%d z=%d band=%d n=%d" % (
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

        def druecke_tuer(tag, kopf_vor, ankunft):
            lauf(1.0)
            zustand("%s VOR" % tag)
            lauf(0.2, B.XUSB_GAMEPAD_X)
            t0 = time.time()
            while time.time() - t0 < 20:
                lauf(1.0)
                kopf = rdt_kopf()
                p = pl()
                if kopf[1] != kopf_vor[1] and abs(p[0] - ankunft[0]) < 300 and abs(p[2] - ankunft[2]) < 300:
                    lauf(2.0)
                    p = pl()
                    L("%s: TUER AUSGELOEST — RDT-Kopf %s -> %s, Spieler %s (Ankunft %s)" % (tag, kopf_vor[1][:16], kopf[1][:16], p, ankunft))
                    return kopf
            L("%s: KEINE Tuer nach 20 s (RDT-Kopf %s), Spieler %s" % (tag, rdt_kopf()[1][:16], pl()))
            return None

        zustand("START")
        L("== Weg zur Nordseite der Kiste ==")
        gehe_zu(9100, -6300)
        gehe_zu(-100, -6300)
        gehe_zu(-100, -8400)
        gehe_zu(-100, -11200)
        L("== Schub 1: nach Sueden bis Kistenmitte z >= -7050 ==")
        schiebe_bis(3072, "z", ">=", -7050, 60, "SUED", lambda o: (o[0] + 600, o[2] - 1450))
        lauf(1.5)
        L("== Umweg in die Tasche westlich der Kiste ==")
        gehe_zu(-100, -8400)
        gehe_zu(-100, -5520, 150)
        gehe_zu(-3100, -5520, 150)
        L("== Schub 2: nach Osten bis Kistenmitte x >= 7550 ==")
        schiebe_bis(0, "x", ">=", 7550, 240, "OST", lambda o: (o[0] - 1450, min(o[2] + 600, -5500)))
        lauf(1.5)
        L("== Umweg nach Sueden der Kiste ==")
        o = obj(K)
        gehe_zu(o[0] - 1450, -5600, 150)
        L("== Schub 3: nach Norden bis Fall-Ausloeser ==")
        grund = schiebe_bis(1024, "z", "<=", -12000, 120, "NORD", lambda o: (o[0], o[2] + 1450))
        pad()
        for a in (BP_OK, BP_BLOCK, BP_FALL):
            g.cmd("z0,%x,4" % a)
        L("Haltepunkte entfernt (Ende Schub 3: %s); 3 s laufen lassen:" % grund)
        lauf(3.0)
        zustand("NACH FALL")
        pool_dump("NACH-FALL")
        L("== Zurueck zur Tuer slot 0 @0x01C4A (x 8100..10100 z 4700..6700 floor 3) ==")
        p = pl()
        gehe_zu(7600, -6300, 150)
        gehe_zu(9100, -6300, 150)
        gehe_zu(9100, 4300, 150)
        drehe_auf(3072, 40)
        kopf2000 = druecke_tuer("TUER 2050 slot 0", kopf2050, ANK_2000)
        if kopf2000 is None:
            drehe_auf(3072, 40)
            kopf2000 = druecke_tuer("TUER 2050 slot 0 (2)", kopf2050, ANK_2000)
        if kopf2000 is None:
            L("Tuer nach ROOM2000 nicht ausgeloest - Abbruch")
            return 5
        L("RDT ROOM2000:", kopf2000)
        gegner_weg("2000")
        drehe_auf(2048, 40)
        kopf_zurueck = druecke_tuer("TUER 2000 slot 4", kopf2000, ANK_2050)
        if kopf_zurueck is None:
            drehe_auf(2048, 40)
            kopf_zurueck = druecke_tuer("TUER 2000 slot 4 (2)", kopf2000, ANK_2050)
        if kopf_zurueck is None:
            L("Tuer nach ROOM2050 nicht ausgeloest - Abbruch")
            return 6
        L("RDT nach der Rueckkehr: %s (ROOM2050 beim Start: %s) -> %s" % (kopf_zurueck[1], kopf2050[1],
          "GLEICH" if kopf_zurueck[1] == kopf2050[1] else "VERSCHIEDEN"))
        zustand("WIEDERKEHR")
        pool_dump("WIEDERKEHR")
        o6 = obj(K)
        L("ERGEBNIS: obj 6 nach dem Wiederbetreten (%d,%d,%d) faellt=%d band=%d — Record main00 @0x01D0E %s -> %s" % (
            o6[0], o6[1], o6[2], o6[3], o6[5], REC6, "AUF DEM RECORD" if (o6[0], o6[1], o6[2]) == REC6 else "NICHT auf dem Record"))
        L("Ereignisse gesamt:", len(ereignisse), "Fall-Ausloeser:", fall[0],
          "HP:", struct.unpack("<h", g.mem(0x800ACAEE, 2))[0])
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
