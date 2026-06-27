/* FUN_8010b568 @ 0x8010b568  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b568(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_FUN_80118368)[*(byte *)(_DAT_800ac784 + 5)])();
  (*(code *)(&PTR_FUN_801183ac)[*(byte *)(_DAT_800ac784 + 5)])();
  return;
}


