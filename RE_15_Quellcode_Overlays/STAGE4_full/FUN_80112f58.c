/* FUN_80112f58 @ 0x80112f58  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112f58(void)

{
  char cVar1;
  short sVar2;
  
  switch(*(undefined1 *)((int)_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x12;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
    _DAT_800acbfc = _DAT_800ac784;
    func_0x8001ac38(&DAT_800aca54);
    *_DAT_800ac784 = *_DAT_800ac784 | 0x1000;
    DAT_800aca58 = 5;
    DAT_800aca59 = 1;
    DAT_800aca5a = 0;
    _DAT_800aca54 = _DAT_800aca54 | 0x1000;
    _DAT_800acbcc = _DAT_800ac784[0x5e];
    _DAT_800acbd0 = _DAT_800ac784[0x5f];
    DAT_800acae7 = DAT_800acae7 | 1;
    func_0x8001a8f8(&DAT_800aca88,0x800);
    break;
  case 1:
    break;
  case 2:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x13;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 3;
    *(undefined2 *)(_DAT_800ac784 + 0x27) = 100;
    *(undefined1 *)((int)_DAT_800ac784 + 0x9e) = 0x32;
  case 3:
    func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    sVar2 = func_0x80037024();
    *(short *)(_DAT_800ac784 + 0x27) = (short)_DAT_800ac784[0x27] + -1 + sVar2 * -100;
    if ((short)_DAT_800ac784[0x27] < 0) {
      *(undefined1 *)((int)_DAT_800ac784 + 6) = 4;
      DAT_800aca5a = 4;
    }
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x9e);
    *(char *)((int)_DAT_800ac784 + 0x9e) = cVar1 + -1;
    if (cVar1 != '\0') {
      return;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 10;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    return;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x14;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 7;
  case 5:
    if (*(char *)((int)_DAT_800ac784 + 0x95) == '\x18') {
      func_0x800453d0(0);
    }
    break;
  case 6:
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 0x3c;
    *(undefined1 *)((int)_DAT_800ac784 + 5) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 6) = 0;
    *_DAT_800ac784 = *_DAT_800ac784 & 0xffffefff;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
  default:
    goto switchD_80112f98_default;
  }
  func_0x8001ad68(_DAT_800ac784,_DAT_800ac784[0x21],_DAT_800ac784[0x5b]);
  cVar1 = func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + cVar1;
switchD_80112f98_default:
  return;
}


