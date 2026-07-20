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

#endif /* RE15_INV_UI_H */
