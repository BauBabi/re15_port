/* FUN_80114594 @ 0x80114594  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114594(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(3);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 100;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001a8f8(&DAT_800aca88,100);
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) + 0x30;
  if (*(int *)(_DAT_800ac784 + 0x38) < *(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee) {
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  }
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if ((*(ushort *)(_DAT_800ac784 + 0x1dc) < 1000) &&
     (*(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee < *(int *)(_DAT_800ac784 + 0x38))) {
    FUN_80115d74(8);
  }
  return;
}


