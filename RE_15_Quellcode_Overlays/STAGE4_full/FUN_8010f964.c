/* FUN_8010f964 @ 0x8010f964  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010f964(void)

{
  char cVar1;
  short sVar2;
  undefined4 uVar3;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) + 400;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1c;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    _DAT_800acbfc = _DAT_800ac784;
    *(undefined2 *)(_DAT_800ac784 + 0x28) = _DAT_800aca88;
    *(undefined2 *)((int)_DAT_800ac784 + 0xa2) = _DAT_800aca90;
    _DAT_800acaf4 = _DAT_800aca88;
    _DAT_800acaf6 = _DAT_800aca90;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    DAT_800acae7 = DAT_800acae7 | 1;
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    _DAT_800aca58 = 5;
    uVar3 = 6;
    *(undefined2 *)((int)_DAT_800ac784 + 0x6a) = _DAT_800acabe;
    goto LAB_8010fc44;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\0') {
      func_0x800453d0(10);
    }
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      _DAT_800aca58 = CONCAT12(4,_DAT_800aca58);
    }
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1e;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  case 5:
    uVar3 = 5;
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x1d') {
LAB_8010fc44:
      func_0x800453d0(uVar3);
    }
  case 1:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
    break;
  case 6:
    _DAT_800ac784[0xe] = (int)*(short *)((int)_DAT_800ac784 + 0x1ba);
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 7;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    func_0x800453d0(3);
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    *(undefined1 *)(_DAT_800ac784 + 0x78) = 0;
    _DAT_800ac784[0x1e] = (uint)(&PTR_DAT_80119e74)[(byte)_DAT_800ac784[0x79]];
    *(undefined1 *)((int)_DAT_800ac784 + 0x93) = 0;
  case 7:
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '1') {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 8;
    }
    break;
  case 8:
    *(undefined2 *)(_DAT_800ac784 + 0x77) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 2;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)((int)_DAT_800ac784 + 7) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0x1d;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
  }
  return;
}


