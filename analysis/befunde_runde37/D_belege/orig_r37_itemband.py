"""orig_r37_itemband.py - Runde 37 Spur D: Band-Gate der Item-Saetze + Band-Wege am ORIGINAL messen.

DuckStation (MZD-Disc, Auslieferungs-EXE @0x80026e4c 0800e003) mit stage_saves/mzd_debugmenu.sav. Der Raum wird wie in
G2_belege/orig_11f0_spielstand.py NICHT per Pad-Tipper gewaehlt, sondern im Menue-Zustand gesetzt (Menue-Funktion
@0x80014444: Zeile 0x800BBE5D = 1 JUMP, Stage-Index 0x800BBE5E, Raumindex 0x800BBE5F + stage), dann Quadrat = laden.
Pruefung: SCA-Zellen im RAM == ROOMxxxx.RDT. Danach im SELBEN Lauf messen (kein Spielstand noetig):
  Spieler-Entity 0x800ACA54 (+0x34/+0x38/+0x3C Lage, +0x6A Blick, +0x82 Band = 0x800ACAD6), Gegner aus (Aktiv-Bit
  Entity 0x800ACC2C + 0x1F4*k, g_active_count 0x800ACA4E, wie R_belege/orig_2050_wiederkehr.py),
  Taken-Bit n der Bank 9 = *(*(0x80074688) + 4*(n>>5)) & (0x80000000 >> (n&31))  (FUN_8004efe4 @0x8004efe4).
Plaene:
  3010  A  Band 0 UNTER dem Steg (-10700,0,844) Blick 0 -> 3x Quadrat -> Taken 20 (SHOTGUN SHELLS slot 5 @0x02094 floor 1)
        C  Klettern: (-10322,0,-11175) Blick 3072 Band 0 -> Quadrat -> nach 4 s y/Band (Original-Klettern @0x80038200-08)
        B  Band 1 AUF dem Steg (-10700,-1800,844) Blick 0 -> 3x Quadrat -> Taken 20
  2030  L  Lift A: (6600,-5400,10900) Blick 3072 Band 3 -> Quadrat -> nach 9 s y/Band (sub02 @0x01E66 Member 0x12 = 6)
        S3 Band 3 bei der SPAS-12 (-650,-5400,14500) Blick 0 -> 3x Quadrat -> Taken 109 (slot 11 @0x01BD6 floor 6)
        S6 Band 6 dort (-650,-10800,14500) -> Taken 109
        T  Treppe slot 8 @0x01B8E: (-6350,-5400,-7700) Blick 0 Band 3 -> Quadrat -> nach 5 s Lage/Band (West-Haelfte = Abstieg)
        G3 Band 3 bei den Schrotpatronen (4300,-5400,7100) Blick 0 -> Taken 108 (slot 10 @0x01BC0 floor 1)
        G1 Band 1 dort (4300,-1800,7100) -> Taken 108
settings.ini: gesichert, EnableGDBServer/ConfirmPowerOff/Pad-Bindungen nur fuer den Lauf, danach zurueckgeschrieben.
Aufruf: python orig_r37_itemband.py <ausgabeordner> 3010|2030|2030L
  2030L L  wie 2030 L, aber (3,126) (Lift-Strom) vor dem Laden gesetzt -> Fahrt + Band; S6L SPAS-12 danach"""
import os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
RB = os.path.join(HIER, "..", "..", "befunde_runde36", "R_belege")
sys.path.insert(0, RB)
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL

BAUM = os.path.join(HIER, "..", "..", "..")
BASIS = os.path.join(BAUM, "stage_saves", "mzd_debugmenu.sav")
if not os.path.exists(BASIS):
    BASIS = r"C:\workspace\git\reAi_v2\stage_saves\mzd_debugmenu.sav"
ENT, ENT_SZ = 0x800ACC2C, 0x1F4
ZONE9_PTR = 0x80074688

RAEUME = {   # Plan -> (Stage-Index 0-basiert, Raumindex, RDT)
    "3010": (2, 0x01, "STAGE3/ROOM3010.RDT"),
    "2030": (1, 0x03, "STAGE2/ROOM2030.RDT"),
    "2030L": (1, 0x03, "STAGE2/ROOM2030.RDT"),   # wie 2030, aber (3,126) = Strom VOR dem Laden gesetzt (Lift A)
}


