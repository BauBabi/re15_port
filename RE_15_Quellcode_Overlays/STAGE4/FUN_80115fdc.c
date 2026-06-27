/* FUN_80115fdc @ 0x80115fdc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115fdc(undefined4 param_1)

{
  char cVar1;
  int in_v0;
  
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a),_DAT_800acbdc + 0x5a0);
  cVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  DAT_800aca5a = DAT_800aca5a + cVar1;
  (*_DAT_800b0000)();
  return;
}


