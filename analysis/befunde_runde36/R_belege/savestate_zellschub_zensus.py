"""savestate_zellschub_zensus.py — Runde 36 Spur R, Nachbesserung 3 (P1): der Ruhe-Zweig LAB_8002c0b8 des
Objekt-Passes (FUN_8003b558(obj,2) + Ausschiebung FUN_8003da78) gegen die ORIGINAL-Spielstaende.

Je DuckStation-Savestate (stage_saves/*.sav, RAM ueber re15_ss.py) wird der Objekt-Pool 0x800b3f98 (Stride 0x94,
Anzahl = RDT-Kopf[+2] ueber DAT_800ac778) gelesen: Lage +0x34/+0x38/+0x3C, Spiegel +0x40/+0x44 (u16), Box ueber
+0x78 (zeigt in den Obj_model_set-Record pc+22, Record-Lage = pc+10/+12/+14, LAB_80040914 @0x800409b4-d4), Band +0x82,
Zaehler +0x8C, faellt +4, Wort +0. Die SCA-Zellen kommen aus der RAM (RDT+0x20 -> SCA-Kopf: ceil_x/ceil_z, 6 absolute
Quadrant-Zeiger). Geprueft wird:
  (a) VERSCHOBEN: Lage != Record-Lage (der Ruhe-Zweig oder ein Schub hat das Objekt bewegt),
  (b) TREFFER-JETZT: FUN_8003b558(obj,2) traefe an der aktuellen Lage (= das Original haette im naechsten Bild
      ausgeschoben; ein ruhendes Objekt darf das nie zeigen, sonst ist das Modell falsch) — mit dem Nicht-Quadrat-
      Zweig @0x8003b684-734 und ohne ihn.
Aufruf: python savestate_zellschub_zensus.py [stage_saves-Ordner]
"""
import glob, os, struct, sys

sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\worktrees\r36_dachkiste\.claude\skills\re15-room-capture\scripts")
from re15_ss import Ram

RDT_PTR, POOL, OBJ_SZ, PL, STAND = 0x800AC778, 0x800B3F98, 0x94, 0x800ACA54, 0x800AC788
RDT_DIR = r"C:\workspace\git\reAi_v2\.claude\worktrees\r36_dachkiste\re15_port\shared_assets\PSX"


def rdt_namen():
    """Kennung (erste 8 SCA-Zellen) -> Raumnamen aus den RDT-Dateien."""
    m = {}
    for p in glob.glob(os.path.join(RDT_DIR, "STAGE*", "ROOM*.RDT")):
        d = open(p, "rb").read()
        if len(d) < 0x40:
            continue
        off = struct.unpack_from("<I", d, 0x20)[0]
        if off == 0 or off + 28 + 96 > len(d):
            continue
        key = d[off + 0x18: off + 0x18 + 96]   # Zellen ab SCA+0x18 (RAM: Zeiger[0] = SCA+0x18)
        m.setdefault(key, []).append(os.path.basename(p)[:-4])
    return m


def lagecode(c, lo, hi):
    """@0x8003b698-6cc: Bit1 = c < hi, Bit0 = c < lo -> 3 westlich, 2 innerhalb, 0 oestlich."""
    return (((c - hi) & 0x80000000) | (((c - lo) & 0x80000000) >> 1)) >> 30


def treffer(zellen, ceil, pos, box, band, w0, mask=2, nq=True):
    cx, cy, cz, hx, hy, hz = box
    x, y, z = pos
    cxw, czw = cx + x, cz + z
    q = ((((czw - ceil[1]) & 0x80000000) | (((cxw - ceil[0]) & 0x80000000) >> 1)) >> 30)
    out = []
    for (w, d, x0, z0, t, u0, u1, fl) in zellen[q]:
        cb = (struct.unpack("<h", struct.pack("<H", ((fl << 8) | u1) & 0xffff))[0]) >> 12
        if cb != band:
            continue
        if ((u0 - 256 if u0 >= 128 else u0) & (mask & 0xff)) == 0:
            continue
        if w0 & 8:
            continue
        r = hx
        if nq and hx != hz:
            xc = lagecode(cxw, x0, x0 + w)
            zc = lagecode(czw, z0, z0 + d)
            if xc == 2 and zc != 2 and zc in (3, 0):
                r = hz
        if ((cxw - (x0 - r)) & 0xffffffff) < ((w + 2 * r) & 0xffffffff) and \
           ((czw - (z0 - r)) & 0xffffffff) < ((d + 2 * r) & 0xffffffff):
            out.append((q, x0, x0 + w, z0, z0 + d, u0, r))
    return out


