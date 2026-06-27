/* FUN_80039694 @ 0x80039694  (Ghidra headless, raw MIPS overlay) */

void FUN_80039694(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = 0;
  iVar1 = 0;
  do {
    (&DAT_800eaad8)[iVar1] = 0;
    (&DAT_800eac28)[iVar1] = 0;
    uVar2 = uVar2 + 1;
    iVar1 = iVar1 + 10;
  } while (uVar2 < 0x21);
  PTR_DAT_800a268c = &DAT_800eaad8;
  PTR_DAT_800a2690 = &DAT_800eac28;
  DAT_800cbc21 = 0;
  DAT_800cbc20 = 0;
  return;
}


