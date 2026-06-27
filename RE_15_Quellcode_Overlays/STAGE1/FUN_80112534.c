/* FUN_80112534 @ 0x80112534  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112534(void)

{
  char cVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  
  uVar5 = (uint)*(byte *)(_DAT_800ac784 + 6);
  if (uVar5 == 0) {
    *(byte *)(_DAT_800ac784 + 0x1d5) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x32;
    FUN_80115d94(6);
    func_0x800453d0(0);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (uVar5 != 1) {
    if (*(int *)(_DAT_800ac784 + 0x38) <= (int)(uVar5 - 0xe10)) {
      if (*(int *)(_DAT_800ac784 + 0x38) < (int)(uVar5 - 0x1518)) {
        *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
        iVar4 = _DAT_800ac784;
        iVar3 = (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
        *(int *)(_DAT_800ac784 + 0x38) = iVar3;
        *(char *)(iVar4 + 0x1d6) = (char)iVar3 + '\x01';
        *(undefined2 *)(_DAT_800ac784 + 0x9c) = 100;
        FUN_801161e8();
        if (_DAT_800acaee < 0) {
          DAT_800aca58 = 3;
          _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
          DAT_800aca59 = 0;
          _DAT_800aca5a = 0;
          *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
        }
      }
      else {
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
        *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
        func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c)
                        ,0,0x200);
      }
      func_0x80019700();
      func_0x800453d0(2);
      *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                              *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
      if (iVar4 != 0) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
      }
      cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
      *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
      if (cVar1 == '\0') {
        func_0x800453d0(2);
        *(undefined1 *)(_DAT_800ac784 + 0x1d5) = 0x1e;
      }
      if ((uint)*(byte *)(_DAT_800ac784 + 0x1d5) % 10 == 0) {
        func_0x800453d0(0);
      }
      sVar2 = func_0x80037024();
      *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -1 + sVar2 * -3;
      if (*(short *)(_DAT_800ac784 + 0x9c) < 0) {
        _DAT_800aca5a = CONCAT11(DAT_800aca5b,DAT_800aca5a + '\x01');
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x4000;
        FUN_80115d74(0xe);
      }
      return;
    }
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    FUN_80113694();
    return;
  }
  cVar1 = *(char *)(_DAT_800ac784 + 0x1d5);
  *(char *)(_DAT_800ac784 + 0x1d5) = cVar1 + -1;
  if (cVar1 == '\0') {
    FUN_80115d74(9);
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  cVar1 = FUN_80115dc8();
  *(ushort *)(_DAT_800ac784 + 0x1e4) = (*(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f) * (short)cVar1;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


