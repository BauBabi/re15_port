"""orig_cursor_zensus.py - Runde 36 G2: Zensus der Panel-Cursor am ORIGINAL (DuckStation, MZD-Disc, Auslieferungs-EXE
@0x80026e4c 0800e003). Gegenstueck zu re15_port/tests/unit/probe_r36_g2_cursor_rahmen.c (Port).

Je Raum: stage_saves/mzd_debugmenu.sav laden, Debug-Menue-Zustand setzen (Menue-Funktion @0x80014444: Zeile 0x800BBE5D
= 1 JUMP @0x80014698, Stage 0x800BBE5E @0x800146E4, Raumindex 0x800BBE5F + Stage @0x800146F4/@0x8001470C), Quadrat
(Kante 0x800AC762, @0x80014A38) = laden. Pruefung: alle SCA-Zellen im RAM == RDT-Datei (RDT-Basis DAT_800ac778,
SCA-Zeiger RDT+0x20 = FUN_8003b558 @0x8003b5dc, Bereichszeiger @0x8003b600-614), Cursor-Pool-Platz k = obj
(Installer @0x8004093c-958): +0 = 0x0101, Typ 4, Band 0.
Fahrt je Richtung (OBEN +z, UNTEN -z, LINKS -x, RECHTS +x): am Haltepunkt @0x8002bd44 (Eintritt Objekt-Pass, nach
der SCD @0x8001cdec im selben Bild) wird obj+0x34 bzw. +0x3C um +-200 verschoben — dieselben Felder und derselbe
Zeitpunkt wie Add_speed (@0x80040f40 `lw v1,52(a1) / addu / sw v0,52(a1)`, `... 60(a1)`); am Haltepunkt @0x8002c158
(Austritt) wird die Lage gelesen; @0x8003da78 protokolliert die Zelle (a0) jeder Ausschiebung des Cursors (a1).
Bis der Cursor 15 Bilder steht (max 400). Kein Pad: die Schubkette (@0x8002bf24 HOCH) greift nie.
settings.ini: gesichert, EnableGDBServer/ConfirmPowerOff/Pad-Bindungen nur fuer den Lauf, danach zurueck.
Optionale Spielflags vor dem Laden (Cursor, die sub00/main00 nur in einem Else-Zweig installieren): ":b.n=v" je Flag,
Bank b = Tabelle @0x80074664[b] (3 -> 0x800b0ff8, 4 -> 0x800b1018; Set-Handler @0x8003fdd0-fe0c: Wort n>>5, Maske
0x80000000 >> (n & 31)). Bank 5 ist raumlokal (FUN_8003ecec `sw zero,0x800b1028` @0x8003ed74) und laesst sich so nicht
vorbelegen.
Aufruf: python orig_cursor_zensus.py <ausgabeordner> <psx-wurzel> STAGEn:ROOMxxxx:obj[,obj][:b.n=v ...] ..."""
import os, shutil, struct, subprocess, sys, time

HIER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HIER, "..", "R_belege"))
from orig_1090_push import Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, INI, PORT

BASIS = os.path.abspath(os.path.join(HIER, "..", "..", "..", "stage_saves", "mzd_debugmenu.sav"))
POOL, OBJ_SZ = 0x800B3F98, 0x94
BP_IN, BP_HIT, BP_OUT = 0x8002BD44, 0x8003DA78, 0x8002C158
RICHTUNG = (("OBEN", 0, 200), ("UNTEN", 0, -200), ("LINKS", -200, 0), ("RECHTS", 200, 0))


def s32(b, o):
    return struct.unpack_from("<i", b, o)[0]


BANK = {3: 0x800B0FF8, 4: 0x800B1018}


