void FUN_80020f8c(void)

{
  int iVar1;
  int iVar2;
  
  ResetCallback();
  SetGraphDebug(0);
  InitGeom();
  FUN_80066d60(0xa0,0x78);
  FUN_8002134c();
  InitCARD(1);
  StartCARD();
  _bu_init();
  ChangeClearPAD(0);
  FUN_80026f90();
  SetDefDrawEnv((DRAWENV *)&DAT_800b5370,0,0,0x140,0xf0);
  SetDefDrawEnv((DRAWENV *)&DAT_800b53cc,0,0xf0,0x140,0xf0);
  SetDefDispEnv((DISPENV *)&DAT_800b5428,0,0xf0,0x140,0xf0);
  SetDefDispEnv((DISPENV *)&DAT_800b543c,0,0,0x140,0xf0);
  DAT_800b53e3 = 1;
  DAT_800b5387 = 1;
  iVar1 = GetGraphType();
  iVar2 = 0x300;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0xc0;
    if (iVar1 == 2) {
      iVar2 = 0x300;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_8008f644,0,0,iVar2,(RECT *)0x0);
  iVar1 = GetGraphType();
  iVar2 = 0x300;
  if (iVar1 != 1) {
    iVar1 = GetGraphType();
    iVar2 = 0xc0;
    if (iVar1 == 2) {
      iVar2 = 0x300;
    }
  }
  SetDrawMode((DR_MODE *)&DAT_8008f650,0,0,iVar2,(RECT *)0x0);
  FUN_80029590();
  ResetGraph(0);
  FUN_80021eb4();
  DAT_800aca34 = 0;
  FUN_80029a98(0,&LAB_80021138);
  return;
}
