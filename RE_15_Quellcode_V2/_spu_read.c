undefined4 _spu_read(undefined4 param_1,undefined4 param_2)

{
  _spu_t(2,(uint)DAT_80076d58 << (DAT_80076d80 & 0x1f));
  _spu_t(0);
  _spu_t(3,param_1,param_2);
  return param_2;
}
