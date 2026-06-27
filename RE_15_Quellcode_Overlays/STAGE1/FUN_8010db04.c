/* FUN_8010db04 @ 0x8010db04  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010db04(void)

{
  undefined2 uVar1;
  int iVar2;
  ushort uVar3;
  short sVar4;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar3 & 7) + 10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar2 = _DAT_800ac784;
  if (*(short *)(_DAT_800ac784 + 0x1dc) < 0x1f) {
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1e2) = (uVar3 & 0xf) + 1;
    func_0x8001a8f8(&DAT_800aca88,(int)*(short *)(_DAT_800ac784 + 0x1e2));
  }
  else {
    if (*(short *)(_DAT_800ac784 + 0x9c) == 0) {
      uVar1 = *(undefined2 *)(_DAT_800ac784 + 0x6a);
      func_0x8001af20();
      *(char *)(iVar2 + 0x9f) = (char)uVar1;
    }
    sVar4 = func_0x8001aa68((int)*(char *)(_DAT_800ac784 + 0x9f),0x20);
    *(short *)(_DAT_800ac784 + 0x6a) = *(short *)(_DAT_800ac784 + 0x6a) + sVar4;
    sVar4 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar4 + 1;
    if (0x5a < sVar4) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      return;
    }
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(short *)(_DAT_800ac784 + 0x1e6) != 0) {
    *(short *)(_DAT_800ac784 + 0x1e6) = *(short *)(_DAT_800ac784 + 0x1e6) + -1;
  }
  if ((*(char *)(_DAT_800ac784 + 0x95) == '\0') || (*(char *)(_DAT_800ac784 + 0x95) == '\x1e')) {
    func_0x800453d0(6);
  }
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x1e) {
    FUN_8010e540(0,1);
    return;
  }
  FUN_80111870(0,0);
  func_0x800245d8(8);
  return;
}


