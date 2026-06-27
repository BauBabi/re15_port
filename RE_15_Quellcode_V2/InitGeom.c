void InitGeom(void)

{
  undefined4 unaff_retaddr;
  
  DAT_8007d664 = unaff_retaddr;
  _patch_gte();
  setCopReg(0,Status,Status | 0x40000000,0);
  gte_ldZSF3(0x155);
  gte_ldZSF4(0x100);
  gte_ldH(1000);
  gte_ldDQA(0xffffef9e);
  gte_ldDQB(0x1400000);
  gte_ldOFX(0);
  gte_ldOFY(0);
  return;
}
