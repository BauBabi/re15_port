/* FUN_8010e740 @ 0x8010e740  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010e740(void)

{
  char cVar1;
  short sVar2;
  undefined1 uVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    func_0x800453d0(3);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x28) = _DAT_800aca88;
    *(undefined2 *)((int)_DAT_800ac784 + 0xa2) = _DAT_800aca90;
    _DAT_800acaf4 = _DAT_800aca88;
    _DAT_800acaf6 = _DAT_800aca90;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    DAT_800aca59 = 1;
    DAT_800aca5a = 0;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x18;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if ((cVar1 == '\0') || (_DAT_800acaee < 0)) {
      uVar3 = 0xb;
LAB_8010eb68:
      *(undefined1 *)((int)_DAT_800ac784 + 5) = uVar3;
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    }
    else {
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
      if ((short)_DAT_800ac784[0x27] < 0) {
        *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
        DAT_800aca5a = 4;
      }
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x19;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    cVar1 = func_0x8001af20();
    *(byte *)((int)_DAT_800ac784 + 0x9e) = cVar1 + 0x1eU & 0x3f;
  case 5:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    if (*(char *)((int)_DAT_800ac784 + 6) == '\x06') {
      func_0x800453d0(7);
    }
    break;
  case 6:
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    uVar3 = 0xc;
    goto LAB_8010eb68;
  default:
    break;
  }
  return;
}


