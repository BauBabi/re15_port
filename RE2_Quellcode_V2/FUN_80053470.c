/* FUN_80053470 @ 0x80053470  (Ghidra headless, raw MIPS overlay) */

void FUN_80053470(void)

{
  if ((DAT_800cfbd8 & 0x80000) != 0) {
    FUN_8002c1a0(0x102,0xffff8000,2,7);
    FUN_8002c2b0(2,0x800,0,0);
  }
  return;
}


