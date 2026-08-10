#!/usr/bin/env python3
"""Generate engine/src/gen/re2_ems_toc.inc -- die byte-true Datenbasis des RE2-Modus-
Asset-Loaders (CDEMD0.EMS-Sektor-Tabelle + ENEMSE.VBS-Bank-Tabelle aus der RE2-EXE).

Quelle: info/re2leon/PSX.EXE (RE2 Retail Leon; Header 'PS-X EXE', t_addr 0x80010000,
t_size 0xF0800 -- selbst gelesen @Datei 0x18/0x1C). Datei-Offset = RAM - 0x80010000 + 0x800.
NB: eine fruehere Notiz nannte Datei 0x8B3F4 fuer die TOC -- FALSCH um 0x200; die Rohbytes
@0x8B5F4 (= 0x8009adf4 - 0x80010000 + 0x800) tragen exakt die verifizierten Werte (unten).
Wie gen_mc_title_bank.py wird die Tabelle build-zeitlich vendored und committet (die RE2-EXE
liegt bewusst NICHT in shared_assets).

TABELLE 1 -- CDEMD0.EMS Sektor-TOC @0x8009adf4 (Datei 0x8B5F4), 300 Eintraege x
{u32 sektor_offset_im_EMS, u32 byte_groesse} = 0x960 B (endet exakt am Byte-Array
DAT_8009b754). 4 Eintraege pro kind k, Index-Basis (k-0x10)*4, kinds 0x10..0x5A (75 Slots):
  +0/+1 = zwei vorgelinkte AI-Overlay-Kopien (Slot-1 @0x8010D000 / Slot-0 @0x80100000;
          Lader FUN_8001b710, Ziel 0x80100000 + slot*0xD000)
  +2    = TIM   (Binder FUN_8001aaa8: idx=(kind-0x10)*4|2 @0x8001ab4c-50)
  +3    = EMD   (idx |3 @0x8001ab7c-80)
Byte-Anker (in DIESEM Generator hart geprueft):
  kind 0x10 (Zombie-Cop): (0,53068) (26,53068) (52,66592) (85,146892)
    - 53068 == 0xCF4C == Groesse von COMMON/BIN/EMZ0.BIN (Overlay-Kopie; EMZ0.BIN ==
      CDEMD0.EMS[0xD000:0x19F4C], Lane-I-Report byte-verifiziert)
  kind 0x20 (Hund):  ModB=(1206,22266) TIM=(1217,66592) EMD=(1250,121144)
  kind 0x21 (Kraehe): Code 0x4A88 @Sektor 0x51E/0x528, TIM 0x8220 @0x532, EMD 0x7E28 @0x543

TABELLE 2 -- ENEMSE.VBS Bank-TOC @0x800a7b1c (Datei 0x9831C), 73 Baenke x 0x10 B.
Loader FUN_8005a09c (RE2_Quellcode_V2): pro Bank zwei 8-B-Subeintraege (bank*0x10 = EDT,
bank*0x10|8 = VBD), Subeintrag = {u32 byte_groesse (-> DAT_800d5308),
u16 rel_sektor_lo @+4 + u8 rel_sektor_hi @+6 (-> DAT_800d5314, 24-bit relativ zum
VBS-Dateistart file-id 0x161), u8 cd_flags @+7 (-> DAT_800d531e, CD-Read-Parameter --
fuer den Port irrelevant, wird beim Zugriff maskiert)}.
Byte-Anker: Bank 0 = EDT{0xCA8 B @Sektor 0} + VBD{0x11FE0 B @Sektor 2} (EDT belegt
Sektor 0..1, VBD schliesst kontig an); Bank 72 (letzte) endet exakt am VBS-Ende
(3269 Sektoren = 0x662800 B). EDT-Layout: SE-Map @+0 (u32-Eintraege), VH ("pBAV")
@u32[edt_size-8] (DAT_800d75ac = base + *(int*)(base-8+groesse); Bank 0/11: 0x80,
Rohbytes dort = 70 42 41 56 = "pBAV", selbst verifiziert).
"""
import os
import struct

