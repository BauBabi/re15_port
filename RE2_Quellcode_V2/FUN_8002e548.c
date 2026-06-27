/* FUN_8002e548 @ 0x8002e548  (Ghidra headless, raw MIPS overlay) */

void FUN_8002e548(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  uint uVar8;
  undefined4 *puVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  
  iVar2 = DAT_800d8ce8;
  iVar10 = DAT_800cc1e0;
  iVar6 = *(int *)(param_1 + 0x198);
  piVar1 = *(int **)(iVar6 + 0x76c);
  iVar4 = *piVar1;
  *(int *)(DAT_800d8ce8 + 8) = DAT_800cc1e0;
  iVar4 = iVar4 - (int)piVar1;
  FUN_80076a00(iVar10,*(undefined4 *)(iVar6 + 0x76c),iVar4);
  iVar5 = *(int *)(iVar6 + 0x76c);
  FUN_80076a00(iVar10 + iVar4,*(int *)(iVar5 + 0x10),
               *(int *)(iVar5 + -0xc) - ((*(int *)(iVar5 + 0x10) + 0xc) - iVar5));
  piVar1 = *(int **)(iVar6 + 0x76c);
  uVar8 = *(int *)(iVar2 + 8) - (int)piVar1;
  uVar12 = uVar8 & 0xfffffffc;
  piVar7 = (int *)(DAT_800d8ce8 + 0x10);
  iVar10 = uVar8 - (piVar1[4] - *piVar1);
  uVar11 = (uint)piVar1[-1] >> 1;
  piVar1 = (int *)(*(int *)(param_1 + 0x198) + (*(byte *)(param_1 + 0x107) - uVar11) * 0xac + 0x94);
  uVar8 = uVar11;
  do {
    piVar7[-2] = piVar1[-0x23] + uVar12;
    *piVar7 = piVar1[-0x21] + uVar12;
    *(int *)(piVar7[-2] + 0x10) = *(int *)(piVar7[-2] + 0x10) + iVar10;
    *(int *)(*piVar7 + 0x10) = *(int *)(*piVar7 + 0x10) + iVar10;
    uVar8 = uVar8 - 1;
    piVar7[0x19] = piVar1[-8];
    iVar2 = *piVar1;
    piVar1 = piVar1 + 0x2b;
    piVar7[0x21] = iVar2;
    FUN_8002e1b4(piVar7[-2]);
    iVar2 = *piVar7;
    piVar7 = piVar7 + 0x2b;
    FUN_8002e240(iVar2);
  } while (uVar8 != 0);
  puVar9 = (undefined4 *)(DAT_800d8ce8 + 0x70);
  uVar3 = DAT_800df334;
  iVar10 = DAT_800d8ce8;
  do {
    uVar3 = FUN_8002cbc4(iVar10 + 4,uVar3,0);
    uVar3 = FUN_8002cd24(iVar10 + 4,uVar3,0);
    iVar10 = iVar10 + 0xac;
    FUN_8002e1fc(puVar9[-0x1a],puVar9[-0x19]);
    uVar11 = uVar11 - 1;
    FUN_8002e288(puVar9[-0x18],puVar9[-0x17]);
    *puVar9 = 0xf0a0a0;
    iVar2 = DAT_800d8ce8;
    puVar9 = puVar9 + 0x2b;
  } while (uVar11 != 0);
  iVar10 = *(int *)(param_1 + 0x1a4);
  *(undefined4 *)(iVar10 + 0x76c) = *(undefined4 *)(DAT_800d8ce8 + 8);
  *(undefined4 *)(iVar10 + 0x774) = *(undefined4 *)(iVar2 + 0x10);
  *(undefined4 *)(iVar10 + 0x770) = *(undefined4 *)(iVar2 + 0xc);
  *(undefined4 *)(iVar10 + 0x778) = *(undefined4 *)(iVar2 + 0x14);
  return;
}


