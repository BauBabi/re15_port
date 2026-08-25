#!/usr/bin/env python3
"""re2_files_cut.py - schneidet COMMON/DATA/FILES.TIM (RE2 Retail Leon) in die 216
einzelnen TIMs der 25 Dokumente, exakt nach den Tabellen der RE2-PSX.EXE.

Belege (RE2 PSX.EXE, t_addr 0x80010000, t_size 0xf0800, file-off = addr - 0x8000f800):
  0x800a94b4  Slot-Record-Tabelle, 191 x 8 B  {u32 groesse, u24 sektor-offset, u8 pad}
              gelesen @0x8006d4d8 (groesse), @0x8006d518/0x8006d50c (sektor lo/hi),
              @0x8006d4f0 (+7).  byteoff = sektor * 0x800.
  0x800a9ad0  Dokument -> erster Slot, 25 x u8, gelesen @0x8006d480
  0x800aa144  Dokument-Record, 25 x 4 B {u16 max_page, u8 (256-page_h), u8 pad}
              max_page  gelesen @0x80076224 / @0x80072520 / @0x8006d0b0
              256-h     gelesen @0x8007603c (s5 = 256 - s3 = Seitenhoehe @0x80076044)
  Slot-Adresse = doc_start                (Titelseite, Flag 0x800d5c05 == 0, @0x8006d484)
               = doc_start + 1 + page     (Folgeseite, @0x8006d488-98)
  CD-Datei-Id von FILES.TIM = 166 (@0x8006d4b4 sh -> 0x800d531c, jal 0x80012fb8 @0x8006d530)
  Basis-LBA    = u16 @0x80098dd8 | u8 @0x80098dda << 16 = 42049 (@0x8006d4bc/0x8006d4a8)
"""
import os, struct, sys, csv

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
EXE  = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
SRC  = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "FILES.TIM")
OUT  = os.path.join(REPO, "build", "extracted", "re2_files")

T = 0x8000f800
REC_TAB, DOC_TAB, CNT_TAB = 0x800a94b4, 0x800a9ad0, 0x800aa144
N_SLOT, N_DOC = 191, 25


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else OUT
    exe = open(EXE, "rb").read()
    tim = open(SRC, "rb").read()
    fo = lambda a: a - T

    docs = [exe[fo(DOC_TAB) + i] for i in range(N_DOC)]
    recs = []
    for i in range(N_SLOT):
        o = fo(REC_TAB + i * 8)
        size = struct.unpack_from("<I", exe, o)[0]
        sect = struct.unpack_from("<H", exe, o + 4)[0] | (exe[o + 6] << 16)
        recs.append((size, sect))
    meta = []
    for i in range(N_DOC):
        o = fo(CNT_TAB + i * 4)
        meta.append((struct.unpack_from("<H", exe, o)[0], 256 - exe[o + 2]))

    os.makedirs(out, exist_ok=True)
    rows = []

    def parse(off):
        magic, flags = struct.unpack_from("<II", tim, off)
        assert magic == 0x10, "kein TIM @0x%x" % off
        pmode, cf = flags & 7, (flags >> 3) & 1
        o, clut = off + 8, None
        if cf:
            clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", tim, o)
            clut = (clen, cx, cy, cw, ch); o += clen
        ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", tim, o); o += ilen
        bpp = {0: 4, 1: 8, 2: 16, 3: 24}[pmode]
        return dict(off=off, end=o, bpp=bpp, w=iw * 16 // bpp, h=ih, hw=iw, clut=clut)

    for di in range(N_DOC):
        first = docs[di]
        last  = docs[di + 1] if di + 1 < N_DOC else N_SLOT
        for si in range(first, last):
            size, sect = recs[si]
            base = sect * 0x800
            assert base + size <= len(tim)
            page = si - first - 1          # -1 = Titelseite des Dokuments
            t = parse(base)
            while True:
                role = "page" if t["bpp"] == 4 else "paper"
                tag  = "title" if page < 0 else "p%02d" % page
                name = "FILE%02d_%s_%s.TIM" % (di, tag, role)
                open(os.path.join(out, name), "wb").write(tim[t["off"]:t["end"]])
                rows.append([di, si, page, role, "0x%06x" % t["off"], t["end"] - t["off"],
                             t["bpp"], t["w"], t["h"], meta[di][0], meta[di][1], name])
                if t["end"] >= base + size:
                    break
                t = parse(t["end"])

    with open(os.path.join(out, "toc.csv"), "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["doc", "slot", "page", "role", "file_off", "tim_bytes", "bpp",
                    "width", "height", "doc_max_page", "doc_page_h", "name"])
        w.writerows(rows)
    print("%d TIMs, %d Dokumente -> %s" % (len(rows), N_DOC, out))


if __name__ == "__main__":
    main()
