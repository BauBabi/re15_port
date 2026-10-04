"""re2_gdb_push.py — WER schreibt Leons x/z nach dem Pin? (RE2-Original, DuckStation-GDB)
Haltepunkt auf B4 P0 des Arms (EM2D @0x80100BC4, Griff-Bild), dann Schreib-Wachpunkt (Z2) auf PL.x/PL.z
(0x800CFC30/0x800CFC38) und je Treffer PC + RA + Wert protokollieren.
Aufruf: R2_ONLY=<Satz> python re2_gdb_push.py <spielstand> <ausgabe> <x> <z> <yaw> [treffer]
Runde 35 Spur H, Nachbesserung 3."""
import os, struct, subprocess, sys, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import re2_gdb_grab as G

def regs(g):
    r = g.cmd("g")
    w = [struct.unpack("<I", bytes.fromhex(r[i:i + 8].decode()))[0] for i in range(0, len(r) - 7, 8)]
    return w   # MIPS-GDB: r0..r31, sr, lo, hi, bad, cause, pc (Index 37)

def main():
    state, out = os.path.normpath(sys.argv[1]), os.path.abspath(sys.argv[2])
    lx, lz, lyaw = int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5])
    nmax = int(sys.argv[6]) if len(sys.argv) > 6 else 12
    os.makedirs(out, exist_ok=True)
    log = open(os.path.join(out, "push_log.txt"), "w")
    def L(*a):
        s = " ".join(str(x) for x in a); print(s, flush=True); log.write(s + "\n"); log.flush()
    vor = G.pids()
    subprocess.Popen([G.DUCK, "-batch", "-statefile", state, G.CUE], cwd=out)
    eigene = set()
    for _ in range(15):
        time.sleep(1); eigene = G.pids() - vor
        if eigene: break
    try:
        g = G.Gdb(G.PORT); time.sleep(6); g.sk.sendall(b"\x03"); time.sleep(0.5); L("Stopp", g.cmd("?"))
        PL = G.PL
        for a, v in ((0x38, lx), (0x40, lz)): g.wmem(PL + a, struct.pack("<i", v))
        for a, v in ((0x44, lx), (0x48, lz), (0x118, lx), (0x11A, lz), (0x76, lyaw)): g.wmem(PL + a, struct.pack("<h", v))
        only = os.getenv("R2_ONLY")
        if only is not None:
            for k in range(1, 33):
                p = g.u32(G.ETAB + 4 * k)
                if 0x80000000 <= p < 0x80200000 and p != 0x800D424C and g.mem(p + 8, 1)[0] == 0x2D:
                    if g.mem(p + 0xC, 1)[0] - 2 != int(only):
                        g.wmem(p + 4, struct.pack("<I", 0x00000701))
        L("Z0 B4P0:", g.cmd("Z0,%x,4" % 0x80100BC4))
        g.sk.sendall(b"$c#63"); L("Griff-Bild:", g.recv(120))
        L("vor dem Pin PL x/z", g.s32(PL + 0x38), g.s32(PL + 0x40))
        g.cmd("z0,%x,4" % 0x80100BC4)
        L("Z2 x:", g.cmd("Z2,%x,4" % (PL + 0x38)), "Z2 z:", g.cmd("Z2,%x,4" % (PL + 0x40)))
        for i in range(nmax):
            g.sk.sendall(b"$c#63"); r = g.recv(60)
            w = regs(g)
            L("Treffer %d: %s pc=%08x ra=%08x | PL x=%d z=%d (+0x44 %d +0x48 %d)" % (
                i, r.decode(errors="replace"), w[37] if len(w) > 37 else 0, w[31], g.s32(PL + 0x38), g.s32(PL + 0x40),
                g.s16(PL + 0x44), g.s16(PL + 0x48)))
        g.cmd("z2,%x,4" % (PL + 0x38)); g.cmd("z2,%x,4" % (PL + 0x40))
        g.sk.sendall(b"$c#63")
    finally:
        for p in eigene: subprocess.run(["taskkill", "/PID", p, "/F"], capture_output=True)

if __name__ == "__main__":
    main()
