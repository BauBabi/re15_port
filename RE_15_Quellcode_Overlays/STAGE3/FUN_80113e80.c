/* FUN_80113e80 @ 0x80113e80  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113e80(void)

{
  *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)(&LAB_8011ec44 + (uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4);
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 0x95) == '1') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 8;
    FUN_80114780();
    return;
  }
  return;
}


