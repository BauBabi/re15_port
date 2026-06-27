/* FUN_8010d048 @ 0x8010d048  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d048(void)

{
  int iVar1;
  uint uVar2;
  
  if ((*(short *)(_DAT_800ac784 + 0x1d6) == 0) &&
     (iVar1 = func_0x8001a804(3000,0x180,&DAT_800aca88), iVar1 < 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 8;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    if ((DAT_800acae7 != '\0') && (*(undefined1 *)(_DAT_800ac784 + 5) = 6, _DAT_800acaee < 0x32)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      uVar2 = func_0x8001af20();
      if ((uVar2 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      }
    }
  }
  else if (((*(byte *)(_DAT_800ac784 + 0x90) & 3) != 0) &&
          ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)
            ) + 0x200 & 0xfff) < 0x400)) {
    *(undefined2 *)(_DAT_800ac784 + 0x1ea) = *(undefined2 *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10);
    *(ushort *)(_DAT_800ac784 + 0x1e8) = *(byte *)(_DAT_800ac784 + 0x90) & 1;
    *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


