/* FUN_80055f50 @ 0x80055f50  (Ghidra headless, raw MIPS overlay) */

uint FUN_80055f50(ushort *param_1,uint param_2)

{
  uint uVar1;
  
  uVar1 = (uint)(param_2 < 0x2c);
  if (uVar1 != 0) {
    switch((uint)(&switchD_80055f70::switchdataD_800112f8)[param_2] & 0xfffffffe) {
    case 0x80055f78:
      uVar1 = (uint)*param_1;
      break;
    case 0x80055f84:
      uVar1 = (uint)param_1[1];
      break;
    case 0x80055f90:
      uVar1 = (uint)(byte)param_1[2];
      break;
    case 0x80055f9c:
      uVar1 = (uint)*(byte *)((int)param_1 + 5);
      break;
    case 0x80055fa8:
      uVar1 = (uint)(byte)param_1[3];
      break;
    case 0x80055fb4:
      uVar1 = (uint)*(byte *)((int)param_1 + 7);
      break;
    case 0x80055fc0:
      uVar1 = (uint)(byte)param_1[4];
      break;
    case 0x80055fcc:
      uVar1 = (uint)param_1[0x87];
      break;
    case 0x80055fd8:
      uVar1 = (uint)(byte)param_1[5];
      break;
    case 0x80055fe4:
      uVar1 = (uint)*(byte *)((int)param_1 + 0xb);
      break;
    case 0x80055ff0:
      uVar1 = *(uint *)(param_1 + 8);
      break;
    case 0x80055ffc:
      uVar1 = *(uint *)(param_1 + 0x1c);
      break;
    case 0x80056008:
      uVar1 = *(uint *)(param_1 + 0x1e);
      break;
    case 0x80056014:
      uVar1 = *(uint *)(param_1 + 0x20);
      break;
    case 0x80056020:
      uVar1 = (uint)(short)param_1[0x3a];
      break;
    case 0x8005602c:
      uVar1 = (uint)(short)param_1[0x3b];
      break;
    case 0x80056038:
      uVar1 = (uint)(short)param_1[0x3c];
      break;
    case 0x80056044:
      uVar1 = (uint)(byte)param_1[0x83];
      break;
    case 0x80056050:
      uVar1 = (uint)param_1[0xaa];
      break;
    case 0x8005605c:
      uVar1 = (uint)(short)param_1[0xe1];
      break;
    case 0x80056068:
      uVar1 = (uint)(short)param_1[0xe2];
      break;
    case 0x80056074:
      uVar1 = (uint)(short)param_1[0xe3];
      break;
    case 0x80056080:
      uVar1 = (uint)param_1[0xe6];
      break;
    case 0x8005608c:
      uVar1 = (uint)(short)param_1[0xea];
      break;
    case 0x80056098:
      uVar1 = (uint)(short)param_1[0xeb];
      break;
    case 0x800560a4:
      uVar1 = (uint)(short)param_1[0xec];
      break;
    case 0x800560b0:
      uVar1 = (uint)(short)param_1[0xed];
      break;
    case 0x800560bc:
      uVar1 = (uint)(byte)param_1[0xa2];
      break;
    case 0x800560c8:
      uVar1 = (uint)(short)param_1[0xa2];
      break;
    case 0x800560d4:
      uVar1 = (uint)(short)param_1[0xa3];
      break;
    case 0x800560e0:
      uVar1 = (uint)(short)param_1[0xa4];
      break;
    case 0x800560ec:
      uVar1 = (uint)(byte)param_1[0xa7];
      break;
    case 0x800560f8:
      uVar1 = (uint)(short)param_1[0x4a];
      break;
    case 0x80056104:
      uVar1 = (uint)(short)param_1[0x4c];
      break;
    case 0x80056110:
      uVar1 = (uint)(short)param_1[0x4b];
      break;
    case 0x8005611c:
      uVar1 = (uint)param_1[0x4d];
      break;
    case 0x80056128:
      uVar1 = (uint)param_1[0x4f];
      break;
    case 0x80056134:
      uVar1 = (uint)param_1[0x4e];
      break;
    case 0x80056140:
      uVar1 = (uint)(short)param_1[0xef];
      break;
    case 0x8005614c:
      uVar1 = (uint)(short)param_1[0x8c];
      break;
    case 0x80056158:
      uVar1 = (uint)(short)param_1[0x8d];
      break;
    case 0x80056164:
      uVar1 = (uint)param_1[0x10c];
      break;
    case 0x80056170:
      uVar1 = (uint)param_1[0x10d];
      break;
    case 0x8005617c:
      uVar1 = (uint)*(byte *)((int)param_1 + 0x1d3);
    }
  }
  return uVar1;
}


