/* FUN_8010bf14 @ 0x8010bf14  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf14(void)

{
  short sVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  undefined8 uVar5;
  
  uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar4 != 1) {
    if (1 < uVar4) {
      iVar3 = 3;
      if (uVar4 == 2) {
        sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
        *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
        if (sVar1 == 0) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 3;
          *(undefined2 *)(_DAT_800ac784 + 0x8c) = 200;
          *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
          *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        }
        uVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                                *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
        iVar3 = (int)uVar5;
        uVar4 = (int)((ulonglong)uVar5 >> 0x20) * 0x20 + 0x800;
LAB_8010c920:
        (**(code **)(iVar3 * 4 + uVar4))();
        if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
          *(undefined1 *)(_DAT_800ac784 + 4) = 1;
          *(undefined1 *)(_DAT_800ac784 + 5) = 4;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        }
        return;
      }
      if (uVar4 != 3) goto LAB_8010c920;
      sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
      *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
      if (sVar1 == 0) {
        bVar2 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 5) = (bVar2 & 1) + 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      goto LAB_8010c0f8;
    }
    iVar3 = 1;
    if (uVar4 != 0) goto LAB_8010c920;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 800;
  }
  sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    FUN_8010c8f8();
    return;
  }
LAB_8010c0f8:
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x800245d8(0);
  return;
}


