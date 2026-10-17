"""orig_abe_messung.py - Runde 38 Spur W, Nachbesserung 1 (M1): halbtransparente Raum-Objekte am ORIGINAL
(DuckStation, MZD-Disc = Auslieferungs-EXE, Pruefung @0x80026e4c == 0800e003) messen.

Gemessen wird, was die GPU wirklich bekommt: die Primitive, die FUN_80025940 (GT3) / FUN_80025a98 (GT4) beim
Modell-Bind fuer ein Objekt bauen und die Zeichner FUN_80022f0c / FUN_80023098 / FUN_800254a0 / FUN_800256b0 je Bild
fuellen. Je Objekt des Pools (0x800B3F98 + 0x94*k): Flags (+0x00), Typ (+0x08), Attribut (+0x0C), Modellkoepfe
(+0x10 Dreiecke / +0x18 Vierecke, Anzahl = Kopf+20) und Primitiv-Puffer (+0x14 GT3, 2 x 40 Byte je Dreieck /
+0x1C GT4, 2 x 52 Byte je Viereck, Puffer-Index = u8 @0x800ACA34 wie FUN_8002c18c @0x8002c308 `lbu v0,676(s5)`).
Je Primitiv: Code-Byte (Wort 1, Byte 3: 0x34/0x3C deckend, 0x36/0x3E halbtransparent) und Texpage (Halbwort an
Byte 26 = Wort 6 oben; Bits 5-6 = ABR).
Danach 2 s laufen, geordnet schliessen (SaveStateOnExit) -> resume.sav -> beide Bildpuffer der VRAM als PNG.

Ablauf: Debug-Menue-Spielstand -> JUMP <stage0>/<raumidx> per RAM + Quadrat (wie orig_objmessung.py); Gegner aus;
optional Spieler an (x,z,band) setzen; Spieler per RAM durch die RVD-Zonen-Kette bis zum Ziel-Cut.
Aufruf: python orig_abe_messung.py <ausgabe> <rdt> <stage0> <raumidx> <ziel-cut> <objs,kommagetrennt> [x z band]"""
import os, sys, struct, time, shutil, subprocess
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from orig_20a0_rolladen import (Gdb, ini_set, pad_bind_alle, pids, DUCK, CUE, STATE, INI, RESUME, PORT, PL, POOL,
                                OBJ_SZ, ENT0, ENT_SZ, ACTIVE_CNT, DBG_ROW, DBG_STAGE, DBG_ROOM0, CUT_SHOWN, ROOM_IDX)
from orig_objmessung import zonen, kette

BUF_IDX = 0x800ACA34   # FUN_8002c18c @0x8002c308 `lbu v0,676(s5)` (s5 = 0x800AC790)


def prims_lesen(g, o, buf):
    """[(art, k, code, tpage, tag)] fuer alle Dreiecke/Vierecke des Pool-Objekts o (Bytes)."""
    res = []
    for art, kopf_off, prim_off, stride, half in (('T', 0x10, 0x14, 80, 40), ('Q', 0x18, 0x1C, 104, 52)):
        kopf = struct.unpack_from('<I', o, kopf_off)[0]
        prim = struct.unpack_from('<I', o, prim_off)[0]
        if not (0x80000000 <= kopf < 0x80200000 and 0x80000000 <= prim < 0x80200000):
            continue
        n = struct.unpack('<I', g.mem(kopf + 20, 4))[0]
        if n > 400:
            continue
        for k in range(n):
            p = g.mem(prim + buf * half + k * stride, half)
            tag = struct.unpack_from('<I', p, 0)[0]
            code = p[7]
            tpage = struct.unpack_from('<H', p, 26)[0]
            res.append((art, k, code, tpage, tag))
    return res


