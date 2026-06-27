/* FUN_8010d2c0 @ 0x8010d2c0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d2c0(void)

{
  char cVar1;
  int in_v1;
  
  _DAT_800acaee = _DAT_800acaee + -10;
  *(undefined2 *)(in_v1 + 0x1dc) = 0x2d;
  func_0x800453d0();
  DAT_800aca58 = 2;
  cVar1 = func_0x8001a780(&DAT_800aca54);
  DAT_800aca59 = cVar1 + '\x04';
  DAT_800aca5a = 0;
  if (_DAT_800acaee < 0) {
    DAT_800aca58 = 3;
    DAT_800aca59 = '\0';
  }
  DAT_800acae7 = DAT_800acae7 | 1;
  *(byte *)(_DAT_800ac784 + 5) = DAT_800acae7;
  *(undefined1 *)(_DAT_800ac784 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x1d;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
  return;
}


