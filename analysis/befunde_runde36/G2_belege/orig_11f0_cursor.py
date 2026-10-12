"""orig_11f0_cursor.py - Runde 36 G2: der Generator-Cursor obj 0 (ROOM11F0) am ORIGINAL - schiebt der Objekt-Pass ihn
aus den SCA-Zellen (Ruhe-Zweig LAB_8002c0b8: FUN_8003b558(obj,2) @0x8002c0d0, Ausschiebung FUN_8003da78 @0x8003b79c)?
DuckStation (MZD-Disc, Auslieferungs-EXE @0x80026e4c 0800e003), Spielstand G2_belege/room11f0_orig.sav (JUMP 11F,
orig_11f0_spielstand.py; SCD und SCA im RAM == ROOM11F0.RDT, savestate_11f0_pruefen.txt).
Raetsel scharf wie sub16 @0x015C2..0x015F2 (13x Set(5,b,1), b = 0..12): Bank 5 = 0x800b1028 (Tabelle @0x80074664[5],
Set-Handler @0x8003fdf8-fe0c, Bit b -> Maske 0x80000000 >> b) |= 0xFFF80000. Danach liest sub01 je Bild Sce_key_ck 0x01/
0x04/0x02/0x08 (@0x01098/@0x010B0/@0x010C8/@0x010E0) und startet sub02..05 = Work_set(3,0) + Speed_set(2|0, +-200) +
Add_speed (@0x012F2..0x0131E; Handler @0x80040d2c/@0x80040f14/@0x80040f40: obj+0x34/+0x3C += Tempo).
Haltepunkte:
  BP_IN   @0x8002bd44  Eintritt Objekt-Pass (einmal je Bild; die SCD @0x8001cdec lief im selben Bild davor)
  BP_HIT  @0x8003da78  Ausschiebung (a0 = Zelle, a1 = Objekt, a2 = r) - protokolliert, wenn a1 = Cursor
  BP_OUT  @0x8002c158  Austritt Objekt-Pass (nach der Schleife @0x8002c150)
RAM: Cursor = Pool-Platz 0 = 0x800b3f98 (+0x34/+0x3C Lage, +0x40/+0x44 Spiegel), Pad gehalten 0x800ac768, Spieler 0x800aca54.
Plan: OBEN, UNTEN, LINKS, RECHTS - je die Taste halten, bis der Cursor nach dem Anlaufen 15 Bilder steht (max 160 Bilder).
Aufruf: python orig_11f0_cursor.py <ausgabeordner>"""
import os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "R_belege"))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT

STATE = os.path.join(HIER, "room11f0_orig.sav")
CURSOR = 0x800B3F98
PL = 0x800ACA54
BANK5 = 0x800B1028
BP_IN, BP_HIT, BP_OUT = 0x8002BD44, 0x8003DA78, 0x8002C158


def s32(b, o):
    return struct.unpack_from("<i", b, o)[0]


