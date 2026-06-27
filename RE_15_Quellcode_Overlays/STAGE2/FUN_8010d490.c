/* FUN_8010d490 @ 0x8010d490  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d490(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 6;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1e2) = 0x78;
        return;
      }
      goto LAB_8010dd80;
    }
    if (uVar2 != 0) goto LAB_8010dd80;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x14;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  uVar2 = (uint)*(byte *)(_DAT_800ac784 + 6) + iVar1;
  *(char *)(_DAT_800ac784 + 6) = (char)uVar2;
LAB_8010dd80:
  if (*(char *)(uVar2 + 0x1e0) == '\0') {
    (**(code **)((uint)*(byte *)(uVar2 + 5) * 4 + -0x7fee7394))();
    return;
  }
  FUN_8010e91c();
  FUN_8010e5c8();
  return;
}


