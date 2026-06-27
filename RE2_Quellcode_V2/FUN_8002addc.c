/* FUN_8002addc @ 0x8002addc  (Ghidra headless, raw MIPS overlay) */

void FUN_8002addc(void)

{
  *DAT_800dfd70 = *DAT_800dfd70 & 0xff000000 | DAT_800dfd6c & 0xffffff;
  return;
}


