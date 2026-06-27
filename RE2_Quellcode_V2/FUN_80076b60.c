/* FUN_80076b60 @ 0x80076b60  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80076b60(int param_1,undefined4 *param_2,int param_3,int param_4)

{
  uint uVar1;
  int iVar2;
  int *piVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  uint uVar10;
  undefined4 uVar11;
  
  uVar11 = *param_2;
  FUN_8002cfd8(param_2 + 1);
  iVar7 = 0;
  uVar10 = param_2[2];
  uVar5 = param_2[9];
  uVar8 = 0;
  if (uVar10 != 0) {
    piVar6 = param_2 + 8;
    do {
      iVar4 = *piVar6;
      iVar2 = (uVar8 & 1) + 3;
      piVar9 = (int *)piVar6[1];
      iVar7 = iVar7 + iVar2 * iVar4 * 4;
      if (iVar4 != 0) {
        piVar3 = piVar9 + 1;
        do {
          iVar4 = iVar4 + -1;
          *piVar3 = *piVar3 + param_3 * 0x10000;
          piVar3 = piVar3 + iVar2;
          *piVar9 = *piVar9 + param_4 * 0x400000;
          piVar9 = piVar9 + iVar2;
        } while (iVar4 != 0);
      }
      uVar8 = uVar8 + 1;
      piVar6 = piVar6 + 7;
    } while (uVar8 < uVar10);
  }
  if (param_1 == 0) {
    iVar7 = (int)&DAT_8018fff0 - iVar7;
    FUN_80010778(iVar7,uVar5);
    uVar8 = 0;
    if (uVar10 != 0) {
      piVar6 = param_2 + 8;
      do {
        uVar1 = uVar8 & 1;
        piVar6[1] = iVar7;
        uVar8 = uVar8 + 1;
        iVar7 = iVar7 + (uVar1 + 3) * *piVar6 * 4;
        piVar6 = piVar6 + 7;
      } while (uVar8 < uVar10);
    }
  }
  return uVar11;
}


