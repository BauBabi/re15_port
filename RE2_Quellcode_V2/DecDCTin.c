/* DecDCTin @ 0x800949dc  (Ghidra headless, raw MIPS overlay) */

void DecDCTin(u_long *buf,int mode)

{
  if ((mode & 1U) != 0) {
    LIBPRESS_OBJ_19C();
    return;
  }
  *buf = *buf | 0x8000000;
  if ((mode & 2U) != 0) {
    LIBPRESS_OBJ_1C8();
    return;
  }
  *buf = *buf & 0xfdffffff;
  MDEC_in(buf,(short)*buf);
  return;
}


