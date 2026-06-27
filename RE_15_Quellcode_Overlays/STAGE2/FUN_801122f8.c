/* FUN_801122f8 @ 0x801122f8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801122f8(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80112380:
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    sVar2 = func_0x8001aa68(((int)*(char *)(_DAT_800ac784 + 0x9f) & 0xf0U) << 4,0x10);
    *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
    if (sVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    FUN_80112e48();
    return;
  }
  if (bVar1 < 2) {
    if (bVar1 != 0) {
      FUN_80112e48();
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    goto LAB_80112380;
  }
  if (bVar1 == 2) {
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xf;
    if (*(short *)(_DAT_800ac784 + 0x1ec) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x9f) & 0xf0) << 4;
  }
  else if (bVar1 != 3) {
    FUN_80112e48();
    return;
  }
  if (*(short *)(_DAT_800ac784 + 0x1ec) == 0) {
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x15') {
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0xa8c;
      *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + 0x708;
    }
    if (*(short *)(_DAT_800ac784 + 0x1ec) == 0) goto LAB_80112500;
  }
  if (*(char *)(_DAT_800ac784 + 0x95) == '\x1a') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x904;
    *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
  }
LAB_80112500:
  iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar3 == 0) {
    return;
  }
  *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + -1;
  *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + 0x708;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 3000;
  if (*(short *)(_DAT_800ac784 + 0x1ec) != 0) {
    *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x02';
    *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0xe10;
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -200;
  }
  *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
  func_0x800245d8(0);
  *(undefined2 *)(_DAT_800ac784 + 0x40) = *(undefined2 *)(_DAT_800ac784 + 0x34);
  *(undefined2 *)(_DAT_800ac784 + 0x44) = *(undefined2 *)(_DAT_800ac784 + 0x3c);
  *(undefined2 *)(_DAT_800ac784 + 0x42) = *(undefined2 *)(_DAT_800ac784 + 0x38);
  *(undefined1 *)(_DAT_800ac784 + 5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}


