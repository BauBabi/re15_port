/* SYS_OBJ_27A8 @ 0x80092688  (Ghidra headless, raw MIPS overlay) */

uint SYS_OBJ_27A8(void)

{
  undefined4 unaff_s2;
  undefined4 unaff_s3;
  
  *(undefined4 *)(&DAT_800e89a0 + DAT_800b2804 * 0x60) = unaff_s2;
  *(undefined4 *)(&DAT_800e8998 + DAT_800b2804 * 0x60) = unaff_s3;
  DAT_800b2804 = DAT_800b2804 + 1 & 0x3f;
  SetIntrMask(DAT_800b280c);
  _exeque();
  return DAT_800b2804 - DAT_800b2808 & 0x3f;
}


