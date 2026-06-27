/* FUN_80117c90 @ 0x80117c90  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80117c90(void)

{
  char cVar1;
  byte bVar2;
  char cVar3;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x18);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = '\x18';
  }
  else {
    FUN_8011bf50(0,*(byte *)(_DAT_800ac784 + 0x95) < 0x29);
    cVar1 = *(char *)(_DAT_800ac784 + 0x95);
    cVar3 = ')';
  }
  if ((cVar1 == '\x01') || (cVar1 == cVar3)) {
    func_0x800453d0(8);
  }
  return;
}


