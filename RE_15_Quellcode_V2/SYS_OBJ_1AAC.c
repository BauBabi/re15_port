undefined4 SYS_OBJ_1AAC(void)

{
  uint uVar1;
  undefined4 uVar2;
  ushort in_v1;
  ushort *in_t0;
  uint in_t1;
  
  in_t0[3] = in_v1;
  if (((*in_t0 & 0x3f) == 0) && ((in_t0[2] & 0x3f) == 0)) {
    DAT_8008fe28 = 0x4ffffff;
    DAT_8008fe2c = 0xe6000000;
    DAT_8008fe30 = in_t1 & 0xffffff | 0x2000000;
    DAT_8008fe34 = *(undefined4 *)in_t0;
    DAT_8008fe38 = *(undefined4 *)(in_t0 + 2);
    _cwc(&DAT_8008fe28);
    return 0;
  }
  DAT_8008fe28 = 0x708fe48;
  DAT_8008fe38 = 0xe6000000;
  DAT_8008fe3c = in_t1 & 0xffffff | 0x60000000;
  DAT_8008fe2c = 0xe3000000;
  DAT_8008fe30 = 0xe4ffffff;
  DAT_8008fe34 = 0xe5000000;
  DAT_8008fe40 = *(undefined4 *)in_t0;
  DAT_8008fe44 = *(undefined4 *)(in_t0 + 2);
  DAT_8008fe48 = 0x3ffffff;
  uVar1 = _param(3);
  DAT_8008fe4c = uVar1 | 0xe3000000;
  uVar1 = _param(4);
  DAT_8008fe50 = uVar1 | 0xe4000000;
  uVar1 = _param(5);
  DAT_8008fe54 = uVar1 | 0xe5000000;
  uVar2 = SYS_OBJ_1BF8();
  return uVar2;
}
