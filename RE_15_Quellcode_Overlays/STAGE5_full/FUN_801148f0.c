/* FUN_801148f0 @ 0x801148f0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801148f0(void)

{
  if ((((*(short *)(_DAT_800ac784 + 0x1dc) == 0) ||
       ((&DAT_8011fd10)[*(byte *)(_DAT_800ac784 + 5)] == '\0')) ||
      (*(char *)(_DAT_800ac784 + 6) != '\x01')) || ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0))
  {
    (*(code *)(&PTR_FUN_8011fd28)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  else {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
    func_0x800453d0(0xb);
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = *(undefined2 *)(_DAT_800ac784 + 0x1dc);
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}