def main():
    ordner = sys.argv[1] if len(sys.argv) > 1 else r"C:\workspace\git\reAi_v2\stage_saves"
    namen = rdt_namen()
    n_sav = n_obj = n_versch = n_treffer = 0
    for sav in sorted(glob.glob(os.path.join(ordner, "*.sav"))):
        try:
            ram = Ram(sav)
        except Exception as e:
            print("%-50s SKIP (%s)" % (os.path.basename(sav), e))
            continue
        rdt = ram.u32(RDT_PTR)
        if not (0x80000000 <= rdt < 0x80200000):
            print("%-50s SKIP (RDT-Zeiger %08x)" % (os.path.basename(sav), rdt))
            continue
        n = ram.u8(rdt + 2)
        sca = ram.u32(rdt + 0x20)
        if not (0x80000000 <= sca < 0x80200000):
            print("%-50s SKIP (SCA-Zeiger %08x)" % (os.path.basename(sav), sca))
            continue
        ceil = (ram.s16(sca), ram.s16(sca + 2))
        # SCA-Kopf: ceil_x, ceil_z, dann 5 absolute Zeiger (Quadrant 0..3 Start + Rest); FUN_8003b558 liest
        # [4+4q] und [8+4q] fuer q = 0..3 (@0x8003b5f4-624), Quadrant 4 kommt nie vor (2-Bit-Code).
        zeiger = [ram.u32(sca + 4 + 4 * i) for i in range(5)]
        if any(not (0x80000000 <= z < 0x80200000) for z in zeiger):
            print("%-50s SKIP (SCA-Zeiger %s)" % (os.path.basename(sav), ["%08x" % z for z in zeiger]))
            continue
        zellen = []
        for qi in range(4):
            cells = []
            a = zeiger[qi]
            while a + 12 <= zeiger[qi + 1] and len(cells) < 4096:
                cells.append(struct.unpack("<HHhhBBBB", ram.bytes(a, 12)))
                a += 12
            zellen.append(cells)
        key = ram.bytes(zeiger[0], 96)
        raum = "/".join(namen.get(key, ["?"]))
        pl = (ram.s32(PL + 0x34), ram.s32(PL + 0x38), ram.s32(PL + 0x3C), ram.u8(PL + 0x82))
        stand = ram.u32(STAND)
        n_sav += 1
        print("%-50s %-18s n=%d Spieler=(%d,%d,%d) b%d stand=%08x" % (os.path.basename(sav)[:50], raum, n, pl[0], pl[1], pl[2], pl[3], stand))
        for k in range(min(n, 16)):
            o = POOL + OBJ_SZ * k
            w0 = ram.u32(o)
            if not (w0 & 1):
                continue
            typ = ram.u8(o + 8)
            pos = (ram.s32(o + 0x34), ram.s32(o + 0x38), ram.s32(o + 0x3C))
            sp = (ram.u16(o + 0x40), ram.u16(o + 0x44))
            bp = ram.u32(o + 0x78)
            if not (0x80000000 <= bp < 0x80200000):
                print("   Pool %2d: Box-Zeiger %08x ungueltig" % (k, bp))
                continue
            box = struct.unpack("<6h", ram.bytes(bp, 12))
            rec = (ram.s16(bp - 22 + 10), ram.s16(bp - 22 + 12), ram.s16(bp - 22 + 14))
            band = ram.u8(o + 0x82)
            cnt = ram.u8(o + 0x8C)
            fall = ram.u8(o + 4)
            n_obj += 1
            versch = (pos[0] != rec[0] or pos[2] != rec[2])
            tr = treffer(zellen, ceil, pos, box, band, w0, 2, True)
            tr_alt = treffer(zellen, ceil, pos, box, band, w0, 2, False)
            flags = []
            if versch:
                n_versch += 1
                flags.append("VERSCHOBEN d=(%d,%d)" % (pos[0] - rec[0], pos[2] - rec[2]))
            if tr:
                n_treffer += 1
                flags.append("TREFFER-JETZT %s" % tr)
            if tr_alt and not tr:
                flags.append("nur-ohne-NQ-Zweig %s" % tr_alt)
            if o == stand:
                flags.append("STAND")
            if flags or typ == 4:
                print("   Pool %2d: w0 %08x typ %d band %d pos %s rec %s spiegel (%d,%d) cnt %d fall %d box %s %s" % (
                    k, w0, typ, band, pos, rec, struct.unpack("<h", struct.pack("<H", sp[0]))[0],
                    struct.unpack("<h", struct.pack("<H", sp[1]))[0], cnt, fall, list(box), " | ".join(flags)))
    print("SUMME: %d Spielstaende, %d aktive Objekte, %d verschoben, %d mit Treffer-jetzt" % (n_sav, n_obj, n_versch, n_treffer))


if __name__ == "__main__":
    main()
