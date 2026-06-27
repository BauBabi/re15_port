/* FUN_8007526c @ 0x8007526c  (Ghidra headless, raw MIPS overlay) */

void FUN_8007526c(void)

{
  uint uVar1;
  short *psVar2;
  short sVar3;
  int iVar4;
  short sVar5;
  undefined *p;
  int iVar6;
  
  p = &DAT_80199200;
  iVar6 = 5;
  uVar1 = (uint)DAT_800ce5e0;
  if (uVar1 != 0) {
    p = &DAT_80199214;
  }
  sVar5 = 0xc0;
  do {
    iVar6 = iVar6 + -1;
    iVar4 = 7;
    psVar2 = (short *)(p + 10);
    sVar3 = 0x150;
    do {
      sVar3 = sVar3 + -0x30;
      iVar4 = iVar4 + -1;
      psVar2[-1] = sVar3;
      *psVar2 = sVar5;
      AddPrim(&UNK_800cc204 + uVar1 * 0x20,p);
      psVar2 = psVar2 + 0x14;
      p = p + 0x28;
    } while (iVar4 != 0);
    sVar5 = sVar5 + -0x30;
  } while (iVar6 != 0);
  AddPrim(&UNK_800cc204 + uVar1 * 0x20,&UNK_800d6b60 + (uint)DAT_800ce5e0 * 0xc);
  return;
}


