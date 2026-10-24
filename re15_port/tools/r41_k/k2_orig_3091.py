#!/usr/bin/env python3
"""Runde 41 Spur K: ORIGINAL (DuckStation-GDB, MZD-Disc, saubere EXE) - die ROOM3091-Szene (sub07) mit SHERRY als
Spielfigur Bild fuer Bild mitschreiben (Clip/Bild/Sub/Kanal je Aktor).

Ausgang: ein Spielstand mit Modell 0x0F in ROOM30C1 (HASH-957757946319438E_1.sav, Kopie im Scratchpad).
Ablauf (Muster analysis/befunde_runde37/N_belege/n2_gdb_elza.py, Spur J j41_szene.py):
  1. GDB-Server nur fuer den Lauf (settings.ini [Debug] EnableGDBServer; danach BYTEGLEICH zurueck), resume.sav des
     Nutzers vorher kopiert und nachher bytegleich zurueckgelegt.
  2. Haltepunkt 0x8001ce04 (Bildtakt der Hauptschleife, `jal 0x8001a50c`).
  3. Bild 20: Flags (4,8)=1 (Sicherung oben, ROOM3091 sub05 @0x016AA) und (9,21)=1 (Riegelschluessel, sub06 @0x016C2)
     setzen (Bank-Tabelle @0x80074664: [4]=0x800b1018, [9]=0x800b1078; Maske 0x80000000>>bit), dann Tuerwechsel wie
     FUN_800430bc (@0x800430c4 Nutzlastzeiger -> 0x800ac9a8, @0x800430d4 0x800b5359 = 1, @0x800430e4 0x800aca40 |=
     0xff000000): 12-B-Nutzlast (x,y,z,dir,stage,raum,cut,floor) an der Stelle der Nutzlast des Door_aot_set Slot 0
     des aktuellen Raums (ROOM30C1 @0x00B5A, Ziel 3091 = Stage 2 Raum 9).
  4. Nach der Ankunft: Spieler vor die Riegeltuer (Slot 1, Rechteck (-25610,1300)+(1890,750)) stellen und Kreuz
     druecken (vgamepad, A) -> sub01 -> sub02 -> Ck(9,21) -> sub07 (Szene). Danach alle --takt Bilder Kreuz.
  5. Je Bild: Zaehler, VSync 0x800787dc, Raum 0x800b0fe2, Cut 0x800b0fe4, Flag (4,16); Spieler 0x800aca54 und
     Gegner 0..2 (0x800acc2c + i*0x1F4): +0x00 +0x04 +0x05 +0x06 +0x08 +0x8f +0x94 +0x95 +0x168 +0x16c +0x170 +0x174
     +0x180 +0x184 +0x1c4 Lage +0x34/+0x3c Gier +0x6a.
Aufruf: k2_orig_3091.py --state <sav> --out <log> [--bilder 6000] [--takt 40]
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil, hashlib, re

DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
RES = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "savestates", "HASH-957757946319438E_resume.sav")

ap = argparse.ArgumentParser()
ap.add_argument("--state", required=True)
ap.add_argument("--out", required=True)
ap.add_argument("--bilder", type=int, default=6000)
ap.add_argument("--takt", type=int, default=40)
ap.add_argument("--spieler", default="-24665,0,1000,3072")   # x,y,z,dir vor der Riegeltuer (Slot 1)
ap.add_argument("--ohne_pad", action="store_true")
a = ap.parse_args()

def md5(p):
    return hashlib.md5(open(p, "rb").read()).hexdigest() if os.path.exists(p) else None

r0 = subprocess.run(["tasklist", "/FI", "IMAGENAME eq duckstation-qt-x64-ReleaseLTCG.exe", "/NH"], capture_output=True)
if b"duckstation" in r0.stdout.lower():
    sys.exit("ABBRUCH: ein anderes DuckStation laeuft bereits (andere Spur)")
ini_alt = open(INI, "rb").read()
assert b"EnableGDBServer = false" in ini_alt, "settings.ini: EnableGDBServer nicht false (laeuft eine andere Messung?)"
res_md5 = md5(RES)
res_kopie = a.out + ".resume_vorher.sav"
if res_md5:
    shutil.copyfile(RES, res_kopie)
open(INI, "wb").write(ini_alt.replace(b"EnableGDBServer = false", b"EnableGDBServer = true", 1)
                      .replace(b"ConfirmPowerOff = true", b"ConfirmPowerOff = false", 1))

out = open(a.out, "w", encoding="utf-8")
def log(*x):
    s_ = " ".join(str(v) for v in x)
    print(s_, flush=True)
    out.write(s_ + "\n"); out.flush()

sock = None; buf = b""
def send(c):
    k = sum(c.encode()) & 0xff
    sock.sendall(b"$" + c.encode() + b"#%02x" % k)
def recv_pkt():
    global buf
    while True:
        i = buf.find(b"$")
        if i >= 0:
            j = buf.find(b"#", i)
            if j >= 0 and len(buf) >= j + 3:
                p = buf[i + 1:j]; buf = buf[j + 3:]
                sock.sendall(b"+")
                return p.decode(errors="replace")
        d = sock.recv(65536)
        if not d: raise EOFError
        buf += d
def cmd(c):
    send(c); return recv_pkt()
def rd(addr, n):
    r = b""
    while n > 0:
        k = min(n, 0x800)
        r += bytes.fromhex(cmd("m%08x,%x" % (addr, k)))
        addr += k; n -= k
    return r
def u8(x): return rd(x, 1)[0]
def u16(x): return struct.unpack("<H", rd(x, 2))[0]
def u32(x): return struct.unpack("<I", rd(x, 4))[0]
def wr(addr, data):
    r = cmd("M%08x,%x:%s" % (addr, len(data), data.hex())); assert r == "OK", r
def regs():
    g = cmd("g")
    return [struct.unpack("<I", bytes.fromhex(g[i * 8:i * 8 + 8]))[0] for i in range(38)]
def flag_set(bank_base, bit):
    w = bank_base + (bit >> 5) * 4
    v = u32(w) | (0x80000000 >> (bit & 31)); wr(w, struct.pack("<I", v)); return v
def flag_get(bank_base, bit):
    return (u32(bank_base + (bit >> 5) * 4) >> (31 - (bit & 31))) & 1

FELDER = [(0x00, 4), (0x04, 1), (0x05, 1), (0x06, 1), (0x08, 1), (0x8f, 1), (0x94, 1), (0x95, 1), (0x168, 4),
          (0x16c, 4), (0x170, 4), (0x174, 4), (0x180, 4), (0x184, 4), (0x1c4, 2), (0x34, 4), (0x3c, 4), (0x6a, 2)]
def entity(base):
    blk = rd(base, 0x1f4)
    v = {}
    for off, n in FELDER:
        v[off] = int.from_bytes(blk[off:off + n], "little", signed=(off in (0x34, 0x3c)))
    return v

gp = None
if not a.ohne_pad:
    import vgamepad as vg
    gp = vg.VX360Gamepad(); time.sleep(2.0)
    A_BTN = vg.XUSB_BUTTON.XUSB_GAMEPAD_A   # Kreuz: Nachricht schliessen (virt. 0x8000 <- raw CROSS, Preset @0x80073dbc)
    Q_BTN = vg.XUSB_BUTTON.XUSB_GAMEPAD_X   # Viereck: Untersuchen/AOT (raw 0x80 -> FUN_80042bac, virt. 0x4000)
proc = None
try:
    import ctypes
    lang = ctypes.create_unicode_buffer(1024)
    ctypes.windll.kernel32.GetLongPathNameW(os.path.abspath(a.state), lang, 1024)
    zustand = lang.value or os.path.abspath(a.state)
    log("Spielstand", zustand, "md5", md5(zustand))
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", zustand, CUE])
    for k in range(240):
        try:
            sock = socket.create_connection(("127.0.0.1", 2345), timeout=2); break
        except OSError:
            time.sleep(0.5)
    if sock is None: raise SystemExit("keine GDB-Verbindung")
    sock.settimeout(60)
    log("?", cmd("?"))
    log("EXE-Stub @0x80026e4c", rd(0x80026e4c, 4).hex(), "(sauber = 0800e003)",
        "Modell DAT_800aca5c", u8(0x800aca5c), "Waffe DAT_800aca5d", u8(0x800aca5d))
    BP = 0x8001ce04
    log("Z0 %08x" % BP, cmd("Z0,%08x,4" % BP))
    n = 0; angekommen = None; szene = None; gedrueckt = None; naechster = None; t0 = time.time()
    while n < a.bilder:
        send("c"); recv_pkt()
        n += 1
        raum = u16(0x800b0fe2); stage = u16(0x800b0fe0)
        if n == 20:
            v1 = flag_set(0x800b1018, 8); v2 = flag_set(0x800b1078, 21)
            log("FLAGS (4,8)=1 -> Wort %08x, (9,21)=1 -> Wort %08x" % (v1, v2))
            rdtp = u32(0x800ac778)
            off = 0x00B5A   # ROOM30C1 Door_aot_set Slot 0 (Ziel 3091 Cut 0)
            x, y, z, d = [int(t) for t in a.spieler.split(",")]
            nl = struct.pack("<hhhhBBBB", x, y, z, d, 2, 9, 0, 0)
            wr(rdtp + off + 14, nl)
            wr(0x800ac9a8, struct.pack("<I", rdtp + off + 14))
            wr(0x800b5359, b"\x01")
            pf = u32(0x800aca40); wr(0x800aca40, struct.pack("<I", pf | 0xff000000))
            log("SPRUNG bei Bild %d: Raum %d%02X -> 3091, Nutzlast %s @%08x" % (n, stage + 1, raum, nl.hex(), rdtp + off + 14))
            continue
        if angekommen is None and n > 20 and stage == 2 and raum == 9:
            angekommen = n; log("ANKUNFT 3091 bei Bild %d (VSync %d)" % (n, u32(0x800787dc)))
        if angekommen is not None and szene is None and gp is not None:
            if gedrueckt is None and n - angekommen >= 60:
                gp.press_button(button=Q_BTN); gp.update(); gedrueckt = n
                log("VIERECK an der Riegeltuer bei Bild %d" % n)
            elif gedrueckt is not None and n - gedrueckt == 8:
                gp.release_button(button=Q_BTN); gp.update()
            elif gedrueckt is not None and n - gedrueckt > 90:
                gedrueckt = None   # erneut versuchen, Spieler um 90 Grad weiterdrehen (Blickrichtung zur Tuer unbekannt)
                d0 = u16(0x800aca54 + 0x6a); wr(0x800aca54 + 0x6a, struct.pack("<H", (d0 + 0x400) & 0xfff))
                log("Gier %d -> %d" % (d0, (d0 + 0x400) & 0xfff))
        if szene is None and angekommen is not None and flag_get(0x800b1018, 16):
            szene = n; naechster = n + a.takt
            log("SZENE (4,16)=1 bei Bild %d (VSync %d)" % (n, u32(0x800787dc)))
        if szene is not None:
            if gp is not None:
                if n == naechster:
                    gp.press_button(button=A_BTN); gp.update()
                elif n == naechster + 4:
                    gp.release_button(button=A_BTN); gp.update(); naechster += a.takt
            pl = entity(0x800aca54)
            zeile = ["B%d" % (n - szene), "vs%d" % u32(0x800787dc), "R%d%02X" % (stage + 1, raum), "cut%d" % u16(0x800b0fe4)]
            for nm, base in (("S", 0x800aca54), ("E0", 0x800acc2c), ("E1", 0x800acc2c + 0x1f4), ("E2", 0x800acc2c + 0x3e8)):
                e = pl if nm == "S" else entity(base)
                zeile.append("%s:t%02x st%d sub%d ph%d clip%d f%d f8f%d fl%04x w168=%08x w16c=%08x w170=%08x w174=%08x "
                             "w180=%08x w184=%08x pos(%d,%d) dir%d" % (
                    nm, e[0x08], e[0x04], e[0x05], e[0x06], e[0x94], e[0x95], e[0x8f], e[0x1c4], e[0x168], e[0x16c],
                    e[0x170], e[0x174], e[0x180], e[0x184], e[0x34], e[0x3c], e[0x6a]))
            log(" | ".join(zeile))
            if stage == 2 and raum == 0x0c:
                log("RAUMWECHSEL nach 30C1 bei Szenenbild %d -> Ende" % (n - szene)); break
        if time.time() - t0 > 1500:
            log("ZEITGRENZE"); break
    cmd("z0,%08x,4" % BP)
    if szene is None: log("SZENE NICHT AUSGELOEST")
    rdtp = u32(0x800ac778)
    log("RDT im RAM @%08x, +0x5C (RBJ) = %08x" % (rdtp, u32(rdtp + 0x5c)))
finally:
    try:
        if sock is not None:
            try: send("D"); recv_pkt()
            except Exception: pass
            sock.close()
    except Exception: pass
    if gp is not None:
        try: gp.reset(); gp.update()
        except Exception: pass
    if proc is not None:
        subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
        for _ in range(60):
            if proc.poll() is not None: break
            time.sleep(0.5)
        if proc.poll() is None:
            subprocess.run(["taskkill", "/F", "/PID", str(proc.pid)], capture_output=True)
    time.sleep(2.0)
    open(INI, "wb").write(ini_alt)
    log("settings.ini zurueck:", open(INI, "rb").read() == ini_alt)
    if res_md5:
        shutil.copyfile(res_kopie, RES)
        log("resume.sav zurueck:", md5(RES) == res_md5)
    out.close()
