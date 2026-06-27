/* FUN_80083e2c @ 0x80083e2c  (Ghidra headless, raw MIPS overlay) */

int FUN_80083e2c(short param_1,short param_2,short param_3,undefined2 param_4)

{
  short sVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  
  _SsVmVSetUp((int)param_2,(int)param_3);
  iVar3 = 0;
  iVar4 = 0;
  DAT_800dcc46 = param_1;
  if (DAT_800d7854 != 0) {
    iVar2 = 0;
    do {
      sVar1 = FUN_80083c2c(iVar2 >> 0x10,(int)param_1,(int)param_2,(int)param_3,param_4);
      iVar4 = iVar4 + sVar1;
      iVar3 = iVar3 + 1;
      iVar2 = iVar3 * 0x10000;
    } while (iVar3 * 0x10000 >> 0x10 < (int)(uint)DAT_800d7854);
  }
  return iVar4;
}


