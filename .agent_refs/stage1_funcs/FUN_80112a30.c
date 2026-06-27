/* FUN_80112a30 @ 0x80112a30  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a30(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      goto LAB_80112af4;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 1;
    FUN_80115d94(6);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0xffb0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x3c;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
LAB_80112af4:
  if (*(char *)(_DAT_800ac784 + 0x95) == '\b') {
    func_0x800453d0(1);
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 6;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  FUN_80115e24();
  func_0x800245d8(0);
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar2 != 0) {
    FUN_80115d74(5);
  }
  return;
}


