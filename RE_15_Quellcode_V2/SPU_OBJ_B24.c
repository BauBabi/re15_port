void SPU_OBJ_B24(void)

{
  undefined4 local_res0;
  
  for (; local_res0 < 0xf0; local_res0 = local_res0 + 1) {
  }
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 0x30;
  SPU_OBJ_B90();
  return;
}
