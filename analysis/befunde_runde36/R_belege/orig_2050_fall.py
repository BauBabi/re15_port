"""orig_2050_fall.py (Nachbesserung 3, P1: Fall-Protokoll + Ruhelage; abgeleitet von orig_2050_push.py) - Runde 36 Spur R, Nachbesserung 2 (N3): die Kiste obj 6 in ROOM2050 am ORIGINAL (DuckStation,
MZD-Disc = Auslieferungs-EXE @0x80026e4c 0800e003, Spielstand R_belege/room2050_orig.sav: Leon an der Tuer-Ankunft
(9100,-5400,4100) Band 3, per Debug-Menue JUMP 205 SEPTIC POOL geladen) mit dem virtuellen Pad denselben Weg fahren
wie der Port (wegplan_2050_fall.txt, nb2_2050fall_c) und ueber den GDB-Server je Schritt messen:
  Haltepunkte  @0x8002bfc0 Schub ANGENOMMEN, @0x8002c0b0 Schub GESPERRT (Ruecksetzung @0x8002c0b8-d4),
               @0x8002c13c FALL-AUSLOESER (Objekt-Pass FUN_8003b7f0(.,0x1c2,Band) & 2 -> FUN_8002d3d8),
               @0x8003da78 TREFFER-ZELLE (a0 = sperrende Zelle).
  RAM          Spieler 0x800aca54, Objekt-Pool 0x800b3f98 + 0x94*k (obj 6 = k 6, im Spielstand geprueft),
               Gegner-Entities 0x800acc2c + s*0x1F4 (zwei Adult Spider 0x25).
Gegner: die Entity-Schleife FUN_8001a50c prueft @0x8001a55c-568 `lw v0,0(entity) / andi v0,v0,1 / beq v0,zero ->
0x8001a5a0` (Aktiv-Bit) und ruft den Handler @0x8001a580-588 UNGEPRUEFT (`lw v0,0(v0) / jalr v0`). Darum werden die
beiden Spinnen wie im Port (re15_actor_free: active = 0) ueber Bit 0 ihres Entity-Worts abgeschaltet (0x0801 ->
0x0800) UND g_active_count @0x800aca4e um 2 gesenkt (die Schleife zaehlt nur aktive Entities, @0x8001a58c/@0x8001a5b0-b8),
nicht ueber den Dispatch-Eintrag 0x80072c40 (jalr 0 waere ein Absturz).
Plan (Geometrie sca_dump.py ROOM2050 3, Dossier Nachbesserung 2):
  1 Spieler noerdlich der Kiste (-1000,-10900), Blick 3072 (+Z), UP: Kiste nach Sueden bis z >= -7050
  2 Umweg Ost/Sued/West in die Tasche (-3100,-6400), Blick 0 (+X), UP: Kiste nach Osten bis x >= 7550
  3 Umweg nach Sueden (7600,-5600), Blick 1024 (-Z), UP: Kiste nach Norden bis z <= -12000 oder Fall-Ausloeser.
Aufruf: python orig_2050_push.py <ausgabeordner>
settings.ini wird gesichert und am Ende zurueckgeschrieben."""
import os, shutil, struct, subprocess, sys, time, math

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL, POOL, OBJ_SZ, BP_OK, BP_BLOCK, BP_FALL, BP_HIT

