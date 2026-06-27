uint FUN_80041358(uint *param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = (uint)(param_2 < 0x14);
  if (uVar1 != 0) {
    switch((uint)(&switchD_8004137c::switchdataD_80010cfc)[param_2] & 0xfffffffe) {
    case 0x80041384:
      uVar1 = param_1[0xd];
      break;
    case 0x80041390:
      uVar1 = param_1[0xe];
      break;
    case 0x8004139c:
      uVar1 = param_1[0xf];
      break;
    case 0x800413a8:
      uVar1 = (uint)(short)param_1[0x1a];
      break;
    case 0x800413b4:
      uVar1 = (uint)*(short *)((int)param_1 + 0x6a);
      break;
    case 0x800413c0:
      uVar1 = (uint)(short)param_1[0x1b];
      break;
    case 0x800413cc:
      uVar1 = *param_1;
      break;
    case 0x800413d8:
      uVar1 = param_1[3];
      break;
    case 0x800413e4:
      uVar1 = (uint)(byte)param_1[1];
      break;
    case 0x800413f0:
      uVar1 = (uint)*(byte *)((int)param_1 + 5);
      break;
    case 0x800413fc:
      uVar1 = (uint)*(byte *)((int)param_1 + 6);
      break;
    case 0x80041408:
      uVar1 = (uint)*(byte *)((int)param_1 + 7);
      break;
    case 0x80041414:
      uVar1 = (uint)*(byte *)((int)param_1 + 9);
      break;
    case 0x80041420:
      uVar1 = (uint)(byte)param_1[2];
      break;
    case 0x8004142c:
      uVar1 = (uint)*(byte *)((int)param_1 + 10);
      break;
    case 0x80041438:
      uVar1 = (uint)*(byte *)((int)param_1 + 0xb);
      break;
    case 0x80041444:
      uVar1 = (uint)(ushort)param_1[0x71];
      break;
    case 0x80041450:
      uVar1 = (uint)(ushort)param_1[0x26];
      break;
    case 0x8004145c:
      uVar1 = (uint)*(byte *)((int)param_1 + 0x82);
      break;
    case 0x80041468:
      uVar1 = (uint)*(short *)((int)param_1 + 0x1ba);
    }
  }
  return uVar1;
}
