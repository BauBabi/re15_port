undefined4 _clr(ushort *param_1,uint param_2)

{
  ushort uVar1;
  undefined4 uVar2;
  uint uVar3;
  ushort uVar4;
  
  uVar1 = param_1[2];
  if ((short)uVar1 < 0) {
    uVar1 = 0;
  }
  else if (DAT_8007e354 + -1 < (int)(short)uVar1) {
    uVar2 = SYS_OBJ_1A68();
    return uVar2;
  }
  uVar4 = param_1[3];
  param_1[2] = uVar1;
  if ((short)uVar4 < 0) {
    uVar4 = 0;
  }
  else if (DAT_8007e356 + -1 < (int)(short)uVar4) {
    uVar2 = SYS_OBJ_1AAC();
    return uVar2;
  }
  param_1[3] = uVar4;
  if (((*param_1 & 0x3f) == 0) && ((param_1[2] & 0x3f) == 0)) {
    DAT_8008fe28 = 0x4ffffff;
    DAT_8008fe2c = 0xe6000000;
    DAT_8008fe30 = param_2 & 0xffffff | 0x2000000;
    DAT_8008fe34 = *(undefined4 *)param_1;
    DAT_8008fe38 = *(undefined4 *)(param_1 + 2);
    _cwc(&DAT_8008fe28);
    return 0;
  }
  DAT_8008fe28 = 0x708fe48;
  DAT_8008fe38 = 0xe6000000;
  DAT_8008fe3c = param_2 & 0xffffff | 0x60000000;
  DAT_8008fe2c = 0xe3000000;
  DAT_8008fe30 = 0xe4ffffff;
  DAT_8008fe34 = 0xe5000000;
  DAT_8008fe40 = *(undefined4 *)param_1;
  DAT_8008fe44 = *(undefined4 *)(param_1 + 2);
  DAT_8008fe48 = 0x3ffffff;
  uVar3 = _param(3);
  DAT_8008fe4c = uVar3 | 0xe3000000;
  uVar3 = _param(4);
  DAT_8008fe50 = uVar3 | 0xe4000000;
  uVar3 = _param(5);
  DAT_8008fe54 = uVar3 | 0xe5000000;
  uVar2 = SYS_OBJ_1BF8();
  return uVar2;
}
