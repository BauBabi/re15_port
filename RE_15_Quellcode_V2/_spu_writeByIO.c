void _spu_writeByIO(word *param_1,uint param_2)

{
  int iVar1;
  int local_28;
  
  SOUND_RAM_DATA_TRANSFER_ADDR = DAT_80076d58;
  local_28 = 0;
  do {
    local_28 = local_28 + 1;
  } while (local_28 < 0xf0);
  if (param_2 == 0) {
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf;
    DAT_80076d54 = 0;
    return;
  }
  if (0x40 < param_2) {
    param_2 = 0x40;
  }
  iVar1 = 0;
  if (0 < (int)param_2) {
    do {
      SOUND_RAM_DATA_TRANSFER_FIFO = *param_1;
      param_1 = param_1 + 1;
      iVar1 = iVar1 + 2;
    } while (iVar1 < (int)param_2);
  }
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf | 0x10;
  SPU_OBJ_6DC();
  return;
}
