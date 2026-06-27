/* FUN_80116d00 @ 0x80116d00  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116d00(void)

{
  ushort uVar1;
  uint uVar2;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar2 = 0x8030000;
    break;
  case 1:
  case 2:
  case 4:
    uVar2 = 0x10030000;
    break;
  default:
    goto switchD_80116d3c_default;
  }
  func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d0) << 8 | uVar2,
                  (int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x40,
                  &DAT_80121248);
switchD_80116d3c_default:
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1d4) = (uVar1 & 0x3f) + 0x10;
  return;
}


