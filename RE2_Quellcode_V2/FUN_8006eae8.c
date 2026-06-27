/* FUN_8006eae8 @ 0x8006eae8  (Ghidra headless, raw MIPS overlay) */

uint FUN_8006eae8(undefined4 param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = param_2;
  switch(param_1) {
  case 0:
    if (0x17 < param_2) {
      uVar1 = param_2 - 0x14;
    }
    break;
  case 2:
    if (param_2 == 0x16) {
      uVar1 = 0x11;
    }
    break;
  case 3:
    if (param_2 == 0x1b) {
      uVar1 = 0xe;
    }
    else {
      uVar1 = param_2 - 8;
      if (0x11 < param_2) {
        uVar1 = param_2 - 9;
      }
    }
    break;
  case 4:
    if (param_2 != 0x17) {
      return param_2 - 0x10;
    }
    param_2 = 0x10;
    goto LAB_8006eb84;
  case 5:
    if (param_2 == 8) goto LAB_8006ec84;
    if (param_2 < 0x19) {
      if (param_2 < 0x11) {
        return param_2;
      }
      return param_2 - 0xe;
    }
LAB_8006eb84:
    uVar1 = param_2 - 0x10;
    break;
  case 6:
    uVar1 = param_2 - 3;
    if (param_2 < 9) {
      uVar1 = param_2 - 2;
    }
    break;
  case 7:
    if (param_2 == 9) {
      uVar1 = 4;
    }
    else if (param_2 == 0xb) {
      uVar1 = 5;
    }
    break;
  case 8:
    if (param_2 < 2) goto switchD_8006eb04_caseD_d;
    if (param_2 < 6) {
      return param_2 - 1;
    }
    if (param_2 < 9) {
      return param_2 - 2;
    }
    if (param_2 < 0xb) {
      return param_2 - 3;
    }
    if (0x10 < param_2) {
      return param_2;
    }
  case 1:
    uVar1 = param_2 - 4;
    break;
  case 9:
    if (param_2 == 8) {
      uVar1 = 3;
    }
    break;
  case 10:
    uVar1 = param_2 - 3;
    break;
  case 0xb:
    if (param_2 != 3) {
      return param_2 - 5;
    }
  default:
    uVar1 = 0;
    break;
  case 0xc:
    uVar1 = (uint)(param_2 != 9);
    break;
  case 0xd:
  case 0x12:
switchD_8006eb04_caseD_d:
    break;
  case 0x10:
    if (param_2 == 5) {
      return 0xc;
    }
    if (param_2 == 1) {
      return 0xb;
    }
    if (param_2 != 0x11) {
      return param_2 - 6;
    }
LAB_8006ec84:
    uVar1 = 2;
    break;
  case 0x11:
    uVar1 = param_2 - 0x12;
    if (param_2 == 0xe) {
      uVar1 = 6;
    }
    break;
  case 0x13:
    uVar1 = (uint)(param_2 != 3);
  }
  return uVar1;
}


