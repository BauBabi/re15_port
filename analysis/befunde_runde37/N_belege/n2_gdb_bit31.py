#!/usr/bin/env python3
"""Runde 37 Spur N — Diagnose: wo verliert der kuenstliche Stage-Wechsel (Tuer-Nutzlast per GDB) das Elza-Bit?

Gleiches Verfahren wie n2_gdb_elza.py (Spielstand-Kopie, GDB 2345, settings.ini zurueck, resume.sav des Nutzers
bytegleich zurueck). Haltepunkte mit Lesen von DAT_800aca3c (Bit 31 = Elza) und DAT_800aca5c (Spielermodell):
  0x8001d870  FUN_8001d600 Tuerzweig: Nutzlast lesen        0x8001d988  `jal 0x800396fc` (nach Stage-Laden @0x8001d980)
  0x800397e4  Raumlader `srl a0,a0,31` (RDT-Variante)        0x80022300  Modell-Lader
Aufruf: n2_gdb_bit31.py --state <sav> --out <txt> --sprung <nutzlast-hex>
"""
import socket, subprocess, time, sys, struct, argparse, os, shutil, hashlib, re, ctypes

BAUM = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
DUCK = r"C:\Users\mjoedicke\AppData\Local\Programs\DuckStation\duckstation-qt-x64-ReleaseLTCG.exe"
CUE = r"C:\Users\mjoedicke\Downloads\ePSXe2018\Biohazard 1.5 (MZD Mod) Update 25-01-2025.cue"
INI = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "settings.ini")
RES = os.path.join(os.environ["LOCALAPPDATA"], "DuckStation", "savestates", "HASH-957757946319438E_resume.sav")
ap = argparse.ArgumentParser()
ap.add_argument("--state", required=True)
ap.add_argument("--out", required=True)
ap.add_argument("--sprung", required=True)
ap.add_argument("--extra", default="", help="weitere Haltepunkte hex, Komma")
a = ap.parse_args()
PSX = os.path.join(BAUM, 're15_port/shared_assets/PSX/')


def door_off(stage_idx, room, variante):
    fn = os.path.join(PSX, 'STAGE%d' % (stage_idx + 1), 'ROOM%d%02X%d.RDT' % (stage_idx + 1, room, variante))
    txt = subprocess.run([sys.executable, os.path.join(BAUM, 're15_port/tools/scd_dump_room.py'), fn],
                         capture_output=True, text=True).stdout
    m = re.search(r'0x([0-9A-F]+) \(\+[0-9A-F]+\) Door_aot_set', txt)
    return int(m.group(1), 16), os.path.basename(fn)


def md5(p):
    return hashlib.md5(open(p, 'rb').read()).hexdigest() if os.path.exists(p) else None


ini_alt = open(INI, "r", encoding="utf-8", errors="replace").read()
assert "EnableGDBServer = false" in ini_alt
res_md5 = md5(RES)
res_kopie = a.out + ".resume_vorher.sav"
if res_md5:
    shutil.copyfile(RES, res_kopie)
open(INI, "w", encoding="utf-8").write(ini_alt.replace("EnableGDBServer = false", "EnableGDBServer = true", 1))
out = open(a.out, "w", encoding="utf-8")


def log(*x):
    s_ = " ".join(str(v) for v in x)
    print(s_, flush=True)
    out.write(s_ + "\n")
    out.flush()


sock = None
buf = b""


def send(c):
    sock.sendall(b"$" + c.encode() + b"#%02x" % (sum(c.encode()) & 0xff))


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


def rd(addr, n): return bytes.fromhex(cmd("m%08x,%x" % (addr, n)))
def u8(x): return rd(x, 1)[0]
def u16(x): return struct.unpack("<H", rd(x, 2))[0]
def u32(x): return struct.unpack("<I", rd(x, 4))[0]


def wr(addr, data):
    assert cmd("M%08x,%x:%s" % (addr, len(data), data.hex())) == "OK"


def regs():
    g = cmd("g")
    return [struct.unpack("<I", bytes.fromhex(g[i * 8:i * 8 + 8]))[0] for i in range(38)]


proc = None
try:
    lang = ctypes.create_unicode_buffer(1024)
    ctypes.windll.kernel32.GetLongPathNameW(os.path.abspath(a.state), lang, 1024)
    proc = subprocess.Popen([DUCK, "-batch", "-statefile", lang.value or os.path.abspath(a.state), CUE])
    for k in range(240):
        try:
            sock = socket.create_connection(("127.0.0.1", 2345), timeout=2)
            break
        except OSError:
            time.sleep(0.5)
    open(INI, "w", encoding="utf-8").write(ini_alt)
    if sock is None:
        raise SystemExit("keine Verbindung")
    sock.settimeout(60)
    log("?", cmd("?"), "aca3c %08x aca5c %02x" % (u32(0x800aca3c), u8(0x800aca5c)))
    nl = bytes.fromhex(a.sprung)
    cur = (u16(0x800b0fe0), u16(0x800b0fe2))
    off, fn = door_off(cur[0], cur[1], u32(0x800aca3c) >> 31)
    rdtp = u32(0x800ac778)
    wr(rdtp + off + 14, nl)
    wr(0x800ac9a8, struct.pack("<I", rdtp + off + 14))
    wr(0x800b5359, b"\x01")
    wr(0x800aca40, struct.pack("<I", u32(0x800aca40) | 0xff000000))
    log("SPRUNG aus %s -> Nutzlast %s" % (fn, nl.hex()))
    BP = [0x8001d870, 0x8001d988, 0x800397e4, 0x80022300] + [int(x, 16) for x in a.extra.split(",") if x]
    for b in BP:
        cmd("Z0,%08x,4" % b)
    t0 = time.time()
    n = 0
    while time.time() - t0 < 300:
        send("c")
        recv_pkt()
        r = regs()
        pc = r[37]
        log("HALT %08x  aca3c %08x  aca5c %02x  stage %d room %02x  a0 %08x v0 %08x  ra %08x" % (
            pc, u32(0x800aca3c), u8(0x800aca5c), u16(0x800b0fe0), u16(0x800b0fe2), r[4], r[2], r[31]))
        if pc == 0x80022300:
            n += 1
            if n >= 1:
                break
    for b in BP:
        cmd("z0,%08x,4" % b)
    send("c")
    time.sleep(1.0)
finally:
    if proc is not None:
        subprocess.run(["taskkill", "/PID", str(proc.pid)], capture_output=True)
        for _ in range(60):
            time.sleep(0.5)
            if proc.poll() is not None:
                break
        if proc.poll() is None:
            subprocess.run(["taskkill", "/F", "/PID", str(proc.pid)], capture_output=True)
        time.sleep(1.0)
    open(INI, "w", encoding="utf-8").write(ini_alt)
    if res_md5:
        shutil.copyfile(res_kopie, RES)
    log("settings.ini zurueck:", open(INI, encoding="utf-8", errors="replace").read() == ini_alt,
        "| resume.sav des Nutzers zurueck:", md5(RES) == res_md5)
    out.close()
