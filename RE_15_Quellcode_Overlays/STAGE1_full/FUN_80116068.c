/* FUN_80116068 @ 0x80116068  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116068(void)

{
  ushort uVar1;
  
  uVar1 = _DAT_800aca50 & 0x4000;
  if (((_DAT_800aca50 & 0x8000) != 0) &&
     (uVar1 = _DAT_800aca50 & 0x4000, *(byte *)(_DAT_800ac784 + 9) < 0x80)) {
    uVar1 = _DAT_800aca50 & 0xfff;
    if ((*(char *)(_DAT_800ac784 + 0x1d8) == '\0') &&
       (uVar1 = _DAT_800aca50, 4 < *(byte *)(_DAT_800ac784 + 5))) {
      FUN_80115d74(0xf);
      uVar1 = _DAT_800aca50;
    }
    _DAT_800aca50 = uVar1;
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    uVar1 = _DAT_800aca50 & 0x4000;
  }
  if (uVar1 != 0) {
    uVar1 = _DAT_800aca50 & 0xfff;
    if ((*(char *)(_DAT_800ac784 + 0x1d8) != '\0') ||
       (uVar1 = _DAT_800aca50, 3 < *(byte *)(_DAT_800ac784 + 5))) {
      _DAT_800aca50 = uVar1;
      FUN_80115d74(0xe);
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
  }
  if ((_DAT_800aca50 & 0x2000) != 0) {
    if (*(char *)(_DAT_800ac784 + 0x1d8) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x1000;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 0;
    if ((3 < *(byte *)(_DAT_800ac784 + 5)) && (*(char *)(_DAT_800ac784 + 9) != -0x80)) {
      FUN_80115d74(0x11);
    }
  }
  return;
}


