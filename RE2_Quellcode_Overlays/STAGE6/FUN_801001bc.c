/* FUN_801001bc @ 0x801001bc  (Ghidra headless overlay RE) */

/* WARNING: Control flow encountered bad instruction data */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801001bc(void)

{
  int iVar1;
  
  if (((_DAT_800ce306 & 0x800) != 0) && (_DAT_8011bca2 == 0)) {
    _DAT_8011bc6c = 0x400;
    DAT_8011bc62 = 1;
  }
  _DAT_8011bcd2 = _DAT_8011bcd2 + _DAT_8011bc6c;
  func_0x8002c2b0(0,(int)_DAT_8011bcd2,0,0);
  iVar1 = func_0x8002c350(0);
  if (iVar1 == 0) {
    if (5 < _DAT_8011bcd4) {
      _DAT_8011bcf8 = 0;
      if (_DAT_8011bc98 != 0) {
        do {
          iVar1 = *(int *)((_DAT_8011bcf8 + DAT_8011bce4) * 4 + -0x7fee4c88);
          if (iVar1 != 0) {
            func_0x800313a4(0x3c,_DAT_8011bcf8 * 0xe + 0x20,iVar1,0x4002);
          }
          _DAT_8011bcf8 = _DAT_8011bcf8 + 1;
        } while (_DAT_8011bcf8 < _DAT_8011bc98);
      }
      func_0x8011a4e8();
      func_0x8011aec8();
      _DAT_8011bca8 = _DAT_8011bca8 + 1;
      if (_DAT_8011bca8 == -1) {
        _DAT_8011bca8 = -2;
      }
      func_0x80031f94(1);
                    /* WARNING: Bad instruction - Truncating control flow here */
      halt_baddata();
    }
                    /* WARNING: Could not emulate address calculation at 0x8010028c */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(_DAT_8011bcd4 * 4 + -0x7fee5fec))();
    return;
  }
  _DAT_8011bc6c = 0;
  _DAT_8011bcd2 = 0xffff;
                    /* WARNING: Bad instruction - Truncating control flow here */
  halt_baddata();
}


