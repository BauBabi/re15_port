/* FUN_80113f40 @ 0x80113f40  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113f40(void)

{
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
    *(ushort *)(_DAT_800ac784 + 0x1ba) = (ushort)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    func_0x8004ef90(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  (*(code *)(&PTR_FUN_80118f38)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


