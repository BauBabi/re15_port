/* FUN_80055cb0 @ 0x80055cb0  (Ghidra headless, raw MIPS overlay) */

void FUN_80055cb0(undefined2 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  uVar1 = (undefined1)param_3;
  uVar2 = (undefined2)param_3;
  switch(param_2) {
  case 0:
    *param_1 = uVar2;
    break;
  case 1:
    param_1[1] = uVar2;
    break;
  case 2:
    *(undefined1 *)(param_1 + 2) = uVar1;
    break;
  case 3:
    *(undefined1 *)((int)param_1 + 5) = uVar1;
    break;
  case 4:
    *(undefined1 *)(param_1 + 3) = uVar1;
    break;
  case 5:
    *(undefined1 *)((int)param_1 + 7) = uVar1;
    break;
  case 6:
    *(undefined1 *)(param_1 + 4) = uVar1;
    break;
  case 7:
    param_1[0x87] = uVar2;
    break;
  case 8:
    *(undefined1 *)(param_1 + 5) = uVar1;
    break;
  case 9:
    *(undefined1 *)((int)param_1 + 0xb) = uVar1;
    break;
  case 10:
    *(undefined4 *)(param_1 + 8) = param_3;
    break;
  case 0xb:
    *(undefined4 *)(param_1 + 0x1c) = param_3;
    break;
  case 0xc:
    *(undefined4 *)(param_1 + 0x1e) = param_3;
    break;
  case 0xd:
    *(undefined4 *)(param_1 + 0x20) = param_3;
    break;
  case 0xe:
    param_1[0x3a] = uVar2;
    break;
  case 0xf:
    param_1[0x3b] = uVar2;
    break;
  case 0x10:
    param_1[0x3c] = uVar2;
    break;
  case 0x11:
    *(undefined1 *)(param_1 + 0x83) = uVar1;
    break;
  case 0x12:
    param_1[0xaa] = uVar2;
    break;
  case 0x13:
    param_1[0xe1] = uVar2;
    break;
  case 0x14:
    param_1[0xe2] = uVar2;
    break;
  case 0x15:
    param_1[0xe3] = uVar2;
    break;
  case 0x16:
    param_1[0xe6] = uVar2;
    break;
  case 0x17:
    param_1[0xea] = uVar2;
    break;
  case 0x18:
    param_1[0xeb] = uVar2;
    break;
  case 0x19:
    param_1[0xec] = uVar2;
    break;
  case 0x1a:
    param_1[0xed] = uVar2;
    break;
  case 0x1b:
    *(undefined1 *)(param_1 + 0xa2) = uVar1;
    break;
  case 0x1c:
    param_1[0xa2] = uVar2;
    break;
  case 0x1d:
    param_1[0xa3] = uVar2;
    break;
  case 0x1e:
    param_1[0xa4] = uVar2;
    break;
  case 0x1f:
    *(undefined1 *)(param_1 + 0xa7) = uVar1;
    break;
  case 0x20:
    param_1[0x4a] = uVar2;
    break;
  case 0x21:
    param_1[0x4c] = uVar2;
    break;
  case 0x22:
    param_1[0x4b] = uVar2;
    break;
  case 0x23:
    param_1[0x4d] = uVar2;
    break;
  case 0x24:
    param_1[0x4f] = uVar2;
    break;
  case 0x25:
    param_1[0x4e] = uVar2;
    break;
  case 0x26:
    param_1[0xef] = uVar2;
    break;
  case 0x27:
    param_1[0x8c] = uVar2;
    break;
  case 0x28:
    param_1[0x8d] = uVar2;
    break;
  case 0x29:
    param_1[0x10c] = uVar2;
    break;
  case 0x2a:
    param_1[0x10d] = uVar2;
    break;
  case 0x2b:
    *(undefined1 *)((int)param_1 + 0x1d3) = uVar1;
  }
  return;
}


