/* FUN_80110c88 @ 0x80110c88  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110c88(int param_1)

{
  int iVar1;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d4) < 3000) && (*(short *)(_DAT_800ac784 + 0x1d6) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
  }
  else {
    if ((_DAT_800aca58 != 0x701) || (DAT_800aca5a != '\x02')) {
      if (5000 < *(short *)(_DAT_800ac784 + 0x1d4)) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined2 *)(_DAT_800ac784 + 6) = 2;
        if ((5000 < *(short *)(_DAT_800ac784 + 0x1d4)) &&
           (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x80), iVar1 == 0)) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 1;
          *(undefined2 *)(_DAT_800ac784 + 6) = 0xb;
        }
      }
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
  }
  _DAT_000000a4 = (short)param_1 + -1;
  if (param_1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 0;
    *(char *)(_DAT_800ac784 + 6) = (char)((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) << 2);
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


