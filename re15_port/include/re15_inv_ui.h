/*
 * re15_inv_ui.h — the VERBATIM RE1.5 status/inventory-screen UI data region
 * (Wave 0 of the S1-3 inventory campaign; RE campaign wf_b4a239f2, plan
 * shots/inv_plan.md, raw reports shots/inv_re_reports.md).
 *
 * re15_inv_ui_blob[] = PSX.EXE bytes [0x80074A8C, 0x80076614) embedded verbatim
 * (s_gait_blob pattern) — no table is re-typed by hand. Consumers index via the
 * RE15_INV_OFF() macro with the table's @0x address, so every use site carries
 * its citation. All addresses RE1.5-EXE, spot-checked against the live bytes
 * (cells/ECG-color/buttons/panel-master byte-exact, 2026-07-20).
 *
 * Table map (from the campaign; per-table structure notes in the generator):
 *   @0x80074A8C panel MASTER (stride 0xC {clut_idx u16, prim_count u16,
 *               template u32, prim_buf u32}; builder FUN_800467a8, animator
 *               FUN_80047648, draw chain FUN_80049a5c)
 *   @0x80074BDC screen task phase table (task LAB_8004603c)
 *   @0x80074C88 combine PAIR lists (4B {partner,result,action,pic}) ..0x80074DA7
 *   @0x80074DA8 per-item PROP table (stride 12; matcher FUN_8004e900)
 *   @0x80075108 prim geometry TEMPLATES (chrome group 0 = 26 SPRTs) ..0x800755F8
 *   @0x80075240 ARMS group geometry (15 rows)
 *   @0x80075390 button table (stride 0xC {dx,dy,w,h,u,v})
 *   @0x80075774 ECG waveform tables (targets of @0x80076214: 0x80075774/592C/
 *               5AE4/5CEC/5EA4/605C; last ends EXACTLY at the pointer table —
 *               self-closing region)
 *   @0x80076214 ECG waveform POINTER table (6 u32 per condition; values are
 *               PSX addresses — rebase with RE15_INV_OFF before indexing)
 *   @0x8007622C ECG trace colors (4 cond x 4B; Fine = 20 ff 20)
 *   @0x80076244 item-icon UV table
 *   @0x80076274 item grid CELLS (11 s16 pairs; 10 cells + entry 10 =
 *               (-66,88) = Standard-Arms box; base regs (215,26))
 */
#ifndef RE15_INV_UI_H
#define RE15_INV_UI_H

#define RE15_INV_UI_BLOB_BASE 0x80074A8Cu
#define RE15_INV_UI_BLOB_END  0x800762A0u   /* EXACT: grid cell table 0x80076274 + 11*4 = 0x2C */
#define RE15_INV_UI_BLOB_SIZE (RE15_INV_UI_BLOB_END - RE15_INV_UI_BLOB_BASE)

extern const unsigned char re15_inv_ui_blob[RE15_INV_UI_BLOB_SIZE];

/* Index the blob by the ORIGINAL PSX address — the use site then reads as e.g.
 * RE15_INV_PTR(0x80076274) and carries its own citation. */
#define RE15_INV_OFF(addr) ((unsigned)((addr) - RE15_INV_UI_BLOB_BASE))
#define RE15_INV_PTR(addr) (&re15_inv_ui_blob[RE15_INV_OFF(addr)])

/* ---- MAP wave: second verbatim region [0x800762A0, 0x80076C00) ----
 *   @0x800762A0 the 14 room-rect LISTS (stride-12 {u16 x,y,w,h; u8 u@+8; u8 v@+10},
 *               FUN_80046fd8 @0x8004731c-60, walk @0x800473bc/@0x800473c8);
 *               contiguous, list 13 @0x80076834 + 12 == the pair table (self-closing)
 *   @0x80076840 per-map-page room-rect PAIR table, 14 x {u16 count, u16 pad, u32 ptr}
 *               (FUN_80046fd8 @0x80047048-70: lhu 0x80076840+page*8 / lw 0x80076844+)
 *   @0x800768B0 marker scale rows stride 8 {u16 x_off, y_off, x_scale, z_scale},
 *               index DAT_800b260d = global room-slot 0..105 (FUN_800473f8
 *               @0x800474e8/0x80047518/0x80047444/0x8004747c; builder FUN_80046fd8
 *               @0x80047000-0c/0x8004707c-88; max slot = stage-6 base 98 @0x8004c044
 *               + bound 8 @0x8004bf7c) */
#define RE15_INV_MAP_BLOB_BASE 0x800762A0u
#define RE15_INV_MAP_BLOB_END  0x80076C00u
#define RE15_INV_MAP_BLOB_SIZE (RE15_INV_MAP_BLOB_END - RE15_INV_MAP_BLOB_BASE)

extern const unsigned char re15_inv_map_blob[RE15_INV_MAP_BLOB_SIZE];

#define RE15_INV_MAP_OFF(addr) ((unsigned)((addr) - RE15_INV_MAP_BLOB_BASE))
#define RE15_INV_MAP_PTR(addr) (&re15_inv_map_blob[RE15_INV_MAP_OFF(addr)])

#endif /* RE15_INV_UI_H */
