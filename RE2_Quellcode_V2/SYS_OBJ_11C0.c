/* SYS_OBJ_11C0 @ 0x800910a0  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_11C0(void)

{
  bool bVar1;
  uchar *unaff_s0;
  uint unaff_s3;
  
  bVar1 = *(short *)(unaff_s0 + 6) < 0x121;
  if (unaff_s0[0x12] == '\0') {
    bVar1 = *(short *)(unaff_s0 + 6) < 0x101;
  }
  if (!bVar1) {
    unaff_s3 = unaff_s3 | 0x24;
  }
  (**(code **)(PTR_PTR_800b26f8 + 0x10))(unaff_s3);
  memcpy("",unaff_s0,0x14);
  return;
}


