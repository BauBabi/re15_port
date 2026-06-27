/* FUN_80031bf0 @ 0x80031bf0  (Ghidra headless, raw MIPS overlay) */

void FUN_80031bf0(void)

{
  undefined *ot;
  undefined *p;
  int iVar1;
  
  iVar1 = 8;
  p = &DAT_800e2ab0 + (uint)DAT_800ce5e0 * 0xc0;
  ot = &DAT_800cc1e8 + (uint)DAT_800ce5e0 * 0x20;
  do {
    AddPrim(ot,p);
    AddPrim(ot,p + 0xc);
    p = p + 0x18;
    iVar1 = iVar1 + -1;
    ot = ot + 4;
  } while (iVar1 != 0);
  DAT_800e8430 = &DAT_800e2c30 + (uint)DAT_800ce5e0 * 0x500;
  DAT_800e8434 = &DAT_800e5430 + (uint)DAT_800ce5e0 * 0x600;
  return;
}


