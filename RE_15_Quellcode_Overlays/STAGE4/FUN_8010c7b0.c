/* FUN_8010c7b0 @ 0x8010c7b0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c7b0(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  int iVar4;
  ushort unaff_s3;
  short unaff_s4;
  undefined1 *puVar5;
  
  if (_DAT_800ac784[6] == '\0') {
    _DAT_800ac784[6] = 1;
    _DAT_800ac784[0x94] = 4;
    if (_DAT_800ac784[7] == '\0') {
      _DAT_800ac784[0x94] = 5;
      bVar2 = func_0x8001af20();
      _DAT_800ac784[0x9e] = (bVar2 & 0x1f) + 2;
    }
    _DAT_800ac784[0x95] = 0;
    _DAT_800ac784[0x8f] = 7;
  }
  bVar2 = func_0x8001af20();
  _DAT_800ac784[0x9e] = (bVar2 & 0x1f) + 6;
  func_0x8001a8f8(&DAT_800aca88,(int)(char)_DAT_800ac784[0x9e]);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (_DAT_800ac784[7] == '\0') {
    if ((byte)_DAT_800ac784[0x95] < 0x18) {
      FUN_80111208(0,1);
      if (_DAT_800ac784[0x95] != '\x01') {
        FUN_8010d140();
        return;
      }
LAB_8010c948:
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
    if (unaff_s4 == 0) {
      return;
    }
    puVar5 = &DAT_800b0000;
  }
  else {
    puVar5 = _DAT_800ac784;
    if ((byte)_DAT_800ac784[0x95] < 0x29) {
      FUN_80111208(0,1);
      if ((_DAT_800ac784[0x95] != '\x01') && (_DAT_800ac784[0x95] != ')')) {
        return;
      }
      goto LAB_8010c948;
    }
  }
  iVar4 = *(int *)(puVar5 + -0x387c);
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
  _DAT_800ac784[5] = DAT_800acae7;
  _DAT_800ac784[6] = 1;
  _DAT_800ac784[7] = 0;
  _DAT_800ac784[0x95] = 0x1d;
  _DAT_800ac784[0x8f] = 7;
  _DAT_800ac784[0x94] = 3;
  return;
}


