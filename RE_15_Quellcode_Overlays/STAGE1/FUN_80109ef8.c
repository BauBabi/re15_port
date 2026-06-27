/* FUN_80109ef8 @ 0x80109ef8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80109ef8(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        FUN_8010a8b8();
        return;
      }
      DAT_800aca58 = 7;
      DAT_800aca59 = 0;
      return;
    }
    if (DAT_800aca5a != 0) {
      FUN_8010a8b8();
      return;
    }
    DAT_800aca5a = 1;
    DAT_800acae9 = '\0';
    DAT_800acae8 = DAT_800acaf3 + '\x06';
    DAT_800acae3 = 0;
    func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801201c8
                   );
    func_0x80045024(0x4010001,_DAT_800ac784 + 0x34);
  }
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  if (DAT_800acae9 == '#') {
    func_0x80045024(0x2070001,_DAT_800ac784 + 0x34);
    _DAT_800acaee = 0xffff;
  }
  if (DAT_800acae9 == '7') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_801201c8
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8010a8b8();
  return;
}


