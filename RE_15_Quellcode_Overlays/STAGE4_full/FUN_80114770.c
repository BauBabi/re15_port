/* FUN_80114770 @ 0x80114770  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114770(void)

{
  if ((((*(short *)(_DAT_800ac784 + 0x1dc) == 0) ||
       ((&DAT_8011a1a8)[*(byte *)(_DAT_800ac784 + 5)] == '\0')) ||
      (*(char *)(_DAT_800ac784 + 6) != '\x01')) || ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) != 0))
  {
    (*(code *)(&PTR_FUN_8011a1c0)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  else {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x601;
    func_0x800453d0(0xb);
    *(undefined2 *)(_DAT_800ac784 + 0x9a) = *(undefined2 *)(_DAT_800ac784 + 0x1dc);
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  }
  return;
}


