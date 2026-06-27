/* FUN_80111488 @ 0x80111488  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111488(void)

{
  undefined2 uVar1;
  int iVar2;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 3000) && (*(short *)(_DAT_800ac784 + 0x1d6) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = 8;
  }
  else if ((_DAT_800aca58 == 0x701) && (DAT_800aca5a == '\x02')) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = 8;
  }
  else {
    if (*(short *)(_DAT_800ac784 + 0x1d4) < 0x1389) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    *(undefined2 *)(_DAT_800ac784 + 6) = 2;
    if (*(short *)(_DAT_800ac784 + 0x1d4) < 0x1389) {
      return;
    }
    iVar2 = func_0x8001a9cc(&DAT_800aca88,0x80);
    if (iVar2 != 0) {
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    uVar1 = 0xb;
  }
  *(undefined2 *)(_DAT_800ac784 + 6) = uVar1;
  return;
}


