/* FUN_8011431c @ 0x8011431c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011431c(void)

{
  int iVar1;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0xb4;
    *(char *)(_DAT_800ac784 + 0x1d6) = *(char *)(_DAT_800ac784 + 0x1d6) + '\x01';
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    if (*(short *)(_DAT_800ac784 + 0x1ec) < 2000) {
      *(short *)(_DAT_800ac784 + 0x1e4) = -*(short *)(_DAT_800ac784 + 0x1e4);
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + -1;
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    func_0x8001a8f8(&DAT_800aca88,0x32);
    if (*(char *)(_DAT_800ac784 + 0x1d0) != '\0') {
      FUN_80115d74(0x10);
    }
    func_0x800245d8(0);
  }
  return;
}