ROOT = os.path.join(os.path.dirname(__file__), "..")
SRC = os.path.join(ROOT, "..", "info", "re2leon", "PSX.EXE")
EMZ0 = os.path.join(ROOT, "..", "info", "re2leon", "COMMON", "BIN", "EMZ0.BIN")
VBS = os.path.join(ROOT, "shared_assets", "RE2", "ENEMSE.VBS")
OUT = os.path.join(ROOT, "engine", "src", "gen", "re2_ems_toc.inc")

T_ADDR = 0x80010000          # PS-X EXE t_addr @Datei 0x18 (unten verifiziert)
def file_off(vaddr): return vaddr - T_ADDR + 0x800

CDEMD0_TOC_VA = 0x8009ADF4   # 300 x {u32 sektor, u32 groesse}; Ende = DAT_8009b754
CDEMD0_TOC_N = 300           # (0x8009b754 - 0x8009adf4) / 8 = 300 = 75 kinds x 4
ENEMSE_TOC_VA = 0x800A7B1C   # 73 Baenke x 0x10 (FUN_8005a09c: idx = bank*0x10 / |8)
ENEMSE_BANKS = 73            # == Zeilenzahl der Paar-Tabelle @0x800a7400 (FUN_80052b38)

def carray_u32(name, words, note):
    lines = [f"/* {note} */", f"static const uint32_t {name}[{len(words)}] = {{"]
    for i in range(0, len(words), 6):
        lines.append("    " + ", ".join(f"0x{w:08x}u" for w in words[i:i+6]) + ",")
    lines.append("};")
    return "\n".join(lines)

