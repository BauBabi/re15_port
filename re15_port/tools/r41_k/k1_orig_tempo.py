#!/usr/bin/env python3
"""Runde 41 Spur K: ORIGINAL (DuckStation-GDB) - Gehen/Rennen der Spielfigur SHERRY (Modell 0x0F) in ROOM30C1 je Bild
mitschreiben (Lage 0x800aca88/0x800aca90, Tempo DAT_800acae0, Clip +0x94/+0x95, Zustand). Belegt die Tabellen-Zeile 15
(GEHEN @0x80073ec2 = [50,48], RENNEN @0x80073f42 = [120,64]) am laufenden Original.
(Werkzeug-Kopf aus k2_orig_3091.py.)

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
ap.add_argument("--rennen_zuerst", action="store_true")
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
import vgamepad as vg
gp = vg.VX360Gamepad(); time.sleep(2.0)
UP = vg.XUSB_BUTTON.XUSB_GAMEPAD_DPAD_UP
KR = vg.XUSB_BUTTON.XUSB_GAMEPAD_A
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
    log("EXE-Stub", rd(0x80026e4c, 4).hex(), "Modell", u8(0x800aca5c), "Waffe", u8(0x800aca5d))
    BP = 0x8001ce04
    log("Z0", cmd("Z0,%08x,4" % BP))
    n = 0; lx = lz = None
    while n < (90 if a.rennen_zuerst else 330):
        send("c"); recv_pkt(); n += 1
        if a.rennen_zuerst:
            if n == 30: gp.press_button(button=UP); gp.press_button(button=KR); gp.update(); log("RENNEN an (hoch + Kreuz) Bild", n)
            if n == 50: gp.release_button(button=UP); gp.release_button(button=KR); gp.update(); log("RENNEN aus Bild", n)
        else:
            if n == 30: gp.press_button(button=UP); gp.update(); log("GEHEN an (Steuerkreuz hoch) Bild", n)
            if n == 120: gp.release_button(button=UP); gp.update(); log("GEHEN aus Bild", n)
            if n == 170: gp.press_button(button=UP); gp.press_button(button=KR); gp.update(); log("RENNEN an (hoch + Kreuz) Bild", n)
            if n == 260: gp.release_button(button=UP); gp.release_button(button=KR); gp.update(); log("RENNEN aus Bild", n)
        x = struct.unpack("<i", rd(0x800aca88, 4))[0]; z = struct.unpack("<i", rd(0x800aca90, 4))[0]
        sp = struct.unpack("<h", rd(0x800acae0, 2))[0]
        d = "-" if lx is None else "%.1f" % (((x - lx) ** 2 + (z - lz) ** 2) ** 0.5)
        lx, lz = x, z
        log("B%d vs%d pos(%d,%d) schritt %s tempo(acae0) %d clip %d f %d st %d sub %d/%d dir %d" % (
            n, u32(0x800787dc), x, z, d, sp, u8(0x800aca54 + 0x94), u8(0x800aca54 + 0x95), u8(0x800aca58),
            u8(0x800aca59), u8(0x800aca5a), u16(0x800aca54 + 0x6a)))
    cmd("z0,%08x,4" % BP)
finally:
    try:
        if sock is not None:
            try: send("D"); recv_pkt()
            except Exception: pass
            sock.close()
    except Exception: pass
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
