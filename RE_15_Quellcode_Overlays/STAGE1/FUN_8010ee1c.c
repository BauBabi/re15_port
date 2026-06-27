/* FUN_8010ee1c @ 0x8010ee1c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee1c(void)

{
  char cVar1;
  int in_v0;
  
  *(undefined1 *)(*(int *)(in_v0 + -0x387c) + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
  *(short *)(_DAT_800ac784 + 0x9c) =
       *(short *)(_DAT_800ac784 + 0x1ba) - *(short *)(_DAT_800ac784 + 0x38);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),*(undefined1 *)(_DAT_800ac784 + 7),
                          0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  if (*(byte *)(_DAT_800ac784 + 0x95) < 0x11) {
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + -3;
    func_0x800245d8(0x800);
    *(int *)(_DAT_800ac784 + 0x38) =
         (((int)*(short *)(_DAT_800ac784 + 0x9c) / 0xe) * 0x10000 >> 0x10) +
         *(int *)(_DAT_800ac784 + 0x38);
    if ((int)*(short *)(_DAT_800ac784 + 0x1ba) < *(int *)(_DAT_800ac784 + 0x38)) {
      *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
      FUN_8010f7f0();
      return;
    }
  }
  return;
}


