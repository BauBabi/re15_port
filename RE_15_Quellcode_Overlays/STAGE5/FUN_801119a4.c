/* FUN_801119a4 @ 0x801119a4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801119a4(void)

{
  int iVar1;
  
  if (((DAT_800acae7 == '\0') && (iVar1 = func_0x8001a804(8000,0xc0,&DAT_800aca88), iVar1 < 0)) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 2;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    FUN_80112294();
    return;
  }
  if ((((*(ushort *)(*(int *)(_DAT_800ac784 + 0x1b4) + 10) & 0xc) != 0) &&
      ((*(byte *)(_DAT_800ac784 + 9) & 0x19) == 0)) &&
     ((((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(_DAT_800ac784 + 0x6a)) +
       0x200 & 0xfff) < 0x400)) {
    *(byte *)(_DAT_800ac784 + 0x9f) = *(byte *)(_DAT_800ac784 + 0x90);
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


