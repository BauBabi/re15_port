"""orig_3070_eskorte.py - Runde 41 Spur H: zwei Eskorte-NPCs (Ada 0x42 + Marvin 0x40, ROOM3070 sub00 @0x0343C/@0x03450,
Verhaltensbyte 0x00 = Zustand 1, beide mit DEMSELBEN Folgepunkt 1500 hinter dem Spieler) am ORIGINAL (DuckStation,
MZD-Disc = Auslieferungs-EXE, @0x80026e4c = 0800e003).

Frage: halten sich die beiden im Original auseinander (Wurzel-Schwanz aec4/b544 @0x8011b7f0/@0x8011b7f8 bzw.
@0x8011bdc0/@0x8011bdc8 in STAGE3.BIN), oder laufen sie wie im Port vor Runde 41 ineinander?

Ablauf:
  1 Spielstand stage_saves/mzd_debugmenu.sav (JUMP-Zeile des Debug-Menues) per -statefile, GDB-Server an.
  2 Flags per RAM setzen (Bank-Tabelle 0x80074664: Bank 3 -> 0x800b0ff8, Bank 4 -> 0x800b1018; Wort = Bit >> 5,
    Maske = 0x80000000 >> (Bit & 31), u32 little endian): (4,215), (4,236), (3,180) = 1 -> sub00 waehlt den
    Eskorte-Zweig (Ck(4,215)/Ck(4,236)/Ck(3,180) @0x033CA/@0x03408/@0x03438).
  3 Debug-Menue: Up, Up, Down (JUMP-Zeile), Triangle x2 (Stage 3), Right x7 (0x307 WAREHOUSE), Square (laden).
  4 Leon steht; je 0.25 s anhalten und das Gegner-Feld 0x800acc2c (Stride 0x1F4: +0x04 Zustand, +0x08 Typ,
    +0x34/+0x3C Lage) + Spieler 0x800aca54 lesen. Danach zwei kurze Wege (UP), damit die Eskorten erneut folgen.
Ausgabe: <ordner>/log.txt (je Probe Lage beider NPCs + Abstand), Zusammenfassung min. Abstand.
Aufruf: python orig_3070_eskorte.py <ausgabeordner>. settings.ini wird gesichert und zurueckgeschrieben."""
import os, shutil, struct, subprocess, sys, time, math

R36 = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "befunde_runde36", "R_belege")
sys.path.insert(0, os.path.abspath(R36))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT, PL

REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
STATE = os.path.join(REPO, "stage_saves", "mzd_debugmenu.sav")
ENT, ENT_SZ = 0x800ACC2C, 0x1F4
BANK = {3: 0x800B0FF8, 4: 0x800B1018}
FLAGS = [(4, 215), (4, 236), (3, 180)]


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
    time.sleep(3.0)

    def tap(btn, hold=0.13, gap=0.6):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)

    def halte(btn, secs):
        gp.press_button(button=btn); gp.update(); time.sleep(secs)
        gp.release_button(button=btn); gp.update(); time.sleep(0.1)

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
        time.sleep(10)
        L("Stopp:", g.stop())
        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")

        def flags_setzen(tag):
            for bank, bit in FLAGS:
                a = BANK[bank] + 4 * (bit >> 5)
                m = 0x80000000 >> (bit & 31)
                w = struct.unpack("<I", g.mem(a, 4))[0]
                g.memw(a, struct.pack("<I", w | m))
                L("%s Flag (%d,%d): @%08x %08x -> %08x" % (tag, bank, bit, a, w, struct.unpack("<I", g.mem(a, 4))[0]))

        flags_setzen("vor dem Sprung")
        g.cont()
        time.sleep(0.5)
        for _ in range(2):
            tap(B.XUSB_GAMEPAD_DPAD_UP, hold=0.06, gap=0.30)
        tap(B.XUSB_GAMEPAD_DPAD_DOWN, hold=0.06, gap=0.50)
        g.stop()
        # Menue-Cursor direkt setzen statt Tasten zu zaehlen (Lauf 1 verlor einen Right-Druck und landete in 0x306):
        # Stage-Index 0x800BBE5E (@0x800146E4), Raumindex je Stage 0x800BBE5F + stage (@0x800146F4/0x8001470C),
        # Auswahlzeile 0x800BBE5D = 1 (JUMP, @0x80014698). Stage 2 = STAGE3, Index 7 -> 0x307 WAREHOUSE.
        L("Menue vorher: Zeile %d Stage %d Index[2] %d" % (g.mem(0x800BBE5D, 1)[0], g.mem(0x800BBE5E, 1)[0],
                                                          g.mem(0x800BBE61, 1)[0]))
        g.memw(0x800BBE5E, bytes([2]))
        g.memw(0x800BBE61, bytes([7]))
        L("Menue gesetzt: Zeile %d Stage %d Index[2] %d" % (g.mem(0x800BBE5D, 1)[0], g.mem(0x800BBE5E, 1)[0],
                                                           g.mem(0x800BBE61, 1)[0]))
        flags_setzen("in der Menue-Zeile")
        g.cont()
        tap(B.XUSB_GAMEPAD_X, gap=0.6)
        L("Square -> Laden, warte 20 s")
        time.sleep(20)

        def lage():
            p = g.mem(PL, 0x90)
            pl = (struct.unpack_from("<i", p, 0x34)[0], struct.unpack_from("<i", p, 0x3C)[0],
                  struct.unpack_from("<h", p, 0x6A)[0])
            n = g.mem(0x800ACA4E, 1)[0]
            ents = []
            for s in range(8):
                e = g.mem(ENT + ENT_SZ * s, 0x40)
                w0 = struct.unpack_from("<I", e, 0)[0]
                ents.append((s, w0 & 1, e[4], e[5], e[8], struct.unpack_from("<i", e, 0x34)[0],
                             struct.unpack_from("<i", e, 0x3C)[0], e[9]))
            return pl, n, ents

        g.stop()
        pl, n, ents = lage()
        L("nach dem Laden: Spieler", pl, "g_active_count", n, "Raum/Stage 0x800b0fe0/fe2 = %d/%d" % (
            g.mem(0x800B0FE0, 1)[0], g.mem(0x800B0FE2, 1)[0]))
        for e in ents:
            L("  Entity %d aktiv=%d +4=%d +5=%d typ=0x%02x lage=(%d,%d) +9=0x%02x" % e)
        npc = [e for e in ents if e[1] and e[4] in (0x40, 0x42)]
        if len(npc) < 2:
            L("KEINE zwei NPC 0x40/0x42 aktiv - Flags im Ladeweg nicht wirksam? Abbruch der Messung")
            g.cont()
            return 5
        ia, ib = npc[0][0], npc[1][0]
        g.cont()
        proben = []

        def probe(tag):
            g.stop()
            pl, n, ents = lage()
            a, b = ents[ia], ents[ib]
            d = math.hypot(a[5] - b[5], a[6] - b[6])
            proben.append(d)
            L("%s Spieler=(%d,%d,r%d) | E%d typ=0x%02x st=%d sub=%d (%d,%d) | E%d typ=0x%02x st=%d sub=%d (%d,%d) | d=%.0f" % (
                tag, pl[0], pl[1], pl[2], a[0], a[4], a[2], a[3], a[5], a[6], b[0], b[4], b[2], b[3], b[5], b[6], d))
            g.cont()

        t0 = time.time()
        while time.time() - t0 < 25:
            probe("STEHEN %.1fs" % (time.time() - t0))
            time.sleep(0.25)
        for k in range(2):
            halte(B.XUSB_GAMEPAD_DPAD_UP, 1.5)
            t1 = time.time()
            while time.time() - t1 < 12:
                probe("NACH WEG %d %.1fs" % (k, time.time() - t1))
                time.sleep(0.25)
        L("ZUSAMMENFASSUNG: %d Proben, min. Abstand %.0f, letzter %.0f (Radien-Summe 900)" % (
            len(proben), min(proben), proben[-1]))
        g.stop()
        g.cont()
        time.sleep(0.3)
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        gp.reset(); gp.update()
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
