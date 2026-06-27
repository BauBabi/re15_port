/* FUN_80111398 @ 0x80111398  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111398(void)

{
  char cVar1;
  int iVar2;
  
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    if (*(char *)(_DAT_800ac784 + 9) != 'C') {
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xf0;
    func_0x800453d0(5);
  case 1:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),
                            *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    if (0xc < *(byte *)(_DAT_800ac784 + 0x95)) {
      *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 6;
      func_0x800245d8(0);
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x14;
      *(undefined2 *)(_DAT_800ac784 + 0x1ba) = 0;
    }
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x15;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  case 3:
    if (*(byte *)(_DAT_800ac784 + 0x95) < 2) {
      func_0x800245d8(0);
    }
    else {
      iVar2 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffe2,
                              (int)*(short *)(_DAT_800ac784 + 0x1ba));
      if (iVar2 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 4;
      }
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),
                    *(undefined1 *)(_DAT_800ac784 + 7),0x200);
    if (*(char *)(_DAT_800ac784 + 0x95) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 5;
    }
    break;
  case 4:
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0x78;
    *(undefined1 *)(_DAT_800ac784 + 0x82) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  }
  return;
}


