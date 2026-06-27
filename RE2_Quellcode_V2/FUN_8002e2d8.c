/* FUN_8002e2d8 @ 0x8002e2d8  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8002e2d8(int param_1,int param_2)

{
  int *piVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined4 uVar5;
  int *piVar6;
  undefined4 *puVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  int iVar11;
  uint uVar12;
  uint uVar13;
  
  iVar11 = *(int *)(param_1 + 0x198);
  uVar12 = (uint)*(byte *)(param_1 + 0x107);
  *(int *)(param_1 + 0x1a4) = param_2;
  iVar8 = param_2 + uVar12 * 0xac;
  iVar9 = iVar8;
  if (*(byte *)(param_1 + 8) < 0x10) {
    iVar9 = iVar8 + 0xd70;
    DAT_800d8ce8 = iVar8;
  }
  FUN_80076a00(param_2,iVar11);
  piVar1 = *(int **)(iVar11 + 8);
  piVar6 = (int *)(param_2 + 0x10);
  iVar8 = *piVar1;
  piVar10 = (int *)(iVar11 + 0x10);
  *(int *)(param_2 + 8) = iVar9;
  iVar8 = iVar8 - (int)piVar1;
  FUN_80076a00(iVar9,*(undefined4 *)(iVar11 + 8),iVar8);
  iVar9 = iVar9 + iVar8;
  iVar8 = *(int *)(*(int *)(iVar11 + 8) + 0x10);
  iVar4 = *(int *)(*(int *)(param_1 + 0x14) + -0xc) - ((iVar8 + 0xc) - *(int *)(param_1 + 0x14));
  FUN_80076a00(iVar9,iVar8,iVar4);
  piVar1 = *(int **)(iVar11 + 8);
  iVar8 = *(int *)(param_2 + 8) - (int)piVar1;
  iVar11 = (iVar8 >> 2) * 4;
  iVar8 = iVar8 - (piVar1[4] - *piVar1);
  uVar13 = uVar12;
  do {
    piVar6[-2] = piVar10[-2] + iVar11;
    *piVar6 = *piVar10 + iVar11;
    *(int *)(piVar6[-2] + 0x10) = *(int *)(piVar6[-2] + 0x10) + iVar8;
    uVar13 = uVar13 - 1;
    *(int *)(*piVar6 + 0x10) = *(int *)(*piVar6 + 0x10) + iVar8;
    piVar10 = piVar10 + 0x2b;
    FUN_8002e1b4(piVar6[-2]);
    iVar3 = *piVar6;
    piVar6 = piVar6 + 0x2b;
    FUN_8002e240(iVar3);
  } while (uVar13 != 0);
  iVar8 = *(int *)(param_1 + 0x198);
  uVar5 = *(undefined4 *)(param_1 + 0x198);
  uVar2 = *(undefined4 *)(*(int *)(param_1 + 0x1a4) + 8);
  puVar7 = (undefined4 *)(*(int *)(param_1 + 0x1a4) + 0x70);
  *(undefined4 *)(param_1 + 0x198) = *(undefined4 *)(param_1 + 0x1a4);
  *(undefined4 *)(param_1 + 0x14) = uVar2;
  uVar2 = FUN_80028368(param_1,iVar9 + iVar4,*(undefined4 *)(param_1 + 0x108),0);
  *(undefined4 *)(param_1 + 0x198) = uVar5;
  *(undefined4 *)(param_1 + 0x14) = *(undefined4 *)(iVar8 + 8);
  do {
    uVar12 = uVar12 - 1;
    FUN_8002e1fc(puVar7[-0x1a],puVar7[-0x19]);
    FUN_8002e288(puVar7[-0x18],puVar7[-0x17]);
    *puVar7 = 0x807878;
    puVar7 = puVar7 + 0x2b;
  } while (uVar12 != 0);
  return uVar2;
}


