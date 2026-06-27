/* FUN_80114ba4 @ 0x80114ba4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114ba4(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
      cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
      *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
      if (cVar2 != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 1) = 7;
      FUN_80115d74(0);
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    func_0x800453d0(3);
    *(undefined2 *)((int)_DAT_800ac784 + 0x9a) = 0xffff;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x7a) = 0x26;
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    _DAT_800aca50 = _DAT_800aca50 + 1;
    if (*(char *)((int)_DAT_800ac784 + 0x1db) != '\0') {
      _DAT_800aca50 = _DAT_800aca50 & 0xf0ff | 0x800;
    }
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784 + 8,
                    &DAT_8012110c);
  }
  *(short *)(_DAT_800ac784 + 0x1b) = (short)_DAT_800ac784[0x1b] + 0x8c;
  if (0x400 < (short)_DAT_800ac784[0x1b]) {
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0x400;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x3c;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + (short)_DAT_800ac784[0x7a];
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  uVar3 = (int)*(short *)((int)_DAT_800ac784 + 0x1ba) - 400;
  if ((int)uVar3 <= (int)_DAT_800ac784[0xe]) {
    _DAT_800ac784[0xe] = uVar3;
    *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
    FUN_80115d94(10);
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
    func_0x800453d0(5);
    *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
  }
  return;
}


