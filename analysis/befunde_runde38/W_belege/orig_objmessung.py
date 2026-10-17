"""orig_objmessung.py - Runde 38 Spur W: Gegenprobe der Objekt-Polygon-Schranken an einem ZWEITEN Fall im ORIGINAL
(DuckStation, Auslieferungs-EXE): ein Raum-Objekt, das der LICHT-Viereck-Zeichner FUN_800256b0 zeichnet (der
Rolladen 20A0 lief durch den Flach-Zeichner FUN_80023098).

Ablauf: Debug-Menue-Spielstand -> JUMP <stage>/<raum> per RAM (0x800BBE5D/5E/5F+stage) + Quadrat; Gegner aus; andere
Objekte des Pools ausblenden (Flags-Wort Bit 0 loeschen, FUN_8002c18c ueberspringt w0 & 1 == 0); Spieler per RAM durch
die RVD-Zonen bis zum Ziel-Cut fuehren (Kette aus der RDT: Zone cam_from -> cam_to, Mitte des Vierecks); dann
Haltepunkte in FUN_800256b0: @0x800256b0 (Eintritt, a0 = Modellkopf, Anzahl +20), @0x800257c0 (`bgez v0` nach NCLIP:
v0 = OPZ), @0x800258b4 (`beq v0,zero` nach `sra 6`: OTZ = *(sp), SXY0..3 = Prim+8/+20/+32/+44), @0x80025914 (`sw` =
eingehaengt). Ein voller Aufruf. Danach 2 s laufen, geordnet schliessen (SaveStateOnExit -> Bild).
Aufruf: python orig_objmessung.py <ausgabe> <rdt-datei> <stage0> <raumidx> <obj> <ziel-cut> <anzahl> [x z band]
  (anzahl = Viereckzahl des Objektmodells: nur DIESER Aufruf von FUN_800256b0 wird aufgezeichnet — der Zeichner
   laeuft auch fuer andere Modelle; x z band = optionale Startlage des Spielers vor der Zonen-Kette)"""
import os, sys, struct, time, shutil, subprocess
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from orig_20a0_rolladen import (Gdb, ini_set, pad_bind_alle, pids, sxy, DUCK, CUE, STATE, INI, RESUME, PORT, PL, POOL,
                                OBJ_SZ, ENT0, ENT_SZ, ACTIVE_CNT, DBG_ROW, DBG_STAGE, DBG_ROOM0, CUT_SHOWN, ROOM_IDX)

BP_ENTRY, BP_NCLIP, BP_OTZ, BP_LINK = 0x800256B0, 0x800257C0, 0x800258B4, 0x80025914


def zonen(rdt):
    d = open(rdt, 'rb').read()
    p = struct.unpack_from('<I', d, 0x28)[0]
    out = []
    while p + 20 <= len(d):
        m, cf, ct = struct.unpack_from('<HBB', d, p)
        if m == 0xFFFF and cf == 0xFF and ct == 0xFF:
            break
        pts = struct.unpack_from('<8h', d, p + 4)
        out.append((cf, ct, [(pts[k], pts[k + 1]) for k in range(0, 8, 2)], m >> 8))
        p += 20
    return out


def kette(zs, start, ziel):
    """BFS ueber die Ausloese-Zonen (nicht die erste Zone je cam_from = Anker)."""
    erste = set()
    trig = []
    for i, (cf, ct, q, fl) in enumerate(zs):
        if cf not in erste:
            erste.add(cf)
            continue
        trig.append((cf, ct, q))
    from collections import deque
    dq = deque([(start, [])])
    seen = {start}
    while dq:
        c, weg = dq.popleft()
        if c == ziel:
            return weg
        for cf, ct, q in trig:
            if cf == c and ct not in seen:
                seen.add(ct)
                dq.append((ct, weg + [(cf, ct, q)]))
    return None


