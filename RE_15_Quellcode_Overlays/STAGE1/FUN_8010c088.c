/* FUN_8010c088 @ 0x8010c088  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c088(void)

{
  int in_v1;
  undefined8 uVar1;
  
  *(undefined1 *)(*(int *)(in_v1 + -0x387c) + 0x8f) = 7;
  uVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  (**(code **)((int)uVar1 * 4 + (int)((ulonglong)uVar1 >> 0x20) * 0x20 + 0x800))();
  if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
    *(undefined1 *)(_DAT_800ac784 + 4) = 1;
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


