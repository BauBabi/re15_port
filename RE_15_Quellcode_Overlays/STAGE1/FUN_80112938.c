/* FUN_80112938 @ 0x80112938  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112938(void)

{
  char cVar1;
  int iVar2;
  int unaff_s0;
  
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(7);
    *(undefined2 *)(_DAT_800ac784 + 0x1e4) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) + *(short *)(_DAT_800ac784 + 0x1e6);
    *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + -7;
    if (((*(ushort *)(_DAT_800ac784 + 0x1dc) < 600) &&
        (*(ushort *)(_DAT_800ac784 + 0x1ec) - 1 < 0xe0f)) && (DAT_800acae7 == '\0')) {
      *(undefined1 *)(_DAT_800ac784 + 0x1d7) = 6;
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffec;
      func_0x800453d0(4);
      DAT_800aca58 = 2;
      _DAT_800acaee = _DAT_800acaee + -4;
      cVar1 = func_0x8001a780(unaff_s0 + -0x34);
      DAT_800aca59 = cVar1 + '\x02';
      _DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
        DAT_800aca59 = '\0';
        _DAT_800aca5a = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      }
      FUN_801161e8();
    }
    if (*(byte *)(_DAT_800ac784 + 0x1d7) != 0) {
      func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d7) << 0xb,
                      (int)*(short *)(_DAT_800ac784 + 0x6a),*(int *)(_DAT_800ac784 + 0x188) + 0x198,
                      &DAT_8012110c);
      *(char *)(_DAT_800ac784 + 0x1d7) = *(char *)(_DAT_800ac784 + 0x1d7) + -1;
    }
    *(int *)(_DAT_800ac784 + 0x38) =
         (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
    func_0x800245d8(0);
    return;
  }
  *(short *)(_DAT_800ac784 + 0x1e4) = *(short *)(_DAT_800ac784 + 0x1e4) + 10;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
  iVar2 = *(short *)(_DAT_800ac784 + 0x1ba) + -400;
  if (iVar2 <= *(int *)(_DAT_800ac784 + 0x38)) {
    *(int *)(_DAT_800ac784 + 0x38) = iVar2;
  }
  iVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  if (iVar2 != 0) {
    FUN_80115d74(0);
    *(undefined1 *)(_DAT_800ac784 + 0x1da) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1d6) = 0;
  }
  return;
}


