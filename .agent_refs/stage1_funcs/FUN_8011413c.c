/* FUN_8011413c @ 0x8011413c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011413c(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
      *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 == '\0') {
        FUN_80115d74(9);
      }
      goto LAB_801142a0;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(0);
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x20;
    *(undefined2 *)(_DAT_800ac784 + 0x1de) = 0x3c;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) != 0) {
      *(short *)(_DAT_800ac784 + 0x1de) = -*(short *)(_DAT_800ac784 + 0x1de);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x6a) =
       *(short *)(_DAT_800ac784 + 0x6a) + *(short *)(_DAT_800ac784 + 0x1de);
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x5a;
  }
LAB_801142a0:
  cVar2 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar2;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


