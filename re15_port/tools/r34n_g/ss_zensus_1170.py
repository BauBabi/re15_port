#!/usr/bin/env python3
"""Spur G (Runde 34 Nacht, Stufe BAU) - vollstaendiger Savestate-Zensus ROOM1170 (Auflage 4).

Laeuft ueber ALLE DuckStation-Savestates eines Ordners (rekursiv), sortiert die gepatchten aus
und wertet jeden Stand in ROOM1170 (STAGE 1, Raum 0x17) aus:

  * Sauberkeit der EXE: Wort @0x80026e4c muss der Auslieferungs-Stub 0x03e00008 (jr ra) sein;
    die PATCHED-EXE-Staende tragen dort 0x0801c224 (j 0x80070890) - CLAUDE.md.
  * Stage per Overlay-Fingerabdruck CRC32(0x80100000..+0x8000) (re15_savestate_inspect.py:
    0x96290818 = STAGE 1), Raum DAT_800b0fe2 (lh @0x80021d48), Cut DAT_800b0fe4
    (lh @0x80021d44), Doppelpuffer DAT_800aca34 (lbu @0x8002156c), VBlank-Zaehler 0x800787dc
    (VSync(-1): lw @0x80062004).
  * RAM-Hintergrundkopie 0x80198000 (Quelle des Uploads: ori a1,a1,0x8000 @0x80043878;
    geschrieben nur per StoreImage @0x80021e44 beim Cut-Wechsel) gegen die BSS-Dekodierung des
    Ports (probe_r34n_g_bss): max |Delta| ueber das GANZE Bild und im Schild-Rechteck.
  * Schild-Rechteck des Cuts in BEIDEN Bildspeichern (VRAM y=0 / y=240; RECT @0x80072f2c,
    y = -(buf!=0) & 0xF0 @0x8002156c..80) gegen die RAM-Kopie: max |Delta|.
  * OT-Walk (ss_ot_walk.walk) aller drei OTs beider Puffer (DrawOTag @0x800215bc/d0/e4):
    Zahl der Primitive, deren Rechteck das Schild-Rechteck DES JEWEILIGEN CUTS schneidet.

Schild-Rechtecke (inklusive Rand, gemessen an der BSS-Dekodierung, Blau-Maske b>150, b>r+60,
b>g+40; G15 F): Cut 2 x285..312 y50..63, Cut 3 x292..319 y6..21, Cut 4 x317..319 y12..21 (Rand),
Cut 10 x262..287 y0..10. Andere Cuts zeigen das Schild nicht.

  C:/Python310/python.exe re15_port/tools/r34n_g/ss_zensus_1170.py <ordner> <bss-praefix>
      <bss-praefix>_cutNN.ppm = Ausgabe von probe_r34n_g_bss auf STAGE1/ROOM117.BSS
"""
import glob, os, struct, sys, zlib
import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, "..", "..", "..", ".claude", "skills",
                                "re15-savestate-ghidra", "scripts"))
sys.path.insert(0, HERE)
import re15_ss  # noqa: E402
import ss_ot_walk  # noqa: E402

# Schild-Rechtecke je Cut, INKLUSIVE Grenzen (x0, y0, x1, y1), 2 px Rand um die Blau-Maske.
SIGN = {2: (283, 48, 314, 65), 3: (290, 4, 319, 23), 4: (315, 10, 319, 23), 10: (260, 0, 289, 12)}
STAGE1_FP = 0x96290818
STUB_CLEAN = 0x03e00008   # jr ra   @0x80026e4c (Auslieferungsstand)
STUB_PATCH = 0x0801c224   # j 0x80070890 (gepatchte EXE, CLAUDE.md)


def rgb8(a16):
    """PSX-15-Bit (u16-Array) -> (h, w, 3) uint8, Kanal << 3."""
    r = (a16 & 31) << 3
    g = ((a16 >> 5) & 31) << 3
    b = ((a16 >> 10) & 31) << 3
    return np.stack([r, g, b], axis=-1).astype(np.int16)


def load_ppm(path):
    data = open(path, "rb").read()
    # P6\n320 240\n255\n
    parts = data.split(b"\n", 3)
    w, h = [int(v) for v in parts[1].split()]
    return np.frombuffer(parts[3][:w * h * 3], dtype=np.uint8).reshape(h, w, 3).astype(np.int16)


def vram_rect(r, x0, y0, w, h):
    vb = r.vram_base if r.vram_base is not None else r.base + 0x200000
    rows = []
    for y in range(y0, y0 + h):
        o = vb + (y * 1024 + x0) * 2
        rows.append(np.frombuffer(r.blob[o:o + w * 2], dtype="<u2"))
    return np.stack(rows)


