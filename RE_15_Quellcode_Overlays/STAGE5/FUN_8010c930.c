/* FUN_8010c930 @ 0x8010c930  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c930(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  ushort unaff_s3;
  short unaff_s4;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    if (*(char *)(_DAT_800ac784 + 7) == '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 5;
      bVar2 = func_0x8001af20();
      *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 2;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  bVar2 = func_0x8001af20();
  *(byte *)(_DAT_800ac784 + 0x9e) = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x18) {
      FUN_80111388(0,1);
      if (*(char *)(_DAT_800ac784 + 0x95) != '\x01') {
        FUN_8010d2c0();
        return;
      }
LAB_8010cac8:
      func_0x800453d0(8);
      return;
    }
    do {
      if (*(char *)(iRamffffc784 + 0x95) == (&LAB_8011fae4)[unaff_s3]) {
        unaff_s4 = 1;
      }
      bVar1 = unaff_s3 != 0;
      unaff_s3 = unaff_s3 - 1;
    } while (bVar1);
    if (unaff_s4 == 0) {
      return;
    }
    iVar4 = -0x7ff50000;
  }
  else {
    iVar4 = _DAT_800ac784;
    if (*(byte *)(_DAT_800ac784 + 0x95) < 0x29) {
      FUN_80111388(0,1);
      if ((*(char *)(_DAT_800ac784 + 0x95) != '\x01') && (*(char *)(_DAT_800ac784 + 0x95) != ')')) {
        return;
      }
      goto LAB_8010cac8;
    }
  }
  iVar4 = *(int *)(iVar4 + -0x387c);
  if ((*(short *)(iVar4 + 0x1d8) == 0) && (*(short *)(iVar4 + 0x1da) == 0)) {
    return;
  }
  _DAT_800acaee = _DAT_800acaee + -10;
  *(undefined2 *)(iVar4 + 0x1dc) = 0x2d;
  func_0x800453d0(4,0);
  DAT_800aca58 = 2;
  cVar3 = func_0x8001a780(&DAT_800aca54);
  DAT_800aca59 = cVar3 + '\x04';
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


