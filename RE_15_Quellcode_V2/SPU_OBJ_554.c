undefined4 SPU_OBJ_554(void)

{
  int in_stack_00000010;
  
  for (; in_stack_00000010 < 0xf0; in_stack_00000010 = in_stack_00000010 + 1) {
  }
  DAT_80076d8c = 1;
  SPU_CTRL_REG_CPUCNT = 0xc000;
  DAT_80076d90 = 0;
  DAT_80076d94 = 0;
  return 0;
}
