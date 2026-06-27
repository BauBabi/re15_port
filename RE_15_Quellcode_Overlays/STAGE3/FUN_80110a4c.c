/* FUN_80110a4c @ 0x80110a4c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110a4c(void)

{
  int iVar1;
  uint uVar2;
  int in_v1;
  
  if ((*(ushort *)(in_v1 + 0x1d0) & 1) != 0) {
    *(undefined1 *)(in_v1 + 5) = 3;
    FUN_801112a4();
    return;
  }
  if ((*(ushort *)(_DAT_800ac784 + 0x1d4) < 6000) && (_DAT_800aca58 == 0x701)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    iVar1 = _DAT_800ac784;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    if (iVar1 == 0) {
      uRam00000706 = 6;
      return;
    }
    if (((((uRam000008d1 & 1) != 0) && (cRam000008e3 == '\0')) && (sRam000008d7 == 0)) &&
       ((((6000 < uRam000008d5 && (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20), iVar1 == 0)) &&
         ((_DAT_800aca58 == 0x701 || (uVar2 = func_0x8001af20(), (uVar2 & 1) != 0)))) &&
        (iVar1 = func_0x8001a780(&DAT_800aca54), iVar1 == 0)))) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
  if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 3;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  }
  return;
}


