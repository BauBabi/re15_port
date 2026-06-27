/* _spu_Fw @ 0x80078ebc  (Ghidra headless, raw MIPS overlay) */

undefined4 _spu_Fw(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (DAT_800ab238 == 0) {
    _spu_t(2,(uint)DAT_800ab234 << (DAT_800ab244 & 0x1f));
    _spu_t(1);
    _spu_t(3,param_1,param_2);
    uVar1 = SPU_OBJ_8CC();
    return uVar1;
  }
  _spu_FwriteByIO(param_1,param_2);
  return param_2;
}


