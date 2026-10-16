#!/usr/bin/env python3
"""Runde 37 Spur N — ORIGINAL (DuckStation-GDB, MZD-Disc, saubere EXE): was laedt FUN_80022300 fuer Elza?

Muster: Runde 36 F_belege/nb4/gdb_affen.py (Spur J jnb6/gdbm2.py). Ablauf:
  1. Elza-Spielstand (Kopie) direkt laden (-batch -statefile), GDB-Server Port 2345 nur fuer den Lauf an
     (settings.ini [Debug] EnableGDBServer; vorher geprueft, nach dem Prozessende bytegleich zurueckgeschrieben).
  2. Tuerwechsel wie FUN_800430bc (@0x800430c4 sw a0 -> 0x800ac9a8, @0x800430d4 sb 1 -> 0x800b5359,
     @0x800430e4 g_pauseflags 0x800aca40 |= 0xff000000): Ziel-Nutzlast (12 B: x,y,z,dir,stage,room,cut,floor —
     gelesen von FUN_8001d600 @0x8001d87c-d970) steht an der Stelle der Nutzlast eines Door_aot_set des AKTUELLEN
     Raums (RDT im RAM ab *0x800ac778; derselbe Speicher, aus dem das Spiel selbst liest).
  3. Haltepunkte: 0x80022300 (Eintritt Lader: a2 = Entity, Typ +8, Elza-Bit 0x800aca3c),
     0x80022470 (nach dem CD-Lesen `jal 0x80013c50` @0x80022468: s4 = Blob im RAM, Datei-Id 0x800be572 =
     0x26/0x27 @0x8002234c/@0x800223bc, TOC-Laenge 0x800be574, Sektor-Feld 0x800be57c), 0x8001ce04 (Bildtakt,
     `jal 0x8001a50c` der Hauptschleife).
     Je Laderaufruf: Blob im RAM (TOC-Laenge) == CDEMD0-Blob? == CDEMD1-Blob? (TOC @0x80072f38 / @0x80073178)
  4. Optional: am Ende geordnet schliessen (SaveStateOnExit) -> resume.sav wird KOPIERT (Bild + RAM des
     Endstands), die Datei des Nutzers danach bytegleich zurueckgelegt (md5 geprueft).
Aufruf: n2_gdb_elza.py --state <sav> --out <txt> --sprung <nutzlast-hex>:<bilder> [...] [--endstand <sav-ziel>]
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil, hashlib, re

BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
RES = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "savestates", "HASH-957757946319438E_resume.sav")

ap = argparse.ArgumentParser()
ap.add_argument("--state", required=True)
ap.add_argument("--out", required=True)
ap.add_argument("--sprung", action="append", default=[])
ap.add_argument("--start_bilder", type=int, default=30)
ap.add_argument("--endstand", default=None)
ap.add_argument("--endstand_halt", action="store_true")
ap.add_argument("--setze", action="append", default=[],
                help="NUR fuer Bildbelege: <hexadresse>:<hexbytes>, einmal nach dem letzten Sprung geschrieben")
ap.add_argument("--nach_bilder", type=int, default=0, help="Bilder nach --setze bis zum Ende")
a = ap.parse_args()

EXE = open(os.path.join(BAUM, 'info/Re1.5/PSX.EXE'), 'rb').read()
T = struct.unpack_from('<I', EXE, 0x18)[0]


def xr(addr, n):
    o = 0x800 + addr - T
    return EXE[o:o + n]


PSX = os.path.join(BAUM, 're15_port/shared_assets/PSX/')
EMS = {0: open(PSX + 'EMD/CDEMD0.EMS', 'rb').read(), 1: open(PSX + 'EMD/CDEMD1.EMS', 'rb').read()}
TOC = {0: 0x80072f38, 1: 0x80073178}


def blob(arch, t):
    s, z = struct.unpack_from('<II', xr(TOC[arch] + (t - 0x10) * 8, 8))
    return EMS[arch][s * 2048:s * 2048 + z] if z else b''


def door_off(stage_idx, room, variante=1):
    fn = os.path.join(PSX, 'STAGE%d' % (stage_idx + 1), 'ROOM%d%02X%d.RDT' % (stage_idx + 1, room, variante))
    txt = subprocess.run([sys.executable, os.path.join(BAUM, 're15_port/tools/scd_dump_room.py'), fn],
                         capture_output=True, text=True).stdout
    m = re.search(r'0x([0-9A-F]+) \(\+[0-9A-F]+\) Door_aot_set', txt)
    return int(m.group(1), 16), os.path.basename(fn)


def md5(p):
    return hashlib.md5(open(p, 'rb').read()).hexdigest() if os.path.exists(p) else None


ini_alt = open(INI, "r", encoding="utf-8", errors="replace").read()
assert "EnableGDBServer = false" in ini_alt, "settings.ini: EnableGDBServer nicht false (laeuft eine andere Messung?)"
res_md5 = md5(RES)
res_kopie = a.out + ".resume_vorher.sav"
if res_md5:
    shutil.copyfile(RES, res_kopie)
# Nur fuer den Lauf: GDB-Server an; ConfirmPowerOff aus (sonst haelt die Rueckfrage das geordnete Schliessen auf
# und es gibt keinen Endstand). Beides wird nach dem Prozessende bytegleich zurueckgeschrieben (ini_alt).
open(INI, "w", encoding="utf-8").write(ini_alt.replace("EnableGDBServer = false", "EnableGDBServer = true", 1)
                                       .replace("ConfirmPowerOff = true", "ConfirmPowerOff = false", 1))


def ini_zurueck():
    open(INI, "w", encoding="utf-8").write(ini_alt)


out = open(a.out, "w", encoding="utf-8")


def log(*x):
    s_ = " ".join(str(v) for v in x)
    print(s_, flush=True)
    out.write(s_ + "\n")
    out.flush()


proc = None
sock = None
buf = b""


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
                p = buf[i + 1:j]
                buf = buf[j + 3:]
                sock.sendall(b"+")
                return p.decode(errors="replace")
        d = sock.recv(65536)
        if not d:
            raise EOFError
        buf += d


def cmd(c):
    send(c)
    return recv_pkt()


def rd(addr, n):
    r = b""
    while n > 0:
        k = min(n, 0x800)
        r += bytes.fromhex(cmd("m%08x,%x" % (addr, k)))
        addr += k
        n -= k
    return r


def u8(x): return rd(x, 1)[0]
def u16(x): return struct.unpack("<H", rd(x, 2))[0]
def u32(x): return struct.unpack("<I", rd(x, 4))[0]


def wr(addr, data):
    r = cmd("M%08x,%x:%s" % (addr, len(data), data.hex()))
    assert r == "OK", r


def regs():
    g = cmd("g")
    return [struct.unpack("<I", bytes.fromhex(g[i * 8:i * 8 + 8]))[0] for i in range(38)]


try:
    import ctypes
    lang = ctypes.create_unicode_buffer(1024)
    ctypes.windll.kernel32.GetLongPathNameW(os.path.abspath(a.state), lang, 1024)
    zustand = lang.value or os.path.abspath(a.state)
    log("Spielstand", zustand)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", zustand, CUE])
    for k in range(240):
        try:
            sock = socket.create_connection(("127.0.0.1", 2345), timeout=2)
            break
        except OSError:
            time.sleep(0.5)
            if k % 20 == 0:
                log("warte auf GDB", k, "proc.poll", proc.poll())
    if sock is None:
        raise SystemExit("keine Verbindung")
    sock.settimeout(60)
    log("?", cmd("?"))
    log("EXE-Stub @0x80026e4c", rd(0x80026e4c, 4).hex(), "(sauber = 0800e003)")
    BP = (0x80022300, 0x80022470, 0x8001ce04)
    for b in BP:
        log("Z0 %08x" % b, cmd("Z0,%08x,4" % b))
    bilder = 0
    ziel = None
    sprung_i = 0
    warte = a.start_bilder
    seit = 0
    pend = {}
    t0 = time.time()
    while True:
        send("c")
        recv_pkt()
        pc = regs()[37]
        if pc == 0x80022300:
            r = regs()
            ent = r[6]
            pend = {"ent": ent, "typ": u8(ent + 8), "elza": u32(0x800aca3c) >> 31, "p97": u8(ent + 0x97),
                    "vs": u32(0x800787dc)}
            continue
        if pc == 0x80022470:
            r = regs()
            s4 = r[20]
            typ = pend.get("typ", -1)
            fid = u16(0x800be572)
            tl = u32(0x800be574)
            ts = u32(0x800be57c)
            ram = rd(s4, tl) if 0 < tl < 0x60000 else b""
            b0 = blob(0, typ) if 0x10 <= typ < 0x50 else b""
            b1 = blob(1, typ) if 0x10 <= typ < 0x50 else b""
            raum = "%d%02X(Variante %d)" % (u16(0x800b0fe0) + 1, u16(0x800b0fe2), u32(0x800aca3c) >> 31)
            log("LADER Raum %s VSync %d Entity %08x Typ 0x%02X +0x97=%d Elza-Bit %d | Datei-Id 0x%02X TOC-Laenge %d "
                "Sektor-Feld %d | Blob im RAM @%08x == CDEMD0: %s == CDEMD1: %s" % (
                    raum, pend.get("vs", 0), pend.get("ent", 0), typ, pend.get("p97", -1), pend.get("elza", -1),
                    fid, tl, ts, s4, ram == b0, ram == b1))
            pend = {}
            continue
        bilder += 1
        seit += 1
        if ziel is not None and seit == 1:
            pass
        if ziel is not None and u16(0x800b0fe2) == ziel[1] and u16(0x800b0fe0) == ziel[0] and not ziel[2]:
            ziel = (ziel[0], ziel[1], True)
            log("Raum %d%02X erreicht bei Bild %d (VSync %d)" % (ziel[0] + 1, ziel[1], bilder, u32(0x800787dc)))
        if seit >= warte:
            if sprung_i >= len(a.sprung):
                if a.setze and not getattr(a, "_gesetzt", False):
                    for sz in a.setze:
                        ad, hx = sz.split(":")
                        wr(int(ad, 16), bytes.fromhex(hx))
                        log("SETZE %s := %s (Bildbeleg, kein Spielverhalten)" % (ad, hx))
                    a._gesetzt = True
                    warte = a.nach_bilder
                    seit = 0
                    continue
                break
            nl, n_bilder = a.sprung[sprung_i].split(":")
            nl = bytes.fromhex(nl)
            sprung_i += 1
            cur = (u16(0x800b0fe0), u16(0x800b0fe2))
            off, fn = door_off(cur[0], cur[1], u32(0x800aca3c) >> 31)
            rdtp = u32(0x800ac778)
            ziel = (nl[8], nl[9], False)
            wr(rdtp + off + 14, nl)
            wr(0x800ac9a8, struct.pack("<I", rdtp + off + 14))
            wr(0x800b5359, b"\x01")
            pf = u32(0x800aca40)
            wr(0x800aca40, struct.pack("<I", pf | 0xff000000))
            log("SPRUNG %d bei Bild %d: aus %s (Door_aot_set @0x%05X, Nutzlast @%08x) -> Stage-Index %d Raum 0x%02X "
                "Cut %d, Nutzlast %s" % (sprung_i, bilder, fn, off, rdtp + off + 14, nl[8], nl[9], nl[10], nl.hex()))
            warte = int(n_bilder)
            seit = 0
        if time.time() - t0 > 900:
            log("ZEITGRENZE")
            break
    for b in BP:
        cmd("z0,%08x,4" % b)
    for i in range(20):
        e = 0x800acc2c + i * 0x1f4
        if u32(e) == 0:
            continue
        log("ENDE Entity %2d Typ 0x%02X Lage (%d,%d) +0x16c %08x +0x10 %08x +0x94 %d" % (
            i, u8(e + 8), struct.unpack("<i", rd(e + 0x34, 4))[0], struct.unpack("<i", rd(e + 0x3c, 4))[0],
            u32(e + 0x16c), u32(e + 0x10), u8(e + 0x94)))
    log("ENDE Spieler Lage (%d,%d) Raum %d%02X Cut %d" % (
        struct.unpack("<i", rd(0x800aca54 + 0x34, 4))[0], struct.unpack("<i", rd(0x800aca54 + 0x3c, 4))[0],
        u16(0x800b0fe0) + 1, u16(0x800b0fe2), u16(0x800b0fe4)))
    # Endstand: im GDB-HALT (verbunden) geordnet schliessen — so wie der Lauf, dessen duckstation.log
    # "Setting pending main window close flag" -> "Saving state" zeigt (Runde 36 R).
    if a.endstand_halt:
        pass
    else:
        send("c")
        time.sleep(4.0)
finally:
    if proc is not None:
        subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
        for _ in range(180):
            time.sleep(0.5)
            if proc.poll() is not None:
                break
        log("Prozessende:", "geordnet" if proc.poll() is not None else "ERZWUNGEN")
        if proc.poll() is None:
            subprocess.run(["taskkill", "/F", "/PID", str(proc.pid)], capture_output=True)
        time.sleep(1.0)
    ini_zurueck()
    try:
        sock.close()
    except Exception:
        pass
    if a.endstand and os.path.exists(RES) and md5(RES) != res_md5:
        shutil.copyfile(RES, a.endstand)
        log("Endstand kopiert ->", a.endstand)
    if res_md5:
        shutil.copyfile(res_kopie, RES)
    log("settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace").read() == ini_alt,
        "| resume.sav des Nutzers zurueck:", md5(RES) == res_md5)
    out.close()
