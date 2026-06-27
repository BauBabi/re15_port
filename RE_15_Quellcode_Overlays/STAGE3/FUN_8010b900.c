/* FUN_8010b900 @ 0x8010b900  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b900(void)

{
  int iVar1;
  undefined1 extraout_v1;
  
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    iVar1 = FUN_80107d9c();
    *(undefined1 *)(iVar1 + 5) = extraout_v1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    return;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 +
              (uint)*(byte *)(_DAT_800ac784 + 5) * 0x20 + -0x7fee1874))();
  return;
}


