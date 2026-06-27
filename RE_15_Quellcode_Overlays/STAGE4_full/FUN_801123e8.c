/* FUN_801123e8 @ 0x801123e8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801123e8(void)

{
  undefined1 uVar1;
  int iVar2;
  
  if ((((DAT_800acae7 == '\0') && (iVar2 = func_0x8001a804(3000,0x100,&DAT_800aca88), iVar2 < 0)) &&
      (*(short *)(_DAT_800ac784 + 0x1de) == 0)) &&
     ((0x50 < _DAT_800acaee && (0x9c4 < *(ushort *)(_DAT_800ac784 + 0x1d4))))) {
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 100;
    uVar1 = 7;
LAB_801124c0:
    *(undefined1 *)(_DAT_800ac784 + 5) = uVar1;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  else {
    if (DAT_800acae7 == '\0') {
      iVar2 = func_0x8001a804(0x708,0x180,&DAT_800aca88);
      if ((iVar2 < 0) && (uVar1 = 4, *(short *)(_DAT_800ac784 + 0x1de) == 0)) goto LAB_801124c0;
      if ((DAT_800acae7 == '\0') &&
         (((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) == 0 &&
          (7000 < *(ushort *)(_DAT_800ac784 + 0x1d4))))) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 2;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 1;
        return;
      }
    }
    if ((((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
        ((*(byte *)(_DAT_800ac784 + 9) & 0x19) == 0)) &&
       ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) +
         0x200 & 0xfff) < 0x400)) {
      *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
      *(undefined2 *)(_DAT_800ac784 + 6) = 0;
    }
  }
  return;
}