def raum(out, psx, stage, rid, objs, L, gp, B, flags=()):
    """Einen Raum laden und alle Cursor messen. Rueckgabe: Liste (obj, richtung, start, bewegt, ende, erste)."""
    rdt = open(os.path.join(psx, stage, "ROOM%04X.RDT" % rid), "rb").read()
    sca_f = struct.unpack_from("<I", rdt, 0x20)[0]
    n_all = sum(struct.unpack_from("<5I", rdt, sca_f + 4))
    zellen_datei = rdt[sca_f + 24:sca_f + 24 + 12 * n_all]
    st_idx = int(stage[5:]) - 1
    r_idx = (rid >> 4) & 0xFF
    erg = []
    vor = pids()
    eigene = set()
    try:
        subprocess.Popen([DUCK, "-batch", "-statefile", BASIS, CUE], cwd=out)
        for _ in range(20):
            time.sleep(1)
            eigene = pids() - vor
            if eigene:
                break
        g = Gdb(PORT)
        time.sleep(10)
        g.stop()
        m = g.mem(0x800BBE5C, 8)
        g.cmd("M%x,1:%02x" % (0x800BBE5E, st_idx))
        g.cmd("M%x,1:%02x" % (0x800BBE5F + st_idx, r_idx))
        for (bk, n, v) in flags:
            a = BANK[bk] + 4 * (n >> 5)
            w = struct.unpack("<I", g.mem(a, 4))[0]
            m_ = 0x80000000 >> (n & 31)
            w2 = (w | m_) if v else (w & ~m_ & 0xFFFFFFFF)
            g.cmd("M%x,4:%s" % (a, struct.pack("<I", w2).hex()))
            L("  Flag %d:%d := %d (@%08x %08x -> %08x)" % (bk, n, v, a, w, struct.unpack("<I", g.mem(a, 4))[0]))
        m2 = g.mem(0x800BBE5C, 8)
        L("ROOM%04X: Menue %s -> %s (Zeile %d, Stage %d, Raumindex 0x%02X)" % (rid, m.hex(), m2.hex(), m2[1], m2[2],
                                                                               m2[3 + st_idx]))
        g.cont()
        time.sleep(1.0)
        gp.press_button(button=B.XUSB_GAMEPAD_X); gp.update(); time.sleep(0.13)
        gp.release_button(button=B.XUSB_GAMEPAD_X); gp.update()
        time.sleep(22 if st_idx else 12)
        g.stop()
        L("  EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex())
        base = struct.unpack("<I", g.mem(0x800AC778, 4))[0]
        sca = struct.unpack("<I", g.mem(base + 0x20, 4))[0]
        q0 = struct.unpack("<I", g.mem(sca + 4, 4))[0]
        ram = g.mem(q0, 12 * n_all) if 0x80000000 <= q0 < 0x80200000 else b""
        # u0 (Zellbyte +9) darf abweichen: Sca_id_set (Opcode 0x37, @0x8004179c) setzt es beim Raumaufbau um
        ok = len(ram) == len(zellen_datei) and all(
            ram[i] == zellen_datei[i] for i in range(len(ram)) if i % 12 != 9)
        L("  RDT-Basis %08x SCA %08x: %d Zellen im RAM == %s/ROOM%04X.RDT @0x%05X (ohne u0): %s" % (
            base, sca, n_all, stage, rid, sca_f + 24, ok))
        if ok:
            for i in range(n_all):
                if ram[12 * i + 9] != zellen_datei[12 * i + 9]:
                    L("    Zelle %d @%08x u0 Datei 0x%02x -> RAM 0x%02x (Sca_id_set)" % (
                        i, q0 + 12 * i, zellen_datei[12 * i + 9], ram[12 * i + 9]))
        if not ok:
            return erg
        for a in (BP_IN, BP_HIT, BP_OUT):
            g.cmd("Z0,%x,4" % a)
        for obj in objs:
            adr = POOL + OBJ_SZ * obj
            o = g.mem(adr, OBJ_SZ)
            f0 = struct.unpack_from("<I", o, 0)[0]
            L("  obj %d @%08x +0=%04x typ %d band %d lage (%d,%d,%d)" % (obj, adr, f0, o[8], o[0x82], s32(o, 0x34),
                                                                       s32(o, 0x38), s32(o, 0x3C)))
            if f0 != 0x0101 or o[8] != 4 or o[0x82] != 0:
                L("  obj %d ist nicht der Panel-Cursor - uebersprungen" % obj)
                continue
            for name, dx, dz in RICHTUNG:
                o = g.mem(adr, OBJ_SZ)
                start = (s32(o, 0x34), s32(o, 0x3C))
                bewegt = steh = n = 0
                vorher = start
                erste = None
                treffer = []
                soll = None
                g.cont()
                while n < 400 and steh < 15:
                    r = g.wait_stop(5.0)
                    if r is None:
                        g.stop()
                        L("    (kein Haltepunkt in 5 s)")
                        g.cont()
                        continue
                    rg = g.regs()
                    pc = rg[37] if len(rg) >= 38 else 0
                    if pc == BP_IN:
                        x, z = s32(g.mem(adr + 0x34, 4), 0), s32(g.mem(adr + 0x3C, 4), 0)
                        if dx:
                            g.cmd("M%x,4:%s" % (adr + 0x34, struct.pack("<i", x + dx).hex()))
                        if dz:
                            g.cmd("M%x,4:%s" % (adr + 0x3C, struct.pack("<i", z + dz).hex()))
                        soll = (x + dx, z + dz)
                    elif pc == BP_HIT and rg[5] == adr:
                        zl = g.mem(rg[4], 12)
                        w, d, zx, zz, t, u0, u1, fl = struct.unpack("<HHhhBBBB", zl)
                        treffer.append("@%08x x %d..%d z %d..%d r=%d" % (rg[4], zx, zx + w, zz, zz + d, rg[6]))
                    elif pc == BP_OUT and soll is not None:
                        o = g.mem(adr, 0x40)
                        lage = (s32(o, 0x34), s32(o, 0x3C))
                        n += 1
                        if lage != vorher:
                            bewegt += 1
                            steh = 0
                        else:
                            steh += 1
                        if erste is None and lage != soll:
                            erste = (soll, lage, list(treffer))
                        treffer = []
                        vorher = lage
                        soll = None
                    g.cont()
                g.stop()
                L("    %-6s start (%d,%d) bewegt %3d ende (%d,%d)%s" % (
                    name, start[0], start[1], bewegt, vorher[0], vorher[1],
                    ("  erste Ausschiebung (%d,%d) -> (%d,%d) Zelle %s" % (erste[0][0], erste[0][1], erste[1][0],
                                                                         erste[1][1], "; ".join(erste[2])))
                    if erste else "  KEINE Ausschiebung"))
                erg.append((obj, name, start, bewegt, vorher, erste))
                # 5 Bilder ohne Verschiebung (Port: frame_fahrt(ci,0,0) x 5)
                k = 0
                g.cont()
                while k < 5:
                    r = g.wait_stop(5.0)
                    if r is None:
                        break
                    rg = g.regs()
                    if (rg[37] if len(rg) >= 38 else 0) == BP_OUT:
                        k += 1
                    g.cont()
                g.stop()
        for a in (BP_IN, BP_HIT, BP_OUT):
            g.cmd("z0,%x,4" % a)
        g.cont()
        return erg
    finally:
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        t0 = time.time()
        while time.time() - t0 < 15 and (pids() & eigene):
            time.sleep(1)


def main():
    out = os.path.abspath(sys.argv[1])
    psx = os.path.abspath(sys.argv[2])
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "log.txt"), "a")

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
    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        pad_bind_alle(INI)
        for spec in sys.argv[3:]:
            teile = spec.split(":")
            stage, room, objs = teile[0], teile[1], teile[2]
            fl = []
            for f in teile[3:]:
                bn, v = f.split("=")
                b_, n_ = bn.split(".")
                fl.append((int(b_), int(n_), int(v)))
            rid = int(room[4:], 16)
            raum(out, psx, stage, rid, [int(x) for x in objs.split(",")], L, gp, B, fl)
        return 0
    finally:
        shutil.copy2(bak, INI)
        os.remove(bak)
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" %
          ("EnableGDBServer = false" in open(INI, encoding="utf-8").read()))


if __name__ == "__main__":
    sys.exit(main())
