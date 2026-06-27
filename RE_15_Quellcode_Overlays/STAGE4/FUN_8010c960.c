/* FUN_8010c960 @ 0x8010c960  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c960(void)

{
  int iVar1;
  uint uVar2;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(4000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    return;
  }
  if ((((((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0) &&
        (*(char *)(_DAT_800ac784 + 0x1e2) == '\0')) &&
       ((*(short *)(_DAT_800ac784 + 0x1d6) == 0 &&
        ((6000 < *(ushort *)(_DAT_800ac784 + 0x1d4) &&
         (iVar1 = func_0x8001a9cc(&DAT_800aca88,0x20), iVar1 == 0)))))) &&
      ((_DAT_800aca58 == 0x701 || (uVar2 = func_0x8001af20(), (uVar2 & 1) != 0)))) &&
     (iVar1 = func_0x8001a780(&DAT_800aca54), iVar1 == 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 7;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


