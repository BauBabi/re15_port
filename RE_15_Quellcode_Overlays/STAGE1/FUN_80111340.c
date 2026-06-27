/* FUN_80111340 @ 0x80111340  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111340(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int in_v0;
  int iVar1;
  
  DAT_800acae9 = 0;
  DAT_800acaea = 0;
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a),param_3 + 0x5a0,param_4 + 0x10e0);
  func_0x80037edc(0,10);
  func_0x8001ad68(&DAT_800aca54,_DAT_800acbcc,_DAT_800acbd0);
  iVar1 = func_0x8001f314(_DAT_800acbcc,_DAT_800acbd0,0,0x200);
  if (iVar1 != 0) {
    DAT_800aca5a = 6;
    _DAT_800acabe = _DAT_800acabe + 0x800;
    _DAT_800aca54 = _DAT_800aca54 & 0xffffefff;
    FUN_80111c9c();
    return;
  }
  return;
}


