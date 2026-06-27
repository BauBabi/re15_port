/* FUN_801132d0 @ 0x801132d0  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801132d0(void)

{
  char cVar1;
  int in_v1;
  int unaff_s0;
  
  *(undefined1 *)(in_v1 + 0x1d7) = 6;
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


