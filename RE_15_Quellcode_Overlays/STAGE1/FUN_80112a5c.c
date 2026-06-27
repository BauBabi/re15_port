/* FUN_80112a5c @ 0x80112a5c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80112a5c(void)

{
  undefined1 uVar1;
  char cVar2;
  int extraout_v1;
  undefined4 uVar3;
  int unaff_s0;
  
  if (*(char *)(_DAT_800ac784 + 0x1d3) != '\0') {
    *(undefined1 *)(_DAT_800ac784 + 0x1d3) = 0;
    if ((*(byte *)(_DAT_800ac784 + 0x1d4) & 0xf) != 0) {
      FUN_801132d0(9);
      return;
    }
    if (((*(byte *)(_DAT_800ac784 + 0x1d4) & 0x80) != 0) || (*(char *)(_DAT_800ac784 + 5) == '\x06')
       ) {
      uVar1 = FUN_80115f00();
      *(undefined1 *)(extraout_v1 + 0x1d7) = uVar1;
      *(undefined2 *)(_DAT_800ac784 + 0x1e6) = 0xffec;
      func_0x800453d0();
      DAT_800aca58 = 2;
      _DAT_800acaee = _DAT_800acaee + -4;
      cVar2 = func_0x8001a780(unaff_s0 + -0x34);
      DAT_800aca59 = cVar2 + '\x02';
      _DAT_800aca5a = 0;
      if (_DAT_800acaee < 0) {
        DAT_800aca58 = 3;
        _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
        DAT_800aca59 = '\0';
        _DAT_800aca5a = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
      }
      FUN_801161e8();
      if (*(byte *)(_DAT_800ac784 + 0x1d7) != 0) {
        func_0x80019700((uint)*(byte *)(_DAT_800ac784 + 0x1d7) << 0xb,
                        (int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
        *(char *)(_DAT_800ac784 + 0x1d7) = *(char *)(_DAT_800ac784 + 0x1d7) + -1;
      }
      *(int *)(_DAT_800ac784 + 0x38) =
           (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
      func_0x800245d8(0);
      return;
    }
    FUN_80115d74(6);
    if (2 < *(byte *)(_DAT_800ac784 + 0x1d6)) {
      FUN_80115f00();
    }
  }
  if ((((*(char *)(_DAT_800ac784 + 0x1d2) != '\0') && (*(char *)(_DAT_800ac784 + 0x1db) != '\0')) &&
      (*(ushort *)(_DAT_800ac784 + 0x1dc) < 10000)) && (*(byte *)(_DAT_800ac784 + 0x1d6) < 3)) {
    uVar3 = 10;
    if (*(byte *)(_DAT_800ac784 + 9) < 0x80) {
      uVar3 = 0xc;
    }
    FUN_80115d74(uVar3);
  }
  return;
}


