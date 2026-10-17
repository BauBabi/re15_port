#!/usr/bin/env python3
"""Runde 38 Spur R — RE2-ORIGINAL-Messung (RE2 Retail Leon SLUS-00748, DuckStation-GDB): wo stehen und wo werden die
Baby-Spinnen (Typ 0x26) GEZEICHNET? Nutzerpunkt 3 "Minispinnen fliegen weitestgehend in der Luft".

Befund im Port (Dossier 4.3): das RE2-EM026-Skelett traegt in jedem Keyframe eine Wurzel-Y von -1797..-1585 (Bone 0
rel (0,0,0)), der Port zeichnet die Babys damit 1166..1797 ueber ihrer Entity-Y. Frage an das Original: wie hoch liegt
die Entity-Y (+0x3C) gegenueber dem Boden (+0x1C2), und wo liegt die Weltlage von Part 0 (Part-Pool +0x198, Part 0
+0x5C/+0x60/+0x64 s32 = Translation der Welt-Matrix, Muster re2_gdb_grab.py Runde 35 Spur H)?

Ablauf:
  1. Spielstand SLUS-00748_5.sav (Stage 1 Raum 5 = ROOM2050, Runde 35 H belegt) laden, GDB.
  2. Raumsprung per RAM nach ROOM2010 (Stage-Byte 1, Raum-Byte 1, zwei Adult-Spinnen am Boden, RDT @0x1104/0x111A)
     — exakt der RE2-Tuer-Handler (re2_arm_grab.lua, Runde 35: 0x800DF348 := 1, 0x800CE550 := Satz @0x80010B5C,
     0x800CFBDC |= 0xFF000000; FUN_80026b7c liest x/y/z/Blick/Stage/Raum/Cut/Ebene aus dem Satz).
  3. Haltepunkt 0x800268A4 = direkt hinter `jal 0x80027160` (Part-Matrizen der Entity s0 fuer DIESES Bild fertig,
     @0x8002689c; der Boden +0x1C2 wurde davor @0x80026874 `sh v0,450(s0)` geschrieben). Je Treffer mit s0 =
     Spinne/Baby eine Zeile.
  4. Nach --tod Bildern Spinne 0 per RAM in den TOD (+0x4 = 3, +0x5 = 1, +0x6 = +0x7 = 0 -> DEATH-Wurzel @0x80103C80,
     generische Zeile, Babys FUN_80105D38(self,0x2002,(rand&3)+6) @0x80104590-A4).
settings.ini: EnableGDBServer wird fuer den Lauf auf true gesetzt und danach auf den VORHERIGEN Inhalt zurueckgesetzt.
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE  = r"C:\Users\mjoedicke\Downloads\ePSXe2018\re2leon.cue"
INI  = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
SAVES = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "savestates")
ap = argparse.ArgumentParser()
ap.add_argument("--state", default=os.path.join(SAVES, "SLUS-00748_5.sav"))
ap.add_argument("--x", type=int, default=-23000); ap.add_argument("--z", type=int, default=-26000)
ap.add_argument("--yaw", type=int, default=2358); ap.add_argument("--cut", type=int, default=4)
ap.add_argument("--frames", type=int, default=420); ap.add_argument("--tod", type=int, default=40)
ap.add_argument("--out", required=True)
ap.add_argument("--modus", default="bp", help="bp = Haltepunkt je Entity; int = periodisch anhalten (ohne Haltepunkt)")
a = ap.parse_args()
PL, MODE, DOORPTR, FLAGS_DC = 0x800CFBF8, 0x800DF348, 0x800CE550, 0x800CFBDC
STAGE, ROOM, CUT, REC, ETAB = 0x800D481C, 0x800D481E, 0x800CFBF2, 0x80010B5C, 0x800CFE18
BP = 0x800268A4

ini_alt = open(INI, "r", encoding="utf-8", errors="replace").read()
shutil.copyfile(INI, a.out + ".settings.ini.vorher")
if "EnableGDBServer = false" in ini_alt:
    open(INI, "w", encoding="utf-8").write(ini_alt.replace("EnableGDBServer = false", "EnableGDBServer = true", 1))
proc = None
def ini_zurueck():
    open(INI, "w", encoding="utf-8").write(ini_alt)
try:
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", a.state, CUE])
    s = None
    for _ in range(120):
        try: s = socket.create_connection(("127.0.0.1", 2345), timeout=2); break
        except OSError: time.sleep(0.5)
    if s is None: raise SystemExit("keine Verbindung")
    ini_zurueck()
    s.settimeout(60); buf = b""
    def send(cmd):
        c = sum(cmd.encode()) & 0xff
        s.sendall(b"$" + cmd.encode() + b"#%02x" % c)
    def recv_pkt():
        global buf
        while True:
            i = buf.find(b"$")
            if i >= 0:
                j = buf.find(b"#", i)
                if j >= 0 and len(buf) >= j + 3:
                    pkt = buf[i+1:j]; buf = buf[j+3:]; s.sendall(b"+"); return pkt.decode(errors="replace")
            d = s.recv(65536)
            if not d: raise EOFError
            buf += d
    def cmd(c): send(c); return recv_pkt()
    def rd(addr, n): return bytes.fromhex(cmd("m%08x,%x" % (addr, n)))
    def wr(addr, data): r = cmd("M%08x,%x:%s" % (addr, len(data), data.hex())); assert r == "OK", r
    def u8(adr): return rd(adr, 1)[0]
    def u32(adr): return struct.unpack("<I", rd(adr, 4))[0]
    out = open(a.out, "w")
    print("?", cmd("?"))
    st0, rm0 = struct.unpack("<hh", rd(STAGE, 4))
    out.write("# Spielstand: stage %d room %d PL (%d,%d,%d)\n" % ((st0, rm0) + struct.unpack("<3i", rd(PL + 0x38, 12))))
    # --- Raumsprung (re2_arm_grab.lua) ---
    wr(REC + 0, struct.pack("<hhhh", a.x, 0, a.z, a.yaw))
    wr(REC + 8, bytes([1, 1, a.cut, 0]))
    wr(DOORPTR, struct.pack("<I", REC))
    wr(FLAGS_DC, struct.pack("<I", u32(FLAGS_DC) | 0xFF000000))
    wr(MODE, b"\x01")
    out.write("# Sprung ROOM2010 geschrieben (%d,%d) Blick %d Cut %d\n" % (a.x, a.z, a.yaw, a.cut))
    out.flush()
    # Raumwechsel OHNE Haltepunkt laufen lassen (CD-Laden), dann anhalten und pruefen.
    for versuch in range(30):
        send("c"); time.sleep(3.0)
        s.sendall(b"\x03"); recv_pkt()
        st, rm = struct.unpack("<hh", rd(STAGE, 4))
        px, py, pz = struct.unpack("<3i", rd(PL + 0x38, 12))
        print("Warten %d: stage %d room %d PL (%d,%d,%d)" % (versuch, st, rm, px, py, pz), flush=True)
        if st == 1 and rm == 1 and px == a.x and pz == a.z:
            break
    else:
        raise SystemExit("ROOM2010 nicht geladen")
    out.write("# geladen nach %d Versuchen\n" % (versuch + 1)); out.flush()
    if a.modus == "int":
        # Periodisch anhalten (kein Haltepunkt): je Probe alle Spinnen/Babys aus der Entity-Tabelle lesen.
        t0 = time.time(); probe = 0; tod_done = False
        while probe < a.frames:
            send("c"); time.sleep(0.1)
            s.sendall(b"\x03"); recv_pkt()
            probe += 1
            pl = rd(PL, 0x248)
            out.write("P%d t=%.1f PL(%d,%d,%d) y1c2=%d\n" % ((probe, time.time() - t0) + struct.unpack_from("<3i", pl, 0x38)
                                                           + (struct.unpack_from("<h", pl, 0x1C2)[0],)))
            if probe in (1, 20):
                tab = rd(ETAB, 33 * 4)
                zeile = []
                for k in range(33):
                    p = struct.unpack_from("<I", tab, 4 * k)[0]
                    if 0x80000000 <= p < 0x801FFE00 and p != 0x800D424C:
                        zeile.append("%d:%08x:t%02x:w0=%08x" % (k, p, u8(p + 8), u32(p)))
                out.write("# ETAB %s\n" % " ".join(zeile))
            for k in range(33):
                p = u32(ETAB + 4 * k)
                if not (0x80000000 <= p < 0x801FFE00) or p == 0x800D424C: continue
                e = rd(p, 0x248)
                typ = e[8]
                if typ not in (0x25, 0x26): continue
                if probe == a.tod and not tod_done and typ == 0x25:
                    wr(p + 4, bytes([3, 1, 0, 0])); tod_done = True
                    out.write("# TOD Spinne Satz %d @%08x (vorher +4..7 = %s)\n" % (k, p, e[4:8].hex()))
                parts = struct.unpack_from("<I", e, 0x198)[0]
                t = (0, 0, 0)
                if 0x80000000 <= parts < 0x801FFE00:
                    t = struct.unpack("<3i", rd(parts + 0x5C, 12))
                x, y, z = struct.unpack_from("<3i", e, 0x38)
                out.write("  [%d] E@%08x t=%02x w0=%08x st=%s g9=%02x @(%d,%d,%d) y1c2=%d m222=%d f10e=%04x hp=%d"
                          " P0t(%d,%d,%d) P0t.y-y=%d\n" % (
                              k, p, typ, struct.unpack_from("<I", e, 0)[0], e[4:8].hex(), e[9], x, y, z,
                              struct.unpack_from("<h", e, 0x1C2)[0], e[0x222], struct.unpack_from("<H", e, 0x10E)[0],
                              struct.unpack_from("<h", e, 0x156)[0], t[0], t[1], t[2], t[1] - y))
            out.flush()
        out.write("# fertig: %d Proben, %.0f s\n" % (probe, time.time() - t0))
        out.close()
        raise SystemExit(0)
    print("Z0", cmd("Z0,%08x,4" % BP))
    n_bild = -1; tod_done = False; t0 = time.time(); treffer = 0
    marke = None          # Bild-Takt: die erste Adult-Spinne, die nach dem Laden den Haltepunkt trifft
    while n_bild < a.frames:
        send("c"); recv_pkt()
        regs = cmd("g")
        s0 = struct.unpack("<I", bytes.fromhex(regs[16*8:17*8]))[0]
        if time.time() - t0 > 900: raise SystemExit("Zeitlimit")
        if not (0x80000000 <= s0 < 0x801FFE00):
            continue
        if marke is None:
            st, rm = struct.unpack("<hh", rd(STAGE, 4))
            if st == 1 and rm == 1 and u8(s0 + 8) == 0x25:
                marke = s0
                out.write("# Bild-Takt = Spinne @%08x\n" % s0)
            else:
                continue
        if s0 == marke:
            if True:
                n_bild += 1
                if n_bild == 0:
                    out.write("# ROOM2010 geladen, PL (%d,%d,%d)\n" % struct.unpack("<3i", rd(PL + 0x38, 12)))
                pl = rd(PL, 0x248)
                out.write("B%d PL(%d,%d,%d) y1c2=%d hp=%d\n" % ((n_bild,) + struct.unpack_from("<3i", pl, 0x38)
                                                              + (struct.unpack_from("<h", pl, 0x1C2)[0],
                                                                 struct.unpack_from("<h", pl, 0x156)[0])))
                if n_bild == a.tod and not tod_done:
                    for k in range(33):
                        p = u32(ETAB + 4 * k)
                        if 0x80000000 <= p < 0x80200000 and p != 0x800D424C and u8(p + 8) == 0x25:
                            vorher = rd(p + 4, 4).hex()
                            wr(p + 4, bytes([3, 1, 0, 0]))
                            out.write("# TOD Spinne Satz %d @%08x (vorher +4..7 = %s)\n" % (k, p, vorher))
                            tod_done = True
                            break
        typ = u8(s0 + 8)
        if typ not in (0x25, 0x26):
            continue
        e = rd(s0, 0x248)
        parts = struct.unpack_from("<I", e, 0x198)[0]
        t = (0, 0, 0)
        if 0x80000000 <= parts < 0x80200000:
            t = struct.unpack("<3i", rd(parts + 0x5C, 12))
        x, y, z = struct.unpack_from("<3i", e, 0x38)
        out.write("  E@%08x t=%02x w0=%08x st=%s g9=%02x @(%d,%d,%d) yaw=%d y1c2=%d m222=%d f10e=%04x hp=%d"
                  " P0t(%d,%d,%d) P0t.y-y=%d\n" % (
                      s0, typ, struct.unpack_from("<I", e, 0)[0], e[4:8].hex(), e[9], x, y, z,
                      struct.unpack_from("<h", e, 0x76)[0], struct.unpack_from("<h", e, 0x1C2)[0], e[0x222],
                      struct.unpack_from("<H", e, 0x10E)[0], struct.unpack_from("<h", e, 0x156)[0],
                      t[0], t[1], t[2], t[1] - y))
        treffer += 1
    out.write("# fertig: %d Bilder, %d Spinnen-Zeilen, %.0f s\n" % (n_bild, treffer, time.time() - t0))
    out.close()
finally:
    try:
        if proc is not None:
            try: cmd("z0,%08x,4" % BP); send("c")
            except Exception: pass
            subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
            for _ in range(20):
                if proc.poll() is not None: break
                time.sleep(0.5)
            if proc.poll() is None: subprocess.run(["taskkill", "/F", "/PID", str(proc.pid)], capture_output=True)
    finally:
        ini_zurueck()
        print("DONE", "settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace").read() == ini_alt)
