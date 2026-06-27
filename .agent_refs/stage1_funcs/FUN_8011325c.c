/* FUN_8011325c @ 0x8011325c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011325c(void)

{
  int iVar1;
  undefined4 uVar2;
  
  if (*(char *)(_DAT_800ac784 + 0x1d3) == '\0') goto LAB_80113300;
  *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
  if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0xf) == 0) {
    iVar1 = _DAT_800ac784;
    if (((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) == 0) &&
       (iVar1 = 6, *(char *)(_DAT_800ac784 + 5) != '\x06')) goto LAB_801132d0;
    FUN_80115f00(iVar1);
  }
  else {
    iVar1 = 9;
LAB_801132d0:
    FUN_80115d74(iVar1);
  }
  if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
    FUN_80115f00();
  }
LAB_80113300:
  if ((((*(char *)(_DAT_800ac784 + 0x1d2) != '\0') && (*(char *)(_DAT_800ac784 + 0x1db) != '\0')) &&
      (*(ushort *)(_DAT_800ac784 + 0x1dc) < 10000)) && (*(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    uVar2 = 10;
    if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
      uVar2 = 0xc;
    }
    FUN_80115d74(uVar2);
  }
  return;
}


