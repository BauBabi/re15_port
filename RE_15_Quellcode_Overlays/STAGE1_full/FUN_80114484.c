/* FUN_80114484 @ 0x80114484  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114484(void)

{
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    FUN_80115d94(8);
    FUN_801161e8();
    func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&DAT_8012110c);
    _DAT_800acaee = _DAT_800acaee - 4;
    if ((int)((uint)_DAT_800acaee << 0x10) < 0) {
      DAT_800aca58 = 3;
      _DAT_800aca50 = _DAT_800aca50 & 0xfff | 0x2000;
      DAT_800aca59 = 0;
      _DAT_800aca5a = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x1d8) = 1;
    }
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  else if (*(char *)(_DAT_800ac784 + 6) != '\x01') {
    return;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


