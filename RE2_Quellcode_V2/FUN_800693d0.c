/* FUN_800693d0 @ 0x800693d0  (Ghidra headless, raw MIPS overlay) */

void FUN_800693d0(undefined4 param_1,undefined4 param_2,uint param_3,int param_4)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (byte)(&DAT_800a9e1d)[param_4 * 8] & 0xf;
  if ((((&DAT_800a9e1d)[param_4 * 8] & 0xf) != 0) &&
     (iVar1 = FUN_80077360(&DAT_800d48ec,uVar2), iVar1 != 0)) {
    param_4 = uVar2 + 0x81;
  }
  FUN_80031070(param_1,param_2,param_3 | 0x4000,param_4);
  return;
}


