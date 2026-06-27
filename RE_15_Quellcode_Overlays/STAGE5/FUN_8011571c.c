/* FUN_8011571c @ 0x8011571c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011571c(void)

{
  char cVar1;
  int in_v0;
  
  if (in_v0 == 0) {
    _DAT_800acabe = _DAT_800acabe + 0x800;
  }
  func_0x80045024(0x4000001,_DAT_800ac784 + 0x34);
  _DAT_800aca3c = _DAT_800aca3c | 0xc0;
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  FUN_8011615c();
  return;
}


