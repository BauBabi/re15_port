/* FUN_80019628 @ 0x80019628  (Ghidra headless, raw MIPS overlay) */

short * FUN_80019628(undefined4 param_1,int param_2,int param_3,int param_4)

{
  ushort uVar1;
  int iVar2;
  short *psVar3;
  ushort *puVar4;
  int iVar5;
  short *psVar6;
  uint uVar7;
  short *psVar8;
  uint uVar9;
  int *piVar10;
  
  puVar4 = *(ushort **)(param_3 + 0x34);
  piVar10 = *(int **)(param_2 + 8);
  uVar9 = (uint)*(ushort *)(param_3 + 2);
  *(int *)(param_3 + 4) = param_4;
  iVar2 = uVar9 * 8;
  *(int *)(param_3 + 8) = param_4 + iVar2;
  iVar5 = param_4 + iVar2 + iVar2;
  *(int *)(param_3 + 0x1c) = iVar5;
  iVar5 = iVar5 + iVar2;
  *(int *)(param_3 + 0x20) = iVar5;
  psVar6 = (short *)(iVar5 + iVar2);
  *(short **)(param_3 + 0xc) = psVar6;
  psVar3 = psVar6 + uVar9 * 4;
  psVar8 = psVar6 + 2;
  iVar2 = *piVar10;
  uVar7 = uVar9;
  do {
    uVar7 = uVar7 - 1;
    *psVar6 = *(short *)((uint)*puVar4 * 8 + iVar2) + *(short *)(param_3 + 0x14);
    psVar6 = psVar6 + 4;
    psVar8[-1] = *(short *)((uint)*puVar4 * 8 + iVar2 + 2) + *(short *)(param_3 + 0x16);
    uVar1 = *puVar4;
    puVar4 = puVar4 + 2;
    *psVar8 = *(short *)((uint)uVar1 * 8 + iVar2 + 4) + *(short *)(param_3 + 0x18);
    psVar8 = psVar8 + 4;
  } while (uVar7 != 0);
  *(short **)(param_3 + 0x24) = psVar3;
  psVar6 = psVar3 + uVar9 * 4;
  psVar8 = psVar3 + 2;
  iVar2 = *piVar10;
  puVar4 = *(ushort **)(param_3 + 0x34);
  uVar7 = uVar9;
  do {
    uVar7 = uVar7 - 1;
    *psVar3 = *(short *)((uint)*puVar4 * 8 + iVar2) + *(short *)(param_3 + 0x2c);
    psVar3 = psVar3 + 4;
    psVar8[-1] = *(short *)((uint)*puVar4 * 8 + iVar2 + 2) + *(short *)(param_3 + 0x2e);
    uVar1 = *puVar4;
    puVar4 = puVar4 + 2;
    *psVar8 = *(short *)((uint)uVar1 * 8 + iVar2 + 4) + *(short *)(param_3 + 0x30);
    psVar8 = psVar8 + 4;
  } while (uVar7 != 0);
  *(short **)(param_3 + 0x10) = psVar6;
  psVar8 = psVar6 + uVar9 * 4;
  psVar3 = psVar6 + 2;
  iVar2 = piVar10[2];
  puVar4 = *(ushort **)(param_3 + 0x34);
  do {
    *psVar6 = *(short *)((uint)*puVar4 * 8 + iVar2);
    uVar9 = uVar9 - 1;
    psVar3[-1] = *(short *)((uint)*puVar4 * 8 + iVar2 + 2);
    uVar1 = *puVar4;
    psVar6 = psVar6 + 4;
    puVar4 = puVar4 + 2;
    *psVar3 = *(short *)((uint)uVar1 * 8 + iVar2 + 4);
    psVar3 = psVar3 + 4;
  } while (uVar9 != 0);
  return psVar8;
}


