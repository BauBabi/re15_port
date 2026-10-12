"""RE2-Original (SLUS-00748) in DuckStation per GDB: Spielstand laden, je Gegner-Takt (Haltepunkt auf der EXE-Gegner-
schleife @0x800265E0 `sw v0,496(s0)` = +0x1F0 = Abstand, s0 = Entity) Zustandswort, Deskriptor, Lage, Abstand, Sicht
mitschreiben. Runde 37 Spur H. settings.ini (EnableGDBServer, SaveStateOnExit) und SLUS-00748_resume.sav werden
gesichert und am Ende zurueckgeschrieben.
Aufruf: python re2_gdb_zombie.py <spielstand.sav> <ausgabeordner> [takte_je_gegner=900] [lx lz yaw]"""
import os, shutil, socket, struct, subprocess, sys, time
sys.path.insert(0, r"C:/workspace/git/reAi_v2/.claude/worktrees/r37_zombieaktiv/analysis/befunde_runde35/H_raeume/re2_mess")
from re2_gdb_grab import Gdb, ini_set, pids, DUCK, CUE, INI, SAVES, PORT, PL, ETAB
BP = 0x800265E0

def regs(g):
    r = g.cmd("g")
    v = bytes.fromhex(r.decode()[:32 * 8])
    return struct.unpack("<32I", v)

def main():
    state, out = os.path.normpath(sys.argv[1]), os.path.abspath(sys.argv[2])
    ntakt = int(sys.argv[3]) if len(sys.argv) > 3 else 900
    stellen = len(sys.argv) > 6
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
        st, rm = g.s16(0x800D481C), g.s16(0x800D481E)
        L("stage", st, "room", rm, "PL", g.s32(PL + 0x38), g.s32(PL + 0x40), "yaw", g.s16(PL + 0x76), "PLw4 %08x" % g.u32(PL + 4))
        if stellen:
            lx, lz, lyaw = int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6])
            g.wmem(PL + 0x38, struct.pack("<i", lx)); g.wmem(PL + 0x40, struct.pack("<i", lz))
            g.wmem(PL + 0x44, struct.pack("<h", lx)); g.wmem(PL + 0x48, struct.pack("<h", lz))
            g.wmem(PL + 0x118, struct.pack("<h", lx)); g.wmem(PL + 0x11A, struct.pack("<h", lz))
            g.wmem(PL + 0x76, struct.pack("<h", lyaw))
            L("Leon gestellt", lx, lz, lyaw)
        L("Z0:", g.cmd("Z0,%x,4" % BP))
        txt = open(os.path.join(out, "takte.txt"), "w")
        zaehl = {}
        hits = 0
        t0 = time.time()
        while time.time() - t0 < 600:
            g.sk.sendall(b"$c#63")
            g.recv(60)
            rg = regs(g)
            s0 = rg[16]
            if not (0x80000000 <= s0 < 0x80200000):
                continue
            e = g.mem(s0, 0x248)
            typ = e[8]
            if typ < 0x10 or typ > 0x1f:
                continue
            n = zaehl.get(s0, 0); zaehl[s0] = n + 1
            if n == 0 and os.getenv("R2_STEHEN") == "1":
                # Gegenprobe Deskriptor 0: Zustandswort 0x0001 (Stehen, P0) + Deskriptor 0 schreiben, wie es der INIT
                # @0x801006ac-b8 fuer +0x10E&0x3F == 0 hinterlaesst; der Original-Code laeuft danach unveraendert.
                g.wmem(s0 + 4, struct.pack("<I", 0x00000001)); g.wmem(s0 + 0x10E, struct.pack("<H", 0))
                e = g.mem(s0, 0x248)
            hits += 1
            w4 = struct.unpack_from("<I", e, 4)[0]; f10e = struct.unpack_from("<H", e, 0x10E)[0]
            x = struct.unpack_from("<i", e, 0x38)[0]; z = struct.unpack_from("<i", e, 0x40)[0]
            d = struct.unpack_from("<I", e, 0x1F0)[0]; los = struct.unpack_from("<H", e, 0x154)[0]
            px = g.s32(PL + 0x38); pz = g.s32(PL + 0x40)
            txt.write("%d %08x typ=%02x w4=%08x f10e=%04x pos=(%d,%d) d=%d los=%04x t158=%d t15a=%d PL=(%d,%d)\n" % (
                n, s0, typ, w4, f10e, x, z, d, los, struct.unpack_from("<h", e, 0x158)[0],
                struct.unpack_from("<h", e, 0x15A)[0], px, pz))
            if n % 60 == 0: txt.flush()
            if min(zaehl.values()) >= ntakt and len(zaehl) >= 1 and max(zaehl.values()) >= ntakt:
                if all(v >= ntakt for v in zaehl.values()): break
        txt.close()
        L("Takte je Entity:", {"%08x" % k: v for k, v in zaehl.items()}, "Treffer", hits, "Zeit %.0f s" % (time.time() - t0))
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
