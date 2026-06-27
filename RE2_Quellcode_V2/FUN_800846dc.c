/* FUN_800846dc @ 0x800846dc  (Ghidra headless, raw MIPS overlay) */

void FUN_800846dc(short param_1)

{
  uint uVar1;
  byte bVar2;
  
  bVar2 = 0;
  if (DAT_800d7854 != 0) {
    uVar1 = 0;
    do {
      if ((&DAT_800cbce0)[uVar1 * 0x1b] == param_1) {
        DAT_800dcc4a = (ushort)bVar2;
        FUN_80083690(0);
      }
      bVar2 = bVar2 + 1;
      uVar1 = (uint)bVar2;
    } while (bVar2 < DAT_800d7854);
  }
  return;
}


