/* FUN_801147b8 @ 0x801147b8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801147b8(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_801150f8();
        return;
      }
      goto LAB_8011486c;
    }
    if (bVar1 != 0) {
      FUN_801150f8();
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xff88;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_8011486c:
  if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x8001a8f8(&DAT_800aca88,0x14);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (2000 < *(short *)(_DAT_800ac784 + 0x1ec)) {
    FUN_80115d74(1);
  }
  return;
}


