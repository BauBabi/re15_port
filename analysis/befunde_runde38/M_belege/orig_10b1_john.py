"""orig_10b1_john.py - Runde 38 Spur M: John (Typ 0x49) in ROOM10B1 (Elza) am ORIGINAL messen.

DuckStation (MZD-Disc, Auslieferungs-EXE-Pruefung @0x80026e4c == 0800e003) mit stage_saves/mzd_debugmenu.sav
(Debug-Menue offen). Ueber den GDB-Server (127.0.0.1:2345):
  - Code-Gleichheit RAM == info/Re1.5/PSX.EXE fuer Sonde 0x8002d7d8, Subs 4/5 0x80051148-0x800517f0,
    Kanten-/Sprung-Sub 0x80052508-0x80052fd0 (die MZD-Disc koennte Code patchen -> erst pruefen)
  - Debug-Menue: Raumindex Stage 0 (0x800BBE5F) := 0x0B (JUMP 10B), Elza-Bit 0x800ACA3C |= 0x80000000 (RDT-Wahl
    `srl a0,a0,31` @0x800397e4 -> ROOM10B1), Flags (3,0x80)(3,0x82) (Bank 3 = 0x800B0FF8 aus der Tabelle @0x80074664,
    Bit 0x80 -> Wort 4, Maske 0x80000000 wie Ck @0x8003fd10-40) = Stand nach ROOM1171 sub02
  - Quadrat = laden (Kante 0x800AC762 & 0x80 @0x80014a38); danach Raum pruefen (SCA + SCD im RAM == ROOM10B1.RDT)
  - Spieler 0x800ACA54 auf den Band-5-Steg (0,-9000,-12000) setzen (Cut 2 wie im Port-Lauf), damit sub02 @0x01888
    `While(Cmp(0x0A != 2))` John weiterlaufen laesst
  - John = Entity 0 = 0x800ACC2C (Stride 0x1F4, FUN_8001a50c) je ~0,1 s abtasten: +0x4/+0x5/+0x6, Lage, Band +0x82,
    Clip +0x94/+0x95, Sonde +0x9f, +0x1ba, +0x1c0
settings.ini: gesichert, EnableGDBServer/ConfirmPowerOff/SaveStateOnExit/Pad-Bindungen nur fuer den Lauf, danach
zurueckgeschrieben.
Aufruf: python orig_10b1_john.py <ausgabe.log> <PSX.EXE> <ROOM10B1.RDT> [sekunden]"""
import os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "..", "befunde_runde36", "R_belege"))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT

BASIS = os.path.join(HIER, "..", "..", "..", "stage_saves", "mzd_debugmenu.sav")
if not os.path.exists(BASIS):
    BASIS = r"C:\workspace\git\reAi_v2\stage_saves\mzd_debugmenu.sav"
PL, JOHN = 0x800ACA54, 0x800ACC2C