STATE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "room2050_orig.sav")
ENT, ENT_SZ = 0x800ACC2C, 0x1F4
K = 6   # obj 6 = Pool-Index 6 (room2050_orig.sav: (-1600,-5400,-9348) Band 3)


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

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        L("Spieler:", pl(), "obj6:", obj(K), "HP:", struct.unpack("<h", g.mem(0x800ACAEE, 2))[0])
        # Gegner abschalten: Aktiv-Bit (Guard @0x8001a564-568) UND g_active_count @0x800aca4e (lbu s1,-13746(s1)
        # @0x8001a528): die Schleife zaehlt nur aktive Entities (addiu s0,s0,1 im Delay-Slot des jalr @0x8001a58c) und
        # laeuft bis s0 == g_active_count (@0x8001a5b0-b8) — ohne Zaehlerkorrektur laeuft sie ueber den Pool hinaus
        # (gemessen orig_2050_diag2_ent: Padwort kommt an, Leon dreht nicht; ent2 mit Korrektur: dreht).
        for s in range(2):
            a = ENT + ENT_SZ * s
            w0 = struct.unpack("<H", g.mem(a, 2))[0]
            e8 = g.mem(a + 8, 1)[0]
            pos = struct.unpack("<iii", g.mem(a + 0x34, 12))
            g.memw(a, struct.pack("<H", w0 & 0xFFFE))
            L("Entity %d @%08x: w0 %04x typ 0x%02x pos %s -> w0 %04x (Aktiv-Bit 0 geloescht)" % (
                s, a, w0, e8, pos, struct.unpack("<H", g.mem(a, 2))[0]))
        n_akt = g.mem(0x800ACA4E, 1)[0]
        g.memw(0x800ACA4E, bytes([max(0, n_akt - 2)]))
        L("g_active_count @0x800aca4e: %d -> %d" % (n_akt, g.mem(0x800ACA4E, 1)[0]))
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
        ereignisse = []
        fall = [0]

        def lauf(sekunden, *btns, bed=None, stall=None):
            """Pad halten; Haltepunkte protokollieren; bed(obj) -> True beendet vorzeitig; stall = Sekunden ohne
            angenommenen Schub (nach 3 s Anlauf), die den Schub beenden. Ende: angehalten."""
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
                    L("  TREFFER-ZELLE a0=%08x obj=%08x: x %d..%d z %d..%d typ %d u0 0x%02x u1 0x%02x fl 0x%02x | obj6=%s" % (
                        a0, a1, x, x + w, z, z + d, t, u0, u1, fl, o))
                else:
                    s4, s5 = rg[20], rg[21]
                    ereignisse.append((tag, o, p))
                    if tag == "SCHUB-OK" and len(ereignisse) % 25 != 1 and pc == BP_OK:
                        pass   # jeden 25. angenommenen Schub ausfuehrlich, die uebrigen nur zaehlen
                    else:
                        L("  %s obj6=%s spieler=%s s4/s5=(%d,%d)" % (tag, o, p, (s4 & 0xffff) - (0x10000 if s4 & 0x8000 else 0),
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
            L("%s spieler=(%d,%d,%d) blick=%d band=%d sub=%d | obj6=(%d,%d,%d) faellt=%d z=%d band=%d n=%d" % (
                tag, p[0], p[1], p[2], p[3], p[4], p[5], o[0], o[1], o[2], o[3], o[4], o[5], o[6]))
            return p, o

        r_sign = 0
        # Gemessen (orig_2050_diag1): die erste halbe Sekunde nach dem ersten Tastendruck kommt nichts an (Padwort 0),
        # danach durchgehend (+1440 Blick je 0.5 s = 96/Bild). Darum erst 0.8 s aufwaermen, dann 0.4 s messen
        # (< 2048 Drehung, sonst ist das Vorzeichen modulo 4096 mehrdeutig — Lauf c: dBlick -1888 = +2208).
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
            """Schub in Runden: Spieler an die Station (relativ zur AKTUELLEN Kistenlage), Blick, UP halten, bis die
            Bedingung gilt, die Kiste faellt, dreimal gesperrt wird oder der Schub abreisst (Lauf c: Leon driftete beim
            Suedschub von x -883 ueber -250 hinaus seitlich von der Kiste, die Reichweite FUN_8002d1e8 riss ab)."""
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
        L("== Weg zur Nordseite der Kiste (Ostkorridor x 8818..9432, dann z -6300 nach Westen) ==")
        gehe_zu(9100, -6300)
        gehe_zu(-100, -6300)
        gehe_zu(-100, -8400)
        gehe_zu(-100, -11200)
        L("== Schub 1: nach Sueden bis Kistenmitte z >= -7050 (r*[43] z -8750..-8050 verlangt z >= -7150) ==")
        schiebe_bis(3072, "z", ">=", -7050, 60, "SUED", lambda o: (o[0] + 600, o[2] - 1450))
        lauf(1.5)
        L("== Umweg in die Tasche westlich der Kiste (x <= -2950, z (-6632,-5368)) ==")
        gehe_zu(-100, -8400)
        gehe_zu(-100, -5520, 150)
        gehe_zu(-3100, -5520, 150)
        L("== Schub 2: nach Osten bis Kistenmitte x >= 7550 (Fenster [7550,7650) zwischen r*[44] und r*[40]) ==")
        schiebe_bis(0, "x", ">=", 7550, 240, "OST", lambda o: (o[0] - 1450, min(o[2] + 600, -5500)))
        lauf(1.5)
        L("== Umweg nach Sueden der Kiste ==")
        o = obj(K)
        gehe_zu(o[0] - 1450, -5600, 150)
        L("== Schub 3: nach Norden bis z <= -12000 oder Fall-Ausloeser (Absprungzelle r*[49] z -13950..-12150, +450) ==")
        grund = schiebe_bis(1024, "z", "<=", -12000, 120, "NORD", lambda o: (o[0], o[2] + 1450))
        pad()
        # ---- Nachbesserung 3 (P1): FALL-PROTOKOLL — je Stopp in FUN_8003da78 die Zelle (a0), r (a2), Lage und
        # Spiegel +0x40/+0x44 VOR der Ausschiebung; danach OHNE Haltepunkte 2 s laufen lassen und die Ruhelage lesen
        # (die NACH-FALL-Samples der Abnahme waren Haltepunkt-Stopps, keine Bildende-Lagen).
        def obj_voll(k):
            o = g.mem(POOL + OBJ_SZ * k, OBJ_SZ)
            return (struct.unpack_from("<i", o, 0x34)[0], struct.unpack_from("<i", o, 0x38)[0],
                    struct.unpack_from("<i", o, 0x3C)[0], o[4], o[5], o[0x82],
                    struct.unpack_from("<h", o, 0x40)[0], struct.unpack_from("<h", o, 0x44)[0])
        L("== FALL-PROTOKOLL: Stopps in FUN_8003da78 (@0x8003b79c je getroffener Zelle) waehrend des Falls ==")
        g.cont()
        t0 = time.time()
        n_hit = 0
        ruhe_seit = None
        while time.time() - t0 < 40:
            r = g.wait_stop(0.5)
            if r is None:
                g.stop()
                o = obj_voll(K)
                L("  Sample obj=(%d,%d,%d) faellt=%d z=%d band=%d spiegel=(%d,%d)" % o)
                if o[3] == 0:
                    if ruhe_seit is None:
                        ruhe_seit = time.time()
                    elif time.time() - ruhe_seit > 2.0:
                        g.cont()
                        break
                g.cont()
                continue
            rg = g.regs()
            pc = rg[37] if len(rg) >= 38 else 0
            o = obj_voll(K)
            if pc == BP_HIT:
                n_hit += 1
                a0, a2 = rg[4], rg[6] & 0xffff
                w, d, x, z, t, u0, u1, fl = struct.unpack("<HHhhBBBB", g.mem(a0, 12))
                L("  HIT %d: Zelle a0=%08x x %d..%d z %d..%d u0 0x%02x fl 0x%02x r=%d | obj VOR=(%d,%d,%d) faellt=%d z=%d band=%d spiegel=(%d,%d)" % (
                    n_hit, a0, x, x + w, z, z + d, u0, fl, a2, o[0], o[1], o[2], o[3], o[4], o[5], o[6], o[7]))
            elif pc in (BP_OK, BP_BLOCK, BP_FALL):
                L("  stop %s obj=%s" % ({BP_OK: "SCHUB-OK", BP_BLOCK: "SCHUB-GESPERRT", BP_FALL: "FALL-AUSLOESER"}[pc], o))
            else:
                L("  stop@%08x obj=%s" % (pc, o))
            g.cont()
        g.stop()
        for a in (BP_OK, BP_BLOCK, BP_FALL, BP_HIT):
            g.cmd("z0,%x,4" % a)
        L("Haltepunkte entfernt; Ruhelage ohne Haltepunkte:")
        for i in range(3):
            g.cont()
            time.sleep(2.0)
            g.stop()
            o = obj_voll(K)
            L("  RUHELAGE %d: obj=(%d,%d,%d) faellt=%d z=%d band=%d spiegel=(%d,%d)" % ((i,) + o))
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
