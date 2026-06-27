/* FUN_80083c2c @ 0x80083c2c  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80083c2c(short param_1,short param_2,short param_3,short param_4,int param_5)

{
  undefined2 uVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  
  iVar5 = (int)param_1;
  uVar2 = 0;
  if ((((&DAT_800cbce0)[iVar5 * 0x1b] == param_2) &&
      (uVar2 = 0, (&DAT_800cbce8)[iVar5 * 0x1b] == param_3)) &&
     (uVar2 = 0, (&DAT_800cbce4)[iVar5 * 0x1b] == param_4)) {
    uVar6 = (uint)(ushort)(&DAT_800cbcde)[iVar5 * 0x1b];
    uVar4 = (uint)(ushort)(&DAT_800cbce6)[iVar5 * 0x1b] + (uint)DAT_800dcc37 * 0x10;
    iVar5 = (param_5 + -0x40) * 0x10000 >> 0x10;
    if (iVar5 < 1) {
      if (iVar5 < 0) {
        iVar5 = iVar5 * (uint)*(byte *)((uVar4 & 0xffff) * 0x20 + DAT_800d784c + 0xc);
        iVar3 = iVar5;
        if (iVar5 < 0) {
          iVar3 = iVar5 + 0x3f;
        }
        uVar6 = (uVar6 + (iVar3 >> 6)) - 1;
        uVar4 = (iVar5 + (iVar3 >> 6) * -0x40) * 2 + 0x7f;
      }
      else {
        uVar4 = 0;
      }
    }
    else {
      iVar5 = iVar5 * (uint)*(byte *)((uVar4 & 0xffff) * 0x20 + DAT_800d784c + 0xd);
      uVar6 = uVar6 + iVar5 / 0x3f;
      uVar4 = (iVar5 % 0x3f) * 2;
    }
    iVar5 = (int)param_1;
    DAT_800dcc3c = (undefined1)(&DAT_800cbce6)[iVar5 * 0x1b];
    DAT_800dcc4a = param_1;
    uVar1 = note2pitch2(uVar6 & 0xffff,uVar4 & 0xffff);
    (&DAT_800dccb4)[iVar5 * 8] = uVar1;
    (&DAT_800d4c80)[iVar5] = (&DAT_800d4c80)[iVar5] | 4;
    uVar2 = 1;
  }
  return uVar2;
}


