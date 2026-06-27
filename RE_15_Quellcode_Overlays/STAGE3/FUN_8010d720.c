/* FUN_8010d720 @ 0x8010d720  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d720(void)

{
  char cVar1;
  short in_v0;
  int in_v1;
  byte *unaff_s0;
  
  _DAT_800acaee = in_v0 - 10;
  if ((*(short *)(*(int *)(in_v1 + -0x387c) + 0x1e4) == 0) &&
     (-1 < (int)((uint)_DAT_800acaee << 0x10))) {
    func_0x800453d0(3);
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    DAT_800aca58 = 2;
    cVar1 = func_0x8001a780(unaff_s0 + -0x93);
    DAT_800aca59 = cVar1 + '\x02';
    DAT_800aca5a = 0;
    *unaff_s0 = *unaff_s0 | 1;
    FUN_8010e078();
    return;
  }
  cVar1 = func_0x8001a780(unaff_s0 + -0x93);
  *(char *)(_DAT_800ac784 + 5) = cVar1 + '\t';
  *(undefined1 *)(_DAT_800ac784 + 6) = 0;
  *(int *)(_DAT_800ac784 + 0x38) = (int)*(short *)(_DAT_800ac784 + 0x1ba);
  *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
  FUN_8010e2d0();
  return;
}


