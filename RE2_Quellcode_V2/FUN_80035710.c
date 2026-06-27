/* FUN_80035710 @ 0x80035710  (Ghidra headless, raw MIPS overlay) */

void FUN_80035710(int param_1,int param_2)

{
  int iVar1;
  
  iVar1 = param_1 * 0x1f8;
  if (DAT_800ce54c != param_2) {
    DAT_800ce54c = param_2;
    FUN_80076b60(1,param_2,(&DAT_800d0428)[iVar1],(&DAT_800d0429)[iVar1]);
  }
  (&DAT_800d0338)[param_1 * 0x7e] = param_2 + 0xc;
  *(undefined4 *)(&DAT_800d0328 + iVar1) = 0;
  (&DAT_800d0340)[param_1 * 0x7e] = param_2 + 0x28;
  (&DAT_800d03a0)[param_1 * 0x7e] = 0x808080;
  DAT_800ce32c = FUN_8002cbc4(&DAT_800d0334 + param_1 * 0x7e,DAT_800ce32c,0);
  DAT_800ce32c = FUN_8002cd24(&DAT_800d0334 + param_1 * 0x7e,DAT_800ce32c,0);
  return;
}


