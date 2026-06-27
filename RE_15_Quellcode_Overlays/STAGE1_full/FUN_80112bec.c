/* FUN_80112bec @ 0x80112bec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112bec(void)

{
  byte bVar1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      goto LAB_80112ca0;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xff88;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xa0;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_80112ca0:
  if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  FUN_80115e24();
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


