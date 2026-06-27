/* FUN_8010b814 @ 0x8010b814  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b814(void)

{
  int iVar1;
  undefined8 uVar2;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    iVar1 = FUN_80107cb0();
    *(undefined1 *)(*(int *)(iVar1 + -0x387c) + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    uVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    (**(code **)((int)uVar2 * 4 + (int)((ulonglong)uVar2 >> 0x20) * 0x20 + 0x800))();
    if (*(char *)(_DAT_800ac784 + 0x1d2) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 4) = 1;
      *(undefined1 *)(_DAT_800ac784 + 5) = 4;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    }
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fedf9c4))();
  return;
}


