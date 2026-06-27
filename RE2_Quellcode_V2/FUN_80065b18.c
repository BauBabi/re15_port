/* FUN_80065b18 @ 0x80065b18  (Ghidra headless, raw MIPS overlay) */

int FUN_80065b18(undefined4 param_1,int param_2)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int iVar6;
  
  bVar2 = false;
  iVar6 = 0;
  iVar5 = 2;
  piVar4 = &DAT_800d7648;
  do {
    iVar3 = *piVar4;
    if (iVar3 != 0) {
      if (DAT_800cfdac == iVar3) {
        bVar2 = true;
      }
      else if (param_2 != iVar3) {
        iVar6 = iVar3;
      }
    }
    piVar4 = piVar4 + 1;
    bVar1 = iVar5 != 0;
    iVar5 = iVar5 + -1;
  } while (bVar1);
  if ((iVar6 == 0) && (iVar6 = 0, bVar2)) {
    iVar6 = DAT_800cfdac;
  }
  return iVar6;
}