def main():
    with open(SRC, "rb") as f:
        d = f.read()
    assert d[:8] == b"PS-X EXE", "info/re2leon/PSX.EXE: kein PS-X-EXE-Header"
    t_addr, t_size = struct.unpack_from("<II", d, 0x18)
    assert t_addr == T_ADDR and t_size == 0xF0800, \
        f"unerwartete EXE-Geometrie t_addr=0x{t_addr:X} t_size=0x{t_size:X}"

    toc_off = file_off(CDEMD0_TOC_VA)                       # 0x8B5F4
    toc = list(struct.unpack_from(f"<{CDEMD0_TOC_N*2}I", d, toc_off))

    def kind_entry(kind, rec):
        i = ((kind - 0x10) * 4 + rec) * 2
        return toc[i], toc[i + 1]                           # (sektor, groesse)

    # --- Anker 1: kind 0x10 -- ModB (Slot-0-Overlay-Kopie) == EMZ0.BIN (53068 B) ---
    assert kind_entry(0x10, 0) == (0, 53068),      kind_entry(0x10, 0)
    assert kind_entry(0x10, 1) == (26, 53068),     kind_entry(0x10, 1)   # 26*0x800 = 0xD000
    assert kind_entry(0x10, 2) == (52, 66592),     kind_entry(0x10, 2)   # TIM 0x10420 @0x1A000
    assert kind_entry(0x10, 3) == (85, 146892),    kind_entry(0x10, 3)   # EMD 0x23DCC @0x2A800
    if os.path.exists(EMZ0):
        assert os.path.getsize(EMZ0) == 53068, "EMZ0.BIN-Groesse != kind-0x10-ModB"

    # --- Anker 2: Hund kind 0x20 (Lane-Report EM020) ---
    assert kind_entry(0x20, 1) == (1206, 22266),   kind_entry(0x20, 1)
    assert kind_entry(0x20, 2) == (1217, 66592),   kind_entry(0x20, 2)
    assert kind_entry(0x20, 3) == (1250, 121144),  kind_entry(0x20, 3)

    # --- Anker 3: Kraehe kind 0x21 (Lane-Report EM021: 0x51E/0x528/0x532/0x543) ---
    assert kind_entry(0x21, 0) == (0x51E, 0x4A88), kind_entry(0x21, 0)
    assert kind_entry(0x21, 1) == (0x528, 0x4A88), kind_entry(0x21, 1)
    assert kind_entry(0x21, 2) == (0x532, 0x8220), kind_entry(0x21, 2)
    assert kind_entry(0x21, 3) == (0x543, 0x7E28), kind_entry(0x21, 3)

    # --- ENEMSE-Bank-Tabelle ---
    ene_off = file_off(ENEMSE_TOC_VA)                       # 0x9831C
    ene = list(struct.unpack_from(f"<{ENEMSE_BANKS*4}I", d, ene_off))
    # Anker: Bank 0 = EDT{0xCA8 @s0} VBD{0x11FE0 @s2}; Sektor = 24-bit (Byte +7 = CD-Flags)
    assert ene[0] == 0xCA8 and (ene[1] & 0xFFFFFF) == 0,  (ene[0], ene[1])
    assert ene[2] == 0x11FE0 and (ene[3] & 0xFFFFFF) == 2, (ene[2], ene[3])
    if os.path.exists(VBS):
        vbs_sz = os.path.getsize(VBS)
        assert vbs_sz == 0x662800, f"ENEMSE.VBS-Groesse 0x{vbs_sz:X} != 0x662800"
        for b in range(ENEMSE_BANKS):
            esz, es, vsz, vs = ene[b*4], ene[b*4+1] & 0xFFFFFF, ene[b*4+2], ene[b*4+3] & 0xFFFFFF
            assert es * 0x800 + esz <= vbs_sz and vs * 0x800 + vsz <= vbs_sz, f"Bank {b} out of file"
        last_end = (ene[72*4+3] & 0xFFFFFF) + (ene[72*4+2] + 0x7FF) // 0x800
        assert last_end == vbs_sz // 0x800, f"letzte Bank endet @Sektor {last_end}, VBS hat {vbs_sz//0x800}"

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", newline="\n") as f:
        f.write("/* AUTO-GENERATED by tools/gen_re2_ems_toc.py -- DO NOT EDIT.\n"
                " * Quelle: info/re2leon/PSX.EXE (RE2 Retail Leon, t_addr 0x80010000).\n"
                " * Beide Tabellen sind ROHE u32-Worte exakt wie in der EXE (Byte-Anker im\n"
                " * Generator-Header; Zugriffs-Semantik in re2_ems.c). */\n\n")
        f.write(carray_u32("s_re2_cdemd0_toc", toc,
                "CDEMD0.EMS-Sektor-TOC @0x8009adf4 (Datei 0x8B5F4): 300 x {u32 sektor, u32 groesse},\n"
                " * 4 pro kind (k-0x10)*4: +0/+1 Overlay-Kopien (FUN_8001b710), +2 TIM, +3 EMD\n"
                " * (Binder FUN_8001aaa8 @0x8001ab4c-50 / @0x8001ab7c-80); kinds 0x10..0x5A") + "\n\n")
        f.write(carray_u32("s_re2_enemse_toc", ene,
                "ENEMSE.VBS-Bank-TOC @0x800a7b1c (Datei 0x9831C): 73 Baenke x 4 Worte\n"
                " * {u32 edt_groesse, u32 edt_sektor24|flags, u32 vbd_groesse, u32 vbd_sektor24|flags}\n"
                " * (Loader FUN_8005a09c: DAT_800d5308=+0, DAT_800d5314=+4(u16)+(+6)<<16, DAT_800d531e=+7)") + "\n")
    print(f"wrote {OUT}: cdemd0 {CDEMD0_TOC_N}x2 u32, enemse {ENEMSE_BANKS}x4 u32")

if __name__ == "__main__":
    main()
