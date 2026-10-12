"""Abnahme 3 Spur R - ROOM3050 am ORIGINAL: ist die Super Redhawk (Item_aot_set slot 6 @0x01EC6, floor 1, Rechteck
x -24800..-23000 z -23700..-22900, Taken-Bit pc[18] = 92 in Zone 9) NUR mit Spielerband 1 aufnehmbar (AOT-Scan FUN_80042bac
@0x80042cac-0x80042ccc: rec[2] == Spieler+0x82, sonst naechster Eintrag)?
A/B an derselben x/z/Blick-Stellung (-24026,-22770) Blick 1072 (Punkt 620 voraus z -23390 im Rechteck):
  A  y 0,     +0x82 = 0 (Boden der unteren Ebene)            -> erwartet KEINE Aufnahme
  A2 y -7200, +0x82 = 4 (obere Ebene, Nordgang; = Port-Lauf m3) -> erwartet KEINE Aufnahme
  B  y 0,     +0x82 = 1 (Band 1 = Dach der Kiste)             -> erwartet Aufnahme (Taken-Bit 92 gesetzt)
Taken-Bit: FUN_8004efe4(base = *(0x80074688), 92) = base[92>>5] & (0x80000000 >> (92 & 31)) = Wort +8, Maske 0x8.
Spielstand R_belege/room3050_orig.sav (JUMP 305, Auslieferungs-EXE). Aufruf: python orig_abn3_3050_item.py <ausgabeordner>"""
import os, shutil, struct, subprocess, sys, time

RB = r"C:\workspace\git\reAi_v2\.claude\worktrees\r36_dachkiste\analysis\befunde_runde36\R_belege"
sys.path.insert(0, RB)
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL

STATE = os.path.join(RB, "room3050_orig.sav")
ZONE9_PTR = 0x80074688
POS = (-24026, -22770)
BLICK = 1072


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
            return {"x": struct.unpack_from("<i", e, 0x34)[0], "y": struct.unpack_from("<i", e, 0x38)[0],
                    "z": struct.unpack_from("<i", e, 0x3C)[0], "blick": struct.unpack_from("<h", e, 0x6A)[0],
                    "band": e[0x82], "sub": e[0x05], "st": e[0x04]}

        def taken():
            base = struct.unpack("<I", g.mem(ZONE9_PTR, 4))[0]
            w = struct.unpack("<I", g.mem(base + 8, 4))[0]
            return base, w, 1 if (w & 0x8) else 0

        def lauf(sek, *btns):
            pad(*btns)
            g.cont()
            time.sleep(sek)
            g.stop()
            pad()

        def setze(y, band):
            g.memw(PL + 0x34, struct.pack("<i", POS[0]))
            g.memw(PL + 0x38, struct.pack("<i", y))
            g.memw(PL + 0x3C, struct.pack("<i", POS[1]))
            g.memw(PL + 0x6A, struct.pack("<h", BLICK))
            g.memw(PL + 0x82, bytes([band]))

        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        L("Spieler START:", pl())
        b0 = taken()
        L("Zone-9-Basis *(0x80074688) = %08x, Wort +8 = %08x, Taken-Bit 92 = %d" % b0)
        # Pad-Probe (der virtuelle Pad kommt erst nach dem ersten Druck an)
        lauf(1.2, B.XUSB_GAMEPAD_DPAD_RIGHT)
        L("Spieler nach Pad-Probe:", pl())
        for tag, y, band in (("A  (y 0, Band 0)", 0, 0), ("A2 (y -7200, Band 4)", -7200, 4), ("B  (y 0, Band 1)", 0, 1)):
            setze(y, band)
            lauf(0.5)
            setze(y, band)
            p = pl()
            L("%s VOR: Spieler %s" % (tag, p))
            for k in range(3):
                lauf(0.25, B.XUSB_GAMEPAD_X)
                lauf(1.5)
                L("  %s nach Druck %d: Spieler %s, Taken-Bit 92 = %d" % (tag, k + 1, pl(), taken()[2]))
            t = taken()
            L("%s ERGEBNIS: Taken-Bit 92 = %d (Wort +8 = %08x)" % (tag, t[2], t[1]))
            lauf(1.0)
        # C: Lift sub04 (Aot slot 4 sce 3 floor 4, x -3300..-500 z -9900..-6400; @0x023D0 Member_set 0x12 = 0 -> FUN_8004116c
        #    @0x80041228 sb a2,130(a0)): Spielerband nach dem Abstieg. Flag (3,82): Zone 3 = *(0x80074664 + 12), Wort +8,
        #    Maske 0x80000000 >> 18 = 0x2000 (FUN_8004efe4).
        z3 = struct.unpack("<I", g.mem(0x80074670, 4))[0]
        def f382():
            return 1 if (struct.unpack("<I", g.mem(z3 + 8, 4))[0] & 0x2000) else 0
        L("C  Lift: Zone-3-Basis %08x, (3,82) = %d" % (z3, f382()))
        for _ in range(2):
            g.memw(PL + 0x34, struct.pack("<i", -1900))
            g.memw(PL + 0x38, struct.pack("<i", -7200))
            g.memw(PL + 0x3C, struct.pack("<i", -8150))
            g.memw(PL + 0x6A, struct.pack("<h", 0))
            g.memw(PL + 0x82, bytes([4]))
            lauf(0.5)
        L("C  VOR: Spieler %s, (3,82) = %d" % (pl(), f382()))
        lauf(0.25, B.XUSB_GAMEPAD_X)
        for k in range(8):
            lauf(1.0)
            L("  C  t=%d s: Spieler %s, (3,82) = %d" % (k + 1, pl(), f382()))
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
