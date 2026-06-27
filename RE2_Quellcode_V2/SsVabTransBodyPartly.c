/* SsVabTransBodyPartly @ 0x80085be4  (Ghidra headless, raw MIPS overlay) */

short SsVabTransBodyPartly(uchar *param_1,ulong param_2,short param_3)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  
  if (((ushort)param_3 < 0x11) && (iVar3 = (int)param_3, (&DAT_800dcc68)[iVar3] == '\x02')) {
    if (DAT_800abbc4 == 0) {
      DAT_800abbc4 = *(uint *)(&DAT_800eada0 + iVar3 * 4);
      DAT_800abbc8 = param_3;
      SpuSetTransferMode(0);
      SpuSetTransferStartAddr(*(ulong *)(&DAT_800eadf0 + iVar3 * 4));
    }
    sVar1 = DAT_800abbc8;
    iVar4 = (int)DAT_800abbc8;
    if (iVar4 == iVar3) {
      if (DAT_800abbc4 < param_2) {
        param_2 = DAT_800abbc4;
      }
      FUN_80082710(1);
      SpuWritePartly(param_1,param_2);
      DAT_800abbc4 = DAT_800abbc4 - param_2;
      sVar2 = -2;
      if (DAT_800abbc4 == 0) {
        DAT_800abbc8 = -1;
        DAT_800abbc4 = 0;
        (&DAT_800dcc68)[iVar4] = 1;
        sVar2 = sVar1;
      }
      return sVar2;
    }
  }
  FUN_80082710(0);
  sVar1 = VS_VTBP_OBJ_13C();
  return sVar1;
}


