/* FUN_801143a4 @ 0x801143a4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801143a4(void)

{
  char cVar1;
  uint *puVar2;
  uint uVar3;
  
  puVar2 = (uint *)(uint)*(byte *)((int)_DAT_800ac784 + 7);
  if (puVar2 != (uint *)0x1) {
    if ((uint *)0x1 < puVar2) {
      if (puVar2 == (uint *)0x2) {
        func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
        cVar1 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar1 + -1;
        if (cVar1 != '\0') {
          return;
        }
        *(undefined1 *)(_DAT_800ac784 + 1) = 7;
        FUN_80115d74(0);
        *_DAT_800ac784 = *_DAT_800ac784 | 2;
        *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
        return;
      }
      goto LAB_80114e3c;
    }
    if (puVar2 != (uint *)0x0) goto LAB_80114e3c;
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
  if ((int)_DAT_800ac784[0xe] < (int)uVar3) {
    return;
  }
  _DAT_800ac784[0xe] = uVar3;
  *(undefined2 *)(_DAT_800ac784 + 0x1b) = 0;
  FUN_80115d94(10);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0xb;
  func_0x800453d0(5);
  puVar2 = _DAT_800ac784;
  *(char *)((int)_DAT_800ac784 + 7) = *(char *)((int)_DAT_800ac784 + 7) + '\x01';
LAB_80114e3c:
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


