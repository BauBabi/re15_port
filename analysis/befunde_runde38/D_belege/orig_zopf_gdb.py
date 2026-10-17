"""orig_zopf_gdb.py - Runde 38 Spur D "zopf": Elzas Zopf am ORIGINAL RE1.5 (DuckStation, MZD-Disc = Auslieferungs-EXE,
Stub @0x80026e4c 0800e003) ueber den GDB-Server (127.0.0.1:2345) messen. Dossier analysis/befunde_runde38/D_zopf.md §4.6.

Ablauf:
  1. Spielstand stage_saves/mzd_debugmenu.sav (Debug-Menue, Leon, ROOM1240). Per RAM: angeforderter PL-Index
     0x800b0ff0 = 4 (Raumlader FUN_800396fc vergleicht @0x80039768-770 und laedt bei Ungleichheit das Spielermodell
     neu, jal 0x800314b0 @0x80039788) und Elza-Bit 0x800aca3c |= 0x80000000 (Raumdatei = Basis + Elza-Bit @0x800397e4-ec).
  2. Debug-JUMP (Pad: Up x2, Down, Left x18 = 0x124 -> 0x112, Square) -> ROOM1121 mit PL04.
  3. Pruefen: Charakter-Byte 0x800aca5c & 0xf == 4, Teile-Zahl Spieler +0x83 == 21, Flags Teil 15..20 (+0x00).
  4. Haltepunkte  BP_IN  @0x80024c30  Eintritt Zopf-Treiber (a0 = Spieler)  -> Eingang + Glieder VORHER
                  BP_OUT @0x80024e10  nach dem 4. FUN_80024e40 (jal @0x80024e08) -> Glieder NACHHER + Teil 8 +0x40
     Phasen (je Bild ein BP_IN/BP_OUT-Paar): ruhe 15 | drehung R 30 | nachlauf 90 | gehen U 40 | stehen 60.
  5. Zurueck auf (2500,10000) Blick 2048, 5 s laufen lassen, geordnet schliessen (SaveStateOnExit) -> Bild aus dem VRAM.
Ausgabe <ordner>/frames.txt (eine Zeile je Bild, Felder s.u.), log.txt, resume.sav.
Aufruf: python orig_zopf_gdb.py <ausgabeordner>
settings.ini wird gesichert und am Ende zurueckgeschrieben (EnableGDBServer = false)."""
import glob, os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
BAUM = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(BAUM, "analysis", "befunde_runde36", "R_belege"))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT   # noqa: E402

STATE = os.path.join(BAUM, "stage_saves", "mzd_debugmenu.sav")
PL = 0x800ACA54
TEILE_ZEIGER = PL + 0x188          # lw v1,0x800acbdc @0x80031b38
PL_INDEX = 0x800B0FF0              # angeforderter PL-Index (lh @0x80039768)
ELZA_BIT = 0x800ACA3C              # Bit 31 (srl a0,a0,31 @0x800397e4)
CHAR = 0x800ACA5C                  # Charakter-Byte (andi 0x4 @0x80031d8c)
NACKEN = 0x800ACC0C                # Spieler +0x1b8 (lbu @0x80024ce0)
PAD = 0x800AC768                   # Pad gehalten
BP_IN, BP_OUT = 0x80024C30, 0x80024E10
TS = 0xAC                          # Teil-Stride (FUN_8001e56c)


def s16(b, o): return struct.unpack_from("<h", b, o)[0]
def u16(b, o): return struct.unpack_from("<H", b, o)[0]
def s32(b, o): return struct.unpack_from("<i", b, o)[0]
def u32(b, o): return struct.unpack_from("<I", b, o)[0]


def matrix(b, o):
    """MATRIX: short m[3][3] @+0, pad, long t[3] @+0x14."""
    return [s16(b, o + 2 * i) for i in range(9)], [s32(b, o + 0x14 + 4 * i) for i in range(3)]


