/* FUN_80110f44 @ 0x80110f44  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80110f44(void)

{
  byte bVar1;
  int iVar2;
  
  bVar1 = DAT_800aca5a < 2;
  if (DAT_800aca5a != 1) {
    if (!(bool)bVar1) {
      bVar1 = 0;
      iVar2 = 0;
      if (DAT_800aca5a == 2) {
        func_0x80037edc(0,10);
        func_0x80037edc(5,0x32);
        func_0x80037edc(7,0x32);
        _DAT_800aca58 = 7;
        return;
      }
      goto code_r0x801118bc;
    }
    iVar2 = 0x2000;
    if (DAT_800aca5a != 0) goto code_r0x801118bc;
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a07c);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x8011a07c);
  }
  iVar2 = _DAT_800acbcc;
  bVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  _DAT_800aca58 = CONCAT12(DAT_800aca5a + bVar1,_DAT_800aca58);
code_r0x801118bc:
  if (((bVar1 & 0x19) == 0) &&
     ((((*(byte *)(iVar2 + 0x90) & 0xf0) * 0x10 - (int)*(short *)(iVar2 + 0x6a)) + 0x200 & 0xfff) <
      0x400)) {
    *(byte *)(iVar2 + 0x9f) = *(byte *)(iVar2 + 0x90);
    *(undefined1 *)(_DAT_800ac784 + 5) = 0xb;
    *(undefined2 *)(_DAT_800ac784 + 6) = 0;
  }
  return;
}


