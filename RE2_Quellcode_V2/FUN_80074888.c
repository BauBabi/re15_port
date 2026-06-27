/* FUN_80074888 @ 0x80074888  (Ghidra headless, raw MIPS overlay) */

void FUN_80074888(int param_1)

{
  short sVar1;
  short sVar2;
  u_short uVar3;
  uint uVar4;
  int iVar5;
  u_short *puVar6;
  short *psVar7;
  int iVar8;
  uint uVar9;
  undefined *p;
  undefined *p_00;
  undefined *ot;
  
  sVar2 = DAT_800d5c3a;
  sVar1 = DAT_800d5c38;
  p = &DAT_8019ba00;
  uVar4 = (uint)DAT_800d5bff;
  ot = &DAT_800cc200 + (uint)DAT_800ce5e0 * 0x20;
  if (param_1 == 0) {
    DAT_800ab0ce = 0x38;
    uVar9 = 8;
  }
  else {
    DAT_800ab0ce = 0x4d;
    uVar9 = 9;
  }
  if (DAT_800ce5e0 != 0) {
    p = &DAT_8019ba14;
  }
  psVar7 = (short *)(p + 10);
  iVar5 = uVar9 << 2;
  do {
    p_00 = p;
    uVar9 = uVar9 - 1;
    psVar7[-1] = *(short *)(&DAT_800ab0c8 + iVar5) + sVar1;
    p = p_00 + 0x28;
    *psVar7 = *(short *)(&DAT_800ab0ca + iVar5) + sVar2;
    AddPrim(ot,p_00);
    psVar7 = psVar7 + 0x14;
    iVar5 = iVar5 + -4;
  } while (4 < uVar9);
  puVar6 = (u_short *)(p_00 + 0x36);
  AddPrim(ot,&UNK_800d6aa0 + (uint)DAT_800ce5e0 * 0xc);
  iVar5 = uVar9 * 4;
  do {
    iVar8 = iVar5 + -4;
    if (iVar8 == uVar4 << 2) {
      *(undefined1 *)(puVar6 + -5) = 0x40;
                    /* Possible PsyQ macro: setLineF2() */
      *(undefined1 *)((int)puVar6 + -9) = 0x40;
    }
    else {
      *(undefined1 *)(puVar6 + -5) = 0x80;
      *(undefined1 *)((int)puVar6 + -9) = 0x80;
    }
    *(undefined1 *)(puVar6 + -4) = 0x80;
    puVar6[-3] = *(short *)(&DAT_800ab0c8 + iVar5) + sVar1;
    puVar6[-2] = *(short *)(&DAT_800ab0ca + iVar5) + sVar2;
    uVar3 = GetClut(0x100,0x1ea);
    *puVar6 = uVar3;
    AddPrim(ot,p);
    puVar6 = puVar6 + 0x14;
    p = p + 0x28;
    iVar5 = iVar8;
  } while (iVar8 != 0);
  AddPrim(ot,&UNK_800d6b18 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


