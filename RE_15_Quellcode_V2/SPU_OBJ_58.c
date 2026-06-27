undefined4 SPU_OBJ_58(ushort param_1)

{
  undefined4 local_res0;
  
  for (; local_res0 < 0xf0; local_res0 = local_res0 + 1) {
  }
  SPU_CTRL_REG_CPUCNT = param_1 & 0xffcf;
  return 0;
}
