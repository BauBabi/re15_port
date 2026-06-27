/* StSetStream @ 0x80088c04  (Ghidra headless, raw MIPS overlay) */

void StSetStream(u_long mode,u_long start_frame,u_long end_frame,func1 *func1,func2 *func2)

{
  FUN_80088e44(1);
  DAT_800ead74 = 0;
  DAT_800d5218 = func1;
  DAT_800d4c68 = mode & 1;
  DAT_800d7850 = 0;
  DAT_800d6c38 = 0;
  DAT_800d4c64 = 0;
  DAT_800cbc24 = 0;
  DAT_800d521c = func2;
  return;
}


