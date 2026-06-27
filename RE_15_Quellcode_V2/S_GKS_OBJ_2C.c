int S_GKS_OBJ_2C(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  if (param_2 == 0xffffffff) {
    iVar1 = S_GKS_OBJ_8C();
    return iVar1;
  }
  if ((1 << (param_2 & 0x1f) & DAT_80076e1c) == 0) {
    iVar1 = (uint)((&VOICE_00_ADSR_CURR_VOLUME)[param_2 * 8] != 0) << 1;
  }
  else {
    iVar1 = 3;
    if ((&VOICE_00_ADSR_CURR_VOLUME)[param_2 * 8] != 0) {
      iVar1 = S_GKS_OBJ_8C();
      return iVar1;
    }
  }
  return iVar1;
}