def glied(t):
    """Felder, die FUN_80024e40 liest/schreibt (Teil t, 0xac Bytes)."""
    off = [s32(t, 0x2C), s32(t, 0x30), s32(t, 0x34)]
    m, w = matrix(t, 0x40)
    rot = [s16(t, 0x60), s16(t, 0x62), s16(t, 0x64)]
    return off, m, w, rot, s16(t, 0x94), s16(t, 0x96)


def main():
    out = os.path.abspath(sys.argv[1])
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")
    fr = open(os.path.join(out, "frames.txt"), "w")

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

    def tap(btn, hold=0.08, gap=0.45):
        pad(btn)
        time.sleep(hold)
        pad()
        time.sleep(gap)

    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    eigene = set()
    sav_vor = set(glob.glob(os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates\*_resume.sav")))
    t_start = time.time()
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
        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")

        def zustand(tag):
            p = g.mem(PL, 0x1C0)
            ch = g.mem(CHAR, 1)[0]
            st, rm = s16(g.mem(0x800B0FE0, 2), 0), s16(g.mem(0x800B0FE2, 2), 0)
            L("%s: Stage %d Raum 0x%02x cut %d | char 0x%02x PL-Index %d Elza-Bit %d | Teile +0x83 = %d @%08x | "
              "Lage (%d,%d,%d) Blick %d" % (tag, st, rm, s16(g.mem(0x800B0FE4, 2), 0), ch, s16(g.mem(PL_INDEX, 2), 0),
                                            u32(g.mem(ELZA_BIT, 4), 0) >> 31, p[0x83], u32(p, 0x188),
                                            s32(p, 0x34), s32(p, 0x38), s32(p, 0x3C), s16(p, 0x6A)))
            return p

        zustand("VOR")
        g.cmd("M%x,2:%s" % (PL_INDEX, struct.pack("<h", 4).hex()))
        a3c = u32(g.mem(ELZA_BIT, 4), 0)
        g.cmd("M%x,4:%s" % (ELZA_BIT, struct.pack("<I", a3c | 0x80000000).hex()))
        zustand("GESCHRIEBEN")
        g.cont()
        time.sleep(0.5)
        for _ in range(2):
            tap(B.XUSB_GAMEPAD_DPAD_UP, 0.06, 0.30)
        tap(B.XUSB_GAMEPAD_DPAD_DOWN, 0.06, 0.50)
        for i in range(18):
            tap(B.XUSB_GAMEPAD_DPAD_LEFT, 0.08, 0.45)
        time.sleep(0.8)
        L("Square -> Raum laden")
        tap(B.XUSB_GAMEPAD_X, 0.13, 0.6)
        time.sleep(14)
        g.stop()
        p = zustand("GELADEN")
        teile = u32(p, 0x188)
        n_teile = p[0x83]
        if n_teile < 21:
            L("Spieler hat %d Teile (Elza PL04 = 21) - Abbruch" % n_teile)
            return 3
        fl = g.mem(teile, TS * 21)
        L("Flags Teil 15..20:", ["0x%x" % u32(fl, TS * k) for k in range(15, 21)])
        for k in range(17, 21):
            off, m, w, rot, v94, v96 = glied(fl[TS * k:TS * (k + 1)])
            L("  Teil %d: Versatz %s Welt t %s rot %s v %d/%d Elter-Zeiger %08x" % (k, off, w, rot, v94, v96,
                                                                                u32(fl, TS * k + 0x6C)))
        # Blick 2048 (Ruecken zur Kamera wie integration_r38_zopf), Lage pruefen
        g.cmd("M%x,2:%s" % (PL + 0x6A, struct.pack("<h", 2048).hex()))
        g.cont()
        time.sleep(4.0)
        g.stop()
        zustand("EINGESCHWUNGEN")
        g.cmd("Z0,%x,4" % BP_IN)
        g.cmd("Z0,%x,4" % BP_OUT)
        bild = [0]

        def phase(name, btns, n_bilder):
            pad(*btns)
            n = 0
            vorher = None
            g.cont()
            t0 = time.time()
            while n < n_bilder and time.time() - t0 < 300:
                r = g.wait_stop(5.0)
                if r is None:
                    g.stop()
                    L("  (kein Haltepunkt in 5 s)")
                    g.cont()
                    continue
                rg = g.regs()
                pc = rg[37] if len(rg) >= 38 else 0
                if pc == BP_IN:
                    if rg[4] != PL:
                        L("  BP_IN mit a0=%08x (nicht der Spieler) - uebergangen" % rg[4])
                    else:
                        e = g.mem(PL, 0x1C0)
                        t = g.mem(teile, TS * 21)
                        vorher = (e, t, g.mem(NACKEN, 1)[0], u32(g.mem(PAD, 4), 0))
                elif pc == BP_OUT and vorher is not None:
                    e, t, nf, pw = vorher
                    nach = g.mem(teile + TS * 8, TS)
                    nach17 = g.mem(teile + TS * 17, TS * 4)
                    t0m, t0t = matrix(t, 0x18)
                    t8m, t8t = matrix(t, TS * 8 + 0x18)
                    zeile = ["F", str(bild[0]), name, "pad=%04x" % (pw & 0xFFFF),
                             "ent", *map(str, (s16(e, 0x68), s16(e, 0x6A), s16(e, 0x6C), s32(e, 0x34), s32(e, 0x38),
                                               s32(e, 0x3C))),
                             "l0", *map(str, t0m + t0t), "l8", *map(str, t8m + t8t),
                             "a", *map(str, (s16(t, 0x62), s16(t, 0x64), s16(t, TS * 8 + 0x62), s16(t, TS * 8 + 0x94), nf))]
                    zeile.append("vor")
                    for k in range(17, 21):
                        off, m, w, rot, v94, v96 = glied(t[TS * k:TS * (k + 1)])
                        zeile += list(map(str, off + m + w + rot + [v94, v96]))
                    zeile.append("nach")
                    for k in range(4):
                        off, m, w, rot, v94, v96 = glied(nach17[TS * k:TS * (k + 1)])
                        zeile += list(map(str, m + w + rot + [v94, v96]))
                    w8m, w8t = matrix(nach, 0x40)
                    zeile += ["w8", *map(str, w8m + w8t)]
                    fr.write(" ".join(zeile) + "\n")
                    fr.flush()
                    bild[0] += 1
                    n += 1
                    vorher = None
                g.cont()
            g.stop()
            p = g.mem(PL, 0x70)
            L("Phase %s: %d Bilder, Lage (%d,%d) Blick %d" % (name, n, s32(p, 0x34), s32(p, 0x3C), s16(p, 0x6A)))

        phase("ruhe", (), 15)
        phase("drehung", (B.XUSB_GAMEPAD_DPAD_RIGHT,), 30)
        phase("nachlauf", (), 90)
        phase("gehen", (B.XUSB_GAMEPAD_DPAD_UP,), 40)
        phase("stehen", (), 60)
        pad()
        g.cmd("z0,%x,4" % BP_IN)
        g.cmd("z0,%x,4" % BP_OUT)
        # zurueck auf die Bildstelle und ausschwingen lassen
        g.cmd("M%x,4:%s" % (PL + 0x34, struct.pack("<i", 2500).hex()))
        g.cmd("M%x,4:%s" % (PL + 0x3C, struct.pack("<i", 10000).hex()))
        g.cmd("M%x,2:%s" % (PL + 0x6A, struct.pack("<h", 2048).hex()))
        g.cont()
        time.sleep(6.0)
        g.stop()
        zustand("ENDE")
        g.cont()
        time.sleep(0.5)
        try:
            g.sk.close()
        except OSError:
            pass
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        pad()
        t0 = time.time()
        while time.time() - t0 < 25 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        os.remove(bak)
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" %
          ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))
        neu = [f for f in glob.glob(os.path.expandvars(r"%LOCALAPPDATA%\DuckStation\savestates\*_resume.sav"))
               if f not in sav_vor or os.path.getmtime(f) > t_start]
        for f in neu:
            shutil.copy2(f, os.path.join(out, "resume.sav"))
            L("Spielstand beim Schliessen:", f)
        fr.close()


if __name__ == "__main__":
    sys.exit(main())
