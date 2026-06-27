void SPU_OBJ_3F8(void)

{
  int in_stack_00000010;
  undefined4 uStack00000014;
  
  for (; in_stack_00000010 < 0xf0; in_stack_00000010 = in_stack_00000010 + 1) {
  }
  SPU_VOICE_KEY_OFF._0_2_ = 0xffff;
  SPU_VOICE_KEY_OFF._2_2_ = SPU_VOICE_KEY_OFF._2_2_ | 0xff;
  uStack00000014 = 0xd;
  SPU_OBJ_470();
  return;
}
