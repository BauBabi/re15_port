"""RE2-Original (SLUS-00748) in DuckStation per GDB: JEDEN Gegner-Takt eines Zombies mitschreiben (Haltepunkt auf der
EXE-Gegnerschleife @0x800265E0 `sw v0,496(s0)` = +0x1F0 Abstand, s0 = Entity; derselbe Haltepunkt wie Spur H
H_belege/re2_gdb_zombie.py). Runde 37 Spur L: wann wechselt ein RE2-Zombie in den Arme-hoch-Gang +0x4 = 0x201, wie
bewegt/steuert er sich dort, wann greift er.
Je Takt: Zustandswort +0x4, Lage +0x38/+0x40, Gier +0x76, Abstand +0x1F0, Sicht +0x154, Timer +0x158/+0x15A, +0x16A,
Steuerziel +0x1C4/+0x1C6, Clip-Wort +0x14C, Spieler-Lage/Gier, 0x800CFBF6 (Bewegungsbits), 0x800CFDCB (Spieler +0x1D3).
settings.ini (EnableGDBServer, SaveStateOnExit) und SLUS-00748_resume.sav werden gesichert und zurueckgeschrieben.
Aufruf: python re2_gdb_0x201.py <spielstand.sav> <ausgabeordner> [takte_je_gegner=900]"""
import os, shutil, struct, subprocess, sys, time
HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.normpath(os.path.join(HIER, "..", "..", "befunde_runde35", "H_raeume", "re2_mess")))
from re2_gdb_grab import Gdb, ini_set, pids, DUCK, CUE, INI, SAVES, PORT, PL
BP = 0x800265E0


def regs(g):
    r = g.cmd("g")
    v = bytes.fromhex(r.decode()[:32 * 8])
    return struct.unpack("<32I", v)


def main():
    state, out = os.path.normpath(sys.argv[1]), os.path.abspath(sys.argv[2])
    ntakt = int(sys.argv[3]) if len(sys.argv) > 3 else 900
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "w")

    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    if pids():
        L("DuckStation laeuft bereits - Abbruch"); return 2
    shutil.copy2(INI, os.path.join(out, "settings.ini.bak"))
    resume = os.path.join(SAVES, "SLUS-00748_resume.sav")
    if os.path.exists(resume): shutil.copy2(resume, os.path.join(out, "resume.sav.bak"))
    eigene = set()
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "SaveStateOnExit", "false")
        vor = pids()
        subprocess.Popen([DUCK, "-batch", "-statefile", state, CUE], cwd=out)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene: break
        L("DuckStation", eigene)
        g = Gdb(PORT); L("GDB verbunden")
        time.sleep(6)
        g.sk.sendall(b"\x03"); time.sleep(0.5)
        L("Stopp:", g.cmd("?"))
        L("stage", g.s16(0x800D481C), "room", g.s16(0x800D481E), "PL", g.s32(PL + 0x38), g.s32(PL + 0x40),
          "yaw", g.s16(PL + 0x76), "PLw4 %08x" % g.u32(PL + 4))
        L("Z0:", g.cmd("Z0,%x,4" % BP))
        txt = open(os.path.join(out, "takte.txt"), "w")
        txt.write("# n ent typ w4 x z yaw d los t158 t15a b16a sx sz clipw | plx plz plyaw bf6 dcb\n")
        zaehl = {}
        t0 = time.time()
        while time.time() - t0 < 900:
            g.sk.sendall(b"$c#63")
            g.recv(60)
            s0 = regs(g)[16]
            if not (0x80000000 <= s0 < 0x80200000):
                continue
            e = g.mem(s0, 0x248)
            typ = e[8]
            if typ < 0x10 or typ > 0x1f:
                continue
            n = zaehl.get(s0, 0); zaehl[s0] = n + 1
            u32 = lambda o: struct.unpack_from("<I", e, o)[0]
            s32 = lambda o: struct.unpack_from("<i", e, o)[0]
            s16 = lambda o: struct.unpack_from("<h", e, o)[0]
            u16 = lambda o: struct.unpack_from("<H", e, o)[0]
            pl = g.mem(PL, 0x80)
            plx = struct.unpack_from("<i", pl, 0x38)[0]; plz = struct.unpack_from("<i", pl, 0x40)[0]
            plyaw = struct.unpack_from("<h", pl, 0x76)[0]
            bf6 = struct.unpack_from("<H", g.mem(0x800CFBF6, 2), 0)[0]
            dcb = g.mem(0x800CFDCB, 1)[0]
            txt.write("%d %08x %02x %08x %d %d %d %d %04x %d %d %d %d %d %08x | %d %d %d %04x %02x\n" % (
                n, s0, typ, u32(4), s32(0x38), s32(0x40), s16(0x76) & 0xfff, u32(0x1F0), u16(0x154), s16(0x158),
                s16(0x15A), e[0x16A], s16(0x1C4), s16(0x1C6), u32(0x14C), plx, plz, plyaw & 0xfff, bf6, dcb))
            if n % 60 == 0: txt.flush()
            if zaehl and all(v >= ntakt for v in zaehl.values()) and min(zaehl.values()) >= ntakt:
                break
        txt.close()
        L("Takte je Entity:", {"%08x" % k: v for k, v in zaehl.items()}, "Zeit %.0f s" % (time.time() - t0))
        g.cmd("z0,%x,4" % BP)
        return 0
    finally:
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        t0 = time.time()
        while time.time() - t0 < 20 and (pids() & eigene): time.sleep(1)
        shutil.copy2(os.path.join(out, "settings.ini.bak"), INI); L("settings.ini zurueckgeschrieben")
        rb = os.path.join(out, "resume.sav.bak")
        if os.path.exists(rb): shutil.copy2(rb, resume); L("resume.sav zurueckgeschrieben")


if __name__ == "__main__":
    sys.exit(main())
