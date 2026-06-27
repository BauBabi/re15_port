/* FUN_800743f8 @ 0x800743f8  (Ghidra headless, raw MIPS overlay) */

void FUN_800743f8(void)

{
  short sVar1;
  short sVar2;
  u_short uVar3;
  int iVar4;
  u_short *puVar5;
  short *psVar6;
  uint uVar7;
  undefined *p;
  undefined *p_00;
  undefined *ot;
  
  sVar2 = DAT_800d5c36;
  sVar1 = DAT_800d5c34;
  p = &UNK_8019b800;
  uVar7 = 8;
  ot = &UNK_800cc1f0 + (uint)DAT_800ce5e0 * 0x20;
  if (DAT_800ce5e0 != 0) {
    p = &DAT_8019b814;
  }
  psVar6 = (short *)(p + 10);
  iVar4 = 0x20;
  do {
    p_00 = p;
    uVar7 = uVar7 - 1;
    psVar6[-1] = *(short *)(&DAT_800ab084 + iVar4) + sVar1;
    p = p_00 + 0x28;
    *psVar6 = *(short *)(&DAT_800ab086 + iVar4) + sVar2;
    AddPrim(ot,p_00);
    psVar6 = psVar6 + 0x14;
    iVar4 = iVar4 + -4;
  } while (4 < uVar7);
  puVar5 = (u_short *)(p_00 + 0x36);
  AddPrim(ot,&UNK_800d6a10 + (uint)DAT_800ce5e0 * 0xc);
  iVar4 = uVar7 * 4;
  do {
    uVar7 = uVar7 - 1;
    if ((DAT_800d5c00 == '\0') && (DAT_800d5bfe == uVar7)) {
      if (DAT_800d5bf1 < 3) {
        *(undefined1 *)(puVar5 + -5) = 0x80;
        *(undefined1 *)((int)puVar5 + -9) = 0x80;
        *(undefined1 *)(puVar5 + -4) = 0x80;
      }
      else {
        *(undefined1 *)(puVar5 + -5) = 0x80;
                    /* Possible PsyQ macro: setLineF2() */
        *(undefined1 *)((int)puVar5 + -9) = 0x40;
        *(undefined1 *)(puVar5 + -4) = 0x40;
      }
    }
    else {
      *(undefined1 *)(puVar5 + -5) = 0x28;
                    /* Possible PsyQ macro: setPolyF4() */
      *(undefined1 *)((int)puVar5 + -9) = 0x28;
      *(undefined1 *)(puVar5 + -4) = 0x28;
    }
    puVar5[-3] = *(short *)(&DAT_800ab084 + iVar4) + sVar1;
    puVar5[-2] = *(short *)(&DAT_800ab086 + iVar4) + sVar2;
    uVar3 = GetClut(0x100,0x1f0);
    *puVar5 = uVar3;
    AddPrim(ot,p);
    puVar5 = puVar5 + 0x14;
    p = p + 0x28;
    iVar4 = iVar4 + -4;
  } while (uVar7 != 0);
  AddPrim(ot,&UNK_800d6b00 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