class Gdb2(Gdb):
    def memw(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK":
            raise IOError("M %08x -> %r" % (addr, r))


def main():
    out = os.path.abspath(sys.argv[1])
    plan = sys.argv[2]
    stage, raum, rdt_rel = RAEUME[plan]
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")

    def L(*a):
        s = " ".join(str(x) for x in a)
        print(s, flush=True)
        log.write(s + "\n")
        log.flush()

    rdt = open(os.path.join(BAUM, "re15_port", "shared_assets", "PSX", rdt_rel), "rb").read()
    sca_f = struct.unpack_from("<I", rdt, 0x20)[0]
    n_all = sum(struct.unpack_from("<5I", rdt, sca_f + 4))
    zellen_datei = rdt[sca_f + 24:sca_f + 24 + 12 * n_all]
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

    def tap(btn, hold, gap):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)

    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    eigene = set()
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        pad_bind_alle(INI)
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(BASIS), CUE], cwd=out)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        L("DuckStation gestartet", eigene)
        g = Gdb2(PORT)
        L("GDB verbunden")
        time.sleep(10)
        L("Stopp:", g.stop())
        m = g.mem(0x800BBE5C, 8)
        L("Menue 0x800BBE5C.. vorher:", m.hex())
        if plan == "2030L":
            # (3,126) = Lift-Strom (Set @ROOM2060 sub @0x012AE); sub01 @0x01E14 prueft es beim Raumaufbau. Zone 3 =
            # *(0x80074664 + 3*4), Bit 126 = Wort +12, Maske 0x80000000 >> 30 (FUN_8004efe4).
            z3 = struct.unpack("<I", g.mem(0x80074670, 4))[0]
            w = struct.unpack("<I", g.mem(z3 + 12, 4))[0]
            g.memw(z3 + 12, struct.pack("<I", w | 0x2))
            L("(3,126) gesetzt: Zone 3 %08x Wort +12 %08x -> %08x" % (z3, w, struct.unpack("<I", g.mem(z3 + 12, 4))[0]))
        g.memw(0x800BBE5E, bytes([stage]))
        g.memw(0x800BBE5F + stage, bytes([raum]))
        m = g.mem(0x800BBE5C, 8)
        L("Menue nachher:", m.hex(), "(Zeile %d, Stage %d, Raumindex[stage] 0x%02X)" % (m[1], m[2], m[3 + stage]))
        if m[1] != 1 or m[2] != stage or m[3 + stage] != raum:
            L("Menue-Zustand unerwartet - Abbruch")
            return 5
        g.cont()
        time.sleep(1.0)
        tap(B.XUSB_GAMEPAD_X, 0.13, 0.6)
        L("Quadrat (laden) gedrueckt, warte 20 s")
        time.sleep(20)
        L("Stopp:", g.stop())
        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        base = struct.unpack("<I", g.mem(0x800AC778, 4))[0]
        sca = struct.unpack("<I", g.mem(base + 0x20, 4))[0]
        q0 = struct.unpack("<I", g.mem(sca + 4, 4))[0]
        zellen = g.mem(q0, 12 * n_all) if 0x80000000 <= q0 < 0x80200000 else b""
        ok = zellen == zellen_datei
        L("RDT-Basis %08x SCA %08x Zellen im RAM == %s @0x%05X: %s" % (base, sca, rdt_rel, sca_f + 24, ok))
        if not ok:
            L("FALSCHER RAUM - Abbruch")
            return 3

        def pl():
            e = g.mem(PL, 0x90)
            return {"x": struct.unpack_from("<i", e, 0x34)[0], "y": struct.unpack_from("<i", e, 0x38)[0],
                    "z": struct.unpack_from("<i", e, 0x3C)[0], "blick": struct.unpack_from("<h", e, 0x6A)[0],
                    "band": e[0x82], "sub": e[0x05], "st": e[0x04]}

        def taken(n):
            b9 = struct.unpack("<I", g.mem(ZONE9_PTR, 4))[0]
            w = struct.unpack("<I", g.mem(b9 + 4 * (n >> 5), 4))[0]
            return 1 if (w & (0x80000000 >> (n & 31))) else 0

        def lauf(sek, *btns):
            pad(*btns)
            g.cont()
            time.sleep(sek)
            g.stop()
            pad()

        def setze(x, y, z, blick, band):
            g.memw(PL + 0x34, struct.pack("<i", x))
            g.memw(PL + 0x38, struct.pack("<i", y))
            g.memw(PL + 0x3C, struct.pack("<i", z))
            g.memw(PL + 0x6A, struct.pack("<h", blick))
            g.memw(PL + 0x82, bytes([band]))

        def gegner_weg():
            n_akt = g.mem(0x800ACA4E, 1)[0]
            weg = 0
            for sidx in range(8):
                a = ENT + ENT_SZ * sidx
                w0 = struct.unpack("<H", g.mem(a, 2))[0]
                e8 = g.mem(a + 8, 1)[0]
                if (w0 & 1) and e8 != 0:
                    g.memw(a, struct.pack("<H", w0 & 0xFFFE))
                    weg += 1
                    L("  Entity %d @%08x typ 0x%02x -> Aktiv-Bit 0 geloescht" % (sidx, a, e8))
            g.memw(0x800ACA4E, bytes([max(0, n_akt - weg)]))
            L("  g_active_count @0x800aca4e: %d -> %d" % (n_akt, g.mem(0x800ACA4E, 1)[0]))

        def aufnahme(tag, x, y, z, blick, band, bit):
            for _ in range(2):
                setze(x, y, z, blick, band)
                lauf(0.5)
            setze(x, y, z, blick, band)
            L("%s VOR: Spieler %s, Taken-Bit %d = %d" % (tag, pl(), bit, taken(bit)))
            for k in range(3):
                lauf(0.25, B.XUSB_GAMEPAD_X)
                lauf(1.5)
                L("  %s nach Druck %d: Spieler %s, Taken-Bit %d = %d" % (tag, k + 1, pl(), bit, taken(bit)))
            L("%s ERGEBNIS: Taken-Bit %d = %d" % (tag, bit, taken(bit)))
            lauf(1.0)

        def bandweg(tag, x, y, z, blick, band, sek):
            for _ in range(2):
                setze(x, y, z, blick, band)
                lauf(0.5)
            setze(x, y, z, blick, band)
            L("%s VOR: Spieler %s" % (tag, pl()))
            lauf(0.25, B.XUSB_GAMEPAD_X)
            for k in range(sek):
                lauf(1.0)
                L("  %s t=%d s: Spieler %s" % (tag, k + 1, pl()))

        L("Spieler START:", pl())
        gegner_weg()
        lauf(1.2, B.XUSB_GAMEPAD_DPAD_RIGHT)      # Pad-Probe: der virtuelle Pad kommt erst nach dem ersten Druck an
        L("Spieler nach Pad-Probe:", pl())
        gegner_weg()
        if plan == "3010":
            aufnahme("A  (Band 0 unter dem Steg)", -10700, 0, 844, 0, 0, 20)
            bandweg("C  (Klettern an r0[4])", -10322, 0, -11175, 3072, 0, 4)
            aufnahme("B  (Band 1 auf dem Steg)", -10700, -1800, 844, 0, 1, 20)
        elif plan == "2030L":
            bandweg("L  (Lift A, sub02, Strom an)", 6600, -5400, 10900, 3072, 3, 10)
            aufnahme("S6L (nach dem Lift: SPAS-12 auf Band 6)", -650, -10800, 14500, 0, 6, 109)
        else:
            bandweg("L  (Lift A, sub02)", 6600, -5400, 10900, 3072, 3, 9)
            aufnahme("S3 (Band 3 bei der SPAS-12)", -650, -5400, 14500, 0, 3, 109)
            aufnahme("S6 (Band 6 bei der SPAS-12)", -650, -10800, 14500, 0, 6, 109)
            bandweg("T  (Treppe slot 8, West-Haelfte)", -6350, -5400, -7700, 0, 3, 6)
            aufnahme("G3 (Band 3 bei den Schrotpatronen)", 4300, -5400, 7100, 0, 3, 108)
            aufnahme("G1 (Band 1 bei den Schrotpatronen)", 4300, -1800, 7100, 0, 1, 108)
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
