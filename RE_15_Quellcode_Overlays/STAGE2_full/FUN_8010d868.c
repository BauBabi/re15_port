/* FUN_8010d868 @ 0x8010d868  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d868(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 2;
      *(undefined1 *)(_DAT_800ac784 + 0x1e1) = 100;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x14;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 3;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(byte *)(_DAT_800ac784 + 7) & 1,
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar2;
  if ((3 < *(byte *)(_DAT_800ac784 + 0x95)) &&
     (iVar3 = func_0x8001c1a4((int)*(short *)(_DAT_800ac784 + 0x8c),0,0xffffffba,
                              (int)*(short *)(_DAT_800ac784 + 0x1ba)), iVar3 != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  return;
}


