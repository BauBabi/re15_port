"""orig_2060_glied.py — Runde 36 Spur R, Nachbesserung 3 (P1): das Ersatzweg-Glied ROOM2060 am ORIGINAL Ende-zu-Ende
(DuckStation-GDB, MZD-Disc = Auslieferungs-EXE @0x80026e4c 0800e003, Spielstand abn2/room2060_orig.sav = JUMP 206,
Leon an der 2030-Ankunft (7600,0,3950) Band 0, Tritt obj 2 nach dem Raumstart bei (-3143,0,2075)).

Plan (Gegenstueck zum Port-Lauf nb3_2060glied / Wegplan wegplan_2060_glied.txt):
  1. um den Tritt herum vor seine Suedseite, NORD-Schub (Blick 1024) bis Tritt-Mitte z <= -7400
  2. Westseite, OST-Schub (Blick 0) bis Tritt-Mitte x >= 300 (Kerbe slot 2 @0x00E92 x -550..1500 z -8300..-6250)
  3. Latch (3,172) lesen: Zonentabelle @0x80074664[3] = 0x800b0ff8 (Ck-Handler @0x8003fcf4), Wort +20, Bit 0x00080000
  4. Aufstieg: von Sueden (Blick 1024) SQUARE -> Spieler y -1800 Band 1 auf dem Tritt?
  5. oben nach Norden bis zur Eingrenzung, Blick 1024, SQUARE -> TUER (RDT-Kopf wechselt, Spieler an der 2040-Ankunft
     (-12850,-5400,1600)) oder ABSPRUNG (Band 0 noerdlich des Tritts)?
  6. Variante: Tritt weiter nach Norden (z <= -7650), erneut Aufstieg + SQUARE oben.
Haltepunkte wie orig_2050_push.py (@0x8002bfc0 SCHUB-OK, @0x8002c0b0 SCHUB-GESPERRT, @0x8002c13c FALL-AUSLOESER,
@0x8003da78 TREFFER-ZELLE). Pad: SQUARE = XInput X (settings.ini [Pad1] Square = Keyboard/J & SDL-0/X).
Aufruf: python orig_2060_glied.py <ausgabeordner> <savestate>
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


REC = (-3125, 0, 2075)       # ROOM2060 obj 2 Tritt @0x00EA6 Band 0 (Box c(50,-900,0) h(675,900,600))
RDT_PTR = 0x800AC778
FLAG3 = 0x800B0FF8           # Zonentabelle @0x80074664[3]
TUER_ANKUNFT = (-12850, -5400, 1600)   # ROOM2060 slot 1 @0x00DBA -> ROOM2040 Band 3


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

        def rdt_kopf():
            r = struct.unpack("<I", g.mem(RDT_PTR, 4))[0]
            return r, g.mem(r, 16).hex()

        def flag3_172():
            w = struct.unpack("<I", g.mem(FLAG3 + 20, 4))[0]
            return 1 if (w & 0x00080000) else 0

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        K = -1
        for k in range(16):
            o = obj(k)
            bp = struct.unpack("<I", g.mem(POOL + OBJ_SZ * k + 0x78, 4))[0]
            box = list(struct.unpack("<6h", g.mem(bp, 12))) if 0x80000000 <= bp < 0x80200000 else "-"
            w0 = struct.unpack("<I", g.mem(POOL + OBJ_SZ * k, 4))[0]
            sp = struct.unpack("<hh", g.mem(POOL + OBJ_SZ * k + 0x40, 2) + g.mem(POOL + OBJ_SZ * k + 0x44, 2))
            L("  Pool %2d: w0 %08x pos (%d,%d,%d) spiegel (%d,%d) faellt=%d z=%d band=%d box %s" % (k, w0, o[0], o[1], o[2], sp[0], sp[1], o[3], o[4], o[5], box))
            if abs(o[0] - REC[0]) <= 60 and o[1] == REC[1] and abs(o[2] - REC[2]) <= 60 and K < 0:
                K = k
        kopf0 = rdt_kopf()
        L("Spieler:", pl(), "K =", K, "HP:", struct.unpack("<h", g.mem(0x800ACAEE, 2))[0], "RDT:", kopf0, "(3,172) =", flag3_172())
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
                    L("  TREFFER-ZELLE a0=%08x obj=%08x r=%d: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x | obj=%s" % (
                        a0, a1, rg[6] & 0xffff, x, x + w, z, z + d, t, u0, u1, fl, o))
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
            L("%s spieler=(%d,%d,%d) blick=%d band=%d sub=%d | obj=(%d,%d,%d) faellt=%d z=%d band=%d n=%d | (3,172)=%d" % (
                tag, p[0], p[1], p[2], p[3], p[4], p[5], o[0], o[1], o[2], o[3], o[4], o[5], o[6], flag3_172()))
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

        def druecke(btn, dauer, warte, tag):
            """1 s Ruhe (Lauf 1: Square direkt nach dem Drehen (sub 3) / nach dem Schub (sub 8) wurde nicht
            angenommen — der Port-Plan wartet 20 Bilder vor AKTION), dann Knopf dauer s halten, dann warte s
            laufen lassen; Zustand davor/danach."""
            lauf(1.0)
            zustand("%s VOR" % tag)
            lauf(dauer, btn)
            lauf(warte)
            return zustand("%s NACH" % tag)

        def tuer_pruefen(tag):
            kopf = rdt_kopf()
            p = pl()
            if kopf[1] != kopf0[1] or (abs(p[0] - TUER_ANKUNFT[0]) < 300 and abs(p[2] - TUER_ANKUNFT[2]) < 300 and p[1] == TUER_ANKUNFT[1]):
                L("%s: TUER AUSGELOEST — RDT-Kopf %s -> %s, Spieler %s (2040-Ankunft %s)" % (tag, kopf0, kopf, p, TUER_ANKUNFT))
                return True
            L("%s: keine Tuer (RDT-Kopf unveraendert %s), Spieler %s" % (tag, kopf[1][:16], p))
            return False

        zustand("START")
        L("== 1. um den Tritt herum vor seine Suedseite (Port-Weg nb3_2060glied) ==")
        gehe_zu(5000, 3950, 200)
        gehe_zu(5000, -6900, 200)
        gehe_zu(-4700, -6900, 200)
        gehe_zu(-4700, 3250, 200)
        gehe_zu(-3100, 3300, 120)
        L("== NORD-Schub (Blick 1024) bis Tritt-Mitte z <= -7400 ==")
        schiebe_bis(1024, "z", "<=", -7400, 60, "NORD", lambda o: (o[0] + 25, o[2] + 1225), runden=12)
        o = obj(K)
        L("== 2. Westseite, OST-Schub (Blick 0) bis Tritt-Mitte x >= 300 (Kerbe) ==")
        gehe_zu(-4700, -5000, 200)
        gehe_zu(-4700, o[2], 200)
        schiebe_bis(0, "x", ">=", 300, 60, "OST", lambda o: (o[0] - 1500, o[2]), runden=12)
        zustand("NACH-OST")
        L("== 3. Latch (3,172) = %d (Zonentabelle @0x80074664[3] = 0x800b0ff8, Wort +20 Bit 0x00080000) ==" % flag3_172())
        o = obj(K)
        L("== 4. Aufstieg von Sueden: GEHE (obj.x+50, obj.z+1100), Blick 1024, SQUARE ==")
        gehe_zu(o[0] + 50, o[2] + 1100, 120)
        drehe_auf(1024, 40)
        p, o = druecke(B.XUSB_GAMEPAD_X, 0.2, 3.0, "AUFSTIEG-SQUARE")
        oben = (p[1] == -1800 and p[4] == 1)
        if not oben:
            p, o = druecke(B.XUSB_GAMEPAD_X, 0.5, 3.0, "AUFSTIEG-SQUARE-b")
            oben = (p[1] == -1800 and p[4] == 1)
        L("Aufstieg: %s (y %d, Band %d)" % ("OBEN" if oben else "NICHT oben", p[1], p[4]))
        if oben:
            L("== 5. oben nach Norden (UP 1.5 s) bis zur Eingrenzung, Blick 1024, SQUARE ==")
            lauf(1.5, B.XUSB_GAMEPAD_DPAD_UP)
            zustand("OBEN-NORDKANTE")
            drehe_auf(1024, 40)
            p, o = druecke(B.XUSB_GAMEPAD_X, 0.2, 3.0, "OBEN-SQUARE")
            tuer = tuer_pruefen("OBEN-SQUARE")
            if not tuer:
                L("Spieler nach SQUARE oben: y %d Band %d bei (%d,%d) -> %s" % (p[1], p[4], p[0], p[2], "ABSPRUNG (Band 0)" if p[4] == 0 else "noch oben"))
                if p[4] == 1:
                    L("== 5b. oben ein Stueck zurueck (DOWN 0.3 s), Blick 1024, SQUARE ==")
                    lauf(0.3, B.XUSB_GAMEPAD_DPAD_DOWN)
                    drehe_auf(1024, 40)
                    p, o = druecke(B.XUSB_GAMEPAD_X, 0.2, 3.0, "OBEN-SQUARE-2")
                    tuer = tuer_pruefen("OBEN-SQUARE-2")
        if not oben or not tuer:
            L("== 6. Variante: Tritt weiter nach Norden (z <= -7650), Aufstieg, oben SQUARE ==")
            o = obj(K)
            if pl()[4] != 0:
                L("Spieler nicht auf Band 0 - Variante uebersprungen")
            else:
                gehe_zu(o[0] + 25, o[2] + 1225, 120)
                schiebe_bis(1024, "z", "<=", -7650, 60, "NORD2", lambda o: (o[0] + 25, o[2] + 1225), runden=6)
                o = obj(K)
                gehe_zu(o[0] + 50, o[2] + 1100, 120)
                drehe_auf(1024, 40)
                p, o = druecke(B.XUSB_GAMEPAD_X, 0.2, 3.0, "AUFSTIEG2-SQUARE")
                if p[1] == -1800 and p[4] == 1:
                    lauf(1.5, B.XUSB_GAMEPAD_DPAD_UP)
                    zustand("OBEN2-NORDKANTE")
                    drehe_auf(1024, 40)
                    p, o = druecke(B.XUSB_GAMEPAD_X, 0.2, 3.0, "OBEN2-SQUARE")
                    tuer = tuer_pruefen("OBEN2-SQUARE")
                    if not tuer and p[4] == 1:
                        lauf(0.4, B.XUSB_GAMEPAD_DPAD_DOWN)
                        drehe_auf(1024, 40)
                        p, o = druecke(B.XUSB_GAMEPAD_X, 0.2, 3.0, "OBEN2-SQUARE-2")
                        tuer = tuer_pruefen("OBEN2-SQUARE-2")
                else:
                    L("Aufstieg 2: NICHT oben (y %d Band %d)" % (p[1], p[4]))
        pad()
        g.cont()
        time.sleep(1.0)
        g.stop()
        zustand("ENDE")
        tuer_pruefen("ENDE")
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
