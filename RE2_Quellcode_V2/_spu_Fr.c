/* _spu_Fr @ 0x80078f44  (Ghidra headless, raw MIPS overlay) */

undefined4 _spu_Fr(undefined4 param_1,undefined4 param_2)

{
  _spu_t(2,(uint)DAT_800ab234 << (DAT_800ab244 & 0x1f));
  _spu_t(0);
  _spu_t(3,param_1,param_2);
  return param_2;
}


