/* FUN_8010c714 @ 0x8010c714  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c714(void)

{
  short sVar1;
  byte bVar2;
  undefined4 uVar3;
  
  bVar2 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar2 == 1) {
LAB_8010c7bc:
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 2;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    }
  }
  else {
    if (bVar2 < 2) {
      if (bVar2 != 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 6) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 800;
      goto LAB_8010c7bc;
    }
    if (bVar2 == 2) {
      sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
      if (sVar1 == 0) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      uVar3 = 0x800;
      goto LAB_8010c918;
    }
    if (bVar2 != 3) {
      return;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 5) = (bVar2 & 1) + 2;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  uVar3 = 0;
LAB_8010c918:
  func_0x800245d8(uVar3);
  return;
}


