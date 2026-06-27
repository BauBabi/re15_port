/* FUN_801055d0 @ 0x801055d0  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801055d0(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 0x90);
  if (((bVar1 & 3) == 0) ||
     (0x3ff < (((bVar1 & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) + 0x200 & 0xfff))) {
    if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) &&
       (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x2c8), iVar2 != 0)) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
    }
    if ((((DAT_800acae7 == '\0') && (*(uint *)(_DAT_800ac784 + 0x1d0) < 0x4b0)) &&
        (iVar2 = func_0x8001a9cc(&DAT_800aca88,0x200), iVar2 == 0)) &&
       (DAT_800acad6 == *(char *)(_DAT_800ac784 + 0x82))) {
      iVar2 = func_0x8001a780(&DAT_800aca54);
      *(uint *)(_DAT_800ac784 + 4) = (iVar2 + 3) * 0x100 | 1;
    }
    if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 0x5dc) && (_DAT_800acaee < 0)) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0xc01;
    }
    if ((*(ushort *)(_DAT_800ac784 + 0x1c4) & 0x1000) != 0) {
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x1001;
    }
  }
  else {
    *(uint *)(_DAT_800ac784 + 4) = ((bVar1 & 1) + 9) * 0x100 | 1;
  }
  return;
}


