/* FUN_8007f358 @ 0x8007f358  (Ghidra headless, raw MIPS overlay) */

void FUN_8007f358(void)

{
  uint uVar1;
  int iVar2;
  int local_50 [2];
  undefined2 local_48;
  undefined2 local_46;
  undefined2 local_3c;
  undefined4 local_34;
  undefined2 local_16;
  undefined2 local_14;
  
  iVar2 = 0;
  local_50[1] = 0x60093;
  local_3c = 0x1000;
  local_34 = 0x1000;
  local_16 = 0x80ff;
  local_48 = 0;
  local_46 = 0;
  local_14 = 0x4000;
  if (DAT_800d7854 != 0) {
    do {
      uVar1 = (uint)(short)iVar2;
      (&DAT_800cbcd2)[uVar1 * 0x1b] = 0x18;
      (&DAT_800cbcd6)[uVar1 * 0x1b] = 0;
      (&DAT_800cbce0)[uVar1 * 0x1b] = 0xff;
      (&DAT_800cbce2)[uVar1 * 0x1b] = 0;
      (&DAT_800cbce4)[uVar1 * 0x1b] = 0;
      (&DAT_800cbce6)[uVar1 * 0x1b] = 0xff;
      local_50[0] = 1 << (uVar1 & 0x1f);
      FUN_8007f478(local_50);
      DAT_800dcc4a = (short)iVar2;
      FUN_80083690(1);
      iVar2 = iVar2 + 1;
    } while (iVar2 * 0x10000 >> 0x10 < (int)(uint)DAT_800d7854);
  }
  return;
}


