/* FUN_8010b6d4 @ 0x8010b6d4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b6d4(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_FUN_80120264)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801202a8)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


