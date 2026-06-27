void SPU_OBJ_6DC(void)

{
  int in_stack_00000010;
  
  for (; in_stack_00000010 < 0xf0; in_stack_00000010 = in_stack_00000010 + 1) {
  }
  DAT_80076d54 = 0;
  do {
    if ((SPU_STATUS_REG_SPUSTAT & 0x400) == 0) {
      SPU_OBJ_7B0();
      return;
    }
    DAT_80076d54 = DAT_80076d54 + 1;
  } while (DAT_80076d54 < 0x1389);
  printf("SPU:T/O [%s]\n","wait (wrdy H -> L)");
  SPU_OBJ_778();
  return;
}