def main():
    aus = os.path.abspath(sys.argv[1])
    exe = open(sys.argv[2], "rb").read()
    rdt = open(sys.argv[3], "rb").read()
    dauer = float(sys.argv[4]) if len(sys.argv) > 4 else 90.0
    t_addr = struct.unpack_from("<I", exe, 0x18)[0]
    def exe_bytes(a, n):
        o = 0x800 + a - t_addr
        return exe[o:o + n]
    lg = open(aus, "w", encoding="utf-8")
    def L(*a):
        s = " ".join(str(x) for x in a)
        print(s, flush=True)
        lg.write(s + "\n"); lg.flush()
    if pids():
        L("DuckStation laeuft bereits - Abbruch")
        return 2
    import vgamepad as vg
    B = vg.XUSB_BUTTON
    gp = vg.VX360Gamepad()
    time.sleep(2.0)
    def tap(btn, hold, gap):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)
    bak = aus + ".settings.bak"
    shutil.copy2(INI, bak)
    eigene = set()
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        ini_set(INI, "Main", "SaveStateOnExit", "false")
        pad_bind_alle(INI)
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(BASIS), CUE], cwd=HIER)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        L("DuckStation gestartet", eigene)
        g = Gdb(PORT)
        L("GDB verbunden")
        time.sleep(10)
        L("Stopp:", g.stop())
        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        for a, e in ((0x8002D7D8, 0x8002DA4C), (0x80051148, 0x800517F0), (0x80052508, 0x80052FD0),
                     (0x8001BD60, 0x8001BF00)):
            ram = g.mem(a, e - a)
            L("Code %08x-%08x RAM == PSX.EXE: %s" % (a, e, ram == exe_bytes(a, e - a)))
        m = g.mem(0x800BBE5C, 4)
        L("Menue 0x800BBE5C..5F vorher:", m.hex())
        g.cmd("M%x,1:0b" % 0x800BBE5F)
        w = struct.unpack("<I", g.mem(0x800ACA3C, 4))[0]
        g.cmd("M%x,4:%s" % (0x800ACA3C, struct.pack("<I", w | 0x80000000).hex()))
        for wa in (0x800B1008,):   # (3,0x80) Wort 4, Maske 0x80000000; (3,0x82) Wort 4, Maske 0x20000000
            v = struct.unpack("<I", g.mem(wa, 4))[0] | 0x80000000 | 0x20000000
            g.cmd("M%x,4:%s" % (wa, struct.pack("<I", v).hex()))
        L("Menue nachher:", g.mem(0x800BBE5C, 4).hex(), "aca3c:", g.mem(0x800ACA3C, 4).hex(),
          "Bank3 Wort4:", g.mem(0x800B1008, 4).hex())
        g.cont()
        time.sleep(1.0)
        tap(B.XUSB_GAMEPAD_X, 0.13, 0.6)
        L("Quadrat (laden) gedrueckt, warte 12 s")
        time.sleep(12)
        L("Stopp:", g.stop())
        base = struct.unpack("<I", g.mem(0x800AC778, 4))[0]
        sca = struct.unpack("<I", g.mem(base + 0x20, 4))[0]
        sca_f = struct.unpack_from("<I", rdt, 0x20)[0]
        n_all = sum(struct.unpack_from("<5I", rdt, sca_f + 4))
        q0 = struct.unpack("<I", g.mem(sca + 4, 4))[0]
        zellen = g.mem(q0, 12 * n_all) if 0x80000000 <= q0 < 0x80200000 else b""
        L("RDT-Basis %08x SCA-Zellen == ROOM10B1.RDT: %s" % (base, zellen == rdt[sca_f + 24:sca_f + 24 + 12 * n_all]))
        sub_f = struct.unpack_from("<I", rdt, 0x44)[0]
        sub_r = struct.unpack("<I", g.mem(base + 0x44, 4))[0]
        n_sub = 0x1AA0 + 0x40 - sub_f
        sc = g.mem(sub_r, n_sub) if 0x80000000 <= sub_r < 0x80200000 else b""
        L("Sub-SCD %08x == ROOM10B1.RDT @0x%05X (%d B): %s" % (sub_r, sub_f, n_sub, sc == rdt[sub_f:sub_f + n_sub]))
        L("aca3c nach dem Laden:", g.mem(0x800ACA3C, 4).hex(), "Bank3 Wort4:", g.mem(0x800B1008, 4).hex())
        p = g.mem(PL, 0x84)
        L("Spieler vorher: (%d,%d,%d) Band %d" % (struct.unpack_from("<i", p, 0x34)[0], struct.unpack_from("<i", p, 0x38)[0],
                                                  struct.unpack_from("<i", p, 0x3C)[0], p[0x82]))
        g.cmd("M%x,c:%s" % (PL + 0x34, struct.pack("<iii", 0, -9000, -12000).hex()))
        g.cmd("M%x,6:%s" % (PL + 0x40, struct.pack("<hhh", 0, -9000, -12000).hex()))
        g.cmd("M%x,1:05" % (PL + 0x82))
        g.cmd("M%x,2:%s" % (PL + 0x6A, struct.pack("<H", 2048).hex()))
        # Cut 2 ANFORDERN wie der RVD-Scan (DAT_800afbb5 @0x80014300): der Per-Bild-Vergleich @0x800214f8-14
        # (Var 0x0A @0x800b0fe4 != DAT_800afbb5 -> DAT_800b5457 = 1) schaltet dann ueber den eigenen Apply um.
        # (Ein Teleport allein schaltet nicht: die Zonen gehoeren zur AKTUELLEN Kamera-Gruppe DAT_800ac794.)
        g.cmd("M%x,1:02" % 0x800AFBB5)
        p = g.mem(PL, 0x84)
        L("Spieler gesetzt: (%d,%d,%d) Band %d" % (struct.unpack_from("<i", p, 0x34)[0], struct.unpack_from("<i", p, 0x38)[0],
                                                   struct.unpack_from("<i", p, 0x3C)[0], p[0x82]))
        g.cont()
        t0 = time.time()
        vorige = None
        n = 0
        while time.time() - t0 < dauer:
            time.sleep(0.05)
            g.stop()
            j = g.mem(JOHN, 0x1C4)
            pl = g.mem(PL + 0x34, 12)
            cut = struct.unpack("<h", g.mem(0x800B0FE4, 2))[0]   # SCD-Variable 0x0A (Cmp @0x8003ff94: 0x800b0fd0 + 2*var)
            g.cont()
            n += 1
            z = (j[4], j[5], j[6], j[8], struct.unpack_from("<i", j, 0x34)[0], struct.unpack_from("<i", j, 0x38)[0],
                 struct.unpack_from("<i", j, 0x3C)[0], j[0x82], j[0x94], j[0x95], struct.unpack_from("<b", j, 0x9F)[0],
                 struct.unpack_from("<h", j, 0x1BA)[0], struct.unpack_from("<H", j, 0x1C0)[0],
                 struct.unpack_from("<H", j, 0x6A)[0])
            L("t=%6.2f st=%d s1=0x%02x s2=%d typ=%02x x=%d y=%d z=%d b=%d mo=%d af=%d 9f=%d fy=%d 1c0=%04x r=%d | PL %d,%d,%d cut %d" % (
                (time.time() - t0,) + z + struct.unpack("<iii", pl) + (cut,)))
        L("Stichproben:", n)
        return 0
    finally:
        for pp in pids() & eigene:
            subprocess.run(["taskkill", "/PID", pp], capture_output=True)
        t0 = time.time()
        while time.time() - t0 < 15 and (pids() & eigene):
            time.sleep(0.5)
        for pp in pids() & eigene:
            subprocess.run(["taskkill", "/PID", pp, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        os.remove(bak)
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" %
          ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))
        lg.close()


if __name__ == "__main__":
    sys.exit(main())
