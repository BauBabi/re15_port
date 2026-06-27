/* FUN_801160a4 @ 0x801160a4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801160a4(void)

{
  char cVar1;
  
  if (DAT_800aca5a != 1) {
    if (1 < DAT_800aca5a) {
      if (DAT_800aca5a != 2) {
        return;
      }
      func_0x80037edc(0,10);
      func_0x80037edc(5,0x32);
      func_0x80037edc(7,0x32);
      _DAT_800aca58 = 7;
      return;
    }
    if (DAT_800aca5a != 0) {
      return;
    }
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011ee4c
                   );
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,&DAT_8011ee4c
                   );
  }
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + cVar1,_DAT_800aca58);
  return;
}


