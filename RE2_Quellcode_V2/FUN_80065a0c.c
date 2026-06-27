/* FUN_80065a0c @ 0x80065a0c  (Ghidra headless, raw MIPS overlay) */

int FUN_80065a0c(undefined4 param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  ushort *puVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  
  iVar7 = 0;
  iVar8 = 0;
  iVar6 = 2;
  puVar4 = &DAT_800dfad8;
  piVar5 = &DAT_800d7648;
  do {
    iVar3 = *piVar5;
    if (iVar3 == 0) break;
    iVar2 = FUN_80015614(param_1,*(undefined4 *)(iVar3 + 0x38),*(undefined4 *)(iVar3 + 0x40),0x400);
    if (iVar2 == 0) {
      if (*puVar4 < 0x7fffffff) {
        iVar7 = iVar3;
      }
    }
    else if (*puVar4 < 0x7fffffff) {
      iVar8 = iVar3;
    }
    puVar4 = puVar4 + 1;
    piVar5 = piVar5 + 1;
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -1;
  } while (bVar1);
  if (iVar7 == 0) {
    iVar7 = iVar8;
  }
  return iVar7;
}


