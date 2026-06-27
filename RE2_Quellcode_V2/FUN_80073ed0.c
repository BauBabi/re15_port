/* FUN_80073ed0 @ 0x80073ed0  (Ghidra headless, raw MIPS overlay) */

void FUN_80073ed0(void)

{
  short sVar1;
  short sVar2;
  int iVar3;
  short *psVar5;
  undefined *p;
  undefined *ot;
  undefined *p_00;
  int iVar4;
  
  sVar2 = DAT_800d5c2a;
  sVar1 = DAT_800d5c28;
  p_00 = &UNK_8019b200;
  p = &DAT_8019b280;
  ot = &DAT_800cc200 + (uint)DAT_800ce5e0 * 0x20;
  if (DAT_800ce5e0 != 0) {
    p_00 = &DAT_8019b218;
    p = &DAT_8019b294;
  }
  psVar5 = (short *)(p + 10);
  iVar4 = 0x44;
  do {
    iVar3 = iVar4 + -4;
    psVar5[-1] = *(short *)(&DAT_800aaf98 + iVar4) + sVar1;
    *psVar5 = *(short *)(&DAT_800aaf9a + iVar4) + sVar2;
    AddPrim(ot,p);
    psVar5 = psVar5 + 0x14;
    iVar4 = iVar3;
    p = p + 0x28;
  } while (iVar3 != 0);
  *(short *)(p_00 + 8) = sVar1;
  *(short *)(p_00 + 10) = sVar2;
  *(short *)(p_00 + 0xc) = sVar1 + 0xd4;
  *(short *)(p_00 + 0xe) = sVar2;
  *(short *)(p_00 + 0x10) = sVar1;
  *(short *)(p_00 + 0x12) = sVar2 + 0x35;
  *(short *)(p_00 + 0x14) = sVar1 + 0xd4;
  *(short *)(p_00 + 0x16) = sVar2 + 0x35;
  AddPrim(ot,p_00);
  AddPrim(ot,&UNK_800d6a70 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


