/* FUN_800401d4 @ 0x800401d4  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800401d4(int param_1,int param_2)

{
  int iVar1;
  
  if ((((DAT_800cfb74 & 0x40) != 0) || (0x78 < (short)DAT_800cfd4e)) && (param_1 < 0x29)) {
    param_1 = param_1 + (param_1 >> 1);
  }
  if (DAT_800d482a == 3) {
    if (param_1 < 0x1e) {
      param_1 = param_1 * 5;
    }
    else if (param_1 < 0x3c) {
      param_1 = param_1 << 1;
    }
  }
  param_1 = (uint)DAT_800cfd4e - param_1;
  DAT_800cfd4e = (ushort)param_1;
  if (param_2 == 1) {
    if (-1 < param_1 * 0x10000) {
      return 0;
    }
  }
  else {
    if (param_2 != 0) {
      return 1;
    }
    iVar1 = param_1 * 0x10000 >> 0x10;
    if (-1 < iVar1) {
      return 0;
    }
    if ((iVar1 < -0xe) || ((DAT_800cfd4c & 0x1000) != 0)) {
      DAT_800cfb74 = DAT_800cfb74 | 0x4000000;
      DAT_800cfdcb = DAT_800cfdcb | 0x80;
      return 2;
    }
  }
  DAT_800cfd4e = 0;
  DAT_800cfd4c = DAT_800cfd4c | 0x1000;
  return 1;
}


