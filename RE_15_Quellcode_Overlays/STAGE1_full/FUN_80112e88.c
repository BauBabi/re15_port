/* FUN_80112e88 @ 0x80112e88  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112e88(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 3;
        *(int *)(_DAT_800ac784 + 0x38) =
             (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
        if (*(short *)(_DAT_800ac784 + 0x1ba) + -0x2ee < *(int *)(_DAT_800ac784 + 0x38)) {
          *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
          *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0xc;
        }
        FUN_80115e24();
      }
      else {
        if (bVar1 != 3) {
          return;
        }
        *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 9;
        *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + -9;
        *(int *)(_DAT_800ac784 + 0x38) =
             (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
        cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
        *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          FUN_80115d74(8);
        }
      }
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x8c;
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x15;
    FUN_80115d94(5);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  cVar2 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 3;
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 8;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x800245d8(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


