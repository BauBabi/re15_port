/* FUN_8006e7f0 @ 0x8006e7f0  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8006e7f0(undefined4 param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  switch(param_1) {
  case 0:
    if (((param_2 == 0x11) || (param_2 == 0x17)) || ((param_2 == 0x12 && (DAT_800cfcfe == 3)))) {
      return 4;
    }
    if (param_2 < 4) {
      return 0;
    }
    if (param_2 < 8) {
      return 1;
    }
    if (param_2 < 0x16) {
      return 3;
    }
    if (param_2 == 0x16) {
      if (DAT_800cfcfe == 4) {
        return 3;
      }
      return 2;
    }
  default:
    uVar2 = 0;
    break;
  case 1:
    if (param_2 == 0x1b) {
      return 3;
    }
    if (param_2 < 0x11) {
      return 2;
    }
    goto LAB_8006e8ac;
  case 2:
    if (param_2 < 2) {
      return 5;
    }
    if (param_2 != 8) {
      return 6;
    }
LAB_8006e8ac:
    uVar2 = 5;
    break;
  case 3:
    if ((((param_2 == 0) && (3 < DAT_800cfcfe)) || ((param_2 == 1 && (3 < DAT_800cfcfe)))) ||
       ((((param_2 == 3 && (3 < DAT_800cfcfe)) || (param_2 == 0xb)) ||
        ((param_2 == 2 || (uVar2 = 8, param_2 == 9)))))) {
      uVar2 = 7;
    }
    break;
  case 4:
    uVar2 = 9;
    if (2 < param_2) {
      if (param_2 == 8) {
        uVar2 = 9;
      }
      else {
        if (((DAT_800cfbd8 & 0x40000000) == 0) || (param_2 != 3)) {
          if (param_2 == 5) {
            if ((DAT_800cfbd8 & 0x40000000) == 0) {
              iVar1 = FUN_80077360(&DAT_800d4854,0xbe);
              uVar2 = 0x10;
            }
            else {
              iVar1 = FUN_80077360(&DAT_800d4854,0xbe);
              uVar2 = 0xd;
            }
            if (iVar1 != 0) {
              return uVar2;
            }
            iVar1 = FUN_80077360(&DAT_800d4854,99);
            if (iVar1 != 0) {
              return 0xc;
            }
          }
          if (5 < param_2) {
            if (param_2 == 9) {
              return 0xc;
            }
            return 0xb;
          }
        }
        else {
          iVar1 = FUN_80077360(&DAT_800d4854,0x62);
          if (iVar1 == 0) {
            return 0xb;
          }
        }
        uVar2 = 10;
      }
    }
    break;
  case 5:
    if (param_2 == 1) {
      uVar2 = 0x10;
      if (DAT_800d4822 == 0x602) {
        uVar2 = 0xd;
      }
    }
    else if ((param_2 != 3) || (uVar2 = 0xe, 2 < DAT_800cfcfe)) {
      if ((param_2 == 0xe) && (DAT_800d4e00 == '\x01')) {
        uVar2 = 0x11;
      }
      else {
        uVar2 = 0xd;
        if (4 < param_2) {
          if (param_2 == 5) {
            uVar2 = 0xf;
          }
          else {
            uVar2 = 0x11;
            if (param_2 < 0x12) {
              uVar2 = 0x10;
            }
          }
        }
      }
    }
    break;
  case 6:
    iVar1 = FUN_80077360(&DAT_800d4854,0x89);
    uVar2 = 0x12;
    if (iVar1 != 0) {
      uVar2 = 0x13;
    }
  }
  return uVar2;
}


