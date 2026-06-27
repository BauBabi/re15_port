/* FUN_8010d388 @ 0x8010d388  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d388(int param_1)

{
  short sVar1;
  int in_v1;
  
  if (in_v1 != 2) {
    *(short *)(param_1 + 0x1ba) = ((short)in_v1 + 2) * -8;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    FUN_8010e510();
    return;
  }
  sVar1 = *(short *)(param_1 + 0x9c);
  *(short *)(param_1 + 0x9c) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


