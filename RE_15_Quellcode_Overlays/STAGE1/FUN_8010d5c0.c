/* FUN_8010d5c0 @ 0x8010d5c0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d5c0(void)

{
  int in_v1;
  
  if ((*(short *)(in_v1 + 0x1d4) < 4000) && ((*(ushort *)(in_v1 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(in_v1 + 5) = 1;
  }
  else if ((*(short *)(_DAT_800ac784 + 0x1d4) < 5000) && (_DAT_800aca58 == 0x701)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    in_v1 = 0x701;
  }
  else {
    if ((5999 < *(short *)(_DAT_800ac784 + 0x1d4)) || (_DAT_800aca58 != 0x201)) {
      if ((_DAT_800aca50 & 1) != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 1;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      }
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 5) = 1;
    in_v1 = 0x201;
  }
  *(ushort *)(in_v1 + 0x1e8) = *(byte *)(in_v1 + 0x90) & 1;
  *(char *)(_DAT_800ac784 + 5) = '\x0e' - *(char *)(_DAT_800ac784 + 0x1e8);
  *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  return;
}


