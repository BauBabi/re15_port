/* FUN_801137fc @ 0x801137fc  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801137fc(void)

{
  byte bVar1;
  char cVar2;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          FUN_80115d74(5);
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
        }
        *(short *)((int)_DAT_800ac784 + 0x1de) = *(short *)((int)_DAT_800ac784 + 0x1de) + 2;
        func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
        *(short *)(_DAT_800ac784 + 0x23) =
             (short)_DAT_800ac784[0x23] + *(short *)((int)_DAT_800ac784 + 0x1e6);
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
        if ((((ushort)_DAT_800ac784[0x77] < 600) && ((ushort)_DAT_800ac784[0x7b] - 1 < 0xe0f)) &&
           (DAT_800acae7 == '\0')) {
          *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 6;
          *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0xffec;
          func_0x800453d0(4);
          DAT_800aca58 = 2;
          _DAT_800acaee = _DAT_800acaee + -4;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          _DAT_800aca5a = 0;
          if (_DAT_800acaee < 0) {
            DAT_800aca58 = 3;
            _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
            DAT_800aca59 = '\0';
            _DAT_800aca5a = 0;
            *(undefined1 *)(_DAT_800ac784 + 0x76) = 1;
          }
          FUN_801161e8();
        }
        if (*(byte *)((int)_DAT_800ac784 + 0x1d7) != 0) {
          func_0x80019700((uint)*(byte *)((int)_DAT_800ac784 + 0x1d7) << 0xb,
                          (int)*(short *)((int)_DAT_800ac784 + 0x6a),_DAT_800ac784[0x62] + 0x198,
                          &DAT_8012110c);
          *(char *)((int)_DAT_800ac784 + 0x1d7) = *(char *)((int)_DAT_800ac784 + 0x1d7) + -1;
        }
      }
      goto code_r0x80113be0;
    }
    if (bVar1 != 0) goto code_r0x80113be0;
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x50;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 200;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 9;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,*(undefined2 *)((int)_DAT_800ac784 + 0x1de));
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
code_r0x80113be0:
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  return;
}


