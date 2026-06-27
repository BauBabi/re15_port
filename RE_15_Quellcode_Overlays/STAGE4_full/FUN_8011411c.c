/* FUN_8011411c @ 0x8011411c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011411c(void)

{
  char cVar1;
  short sVar2;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x18;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    *(short *)(_DAT_800ac784 + 0x28) = (short)_DAT_800ac784[0xd];
    *(short *)((int)_DAT_800ac784 + 0xa2) = (short)_DAT_800ac784[0xf];
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x66c;
    func_0x800245d8(0x800);
    _DAT_800aca88 = _DAT_800ac784[0xd];
    _DAT_800aca90 = _DAT_800ac784[0xf];
    _DAT_800ac784[0xd] = (int)(short)_DAT_800ac784[0x28];
    _DAT_800ac784[0xf] = (int)*(short *)((int)_DAT_800ac784 + 0xa2);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    cVar1 = func_0x8001a780(&DAT_800aca54);
    DAT_800aca59 = cVar1 + '\x02';
    DAT_800aca5a = 0;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    break;
  case 1:
  case 5:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x19;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
  case 3:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (_DAT_800acaee < 0) {
      _DAT_800acaee = 0;
    }
    else {
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -8;
      if (-1 < (short)_DAT_800ac784[0x27]) {
        return;
      }
    }
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
    DAT_800aca5a = 4;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1a;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    break;
  case 6:
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x1e;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 0xd;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
  default:
    goto switchD_80114158_default;
  }
  cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
switchD_80114158_default:
  return;
}


