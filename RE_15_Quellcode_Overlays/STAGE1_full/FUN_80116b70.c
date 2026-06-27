/* FUN_80116b70 @ 0x80116b70  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116b70(void)

{
  undefined4 uVar1;
  
  switch(*(byte *)(_DAT_800ac784 + 9) & 0x7f) {
  case 0:
  case 3:
    uVar1 = 8;
    break;
  case 1:
  case 2:
  case 4:
    uVar1 = 0x10;
    break;
  default:
    goto switchD_80116bac_default;
  }
  func_0x80019d50(uVar1,3,0x23,*(int *)(_DAT_800ac784 + 0x188) + 0x40);
switchD_80116bac_default:
  return;
}


