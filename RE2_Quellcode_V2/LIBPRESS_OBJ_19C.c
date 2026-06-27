/* LIBPRESS_OBJ_19C @ 0x80094a0c  (Ghidra headless, raw MIPS overlay) */

void LIBPRESS_OBJ_19C(uint *param_1,uint param_2)

{
  uint in_v0;
  
  *param_1 = in_v0;
  if ((param_2 & 2) != 0) {
    LIBPRESS_OBJ_1C8();
    return;
  }
  *param_1 = *param_1 & 0xfdffffff;
  MDEC_in(param_1,(short)*param_1);
  return;
}