def s16(b, o):
    return struct.unpack_from("<h", b, o)[0]


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
        g = Gdb(PORT)
        L("GDB verbunden")
        time.sleep(8)
        L("Stopp:", g.stop())

        def cur():
            o = g.mem(CURSOR, 0x94)
            return (s32(o, 0x34), s32(o, 0x3C), s16(o, 0x40), s16(o, 0x44), struct.unpack_from("<I", o, 0)[0], o[8], o[0x82])

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        base = struct.unpack("<I", g.mem(0x800AC778, 4))[0]
        L("RDT-Basis %08x, Obj_model_set @RDT+0x00E54 im RAM:" % base, g.mem(base + 0xE54, 34).hex())
        c = cur()
        L("Cursor Pool 0: lage (%d,%d) spiegel (%d,%d) +0=%04x typ %d band %d" % c)
        b5 = struct.unpack("<I", g.mem(BANK5, 4))[0]
        g.cmd("M%x,4:%s" % (BANK5, struct.pack("<I", b5 | 0xFFF80000).hex()))
        L("Bank 5 Wort 0: %08x -> %08x (sub16 @0x015C2..0x015F2)" % (b5, struct.unpack("<I", g.mem(BANK5, 4))[0]))
        for a in (BP_IN, BP_HIT, BP_OUT):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))

        bild = [0]

        def phase(name, btn, max_bilder=160, steh_ende=15):
            L("== Phase %s ==" % name)
            pad(btn)
            g.cont()
            anlauf = True
            steh = 0
            vorher = None
            n = 0
            treffer = []
            zeile = None
            t_start = time.time()
            while n < max_bilder and time.time() - t_start < 240:
                r = g.wait_stop(5.0)
                if r is None:
                    g.stop()
                    L("  (kein Haltepunkt in 5 s)")
                    g.cont()
                    continue
                rg = g.regs()
                pc = rg[37] if len(rg) >= 38 else 0
                if pc == BP_IN:
                    c = cur()
                    padw = struct.unpack("<I", g.mem(0x800AC768, 4))[0]
                    p = g.mem(PL, 0x40)
                    zeile = {"in": c, "pad": padw, "spieler": (s32(p, 0x34), s32(p, 0x3C))}
                    treffer = []
                elif pc == BP_HIT:
                    if rg[5] == CURSOR and zeile is not None:
                        z = g.mem(rg[4], 12)
                        w, d, x, zz, t, u0, u1, fl = struct.unpack("<HHhhBBBB", z)
                        c = cur()
                        treffer.append("Zelle@%08x x %d..%d z %d..%d u0 0x%02x fl 0x%02x r=%d vor (%d,%d) spiegel (%d,%d)" % (
                            rg[4], x, x + w, zz, zz + d, u0, fl, rg[6], c[0], c[1], c[2], c[3]))
                elif pc == BP_OUT and zeile is not None:
                    c = cur()
                    bild[0] += 1
                    n += 1
                    lage = (c[0], c[1])
                    if vorher is not None and lage != vorher:
                        anlauf = False
                    if not anlauf and vorher is not None and lage == vorher:
                        steh += 1
                    elif not anlauf:
                        steh = 0
                    L("B%03d %s pad=%04x spieler=(%d,%d) nach-SCD (%d,%d) -> nach-Pass (%d,%d) spiegel (%d,%d)%s" % (
                        bild[0], name, zeile["pad"] & 0xFFFF, zeile["spieler"][0], zeile["spieler"][1],
                        zeile["in"][0], zeile["in"][1], c[0], c[1], c[2], c[3],
                        "".join("\n      TREFFER " + t for t in treffer)))
                    vorher = lage
                    zeile = None
                    if steh >= steh_ende:
                        break
                g.cont()
            g.stop()
            pad()
            L("Phase %s Ende: %d Bilder, Cursor %s, zuletzt %d Bilder Stillstand" % (name, n, vorher, steh))
            # Taste los, kurz ohne Taste weiterlaufen (Haltepunkte bleiben gesetzt -> einzeln durchreichen)
            t1 = time.time()
            g.cont()
            while time.time() - t1 < 1.0:
                r = g.wait_stop(0.3)
                if r is not None:
                    g.cont()
            g.stop()
            return vorher

        e1 = phase("OBEN", B.XUSB_GAMEPAD_DPAD_UP)
        e2 = phase("UNTEN", B.XUSB_GAMEPAD_DPAD_DOWN)
        e3 = phase("LINKS", B.XUSB_GAMEPAD_DPAD_LEFT)
        e4 = phase("RECHTS", B.XUSB_GAMEPAD_DPAD_RIGHT)
        L("ERGEBNIS Endlagen: OBEN %s UNTEN %s LINKS %s RECHTS %s" % (e1, e2, e3, e4))
        for a in (BP_IN, BP_HIT, BP_OUT):
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
        os.remove(bak)
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" %
          ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))


if __name__ == "__main__":
    sys.exit(main())
