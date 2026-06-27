/* FUN_80053644 @ 0x80053644  (Ghidra headless, raw MIPS overlay) */

void FUN_80053644(void)

{
  FUN_80053470();
  if ((DAT_800cfbdc & 0x2000000) == 0) {
    FUN_80010778(0x1f800000,&PTR_LAB_800a74c8,0x400);
    FUN_80052ca0();
    DAT_800d8cc8 = 0;
    DAT_800d8cbc = *(undefined4 *)(DAT_800ce324 + 0x4c);
    FUN_80053138(0,1);
    FUN_800536c4();
  }
  return;
}


