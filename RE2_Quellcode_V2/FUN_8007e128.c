/* FUN_8007e128 @ 0x8007e128  (Ghidra headless, raw MIPS overlay) */

void FUN_8007e128(int param_1,short param_2)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = *(int *)((int)&DAT_800ea250 + ((param_1 << 0x10) >> 0xe)) + param_2 * 0xb0;
  sVar1 = *(short *)(iVar5 + 0x54);
  iVar3 = *(int *)(iVar5 + 0x90);
  iVar4 = iVar3 - sVar1;
  if (iVar4 < 1) {
    if (sVar1 < iVar3) {
      return;
    }
    do {
      do {
        FUN_8007e474((param_1 << 0x10) >> 0x10,(int)param_2);
      } while (*(int *)(iVar5 + 0x90) == 0);
      iVar3 = iVar3 + *(int *)(iVar5 + 0x90);
      iVar4 = iVar3 - *(short *)(iVar5 + 0x54);
    } while (iVar3 < *(short *)(iVar5 + 0x54));
  }
  else {
    sVar2 = *(short *)(iVar5 + 0x52);
    if (0 < sVar2) {
      *(short *)(iVar5 + 0x52) = sVar2 + -1;
      return;
    }
    if (sVar2 != 0) {
      *(int *)(iVar5 + 0x90) = iVar4;
      return;
    }
    *(short *)(iVar5 + 0x52) = sVar1;
    iVar4 = *(int *)(iVar5 + 0x90) + -1;
  }
  *(int *)(iVar5 + 0x90) = iVar4;
  return;
}


