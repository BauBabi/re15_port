/* FUN_8001d894 @ 0x8001d894  (Ghidra headless, raw MIPS overlay) */

void FUN_8001d894(void)

{
  undefined2 *puVar1;
  ushort uVar2;
  short sVar3;
  undefined4 *puVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 *puVar8;
  undefined4 uVar9;
  VECTOR *r1;
  
  iVar5 = DAT_800dcbd0;
  if ((*(ushort *)(DAT_800dcbd0 + 0x18) & 0x800) != 0) {
    puVar4 = *(undefined4 **)(DAT_800dcbd0 + 0x6c);
    uVar6 = puVar4[1];
    uVar7 = puVar4[2];
    uVar9 = puVar4[3];
    *(undefined4 *)(DAT_800dcbd0 + 0x4c) = *puVar4;
    *(undefined4 *)(iVar5 + 0x50) = uVar6;
    *(undefined4 *)(iVar5 + 0x54) = uVar7;
    *(undefined4 *)(iVar5 + 0x58) = uVar9;
    uVar6 = puVar4[5];
    uVar7 = puVar4[6];
    uVar9 = puVar4[7];
    *(undefined4 *)(iVar5 + 0x5c) = puVar4[4];
    *(undefined4 *)(iVar5 + 0x60) = uVar6;
    *(undefined4 *)(iVar5 + 100) = uVar7;
    *(undefined4 *)(iVar5 + 0x68) = uVar9;
  }
  iVar5 = DAT_800dcbd0;
  DAT_1f800000 = DAT_8009db44;
  DAT_1f800004 = DAT_8009db48;
  DAT_1f800008 = DAT_8009db4c;
  DAT_1f80000c = DAT_8009db50;
  DAT_1f800010 = DAT_8009db54;
  DAT_1f800014 = DAT_8009db58;
  DAT_1f800018 = DAT_8009db5c;
  DAT_1f80001c = DAT_8009db60;
  uVar2 = *(ushort *)(DAT_800dcbd0 + 0x18);
  puVar1 = (undefined2 *)(DAT_800dcbd0 + 0x38);
  *(undefined4 *)(DAT_800dcbd0 + 0x3c) = *(undefined4 *)(DAT_800dcbd0 + 0x34);
  *(undefined2 *)(iVar5 + 0x40) = *puVar1;
  if ((uVar2 & 0x400) == 0) {
    RotMatrixY((int)*(short *)(iVar5 + 0x22),(MATRIX *)&DAT_1f800000);
    gte_SetRotMatrix((MATRIX *)&DAT_1f800000);
    gte_ldv0((SVECTOR *)(DAT_800dcbd0 + 0x24));
    gte_rtv0();
    r1 = (VECTOR *)&DAT_1f800020;
    gte_stlvnl((VECTOR *)&DAT_1f800020);
    iVar5 = DAT_800dcbd0;
    puVar8 = &DAT_1f800020;
    *(short *)(DAT_800dcbd0 + 0x34) = (short)DAT_1f800020;
    *(short *)(iVar5 + 0x36) = (short)DAT_1f800024;
    *(short *)(iVar5 + 0x38) = (short)DAT_1f800028;
    gte_SetRotMatrix((MATRIX *)(iVar5 + 0x4c));
    gte_ApplyRotMatrix((SVECTOR *)(iVar5 + 0x2c),r1);
    iVar5 = DAT_800dcbd0;
    puVar4 = (undefined4 *)(DAT_800dcbd0 + 100);
    *(short *)(DAT_800dcbd0 + 0x34) =
         *(short *)(DAT_800dcbd0 + 0x34) +
         (short)*(undefined4 *)(DAT_800dcbd0 + 0x60) + (short)*puVar8;
    *(short *)(iVar5 + 0x36) = *(short *)(iVar5 + 0x36) + (short)*puVar4 + (short)DAT_1f800024;
    sVar3 = *(short *)(iVar5 + 0x38) + (short)*(undefined4 *)(iVar5 + 0x68) + (short)DAT_1f800028;
  }
  else {
    gte_SetRotMatrix((MATRIX *)&DAT_1f800000);
    gte_ldv0((SVECTOR *)(iVar5 + 0x24));
    gte_rtv0();
    gte_stlvnl((VECTOR *)&DAT_1f800020);
    puVar4 = &DAT_1f800020;
    DAT_1f80002c = *(short *)(DAT_800dcbd0 + 0x2c) + (short)DAT_1f800020;
    DAT_1f80002e = *(short *)(DAT_800dcbd0 + 0x2e) + (short)DAT_1f800024;
    DAT_1f800030 = *(short *)(DAT_800dcbd0 + 0x30) + (short)DAT_1f800028;
    gte_SetRotMatrix((MATRIX *)(DAT_800dcbd0 + 0x4c));
    gte_ldv0((SVECTOR *)&DAT_1f80002c);
    gte_rtv0();
    gte_stlvnl((VECTOR *)&DAT_1f800020);
    iVar5 = DAT_800dcbd0;
    *(short *)(DAT_800dcbd0 + 0x34) = (short)*(undefined4 *)(DAT_800dcbd0 + 0x60) + (short)*puVar4;
    *(short *)(iVar5 + 0x36) = (short)*(undefined4 *)(iVar5 + 100) + (short)DAT_1f800024;
    sVar3 = (short)*(undefined4 *)(iVar5 + 0x68) + (short)DAT_1f800028;
  }
  *(short *)(iVar5 + 0x38) = sVar3;
  return;
}


