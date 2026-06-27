/* FUN_80056264 @ 0x80056264  (Ghidra headless, raw MIPS overlay) */

uint FUN_80056264(uint param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = (uint)(param_2 < 0x2c);
  if (param_2 < 0x2c != 0) {
    switch((uint)(&switchD_80056284::switchdataD_800113a8)[param_2] & 0xfffffffe) {
    case 0x8005628c:
      uVar1 = param_1;
      break;
    case 0x80056294:
      uVar1 = param_1 + 2;
      break;
    case 0x8005629c:
      uVar1 = param_1 + 4;
      break;
    case 0x800562a4:
      uVar1 = param_1 + 5;
      break;
    case 0x800562ac:
      uVar1 = param_1 + 6;
      break;
    case 0x800562b4:
      uVar1 = param_1 + 7;
      break;
    case 0x800562bc:
      uVar1 = param_1 + 8;
      break;
    case 0x800562c4:
      uVar1 = param_1 + 0x10e;
      break;
    case 0x800562cc:
      uVar1 = param_1 + 10;
      break;
    case 0x800562d4:
      uVar1 = param_1 + 0xb;
      break;
    case 0x800562dc:
      uVar1 = param_1 + 0x10;
      break;
    case 0x800562e4:
      uVar1 = param_1 + 0x38;
      break;
    case 0x800562ec:
      uVar1 = param_1 + 0x3c;
      break;
    case 0x800562f4:
      uVar1 = param_1 + 0x40;
      break;
    case 0x800562fc:
      uVar1 = param_1 + 0x74;
      break;
    case 0x80056304:
      uVar1 = param_1 + 0x76;
      break;
    case 0x8005630c:
      uVar1 = param_1 + 0x78;
      break;
    case 0x80056314:
      uVar1 = param_1 + 0x106;
      break;
    case 0x8005631c:
      uVar1 = param_1 + 0x154;
      break;
    case 0x80056324:
      uVar1 = param_1 + 0x1c2;
      break;
    case 0x8005632c:
      uVar1 = param_1 + 0x1c4;
      break;
    case 0x80056334:
      uVar1 = param_1 + 0x1c6;
      break;
    case 0x8005633c:
      uVar1 = param_1 + 0x1cc;
      break;
    case 0x80056344:
      uVar1 = param_1 + 0x1d4;
      break;
    case 0x8005634c:
      uVar1 = param_1 + 0x1d6;
      break;
    case 0x80056354:
      uVar1 = param_1 + 0x1d8;
      break;
    case 0x8005635c:
      uVar1 = param_1 + 0x1da;
      break;
    default:
      uVar1 = param_1 + 0x144;
      break;
    case 0x8005636c:
      uVar1 = param_1 + 0x146;
      break;
    case 0x80056374:
      uVar1 = param_1 + 0x148;
      break;
    case 0x8005637c:
      uVar1 = param_1 + 0x14e;
      break;
    case 0x80056384:
      uVar1 = param_1 + 0x94;
      break;
    case 0x8005638c:
      uVar1 = param_1 + 0x98;
      break;
    case 0x80056394:
      uVar1 = param_1 + 0x96;
      break;
    case 0x8005639c:
      uVar1 = param_1 + 0x9a;
      break;
    case 0x800563a4:
      uVar1 = param_1 + 0x9e;
      break;
    case 0x800563ac:
      uVar1 = param_1 + 0x9c;
      break;
    case 0x800563b4:
      uVar1 = param_1 + 0x1de;
      break;
    case 0x800563bc:
      uVar1 = param_1 + 0x118;
      break;
    case 0x800563c4:
      uVar1 = param_1 + 0x11a;
      break;
    case 0x800563cc:
      uVar1 = param_1 + 0x218;
      break;
    case 0x800563d4:
      uVar1 = param_1 + 0x21a;
      break;
    case 0x800563dc:
      uVar1 = param_1 + 0x1d3;
    }
  }
  return uVar1;
}


