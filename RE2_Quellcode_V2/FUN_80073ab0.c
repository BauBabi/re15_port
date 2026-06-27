/* FUN_80073ab0 @ 0x80073ab0  (Ghidra headless, raw MIPS overlay) */

void FUN_80073ab0(void)

{
  short sVar1;
  short sVar2;
  short sVar3;
  short sVar4;
  u_short uVar5;
  u_short *puVar6;
  int iVar7;
  short *psVar9;
  undefined *p;
  undefined *p_00;
  uint uVar10;
  undefined *p_01;
  undefined *p_02;
  undefined *ot;
  uint uVar11;
  int iVar8;
  
  p = &DAT_80198700;
  p_01 = &DAT_80198a00;
  uVar10 = 0xd;
  uVar11 = 10;
  ot = &DAT_800cc200 + (uint)DAT_800ce5e0 * 0x20;
  sVar3 = DAT_800d5c2c + -0x33;
  sVar4 = DAT_800d5c2e + -0xd;
  sVar1 = DAT_800d5c30 + sVar3;
  sVar2 = DAT_800d5c32 + sVar4 + 6;
  if (DAT_800ce5e0 != 0) {
    p = &DAT_80198714;
  }
  psVar9 = (short *)(p + 10);
  iVar8 = 0x34;
  do {
    p_00 = p;
    uVar10 = uVar10 - 1;
    psVar9[-1] = *(short *)(&DAT_800aaef8 + iVar8) + sVar3;
    p = p_00 + 0x28;
    *psVar9 = *(short *)(&DAT_800aaefa + iVar8) + sVar4;
    AddPrim(ot,p_00);
    psVar9 = psVar9 + 0x14;
    iVar8 = iVar8 + -4;
  } while (3 < uVar10);
  if ((DAT_800d5c00 == '\0') && (psVar9 = &DAT_80198a0a, (DAT_800cfbd8 & 0x10000000) != 0)) {
    iVar8 = 0x28;
    p_02 = p_01;
    do {
      uVar11 = uVar11 - 1;
      psVar9[-1] = *(short *)((int)&DAT_800aaf2c + iVar8) + sVar1;
      p_01 = p_02 + 0x28;
      *psVar9 = *(short *)((int)&DAT_800aaf2e + iVar8) + sVar2;
      AddPrim(ot,p_02);
      psVar9 = psVar9 + 0x14;
      iVar8 = iVar8 + -4;
      p_02 = p_01;
    } while (2 < uVar11);
  }
  puVar6 = (u_short *)(p_00 + 0x36);
  AddPrim(ot,&UNK_800d6a88 + (uint)DAT_800ce5e0 * 0xc);
  iVar8 = uVar10 * 4;
  do {
    uVar10 = uVar10 - 1;
    puVar6[-3] = *(short *)(&DAT_800aaef8 + iVar8) + sVar3;
    puVar6[-2] = *(short *)(&DAT_800aaefa + iVar8) + sVar4;
    uVar5 = GetClut(0,0x1e6);
    *puVar6 = uVar5;
    AddPrim(ot,p);
    puVar6 = puVar6 + 0x14;
    p = p + 0x28;
    iVar8 = iVar8 + -4;
  } while (uVar10 != 0);
  if ((DAT_800d5c00 == '\0') && ((DAT_800cfbd8 & 0x10000000) != 0)) {
    puVar6 = (u_short *)(p_01 + 0xe);
    iVar8 = uVar11 << 2;
    do {
      iVar7 = iVar8 + -4;
      puVar6[-3] = *(short *)((int)&DAT_800aaf2c + iVar8) + sVar1;
      puVar6[-2] = *(short *)((int)&DAT_800aaf2e + iVar8) + sVar2;
      uVar5 = GetClut(0,0x1e6);
      *puVar6 = uVar5;
      AddPrim(ot,p_01);
      puVar6 = puVar6 + 0x14;
      p_01 = p_01 + 0x28;
      iVar8 = iVar7;
    } while (iVar7 != 0);
  }
  AddPrim(ot,&UNK_800d6b90 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