def main():
    out = os.path.abspath(sys.argv[1]); rdt = sys.argv[2]
    stage, raum, obj, ziel = int(sys.argv[3]), int(sys.argv[4], 0), int(sys.argv[5]), int(sys.argv[6])
    anzahl = int(sys.argv[7])
    start_xzb = tuple(int(v) for v in sys.argv[8:11]) if len(sys.argv) >= 11 else None
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

    def tap(btn, hold=0.12, gap=0.5):
        gp.press_button(button=btn); gp.update(); time.sleep(hold)
        gp.release_button(button=btn); gp.update(); time.sleep(gap)

    bak = os.path.join(out, "settings.ini.bak")
    shutil.copy2(INI, bak)
    eigene = set()
    resume_mtime0 = os.path.getmtime(RESUME) if os.path.exists(RESUME) else 0
    try:
        ini_set(INI, "Debug", "EnableGDBServer", "true")
        ini_set(INI, "Main", "ConfirmPowerOff", "false")
        ini_set(INI, "Main", "SaveStateOnExit", "true")
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
        time.sleep(8)
        g.stop()
        L("EXE-Stub @0x80026e4c:", g.mem(0x80026E4C, 4).hex(), "(Auslieferung 0800e003)")
        g.cont()
        tap(B.XUSB_GAMEPAD_DPAD_UP, 0.25, 0.8)
        tap(B.XUSB_GAMEPAD_DPAD_UP, 0.06, 0.5)
        tap(B.XUSB_GAMEPAD_DPAD_DOWN, 0.06, 0.6)
        g.stop()
        g.memw(DBG_ROW, bytes([1]))
        g.memw(DBG_STAGE, bytes([stage]))
        g.memw(DBG_ROOM0 + stage, bytes([raum]))
        dm = g.mem(DBG_ROW, 8)
        L("Debug-Menue gesetzt: Zeile %d Stage %d Raumidx %s" % (dm[0], dm[1], list(dm[2:8])))
        g.cont(); time.sleep(0.5)
        tap(B.XUSB_GAMEPAD_X, 0.1, 0.3)
        time.sleep(25 if stage else 14)
        g.stop()

        def pl():
            e = g.mem(PL, 0x90)
            return (struct.unpack_from("<i", e, 0x34)[0], struct.unpack_from("<i", e, 0x38)[0],
                    struct.unpack_from("<i", e, 0x3C)[0], struct.unpack_from("<h", e, 0x6A)[0], e[0x82])

        def objekt(k):
            o = g.mem(POOL + OBJ_SZ * k, OBJ_SZ)
            return (struct.unpack_from("<I", o, 0)[0], struct.unpack_from("<i", o, 0x34)[0],
                    struct.unpack_from("<i", o, 0x38)[0], struct.unpack_from("<i", o, 0x3C)[0],
                    struct.unpack_from("<h", o, 0x6A)[0], o[0x08], struct.unpack_from("<I", o, 0x0C)[0])

        def cut():
            return struct.unpack("<H", g.mem(CUT_SHOWN, 2))[0]

        L("Raumindex:", struct.unpack("<H", g.mem(ROOM_IDX, 2))[0], "Cut:", cut(), "Spieler:", pl())
        for k in range(8):
            L("  Pool %d:" % k, objekt(k))
        for k in range(16):
            a = ENT0 + ENT_SZ * k
            w0 = struct.unpack("<I", g.mem(a, 4))[0]
            if w0 & 1:
                g.memw(a, struct.pack("<I", w0 & ~1))
                L("Entity %d -> Aktiv-Bit geloescht" % k)
        g.memw(ACTIVE_CNT, struct.pack("<H", 0))
        for k in range(16):
            if k == obj:
                continue
            w0 = struct.unpack("<I", g.mem(POOL + OBJ_SZ * k, 4))[0]
            if w0 & 1:
                g.memw(POOL + OBJ_SZ * k, struct.pack("<I", w0 & ~1))
                L("Pool %d ausgeblendet (w0 %08x)" % (k, w0))

        def setze(x, z, band):
            g.memw(PL + 0x34, struct.pack("<iii", x, -band * 1800, z))
            g.memw(PL + 0x82, bytes([band]))

        zs = zonen(rdt)
        if start_xzb:
            setze(start_xzb[0], start_xzb[1], start_xzb[2])
            g.cont(); time.sleep(1.5); g.stop()
            L("Startlage:", pl(), "Cut", cut())
        weg = kette(zs, cut(), ziel)
        L("Zonen-Kette von Cut %d nach %d:" % (cut(), ziel), [(cf, ct) for cf, ct, q in (weg or [])])
        if weg is None:
            L("keine Kette - Abbruch")
            return 3
        band = pl()[4]
        for cf, ct, q in weg:
            mx = sum(p[0] for p in q) // 4; mz = sum(p[1] for p in q) // 4
            setze(mx, mz, band)
            g.cont(); time.sleep(1.0); g.stop()
            L("  Zone %d->%d Mitte (%d,%d): Cut %d Spieler %s" % (cf, ct, mx, mz, cut(), pl()))
        if cut() != ziel:
            L("Ziel-Cut nicht erreicht - Abbruch")
            return 4
        L("Objekt %d:" % obj, objekt(obj))
        for a in (BP_ENTRY, BP_NCLIP, BP_OTZ, BP_LINK):
            L("Z0 %08x:" % a, g.cmd("Z0,%x,4" % a))
        quads, auf, fertig = [], False, False
        t0 = time.time()
        g.cont()
        while not fertig and time.time() - t0 < 60:
            r = g.wait_stop(5.0)
            if r is None:
                L("kein Haltepunkt in 5 s")
                g.stop()
                break
            rg = g.regs()
            pc = rg[37] if len(rg) >= 38 else 0
            if pc == BP_ENTRY:
                n = struct.unpack("<I", g.mem(rg[4] + 20, 4))[0]
                if auf and quads:
                    fertig = True
                elif n == anzahl:
                    auf = True
                    L("FUN_800256b0 Eintritt a0=%08x Anzahl %d (gesuchtes Objekt)" % (rg[4], n))
                else:
                    L("FUN_800256b0 Eintritt a0=%08x Anzahl %d (anderes Modell, uebersprungen)" % (rg[4], n))
            elif pc == BP_NCLIP and auf:
                quads.append({"opz": struct.unpack("<i", struct.pack("<I", rg[2]))[0]})
            elif pc == BP_OTZ and auf and quads:
                prim = rg[5]
                q = quads[-1]
                q["otz"] = struct.unpack("<I", g.mem(rg[29], 4))[0]
                pm = g.mem(prim, 48)
                q["sxy"] = [sxy(struct.unpack_from("<I", pm, o)[0]) for o in (8, 20, 32, 44)]
            elif pc == BP_LINK and auf and quads:
                quads[-1]["ein"] = 1
            if not fertig:
                g.cont()
        for a in (BP_ENTRY, BP_NCLIP, BP_OTZ, BP_LINK):
            g.cmd("z0,%x,4" % a)
        L("Vierecke:", len(quads))
        for i, q in enumerate(quads):
            L("Q%02d opz=%d otz=%s sxy=%s eingehaengt=%d" % (i, q["opz"], q.get("otz", "-"), q.get("sxy", "-"),
                                                          q.get("ein", 0)))
        g.cont(); time.sleep(2.0); g.stop()
        L("Ende: Spieler", pl(), "Cut", cut(), "Objekt", objekt(obj))
        g.cont(); time.sleep(0.5)
        for p in eigene:
            subprocess.run(["taskkill", "/PID", p], capture_output=True)
        return 0
    finally:
        try:
            gp.reset(); gp.update()
        except Exception:
            pass
        t0 = time.time()
        while time.time() - t0 < 25 and (pids() & eigene):
            time.sleep(1)
        for p in pids() & eigene:
            subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)
        shutil.copy2(bak, INI)
        ok = "EnableGDBServer = false" in open(INI, encoding="utf-8").read()
        L("settings.ini zurueckgeschrieben (EnableGDBServer = false: %s)" % ok)
        if os.path.exists(RESUME) and os.path.getmtime(RESUME) > resume_mtime0:
            shutil.copy2(RESUME, os.path.join(out, "orig_obj.sav"))
            L("resume.sav kopiert")


if __name__ == "__main__":
    sys.exit(main())
