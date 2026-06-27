/* FUN_80083690 @ 0x80083690  (Ghidra headless, raw MIPS overlay) */

void FUN_80083690(void)

{
  uint uVar1;
  ushort uVar2;
  ushort uVar3;
  
  uVar1 = (uint)DAT_800dcc4a;
  if (uVar1 < 0x10) {
    uVar3 = (ushort)(1 << (uVar1 & 0x1f));
    uVar2 = 0;
  }
  else {
    uVar3 = 0;
    uVar2 = (ushort)(1 << (uVar1 - 0x10 & 0x1f));
  }
  (&DAT_800cbced)[uVar1 * 0x36] = 0;
  (&DAT_800cbcd4)[uVar1 * 0x1b] = 0;
  (&DAT_800cbcd0)[uVar1 * 0x1b] = 0;
  DAT_800eaea0 = DAT_800eaea0 | uVar3;
  DAT_800c440c = DAT_800c440c & ~DAT_800eaea0;
  DAT_800eaea2 = DAT_800eaea2 | uVar2;
  DAT_800c440e = DAT_800c440e & ~DAT_800eaea2;
  return;
}


