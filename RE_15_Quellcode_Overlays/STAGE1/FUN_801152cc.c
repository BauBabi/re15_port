/* FUN_801152cc @ 0x801152cc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801152cc(void)

{
  int iVar1;
  char cVar2;
  
  DAT_800acae8 = 2;
  DAT_800acae9 = 0;
  cVar2 = DAT_800aca5a + '\x01';
  _DAT_800aca58 = CONCAT12(cVar2,_DAT_800aca58);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    _DAT_800aca58 = 1;
    DAT_800acae7 = DAT_800acae7 & 0xfe;
    _DAT_800aca3c = _DAT_800aca3c & 0xffffff3f;
  }
  return;
}


