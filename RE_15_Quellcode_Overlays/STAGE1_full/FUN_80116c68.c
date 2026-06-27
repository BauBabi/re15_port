/* FUN_80116c68 @ 0x80116c68  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116c68(void)

{
  ushort uVar1;
  undefined4 uVar2;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar2 = 8;
    break;
  case 1:
  case 2:
  case 4:
    uVar2 = 0x10;
    break;
  default:
    goto switchD_80116ca4_default;
  }
  func_0x80019d50(uVar2,3,0x12,*(int *)(_DAT_800ac784 + 0x188) + 0x40);
switchD_80116ca4_default:
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x1d4) = (uVar1 & 0x3f) + 0x10;
  return;
}


