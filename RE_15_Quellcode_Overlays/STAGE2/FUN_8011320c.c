/* FUN_8011320c @ 0x8011320c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011320c(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
  }
  (**(code **)((uint)*(byte *)(_DAT_800ac784 + 6) * 4 + -0x7fee7108))();
  return;
}


