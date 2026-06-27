void S_SCA_OBJ_164(int param_1,int param_2)

{
  ushort uVar1;
  ushort in_t0;
  uint in_t1;
  int in_t2;
  
  uVar1 = in_t0 & 0x7fff;
  if (param_2 != 0) {
    uVar1 = *(ushort *)(param_1 + 6);
    if (0x7f < (short)uVar1) {
      S_SCA_OBJ_194();
      return;
    }
    if ((short)uVar1 < 0) {
      uVar1 = 0;
    }
    uVar1 = uVar1 & 0x7fff;
  }
  SPU_MAIN_VOL_R = uVar1 | (ushort)param_2;
  if ((in_t2 != 0) || ((in_t1 & 0x40) != 0)) {
    CD_VOL_L = *(word *)(param_1 + 0x10);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x80) != 0)) {
    CD_VOL_R = *(word *)(param_1 + 0x12);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x100) != 0)) {
    if (*(int *)(param_1 + 0x14) == 0) {
      S_SCA_OBJ_240();
      return;
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 4;
  }
  if ((in_t2 != 0) || ((in_t1 & 0x200) != 0)) {
    if (*(int *)(param_1 + 0x18) == 0) {
      S_SCA_OBJ_294();
      return;
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 1;
  }
  if ((in_t2 != 0) || ((in_t1 & 0x400) != 0)) {
    EXT_VOL_L = *(word *)(param_1 + 0x1c);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x800) != 0)) {
    EXT_VOL_R = *(word *)(param_1 + 0x1e);
  }
  if ((in_t2 != 0) || ((in_t1 & 0x1000) != 0)) {
    if (*(int *)(param_1 + 0x20) == 0) {
      S_SCA_OBJ_330();
      return;
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 8;
  }
  if ((in_t2 != 0) || ((in_t1 & 0x2000) != 0)) {
    if (*(int *)(param_1 + 0x24) == 0) {
      S_SCA_OBJ_384();
      return;
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 2;
  }
  return;
}
