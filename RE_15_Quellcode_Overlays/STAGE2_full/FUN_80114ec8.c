/* FUN_80114ec8 @ 0x80114ec8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114ec8(void)

{
  int iVar1;
  uint uVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 7)) {
  case 0:
    if (*(char *)(_DAT_800ac784 + 9) == 'A') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e0c;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  case 1:
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0xb) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
      func_0x800245d8(0);
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x1f') {
      *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 1000;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    func_0x800245d8(0x800);
    *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + 0x400;
    goto LAB_80115314;
  case 3:
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 4:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    uVar2 = func_0x8001af20();
    *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar2 & 0xf);
    if (*(int *)(_DAT_800ac784 + 0x38) <= *(short *)(_DAT_800ac784 + 0x1ba) + -0x708) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 5;
    return;
  case 5:
    *(undefined1 *)(_DAT_800ac784 + 7) = 6;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  case 6:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -500) {
      uVar2 = func_0x8001af20();
      *(uint *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 100 + (uVar2 & 0xf);
    }
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x1f') {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 7;
    return;
  case 7:
    *(undefined1 *)(_DAT_800ac784 + 7) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(short *)(_DAT_800ac784 + 0x6c) = *(short *)(_DAT_800ac784 + 0x6c) + -0x400;
    *(undefined **)(_DAT_800ac784 + 0x78) = PTR_DAT_80118e04;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 9) = 5;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
    uVar2 = (*(ushort *)(_DAT_800ac784 + 0x6a) & 0xc00) >> 10;
    if (uVar2 == 1) {
      iVar1 = *(int *)(_DAT_800ac784 + 0x3c) + -0x5d0;
LAB_80115310:
      *(int *)(_DAT_800ac784 + 0x3c) = iVar1;
    }
    else if (uVar2 < 2) {
      if (uVar2 == 0) {
        *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + 0x5d0;
      }
    }
    else if (uVar2 == 2) {
      *(int *)(_DAT_800ac784 + 0x34) = *(int *)(_DAT_800ac784 + 0x34) + -0x5d0;
    }
    else if (uVar2 == 3) {
      iVar1 = *(int *)(_DAT_800ac784 + 0x3c) + 0x5d0;
      goto LAB_80115310;
    }
LAB_80115314:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    *(undefined2 *)(_DAT_800ac784 + 4) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  default:
    break;
  }
  return;
}


