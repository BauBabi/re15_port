uint FUN_80041554(uint param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = (uint)(param_2 < 0x14);
  if (param_2 < 0x14 != 0) {
    switch((uint)(&switchD_80041578::switchdataD_80010d4c)[param_2] & 0xfffffffe) {
    case 0x80041580:
      uVar1 = param_1 + 0x34;
      break;
    case 0x80041588:
      uVar1 = param_1 + 0x38;
      break;
    case 0x80041590:
      uVar1 = param_1 + 0x3c;
      break;
    case 0x80041598:
      uVar1 = param_1 + 0x68;
      break;
    case 0x800415a0:
      uVar1 = param_1 + 0x6a;
      break;
    case 0x800415a8:
      uVar1 = param_1 + 0x6c;
      break;
    case 0x800415b0:
      uVar1 = param_1;
      break;
    case 0x800415b8:
      uVar1 = param_1 + 0xc;
      break;
    case 0x800415c0:
      uVar1 = param_1 + 4;
      break;
    case 0x800415c8:
      uVar1 = param_1 + 5;
      break;
    case 0x800415d0:
      uVar1 = param_1 + 6;
      break;
    case 0x800415d8:
      uVar1 = param_1 + 7;
      break;
    case 0x800415e0:
      uVar1 = param_1 + 9;
      break;
    case 0x800415e8:
      uVar1 = param_1 + 8;
      break;
    case 0x800415f0:
      uVar1 = param_1 + 10;
      break;
    case 0x800415f8:
      uVar1 = param_1 + 0xb;
      break;
    case 0x80041600:
      uVar1 = param_1 + 0x1c4;
      break;
    case 0x80041608:
      uVar1 = param_1 + 0x98;
      break;
    case 0x80041610:
      uVar1 = param_1 + 0x82;
      break;
    case 0x80041618:
      uVar1 = param_1 + 0x1ba;
    }
  }
  return uVar1;
}
