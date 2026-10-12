"""orig_burst.py - Runde 37 Spur G: der Feuerstoss der Beretta M93R (5) / Glock 18 (6) im ORIGINAL (DuckStation-GDB,
MZD-Disc mit Auslieferungs-EXE; Pruefung: @0x80026e4c == 08 00 e0 03), zum Vergleich die Browning HP (3).
Spielstand stage_saves/equip_test.sav (ROOM1140, Leon zielt bereits: 0x800aca5a = 1, Browning angelegt 0x800aca5d = 3,
fuenf RE1.5-Zombies an den Fresser-Plaetzen; RE1.5-Kandidatenschleife gatet nur +0x93 & 3, die Fresser sind treffbar).
Ablauf (KEIN virtuelles Pad - das XInput-Geraet waere global und stoerte fremde exe-Laeufe):
  * Haltepunkt @0x80020c90 (Ruecksprung aus der Pad-Aufbereitung FUN_80030444, `jal 0x80030444` @0x80020c88):
    je Bild das Haltewort 0x800ac768 und das Flankenwort 0x800ac76c UEBERSCHREIBEN: immer R1 (0x100, das Zielen;
    Sub 2 prueft `andi v0,a0,0x100` @0x80033600), im Abzugsbild zusaetzlich 0x40 (Feuer; Sub 1 `andi v0,v0,0x40`
    @0x80033308, Flanke @0x80033340).
  * vor dem ersten Bild: 0x800aca5d := Waffe; Leon auf (x,z) mit Blick b (+0x34/+0x3c s32, +0x40/+0x44 s16, +0x6a).
  * Haltepunkte (je Halt eine Zeile mit Bild, VSync 0x800787dc, Waffe, Munition des Platzes, HP aller Zombies):
      @0x800338a8  Entlade-Handler Feuerstoss 5/6        @0x800337bc  Entlade-Handler Browning 3/4
      @0x80011f50  Schuss-Resolver (Eintritt; a0 = Waffe)  @0x80017b38  Routine 15: Kind-Spawn (a0 = Kind-Code)
      @0x8004eae4  Munition -1 (FUN_8004eae4)
Aufruf: python orig_burst.py <ausgabeordner> <waffe> [--blick B] [--ziel e,x,z] [--abzug N] [--halte N] [--abzug2 N]
settings.ini wird gesichert und am Ende zurueckgeschrieben (EnableGDBServer = false)."""
import os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
BAUM = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
sys.path.insert(0, os.path.join(BAUM, "analysis", "befunde_runde36", "R_belege"))
from orig_1090_push import Gdb, ini_set, pids, DUCK, CUE, INI, PORT   # noqa: E402

STATE = os.path.join(BAUM, "stage_saves", "equip_test.sav")
PL = 0x800ACA54
ENT, ENT_SZ = 0x800ACC2C, 0x1F4
BP_PAD, BP_BURST, BP_BROWN, BP_RES, BP_R15, BP_AMMO = 0x80020C90, 0x800338A8, 0x800337BC, 0x80011F50, 0x80017B38, 0x8004EAE4
BP_WIN, BP_LOS, BP_RET5, BP_RET3 = 0x80012160, 0x80012170, 0x80033974, 0x80033888
NAMEN = {BP_BURST: "ENTLADE-BURST(0x800338a8)", BP_BROWN: "ENTLADE-BROWNING(0x800337bc)",
         BP_RES: "RESOLVER(0x80011f50)", BP_R15: "R15-KIND(0x80017b38)", BP_AMMO: "MUNITION-1(0x8004eae4)",
         BP_WIN: "RESOLVER-GEWINNER(0x80012160 s1)", BP_LOS: "SICHTLINIE(0x80012170 v0)",
         BP_RET5: "NACH-RESOLVE-BURST(0x80033974 v0)", BP_RET3: "NACH-RESOLVE-BROWNING(0x80033888 v0)"}
