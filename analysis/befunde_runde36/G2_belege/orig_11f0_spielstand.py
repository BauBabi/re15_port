"""orig_11f0_spielstand.py - Runde 36 G2: Original-Spielstand ROOM11F0 (Generator) erzeugen.
DuckStation (MZD-Disc, Auslieferungs-EXE @0x80026e4c 0800e003) mit stage_saves/mzd_debugmenu.sav (Debug-Menue offen,
JUMP 124 OPENING). Die Raumwahl wird NICHT per Pad-Tipper gefahren (gemessen diag: 0,3 s Links = -3 Raeume, Auto-Repeat),
sondern im Menue-Zustand gesetzt: Menue-Funktion @0x80014444, Auswahlzeile 0x800BBE5D (1 = JUMP, @0x80014698), Stage-Index
0x800BBE5E (@0x800146E4), Raumindex je Stage 0x800BBE5F + stage (@0x800146F4/@0x8001470C) := 0x1F -> JUMP 11F; dann
Quadrat (Kante 0x800AC762, @0x80014A38) = laden. Pruefung UEBER GDB vor dem Sichern: RDT-Basis DAT_800ac778,
SCA-Zellen im RAM == ROOM11F0.RDT @0x00874 (85 x 12 B), Cursor obj 0 im Pool-Platz 0. Erst dann geordnet schliessen
(SaveStateOnExit -> HASH-957757946319438E_resume.sav) und nach <ausgabe> kopieren.
settings.ini: gesichert, EnableGDBServer/ConfirmPowerOff/Pad-Bindungen (pad_bind_alle, Spur R: zwei XInput-Geraete) nur
fuer den Lauf, danach zurueckgeschrieben.
Aufruf: python orig_11f0_spielstand.py <ausgabe.sav> <ROOM11F0.RDT>"""
import os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "R_belege"))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT

BASIS = os.path.join(HIER, "..", "..", "..", "stage_saves", "mzd_debugmenu.sav")
RESUME = os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates\HASH-957757946319438E_resume.sav")


def main():
    ziel = os.path.abspath(sys.argv[1])
    rdt = open(sys.argv[2], "rb").read()
    sca_f = struct.unpack_from("<I", rdt, 0x20)[0]
    n_all = sum(struct.unpack_from("<5I", rdt, sca_f + 4))
    zellen_datei = rdt[sca_f + 24:sca_f + 24 + 12 * n_all]
    if pids():
        print("DuckStation laeuft bereits - Abbruch")
        return 2
    import vgamepad as vg
    B = vg.XUSB_BUTTON
    gp = vg.VX360Gamepad()
    time.sleep(2.0)

    def tap(btn, hold, gap):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)

    bak = ziel + ".settings.bak"
    shutil.copy2(INI, bak)
    eigene = set()
    ok = False
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        pad_bind_alle(INI)
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", os.path.abspath(BASIS), CUE], cwd=HIER)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        print("DuckStation gestartet", eigene, flush=True)
        g = Gdb(PORT)
        print("GDB verbunden", flush=True)
        time.sleep(10)
        print("Stopp:", g.stop(), flush=True)
        m = g.mem(0x800BBE5C, 4)
        print("Menue 0x800BBE5C..5F vorher:", m.hex(), "(Zeile %d, Stage %d, Raumindex 0x%02X)" % (m[1], m[2], m[3]))
        r = g.cmd("M%x,1:1f" % 0x800BBE5F)
        m = g.mem(0x800BBE5C, 4)
        print("Menue nachher (%s):" % r, m.hex(), "(Zeile %d, Stage %d, Raumindex 0x%02X)" % (m[1], m[2], m[3]))
        if m[1] != 1 or m[2] != 0 or m[3] != 0x1F:
            print("Menue-Zustand unerwartet - Abbruch")
            return 5
        g.cont()
        time.sleep(1.0)
        tap(B.XUSB_GAMEPAD_X, 0.13, 0.6)
        print("Quadrat (laden) gedrueckt, warte 12 s", flush=True)
        time.sleep(12)
        print("Stopp:", g.stop(), flush=True)
        print("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        base = struct.unpack("<I", g.mem(0x800AC778, 4))[0]
        sca = struct.unpack("<I", g.mem(base + 0x20, 4))[0]
        q0 = struct.unpack("<I", g.mem(sca + 4, 4))[0]
        zellen = g.mem(q0, 12 * n_all) if 0x80000000 <= q0 < 0x80200000 else b""
        ok = zellen == zellen_datei
        print("RDT-Basis %08x SCA %08x Zellen im RAM == ROOM11F0.RDT @0x%05X: %s" % (base, sca, sca_f + 24, ok))
        o = g.mem(0x800B3F98, 0x94)
        print("Pool 0: +0=%08x typ %d band %d lage (%d,%d,%d)" % (
            struct.unpack_from("<I", o, 0)[0], o[8], o[0x82], struct.unpack_from("<i", o, 0x34)[0],
            struct.unpack_from("<i", o, 0x38)[0], struct.unpack_from("<i", o, 0x3C)[0]))
        g.cont()
        time.sleep(1.0)
        if not ok:
            print("FALSCHER RAUM - kein Spielstand")
            for p in eigene:
                subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
            return 3
        m0 = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        t0 = time.time()
        while time.time() - t0 < 30 and (pids() & eigene):
            time.sleep(0.5)
        for _ in range(30):
            if os.path.exists(RESUME) and os.path.getmtime(RESUME) > m0:
                break
            time.sleep(0.5)
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > m0:
            shutil.copy2(RESUME, ziel)
            print("Spielstand ->", ziel, os.path.getsize(ziel))
            return 0
        print("SaveStateOnExit hat nicht geschrieben")
        return 4
    finally:
        t0 = time.time()
        while time.time() - t0 < 10 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        os.remove(bak)
        print("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" %
              ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))


if __name__ == "__main__":
    sys.exit(main())
