/* FUN_8010ae88 @ 0x8010ae88  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ae88(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (**(code **)(&LAB_8011978c + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  (**(code **)(&LAB_801197d0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


