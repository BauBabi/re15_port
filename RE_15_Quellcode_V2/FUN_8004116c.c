void FUN_8004116c(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 uVar1;
  undefined2 uVar2;
  
  uVar1 = (undefined1)param_3;
  uVar2 = (undefined2)param_3;
  switch(param_2) {
  case 0:
    param_1[0xd] = param_3;
    break;
  case 1:
    param_1[0xe] = param_3;
    break;
  case 2:
    param_1[0xf] = param_3;
    break;
  case 3:
    *(undefined2 *)(param_1 + 0x1a) = uVar2;
    break;
  case 4:
    *(undefined2 *)((int)param_1 + 0x6a) = uVar2;
    break;
  case 5:
    *(undefined2 *)(param_1 + 0x1b) = uVar2;
    break;
  case 6:
    *param_1 = param_3;
    break;
  case 7:
    param_1[3] = param_3;
    break;
  case 8:
    *(undefined1 *)(param_1 + 1) = uVar1;
    break;
  case 9:
    *(undefined1 *)((int)param_1 + 5) = uVar1;
    break;
  case 10:
    *(undefined1 *)((int)param_1 + 6) = uVar1;
    break;
  case 0xb:
    *(undefined1 *)((int)param_1 + 7) = uVar1;
    break;
  case 0xc:
    *(undefined1 *)((int)param_1 + 9) = uVar1;
    break;
  case 0xd:
    *(undefined1 *)(param_1 + 2) = uVar1;
    break;
  case 0xe:
    *(undefined1 *)((int)param_1 + 10) = uVar1;
    break;
  case 0xf:
    *(undefined1 *)((int)param_1 + 0xb) = uVar1;
    break;
  case 0x10:
    *(undefined2 *)(param_1 + 0x71) = uVar2;
    break;
  case 0x11:
    *(undefined2 *)(param_1 + 0x26) = uVar2;
    break;
  case 0x12:
    *(undefined1 *)((int)param_1 + 0x82) = uVar1;
    break;
  case 0x13:
    *(undefined2 *)((int)param_1 + 0x1ba) = uVar2;
  }
  return;
}
