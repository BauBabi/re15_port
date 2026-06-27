/* FUN_80077874 @ 0x80077874  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x800778c0) */

void FUN_80077874(void)

{
  uint uVar1;
  
  FUN_80077924();
  uVar1 = (uint)(DAT_800cbc18 - (int)(&DAT_800c4418 + (uint)DAT_800ce5e0 * 0xf00)) / 0x28;
  if (DAT_800ab218 < uVar1) {
    DAT_800ab218 = uVar1;
  }
  return;
}


