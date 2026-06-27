/* FUN_80112558 @ 0x80112558  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112558(void)

{
  char cVar1;
  undefined1 in_v0;
  short sVar2;
  int in_v1;
  
  *(undefined1 *)(in_v1 + 0x9e) = in_v0;
  func_0x8001ad68(_DAT_800ac784,*(undefined4 *)(_DAT_800ac784 + 0x84),
                  *(undefined4 *)(_DAT_800ac784 + 0x16c));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if ((cVar1 != '\0') && (-1 < _DAT_800acaee)) {
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar2 * -100;
    if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 4;
      DAT_800aca5a = 4;
      FUN_80112f84();
      return;
    }
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 5) = 8;
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  FUN_80112f84();
  return;
}


