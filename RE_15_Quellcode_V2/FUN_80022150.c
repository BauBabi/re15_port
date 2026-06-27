undefined4 FUN_80022150(byte param_1,undefined4 *param_2,int param_3,int param_4)

{
  int iVar1;
  int *piVar2;
  undefined4 uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  int iVar8;
  int *piVar9;
  uint uVar10;
  undefined4 uVar11;
  
  uVar11 = *param_2;
  if (param_2[1] == 0) {
    FUN_8002288c(param_2 + 1);
  }
  iVar8 = 0;
  uVar10 = param_2[2];
  uVar3 = param_2[9];
  uVar5 = 0;
  if (uVar10 != 0) {
    puVar7 = param_2 + 8;
    do {
      uVar6 = *puVar7;
      iVar1 = (uVar5 & 1) + 3;
      uVar4 = 0;
      piVar9 = (int *)puVar7[1];
      iVar8 = iVar8 + iVar1 * uVar6 * 4;
      if (uVar6 != 0) {
        piVar2 = piVar9 + 1;
        do {
          if (param_1 == 1) {
LAB_80022234:
            *piVar9 = param_4 * 0x400000 + *piVar9;
          }
          else if (param_1 < 2) {
            if (param_1 == 0) {
              *piVar2 = param_3 * 0x10000 + *piVar2;
            }
          }
          else if (param_1 == 2) {
            *piVar2 = param_3 * 0x10000 + *piVar2;
            goto LAB_80022234;
          }
          piVar2 = piVar2 + iVar1;
          uVar4 = uVar4 + 1;
          piVar9 = piVar9 + iVar1;
        } while (uVar4 < uVar6);
      }
      uVar5 = uVar5 + 1;
      puVar7 = puVar7 + 7;
    } while (uVar5 < uVar10);
  }
  iVar1 = (int)&DAT_8018fff0 - iVar8;
  FUN_800104b0(iVar1,uVar3,iVar8);
  uVar5 = 0;
  if (uVar10 != 0) {
    piVar9 = param_2 + 8;
    do {
      uVar4 = uVar5 & 1;
      piVar9[1] = iVar1;
      uVar5 = uVar5 + 1;
      iVar1 = iVar1 + (uVar4 + 3) * *piVar9 * 4;
      piVar9 = piVar9 + 7;
    } while (uVar5 < uVar10);
  }
  return uVar11;
}
