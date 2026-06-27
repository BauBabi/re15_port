/* FUN_80118524 @ 0x80118524  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118524(void)

{
  int iVar1;
  
  if (*(short *)(_DAT_800ac784 + 0x1dc) == 0) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 8;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    iVar1 = func_0x800453d0(7);
    *(undefined2 *)(*(int *)(iVar1 + -0x387c) + 0x9c) = 0;
    iVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
    if (iVar1 != 0) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
    }
    return;
  }
  return;
}


