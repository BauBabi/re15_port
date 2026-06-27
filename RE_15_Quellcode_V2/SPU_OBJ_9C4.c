void SPU_OBJ_9C4(void)

{
  int in_stack_00000010;
  
  for (; in_stack_00000010 < 0xf0; in_stack_00000010 = in_stack_00000010 + 1) {
  }
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf;
  if (DAT_80076d90 == (code *)0x0) {
    DeliverEvent(0xf0000009,0x20);
    return;
  }
  (*DAT_80076d90)();
  SPU_OBJ_A6C();
  return;
}
