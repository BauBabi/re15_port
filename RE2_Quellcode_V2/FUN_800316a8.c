/* FUN_800316a8 @ 0x800316a8  (Ghidra headless, raw MIPS overlay) */

void FUN_800316a8(int param_1)

{
  short *psVar1;
  short *psVar2;
  ushort *puVar3;
  int iVar4;
  
  psVar2 = *(short **)(param_1 + 0x5c88);
  psVar1 = (short *)(param_1 + 0x5988);
  if (psVar2 != psVar1) {
    do {
      puVar3 = (ushort *)(psVar1 + 2);
      iVar4 = (int)*psVar1 + psVar1[1] * 0x10000;
      if ((*puVar3 & 0x200) == 0) {
        psVar1 = (short *)FUN_80031734(puVar3,iVar4,param_1);
      }
      else {
        psVar1 = (short *)FUN_800318a4(puVar3,iVar4,param_1);
      }
    } while (psVar1 < psVar2);
  }
  return;
}


