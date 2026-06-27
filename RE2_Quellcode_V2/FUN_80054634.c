/* FUN_80054634 @ 0x80054634  (Ghidra headless, raw MIPS overlay) */

void FUN_80054634(undefined4 param_1,ushort *param_2,uint param_3)

{
  ushort uVar1;
  
  uVar1 = (ushort)param_3;
  switch(param_1) {
  case 0:
    uVar1 = *param_2 + uVar1;
    break;
  case 1:
    uVar1 = *param_2 - uVar1;
    break;
  case 2:
    *param_2 = *param_2 * uVar1;
    return;
  case 3:
    if (param_3 == 0) {
      trap(0x1c00);
    }
    if ((param_3 == 0xffffffff) && ((short)*param_2 == -0x80000000)) {
      trap(0x1800);
    }
    *param_2 = (ushort)((int)(short)*param_2 / (int)param_3);
    return;
  case 4:
    if (param_3 == 0) {
      trap(0x1c00);
    }
    if ((param_3 == 0xffffffff) && ((short)*param_2 == -0x80000000)) {
      trap(0x1800);
    }
    *param_2 = (ushort)((int)(short)*param_2 % (int)param_3);
    return;
  case 5:
    uVar1 = *param_2 | uVar1;
    break;
  case 6:
    uVar1 = *param_2 & uVar1;
    break;
  case 7:
    uVar1 = *param_2 ^ uVar1;
    break;
  case 8:
    uVar1 = ~*param_2;
    break;
  case 9:
    uVar1 = *param_2 << (param_3 & 0x1f);
    break;
  case 10:
    uVar1 = (ushort)((int)(uint)*param_2 >> (param_3 & 0x1f));
    break;
  case 0xb:
    uVar1 = (ushort)((int)(short)*param_2 >> (param_3 & 0x1f));
    break;
  default:
    goto switchD_80054650_default;
  }
  *param_2 = uVar1;
switchD_80054650_default:
  return;
}