def main():
    if len(sys.argv) < 3:
        print(__doc__)
        return 2
    root, bss_prefix = sys.argv[1], sys.argv[2]
    files = sorted(glob.glob(os.path.join(root, "**", "*.sav"), recursive=True))
    bss = {}
    n_all = n_patched_name = n_patched_stub = n_err = 0
    rows = []
    others = {}
    for f in files:
        n_all += 1
        name = os.path.relpath(f, root).replace("\\", "/")
        if os.path.basename(f).startswith("PATCHED-EXE"):
            n_patched_name += 1
            continue
        try:
            r = re15_ss.Ram(f)
        except Exception as e:  # noqa: BLE001
            n_err += 1
            print("FEHLER %s: %s" % (name, e))
            continue
        stub = r.u32(0x80026e4c)
        if stub != STUB_CLEAN:
            n_patched_stub += 1
            print("GEPATCHT (Stub %08x) %s" % (stub, name))
            continue
        fp = zlib.crc32(r.bytes(0x80100000, 0x8000)) & 0xffffffff
        room = r.u16(0x800b0fe2)
        cut = r.u16(0x800b0fe4)
        if fp != STAGE1_FP or room != 0x17:
            others[(fp, room)] = others.get((fp, room), 0) + 1
            continue
        vc = r.u32(0x800787dc)
        buf = r.u8(0x800aca34)
        ram = np.frombuffer(r.bytes(0x80198000, 320 * 240 * 2), dtype="<u2").reshape(240, 320)
        ram8 = rgb8(ram)
        if cut not in bss:
            p = "%s_cut%02d.ppm" % (bss_prefix, cut)
            bss[cut] = load_ppm(p) if os.path.exists(p) else None
        ref = bss[cut]
        d_full = int(np.abs(ram8 - ref).max()) if ref is not None else -1
        line = "%-34s Vcount=%6d cut=%2d buf=%d | RAM-Kopie vs BSS max|d| %3d" % (
            name, vc, cut, buf, d_full)
        box = SIGN.get(cut)
        hits_total = None
        if box:
            x0, y0, x1, y1 = box
            w, h = x1 - x0 + 1, y1 - y0 + 1
            ram_box = ram8[y0:y1 + 1, x0:x1 + 1]
            d_bss_box = int(np.abs(ram_box - ref[y0:y1 + 1, x0:x1 + 1]).max()) if ref is not None else -1
            fb = [rgb8(vram_rect(r, x0, yb + y0, w, h)) for yb in (0, 240)]
            d_fb = [int(np.abs(fbx - ram_box).max()) for fbx in fb]
            blue = int(((ram_box[..., 2] > 150) & (ram_box[..., 2] > ram_box[..., 0] + 60) &
                        (ram_box[..., 2] > ram_box[..., 1] + 40)).sum())
            hits_total = 0
            n_prims = 0
            for base, stride in ss_ot_walk.OTS:
                for b in (0, 1):
                    prims = ss_ot_walk.walk(r, (base + b * stride) & 0xffffff)
                    n_prims += len(prims)
                    hits_total += sum(1 for p in prims if p[2] and not (
                        p[2][2] < x0 or p[2][0] > x1 or p[2][3] < y0 or p[2][1] > y1))
            line += " | Schild blau %3d, RAM vs BSS %3d, fb0 vs RAM %3d, fb240 vs RAM %3d | OT %4d Pakete, %d ueber dem Schild" % (
                blue, d_bss_box, d_fb[0], d_fb[1], n_prims, hits_total)
        else:
            line += " | (Cut zeigt das Schild nicht)"
        rows.append((cut, line, hits_total))
        print(line)
    print()
    print("Dateien %d, davon PATCHED-EXE (Name) %d, gepatcht per Stub %d, unlesbar %d" % (
        n_all, n_patched_name, n_patched_stub, n_err))
    print("ROOM1170-Staende (STAGE 1, Raum 0x17, saubere EXE): %d" % len(rows))
    by_cut = {}
    for c, _, h in rows:
        by_cut.setdefault(c, []).append(h)
    for c in sorted(by_cut):
        hs = by_cut[c]
        if SIGN.get(c):
            print("  Cut %2d: %2d Staende, OT-Walks mit Schild-Rechteck: %d, Treffer gesamt: %d" % (
                c, len(hs), len(hs), sum(hs)))
        else:
            print("  Cut %2d: %2d Staende (Schild nicht im Bild)" % (c, len(hs)))
    print("Andere Raeume (Fingerabdruck, Raum): %s" % ", ".join(
        "%08x/%02x:%d" % (k[0], k[1], v) for k, v in sorted(others.items())))
    return 0


if __name__ == "__main__":
    sys.exit(main())
