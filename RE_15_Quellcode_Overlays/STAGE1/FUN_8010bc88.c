/* FUN_8010bc88 @ 0x8010bc88  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bc88(void)

{
  *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  (**(code **)(&DAT_8012095c + (*(byte *)((int)_DAT_800ac784 + 9) & 0xf) * 4))();
  return;
}


