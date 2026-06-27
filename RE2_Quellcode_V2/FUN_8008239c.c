/* FUN_8008239c @ 0x8008239c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8008239c(byte param_1)

{
  uint uVar1;
  uint uVar2;
  ushort uVar3;
  int local_50 [2];
  undefined2 local_48;
  undefined2 local_46;
  undefined2 local_3c;
  undefined4 local_34;
  undefined2 local_16;
  undefined2 local_14;
  
  FUN_80082710(0);
  DAT_800d52b0 = 0;
  SpuInitMalloc(0x20,"");
  uVar3 = 0;
  uVar1 = 0;
  do {
    (&DAT_800dccb0)[uVar1] = 0;
    uVar3 = uVar3 + 1;
    uVar1 = (uint)uVar3;
  } while (uVar3 < 0xc0);
  uVar3 = 0;
  uVar1 = 0;
  do {
    (&DAT_800d4c80)[uVar1] = 0;
    uVar3 = uVar3 + 1;
    uVar1 = (uint)uVar3;
  } while (uVar3 < 0x18);
  DAT_800eade8 = 0;
  uVar3 = 0;
  uVar1 = 0;
  do {
    (&DAT_800dcc68)[uVar1] = 0;
    uVar3 = uVar3 + 1;
    uVar1 = (uint)uVar3;
  } while (uVar3 < 0x10);
  DAT_800d7854 = param_1;
  if (0x17 < param_1) {
    DAT_800d7854 = 0x18;
  }
  uVar1 = 0;
  local_50[1] = 0x60093;
  local_3c = 0x1000;
  local_34 = 0x1000;
  local_16 = 0x80ff;
  local_48 = 0;
  local_46 = 0;
  local_14 = 0x4000;
  if (DAT_800d7854 != 0) {
    do {
      uVar2 = uVar1 & 0xffff;
      (&DAT_800cbcd2)[uVar2 * 0x1b] = 0x18;
      (&DAT_800cbce0)[uVar2 * 0x1b] = 0xffff;
      (&DAT_800cbcd0)[uVar2 * 0x1b] = 0xff;
      (&DAT_800cbced)[uVar2 * 0x36] = 0;
      (&DAT_800cbcd4)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcd6)[uVar2 * 0x1b] = 0;
      (&DAT_800cbce2)[uVar2 * 0x1b] = 0;
      (&DAT_800cbce4)[uVar2 * 0x1b] = 0;
      (&DAT_800cbce6)[uVar2 * 0x1b] = 0xff;
      (&DAT_800cbcd8)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcdc)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcda)[uVar2 * 0x36] = 0x40;
      (&DAT_800cbcee)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcf0)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcf2)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcf4)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcfa)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcfc)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcfe)[uVar2 * 0x1b] = 0;
      (&DAT_800cbd00)[uVar2 * 0x1b] = 0;
      (&DAT_800cbd02)[uVar2 * 0x1b] = 0;
      (&DAT_800cbcf6)[uVar2 * 0x1b] = 0;
      local_50[0] = 1 << (uVar1 & 0x1f);
      FUN_8007f478(local_50);
      DAT_800dcc4a = (undefined2)uVar1;
      FUN_80083690(1);
      uVar1 = uVar1 + 1;
    } while ((uVar1 & 0xffff) < (uint)DAT_800d7854);
  }
  DAT_800d4c58 = 0x3fff;
  DAT_800d4c5a = 0x3fff;
  DAT_800c440c = 0;
  DAT_800c440e = 0;
  DAT_800eaea0 = 0;
  DAT_800c4412 = 0;
  DAT_800c4414 = 0;
  DAT_800d4c50 = 0;
  _DAT_800d4c54 = 0;
  DAT_800dfade = 0;
  DAT_800d6c40 = 0;
  DAT_800d7664 = 0x80;
  FUN_80081f20();
  return;
}


