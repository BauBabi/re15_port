/* FUN_80113c84 @ 0x80113c84  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113c84(void)

{
  char cVar1;
  uint *puVar2;
  
  if (*(char *)((int)_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    FUN_801161e8();
    func_0x80019700(0x1000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x198,
                    &DAT_8012110c);
    _DAT_800acaee = _DAT_800acaee - 4;
    if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x76) = 1;
    }
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)((int)_DAT_800ac784 + 6) != '\x01') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    puVar2 = _DAT_800ac784;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *puVar2 = *puVar2 | 0x40;
    *_DAT_800ac784 = *_DAT_800ac784 | 8;
    cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
    *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
    if (cVar1 == '\0') {
      *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
      *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    }
    *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
    *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
    func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
    FUN_801157b4();
    return;
  }
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
  return;
}


