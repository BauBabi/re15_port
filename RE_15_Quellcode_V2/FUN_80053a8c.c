void FUN_80053a8c(short *param_1,u_long *param_2,u_long *param_3,u_long *param_4,int param_5)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  RECT local_28 [2];
  
  DecDCTReset(0);
  DecDCToutCallback((func *)0x0);
  DecDCTvlc(param_2,param_3);
  iVar2 = 0x10;
  DecDCTin(param_3,param_5);
  uVar1 = 0;
  if (param_5 != 0) {
    iVar2 = 0x18;
  }
  local_28[0].x = *param_1;
  local_28[0].y = param_1[1];
  local_28[0].h = param_1[3];
  uVar3 = (uint)(param_1[2] * iVar2) >> 4;
  local_28[0].w = (short)iVar2;
  if (uVar3 != 0) {
    do {
      DecDCTout(param_4,(uint)(iVar2 * param_1[3] * 2) >> 2);
      DecDCToutSync(0);
      LoadImage(local_28,param_4);
      DrawSync(0);
      uVar1 = uVar1 + iVar2;
      local_28[0].x = local_28[0].x + (short)iVar2;
    } while (uVar1 < uVar3);
  }
  return;
}