ALLE_BP = (BP_PAD, BP_BURST, BP_BROWN, BP_RES, BP_R15, BP_AMMO, BP_WIN, BP_LOS, BP_RET5, BP_RET3)


class Gdb2(Gdb):
    def memw(s, addr, data):
        r = s.cmd("M%x,%x:%s" % (addr, len(data), data.hex()))
        if r != b"OK":
            raise IOError("M %08x -> %r" % (addr, r))

    def zset(s, addr):
        r = s.cmd("Z0,%x,4" % addr)
        if r != b"OK":
            raise IOError("Z0 %08x -> %r" % (addr, r))


def main():
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument("out"); ap.add_argument("waffe", type=int)
    ap.add_argument("--leon", default=None, help="x,z (Lage; im Original nicht haltbar - wird zurueckgesetzt)")
    ap.add_argument("--blick", type=int, default=None, help="Blick (+0x6a) in den Halte-Bildern")
    ap.add_argument("--ziel", default=None, help="e,x,z: Zombie e auf (x,z) (+0x34/+0x3c s32, +0x40/+0x44 s16)")
    ap.add_argument("--abzug", type=int, default=40); ap.add_argument("--halte", type=int, default=30)
    ap.add_argument("--abzug2", type=int, default=0, help="zweiter Abzug (Bild), 0 = keiner")
    A = ap.parse_args()
    out = os.path.abspath(A.out); waffe = A.waffe; tap = A.abzug; halte = A.halte
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
    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    eigene = set()
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", STATE, CUE], cwd=out)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        L("DuckStation gestartet", eigene, "Spielstand", STATE)
        g = Gdb2(PORT)
        L("GDB verbunden")
        time.sleep(6)
        L("Stopp:", g.stop())
        stub = g.mem(0x80026E4C, 4).hex()
        L("Auslieferungs-EXE? @0x80026e4c =", stub, "(erwartet 0800e003)")
        exe = open(os.path.join(BAUM, "info", "Re1.5", "PSX.EXE"), "rb").read()
        t_addr = struct.unpack_from("<I", exe, 0x18)[0]
        for a, n in ((0x800338A8, 0xFC), (0x80033460, 0x100), (0x80017AC8, 0xB8), (0x80011F50, 0x80)):
            ram = g.mem(a, n)
            dat = exe[0x800 + a - t_addr: 0x800 + a - t_addr + n]
            L("Code @0x%08x..+0x%x RAM == PSX.EXE: %s" % (a, n, ram == dat))

        def ents():
            r = []
            for i in range(8):
                e = g.mem(ENT + ENT_SZ * i, 0xA0)
                if struct.unpack_from("<I", e, 0)[0] & 1:
                    r.append("e%d t%02x s%d/%d/%d/%d hp%d h93=%02x @(%d,%d)" % (
                        i, e[8], e[4], e[5], e[6], e[7], struct.unpack_from("<h", e, 0x9A)[0], e[0x93],
                        struct.unpack_from("<i", e, 0x34)[0], struct.unpack_from("<i", e, 0x3C)[0]))
            return " ".join(r)

        def mun():
            sl = g.mem(0x800B25C8, 1)[0]
            ent = g.mem(0x800B10AC + 4 * (sl & 0x7F), 4) if sl != 0x80 else b"\0\0\0\0"
            return "slot%d item%d anz%d" % (sl, ent[0], ent[1])

        def zustand():
            w = g.mem(0x800ACA58, 8)
            p = g.mem(PL, 0x90)
            return "aca58=%d aca59=%d aca5a=%d aca5b=%d aca5d=%d acae8=%d acae9=%d acaec=%04x L(%d,%d,%d r%d b%d)" % (
                w[0], w[1], w[2], w[3], w[5], g.mem(0x800ACAE8, 1)[0], g.mem(0x800ACAE9, 1)[0],
                struct.unpack("<H", g.mem(0x800ACAEC, 2))[0],
                struct.unpack_from("<i", p, 0x34)[0], struct.unpack_from("<i", p, 0x38)[0],
                struct.unpack_from("<i", p, 0x3C)[0], struct.unpack_from("<H", p, 0x6A)[0], p[0x82])

        L("vorher:", zustand(), mun())
        L("vorher:", ents())
        g.memw(0x800ACA5D, bytes([waffe]))

        def setze():
            if A.leon:
                lx, lz = [int(v) for v in A.leon.split(",")]
                g.memw(PL + 0x34, struct.pack("<i", lx)); g.memw(PL + 0x3C, struct.pack("<i", lz))
                g.memw(PL + 0x40, struct.pack("<h", lx)); g.memw(PL + 0x44, struct.pack("<h", lz))
            if A.blick is not None:
                g.memw(PL + 0x6A, struct.pack("<H", A.blick & 0xFFF))
            if A.ziel:
                ze, zx, zz = [int(v) for v in A.ziel.split(",")]
                b = ENT + ENT_SZ * ze
                g.memw(b + 0x34, struct.pack("<i", zx)); g.memw(b + 0x3C, struct.pack("<i", zz))
                g.memw(b + 0x40, struct.pack("<h", zx)); g.memw(b + 0x44, struct.pack("<h", zz))
        setze()
        L("gesetzt: aca5d=%d leon=%s blick=%s ziel=%s halte=%d abzug=%d abzug2=%d" % (
            waffe, A.leon, A.blick, A.ziel, halte, tap, A.abzug2))
        for a in ALLE_BP:
            g.zset(a)
        bild = 0
        ende = max(tap, A.abzug2) + 60
        while bild < ende:
            g.cont()
            r = g.wait_stop(20.0)
            if r is None:
                L("kein Halt in 20 s - Abbruch")
                break
            regs = g.regs()
            pc = regs[37] if len(regs) >= 38 else 0
            vs = struct.unpack("<I", g.mem(0x800787DC, 4))[0]
            if pc == BP_PAD:
                bild += 1
                if bild <= halte:
                    setze()
                feuer = (tap <= bild < tap + 2) or (A.abzug2 and A.abzug2 <= bild < A.abzug2 + 2)
                held = 0x100 | (0x40 if feuer else 0)
                edge = 0x40 if bild in (tap, A.abzug2) else 0
                g.memw(0x800AC768, struct.pack("<I", held))
                g.memw(0x800AC76C, struct.pack("<I", edge))
                if bild <= 3 or halte - 1 <= bild <= halte + 3 or tap - 1 <= bild <= tap + 12 or                         (A.abzug2 and A.abzug2 - 1 <= bild <= A.abzug2 + 12) or bild % 10 == 0:
                    L("B%-3d VS%d pad=%03x/%03x %s %s | %s" % (bild, vs, held, edge, zustand(), mun(), ents()))
            else:
                extra = ""
                if pc == BP_RES:
                    extra = "a0(waffe)=%d a1=%08x" % (regs[4], regs[5])
                if pc == BP_R15:
                    extra = "a0(kind)=%08x" % regs[4]
                if pc == BP_WIN:
                    extra = "s1(gewinner)=%08x" % regs[17]
                if pc in (BP_LOS, BP_RET5, BP_RET3):
                    extra = "v0=%08x" % regs[2]
                L("  HALT %s B%d VS%d %s | %s %s | %s" % (NAMEN.get(pc, "%08x" % pc), bild, vs, extra,
                                                       zustand(), mun(), ents()))
        L("nachher:", zustand(), mun())
        L("nachher:", ents())
        for a in ALLE_BP:
            g.cmd("z0,%x,4" % a)
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        ok = "EnableGDBServer = false" in open(INI, encoding="utf-8").read()
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" % ok)
        try:
            os.remove(bak)
        except OSError:
            pass


if __name__ == "__main__":
    sys.exit(main())
