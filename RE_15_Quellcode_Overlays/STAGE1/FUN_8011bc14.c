/* FUN_8011bc14 @ 0x8011bc14  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bc14(void)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = _DAT_800aca58 >> 0x10 & 0xff;
  if (uVar2 != 1) {
    if (1 < uVar2) {
      if (uVar2 == 2) {
        func_0x80037edc(0,10);
        func_0x80037edc(5,0x32);
        func_0x80037edc(7,0x32);
        _DAT_800aca58 = 7;
        return;
      }
      goto LAB_8011c588;
    }
    if (uVar2 != 0) goto LAB_8011c588;
    _DAT_800aca58 = CONCAT12(1,_DAT_800aca58);
    DAT_800acae3 = 7;
    DAT_800acae8 = 0;
    DAT_800acae9 = '\0';
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x80121570);
    func_0x80045024(0x4030001,_DAT_800ac784 + 0x34);
    _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  }
  if (DAT_800acae9 == '<') {
    func_0x80045630(2,0,0);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),_DAT_800acbdc + 0x5a0,0x80121570);
  }
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  uVar2 = (_DAT_800aca58 >> 0x10 & 0xff) + iVar1;
  _DAT_800aca58 = CONCAT12((char)uVar2,_DAT_800aca58);
LAB_8011c588:
  *(undefined1 *)(*(int *)(uVar2 - 0x387c) + 0x94) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x8001af5c(0,0,600,700,_DAT_800ac784 + 0xb0,0x808080);
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}


