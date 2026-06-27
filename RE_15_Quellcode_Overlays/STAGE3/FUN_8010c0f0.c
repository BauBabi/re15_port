/* FUN_8010c0f0 @ 0x8010c0f0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c0f0(int param_1)

{
  ushort uVar1;
  
  uVar1 = _DAT_800aca58;
  if (_DAT_800aca58 == 0x701) {
    *(undefined1 *)(param_1 + 5) = 1;
  }
  else {
    if ((5999 < *(short *)(param_1 + 0x1d4)) || (_DAT_800aca58 != 0x201)) {
      if ((_DAT_800aca50 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      return;
    }
    *(undefined1 *)(param_1 + 5) = 1;
  }
  *(ushort *)(uVar1 + 0x1e8) = *(byte *)(uVar1 + 0x90) & 1;
  *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}