def main():
    out = os.path.abspath(sys.argv[1]); rdt = sys.argv[2]
    stage, raum, ziel = int(sys.argv[3]), int(sys.argv[4], 0), int(sys.argv[5])
    objs = [int(v) for v in sys.argv[6].split(',')]
    start_xzb = tuple(int(v) for v in sys.argv[7:10]) if len(sys.argv) >= 10 else None
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
        time.sleep(4)
        g.stop()
        # Gegner sofort nach dem Laden aus (Aktiv-Bit der Entities 0x800ACC2C + 500*k, g_active_count 0x800ACA4E),
        # sonst packt ein Gegner den Spieler waehrend der Wartezeit (ROOM5060: Spieler im Griff, Lage nicht setzbar).
        weg_n = 0
        for _ in range(int((25 if stage else 14) / 0.5)):
            for k in range(16):
                a = ENT0 + ENT_SZ * k
                w0 = struct.unpack("<I", g.mem(a, 4))[0]
                if w0 & 1:
                    g.memw(a, struct.pack("<I", w0 & ~1))
                    weg_n += 1
            g.memw(ACTIVE_CNT, struct.pack("<H", 0))
            g.cont(); time.sleep(0.5); g.stop()
        L("Gegner beim Laden ausgeschaltet: %d Aktiv-Bits geloescht" % weg_n)

        def pl():
            e = g.mem(PL, 0x90)
            return (struct.unpack_from("<i", e, 0x34)[0], struct.unpack_from("<i", e, 0x38)[0],
                    struct.unpack_from("<i", e, 0x3C)[0], struct.unpack_from("<h", e, 0x6A)[0], e[0x82])

        def cut():
            return struct.unpack("<H", g.mem(CUT_SHOWN, 2))[0]

        L("Raumindex:", struct.unpack("<H", g.mem(ROOM_IDX, 2))[0], "Cut:", cut(), "Spieler:", pl())
        for _ in range(int(os.environ.get("ORIG_PROBE_N", "0"))):   # Beobachtung: steht der Spieler still?
            g.cont(); time.sleep(2.0); g.stop()
            L("  Probe: Spieler %s Cut %d Status %s" % (pl(), cut(), g.mem(PL, 0x10).hex()))
        for k in range(16):
            a = ENT0 + ENT_SZ * k
            w0 = struct.unpack("<I", g.mem(a, 4))[0]
            if w0 & 1:
                g.memw(a, struct.pack("<I", w0 & ~1))
                L("Entity %d -> Aktiv-Bit geloescht" % k)
        g.memw(ACTIVE_CNT, struct.pack("<H", 0))

        def setze(x, z, band):
            g.memw(PL + 0x34, struct.pack("<iii", x, -band * 1800, z))
            g.memw(PL + 0x82, bytes([band]))

        zs = zonen(rdt)
        if start_xzb:
            setze(start_xzb[0], start_xzb[1], start_xzb[2])
            g.cont(); time.sleep(1.5); g.stop()
            L("Startlage:", pl(), "Cut", cut())
        if cut() != ziel:
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
            L("Ziel-Cut nicht erreicht - Abbruch (Spielstand wird trotzdem geordnet gesichert)")
            g.cont(); time.sleep(0.5)
            for p in eigene:
                subprocess.run(["taskkill", "/PID", p], capture_output=True)
            return 4
        g.cont(); time.sleep(2.0); g.stop()
        buf = g.mem(BUF_IDX, 1)[0]
        L("Cut %d, Spieler %s, Puffer-Index @0x800ACA34 = %d" % (cut(), pl(), buf))
        for k in objs:
            o = g.mem(POOL + OBJ_SZ * k, OBJ_SZ)
            w0 = struct.unpack_from('<I', o, 0)[0]
            attr = struct.unpack_from('<I', o, 0x0C)[0]
            pos = struct.unpack_from('<iii', o, 0x34)
            L("Pool %d: Flags %08x Typ %d Attribut 0x%04x (Bit2 %d, Bits 2-3 = %d) Lage %s Farbe+Code obj+0x70 %s"
              % (k, w0, o[8], attr & 0xFFFF, (attr >> 1) & 1, (attr >> 2) & 3, pos, o[0x70:0x74].hex()))
            ps = prims_lesen(g, o, buf)
            arten = {}
            for art, i, code, tpage, tag in ps:
                key = (art, code, tpage)
                arten[key] = arten.get(key, 0) + 1
            for (art, code, tpage), nn in sorted(arten.items()):
                L("   %s x%d: Code 0x%02x (ABE %d) Texpage 0x%04x (ABR %d)"
                  % ('Dreieck' if art == 'T' else 'Viereck', nn, code, (code >> 1) & 1, tpage, (tpage >> 5) & 3))
            for art, i, code, tpage, tag in ps[:4]:
                L("     %s%02d tag %08x code 0x%02x tpage 0x%04x" % (art, i, tag, code, tpage))
        if os.environ.get("ORIG_ESP"):
            # ESP-Effekt-Pool @0x800a73b8, 96 Plaetze zu 0x84 (re15_esp.h RE15_ESP_FX_MAX, FUN_80019700 @0x8001978c);
            # Byte +0x6C = Flags (Bit 0 aktiv, Bit 1 sichtbar: FUN_80053240 @0x800532f4 `lbu v1,0(s0)` mit s0 = Platz+0x6C,
            # @0x800532fc/@0x80053308), Lage s16 an +0x28/
            # +0x2A/+0x2C (@0x80053314/@0x80053330, `lh` @0x80018594/a0/b0).
            for durchgang in range(3):
                pool = g.mem(0x800A73B8, 96 * 0x84)
                aktiv = []
                for k in range(96):
                    e = pool[k * 0x84:(k + 1) * 0x84]
                    if e[0x6C] & 1:
                        aktiv.append((k, e[0x6C], struct.unpack_from('<hhh', e, 0x28), e[0x6C:0x74].hex()))
                L("ESP-Pool Durchgang %d: %d aktive Plaetze" % (durchgang, len(aktiv)))
                for k, fl, pos, kopf in aktiv[:24]:
                    L("   Platz %2d Flags 0x%02x Lage %s Kopf %s" % (k, fl, pos, kopf))
                g.cont(); time.sleep(0.4); g.stop()
        g.cont(); time.sleep(2.0); g.stop()
        L("Ende: Spieler", pl(), "Cut", cut())
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
            shutil.copy2(RESUME, os.path.join(out, "orig_abe.sav"))
            L("resume.sav kopiert")
            try:
                REPO = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", ".."))
                sys.path.insert(0, os.path.join(REPO, ".claude", "skills", "re15-savestate-ghidra", "scripts"))
                import re15_ss
                from PIL import Image
                r = re15_ss.Ram(os.path.join(out, "orig_abe.sav"))
                re15_ss.vram_png(r, os.path.join(out, "vram.png"), true_base=True)
                v = Image.open(os.path.join(out, "vram.png"))
                v.crop((0, 0, 320, 240)).save(os.path.join(out, "buf0.png"))
                v.crop((0, 240, 320, 480)).save(os.path.join(out, "buf1.png"))
                L("Bildpuffer buf0.png (VRAM 0,0) / buf1.png (VRAM 0,240) geschrieben")
            except Exception as ex:
                L("Bildpuffer nicht geschrieben:", ex)


if __name__ == "__main__":
    sys.exit(main())
