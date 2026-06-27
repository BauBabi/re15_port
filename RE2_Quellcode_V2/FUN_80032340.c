/* FUN_80032340 @ 0x80032340  (Ghidra headless, raw MIPS overlay) */

void FUN_80032340(void)

{
  do {
    (*(code *)(&PTR_LAB_800a23ac)[DAT_800d7680])();
    if ((DAT_800cfb74 & 0x8000) != 0) {
      (&DAT_800dfb4d)[(DAT_800ce5e0 ^ 1) * 0x98] = (char)DAT_800d4238;
      *(undefined2 *)(&DAT_800dfb40 + (DAT_800ce5e0 ^ 1) * 0x98) = DAT_800d7682;
    }
    if (DAT_800d7688 != 0) {
      DAT_800ce5e0 = DAT_800ce5e0 ^ 1;
    }
  } while (DAT_800d7680 < 2);
  return;
}


