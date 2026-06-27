undefined4 _spu_write(undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  
  if (DAT_80076d74 == 0) {
    _spu_t(2,(uint)DAT_80076d58 << (DAT_80076d80 & 0x1f));
    _spu_t(1);
    _spu_t(3,param_1,param_2);
    uVar1 = SPU_OBJ_F74();
    return uVar1;
  }
  _spu_writeByIO(param_1,param_2);
  return param_2;
}
