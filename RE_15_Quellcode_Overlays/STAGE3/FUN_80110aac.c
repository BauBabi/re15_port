/* FUN_80110aac @ 0x80110aac  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110aac(void)

{
  int in_v0;
  int iVar1;
  uint uVar2;
  int in_v1;
  
  *(undefined1 *)(in_v0 + 6) = 0;
  iVar1 = _DAT_800ac784;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  if (iVar1 == 0) {
    *(undefined1 *)(in_v1 + 5) = 6;
    return;
  }
  if (((((*(ushort *)(in_v1 + 0x1d0) & 1) != 0) && (*(char *)(in_v1 + 0x1e2) == '\0')) &&
      (*(short *)(in_v1 + 0x1d6) == 0)) &&
     ((((6000 < *(ushort *)(in_v1 + 0x1d4) &&
        (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20), iVar1 == 0)) &&
       ((_DAT_800aca58 == 0x701 || (uVar2 = func_0x8001af20(), (uVar2 & 1) != 0)))) &&
      (iVar1 = func_0x8001a780(&DAT_800aca54), iVar1 == 0)))) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


