/* FUN_80034a84 @ 0x80034a84  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80034a84(void)

{
  uint *puVar1;
  uint *puVar2;
  short sVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint *puVar7;
  uint *puVar8;
  
  puVar1 = &DAT_80192a20;
  puVar2 = &DAT_801fb700;
  do {
    puVar8 = puVar2;
    puVar7 = puVar1;
    uVar4 = puVar7[1];
    uVar5 = puVar7[2];
    uVar6 = puVar7[3];
    *puVar8 = *puVar7;
    puVar8[1] = uVar4;
    puVar8[2] = uVar5;
    puVar8[3] = uVar6;
    uVar4 = _UNK_80193650;
    puVar1 = puVar7 + 4;
    puVar2 = puVar8 + 4;
  } while (puVar7 + 4 != (uint *)&UNK_80193650);
  uVar5 = puVar7[5];
  uVar6 = (int)puVar8 + 0x13U & 3;
  puVar1 = (uint *)(((int)puVar8 + 0x13U) - uVar6);
  *puVar1 = *puVar1 & -1 << (uVar6 + 1) * 8 | _UNK_80193650 >> (3 - uVar6) * 8;
  puVar8[4] = uVar4;
  uVar4 = (int)puVar8 + 0x17U & 3;
  puVar1 = (uint *)(((int)puVar8 + 0x17U) - uVar4);
  *puVar1 = *puVar1 & -1 << (uVar4 + 1) * 8 | uVar5 >> (3 - uVar4) * 8;
  puVar8[5] = uVar5;
  DAT_800d75a0 = (uchar *)((int)&DAT_801fb700 + DAT_801fc330);
  if (-1 < DAT_800d4c48) {
    SsVabClose((short)DAT_800d4c48);
    DAT_800d4c48 = -1;
  }
  DAT_800dbb78 = &DAT_801fb700;
  do {
    sVar3 = SsVabOpenHeadSticky(DAT_800d75a0,0,0x3dc50);
    DAT_800d4c48 = (char)sVar3;
  } while (DAT_800d4c48 == -1);
  SsVabTransBody(&DAT_80193658,(short)DAT_800d4c48);
  SsVabTransCompleted(1);
  return;
}


