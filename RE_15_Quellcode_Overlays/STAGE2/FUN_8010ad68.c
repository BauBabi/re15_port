/* FUN_8010ad68 @ 0x8010ad68  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ad68(void)

{
  if ((*(ushort *)(_DAT_800ac784 + 0x1c0) & 0x9fff) == 0x8001) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 9;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  (*(code *)(&PTR_LAB_80118368)[*(byte *)(_DAT_800ac784 + 5)])();
  (**(code **)(&LAB_801183ac + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


