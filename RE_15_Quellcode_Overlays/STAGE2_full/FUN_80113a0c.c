/* FUN_80113a0c @ 0x80113a0c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113a0c(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
  }
  (*(code *)(&PTR_FUN_80118ef8)[*(byte *)(_DAT_800ac784 + 6)])();
  return;
}


