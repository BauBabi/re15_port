/* FUN_8010c8ac @ 0x8010c8ac  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c8ac(void)

{
  bool bVar1;
  char cVar2;
  int in_v1;
  ushort unaff_s3;
  short unaff_s4;
  
  if (*(byte *)(in_v1 + 0x95) < 0x18) {
    FUN_80111208(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
      FUN_8010d140();
      return;
    }
    func_0x800453d0(8);
    return;
  }
  do {
    if (*(char *)(iRamffffc784 + 0x95) == *(char *)(unaff_s3 + 0x80119f7c)) {
      unaff_s4 = 1;
    }
    bVar1 = unaff_s3 != 0;
    unaff_s3 = unaff_s3 - 1;
  } while (bVar1);
  if ((unaff_s4 != 0) &&
     ((*(short *)(_DAT_800ac784 + 0x1d8) != 0 || (*(short *)(_DAT_800ac784 + 0x1da) != 0)))) {
    _DAT_800acaee = _DAT_800acaee + -10;
    *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
    func_0x800453d0(4,0);
    DAT_800aca58 = 2;
    cVar2 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar2 + '\x04';
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
  return;
}


