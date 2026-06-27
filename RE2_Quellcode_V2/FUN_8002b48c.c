/* FUN_8002b48c @ 0x8002b48c  (Ghidra headless, raw MIPS overlay) */

void FUN_8002b48c(void)

{
  FUN_80031cc0();
  DAT_800cfb74 = DAT_800cfb74 | 0x40;
  ResetCallback();
  SetGraphDebug(0);
  InitGeom();
  FUN_800128c4();
  SetDefDrawEnv((DRAWENV *)&DAT_800dfae0,0,0,0x140,0xf0);
  SetDefDrawEnv((DRAWENV *)&DAT_800dfb78,0,0xf0,0x140,0xf0);
  SetDefDispEnv((DISPENV *)&DAT_800dfb3c,0,0xf0,0x140,0xf0);
  SetDefDispEnv((DISPENV *)&DAT_800dfbd4,0,0,0x140,0xf0);
  DAT_800dfb8f = 1;
  DAT_800dfaf7 = 1;
  DAT_800dfb8e = 0;
  DAT_800dfaf6 = 0;
  FUN_80095378(0);
  FUN_800953cc();
  _bu_init();
  FUN_80097c74(&DAT_800ce2b4,&DAT_800ce2d8);
  FUN_80095ca4();
  SetDrawMode((DR_MODE *)&DAT_800dfb50,0,0,0xc0,(RECT *)0x0);
  SetDrawMode((DR_MODE *)&DAT_800dfbe8,0,0,0xc0,(RECT *)0x0);
  SetDrawMode((DR_MODE *)&DAT_800dfb5c,0,0,0x80,(RECT *)0x0);
  SetDrawMode((DR_MODE *)&DAT_800dfbf4,0,0,0x80,(RECT *)0x0);
  FUN_8002c610();
  DAT_800ce5e0 = 0;
  DAT_800e8768 = 100;
  DAT_800eae3c = 100;
  DAT_800ce5e1 = 0;
  DAT_800dfd56 = 0;
  DAT_800dfd4c = 0;
  DAT_800dfd50 = 0xf00140;
  FUN_80031af4();
  FUN_800783b4();
  FUN_8002aec8();
  FUN_80031f6c(0,&LAB_8002b6d4);
  FUN_80031f6c(1,&LAB_8002b6ac);
  FUN_80031f6c(2,&LAB_8002b6ac);
  ResetGraph(0);
  return;
}


